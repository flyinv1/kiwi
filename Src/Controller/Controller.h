#include <Arduino.h>
#include <Roboclaw.h>

class Controller {

public:

    Estimator estimator;
    Roboclaw throttle;

    Controller();

    bool init();



};