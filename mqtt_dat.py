import paho.mqtt.client as mqtt
import random
from time import sleep
import json

topics = [
    'run/enginemode',
    'run/controlmode',
    'run/duration',
    'run/keyframes',
    'run/igniter/voltage',
    'run/igniter/preburn',
    'run/igniter/duration',
    'run/arm',
    'run/start',
    'run/status',
]

ip = '192.168.0.16'
port = 1883

def on_message(client, userdata, msg):
    print(str(msg.topic) + ": " + str(json.loads(msg.payload)))
    client.publish('echo', msg.payload)

def on_connect(client, userdata, flags, rc):
    for topic in topics:
        print('topic: ' + topic)
        client.subscribe(topic)

client = mqtt.Client()
client.on_message = on_message
client.on_connect = on_connect
client.connect(ip, port)
client.loop_forever()

