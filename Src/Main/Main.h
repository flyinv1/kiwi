#include <Arduino.h>

#ifndef KIWI_MAIN
#define KIWI_MAIN

class Main {

    typedef enum {
        state_boot,
        state_standby,
        state_calibrate,
        state_fire,
        state_cooldown,
        state_abort,
        state_error,
        state_shutdown,
        num_states
    } StateType;

    typedef struct {
        StateType State;
        // main state functions neither take arguments or return a value
        void (*method)(void);
    } StateMachineType;

    StateMachineType StateMachine[] = {
        { state_boot, sm_boot },
        { state_standby, sm_standby },
        { state_calibrate, sm_calibrate },
        { state_fire, sm_fire },
        { state_cooldown, sm_cooldown },
        { state_abort, sm_abort },
        { state_error, sm_error },
        { state_shutdown, sm_shutdown },
    }

    public : Main();

    void init();

    void run()
    {
        if (state < num_states) {
            (*StateMachine[state].method)();
        };
    }

private:
    StateType state = state_boot;

    void sm_boot();

    void sm_standby();

    void sm_calibrate();

    void sm_fire();

    void sm_cooldown();

    void sm_abort();

    void sm_error();

    void sm_shutdown();
};

#endif