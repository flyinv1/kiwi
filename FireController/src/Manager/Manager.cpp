#include "Manager.h"

#include <Arduino.h>

#include "../Controller/Controller.h"
#include "../StateClock/StateClock.h"

Manager::Manager()
{
    encoder = BinaryPacket();
    controller = Controller();
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
    while (!Serial) { };
    missionClock.start();
    controller.init();
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

    // Update SYNC bytes and PING
    int _mil = millis();
    if (state == state_disconnected) {
        // Send ping byte
        if (_mil - comm_timer > DISCONNECT_INTERVAL_MS) {
            sendById(SYNC, nullptr, 0);
            comm_timer = _mil;
        }
    } else {
        // Send the current state as a keepalive ping
        if (millis() - comm_timer > PING_INTERVAL_MS) {
            uint8_t _buff[1] = { state };
            sendById(STATE, _buff, 1);
            comm_timer = _mil;
        }
    }

    // Execute state machine method
    if (state < num_states) {
        (this->*StateMachine[state].method)();
    }
}

/*
    Iterate over statetransition table
    -> find transition which matches current and next states
    -> execute the transition method and set the next state if successful
    return transition success
*/
bool Manager::setState(Manager::StateType next_state)
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
                    return true;
                }
                return false;
            }
        }
    }
    return false;
}

//// STATE MACHINE METHODS

/*
    The MCU has no serial connection to the server
    Ping serial stream every DISCONNECT_INTERVAL_MS milliseconds to establish a connection
*/
void Manager::sm_disconnected()
{
    controller.main();
}

void Manager::sm_standby()
{
    // Update controller
    controller.main();

    if (missionClock.total_et_ms() - data_timer > DAQ_INTERVAL_MS_STANDBY) {
        controller.getEngineDataBuffer(engine_data_buffer);
        sendById(DATA, engine_data_buffer, Controller::engine_data_size * 4);
        data_timer = missionClock.total_et_ms();
    }
}

void Manager::sm_armed()
{
    // Update controller
    controller.main();

    // Transmit data
    if (missionClock.total_et_ms() - data_timer > DAQ_INTERVAL_MS) {
        controller.getEngineDataBuffer(engine_data_buffer);
        sendById(DATA, engine_data_buffer, Controller::engine_data_size * 4);
        data_timer = missionClock.total_et_ms();
    }
}

void Manager::sm_running()
{
    // Update controller
    controller.main();

    if (missionClock.total_et_ms() - data_timer > DAQ_INTERVAL_MS) {
        controller.getEngineDataBuffer(engine_data_buffer);
        sendById(DATA, engine_data_buffer, Controller::engine_data_size * 4);
        data_timer = missionClock.total_et_ms();
    }

    // Check for running exit condition
    if (controller.getState() == Controller::state_safe) {
        setState(state_standby);
    }
}

void Manager::sm_error()
{
    // Update controller
    controller.main();
    if (controller.getState() != Controller::state_safe) {
        // Do something
    }
}

//// STATE TRANSITION METHODS

/*
    smt_disconnected_to_standby

    Controller connected to server
*/
bool Manager::smt_disconnected_to_standby()
{
    led.interval = LED::STANDBY;
    controller.tareThrustCell();
    return true;
}

bool Manager::smt_standby_to_armed()
{
    controller.arm();
    if (controller.getState() == Controller::state_armed) {
        led.interval = LED::ARMED;
        return true;
    } else {
        return false;
    }
}

/*
    smt_armed_to_running
*/
bool Manager::smt_armed_to_running()
{
    // Start the controller's fire sequence
    controller.fire();
    led.interval = LED::RUNNING;
    return true;
}

bool Manager::smt_running_to_standby()
{
    led.interval = LED::STANDBY;
    return true;
}

/*
    smt_armed_to_standby
*/
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
                    (this->*TopicTable[i].callback)(id, _buffer + 1, len - 1);
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

//// SERVER COMM CALLBACKS

/*
    On a synchronization message from the gateway, the fire controller should respond with a sync message of its own and transition into standby mode.
*/
void Manager::_on_sync(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_standby);
}

/*
    Triggered on user 'ARM' input
    The manager attempts to enter 'ARMED' 
        state: state_armed
        transition: smt_armed_to_running
*/
void Manager::_on_arm(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_armed);
}

/*
    Triggered on user 'DISARM' input
    The manager attempts to enter 'STANDBY'
        state: state_standby
        transition: smt_armed_to_standby
*/
void Manager::_on_disarm(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_standby);
}

/*
    Triggered on user 'FIRE' input
    The manager attempts to enter 'RUNNING'
        state: state_running
        transition: smt_armed_to_running
*/
void Manager::_on_start(uint8_t topic, uint8_t* buffer, size_t len)
{
    setState(state_running);
}

/*
    Triggered on user 'ABORT' input
    The controller begins shutdown procedures
        -> begin to monitor controller state and eventually enters standby
*/
void Manager::_on_stop(uint8_t topic, uint8_t* buffer, size_t len)
{
    controller.abort();
}

