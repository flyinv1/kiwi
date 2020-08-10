#include "Controller.h"

#include <Arduino.h>

// #include "src/Libraries/RoboClaw/RoboClaw.h"

#include "../Estimator/Estimator.h"

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
    pinMode(_run_valve_pin, OUTPUT);
    digitalWrite(_run_valve_pin, LOW);
}

void Controller::_openRunValve(void)
{
    digitalWrite(_run_valve_pin, HIGH);
}

void Controller::_closeRunValve(void)
{
    digitalWrite(_run_valve_pin, LOW);
}
