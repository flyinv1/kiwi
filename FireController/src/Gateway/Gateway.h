#include <Arduino.h>
#include <PacketSerial.h>

#ifndef KIWI_GATEWAY
#define KIWI_GATEWAY

// allow up to N simultaneously registered methods
#define NUM_METHODS 256

class Gateway {

public:
    typedef struct {
        uint16_t id;
        void (*method)(const uint8_t* buffer, size_t len);
    } Callback;

    Gateway();

    void init(Stream* stream);

    void setStream(Stream* stream);

    void registerMethod(uint8_t id, void (*onMessage)(const uint8_t* buffer, size_t len));

    void unregisterMethod(uint8_t id);

    void update();

private:
    enum INDICES {
        ID_INDEX,
        LEN_INDEX,
        CRC_INDEX,
        flag_indices,
    };

    Callback api[NUM_METHODS];

    PacketSerial_<COBS> packet;

    void onPacket(const uint8_t* buffer, size_t len);

    uint8_t crc8(const uint8_t* buffer, size_t len);
};

#endif