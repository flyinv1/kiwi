#include "Main.h"

#include <Arduino.h>

#include "../Controller/Controller.h"

Main::Main()
{
    encoder = BinaryPacket();
    controller = Controller();
}

void Main::init()
{
    Serial.begin(115200);
    encoder.setStream(&Serial);
}

void Main::loop()
{
    if (state < num_states) {
        (this->*StateMachine[state].method)();
    }
}

void Main::sm_boot()
{
}

void Main::sm_standby_disconnected()
{
}

void Main::sm_standby_connected()
{
}

void Main::sm_armed()
{
}

void Main::sm_running()
{
}

void Main::sm_error()
{
}

void Main::read()
{
    encoder.read();
    if (encoder.overflow()) {
        encoder.flush();
    } else if (encoder.packetAvailable()) {
        uint8_t* buffer = nullptr;
        size_t len = encoder.packet(buffer);
        uint8_t id = buffer[0];
        _on(id, buffer + 1, len - 1);
    }
}

void Main::_on(uint8_t id, uint8_t* buffer, size_t len)
{
    switch (id) {
    case RUN_ARM: {
        _arm();
    } break;
    case RUN_DISARM: {
        _disarm();
    } break;
    case RUN_START: {
        _start();
    } break;
    case RUN_STOP: {
        _stop();
    } break;
    case SET_CONTROLMODE: {
        if (!controller.setControlModeFrom(buffer, len) == Controller::control_mode_open) {
            // Return error
        }
    } break;
    case SET_ENGINEMODE: {
        if (controller.setEngineModeFrom(buffer, len) == Controller::engine_mode_error) {
            // Return error
        }
    } break;
    case SET_RUNDURATION: {
        _set_run_duration(encoder.readUInt32(buffer, len));
    } break;
    case SET_IGNITERDURATION: {
        _set_igniter_duration(encoder.readUInt32(buffer, len));
    } break;
    case SET_IGNITERPREBURN: {
        _set_igniter_preburn(encoder.readUInt32(buffer, len));
    } break;
    case SET_TARGETS: {
        _set_targets(buffer, len);
    } break;
    case GET_CONN_STATUS: {
        _get_conn_status();
    } break;
    case GET_CONFIGURATION: {
        _get_configuration();
    } break;
    case RUN_CALIBRATE_LOAD: {

    } break;
    case RUN_CALIBRATE_PROPELLANT: {

    } break;
    default: {

    } break;
    }
}

void Main::_arm()
{
}

void Main::_disarm()
{
}

void Main::_start()
{
}

void Main::_stop()
{
}

void Main::_set_run_duration(uint32_t duration)
{
}

void Main::_set_igniter_duration(uint32_t duration)
{
}

void Main::_set_igniter_preburn(uint32_t duration)
{
}

void Main::_set_targets(int32_t* buffer, size_t len)
{
}

void Main::_get_conn_status()
{
}

void Main::_get_configuration()
{
}

void Main::_run_calibrate_load()
{
}

void Main::_run_calibrate_propellant()
{
}