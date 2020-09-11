import paho.mqtt.client as mqtt

class Manager:
    
    def __init__(self):
        self.host = '192.168.1.10'
        self.port = 1883
        self.serialport = None

    def main(self):
        # first update the MQTT client
        self.client.loop()
        self.serialport

    def connect_serial(self, hwid):
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

    

