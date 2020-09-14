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
    while (_stream->available()) {

        uint8_t data = _stream->read();

        if (data == 0x00) {

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

            newPacketLength = _lenDecoded - 1;
            for (int i = 0; i < newPacketLength; i++) {
                newPacket[i] = _decodeBuffer[i + 1];
            }

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
    _writeBuffer[0] = crc8(_writeBuffer, _writeBufferIndex);
    uint8_t _encodeBuffer[_writeBufferIndex];
    size_t _lenEncoded = stuff(_writeBuffer, _writeBufferIndex, _encodeBuffer);

    for (uint8_t i = 0; i < _lenEncoded; i++) {
        _stream->write(_encodeBuffer[i]);
    }

    _stream->write(delimiter);

    flushWriteBuffer();
}

size_t BinaryPacket::packet(uint8_t* buffer)
{
    // Offset the buffer by one byte to remove crc
    for (int i = 0; i < newPacketLength; i++) {
        buffer[i] = newPacket[i];
    }
    size_t len = newPacketLength;
    newPacketLength = 0;
    _packetAvailable = false;
    flush();
    return len;
}

bool BinaryPacket::packetAvailable()
{
    return _packetAvailable;
}

bool BinaryPacket::overflow()
{
    return _bufferOverflow;
}

void BinaryPacket::flush()
{
    _readBufferIndex = 0;
}

void BinaryPacket::flushWriteBuffer()
{
    // reset the write buffer CRC and place the index at 1
    _writeBuffer[0] = 0;
    _writeBufferIndex = 1;
}

bool BinaryPacket::write(uint8_t* buffer, size_t length)
{
    if (_writeBufferIndex + length > 255) {
        return false;
    }

    // Writing simply places data into the write buffer starting at the existing write buffer index
    // When flushing, the write buffer index is reset at 1 to ensure room for the crc byte
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
    uint8_t code = 0;
    uint8_t i = 0;

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
            outputBuffer[write_index++] = '\0';
        }
    }

    return write_index;
}

uint8_t BinaryPacket::crc8(uint8_t* buffer, size_t length)
{
    // 8 bit CRC (CRC8 w/ 0x07 poly)
    uint8_t crc = 0;

    for (int i = 0; i < length; i++) {
        crc = _crcLookup[buffer[i] ^ crc];
    }

    return crc;
}

size_t readFloatBuffer(uint8_t* buffer, size_t len, float* output)
{
    size_t outputLength = ((len / 4) % 4 == 0) ? (len / 4) : len - len % 4;
    for (int i = 0; i < outputLength; i++) {
        union {
            byte b[4];
            float v;
        } f;
        for (int j = 0; j < 4; j++) {
            f.b[i + j] = buffer[i + j];
        }
        output[i] = f.v;
    }
    return outputLength;
}

float readFloat(uint8_t* buffer, size_t len, bool bendian = false)
{
    union {
        byte b[4];
        float v;
    } f;
    for (int i = 0; i < 4; i++) {
        f.b[i] = buffer[i];
    }
    return f.v;
}

size_t readInt32Buffer(uint8_t* buffer, size_t len, int32_t* output)
{
    size_t outputLength = ((len / 4) % 4 == 0) ? (len / 4) : len - len % 4;
    for (int i = 0; i < outputLength; i++) {
        union {
            byte b[4];
            int32_t v;
        } out;
        for (int j = 0; j < 4; j++) {
            out.b[i + j] = buffer[i + j];
        }
        output[i] = out.v;
    }
    return outputLength;
}

int32_t readInt32(uint8_t* buffer, size_t len, bool bendian = false)
{
    if (len == 4) {
        union {
            byte b[4];
            int32_t v;
        } out;
        for (int i = 0; i < 4; i++) {
            out.b[i] = buffer[i];
        }
        return out.v;
    } else {
        return 0;
    }
}

uint32_t readUInt32(uint8_t* buffer, size_t len, bool bendian = false)
{
    if (len == 4) {
        union {
            byte b[4];
            uint32_t v;
        } out;
        for (int i = 0; i < 4; i++) {
            out.b[i] = buffer[i];
        }
        return out.v;
    } else {
        return 0;
    }
}
