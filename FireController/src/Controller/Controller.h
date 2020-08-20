#include <Arduino.h>

#include "../Estimator/Estimator.h"
#include "../KiwiGPIO.h"
#include "../Libraries/RoboClaw/RoboClaw.h"

#ifndef KIWI_CONTROLLER
#define KIWI_CONTROLLER

class Controller {

    typedef enum {
        mode_safe,
        mode_closed,
        mode_open,
        num_modes
    } ModeType;

    typedef struct {
        ModeType type;
        void (Controller::*method)(void);
    } StateMachineType;

    StateMachineType StateMachine[num_modes] = {
        { mode_closed, &Controller::sm_closed },
        { mode_open, &Controller::sm_open }
    };

public:
    Controller();

    void init();

    void main();

private:
    // RoboClaw _throttle_valve;
    Estimator _estimator;

    void sm_closed(void);

    void sm_open(void);

    void _initializeRunValve(void);

    void _openRunValve(void);

    void _closeRunValve(void);
};

#endif