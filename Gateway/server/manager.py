import paho.mqtt.client as mqtt
from serial.tools import list_ports
from serial import Serial
from . import encoder

class Manager:
    
    def __init__(self):
        self.host = '192.168.1.10'
        self.port = 1883
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
                self.serialport = Serial(path)
                return True
        return False

    def open_serial(self):
        if not self.serialport.isOpen():
            self.serialport.open()
        else:
            print('Connected to serial device at: ' + self.serialport.name)

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

    def on_packet(self, id, payload):
        None

    def connect_client(self, host, port):
        # Create an MQTT client
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

        # Connect to the MQTT broker asynchronously
        self.client.connect_async(host, port)
        self.host = host
        self.port = port
            
    def on_connect(self, client, userdata, flags, rc):
        print("")

    def on_message(self, client, userdata, message):
        print("")

    def on_disconnect(self, client, userdata):
        print("")


    

