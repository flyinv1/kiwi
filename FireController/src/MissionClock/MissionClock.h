class MissionClock {

public:
    void start()
    {
        _mission_clock_start = micros();
        _state_clock_start = micros();
    }

    void tick()
    {
        long _t = micros();

        if (_mission_ticks > 0) {
            _mission_dt += (_t - _mission_clock) / _mission_ticks;
        }

        if (_should_update_state) {
            _state_dt += (_t - _state_clock) / _state_ticks;
        } else {
            _should_update_state = true;
        }

        _mission_clock = _t - _mission_clock_start;
        _state_clock = _t - _state_clock_start;

        _state_ticks++;
        _mission_ticks++;
    }

    void nextState()
    {
        _state_clock = micros();
        _should_update_state = false;
    }

    long long mission_et()
    {
        return _mission_clock;
    }

    long long state_et()
    {
        return _state_clock;
    }

    double mission_dt()
    {
        return _mission_dt;
    }

    double state_dt()
    {
        return _state_dt;
    }

private:
    long long _mission_ticks = 0;
    long long _state_ticks = 0;

    long _mission_clock_start;
    long _state_clock_start;

    long long _mission_clock;
    long long _state_clock;

    double _mission_dt;
    double _state_dt;

    bool _should_update_state = false;
}