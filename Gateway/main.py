from dotenv import load_dotenv
import os

from server.manager import Manager

load_dotenv()

host = os.environ.get('host')
port = int(os.environ.get('port'))

manager = Manager()
manager.connect_client(host, port)
manager.start()