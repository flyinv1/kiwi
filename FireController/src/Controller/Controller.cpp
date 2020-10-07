#include "Controller.h"

#include <Arduino.h>
#include <EEPROM.h>
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
    initializeRunValve();
    initializeIgniter();
    throttle_valve.begin(MOTOR_BAUD);
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
    setState(state_preburn);
}

void Controller::abort()
{
    // All active states include a state -> shutdown transition
    setState(state_shutdown);
}

Controller::StateType Controller::getState()
{
    return state;
}

void Controller::setRunDuration(uint32_t duration)
{
    run_duration = clamp<uint32_t>(duration, 0, MAXIMUM_RUN_DURATION_MS);
}

uint32_t Controller::getRunDuration()
{
    return run_duration;
}

void Controller::setIgnitionDuration(uint32_t duration)
{
    ignition_duration = clamp<uint32_t>(duration, 0, MAXIMUM_IGNITION_DURATION_MS);
}

uint32_t Controller::getIgnitionDuration()
{
    return ignition_duration;
}

void Controller::setIgnitionPreburn(uint32_t duration)
{
    ignition_preburn = clamp<uint32_t>(duration, 0, MAXIMUM_IGNITION_PREBURN_MS);
}

uint32_t Controller::getIgnitionPreburn()
{
    return ignition_preburn;
}

void Controller::setIgnitionVoltage(uint32_t voltage)
{
    ignition_voltage = clamp<uint32_t>(voltage, 0, MAXIMUM_IGNITION_VOLTAGE);
}

uint32_t Controller::getIgnitionVoltage()
{
    return ignition_voltage;
}

Controller::ControlMode Controller::getControlMode()
{
    return control_mode;
}

Controller::EngineMode Controller::getEngineMode()
{
    return engine_mode;
}

void Controller::tareThrustCell()
{
    estimator.tareThrustCell();
}

void Controller::setTargetsFrom(uint8_t* buffer, size_t len)
{
    // Set target keyframes
    size_t targets = len > TARGETS * 8 ? TARGETS * 8 : len;
    num_targets = Target::decode(buffer, targets, target_buffer);
    target_index = 0;
}

void Controller::setTargets(Target* _targets, size_t len)
{
    for (int i = 0; i < len; i++) {
        target_buffer[i] = _targets[i];
    }
    num_targets = len;
}

int Controller::getTargetCount()
{
    return num_targets;
}

size_t Controller::getTargets(Target* _outputBuffer, size_t maximum = TARGETS)
{
    for (int i = 0; i < num_targets; i++) {
        _outputBuffer[i] = target_buffer[i];
    }
}

size_t Controller::getTargetBuffer(uint8_t* _outputBuffer)
{
    size_t _output_size = Target::encode(target_buffer, num_targets, _outputBuffer);
    return _output_size;
}

uint32_t Controller::setThrottlePosition(uint32_t position)
{
    uint32_t _clamped = clamp<uint32_t>(position, THROTTLE_POS_OPEN, THROTTLE_POS_CLOSED);
    throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL, THROTTLE_ACC, _clamped, 1);
    return _clamped;
}

uint32_t Controller::setEncoderValue(uint32_t value)
{
    uint32_t _clamped = clamp<uint32_t>(value, THROTTLE_POS_OPEN, THROTTLE_POS_CLOSED);
    throttle_valve.SetEncM1(MOTOR_ADDRESS, _clamped);
    writeEncoderPosition(_clamped);
    return _clamped;
}

/**
 * PRIVATE METHODS
 */

void Controller::sm_safe(void)
{
    readEngineState();
}

void Controller::sm_armed(void)
{
    readEngineState();
}

void Controller::sm_preburn(void)
{
    readEngineState();

    // Check preburn casualty response parameters
    if (engine_mode == ENGINE_MODE_COLD) {
        if (engineState[data_upstream_pressure] > SAFE_PRESSURE_PSI || engineState[data_downstream_pressure] > SAFE_PRESSURE_PSI) {
            // should shutdown -> nitrous may be present in lines
        }
        setState(state_igniting);
    } else {
        if (engineState[data_downstream_pressure] > SAFE_PRESSURE_PSI) {
            // should shutdown
        }
    }

    if (engineClock.state_et_ms() > ignition_preburn) {
        setState(state_igniting);
    }
}

void Controller::sm_igniting(void)
{

    readEngineState();

    if (engine_mode == ENGINE_MODE_COLD) {
        setState(state_firing);
    } else {
    }

    if (engineClock.state_et_ms() > ignition_duration) {
        setState(state_firing);
    }
}

