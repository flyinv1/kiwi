import paho.mqtt.client as mqtt
from serial.tools import list_ports
from serial import Serial
from . import encoder
from . import interface

class Manager:
    
    def __init__(self):
        self.serialport = None
        self.controller_connected = False
        self.serial_buffer = bytearray(256)
        self.serial_read_index = 0


    def main(self):
        # first update the MQTT client
        self.client.loop()
        self.read_serial()


    def connect_serial(self, path):
        ports = list_ports.comports()
        for port in ports:
            if port.device == path:
                self.serialport = Serial(path, timeout=0.01, write_timeout=0.01)
                return True
        return False


    def open_serial(self):
        if not self.serialport.isOpen():
            self.serialport.open()
            self.serialport.flushInput()
        else:
            print('Connected to serial device at: ' + self.serialport.name)


    def close_serial(self):
        if self.serialport.isOpen():
            print('Closing serialport')
            self.write_serial_packet(interface.Keys.CLOSE.value, [])
            self.serialport.close()


    def read_serial(self):
        if self.serialport:
            while self.serialport.inWaiting() > 0:
                _bytes = self.serialport.read(1)
                if len(_bytes) > 0:
                    _byte = _bytes[0]
                    self.serial_buffer[self.serial_read_index] = _byte
                    if _byte == 0x00:
                        _packet = encoder.cobs_decode(self.serial_buffer[0:self.serial_read_index])
                        if encoder.crc(_packet[1:]) == _packet[0]:
                            _id = _packet[1]
                            _msg = _packet[2:]
                            self.on_packet(_id, _msg)   
                        self.serialport.flushInput()
                        self.serial_read_index = 0
                    else:
                        if self.serial_read_index + 1 > 255:
                            self.serialport.flushInput()
                            self.serial_read_index = 0
                        else:
                            self.serial_read_index += 1


    # Write bytes to serial output
    def write_serial_packet(self, id, payload):
        _buffer = bytearray(2 + len(payload))
        _buffer[1] = id
        _buffer[2:] = payload
        _buffer[0] = encoder.crc(_buffer[1:])
        _out_buffer = encoder.cobs_encode(_buffer)
        self.serialport.write(_out_buffer)
        self.serialport.write(bytearray([0x00]))


    # ON_PACKET SERIAL EVENT
    def on_packet(self, id_, payload):
        _payload, _type, _id = interface.on_id(id_, payload)
        print(id_)
        print(payload)
        if _id == interface.Keys.SYNC.value:
            if not self.controller_connected:
                print('Connecting packet serial communication...')
                self.write_serial_packet(interface.Keys.SYNC.value, [])
        elif _id == interface.Keys.STATE.value:
            print('Controller entered state: ' + str(int.from_bytes(_payload, byteorder='little')))
            # Forward to MQTT!
        else:
            # Forward to MQTT!
            None

    # Create a new MQTT client and connect to the main broker
    def connect_client(self, host, port):
        # Create an MQTT client
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

        # Connect to the MQTT broker
        self.client.connect(host, port)


    # ON_CONNECT MQTT EVENT
    def on_connect(self, client, userdata, flags, rc):
        print("Connected to broker with result code {0}".format(str(rc)))
        for _dict in interface.keymap:
            if _dict["topic"] is not None:
                client.subscribe(_dict["topic"])
                print("Subscribed: " + _dict["topic"])


    # ON_MESSAGE MQTT EVENT
    def on_message(self, client, userdata, message):
        _result, _result_type, _id = interface.on_topic(message.topic, message.payload)

        # architecture only supports a couple numeric types + lists - need something more robust for later
        if _result_type == int:
            # cast result to byte array and write out
            _buffer = bytearray(_result.to_bytes(4, byteorder='little', signed=False))
            self.write_serial_packet(_id, _buffer)
        if _result_type == list:
            # assume list of 32 bit integers pairs
            _buffer = bytearray(len(_result) * 4)
            for _int, i in enumerate(_result):
                _buffer[i:i+4] = bytearray(_int.to_bytes(4, byteorder='little', signed=False))
            self.write_serial_packet(_id, _buffer)
        elif _result_type == None:
            print(_result, _id)
            self.write_serial_packet(_id, [])

    # ON_DISCONNECT MQTT EVENT
    def on_disconnect(self, client, userdata):
        print("")


    

