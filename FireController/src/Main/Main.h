#include <Arduino.h>

#include "../BinaryPacket/BinaryPacket.h"
#include "../Controller/Controller.h"

#ifndef KIWI_MAIN
#define KIWI_MAIN

class Main {

    typedef enum {
        state_boot,
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
        { state_boot, &Main::sm_boot },
        { state_standby_disconnected, &Main::sm_standby_disconnected },
        { state_standby_connected, &Main::sm_standby_connected },
        { state_armed, &Main::sm_armed },
        { state_running, &Main::sm_running },
        { state_error, &Main::sm_error }
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

    // typedef struct {
    //     ID id;
    //     void (Main::*method)(uint8_t id, uint8_t * buffer, size_t len);
    // } Callback;

    // Callback callbacks[CALLBACKS] = {
    //     { RUN_ARM }, &Main::_arm,
    //     { RUN_DISARM }, &Main::_disarm,

    // }

public:
    Main();

    void init();

    void loop();

private:

    StateType state = state_boot;

    BinaryPacket encoder;

    Controller controller;

    void sm_boot();

    void sm_standby_disconnected();

    void sm_standby_connected();

    void sm_armed();

    void sm_running();

    void sm_error();

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

    void _get_configuration();

    void _calibrate_propellant();

    void _calibrate_thrust();
};

#endif