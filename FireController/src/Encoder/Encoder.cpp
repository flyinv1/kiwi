#include "Encoder.h"
#include <Arduino.h>

// Constructor
Encoder::Encoder()
{
    _stream = nullptr;
}

Encoder::~Encoder() { }

void Encoder::read()
{
    while (_stream->available() > 0) {
        uint8_t data = _stream->read();
        if (data == 0) {
            // packet terminator, decode the read buffer;
            uint8_t _decodeBuffer[_readBufferIndex];
            size_t _lenDecoded = unstuff(_readBuffer, _readBufferIndex, _decodeBuffer);

            // checksum
            uint8_t _messageCrc = _decodeBuffer[CRC_Index];
            uint8_t _crc = crc8(_decodeBuffer + 1, _lenDecoded - 1);

            if (_messageCrc != _crc) {
                // checksum fails, discard
                return;
            }

        } else {
            // packet is open, add to read buffer if not overflowing
            if (_readBufferIndex + 1 < 256) {
                _readBuffer[_readBufferIndex++] = data;
            } else {
                // Buffer overflow
            }
        }
    }
}

void Encoder::write()
{

    _writeBuffer[CRC_Index] = crc8(_writeBuffer + 1, _writeBufferIndex - 1);
    uint8_t _encodeBuffer[_writeBufferIndex];
    size_t _lenEncoded = stuff(_writeBuffer, _writeBufferIndex, _encodeBuffer);

    for (uint8_t i = 0; i < _lenEncoded; i++) {
        _stream->write(_encodeBuffer[i]);
    }

    flushWriteBuffer();
}

void Encoder::flushWriteBuffer()
{
    _writeBufferIndex = DATA_START_Index;
    _writeBuffer[CRC_Index] = 0;
}

bool Encoder::push(uint8_t* buffer, size_t length)
{
    if (_writeBufferIndex + length > 255) {
        return false;
    }
    for (uint8_t i = 0; i < length; i++) {
        _writeBuffer[_writeBufferIndex++] = buffer[i];
    }
    return true;
}

template <typename T, const int byteLength>
bool Encoder::push(T* buffer, size_t length)
{
    uint8_t expandedLength = byteLength * length;
    uint8_t data[expandedLength];
    data = (uint8_t*)(buffer);
    push(data, expandedLength);
}

void Encoder::onMessage(onMessageCallback callback)
{
    _callback = callback;
}

void Encoder::setStream(Stream* stream)
{
    _stream = stream;
}

size_t Encoder::stuff(uint8_t* inputBuffer, size_t length, uint8_t* outputBuffer)
{

    // This is an implementation of COBS by jacquesf:
    // https://github.com/jacquesf/COBS-Consistent-Overhead-Byte-Stuffing

    size_t read_index = 0;
    size_t write_index = 1;
    size_t code_index = 0;

    uint8_t code = 1;

    while (read_index < length) {
        if (inputBuffer[read_index] == 0) {
            outputBuffer[code_index] = code;
            code = 1;
            code_index = write_index++;
            read_index++;
        } else {
            outputBuffer[write_index++] = inputBuffer[read_index++];
            code++;
            if (code == 0xFF) {
                outputBuffer[code_index] = code;
                code = 1;
                code_index = write_index;
            }
        }
    }

    outputBuffer[code_index] = code;

    return write_index;
}

size_t Encoder::unstuff(uint8_t* inputBuffer, size_t length, uint8_t* outputBuffer)
{
    size_t read_index = 0;
    size_t write_index = 0;
    uint8_t code;
    uint8_t i;

    while (read_index < length) {
        code = inputBuffer[read_index];

        if (read_index + code > length && code != 1) {
            return 0;
        }

        read_index++;

        for (i = 1; i < code; i++) {
            outputBuffer[write_index++] = inputBuffer[read_index++];
        }
        if (code != 0xff && read_index != length) {
            outputBuffer[write_index] = '\0';
        }
    }
}

uint8_t Encoder::crc8(uint8_t* buffer, size_t length)
{
    // 8 bit CRC (CRC8-ITU w/ 0x07)
    uint8_t crc = 0;
    uint8_t i;

    for (i = 0; i < length; i++) {
        crc = _crcLookup[buffer[i] ^ crc];
    }

    return crc;
}
