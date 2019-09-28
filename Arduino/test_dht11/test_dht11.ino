#include "DHTesp.h"

DHTesp dht;
TaskHandle_t tempTaskHandle = NULL;
int dhtPin = 32;

void setup() {
  Serial.begin(9600);
  
  // Initialize temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);
  delay(1000);  
}

void loop() {
  // put your main code here, to run repeatedly:
  TempAndHumidity lastValues = dht.getTempAndHumidity();

  float tempF = lastValues.temperature * 9/5.0 + 32;
  Serial.println("Temperature: " + String(tempF,0));
  Serial.println("Humidity: " + String(lastValues.humidity,0));

  delay(2000);
}
