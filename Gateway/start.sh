#!/bin/bash

# Kiwi thrust stand startup script
# Matt Vredevoogd - 08/16/20

# Get absolute path to server configuration
CONFIG=$(pwd)/server.conf

# Kill mosquitto if already running (on system startup)
echo "Stopping current broker"
systemctl stop mosquitto

# Start MQTT Broker with gateway config
echo "Starting gateway broker"
mosquitto -c $CONFIG -d -v

# Start main python script
python3 $(pwd)/main.py



