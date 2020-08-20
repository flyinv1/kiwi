#!/bin/bash

# Kiwi thrust stand startup script
# Matt Vredevoogd - 08/16/20

CONFIG=$(pwd)/server.conf

echo "Starting KIWI Gateway Controller"

# Start MQTT Broker
mosquitto -c $CONFIG

if python3 $(pwd)/main.py
then
    echo 'python?'
fi



