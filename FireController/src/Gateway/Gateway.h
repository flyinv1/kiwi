#include <Arduino.h>
#include <PacketSerial.h>

#ifndef KIWI_GATEWAY
#define KIWI_GATEWAY

// allow up to N simultaneously registered methods
#define NUM_METHODS 256

class Gateway {

    enum INDICES {
        ID_INDEX = 0,
        LEN_INDEX = 1,
        CRC_INDEX = 2
    };

    typedef struct
    {
        uint16_t id;
        void (*method)(uint8_t* buffer, size_t len);
    } Callback;

public:
    PacketSerial_<COBS> packet;

    Gateway();

    void init(Stream* stream);

    void setStream(Stream* stream);

    void registerMethod(uint8_t id, void (*onMessage)(uint8_t* buffer, size_t len));

    void unregisterMethod(uint8_t id);

    void update();

private:
    Gateway::Callback api[NUM_METHODS];

    void onPacket(const uint8_t* buffer, size_t size);

    uint8_t crc8(uint8_t* buffer, size_t len);
};

#endif