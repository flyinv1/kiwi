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

    int interval;
    if (state == state_disconnected) {
        interval = 200;
    } else {
        interval = 1000;
    }

    if (millis() - t > interval) {
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
        if (id < num_callbacks) {
            (this->*TopicTable[id].callback)(id, buffer, len);
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
    controller.arm();
}

void Manager::_on_disarm(uint8_t topic, uint8_t* buffer, size_t len)
{
    controller.disarm();
}

void Manager::_on_run_start(uint8_t topic, uint8_t* buffer, size_t len)
{
    controller.fire();
}

void Manager::_on_run_stop(uint8_t topic, uint8_t* buffer, size_t len)
{
    controller.abort();
}

void Manager::_on_set_controlmode(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
    }
}

void Manager::_on_set_enginemode(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
    }
}

void Manager::_on_set_runduration(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
    }
}

void Manager::_on_set_igniterpreburn(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
    }
}

void Manager::_on_set_igniterduration(uint8_t topic, uint8_t* buffer, size_t len)
{
}

void Manager::_on_set_targets(uint8_t topic, uint8_t* buffer, size_t len)
{
}

void Manager::_on_get_configuration(uint8_t topic, uint8_t* buffer, size_t len)
{
}

void Manager::_on_run_calibrate_thrust(uint8_t topic, uint8_t* buffer, size_t len)
{
}

void Manager::_on_state(uint8_t topic, uint8_t* buffer, size_t len)
{
}

bool Manager::_configurable()
{
    return (state == state_standby);
}