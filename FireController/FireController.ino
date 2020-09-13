// #include "src/Estimator/Estimator.h"
#include <Arduino.h>

#include "src/Manager/Manager.h"

Manager manager = Manager();

void setup()
{
    manager.init();
    Serial.println("Running");
}

void loop()
{
    manager.loop();
}
