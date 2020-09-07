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
        uint8_t id = buffer[id_index];
        _on(id, buffer, len);
    }
}

void Main::_on(uint8_t id, uint8_t* buffer, size_t len)
{
    switch (id) {
    case RUN_ARM:
        _arm();
        break;
    case RUN_DISARM:
        _disarm();
        break;
    case RUN_START:
        _start();
        break;
    case RUN_STOP:
        _stop();
        break;
    case SET_CONTROLMODE:
        if (!controller.setControlModeFrom(buffer, len) == Controller::control_mode_open) {
            // Return error
        }
        break;
    case SET_ENGINEMODE:
        if (controller.setEngineModeFrom(buffer, len) == Controller::engine_mode_error) {
            // Return error
        }
        break;
    case SET_RUNDURATION:
        uint32_t duration = encoder.readInt(buffer, len);
        _set_run_duration(duration);
        break;
    default:
        break;
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