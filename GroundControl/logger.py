import paho.mqtt.client as mqtt
import os, sys, getopt, traceback
from time import monotonic
from time import strftime
import json

if __name__ == '__main__':

    topics = [
        'cmd/arm',
        'cmd/disarm',
        'cmd/start',
        'cmd/stop',
        'cmd/calibratethrust',
        'set/controlmode',
        'get/controlmode',
        'set/enginemode',
        'get/enginemode',
        'set/duration',
        'get/duration',
        'set/igniter/voltage',
        'get/igniter/voltage',
        'set/igniter/preburn',
        'get/igniter/preburn',
        'set/igniter/duration',
        'get/igniter/duration',
        'set/targets',
        'get/targets',
        'set/data',
        'get/data',
        'cmd/throttle/position',
        'get/throttle/posiiton',
        'cmd/throttle/encoder',
        'get/throttle/encoder',
    ]

    client = None
    host = '192.168.1.19'
    port = 1883
    outputpath = '/Users/mattvredevoogd/Desktop/log.txt'

    try:
        opts, args = getopt.getopt(sys.argv[1:], "h:p:o:", ["host=", "port=", "output="])
    except getopt.GetoptError:
        print('error or something')
    for opt, arg in opts:
        if opt in ('-h', '--host'):
            host = arg
        elif opt in ('-p', '--port'):
            port = int(arg)
        elif opt in ('-o', '--output'):
            if arg[0] == '~':
                outputpath = os.path.expanduser(arg)
            else:
                outputpath = arg

    if os.path.exists(outputpath):
        exc = Exception(f'File already exists at {outputpath}')
        raise(exc)

    with open(outputpath, 'w') as outputfile:
        print('Starting log session')
        print(outputfile.name)

        _t = strftime('%H:%M %D')
        outputfile.write(f'Log session {_t}\n\n')

        try:
            client = mqtt.Client()
            print(f'Connecting to broker at {host}:{port}')
            client.connect(host, port)

            outputfile.write(f'Connected to broker at {host}:{port}\n')

            def on_message(client, userdata, msg):
                _ts = monotonic()
                outputfile.write(f'{_ts} {msg.topic} {msg.payload}\n')

            client.on_message = on_message

            for topic in topics:
                client.subscribe(topic)
                print(topic)

            try:
                client.loop_forever()

            except KeyboardInterrupt:
                print(f'Terminating logs, saved to {outputpath}')
                outputfile.close()
            else:
                print(f'Terminating logs, saved to {outputpath}')
                outputfile.close()
                print('Exiting')

        except KeyboardInterrupt:
            print(f'\nExited while attempting to connect to broker at {host}:{port}')
            exit(1)
        except Exception as exp:
            print(exp)
            traceback.print_exc()
        