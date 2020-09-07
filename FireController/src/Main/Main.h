#include <Arduino.h>

#include "../BinaryPacket/BinaryPacket.h"
#include "../Controller/Controller.h"
#include "../Target/Target.h"

#ifndef KIWI_MAIN
#define KIWI_MAIN

class Main {

    typedef enum {
        state_disconnected,
        state_standby,
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
        { state_disconnected, &Main::sm_disconnected },
        { state_standby, &Main::sm_standby },
        { state_armed, &Main::sm_armed },
        { state_running, &Main::sm_running },
        { state_error, &Main::sm_error }
    };

    enum TRANSITIONS {
        transition_disconnected_standby,
        transition_standby_to_armed,
        transition_armed_to_running,
        transition_running_to_armed,
        transition_armed_to_standby,
        num_transitions
    };

    typedef struct {
        StateType prev;
        StateType next;
        void (Main::*method)(void);
    } StateMachineTransition;

    StateMachineTransition TransitionTable[num_transitions] = {
        { state_disconnected, state_standby, &Main::sm_disconnected_to_standby },
        { state_standby, state_armed, &Main::sm_standby_to_armed },
        { state_armed, state_running, &Main::sm_armed_to_running },
        { state_running, state_armed, &Main::sm_running_to_armed },
        { state_armed, state_standby, &Main::sm_armed_to_standby },
    };

    enum {
        SYNC = 0,
        RUN_ARM = 1,
        RUN_DISARM = 2,
        RUN_START = 3,
        RUN_STOP = 4,
        SET_CONTROLMODE = 5,
        SET_ENGINEMODE = 6,
        SET_RUNDURATION = 7,
        SET_IGNITERPREBURN = 8,
        SET_IGNITERDURATION = 9,
        SET_TARGETS = 10,
        GET_CONN_STATUS = 11,
        GET_CONFIGURATION = 12,
        RUN_CALIBRATE_PROPELLANT = 13,
        RUN_CALIBRATE_LOAD = 14,
        CALLBACKS = 15
    };

public:
    Main();

    void init();

    void loop();

    void setState(StateType next);

private:
    StateType state = state_disconnected;

    BinaryPacket encoder;

    Controller controller;

    void sm_disconnected();

    void sm_standby();

    void sm_armed();

    void sm_running();

    void sm_error();

    void sm_disconnected_to_standby();

    void sm_standby_to_armed();

    void sm_armed_to_running();

    void sm_running_to_armed();

    void sm_armed_to_standby();

    void read();

    /*
        API METHODS
    */
    void _on(uint8_t id, uint8_t* buffer, size_t len);

    void _arm();

    void _disarm();

    void _start();

    void _stop();

    void _set_run_duration(uint32_t duration);

    void _set_igniter_preburn(uint32_t duration);

    void _set_igniter_duration(uint32_t duration);

    void _set_targets(uint8_t* buffer, size_t len);

    void _get_conn_status();

    void _get_configuration();

    void _calibrate_propellant();

    void _calibrate_thrust();
};

#endif