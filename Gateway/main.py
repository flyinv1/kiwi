from dotenv import load_dotenv
import os
from server.manager import Manager
from time import sleep

if __name__ == '__main__':

    load_dotenv()

    host = os.environ.get('host')       # MQTT host address
    port = int(os.environ.get('port'))  # MQTT host port
    path = os.environ.get('path')       # Device path name

    manager = Manager()
    manager.connect_client(host, port)
    manager.connect_serial(path)
    manager.open_serial()

    try:
        while 1:
            manager.main()
    except KeyboardInterrupt:
        # handle things
        print("Closing program")