/*
    User Input
    Set the engine control mode, respond with update control mode buffer
    If the control mode cannot be set, respond with current mode
*/
void Manager::_on_set_controlmode(uint8_t topic, uint8_t* buffer, size_t len)
{
    sendById(SET_CONTROLMODE, buffer, len);
    if (_configurable() && len > 0) {
        uint8_t _control_mode = buffer[0];
        if (_control_mode == Controller::CONTROL_MODE_OPEN || _control_mode == Controller::CONTROL_MODE_CLOSED) {
            Controller::ControlMode _new_control_mode = controller.setControlMode(_control_mode);
            uint8_t _buff[1] = { _new_control_mode };
            sendById(SET_CONTROLMODE, _buff, 1);
        } else {
            // Error setting the control mode
            uint8_t _buff[1] = { Controller::CONTROL_MODE_ERROR };
            sendById(SET_CONTROLMODE, _buff, 1);
        }
    } else {
        Controller::ControlMode _old_control_mode = controller.getControlMode();
        uint8_t _buff[1] = { _old_control_mode };
        sendById(SET_CONTROLMODE, _buff, 1);
    }
}

void Manager::_on_set_enginemode(uint8_t topic, uint8_t* buffer, size_t len)
{
    sendById(SET_ENGINEMODE, buffer, len);
    if (_configurable() && len > 0) {
        uint8_t _engine_mode = buffer[0];
        if (_engine_mode == Controller::ENGINE_MODE_HOT || _engine_mode == Controller::ENGINE_MODE_COLD) {
            Controller::EngineMode _new_engine_mode = controller.setEngineMode(_engine_mode);
            uint8_t _buff[1] = { _new_engine_mode };
            sendById(SET_ENGINEMODE, _buff, 1);
        } else {
            // Invalid engine mode
            uint8_t _buff[1] = { uint8_t(Controller::ENGINE_MODE_ERROR) };
            sendById(SET_ENGINEMODE, _buff, 1);
        }
    } else {
        Controller::EngineMode _old_engine_mode = controller.getEngineMode();
        uint8_t _buff[1] = { _old_engine_mode };
        sendById(SET_ENGINEMODE, _buff, 1);
    }
}

void Manager::_on_set_runduration(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len == 4) {
        uint32_t _duration = encoder.readUInt(buffer, len);
        controller.setRunDuration(_duration);
        sendById(SET_RUNDURATION, buffer, len);
    } else {
        uint8_t _buff[4];
        encoder.castUInt(controller.getRunDuration(), _buff);
        sendById(SET_RUNDURATION, _buff, 4);
    }
}

void Manager::_on_set_igniterpreburn(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len == 4) {
        uint32_t _duration = encoder.readUInt(buffer, len);
        controller.setIgnitionPreburn(_duration);
        sendById(SET_IGNITERPREBURN, buffer, len);
    } else {
        uint8_t _buff[4];
        encoder.castUInt(controller.getIgnitionPreburn(), _buff);
        sendById(SET_IGNITERPREBURN, _buff, 4);
    }
}

void Manager::_on_set_igniterduration(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len == 4) {
        uint32_t _duration = encoder.readUInt(buffer, len);
        controller.setIgnitionDuration(_duration);
        sendById(SET_IGNITERDURATION, buffer, len);
    } else {
        uint8_t _buff[4];
        encoder.castUInt(controller.getIgnitionDuration(), _buff);
        sendById(SET_IGNITERDURATION, _buff, 4);
    }
}

void Manager::_on_set_ignitervoltage(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable() && len == 4) {
        uint32_t _voltage = encoder.readUInt(buffer, len);
        controller.setIgnitionVoltage(_voltage);
        sendById(SET_IGNITERVOLTAGE, buffer, len);
    } else {
        uint8_t _buff[4];
        encoder.castUInt(controller.getIgnitionVoltage(), _buff);
        sendById(SET_IGNITERPREBURN, _buff, 4);
    }
}

void Manager::_on_set_targets(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
        controller.setTargetsFrom(buffer, len);
        size_t _targetBufferSize = controller.getTargetCount() * 8;
        uint8_t _targetBuffer[_targetBufferSize] = {};
        controller.getTargetBuffer(_targetBuffer);
        sendById(SET_TARGETS, _targetBuffer, _targetBufferSize);
    }
}

void Manager::_on_calibrate_thrust(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
        controller.tareThrustCell();
    }
}

void Manager::_on_set_throttle_position(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
        uint32_t _motorPosition = encoder.readUInt(buffer, len);
        uint32_t _clampedPos = controller.setThrottlePosition(_motorPosition);
        uint8_t _positionBuff[4] = {};
        encoder.castUInt(_clampedPos, _positionBuff);
        sendById(THROTTLE_POSITION, _positionBuff, 4);
    }
}

void Manager::_on_set_encoder_value(uint8_t topic, uint8_t* buffer, size_t len)
{
    if (_configurable()) {
        uint32_t _encoderValue = encoder.readUInt(buffer, len);
        uint32_t _clampedPos = controller.setEncoderValue(_encoderValue);
        uint8_t _positionBuff[4] = {};
        encoder.castUInt(_clampedPos, _positionBuff);
        sendById(THROTTLE_ENCODER, _positionBuff, 4);
    }
}

/*
    Request for current state from upstream, send current state
*/
void Manager::_on_state(uint8_t topic, uint8_t* buffer, size_t len)
{
    uint8_t _buff[1] = { state };
    sendById(STATE, _buff, 1);
}

/*
    Python server has sent a disconnect code, enter disconnect state
*/
void Manager::_on_close(uint8_t topic, uint8_t* buffer, size_t len)
{

    if (state == state_armed) {
        setState(state_standby);
    }
    setState(state_disconnected);
}

/*
    Check that the manager is in a configurable state
    Only returns true if in standby
*/
bool Manager::_configurable()
{
    return (state == state_standby);
}