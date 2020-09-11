#include "Main.h"

#include <Arduino.h>

#include "../Controller/Controller.h"
#include "../StateClock/StateClock.h"

Main::Main()
{
    encoder = BinaryPacket();
    controller = Controller();
    missionClock = StateClock();
    missionClock.start();
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
    // Update primary mission clock
    missionClock.tick();

    // Update encoder serial buffer
    read();

    // Execute state machine method
    if (state < num_states) {
        (this->*StateMachine[state].method)();
    }
}

void Main::setState(Main::StateType next_state)
{
    if (next_state < num_states) {
        for (int i = 0; i < num_transitions; i++) {
            if (TransitionTable[i].prev == state && TransitionTable[i].next == next_state) {
                (this->*TransitionTable[i].method)();
                return;
            }
        }
    }
}

void Main::sm_disconnected()
{
    // Update controller and sensors;
    controller.main();

    // If timeout, then send sync byte
    if (missionClock.state_et() > DISCONNECT_INTERVAL_MS) {
        size_t length = 1;
        uint8_t _buffer[length] = { SYNC };
        bool _overflow = encoder.write(_buffer, length);
        encoder.send();

        // Reset the mission clock state timer
        missionClock.advance();
    }
}

void Main::sm_standby()
{
    // Update controller
    controller.main();
}

void Main::sm_armed()
{
    // Update controller
    controller.main();

    // Transmit data
}

void Main::sm_running()
{
    // Update controller
    controller.main();

    // Transmit data
}

void Main::sm_error()
{
    // Update controller
    controller.main();
}

/*

*/
void Main::smt_disconnected_to_standby()
{
}

void Main::smt_standby_to_armed()
{
}

void Main::smt_armed_to_running()
{
}

void Main::smt_running_to_armed()
{
}

void Main::smt_armed_to_standby()
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
    case SYNC: {
        _sync();
    } break;
    case RUN_ARM: {
        controller.arm();
    } break;
    case RUN_DISARM: {
        controller.disarm();
    } break;
    case RUN_START: {
        _start();
    } break;
    case RUN_STOP: {
        _stop();
    } break;
    case SET_CONTROLMODE: {
        if (_configurable()) {
            if (controller.setControlModeFrom(buffer, len) == Controller::CONTROL_MODE_ERROR) {
                // Error setting control mode
            }
        }
    } break;
    case SET_ENGINEMODE: {
        if (_configurable()) {
            if (controller.setEngineModeFrom(buffer, len) == Controller::CONTROL_MODE_ERROR) {
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
            controller.setTargetsFrom(buffer, len);
        }
    } break;
    case GET_CONFIGURATION: {
        _getConfiguration();
    } break;
    case RUN_CALIBRATE_LOAD: {
        controller.tareThrustCell();
    } break;
    case GET_STATE: {
        _getState();
    } break;
    default: {

    } break;
    }
}

/*
    On a synchronization message from the gateway, the fire controller should respond with a sync message of its own and transition into standby mode.
*/
void Main::_sync()
{
}

void Main::_start()
{
}

void Main::_stop()
{
}

void Main::_getConfiguration()
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

void Main::_getState()
{
}