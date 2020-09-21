#include <Arduino.h>
#include <RoboClaw.h>

#include "../Estimator/Estimator.h"
#include "../KiwiGPIO.h"
#include "../StateClock/StateClock.h"
#include "../Target/Target.h"

#ifndef KIWI_CONTROLLER
#define KIWI_CONTROLLER

#define DEFAULT_RUN_DURATION_MS      10000
#define MAXIMUM_RUN_DURATION_MS      20000
#define DEFAULT_IGNITION_DURATION_MS 1000
#define MAXIMUM_IGNITION_DURATION_MS 2000
#define DEFAULT_IGNITION_PREBURN_MS  1000
#define MAXIMUM_IGNITION_PREBURN_MS  2000
#define DEFAULT_IGNITION_VOLTAGE     300
#define MAXIMUM_IGNITION_VOLTAGE     600

#define SAFE_PRESSURE_PSI           5   // Safe pressure threshold in psi
#define OVERPRESSURE_RUN_PSI        850 // Maximum allowable pressure in propellant lines
#define OVERPRESSURE_CHAMBER_PSI    600 // Maximum allowable pressure in chamber
#define SHUTDOWN_DRAIN_INTERVAL     500
#define SHUTDOWN_MAXIMUM_PERIOD     10000
#define IGNITION_PRESSURE_THRESHOLD 200

/*
    Define linear offset and scale for ignition voltage from 8 bit DAC input (pwm approximation)
    V_igniter [0 - 1000V] = Input * IGNITER_SCALE + IGNITER_OFFSET
    Input [0 - 255] = (V_igniter - IGNITER_OFFSET ) / IGNITER_SCALE
*/
#define IGNITER_SCALE          2.8577f
#define IGNITER_OFFSET         -12.9516f
#define IGNITER_DAC_RESOLUTION 8

/*
    Define motor endpoints
    RoboClaw extends Stream, requires a serial baud rate and target MCU address
    throttleAngleToEncoder() + throttleEncoderToAngle() provides a mapping from 0-90deg to 660-0 using ANG and POS values
*/
#define MOTOR_ADDRESS       0x80
#define MOTOR_BAUD          460800
#define MOTOR_TIMEOUT       10000
#define THROTTLE_ANG_CLOSED 0
#define THROTTLE_ANG_OPEN   90
#define THROTTLE_POS_CLOSED 660
#define THROTTLE_POS_OPEN   0
#define THROTTLE_POS_SDN    200
#define THROTTLE_EQ_DBAND   4   // position deadband of 0.5deg
#define TARGET_SCALE        100 // scale angle targets to preserve resolution

/*
    RoboClaw motor control parameters required for position control
    Acceleration and decceleration rates are the same
*/
#define THROTTLE_ACC     4000
#define THROTTLE_VEL     2000
#define THROTTLE_VEL_SDN 150

/**
 *  Define maximum allowable targets, no engine test is planned to allow a large number of target setpoints
 */
#define TARGETS 128

class Controller {

    typedef void (Controller::*StateMethod)(void);
    typedef bool (Controller::*TransitionMethod)(void);

public:
    typedef enum {
        state_safe,
        state_armed,
        state_preburn,
        state_igniting,
        state_firing,
        state_shutdown,
        num_states
    } StateType;

    typedef struct {
        StateType state;
        StateMethod method;
    } StateMachineType;

    StateMachineType StateMachine[num_states] = {
        { state_safe, &Controller::sm_safe },
        { state_armed, &Controller::sm_armed },
        { state_preburn, &Controller::sm_preburn },
        { state_igniting, &Controller::sm_igniting },
        { state_firing, &Controller::sm_firing },
        { state_shutdown, &Controller::sm_shutdown },
    };

    typedef enum {
        transition_safe_to_armed,
        transition_armed_to_preburn,
        transition_preburn_to_igniting,
        transition_igniting_to_firing,
        transition_firing_to_shutdown,
        transition_igniting_to_shutdown,
        transition_preburn_to_shutdown,
        transition_armed_to_safe,
        transition_shutdown_to_safe,
        num_transitions
    } StateTransitions;

    typedef struct {
        StateType prev;
        StateType next;
        TransitionMethod method;
    } StateMachineTransitionType;

    StateMachineTransitionType TransitionTable[num_transitions] = {
        { state_safe, state_armed, &Controller::smt_safe_to_armed },
        { state_armed, state_preburn, &Controller::smt_armed_to_preburn },
        { state_preburn, state_igniting, &Controller::smt_preburn_to_igniting },
        { state_igniting, state_firing, &Controller::smt_igniting_to_firing },
        { state_firing, state_shutdown, &Controller::smt_firing_to_shutdown },
        { state_igniting, state_shutdown, &Controller::smt_igniting_to_shutdown },
        { state_preburn, state_shutdown, &Controller::smt_preburn_to_shutdown },
        { state_shutdown, state_safe, &Controller::smt_shutdown_to_safe },
        { state_armed, state_safe, &Controller::smt_armed_to_safe },
    };

    typedef enum : uint8_t {
        CONTROL_MODE_OPEN = 0,    // Open loop control using throttle position (deg)
        CONTROL_MODE_CLOSED = 1,  // Closed loop control using chamber pressure feedback
        CONTROL_MODE_ERROR = 255, // Error control mode assignment
    } ControlMode;

