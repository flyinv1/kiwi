#include "Manager.h"

#include <Arduino.h>

#include "../Controller/Controller.h"
#include "../StateClock/StateClock.h"

Manager::Manager()
{
    encoder = BinaryPacket();
    // controller = Controller();
    missionClock = StateClock();
    led = LED();
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
}

void Manager::loop()
{
    // Get that sweet, sweet visual feedback
    led.update();

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
                    missionClock.advance();
                    uint8_t _statebuffer[1] = { next_state };
                    sendById(STATE, _statebuffer, 1);
                    state = next_state;
                }
                return;
            }
        }
    }
}

void Manager::sm_disconnected()
{
    // If timeout, then send sync byte
    if (missionClock.state_et() > DISCONNECT_INTERVAL_MS * 1000) {
        sendById(SYNC, nullptr, 0);
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
    led.interval = LED::STANDBY;
    return true;
}

bool Manager::smt_standby_to_armed()
{
    led.interval = LED::ARMED;
    return true;
}

bool Manager::smt_armed_to_running()
{
    led.interval = LED::RUNNING;
    return true;
}

bool Manager::smt_running_to_armed()
{
    led.interval = LED::ARMED;
    return true;
}

bool Manager::smt_armed_to_standby()
{
    led.interval = LED::STANDBY;
    return true;
}

bool Manager::smt_standby_to_disconnected()
{
    led.interval = LED::DISCONNECTED;
    return true;
}

void Manager::read()
{
    encoder.read();
    if (encoder.overflow()) {
        encoder.flush();
    } else if (encoder.packetAvailable()) {
        uint8_t _buffer[256];
        size_t len = encoder.packet(_buffer);
        uint8_t id = _buffer[0];
        if (id < num_callbacks) {
            for (int i = 0; i < num_callbacks; i++) {
                if (TopicTable[i].topic == Manager::TopicType(id)) {
                    (this->*TopicTable[i].callback)(id, _buffer, len);
                }
            }
        }
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
void Manager::_on_sync(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_standby);
}

void Manager::_on_arm(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_armed);
    controller.arm();
}

void Manager::_on_disarm(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_standby);
    controller.disarm();
}

void Manager::_on_run_start(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_running);
    controller.fire();
}

void Manager::_on_run_stop(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_armed);
    controller.abort();
}

void Manager::_on_set_controlmode(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len > 0) {
        Controller::ControlMode _control_mode = buffer[0];
        if (_control_mode == Controller::CONTROL_MODE_OPEN || _control_mode == Controller::CONTROL_MODE_CLOSED) {
            controller.setControlMode(_control_mode);
        } else {
            // Invalid control mode
        }
    }
}

void Manager::_on_set_enginemode(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len > 4) {
        Controller::EngineMode _engine_mode = buffer[0];
        if (_engine_mode == Controller::ENGINE_MODE_HOT || _engine_mode == Controller::ENGINE_MODE_COLD) {
            controller.setEngineMode(_engine_mode);
        } else {
            // Invalid engine mode
        }
    }
    // May want to respond w/ packet for set confirmation ?
}

void Manager::_on_set_runduration(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len == 4) {
        uint32_t _duration = encoder.readUInt32(buffer, len);
        controller.setRunDuration(_duration);
    }
}

void Manager::_on_set_igniterpreburn(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len == 4) {
        uint32_t _duration = encoder.readUInt32(buffer, len);
        controller.setIgnitionPreburn(_duration);
    }
}

void Manager::_on_set_igniterduration(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len == 4) {
        uint32_t _duration = encoder.readUInt32(buffer, len);
        controller.setIgnitionDuration(_duration);
    }
}

void Manager::_on_set_targets(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
        controller.setTargetsFrom(buffer, len);
    }
}

void Manager::_on_run_calibrate_thrust(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
        controller.tareThrustCell();
    }
}

void Manager::_on_state(uint8_t topic, uint8_t* buffer, size_t len)
{
}

void Manager::_on_close(uint8_t topic, uint8_t* buffer, size_t len)
{
    state = state_disconnected;
}

bool Manager::_configurable()
{
    return (state == state_standby);
}