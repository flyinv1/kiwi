// #include "src/Estimator/Estimator.h"
#include <Arduino.h>

#include "src/Manager/Manager.h"

#define __KIWI_USELIVE__ false

Manager manager = Manager();

void setup()
{
    manager.init();
}

void loop()
{
    manager.loop();
}
