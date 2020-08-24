#include "Gateway.h"

Gateway::Gateway()
{
}

void Gateway::init(Stream* stream)
{
    setStream(stream);
}

void Gateway::setStream(Stream* stream)
{
    encoder.setStream(stream);
}

void Gateway::registerMethod(uint8_t id, void (*onMessage)(const uint8_t* buffer, size_t len))
{
    Gateway::Callback callback = {
        id,
        onMessage,
    };
    api[id] = callback;
}

void Gateway::update()
{
    encoder.read();

    if (encoder.overflow()) {
        encoder.flush();
    }

    if (encoder.packetAvailable()) {

        uint8_t* buffer = nullptr;
        size_t len = encoder.packet(buffer);

        uint8_t packet_id = buffer[ID_INDEX];
        api[packet_id].method(buffer + flag_indices, len - flag_indices);

        encoder.flush();
    }
}