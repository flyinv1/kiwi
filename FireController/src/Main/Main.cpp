#include "Main.h"

#include <Arduino.h>

#include "../Controller/Controller.h"

Main::Main()
{
    encoder = BinaryPacket();
    controller = Controller();
}

void Main::init()
{
    // Begin serial communications with RPI
    Serial.begin(115200);
    encoder.setStream(&Serial);

    // Initialize the controller to standby mode
    controller.init();
}

void Main::loop()
{
    encoder.read();
    if (state < num_states) {
        (this->*StateMachine[state].method)();
    }
}

void Main::setState(StateType next)
{
    if (next < num_states) {
        for (int i = 0; i < num_transitions; i++) {
            if (TransitionTable[i].prev == state && TransitionTable[i].next == next) {
                (this->*TransitionTable[i].method)();
                break;
            }
        }
        state = next;
    }
}

void Main::sm_disconnected()
{
    // Update controller and sensors;
    controller.main();
}

void Main::sm_standby()
{
    controller.main();
}

void Main::sm_armed()
{
}

void Main::sm_running()
{
}

void Main::sm_error()
{
}

void Main::read()
{
    encoder.read();
    if (encoder.overflow()) {
        encoder.flush();
    } else if (encoder.packetAvailable()) {
        uint8_t* buffer = nullptr;
        size_t len = encoder.packet(buffer);
        uint8_t id = buffer[0];
        _on(id, buffer + 1, len - 1);
    }
}

void Main::_on(uint8_t id, uint8_t* buffer, size_t len)
{
    switch (id) {
    case RUN_ARM: {
        _arm();
    } break;
    case RUN_DISARM: {
        _disarm();
    } break;
    case RUN_START: {
        _start();
    } break;
    case RUN_STOP: {
        _stop();
    } break;
    case SET_CONTROLMODE: {
        if (_configurable()) {
            if (controller.setControlModeFrom(buffer, len) == Controller::control_mode_error) {
                // Error setting control mode
            }
        }
    } break;
    case SET_ENGINEMODE: {
        if (_configurable()) {
            if (controller.setEngineModeFrom(buffer, len) == Controller::engine_mode_error) {
                // Error setting engine mode
            }
        }
    } break;
    case SET_RUNDURATION: {
        if (_configurable()) {
            controller.setRunDuration(encoder.readUInt32(buffer, len));
        }
    } break;
    case SET_IGNITERDURATION: {
        if (_configurable()) {
            controller.setIgnitionDuration(encoder.readUInt32(buffer, len));
        }
    } break;
    case SET_IGNITERPREBURN: {
        if (_configurable()) {
            controller.setIgnitionPreburn(encoder.readUInt32(buffer, len));
        }
    } break;
    case SET_TARGETS: {
        if (_configurable()) {
            controller.setTargets(buffer, len);
        }
    } break;
    case GET_CONFIGURATION: {
        _get_configuration();
    } break;
    case RUN_CALIBRATE_LOAD: {
    } break;
    case RUN_CALIBRATE_PROPELLANT: {

    } break;
    default: {

    } break;
    }
}

void Main::_arm()
{
}

void Main::_disarm()
{
}

void Main::_start()
{
}

void Main::_stop()
{
}

void Main::_get_configuration()
{
}

void Main::_calibrate_thrust()
{
}

void Main::_calibrate_propellant()
{
}

bool Main::_configurable()
{
    return (state == state_standby);
}