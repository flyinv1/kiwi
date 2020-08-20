#include "Gateway.h"

Gateway::Gateway()
{
}

void Gateway::init(Stream* stream)
{
    setStream(stream);
    packet.setPacketHandler([&](const uint8_t* buffer, size_t len) {
        this->onPacket(buffer, size);
    })
}

void Gateway::setStream(Stream* stream)
{
    packet.setStream(stream);
}

void Gateway::registerMethod(uint8_t id, void (*onMessage)(const uint8_t* buffer, size_t len))
{
    Gateway::Callback callback = {
        id,
        onMessage,
    };
    api[id] = callback;
}

void Gateway::unregisterMethod(uint8_t)
{
}

void Gateway::update()
{
    pserial.update();
}

void Gateway::onPacket(const uint8_t* buffer, size_t size)
{
}

uint8_t Gateway::crc8(const uint8_t* buffer, size_t len)
{
}
