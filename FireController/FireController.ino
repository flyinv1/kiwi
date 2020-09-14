// #include "src/Estimator/Estimator.h"
#include <Arduino.h>

#include "src/Manager/Manager.h"

Manager manager = Manager();

void setup()
{
    manager.init();
}

void loop()
{
    manager.loop();
}
