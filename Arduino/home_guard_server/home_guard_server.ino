/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include "DHTesp.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#define LED_BUILTIN 22   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

// Set these to your desired credentials.
const char *ssid = "yourAP";
const char *password = "yourPassword";

DHTesp dht;
//TaskHandle_t tempTaskHandle = NULL;
int dhtPin = 32;

WiFiServer server(80);

#define DT 60000 // [ms]

struct Point {
  float temperature,
        humidity;
  unsigned long timestamp;
};

unsigned long countMinute = 0, countHour = 0, countDay = 0;
Point ptMinute[60], ptHour[24], ptDay[100];

Point sample_sensor() {
  Point pt;
//  pt.temperature = 1e-3 * random(0, 1000) * 90.0 + 20.0;
//  pt.humidity = 1e-3 * random(0, 1000) * 100.0;
//  pt.timestamp = countMinute;

  TempAndHumidity lastValues = dht.getTempAndHumidity();
  pt.temperature = lastValues.temperature * 9 / 5.0 + 32;
  pt.humidity = lastValues.humidity;
  pt.timestamp = countMinute;
  
  return pt;
}

Point circular_average_points(Point pt[], size_t count, size_t npts, size_t bufflen) {
  // Average previous points 
  Point avg({0, 0, 0});
  for (int k = 0; k < npts; k++) {
    int index = (count-k) % bufflen;
    avg.temperature += pt[index].temperature / npts;
    avg.humidity += pt[index].humidity / npts;
    avg.timestamp += pt[index].timestamp;
  }
  avg.timestamp /= npts;
  return(avg);
}



// retention policy:
//    Sample every minute
//    Keep last 60 minutes (min), 24 hours (h), 
//    Samples older than 1 hour is averaged into 1 hour
//    Samples older than 1 day is averaged into 1 day

// 60 + 24 + d == 1000
// Lasts for 338 days

void setup() {
  
  //pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  
  // Initialize temperature sensor
  Serial.println("Configuring sensor...");
  pinMode(dhtPin, INPUT);
  dht.setup(dhtPin, DHTesp::DHT11);
  delay(1000);
  
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

  // Sample first point
  ptMinute[countMinute++] = sample_sensor();
}

void loop() {
 
  static unsigned long tlast = millis();
  if (millis() - tlast > DT) {  // should avoid millis rollover
    tlast += DT;

    // sample sensor
    ptMinute[countMinute % 60] = sample_sensor();
    countMinute++;

    if (countMinute % 60 == 0) {
      ptHour[countHour % 24] = circular_average_points(ptMinute, countMinute-1, 60, 60);
      countHour++;
      
      if (countHour % 24 == 0) {
        ptDay[countDay % 100] = circular_average_points(ptHour, countHour-1, 24, 24);
        countDay++;
      }
    }
  }

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<h2>Weather Logger</h2>\n");
            client.print("<table style=\"width:100%\">\n");
            client.print("<tr>\n<th align=\"left\">Timestamp</th>\n<th align=\"left\">Temperature [F]</th>\n<th align=\"left\">Humidity [%]</th>\n</tr>\n");

            unsigned long now = countMinute;
            char str[160];
            for (int k = 0; k < 60 && k < countMinute; k+=5) {  // show previous 10 min
              Point pt = ptMinute[(countMinute-1-k)%60];
              sprintf(str, "<tr>\n<td>%u %s</td>\n<td>%.1f</td>\n<td>%.1f</td>\n</tr>\n", now-pt.timestamp, "min ago", pt.temperature, pt.humidity);
              client.print(str);
            }
            
            for (int k = 0; k < 24 && k < countHour; k++) {  // show previous 10 min
              Point pt = ptHour[(countHour-1-k)%24];
              sprintf(str, "<tr>\n<td>%u %s</td>\n<td>%.1f</td>\n<td>%.1f</td>\n</tr>\n", (now-pt.timestamp)/60, "hour ago", pt.temperature, pt.humidity);
              client.print(str);
            }

            for (int k = 0; k < countDay; k++) {  // show previous 10 min
              Point pt = ptDay[(countDay-1-k)%100];
              sprintf(str, "<tr>\n<td>%u %s</td>\n<td>%.1f</td>\n<td>%.1f</td>\n</tr>\n", (now-pt.timestamp)/(60*24), "day ago", pt.temperature, pt.humidity);
              client.print(str);
            }
            client.print("</table>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
  delay(1);
}
