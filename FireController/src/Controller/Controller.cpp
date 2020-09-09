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
    estimator.begin();
    throttle_valve.begin(MOTOR_BAUD);
    initializeRunValve();
    initializeIgniter();
};

void Controller::main()
{
    engineClock.tick();
    estimator.main();
    if (state < num_states) {
        (this->*StateMachine[state].method)();
    }
}

void Controller::setState(Controller::StateType next_state)
{
    if (next_state < num_states) {
        for (int i = 0; i < num_transitions; i++) {
            if (TransitionTable[i].prev == state && TransitionTable[i].next == next_state) {
                if ((this->*TransitionTable[i].method)()) {
                    engineClock.advance();
                    state = next_state;
                }
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
    _target = 0;
}

/**
 * PRIVATE METHODS
 */

void Controller::sm_safe(void)
{
    // Update estimator and populate state
    readEngineState();
}

void Controller::sm_armed(void)
{
    // Update estimator and populate state
    readEngineState();
}

void Controller::sm_preburn(void)
{
    readEngineState();

    // Check preburn casualty response parameters
    if (engine_mode == ENGINE_MODE_COLD) {
        if (engineState.upstream_pressure > SAFE_PRESSURE_PSI || engineState.downstream_pressure > SAFE_PRESSURE_PSI) {
            // should shutdown -> nitrous may be present in lines
        }

    } else {
        if (engineState.downstream_pressure > SAFE_PRESSURE_PSI) {
            // should shutdown
        }
    }
}

void Controller::sm_igniting(void)
{
    if (engine_mode == ENGINE_MODE_COLD) {
    } else {
    }

    if (engineClock.state_et() > _ignition_duration * 1000) {
        if (engine_mode == ENGINE_MODE_HOT) {
            if (engineState.chamber_pressure < IGNITION_PRESSURE_THRESHOLD) {
                setState(state_shutdown);
            } else {
                setState(state_firing);
            }
        } else {
            setState(state_firing);
        }
    }
}

void Controller::sm_firing(void)
{
    targetClock.tick();

    if (engine_mode == ENGINE_MODE_COLD) {
        if (_target < _num_targets) {
            uint32_t _current_throttle_target = uint32_t(_target_buffer[_target].value);
            _current_throttle_target = clamp<uint32_t>(throttleAngleToEncoder(float(_current_throttle_target)), THROTTLE_POS_OPEN, THROTTLE_POS_CLOSED);
            throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL, THROTTLE_ACC, _current_throttle_target, 0);
        }
    } else if (engine_mode == ENGINE_MODE_HOT) {
        // perform closed loop control
    }

    // Transition targets
    if (targetClock.state_et() > _target_buffer[_target].time * 1000) {
        if (_target < _num_targets) {
            _target++;
            targetClock.advance();
        } else {
            setState(state_shutdown);
        }
    } else {
    }
}

void Controller::sm_shutdown(void)
{
    if (engineClock.state_et() > SHUTDOWN_DRAIN_INTERVAL * 1000) {
        throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL_SDN, THROTTLE_ACC, THROTTLE_POS_SDN, 0);
    }
    if (abs(throttle_valve.ReadEncM1(MOTOR_ADDRESS) - THROTTLE_POS_SDN) > THROTTLE_EQ_DBAND) {
        // Monitor upstream and downstream pressure
        if (engineState.upstream_pressure < SAFE_PRESSURE_PSI && engineState.downstream_pressure < SAFE_PRESSURE_PSI) {
            if (engine_mode == ENGINE_MODE_HOT && engineState.chamber_pressure < SAFE_PRESSURE_PSI) {
                setState(state_safe);
            } else {
                setState(state_safe);
            }
        }
    } else if (engineClock.state_et() > SHUTDOWN_MAXIMUM_PERIOD * 1000) {
        setState(state_safe);
    }
}

bool Controller::smt_safe_to_armed(void)
{
    return true;
}

bool Controller::smt_armed_to_preburn(void)
{
    /*
        - Turn on the igniter to the run setpoint
    */
    if (engine_mode == ENGINE_MODE_HOT) {
        activateIgniter();
        setIgniterOutputVoltage(_ignition_voltage);
    }
    return true;
}

bool Controller::smt_preburn_to_igniting(void)
{
    /*
        - Open the run valve
        - Command the throttle valve motor to the full open position
    */
    openRunValve();
    throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL, THROTTLE_ACC, THROTTLE_POS_OPEN, 0);
    return true;
}

