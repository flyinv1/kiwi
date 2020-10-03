from dotenv import load_dotenv
import os
from server.manager import Manager
from time import sleep

if __name__ == '__main__':

    load_dotenv()

    host = os.environ.get('host')       # MQTT host address
    port = int(os.environ.get('port'))  # MQTT host port
    isDev = os.environ.get('mode') == "DEV" # Check if using in debug mode (not running on RPI)

    device_path = '/dev/ttyACM0'
    output_path = '/home/pi/data'
    log_dir = '/home/pi/logs'

    if isDev:
        print('Using development configuration')
        device_path = '/dev/cu.usbmodem82249701' # This may change depending on the serial port used
        output_path = '/Users/mattvredevoogd/Desktop/data'
        log_dir = 'Users/mattvredevoogd/Desktop/logs'

    manager = Manager(output_path, log_dir)
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
        manager.terminate()
        print("\nClosing program")