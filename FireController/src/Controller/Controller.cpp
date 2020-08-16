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
