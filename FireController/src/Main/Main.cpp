#include "Main.h"

#include <Arduino.h>

#include "./Controller/Controller.h"

Main::Main()
{
}

void Main::init()
{
    Serial.begin(115200);
    gateway.init(&Serial);
}