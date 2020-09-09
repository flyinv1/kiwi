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
    engineClock.start();
    estimator.init();
    throttle_valve.begin(MOTOR_BAUD);
    initializeRunValve();
};

void Controller::main()
{
    engineClock.tick();
    if (state < num_states) {
        (this->*StateMachine[state].method)();
    }
}

void Controller::setState(Controller::StateType next_state)
{
    if (next_state < num_states) {
        for (int i = 0; i < num_transitions; i++) {
            if (TransitionTable[i].prev == state && TransitionTable[i].next == next_state) {
                (this->*TransitionTable[i].method)();
                engineClock.advance();
                return;
            }
        }
    }
}

void Controller::arm()
{
    setState(state_armed);
}

void Controller::disarm()
{
    setState(state_safe);
}

void Controller::fire()
{
    setState(state_firing);
}

void Controller::abort()
{
    setState(state_shutdown);
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

void Controller::tareThrustCell()
{
    estimator.tareThrustCell();
}

void Controller::setTargets(uint8_t* buffer, size_t len)
{
    // Set target keyframes
    size_t _targets = len > TARGETS ? TARGETS : len;
    _num_targets = Target::decode(buffer, _targets, _target_buffer);
    _current_target = 0;
    _current_target = _target_buffer[_current_target].value;
}

/**
 * PRIVATE METHODS
 */

void Controller::sm_safe(void)
{
    estimator.main();
}

void Controller::sm_armed(void)
{
    // Update estimator and retrieve state
    estimator.main();
    readEngineState();
}

void Controller::sm_preburn(void)
{
}

void Controller::sm_igniting(void)
{
}

void Controller::sm_firing(void)
{
}

void Controller::sm_shutdown(void)
{
}

void Controller::smt_safe_to_armed(void)
{
}

void Controller::smt_armed_to_preburn(void)
{
}

void Controller::smt_preburn_to_igniting(void)
{
}

void Controller::smt_igniting_to_firing(void)
{
}

void Controller::smt_firing_to_shutdown(void)
{
}

void Controller::smt_igniting_to_shutdown(void)
{
}

void Controller::smt_preburn_to_shutdown(void)
{
}

void Controller::smt_shutdown_to_safe(void)
{
}

void Controller::smt_armed_to_safe(void)
{
}

void Controller::readEngineState()
{
    data.chamber_pressure = estimator.getChamberPressure();
    data.downstream_pressure = estimator.getDownstreamPressure();
    data.upstream_pressure = estimator.getUpstreamPressure();
    data.propellant_mass = estimator.getPropellantMass();
    data.thrust = estimator.getPropellantMass();
    data.mass_flow = 0;

    uint32_t _throttle_position = throttle_valve.ReadEncM1(MOTOR_ADDRESS);
    data.throttle_position = throttleEncoderToAngle(_throttle_position);

    data.mission_elapsed_time = float(engineClock.total_et());
    data.state_elapsed_time = float(engineClock.state_et());
    data.delta_time = float(engineClock.total_dt());
}

void Controller::initializeRunValve(void)
{
    pinMode(pin_run_valve, OUTPUT);
    digitalWrite(pin_run_valve, LOW);
}

void Controller::openRunValve(void)
{
    digitalWrite(pin_run_valve, HIGH);
}

void Controller::closeRunValve(void)
{
    digitalWrite(pin_run_valve, LOW);
}

int Controller::throttleAngleToEncoder(float _angle)
{
    // convert angle input to motor position
    return THROTTLE_POS_CLOSED - ceil(_angle / THROTTLE_ANG_OPEN * THROTTLE_POS_CLOSED);
}

float Controller::throttleEncoderToAngle(int _position)
{
    return float(THROTTLE_POS_CLOSED - _position) / THROTTLE_POS_CLOSED * THROTTLE_ANG_OPEN;
}

Controller::ControlMode Controller::setControlModeFrom(uint8_t* buffer, size_t len)
{
    if (len > 0) {
        uint8_t _requested_mode = buffer[0];
        Controller::ControlMode _control_mode = ControlMode(_requested_mode);
        if (_control_mode == control_mode_open || _control_mode == control_mode_closed) {
            return _control_mode;
        }
    }
    return control_mode_error;
}

Controller::EngineMode Controller::setEngineModeFrom(uint8_t* buffer, size_t len)
{
    if (len > 0) {
        uint8_t _requested_mode = buffer[0];
        Controller::EngineMode _engine_mode = EngineMode(_requested_mode);
        if (_engine_mode == engine_mode_cold || _engine_mode == engine_mode_hot) {
            Estimator::PressureMode _pressure_mode = (_engine_mode == engine_mode_cold) ? Estimator::THROTTLE : Estimator::CHAMBER;
            estimator.setPressureMode(_pressure_mode);
            return _engine_mode;
        }
    }
    return engine_mode_error;
}