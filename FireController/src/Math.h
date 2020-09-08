#include <Arduino.h>

template <class type>
static type clamp(type value, type min, type max)
{
    return (value > min ? value : min) < max ? value : max;
};