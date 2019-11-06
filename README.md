# home-guard
Logs environmental data from your house

A microcontroller measures temperature and send the measurement to a server on the local network
Upon receipt, server sends data to a timeseries database
Finally, a dashboard displays the data on a browser

Uses [ESP32-InfluxDB library](https://github.com/tobiasschuerg/ESP8266_Influx_DB/tree/v2) for Arduino IDE.

## Hardware Components

- Microcontroller Lollin32 Lite (based on ESP32)
- Temperature sensor (#)
- Ubuntu computer

## Software

- MQTT library for ESP32: PubSubClient.h
- MQTT broker: Mosquitto
- Timeseries database and dashboard: InfluxDB 2.0

## Setup

### Timeseries Database

Export token to bash. This is a password giving access to the database. Each token is created on InfluxDB, specifying the type of access (READ/WRITE) and to which buckets.

```bash
export INFLUX_TOKEN=cDXywLbr0AuFmj-ShQWe6WRVznERX7xjMwW6YnpsOypPCjLWcv-2kMAGlpRsaMPA6zbJ_zmLzypuK4wUk8VPbw==
```

Telegraf is a service to enable communication between InfluxDB and external applications in the network. Start a telegraf instance with

```bash
telegraf -config ./conf/mqtt_influx.conf
```

Start MQTT broker. This is a 

```bash
mosquitto
```


## Installation

Python MQTT Library:

```bash
pip install paho.mqtt
```



## Resources

- [InfluxDB 2.0](https://v2.docs.influxdata.com/v2.0/)
- [InfluxDB 2.0 Example project](https://www.influxdata.com/blog/prototyping-iot-with-influxdb-cloud-2-0/)



## Tutorials

[The Definitive Guide To InfluxDB In 2019](https://devconnected.com/the-definitive-guide-to-influxdb-in-2019/)

## Usage

Launch Influx daemon
Probably `influxd` is on the entrypoint

'''bash
docker run --name influxdb -p 9999:9999 quay.io/influxdb/influxdb:2.0.0-alpha
'''

In another terminal

'''bash
docker exec -it influxdb /bin/bash
'''

## Real Usage

$ influxd

$ mosquitto

$ export INFLUX_TOKEN=cDXywLbr0AuFmj-ShQWe6WRVznERX7xjMwW6YnpsOypPCjLWcv-2kMAGlpRsaMPA6zbJ_zmLzypuK4wUk8VPbw==
$ telegraf -config telegraf.conf

$ mosquitto_pub -t sensors/temperature -m "temp,someTag=tag1 val=80"
