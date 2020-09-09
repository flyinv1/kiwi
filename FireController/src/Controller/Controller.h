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
#define DEFAULT_IGNITION_DURATION_MS 400
#define MAXIMUM_IGNITION_DURATION_MS 1000
#define DEFAULT_IGNITION_PREBURN_MS  200
#define MAXIMUM_IGNITION_PREBURN_MS  1000
#define DEFAULT_IGNITION_VOLTAGE     300
#define MAXIMUM_IGNITION_VOLTAGE     600

#define SAFE_PRESSURE_PSI        5   // Safe pressure threshold in psi
#define OVERPRESSURE_RUN_PSI     850 // Maximum allowable pressure in propellant lines
#define OVERPRESSURE_CHAMBER_PSI 600 // Maximum allowable pressure in chamber

/*
    Define linear offset and scale for ignition voltage from 8 bit DAC input (pwm approximation)
    V_igniter [0 - 1000V] = Input * IGNITER_SCALE + IGNITER_OFFSET
    Input [0 - 255] = (V_igniter - IGNITER_OFFSET ) / IGNITER_SCALE
*/
#define IGNITER_SCALE  2.8577f
#define IGNITER_OFFSET -12.9516f

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

/*
    RoboClaw motor control parameters required for position control
    Acceleration and decceleration rates are the same
*/
#define THROTTLE_ACC 2000
#define THROTTLE_VEL 800

/**
 *  Define maximum allowable targets, no engine test is planned to allow a large number of target setpoints
 */
#define TARGETS 32

class Controller {

    typedef void (Controller::*StateMethod)(void);
    typedef void (Controller::*TransitionMethod)(void);

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

    typedef enum {
        control_mode_open = 0,    // Open loop control using throttle position (deg)
        control_mode_closed = 1,  // Closed loop control using chamber pressure feedback
        control_mode_error = 255, // Error control mode assignment
    } ControlMode;

    typedef enum {
        engine_mode_cold = 0,
        engine_mode_hot = 1,
        engine_mode_error = 255,
    } EngineMode;

    typedef struct {
        float chamber_pressure;
        float upstream_pressure;
        float downstream_pressure;
        float thrust;
        float propellant_mass;
        float mass_flow;
        float throttle_position;
        float mission_elapsed_time;
        float state_elapsed_time;
        float delta_time;
    } EngineData;

    Controller();

    void init();

    void main();

    void arm();

    void disarm();

    void fire();

    void abort();

    void setTargets(uint8_t* buffer, size_t len);

    void setRunDuration(uint32_t duration);

    void setIgnitionDuration(uint32_t duration);

    void setIgnitionPreburn(uint32_t duration);

    void setIgnitionVoltage(uint32_t voltage);

    void tareThrustCell();

    ControlMode setControlModeFrom(uint8_t* buffer, size_t len);

    EngineMode setEngineModeFrom(uint8_t* buffer, size_t len);

    void getEngineData(EngineData& data);

private:
    StateType state = state_safe;

    EngineData data;

    StateClock engineClock;

    Target _target_buffer[TARGETS];
    size_t _num_targets;
    uint32_t _current_target;

    /*
        Actuator instances
    */
    RoboClaw throttle_valve = RoboClaw(&Serial4, MOTOR_TIMEOUT);
    Estimator estimator;

    /*
        Control mode defines whether the thruster operates using an open or closed loop chamber pressure control scheme
    */
    ControlMode _control_mode = control_mode_open;

    /*
        Engine mode defines whether the thruster is operated hot or cold. During a hot run, the igniter fires, during a cold run, no ignition sequence is run and nitrous is flowed cold
    */
    EngineMode _engine_mode = engine_mode_cold;

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

    int throttleAngleToEncoder(float _angle);

    float throttleEncoderToAngle(int position);

    void sm_safe(void);

    void sm_armed(void);

    void sm_preburn(void);

    void sm_igniting(void);

    void sm_firing(void);

    void sm_shutdown(void);

    void smt_safe_to_armed(void);

    void smt_armed_to_preburn(void);

    void smt_preburn_to_igniting(void);

    void smt_igniting_to_firing(void);

    void smt_firing_to_shutdown(void);

    void smt_igniting_to_shutdown(void);

    void smt_preburn_to_shutdown(void);

    void smt_shutdown_to_safe(void);

    void smt_armed_to_safe(void);
};

#endif