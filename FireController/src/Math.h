#include <Arduino.h>

template <typename T>
static T clamp(const T& x, const T& min, const T& max)
{
    return x <= min ? min : x >= max ? max : x;
};