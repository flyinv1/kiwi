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
    case SYNC:
        _sync();
    case default:
        break;
    }
}

void Main::_sync()
{
}

void Main::_arm(uint8_t* buffer, size_t len)
{
}

void Main::_disarm(uint8_t* buffer, size_t len)
{
}

void Main::_start(uint8_t* buffer, size_t len)
{
}