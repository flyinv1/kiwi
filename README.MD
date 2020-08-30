# Kiwi

## About
Kiwi is a subscale hybrid rocket engine designed to test closed loop throttle control and engine restartability. The engine uses nitrous oxide and FDM ABS plastic as fuel.

### Project Structure
This repository contains all software components of the project, including the test stand real time controller, telemetry gateway, and ground control application.

__FireController__ - Fire control and data acquisition (real time on Teensy 4.0).

__Gateway__ - Server running an MQTT broker and python gateway to facilitate communication between MCU and ground control station (RPI 4).

__GroundControl__ - Electron application w/ MQTT client as a control interface to the test stand with real time telemetry.

_This project is an extension of __ARCC at UW's__ work to design a low cost, rocket powered VTVL test vehicle. See more at [arcc.space](https://arcc.space)_.
