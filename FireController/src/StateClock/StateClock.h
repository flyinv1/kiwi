#include <Arduino.h>

#ifndef STATE_CLOCK_H
#define STATE_CLOCK_H

class StateClock {

public:
    void start()
    {
        _total_clock_start = micros();
        _state_clock_start = micros();
        // _total_clock = 0;
        // _state_clock = 0;
        // _total_dt = 0;
        // _state_dt = 0;
        // _total_ticks = 0;
        // _state_ticks = 0;
        // _should_update_state = false;
    }

    void tick()
    {
        long _t = micros();

        if (_total_ticks > 0) {
            _total_dt = (_t - _total_clock_start) / _total_ticks;
        }

        if (_should_update_state) {
            _state_dt = (_t - _state_clock_start) / _state_ticks;
        } else {
            _should_update_state = true;
        }

        _total_clock = _t - _total_clock_start;
        _state_clock = _t - _state_clock_start;

        _total_ticks++;
        _state_ticks++;
    }

    void advance()
    {
        _state_clock = 0;
        _state_clock_start = micros();
        _should_update_state = false;
    }

    long long total_et()
    {
        return _total_clock;
    }

    long long total_et_ms()
    {
        return _total_clock / 1000;
    }

    long long state_et()
    {
        return _state_clock;
    }

    long long state_et_ms()
    {
        return _state_clock / 1000;
    }

    float total_dt()
    {
        return _total_dt;
    }

    float total_dt_ms()
    {
        return _total_dt / 1000;
    }

    float state_dt_ms()
    {
        return _state_dt / 1000;
    }

private:
    long long _total_ticks = 0;
    long long _state_ticks = 0;

    long _total_clock_start;
    long _state_clock_start;

    long long _total_clock;
    long long _state_clock;

    double _total_dt;
    double _state_dt;

    bool _should_update_state;
};

#endif