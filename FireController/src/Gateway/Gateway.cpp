#include "Gateway.h"

Gateway::Gateway()
{
}

void Gateway::init(Stream* stream)
{
    setStream(stream);

    packet.setPacketHandler([&](const uint8_t* buffer, size_t len) {
        Serial.write(0);
    });
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

void Gateway::unregisterMethod(uint8_t id)
{
}

void Gateway::update()
{
    packet.update();
    if (packet.overflow()) {
        return;
    }
}

void Gateway::onPacket(const uint8_t* buffer, size_t len)
{
    // Validate the packet checksum
    uint8_t crc = crc8(buffer, len);
    uint8_t packet_crc = buffer[CRC_INDEX];

    // if invalid, discard the packet and return
    if (packet_crc != crc) {
        return;
    }

    // get the packet API id and call the registered function
    uint8_t packet_id = buffer[ID_INDEX];
    api[packet_id].method(buffer + flag_indices, len - flag_indices);
}

uint8_t Gateway::crc8(const uint8_t* buffer, size_t len)
{
    return 0;
}
