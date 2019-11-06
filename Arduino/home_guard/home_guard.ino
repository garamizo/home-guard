#include "DHTesp.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <AWS_IOT.h>

// include your personal WiFi and AWS configuration.
#include "config.h"

AWS_IOT hornbill;
DHTesp dht;
TaskHandle_t tempTaskHandle = NULL;
int dhtPin = 32;

int status = WL_IDLE_STATUS;
int tick = 0, msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];

/*
  void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
  {
  strncpy(rcvdPayload, payLoad, payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
  }
*/

void setup() {
  Serial.begin(9600);

  WiFiManager wifiManager;
  wifiManager.autoConnect("HomeGuard");

  if (hornbill.connect(HOST_ADDRESS, CLIENT_ID,
                       aws_root_ca_pem, certificate_pem_crt, private_pem_key) == 0)
  {
    Serial.println("Connected to AWS");
    delay(1000);

    //    if (!hornbill.subscribe(TOPIC_NAME, mySubCallBackHandler))
    //      Serial.println("Subscribe Successfull");
    //    else {
    //      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
    //      while (1);
    //    }
  }
  else {
    Serial.println("AWS connection failed, Check the HOST Address");
    while (1);
  }

  // Initialize temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);
  delay(1000);
}

void loop() {

  if (msgReceived == 1) {
    msgReceived = 0;
    Serial.print("Received Message:");
    Serial.println(rcvdPayload);
  }

  if (tick >= 60)  // publish to topic every 5seconds
  {
    const size_t bufferSize = JSON_OBJECT_SIZE(2) + 20;
    DynamicJsonBuffer jsonBuffer(bufferSize);

    TempAndHumidity lastValues = dht.getTempAndHumidity();

    int tempF = round(lastValues.temperature * 9 / 5.0 + 32);
    int humidity = round(lastValues.humidity);
    //    Serial.println("Temperature: " + String(tempF, 0));
    //    Serial.println("Humidity: " + String(lastValues.humidity, 0));

    tick = 0;
    sprintf(payload, "Temperature:%d\nHumidity:%d", tempF, humidity);
    if (hornbill.publish(TOPIC_NAME, payload) == 0)
    {
      Serial.print("Publish Message:");
      Serial.println(payload);
    }
    else
    {
      Serial.println("Publish failed");
    }
  }
  vTaskDelay(1000 / portTICK_RATE_MS);
  tick++;
}
