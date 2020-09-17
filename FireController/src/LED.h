#include <Arduino.h>

#ifndef LED_H
#define LED_H

struct LED {

    typedef enum {
        DISCONNECTED = 3000,
        STANDBY = 500,
        ARMED = 200,
        RUNNING = 50,
        ERROR = 3000,
    } IntervalType;

public:
    IntervalType interval = DISCONNECTED;

    LED()
    {
        pinMode(ledPin, OUTPUT);
    }

    void update()
    {
        if (millis() - t > interval) {
            digitalWrite(13, !on);
            t = millis();
            on = !on;
        }
    }

private:
    int ledPin = 13;
    long t = 0;
    bool on = false;
};

#endif