#include <Arduino.h>

#include "../BinaryPacket/BinaryPacket.h"
#include "../Controller/Controller.h"
#include "../StateClock/StateClock.h"
#include "../Target/Target.h"

#ifndef KIWI_MANAGER
#define KIWI_MANAGER

#define DISCONNECT_INTERVAL_MS 2000
#define DAQ_INTERVAL_MS        1

class Manager {

    typedef bool (Manager::*TransitionMethod)(void);
    typedef void (Manager::*StateMethod)(void);

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
        { state_disconnected, &Manager::sm_disconnected },
        { state_standby, &Manager::sm_standby },
        { state_armed, &Manager::sm_armed },
        { state_running, &Manager::sm_running },
        { state_error, &Manager::sm_error }
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
        { state_disconnected, state_standby, &Manager::smt_disconnected_to_standby },
        { state_standby, state_armed, &Manager::smt_standby_to_armed },
        { state_armed, state_running, &Manager::smt_armed_to_running },
        { state_running, state_armed, &Manager::smt_running_to_armed },
        { state_armed, state_standby, &Manager::smt_armed_to_standby },
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
        RUN_CALIBRATE_LOAD = 13,
        STATE = 14,
        CALLBACKS = 15
    };

public:
    Manager();

    ~Manager();

    void init();

    void loop();

    void setState(StateType next);

private:
    bool on = false;

    long t = 0;

    StateType state = state_disconnected;

    BinaryPacket encoder;

    Controller controller;

    StateClock missionClock;

    void sm_disconnected();

    void sm_standby();

    void sm_armed();

    void sm_running();

    void sm_error();

    bool smt_disconnected_to_standby();

    bool smt_standby_to_armed();

    bool smt_armed_to_running();

    bool smt_running_to_armed();

    bool smt_armed_to_standby();

    void read();

    void sendById(uint8_t id, uint8_t* buffer, size_t length);

    bool _configurable();

    // /*
    //     API METHODS
    // */
    void _on(uint8_t id, uint8_t* buffer, size_t len);

    void _sync();

    void _arm();

    void _disarm();

    void _start();

    void _stop();

    void _set_targets(uint8_t* buffer, size_t len);

    void _getConfiguration();

    void _calibrate_propellant();

    void _calibrate_thrust();

    void _getState();
};

#endif