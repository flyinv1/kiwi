#include <Arduino.h>

#include "../Gateway/Gateway.h"
// #include "../Controller/Controller.h"

#ifndef KIWI_MAIN
#define KIWI_MAIN

class Main {

public:
    typedef enum {
        state_init,
        state_standby_disconnected,
        state_standby_connected,
        state_armed,
        state_running,
        state_error,
        num_states
    } StateType;

    typedef struct {
        StateType State;
        void (Main::*method)(void);
    } StateMachineType;

    StateMachineType StateMachine[num_states] = {
        { state_standby_init, &Main::sm_init },
        { state_standby_disconnected, &Main::sm_standby_disconnected },
        { state_standby_connected, &Main::sm_standby_connected },
        { state_armed, &Main::sm_armed },
        { state_running, &Main::sm_running },
        { state_error, &Main::sm_error }
    };

    Gateway gateway();

    Main();

    void init();

    void loop()
    {
        if (state < num_states) {
            (*StateMachine[state].method)();
        };
    }

private:
    StateType state = state_boot;

    Controller controller();

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