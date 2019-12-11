# home-guard
Logs ambient data from your home.

A connected sensor measures room temperature and humidity and store it on a database server on the same local network.
The measurements are available in a dashboard for monitoring and can be exported for post-processing.

## Hardware Components

- Ambient sensor: [DHT-22](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf)
- Sensor MCU: [ESP32 Lollin32 Lite](https://wiki.wemos.cc/products:lolin32:lolin32_lite)
- Server host: Raspberry Pi 2

## Software

- Database server: InfluxDB 1.7.8
- Server agent: Telegraf
- Data monitoring: Grafana

## Setup

**Electrical Circuit**: Wire ambient sensor on MCU as described in [this tutorial](https://howtomechatronics.com/tutorials/arduino/dht11-dht22-sensors-temperature-and-humidity-tutorial-using-arduino/):

![](https://howtomechatronics.com/wp-content/uploads/2016/01/DHT22-DHT11-Circuit-Schematics.png)

**Firmware**: Install [Influx-Arduino library](https://github.com/teebr/Influx-Arduino) and upload home-guard/Arduino/ambient_sensor into MCU.

**IoT Stack**: Clone the [IoT Stack repository](https://github.com/gcgarner/IOTstack) on the server host and install InfluxDB, Telefraf, and Grafana via Docker Compose.

## Usage

**Monitoring**: Connect to home network and access Grafana on `http://serverhost:3000`. Login with `admin` and `<password>`, then access the *House* dashboard.

**Export ambient measurements**: On any terminal (from any host on the local network):

```bash
curl -G 'http://pi:8086/query?pretty=true' --data-urlencode "db=weather" --data-urlencode 'q=SELECT * FROM "ambient"' -H "Accept: application/csv"
```

## Resources

[1] [IoT with an ESP32, InfluxDB and Grafana - Thomas Bruen](https://medium.com/@teebr/iot-with-an-esp32-influxdb-and-grafana-54abc9575fb2)
