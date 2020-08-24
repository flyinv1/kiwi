#include <Arduino.h>

#include "../BinaryPacket/BinaryPacket.h"

#ifndef KIWI_GATEWAY
#define KIWI_GATEWAY

// allow up to N simultaneously registered methods
#define NUM_METHODS 256

class Gateway {

public:
    typedef struct {
        uint8_t id;
        void (*method)(const uint8_t* buffer, size_t len);
    } Callback;

    Gateway();

    void init(Stream* stream);

    void setStream(Stream* stream);

    void registerMethod(uint8_t id, void (*onMessage)(const uint8_t* buffer, size_t len));

    void update();

private:
    enum INDICES {
        ID_INDEX = 0,
        CRC_INDEX = 1,
        LEN_INDEX = 2,
        flag_indices = 3,
    };

    Callback api[NUM_METHODS];

    BinaryPacket encoder;
};

#endif