    typedef enum : uint8_t {
        ENGINE_MODE_COLD = 0,
        ENGINE_MODE_HOT = 1,
        ENGINE_MODE_ERROR = 255,
    } EngineMode;

    typedef enum {
        data_chamber_pressure,
        data_upstream_pressure,
        data_downstream_pressure,
        data_thrust,
        data_propellant_mass,
        data_mass_flow,
        data_throttle_position,
        data_igniter_voltage,
        data_mission_elapsed_time,
        data_state_elapsed_time,
        data_delta_time,
        engine_data_size
    } EngineData;

    Controller();

    void init();
    void main();
    void arm();
    void disarm();
    void fire();
    void abort();

    StateType getState();

    void setTargetsFrom(uint8_t* buffer, size_t len);
    void setTargets(Target* _targets, size_t len);
    size_t getTargets(Target* _outputBuffer, size_t maximum = TARGETS);
    size_t getTargetBuffer(uint8_t* _outputBuffer);
    int getTargetCount();

    void setRunDuration(uint32_t duration);
    uint32_t getRunDuration();

    void setIgnitionDuration(uint32_t duration);
    uint32_t getIgnitionDuration();

    void setIgnitionPreburn(uint32_t duration);
    uint32_t getIgnitionPreburn();

    void setIgnitionVoltage(uint32_t voltage);
    uint32_t getIgnitionVoltage();

    ControlMode setControlMode(ControlMode _mode);
    ControlMode getControlMode();

    EngineMode setEngineMode(EngineMode _mode);
    EngineMode getEngineMode();

    uint32_t setThrottlePosition(uint32_t position);
    uint32_t setEncoderValue(uint32_t value);

    void getEngineData(EngineData* data);
    void getEngineDataBuffer(uint8_t* _output);
    void tareThrustCell();

private:
    StateType state = state_safe;

    float engineState[engine_data_size];

    StateClock engineClock;
    StateClock targetClock;

    /*
        Targets are described as an array of Target structs:
        { 
            uint32_t time;
            uint32_t value;
        }
        Where `time` describes the time (in ms) at which to transition to the next value. The target value depends on the current engine mode, hot will use chamber pressure, while cold will set throttle position.
    */
    Target _target_buffer[TARGETS];
    size_t _num_targets = 0;
    uint32_t _target;

    /*
        Run valve is controlled by digital HIGH / LOW
    */
    bool runValveOpen = false;

    /*
        Igniter is controlled by pwm digital signal
    */
    bool igniterActive = false;
    uint32_t igniterOutputVoltage = 0;
    uint8_t igniterOutputSignal = 0;

    /*
        RoboClaw provides a serial interface to the standalone RoboClaw 2x7A motor controller
    */
    RoboClaw throttle_valve = RoboClaw(&Serial4, MOTOR_TIMEOUT);

    /*
        Estimator interfaces with various instruments to provide data acquisition and feedback for control.
    */
    Estimator estimator;

    /*
        Control mode defines whether the thruster operates using an open throttle position or closed loop chamber pressure control scheme
    */
    ControlMode control_mode = CONTROL_MODE_OPEN;

    /*
        Engine mode defines whether the thruster is operated hot or cold. During a hot run, the igniter fires, during a cold run, no ignition sequence is run and nitrous is cold-flowed
    */
    EngineMode engine_mode = ENGINE_MODE_COLD;

    /*
        Run duration defines the maximum duration of the test sequence after T0
    */
    uint32_t _run_duration = DEFAULT_RUN_DURATION_MS;

    /* 
        Ignition preburn defines the time in ms the igniter burns before starting nitrous flow.
        This allows pyrolized ABS fumes to build up in the chamber and improve ignition reliability.
        The preignition sequence only runs during startup, not during live restarts.
    */
    uint32_t _ignition_preburn = DEFAULT_IGNITION_PREBURN_MS;

    /*
        Ignition duration defines the time in ms the igniter burns after nitrous flow has begun.
    */
    uint32_t _ignition_duration = DEFAULT_IGNITION_DURATION_MS;

    uint32_t _ignition_voltage = DEFAULT_IGNITION_VOLTAGE;

    void setState(StateType next_state);

    void readEngineState();

    void initializeRunValve(void);

    void openRunValve(void);

    void closeRunValve(void);

    void initializeIgniter(void);

    void setIgniterOutputVoltage(uint32_t voltage);

    void shutdownIgniter();

    void activateIgniter();

    uint32_t throttleAngleToEncoder(float _angle);

    float throttleEncoderToAngle(int position);

    void sm_safe(void);

    void sm_armed(void);

    void sm_preburn(void);

    void sm_igniting(void);

    void sm_firing(void);

    void sm_shutdown(void);

    bool smt_safe_to_armed(void);

    bool smt_armed_to_preburn(void);

    bool smt_preburn_to_igniting(void);

    bool smt_igniting_to_firing(void);

    bool smt_firing_to_shutdown(void);

    bool smt_igniting_to_shutdown(void);

    bool smt_preburn_to_shutdown(void);

    bool smt_shutdown_to_safe(void);

    bool smt_armed_to_safe(void);

    void writeEncoderPosition(uint32_t position);

    uint32_t readLastEncoderPosition(void);
};

#endif