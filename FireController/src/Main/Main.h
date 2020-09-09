#include <Arduino.h>

#include "../BinaryPacket/BinaryPacket.h"
#include "../Controller/Controller.h"
#include "../StateClock/StateClock.h"
#include "../Target/Target.h"

#ifndef KIWI_MAIN
#define KIWI_MAIN

#define DISCONNECT_INTERVAL_MS 1000
#define DAQ_INTERVAL_MS        1

class Main {

    typedef void (Main::*TransitionMethod)(void);
    typedef void (Main::*StateMethod)(void);

    typedef enum {
        state_disconnected = 0,
        state_standby = 1,
        state_armed = 2,
        state_running = 3,
        state_error = 4,
        num_states
    } StateType;

    typedef struct {
        StateType state;
        StateMethod method;
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
        TransitionMethod method;
    } StateMachineTransition;

    StateMachineTransition TransitionTable[num_transitions] = {
        { state_disconnected, state_standby, &Main::smt_disconnected_to_standby },
        { state_standby, state_armed, &Main::smt_standby_to_armed },
        { state_armed, state_running, &Main::smt_armed_to_running },
        { state_running, state_armed, &Main::smt_running_to_armed },
        { state_armed, state_standby, &Main::smt_armed_to_standby },
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
        GET_CONFIGURATION = 11,
        RUN_CALIBRATE_PROPELLANT = 12,
        RUN_CALIBRATE_LOAD = 13,
        GET_STATE = 14,
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

    StateClock missionClock;

    void sm_disconnected();

    void sm_standby();

    void sm_armed();

    void sm_running();

    void sm_error();

    void smt_disconnected_to_standby();

    void smt_standby_to_armed();

    void smt_armed_to_running();

    void smt_running_to_armed();

    void smt_armed_to_standby();

    void read();

    bool _configurable();

    /*
        API METHODS
    */
    void _on(uint8_t id, uint8_t* buffer, size_t len);

    void _sync();

    void _arm();

    void _disarm();

    void _start();

    void _stop();

    void _set_targets(uint8_t* buffer, size_t len);

    void _get_configuration();

    void _calibrate_propellant();

    void _calibrate_thrust();
};

#endif