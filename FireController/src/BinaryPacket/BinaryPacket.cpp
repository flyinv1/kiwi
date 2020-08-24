#include "BinaryPacket.h"
#include <Arduino.h>

// Constructor
BinaryPacket::BinaryPacket()
{
    _stream = nullptr;
}

BinaryPacket::~BinaryPacket() { }

void BinaryPacket::read()
{
    while (_stream->available() > 0) {

        uint8_t data = _stream->read();

        if (data == 0) {
            // packet terminator, decode the read buffer;
            uint8_t _decodeBuffer[_readBufferIndex];
            size_t _lenDecoded = unstuff(_readBuffer, _readBufferIndex, _decodeBuffer);

            // checksum
            uint8_t _messageCrc = _decodeBuffer[0];
            uint8_t _crc = crc8(_decodeBuffer + 1, _lenDecoded - 1);

            if (_messageCrc != _crc) {
                // checksum fails, discard
                flush();
                return;
            }

            _packetAvailable = true;

        } else {
            // packet is open, add to read buffer if not overflowing
            if (_readBufferIndex < 255) {
                _readBuffer[_readBufferIndex++] = data;
            } else {
                _bufferOverflow = true;
            }
        }
    }
}

void BinaryPacket::send()
{

    _writeBuffer[0] = crc8(_writeBuffer + 1, _writeBufferIndex - 1);
    uint8_t _encodeBuffer[_writeBufferIndex];
    size_t _lenEncoded = stuff(_writeBuffer, _writeBufferIndex, _encodeBuffer);

    for (uint8_t i = 0; i < _lenEncoded; i++) {
        _stream->write(_encodeBuffer[i]);
    }

    flushWriteBuffer();
}

size_t BinaryPacket::packet(uint8_t* buffer)
{
    buffer = _readBuffer;
    return _readBufferIndex;
}

void BinaryPacket::flush()
{
    _readBufferIndex = 0;
}

void BinaryPacket::flushWriteBuffer()
{
    _writeBufferIndex = 0;
    _writeBuffer[0] = 0;
}

void BinaryPacket::write(uint8_t* buffer, size_t length)
{
    if (_writeBufferIndex + length > 255) {
        return false;
    }
    for (uint8_t i = 0; i < length; i++) {
        _writeBuffer[_writeBufferIndex++] = buffer[i];
    }
    return true;
}

void BinaryPacket::setStream(Stream* stream)
{
    _stream = stream;
}

size_t BinaryPacket::stuff(uint8_t* inputBuffer, size_t length, uint8_t* outputBuffer)
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

size_t BinaryPacket::unstuff(uint8_t* inputBuffer, size_t length, uint8_t* outputBuffer)
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

uint8_t BinaryPacket::crc8(uint8_t* buffer, size_t length)
{
    // 8 bit CRC (CRC8-ITU w/ 0x07)
    uint8_t crc = 0;
    uint8_t i;

    for (i = 0; i < length; i++) {
        crc = _crcLookup[buffer[i] ^ crc];
    }

    return crc;
}
