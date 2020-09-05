#include "Controller.h"

#include <Arduino.h>

#include "../Estimator/Estimator.h"
#include "../KiwiGPIO.h"
#include "../Libraries/RoboClaw/RoboClaw.h"

Controller::Controller() {

};

void Controller::init()
{
    _estimator.init();
    _initializeRunValve();
};

void Controller::main()
{
}

void Controller::sm_closed(void)
{
}

void Controller::sm_open(void)
{
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
    int _input = THROTTLE_POS_CLOSED - _ceil(_angle / THROTTLE_ANG_OPEN * THROTTLE_POS_CLOSED);

    // clamp the input to possible motor position range
    _input = min(THROTTLE_POS_CLOSED, _input);
    _input = max(THROTTLE_POS_OPEN, _input);

    return _input;
}
