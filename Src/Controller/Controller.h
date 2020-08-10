#include <Arduino.h>

// #include "src/Libraries/RoboClaw/RoboClaw.h"

#include "../Estimator/Estimator.h"

#ifndef KIWI_CONTROLLER
#define KIWI_CONTROLLER

class Controller {

    // enum typedef {
    //     mode_safe,
    //     mode_closed,
    //     mode_open
    // } ModeType;

    // typedef struct {
    //     ModeType type;
    //     void (*method)(void);
    // } StateMachineType;

    // StateMachineType StateMachine[num_states] = {
    //     { mode_closed, sm_closed },
    //     { mode_open, sm_open }
    // };

public:
    // Controller();

    // void init();

    // void main();

private:
    //     // RoboClaw _throttle_valve;
    //     Estimator _estimator;

    //     int _run_valve_pin = 0;

    //     void sm_closed(void)
    //     {
    //         // closed loop control
    //     }

    //     void sm_open(void)
    //     {
    //         // open loop control
    //     }

    //     void _initializeRunValve(void)
    //     {
    //         pinMode(_run_valve_pin, OUTPUT);
    //         digitalWrite(_run_valve_pin, LOW);
    //     }

    //     void _openRunValve(void)
    //     {
    //         digitalWrite(_run_valve_pin, HIGH);
    //     }

    //     void _closeRunValve(void)
    //     {
    //         digitalWrite(_run_valve_pin, LOW);
    //     }
};

#endif