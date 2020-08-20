import paho.mqtt.client as mqtt

class Manager:
    
    def __init__(self):
        self.host = 'localhost'
        self.port = 1883

    def start(self):
        print("Starting client")

    def connect_client(self, host, port):
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.connect(self.host, self.port)
            
    def on_connect(self, client, userdata, flags, rc):
        print("")

    def on_message(self, client, userdata, message):
        print("")

    def on_disconnect(self, client, userdata):
        print("")

    

