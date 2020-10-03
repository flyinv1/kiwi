import paho.mqtt.client as mqtt
from serial.tools import list_ports
from serial import Serial
from . import encoder
from . import interface
import json
from time import monotonic, strftime
import struct
import os
import csv

dataStruct = [
    "chamberPressure",
    "upstreamPressure",
    "downstreamPressure",
    "thrust",
    "propellantMass",
    "massFlow",
    "throttlePosition",
    "igniterVoltage",
    "missionTime",
    "managerStateTime",
    "delta",
]


class Manager:
    
    def __init__(self, directory_path, log_path):
        self.serialport = None
        self.controller_connected = False
        self.serial_buffer = bytearray(256)
        self.serial_read_index = 0
        self.timeout_interval = 2
        self.timeout = 0
        self.dataBufferElapsed = 0
        self.dataInterval = 0.1
        self.controller_state = 0
        self.timeoutMax = 0.25
        self.timeoutElapsed = 0
        self.ping = 0
        self.ping_interval = 0.5
        self.runData = []
        self.directory_path = directory_path
        self.logfile = None

        # Check that the directory exists at path or create it
        if not os.path.exists(directory_path):
            os.mkdir(directory_path)

        if not os.path.exists(log_path):
            os.mkdir(log_path)
            # Use log subdirectory for this session
            _t = strftime("%H_%D_%M_%Y")
            log_filename = "log_" + _t
            self.logfile = open(log_filename, 'w')
            self.logfile('Initializing logs: ' + str(monotonic()))


    def main(self):
        # Update the MQTT client
        self.client.loop(0.01)
        self.read_serial()
        _time = monotonic()

        if _time - self.ping > self.ping_interval:
            self.write_serial_packet(interface.Keys.PING.value, [])
            self.ping = _time

        if _time - self.timeout > self.timeout_interval:
            self.client.publish('get/connected', json.dumps(False))
            self.controller_connected = False
            self.timeout = _time

    # Serial packet callback
    def on_packet(self, id_, payload):
        _payload, _id, _type, _emitter = interface.on_serial(id_, payload)

        _time = monotonic()

        self.timeout = _time

        # Log all received packets
        self.logfile.write('packet: {id_}\n{payload}\n')

        # Wait a minimum interval as to not overload broker with sync msg
        if _time - self.timeoutElapsed > self.timeoutMax:
            # After the timeoutMax interval, tell the GUI that the client is connected
            # This is a intermittent signal in the absence of other communication to ensure the GUI and Gateway states remain synced.
            self.client.publish('get/connected', json.dumps(True))
            self.timeoutElapsed = _time


        if _id != interface.Keys.DATA.value:
            print('\npacket id:', _id)
            print('\tpayload:', _payload)
            print('\ttype:', _type)

        # Switch on the packet id

        # SYNC packet
        #   - Update the timout value
        #   - Publish updated controller status
        if _id == interface.Keys.SYNC.value:
            if self.controller_connected == False:
                print('Connecting packet serial communication...')
                self.write_serial_packet(interface.Keys.SYNC.value, [])
                self.controller_connected = True

        # STATE packet
        #   - Publish updated controller state
        elif _id == interface.Keys.STATE.value:
            _newstate = int.from_bytes(_payload, byteorder='little')
            self.client.publish(_emitter, json.dumps(_newstate))
            if _newstate == 1 and self.controller_state == 3:
                self.write_data_out()
                self.runData = []
            self.controller_state = _newstate


        # DATA packet
        #   - Log data to csv
        #   - If interval elapsed, publish buffer
        elif _id == interface.Keys.DATA.value:
            _data = []
            for i in range(int(len(_payload) / 4)):
                _val = struct.unpack('f', bytearray(_payload[4*i:4*i+4]))
                _data.append(_val[0])
            
            # print(_data)

            if self.controller_state == 3:
                # Save data to csv ?
                self.runData.append(_data)

            if _time - self.dataBufferElapsed > self.dataInterval:
                self.client.publish(_emitter, json.dumps(_data))
                self.dataBufferElapsed = _time

        # TARGETS packet
        #   - Forward the updated targets to client
        elif _id == interface.Keys.TARGETS.value:
            _targets = []
            for i in range(int(len(_payload) / 4)):                    
                _targets.append(int.from_bytes(_payload[i:i+4], byteorder='little', signed=False))
            self.client.publish(_emitter, json.dumps(_targets))

        # SETTER callback packet
        #   - Forward updated value to client
        elif _type == int:
            _value = int.from_bytes(_payload, byteorder='little', signed=False)
            self.client.publish(_emitter, json.dumps(_value))

        # Case not covered for now
        else:
            pass


    # MQTT message callback
    def on_message(self, client, userdata, message):
        _payload, _id, _type, _consumer = interface.on_mqtt(message.topic, message.payload)

        print(_payload, _id, _type, _consumer)
        
        _time = monotonic()

        # TARGETS setter
        #   - Write out bytearray from array of uint32
        if _id == interface.Keys.TARGETS.value:
            # assume list of 32 bit integers pairs
            _buffer = bytearray(len(_payload) * 4)
            for i, _int in enumerate(_payload):
                _scaled = _int
                if i % 2 == 1:
                    _scaled = _int * 100
                br = bytearray(_scaled.to_bytes(4, byteorder='little', signed=False))
                _buffer[ i * 4:(i * 4) + 4 ] = br
            self.write_serial_packet(_id, _buffer)

        # GENERIC setter
        #   - Write out bytearray from uint32
        elif _type == int:
            # cast result to byte array and write out
            _buffer = bytearray(_payload.to_bytes(4, byteorder='little', signed=False))
            self.write_serial_packet(_id, _buffer)

        # Command
        #   - no payload
        #   - Write out by id with no buffer
        elif _type == None:

            if _id == interface.Keys.STOP.value:
                print('STOP command received')

            self.write_serial_packet(_id, [])


    # Create a new MQTT client and connect to the main broker
    def connect_client(self, host, port):
        # Create an MQTT client
        self.client = mqtt.Client(client_id='arcc_gateway')
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

        # Connect to the MQTT broker
        self.client.connect(host, port)


    # ON_CONNECT MQTT EVENT
    def on_connect(self, client, userdata, flags, rc):
        print("Connected to broker with result code {0}".format(str(rc)))
        for _dict in interface.keymap:
            if _dict["consumer"] is not None:
                client.subscribe(_dict["consumer"])
                print("Subscribed: " + _dict["consumer"])


    # ON_DISCONNECT MQTT EVENT
    def on_disconnect(self, client, userdata):
        print("MQTT client disconnected")


    def connect_serial(self, path):
        ports = list_ports.comports()
        for port in ports:
            if port.device == path:
                self.serialport = Serial(path, timeout=0, write_timeout=0, baudrate=115200, rtscts=True, dsrdtr=True)
        return False


    def open_serial(self):
        if not self.serialport.isOpen():
            self.serialport.open()
            self.serialport.flushInput()
            self.serialport.flushOutput()
            self.timeout = monotonic()
        else:
            print('Connected to serial device at: ' + self.serialport.name)


    def close_serial(self):
        if self.serialport.isOpen():
            print('Closing serialport')
            self.write_serial_packet(interface.Keys.CLOSE.value, bytearray(0))
            self.serialport.close()
            self.client.publish('get/connected', json.dumps(False))


    def read_serial(self):
        if self.serialport:
            while self.serialport.in_waiting > 0:
                _bytes = self.serialport.read(16)
                for _byte in _bytes:
                    self.serial_buffer[self.serial_read_index] = _byte
                    if _byte == 0x00:
                        _packet = encoder.cobs_decode(self.serial_buffer[0:self.serial_read_index])
                        if type(_packet) == bytearray:
                            if encoder.crc(_packet[1:]) == _packet[0]:
                                _id = _packet[1]
                                _msg = _packet[2:]
                                self.on_packet(_id, _msg)
                        self.serial_read_index = 0
                    else:
                        if self.serial_read_index + 1 == 255:
                            self.serial_read_index = 0
                        else:
                            self.serial_read_index += 1



    # Write bytes to serial output
    def write_serial_packet(self, id_, payload):
        _buffer = bytearray(2 + len(payload))
        _buffer[1] = id_
        _buffer[2:] = payload
        _buffer[0] = encoder.crc(_buffer[1:])
        _out_buffer = encoder.cobs_encode(_buffer)
        self.serialport.write(_out_buffer)
        self.serialport.write(bytearray([0x00]))


    def write_data_out(self):
        # Get current path
        try:
            _data_files = [file for file in os.listdir(self.directory_path) if os.path.isfile(os.path.join(self.directory_path, file))]
            _fname = "run_data_" + str(len(_data_files)) + "_" + strftime("%H_%M") + ".csv"
            print("Saving run data to: \n", self.directory_path, "\nas ", _fname)
            _fpath = os.path.join(self.directory_path, _fname)
            with open(_fpath, 'w', newline='') as csvfile:
                csvwriter = csv.writer(csvfile, delimiter=',', quotechar='|')
                csvwriter.writerow(dataStruct)
                for row in self.runData:
                    csvwriter.writerow(row)
        except Exception as err:
            print("Exception raised while saving run data!")
            print(err)
    

    def terminate(self):
        self.logfile.write('\n')
        self.logfile.write('Terminating logs: ' + str(monotonic()))
        self.logfile.close()
        print('Logs saved to ' + self.logfile.name)

