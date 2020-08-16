#include <Arduino.h>

// #include "../Controller/Controller.h"

#ifndef KIWI_MAIN
#define KIWI_MAIN

class Main {

public:
    typedef enum {
        state_boot,
        state_standby,
        state_calibrate,
        state_fire,
        state_cooldown,
        state_abort,
        state_error,
        state_shutdown,
        state_benchtop,
        num_states
    } StateType;

    typedef struct {
        StateType State;
        void (Main::*method)(void);
    } StateMachineType;

    StateMachineType StateMachine[num_states] = {
        { state_boot, &Main::sm_boot },
        { state_standby, &Main::sm_standby },
        { state_calibrate, &Main::sm_calibrate },
        { state_fire, &Main::sm_fire },
        { state_cooldown, &Main::sm_cooldown },
        { state_abort, &Main::sm_abort },
        { state_error, &Main::sm_error },
        { state_shutdown, &Main::sm_shutdown },
    };

    Main();

    void init();

    void run()
    {
        if (state < num_states) {
            // (*StateMachine[state].method)();
        };
    }

private:
    StateType state = state_boot;

    // Controller controller;

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