bool Controller::smt_igniting_to_firing(void)
{
    /*
        - Start the target clock
        - Shutdown the igniter
    */
    targetClock.start();
    setIgniterOutputVoltage(0);
    shutdownIgniter();
    return true;
}

bool Controller::smt_firing_to_shutdown(void)
{
    /*
        - Close the run valve
        - The motor position is handled by the shutdown loop
    */
    closeRunValve();
    return true;
}

bool Controller::smt_igniting_to_shutdown(void)
{
    /*
        - Shutdown the igniter
        - Close the nitrous run valve
    */
    setIgniterOutputVoltage(0);
    shutdownIgniter();
    closeRunValve();
    return true;
}

bool Controller::smt_preburn_to_shutdown(void)
{
    /*
        - Shutdown the igniter
    */
    setIgniterOutputVoltage(0);
    shutdownIgniter();
    return true;
}

bool Controller::smt_shutdown_to_safe(void)
{
    return true;
}

bool Controller::smt_armed_to_safe(void)
{
    return true;
}

void Controller::readEngineState()
{
    engineState.chamber_pressure = estimator.getChamberPressure();
    engineState.downstream_pressure = estimator.getDownstreamPressure();
    engineState.upstream_pressure = estimator.getUpstreamPressure();
    engineState.propellant_mass = estimator.getPropellantMass();
    engineState.thrust = estimator.getPropellantMass();
    engineState.mass_flow = 0;

    uint32_t _throttle_position = throttle_valve.ReadEncM1(MOTOR_ADDRESS);
    engineState.throttle_position = throttleEncoderToAngle(_throttle_position);

    engineState.mission_elapsed_time = float(engineClock.total_et());
    engineState.state_elapsed_time = float(engineClock.state_et());
    engineState.delta_time = float(engineClock.total_dt());
}

void Controller::initializeRunValve(void)
{
    pinMode(pin_run_valve, OUTPUT);
    digitalWrite(pin_run_valve, LOW);
    runValveOpen = false;
}

void Controller::openRunValve(void)
{
    runValveOpen = true;
    digitalWrite(pin_run_valve, HIGH);
}

void Controller::closeRunValve(void)
{
    runValveOpen = false;
    digitalWrite(pin_run_valve, LOW);
}

void Controller::initializeIgniter(void)
{
    pinMode(pin_igniter_ctr, OUTPUT);
    analogWriteResolution(IGNITER_DAC_RESOLUTION);
    analogWrite(pin_igniter_ctr, 0);
    analogWrite(pin_igniter_sdn, HIGH);
    igniterOutput = 0;
    igniterActive = false;
}

void Controller::setIgniterOutputVoltage(uint32_t _voltage)
{
    // Convert voltage to clamped 8 bit value
    uint8_t _input = clamp<uint8_t>(uint8_t(ceil((_voltage - IGNITER_OFFSET) / IGNITER_SCALE)), 0, MAXIMUM_IGNITION_VOLTAGE);
    igniterOutput = _input;
    analogWrite(pin_igniter_ctr, _input);
}

void Controller::shutdownIgniter(void)
{
    digitalWrite(pin_igniter_sdn, HIGH);
}

void Controller::activateIgniter(void)
{
    igniterActive = false;
    digitalWrite(pin_igniter_sdn, LOW);
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
        Controller::ControlMode _control_mode = ControlMode(buffer[0]);
        if (_control_mode == CONTROL_MODE_OPEN || _control_mode == CONTROL_MODE_CLOSED) {
            control_mode = _control_mode;
            return _control_mode;
        }
    }
    control_mode = CONTROL_MODE_ERROR;
    return CONTROL_MODE_ERROR;
}

Controller::EngineMode Controller::setEngineModeFrom(uint8_t* buffer, size_t len)
{
    if (len > 0) {
        Controller::EngineMode _engine_mode = EngineMode(buffer[0]);
        if (_engine_mode == ENGINE_MODE_COLD || _engine_mode == ENGINE_MODE_HOT) {
            Estimator::PressureMode _pressure_mode = (engine_mode == ENGINE_MODE_COLD) ? Estimator::THROTTLE : Estimator::CHAMBER;
            engine_mode = _engine_mode;
            estimator.setPressureMode(_pressure_mode);
            return engine_mode;
        }
    }
    engine_mode = ENGINE_MODE_ERROR;
    return ENGINE_MODE_ERROR;
}