void Controller::sm_firing(void)
{
    targetClock.tick();

    readEngineState();

    if (control_mode == CONTROL_MODE_CLOSED) {
        if (target_index < num_targets) {
            if (engine_mode == ENGINE_MODE_HOT) {
                /*
                    Engine is hot
                    - Perform closed loop pressure control
                */
                float _pressure_target = float(target_buffer[target_index].value) / TARGET_SCALE;
                float _current_pressure = estimator.getChamberPressure();
                // PID HERE
            } else {
                /*
                    Engine is cold
                    - No closed loop control for this mode
                */
            }
        }
        // perform closed loop control
    }

    /*
        Execute target transition
    */
    if (targetClock.total_et_ms() > target_buffer[target_index].time) {
        if (target_index < num_targets) {
            if (control_mode == CONTROL_MODE_OPEN) {
                float _target_angle = float(target_buffer[target_index].value) / TARGET_SCALE;
                uint32_t _target_position = clamp<uint32_t>(throttleAngleToEncoder(_target_angle), THROTTLE_POS_OPEN, THROTTLE_POS_CLOSED);
                throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL, THROTTLE_ACC, _target_position, 1);
            }
            target_index++;
            targetClock.advance();
        } else {
            setState(state_shutdown);
        }
    } else {
    }

    if (engineClock.state_et_ms() > run_duration) {
        setState(state_shutdown);
    }
}

void Controller::sm_shutdown(void)
{

    readEngineState();

    if (abs(throttle_valve.ReadEncM1(MOTOR_ADDRESS) - THROTTLE_POS_CLOSED) > THROTTLE_EQ_DBAND) {
        throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL_SDN, THROTTLE_ACC, THROTTLE_POS_CLOSED, 1);
        // Monitor upstream and downstream pressure
        // if (engineState.upstream_pressure < SAFE_PRESSURE_PSI && engineState.downstream_pressure < SAFE_PRESSURE_PSI) {
        //     if (engine_mode == ENGINE_MODE_HOT && engineState.chamber_pressure < SAFE_PRESSURE_PSI) {
        //         setState(state_safe);
        //     } else {
        //         setState(state_safe);
        //     }
        // }
    } else {
        setState(state_safe);
    }
    // CHANGE
}

bool Controller::smt_safe_to_armed(void)
{
    /*
        - Start of firing procedure
        - The user must verify the throttle valve is in the open position and calibrate if necessary
        - Open: ENC = 0
        - Closed: ENC = 660
    */
    uint32_t _saved_position = readLastEncoderPosition();
    throttle_valve.SetEncM1(MOTOR_ADDRESS, _saved_position);
    throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL, THROTTLE_ACC, THROTTLE_POS_CLOSED, 1);
    return true;
}

bool Controller::smt_armed_to_preburn(void)
{
    /*
        - Turn on the igniter to the run setpoint
    */
    if (engine_mode == ENGINE_MODE_HOT) {
        activateIgniter();
        setIgniterOutputVoltage(ignition_voltage);
    }
    return true;
}

bool Controller::smt_preburn_to_igniting(void)
{
    /*
        - Open the run valve
        - Command the throttle valve motor to the full open position if hot firing, otherwise go to first target setpoint
    */
    openRunValve();
    uint32_t _initial_throttle;
    if (num_targets > 0) {
        uint32_t _target_angle = float(target_buffer[0].value) / TARGET_SCALE;
        _initial_throttle = clamp<uint32_t>(throttleAngleToEncoder(_target_angle), THROTTLE_POS_OPEN, THROTTLE_POS_CLOSED);
    } else {
        _initial_throttle = THROTTLE_POS_IGN;
    }
    throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL, THROTTLE_ACC, _initial_throttle, 0);
    return true;
}

bool Controller::smt_igniting_to_firing(void)
{
    /*
        - Start the target clock
        - Shutdown the igniter
    */
    target_index = 0;
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
    throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL_SDN, THROTTLE_ACC, THROTTLE_POS_CLOSED, 1);
    closeRunValve();
    return true;
}

bool Controller::smt_igniting_to_shutdown(void)
{
    /*
        - Shutdown the igniter
        - Close the nitrous run valve
    */
    throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL_SDN, THROTTLE_ACC, THROTTLE_POS_CLOSED, 1);
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
    throttle_valve.SpeedAccelDeccelPositionM1(MOTOR_ADDRESS, THROTTLE_ACC, THROTTLE_VEL_SDN, THROTTLE_ACC, THROTTLE_POS_CLOSED, 1);
    setIgniterOutputVoltage(0);
    shutdownIgniter();
    return true;
}

bool Controller::smt_shutdown_to_safe(void)
{
    /*
        - End of firing sequence
        - The motor is moved back to the closed position and the encoder value is recorder
    */
    writeEncoderPosition(throttle_valve.ReadEncM1(MOTOR_ADDRESS));
    // engineClock.start();
    return true;
}

