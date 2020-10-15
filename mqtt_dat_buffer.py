import paho.mqtt.client as mqtt
import random
from time import sleep
from time import thread_time
from math import sin
import json

frequency = 50

ip = '192.168.1.10'
port = 1883

def on_connect(client, userdata, flags, rc):
    client.subscribe('run/start')

def on_publish(client, userdata, mid):
    print('publish')

client = mqtt.Client()
client.on_connect = on_connect
# client.on_publish = on_publish
client.connect(ip, port)
client.loop_start()

while True:
    _p1 = 500 + random.randrange(0, 1000) / 50
    _p2 = 450 + random.randrange(0, 1000) / 40
    _F = 40 + random.randrange(-100, 100) / 50
    _m = 15 + random.randrange(-10, 10) / 10
    _v = 250 + random.randrange(-50, 50)
    _Th = 40 + 10 * sin(thread_time() * 100) + random.randrange(-20, 20) / 20

    buffer = [
        _p1,
        _p2,
        _F,
        _m,
        _v,
        _Th
    ]

    print(buffer)

    client.publish('telemetry/buffer', json.dumps(buffer))

    sleep(1 / frequency)

