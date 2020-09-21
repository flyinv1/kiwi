import json
from enum import Enum

# 
# 'Keys' provides a copy of the MCU's internal callback-id enum
#
class Keys(Enum):
    SYNC = 0
    ARM = 1
    DISARM = 2
    START = 3
    STOP = 4
    CONTROLMODE = 5
    ENGINEMODE = 6
    RUNDURATION = 7
    IGNITERPREBURN = 8
    IGNITERDURATION = 9
    IGNITERVOLTAGE = 10
    TARGETS = 11
    CALIBRATE_THRUST = 12
    CLOSE = 13
    THROTTLE_POSITION = 14
    THROTTLE_ENCODER = 15
    STATE = 17
    DATA = 18
    LOG = 19

#
# The keymap is configured to:
#   1) Read incoming MQTT messages and forward payload to serial packet by id
#   2) Read incoming serial packets and forward payload to MQTT message by topic
#   3) Provide information on expected payload datatype to appropriately convert to/from bytearray
#
keymap = [
    {
        "emitter": None,
        "consumer": None,
        "id": Keys.SYNC,
        "type": None,
    },
    {
        "emitter": None,
        "consumer": "cmd/arm",
        "id": Keys.ARM,
        "type": None
    },
    {
        "emitter": None,
        "consumer": "cmd/disarm",
        "id": Keys.DISARM,
        "type": None
    },
    {
        "emitter": None,
        "consumer": "cmd/start",
        "id": Keys.START,
        "type": None
    },
    {
        "emitter": None,
        "consumer": "cmd/stop",
        "id": Keys.STOP,
        "type": None
    },
    {
        "emitter": None,
        "consumer": "cmd/calibratethrust",
        "id": Keys.CALIBRATE_THRUST,
        "type": None
    },
    {
        "emitter": "get/controlmode",
        "consumer": "set/controlmode",
        "id": Keys.CONTROLMODE,
        "type": int
    },
    {
        "emitter": "get/enginemode",
        "consumer": "set/enginemode",
        "id": Keys.ENGINEMODE,
        "type": int
    },
    {
        "emitter": "get/duration",
        "consumer": "set/duration",
        "id": Keys.RUNDURATION,
        "type": int
    },
        {
        "emitter": "get/igniter/voltage",
        "consumer": "set/igniter/voltage",
        "id": Keys.IGNITERVOLTAGE,
        "type": int
    },
    {
        "emitter": "get/igniter/preburn",
        "consumer": "set/igniter/preburn",
        "id": Keys.IGNITERPREBURN,
        "type": int
    },
    {
        "emitter": "get/igniter/duration",
        "consumer": "set/igniter/duration",
        "id": Keys.IGNITERDURATION,
        "type": int
    },
    {
        "emitter": "get/targets",
        "consumer": "set/targets",
        "id": Keys.TARGETS,
        "type": list,
    },
    {
        "emitter": "get/state",
        "consumer": None,
        "id": Keys.STATE,
        "type": int,
    },
    {
        "emitter": "get/data",
        "consumer": "set/data",
        "id": Keys.DATA,
        "type": list,
    },
    {
        "emitter": None,
        "consumer": None,
        "id": Keys.LOG,
        "type": str
    },
    {
        "emitter": "get/throttle/position",
        "consumer": "cmd/throttle/position",
        "id": Keys.THROTTLE_POSITION,
        "type": int
    },
    {
        "emitter": "get/throttle/encoder",
        "consumer": "cmd/throttle/encoder",
        "id": Keys.THROTTLE_ENCODER,
        "type": int
    },
]


def on_mqtt(topic, payload):
    for _dict in (x for x in keymap if x["consumer"] == topic):
        _payload = json.loads(payload)
        return (_payload, _dict["id"].value, _dict["type"], _dict["consumer"])


def on_serial(id_, payload):
    for _dict in (x for x in keymap if x["id"].value == id_):
        return (payload, _dict["id"].value, _dict["type"], _dict["emitter"])
    return (payload, id_, None, None)

