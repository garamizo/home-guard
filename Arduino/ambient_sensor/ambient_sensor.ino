#include <WiFi.h>
#include "InfluxArduino.hpp"
#include "DHTesp.h"
#include "credentials.h"  // load WIFI_NAME and WIFI_PASS

InfluxArduino influx;
//connection/ database stuff that needs configuring
const char INFLUX_DATABASE[] = "weather";
const char INFLUX_IP[] = "192.168.10.25";
const char INFLUX_USER[] = ""; //username if authorization is enabled.
const char INFLUX_PASS[] = ""; //password for if authorization is enabled.
const char INFLUX_MEASUREMENT[] = "ambient"; //measurement name for the database. (in practice, you can use several, this example just uses the one)

unsigned long DELAY_TIME_US = 60 * 1000 * 1000; //how frequently to send data, in microseconds

DHTesp dht;
int dhtPin = 32;

void setup()
{
  Serial.begin(115200);

  // Initialize temperature sensor
  Serial.println("Configuring sensor...");
  pinMode(dhtPin, INPUT);
  dht.setup(dhtPin, DHTesp::DHT22);
  delay(1000);

  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected!");
  influx.configure(INFLUX_DATABASE, INFLUX_IP); //third argument (port number) defaults to 8086
  // influx.authorize(INFLUX_USER,INFLUX_PASS); //if you have set the Influxdb .conf variable auth-enabled to true, uncomment this
  // influx.addCertificate(ROOT_CERT); //uncomment if you have generated a CA cert and copied it into InfluxCert.hpp
  Serial.print("Using HTTPS: ");
  Serial.println(influx.isSecure()); //will be true if you've added the InfluxCert.hpp file.
}

void loop()
{
  unsigned long startTime = micros(); //used for timing when to send data next.

  //update our field variables
  TempAndHumidity lastValues = dht.getTempAndHumidity();
  float tempF = lastValues.temperature * 9 / 5.0 + 32;
  float humP = lastValues.humidity;

  //write our variables.
  char tags[64];
  char fields[64];

  sprintf(tags, "location=bedroom"); //write a tag called new_tag
  sprintf(fields, "temperature=%0.3f,humidity=%0.3f", tempF, humP);
  bool writeSuccessful = influx.write(INFLUX_MEASUREMENT, tags, fields);
  if (!writeSuccessful)
  {
    Serial.print("error: ");
    Serial.println(influx.getResponse());
  }

  while ((micros() - startTime) < DELAY_TIME_US)
  {
    //wait until it's time for next reading. Consider using a low power mode if this will be a while.
  }
}
