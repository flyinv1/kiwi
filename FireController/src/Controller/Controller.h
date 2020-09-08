#include <Arduino.h>
#include <RoboClaw.h>

#include "../Estimator/Estimator.h"
#include "../KiwiGPIO.h"
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
    _throttlePercentToInput provides a mapping from 0-90deg to 660-0 using ANG and POS values
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

#define TARGETS 32

class Controller {

public:
    typedef enum {
        state_armed,
        state_preburn,
        state_igniting,
        state_firing,
        state_shutdown
    }

    typedef enum {
        state_safe,
        state_closed,
        state_open,
        num_states
    } StateType;

    typedef struct {
        StateType type;
        void (Controller::*method)(void);
    } StateMachineType;

    StateMachineType StateMachine[num_states] = {
        { state_closed, &Controller::sm_closed },
        { state_open, &Controller::sm_open },
        { state_safe, &Controller::sm_safe }
    };

    typedef enum {
        control_mode_open = 0,
        control_mode_closed = 1,
        control_mode_error = 255,
    } ControlMode;

    typedef enum {
        engine_mode_cold = 0,
        engine_mode_hot = 1,
        engine_mode_error = 255,
    } EngineMode;

    Controller();

    void init();

    void main();

    void setState(StateType next);

    void setTargets(uint8_t* buffer, size_t len);

    void setRunDuration(uint32_t duration);

    void setIgnitionDuration(uint32_t duration);

    void setIgnitionPreburn(uint32_t duration);

    void setIgnitionVoltage(uint32_t voltage);

    ControlMode setControlModeFrom(uint8_t* buffer, size_t len);

    EngineMode setEngineModeFrom(uint8_t* buffer, size_t len);

private:
    StateType state = state_safe;

    Target _target_buffer[TARGETS];
    size_t _num_targets;
    uint32_t _current_target;

    /*
        Actuator instances
    */
    RoboClaw _throttle_valve = RoboClaw(&Serial4, MOTOR_TIMEOUT);
    Estimator _estimator;

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

    void sm_closed(void);

    void sm_open(void);

    void sm_safe(void);

    void _initializeRunValve(void);

    void _openRunValve(void);

    void _closeRunValve(void);

    int _throttlePositionToInput(float _angle);
};

#endif