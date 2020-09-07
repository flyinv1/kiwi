#include <Arduino.h>

#ifndef TARGET_H
#define TARGET_H

struct Target {

    uint32_t time = 0;
    uint32_t value = 0;

    Target()
    {
    }

    ~Target()
    {
    }

    static size_t decode(uint8_t* inputBuffer, size_t len, Target* targets)
    {

        size_t outputLength = (len - len % 8) / 8;

        for (uint8_t i = 0; i < outputLength; i++) {

            union {
                byte b[4];
                uint32_t v;
            } _time;

            union {
                byte b[4];
                uint32_t v;
            } _value;

            for (uint8_t j = 0; j < 4; j++) {
                _time.b[j] = inputBuffer[i * 8 + j];
                _value.b[j] = inputBuffer[i * 8 + j + 4];
            }

            Target output;
            output.time = _time.v;
            output.value = _value.v;

            targets[i] = output;
        }

        return outputLength;
    }

    static size_t encode(Target* targets, size_t len, uint8_t* outputBuffer)
    {
        size_t outputLength = len * 8;
        for (uint8_t i = 0; i < len; i++) {

            union {
                byte b[4];
                uint32_t v;
            } _time;

            union {
                byte b[4];
                uint32_t v;
            } _value;

            _time.v = targets[i].time;
            _value.v = targets[i].value;

            for (uint8_t j = 0; j < 4; j++) {
                outputBuffer[i * 8 + j] = _time.b[j];
                outputBuffer[i * 8 + j + 4] = _value.b[j];
            }
        }

        return outputLength;
    }
};

#endif