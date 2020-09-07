#include "Controller.h"

#include <Arduino.h>
#include <RoboClaw.h>

#include "../Estimator/Estimator.h"
#include "../Gateway/Gateway.h"
#include "../KiwiGPIO.h"

Controller::Controller() {};

void Controller::init()
{
    _estimator.init();
    _initializeRunValve();
    _throttle_valve.begin(MOTOR_BAUD);
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
    int _input = THROTTLE_POS_CLOSED - ceil(_angle / THROTTLE_ANG_OPEN * THROTTLE_POS_CLOSED);

    // clamp the input to possible motor position range
    _input = min(THROTTLE_POS_CLOSED, _input);
    _input = max(THROTTLE_POS_OPEN, _input);

    return _input;
}

Controller::ControlMode Controller::setControlModeFrom(uint8_t* buffer, size_t len)
{
}