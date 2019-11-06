#!/usr/bin/env python2

import paho.mqtt.client as mqtt
from time import sleep
from random import uniform
from numpy import clip


client = mqtt.Client()
client.connect("localhost", 1883)

hum = 50.0
while True:
	hum = clip(hum + uniform(-5, 5), 0, 100)
	client.publish("sensors/humidity", "humidity value=%.2f"%hum)
	sleep(5.0)
