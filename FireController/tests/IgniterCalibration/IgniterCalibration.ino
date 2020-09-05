#include <Arduino.h>

int pin = 7;
long input = 0;

void setup()
{
    pinMode(pin, OUTPUT);
    analogWriteResolution(8);
}

void loop()
{
    if (Serial.available()) {
        char c;
        c = Serial.read();
        if (c == 'w') {
            input += 10;
        } else if (c == 's') {
            input -= 10;
        }
        float fs = float(input) / 255 * 100;
        analogWrite(pin, input);
        Serial.print(input);
        Serial.print(" ");
        Serial.print(fs);
        Serial.print(" ");
        Serial.println(fs / 100 * 1000);
        Serial.read();
    }
}