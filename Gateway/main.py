from dotenv import load_dotenv
import os
from server.manager import Manager
from time import sleep

if __name__ == '__main__':

    load_dotenv()

    host = os.environ.get('host')       # MQTT host address
    port = int(os.environ.get('port'))  # MQTT host port
    path = os.environ.get('path')       # Device path name
    output_path = os.environ.get('output_path')

    manager = Manager(output_path)
    manager.connect_client(host, port)
    manager.connect_serial(path)

    if manager.serialport == None:
        print("Error opening serial port: path not found\n.env path: " + path + "\nCheck that the fire controller is connected")
        exit()
    else:
        manager.open_serial()

    try:
        while 1:
            manager.main()
    except KeyboardInterrupt:
        # Safely disconnect from the fire controller
        manager.close_serial()
        print("\nClosing program")