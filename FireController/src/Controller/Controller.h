#include <Arduino.h>
#include <RoboClaw.h>

#include "../Estimator/Estimator.h"
#include "../KiwiGPIO.h"

#ifndef KIWI_CONTROLLER
#define KIWI_CONTROLLER

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

class Controller {

public:
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
        { state_open, &Controller::sm_open }
    };

    typedef enum {
        control_mode_open,
        control_mode_closed
    } ControlMode;

    typedef enum {
        engine_mode_cold,
        engine_mode_hot,
    } EngineMode;

    Controller();

    void init();

    void main();

private:
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
        Ignition preburn defines the time in ms the igniter burns before starting nitrous flow.
        This allows pyrolized ABS fumes to build up in the chamber and improve ignition reliability.
        The preignition sequence only runs during startup, not during live restarts.
    */
    long _ignition_preburn = 200;

    /*
        Ignition duration defines the time in ms the igniter burns after nitrous flow has begun.
    */
    long _ignition_duration = 500;

    /*
        Run duration defines the maximum duration of the test sequence after T0
    */
    long _run_duration = 5000;

    void sm_closed(void);

    void sm_open(void);

    void _initializeRunValve(void);

    void _openRunValve(void);

    void _closeRunValve(void);

    int _throttlePositionToInput(float _angle);
};

#endif