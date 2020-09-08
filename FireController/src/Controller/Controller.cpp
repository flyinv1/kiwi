#include "Controller.h"

#include <Arduino.h>
#include <RoboClaw.h>

#include "../Estimator/Estimator.h"
#include "../Gateway/Gateway.h"
#include "../KiwiGPIO.h"
#include "../Math.h"
#include "../Target/Target.h"

Controller::Controller() {};

void Controller::init()
{
    _estimator.init();
    _initializeRunValve();
    _throttle_valve.begin(MOTOR_BAUD);
};

void Controller::main()
{
    if (state < num_states) {
        (this->*StateMachine[state].method)();
    }
}

void Controller::setState(StateType next)
{
    if (next < num_states) {
        state = next;
    }
}

void Controller::setRunDuration(uint32_t duration)
{
    // Set run duration in ms
    _run_duration = clamp<uint32_t>(duration, 0, MAXIMUM_RUN_DURATION_MS);
}

void Controller::setIgnitionDuration(uint32_t duration)
{
    // Set ignition duration in ms
    _ignition_duration = clamp<uint32_t>(duration, 0, MAXIMUM_IGNITION_DURATION_MS);
}

void Controller::setIgnitionPreburn(uint32_t duration)
{
    // Set ignition preburn duration in ms
    _ignition_preburn = clamp<uint32_t>(duration, 0, MAXIMUM_IGNITION_PREBURN_MS);
}

void Controller::setIgnitionVoltage(uint32_t voltage)
{
    // Set ignition voltage
    _ignition_voltage = clamp<uint32_t>(voltage, 0, MAXIMUM_IGNITION_VOLTAGE);
}

void Controller::setTargets(uint8_t* buffer, size_t len)
{
    // Set target keyframes
    size_t _targets = len > TARGETS ? TARGETS : len;
    _num_targets = Target::decode(buffer, _targets, _target_buffer);
    _current_target = 0;
    _current_target = _target_buffer[_current_target].value;
}

void Controller::sm_safe(void)
{
    _estimator.main();
}

void Controller::sm_closed(void)
{
    /*
        TODO
        Closed loop throttle control
    */
    _estimator.main();
}

void Controller::sm_open(void)
{
    _estimator.main();
}

void Controller::_initializeRunValve(void)
{
    pinMode(pin_run_valve, OUTPUT);
    digitalWrite(pin_run_valve, LOW);
}

void Controller::_openRunValve(void)
{
    digitalWrite(pin_run_valve, HIGH);
}

void Controller::_closeRunValve(void)
{
    digitalWrite(pin_run_valve, LOW);
}

int Controller::_throttlePositionToInput(float _angle)
{
    // convert angle input to motor position
    int _input = THROTTLE_POS_CLOSED - ceil(_angle / THROTTLE_ANG_OPEN * THROTTLE_POS_CLOSED);

    // clamp the input to possible motor position range
    _input = min(THROTTLE_POS_CLOSED, _input);
    _input = max(THROTTLE_POS_OPEN, _input);

    return _input;
}

Controller::ControlMode Controller::setControlModeFrom(uint8_t* buffer, size_t len)
{
    if (len > 0) {
        uint8_t _requested_mode = buffer[0];
        switch (_requested_mode) {
        case 0: {
            _control_mode = control_mode_open;
        } break;
        case 1: {
            _control_mode = control_mode_closed;
        } break;
        default:
            return control_mode_error;
        }
        return _control_mode;
    }
    return control_mode_error;
}

Controller::EngineMode Controller::setEngineModeFrom(uint8_t* buffer, size_t len)
{
    if (len > 0) {
        uint8_t _requested_mode = buffer[0];
        switch (_requested_mode) {
        case 0: {
            _engine_mode = engine_mode_cold;
        } break;
        case 1: {
            _engine_mode = engine_mode_hot;
        } break;
        default:
            return engine_mode_error;
        };
        return _engine_mode;
    }
    return engine_mode_error;
}