bool Controller::smt_armed_to_safe(void)
{
    writeEncoderPosition(throttle_valve.ReadEncM1(MOTOR_ADDRESS));
    // engineClock.start();
    return true;
}

void Controller::readEngineState()
{
    engineState[data_chamber_pressure] = estimator.getChamberPressure();
    engineState[data_downstream_pressure] = estimator.getDownstreamPressure();
    engineState[data_upstream_pressure] = estimator.getUpstreamPressure();
    engineState[data_propellant_mass] = estimator.getPropellantMass();
    engineState[data_thrust] = estimator.getThrust();
    engineState[data_mass_flow] = 0;
    engineState[data_igniter_voltage] = igniterOutputVoltage;

    uint32_t _throttle_position = throttle_valve.ReadEncM1(MOTOR_ADDRESS);
    engineState[data_throttle_position] = throttleEncoderToAngle(_throttle_position);

    engineState[data_mission_elapsed_time] = float(engineClock.total_et());
    engineState[data_state_elapsed_time] = float(engineClock.state_et());
    engineState[data_delta_time] = float(engineClock.total_dt());
}

void Controller::getEngineDataBuffer(uint8_t* _output)
{
    for (int i = 0; i < engine_data_size; i++) {
        union {
            byte b[4];
            float f;
        } u;
        u.f = engineState[i];
        for (int j = 0; j < 4; j++) {
            _output[i * 4 + j] = u.b[j];
        }
    }
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
    igniterOutputSignal = 0;
    igniterOutputVoltage = 0;
    igniterActive = false;
}

void Controller::setIgniterOutputVoltage(uint32_t _voltage)
{
    /*
        Convert voltage to clamped 8 bit value
        0-256 byte signal corresponds to 0-670V output
    */
    igniterOutputVoltage = _voltage;
    uint8_t _input = uint8_t(ceil((float(_voltage) - IGNITER_OFFSET) / IGNITER_SCALE));
    igniterOutputSignal = _input;
    analogWrite(pin_igniter_ctr, _input);
}

void Controller::shutdownIgniter(void)
{
    igniterActive = false;
    digitalWrite(pin_igniter_ctr, 0);
}

void Controller::activateIgniter(void)
{
    igniterActive = true;
}

uint32_t Controller::throttleAngleToEncoder(float _angle)
{
    // convert angle input to motor position
    return THROTTLE_POS_CLOSED - ceil(_angle / THROTTLE_ANG_OPEN * (THROTTLE_POS_CLOSED - THROTTLE_POS_OPEN));
}

float Controller::throttleEncoderToAngle(int _position)
{
    return float(THROTTLE_POS_CLOSED - _position) / (THROTTLE_POS_CLOSED - THROTTLE_POS_OPEN) * THROTTLE_ANG_OPEN;
}

Controller::ControlMode Controller::setControlMode(Controller::ControlMode _mode)
{
    control_mode = _mode;
    return control_mode;
}

Controller::EngineMode Controller::setEngineMode(Controller::EngineMode _mode)
{
    engine_mode = _mode;
    Estimator::PressureMode _pressure_mode = (_mode == Controller::ENGINE_MODE_HOT) ? Estimator::CHAMBER : Estimator::THROTTLE;
    estimator.setPressureMode(_pressure_mode);
    return engine_mode;
}

// Controller::ControlMode Controller::setControlModeFrom(uint8_t* buffer, size_t len)
// {
//     if (len > 0) {
//         Controller::ControlMode _control_mode = ControlMode(buffer[0]);
//         if (_control_mode == CONTROL_MODE_OPEN || _control_mode == CONTROL_MODE_CLOSED) {
//             control_mode = _control_mode;
//             return _control_mode;
//         }
//     }
//     control_mode = CONTROL_MODE_ERROR;
//     return CONTROL_MODE_ERROR;
// }

// Controller::EngineMode Controller::setEngineModeFrom(uint8_t* buffer, size_t len)
// {
//     if (len > 0) {
//         Controller::EngineMode _engine_mode = EngineMode(buffer[0]);
//         if (_engine_mode == ENGINE_MODE_COLD || _engine_mode == ENGINE_MODE_HOT) {
//             Estimator::PressureMode _pressure_mode = (engine_mode == ENGINE_MODE_COLD) ? Estimator::THROTTLE : Estimator::CHAMBER;
//             engine_mode = _engine_mode;
//             estimator.setPressureMode(_pressure_mode);
//             return engine_mode;
//         }
//     }
//     engine_mode = ENGINE_MODE_ERROR;
//     return ENGINE_MODE_ERROR;
// }

void Controller::writeEncoderPosition(uint32_t position)
{
    EEPROM.put(eeprom_last_encoder_count, position);
}

uint32_t Controller::readLastEncoderPosition(void)
{
    uint32_t position;
    EEPROM.get(eeprom_last_encoder_count, position);
    return position;
}