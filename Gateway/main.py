from dotenv import load_dotenv
import os
from serial.tools import list_ports
from server.manager import Manager

load_dotenv()

host = os.environ.get('host')       # MQTT host address
port = int(os.environ.get('port'))  # MQTT host port
hwid = os.environ.get('hwid')       # Serial hardware ID

manager = Manager()
manager.connect_client(host, port)
manager.connect_serial(hwid)

try:
    while 1:
        manager.main()
except KeyboardInterrupt:
    # handle things
    print("Closing program")