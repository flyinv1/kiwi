#include <Arduino.h>

#include "../BinaryPacket/BinaryPacket.h"
#include "../Controller/Controller.h"
#include "../LED.h"
#include "../StateClock/StateClock.h"
#include "../Target/Target.h"

#ifndef KIWI_MANAGER
#define KIWI_MANAGER

#define DISCONNECT_INTERVAL_MS  500
#define PING_INTERVAL_MS        1000
#define DAQ_INTERVAL_MS_STANDBY 100
#define DAQ_INTERVAL_MS         5

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
        transition_disconnected_to_standby,
        transition_standby_to_armed,
        transition_armed_to_running,
        transition_armed_to_standby,
        transition_running_to_standby,
        transition_standby_to_disconnected,
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
        { state_armed, state_standby, &Manager::smt_armed_to_standby },
        { state_running, state_standby, &Manager::smt_running_to_standby },
        { state_standby, state_disconnected, &Manager::smt_standby_to_disconnected }
    };

    typedef enum {
        SYNC = 0,
        ARM = 1,
        DISARM = 2,
        START = 3,
        STOP = 4,
        SET_CONTROLMODE = 5,
        SET_ENGINEMODE = 6,
        SET_RUNDURATION = 7,
        SET_IGNITERPREBURN = 8,
        SET_IGNITERDURATION = 9,
        SET_IGNITERVOLTAGE = 10,
        SET_TARGETS = 11,
        CALIBRATE_THRUST = 12,
        CLOSE = 13,
        THROTTLE_POSITION = 14,
        THROTTLE_ENCODER = 15,
        num_callbacks = 16,
        STATE = 17,
        DATA = 18,
        LOG = 19
    } TopicType;

    typedef void (Manager::*TopicCallback)(uint8_t id, uint8_t* buffer, size_t len);

    typedef struct {
        TopicType topic;
        TopicCallback callback;
    } TopicCallbackType;

    TopicCallbackType TopicTable[num_callbacks] = {
        { SYNC, &Manager::_on_sync },
        { ARM, &Manager::_on_arm },
        { DISARM, &Manager::_on_disarm },
        { START, &Manager::_on_start },
        { STOP, &Manager::_on_stop },
        { SET_CONTROLMODE, &Manager::_on_set_controlmode },
        { SET_ENGINEMODE, &Manager::_on_set_enginemode },
        { SET_RUNDURATION, &Manager::_on_set_runduration },
        { SET_IGNITERVOLTAGE, &Manager::_on_set_ignitervoltage },
        { SET_IGNITERPREBURN, &Manager::_on_set_igniterpreburn },
        { SET_IGNITERDURATION, &Manager::_on_set_igniterduration },
        { SET_TARGETS, &Manager::_on_set_targets },
        { CALIBRATE_THRUST, &Manager::_on_calibrate_thrust },
        { THROTTLE_POSITION, &Manager::_on_set_throttle_position },
        { THROTTLE_ENCODER, &Manager::_on_set_encoder_value },
        { CLOSE, &Manager::_on_close }
    };

public:
    Manager();

    ~Manager();

    void init();

    void loop();

    bool setState(StateType next);

private:
    StateType state = state_disconnected;

    BinaryPacket encoder;

    Controller controller;

    StateClock missionClock;

    LED led;

    uint32_t comm_timer = 0;
    uint32_t data_timer = 0;
    uint8_t engine_data_buffer[Controller::engine_data_size * 4] = {};

    void sm_disconnected();

    void sm_standby();

    void sm_armed();

    void sm_running();

    void sm_error();

    bool smt_disconnected_to_standby();

    bool smt_standby_to_armed();

    bool smt_armed_to_running();

    bool smt_running_to_standby();

    bool smt_armed_to_standby();

    bool smt_standby_to_disconnected();

    void read();

    void sendById(uint8_t id, uint8_t* buffer, size_t length);

    bool _configurable();

    void _on_sync(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_arm(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_disarm(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_start(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_stop(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_calibrate_thrust(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_controlmode(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_enginemode(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_runduration(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_igniterpreburn(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_igniterduration(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_ignitervoltage(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_targets(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_get_configuration(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_throttle_position(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_set_encoder_value(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_state(uint8_t topic, uint8_t* buffer, size_t len);
    void _on_close(uint8_t topic, uint8_t* buffer, size_t len);
};

#endif