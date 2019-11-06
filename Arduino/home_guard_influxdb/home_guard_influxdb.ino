
#include <WiFi.h>
#include <InfluxDb.h>

#define INFLUXDB_HOST "10.42.0.1"
#define WIFI_SSID "geyestest"
#define WIFI_PASS "senha2019"

Influxdb influx(INFLUXDB_HOST);

void setup() {
  Serial.begin(9600);
  Serial.println(" ### Hello ###");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Uncomment the following lines to use the v2.0 InfluxDB
 influx.setVersion(2);
 influx.setOrg("org1");
 influx.setBucket("bucket1");
 influx.setToken("2C0oXyG-dlCxmr2Hyy7ksBRxqvhBXBbFQ_SBsZ7ustM5xGDXNzRvDuZtGPiisA2rsSzw2VcNDAU16fNf42Ah4Q==");
 influx.setPort(9999);
 
  Serial.println("Setup done");
}


int loopCount = 0;

void loop() {
  loopCount++;

  InfluxData row("temperature");
  row.addTag("device", "lab");
  row.addValue("value", random(10, 40));

  Serial.println("Loop!");
  influx.write(row);
  

  delay(5000);
}
