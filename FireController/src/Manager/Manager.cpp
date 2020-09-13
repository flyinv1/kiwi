#include "Manager.h"

#include <Arduino.h>

#include "../Controller/Controller.h"
#include "../StateClock/StateClock.h"

Manager::Manager()
{
    encoder = BinaryPacket();
    // controller = Controller();
    missionClock = StateClock();
}

Manager::~Manager()
{
}

void Manager::init()
{
    // Begin serial communications with RPI
    Serial.begin(115200);
    missionClock.start();
    encoder.setStream(&Serial);

    // Initialize the controller to standby mode
    // controller.init();
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);
}

void Manager::loop()
{
    if (millis() - t > 200) {
        digitalWrite(13, !on);
        on = !on;
        t = millis();
    }

    // Update primary mission clock
    missionClock.tick();

    // Update encoder serial buffer
    read();

    // Execute state machine method
    if (state < num_states) {
        (this->*StateMachine[state].method)();
    }
}

void Manager::setState(Manager::StateType next_state)
{
    if (next_state < num_states) {
        for (int i = 0; i < num_transitions; i++) {
            if (TransitionTable[i].prev == state && TransitionTable[i].next == next_state) {
                if ((this->*TransitionTable[i].method)()) {
                    // Do nothing ?
                }
                return;
            }
        }
    }
}

void Manager::sm_disconnected()
{
    // Update controller and sensors;
    // controller.main();

    // If timeout, then send sync byte
    if (missionClock.state_et() > DISCONNECT_INTERVAL_MS * 1000) {
        size_t length = 5;
        uint8_t _buffer[length] = { SYNC, 5, 10, 20, 30 };
        encoder.write(_buffer, length);
        encoder.send();

        // Reset the mission clock state timer
        missionClock.advance();
    }
}

void Manager::sm_standby()
{
    // Update controller
    // controller.main();
}

void Manager::sm_armed()
{
    // Update controller
    // controller.main();

    // Transmit data
}

void Manager::sm_running()
{
    // Update controller
    // controller.main();

    // Transmit data
}

void Manager::sm_error()
{
    // Update controller
    // controller.main();
}

/*

*/
bool Manager::smt_disconnected_to_standby()
{
}

bool Manager::smt_standby_to_armed()
{
}

bool Manager::smt_armed_to_running()
{
}

bool Manager::smt_running_to_armed()
{
}

bool Manager::smt_armed_to_standby()
{
}

void Manager::read()
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

void Manager::_on(uint8_t id, uint8_t* buffer, size_t len)
{
    switch (id) {
    case SYNC: {
        _sync();
    } break;
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
    case STATE: {
        _getState();
    } break;
    default: {

    } break;
    }
}

void Manager::sendById(uint8_t id, uint8_t* buffer, size_t length)
{
    size_t _length = length + 1;
    uint8_t _buffer[length] = {};

    _buffer[0] = id;

    for (int i = 0; i < length; i++) {
        _buffer[i + 1] = buffer[i];
    }

    if (encoder.write(_buffer, _length)) {
        encoder.send();
    }
}

/*
    On a synchronization message from the gateway, the fire controller should respond with a sync message of its own and transition into standby mode.
*/
void Manager::_sync()
{
    setState(state_standby);
    _getState();
}

void Manager::_arm()
{
}

void Manager::_disarm()
{
}

void Manager::_start()
{
}

void Manager::_stop()
{
}

void Manager::_getConfiguration()
{
}

void Manager::_calibrate_thrust()
{
}

void Manager::_calibrate_propellant()
{
}

bool Manager::_configurable()
{
    return (state == state_standby);
}

/*
    Get current manager and controller states, transmit them upstream
*/
void Manager::_getState()
{
    Controller::StateType _controller_state = controller.getState();
    Manager::StateType _manager_state = state;

    size_t len = 2;
    uint8_t payload[len] = { uint8_t(_manager_state), uint8_t(_controller_state) };
    sendById(STATE, payload, len);
}