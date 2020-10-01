from dotenv import load_dotenv
import os
from server.manager import Manager
from time import sleep

if __name__ == '__main__':

    load_dotenv()

    host = os.environ.get('host')       # MQTT host address
    port = int(os.environ.get('port'))  # MQTT host port
    isDev = os.environ.get('mode') == "DEBUG"

    device_path = '/dev/ttyACM0'
    output_path = '/home/pi/data'
    if isDev:
        print('Using development configuration')
        device_path = '/dev/cu.usbmodem82249701'
        output_path = '/Users/mattvredevoogd/Desktop/data'

    manager = Manager(output_path)
    manager.connect_client(host, port)
    manager.connect_serial(device_path)

    if manager.serialport == None:
        print("Error opening serial port: path not found\n.env path: " + device_path + "\nCheck that the fire controller is connected")
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