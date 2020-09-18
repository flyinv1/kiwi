import json
from enum import Enum

class Keys(Enum):
    SYNC = 0
    RUN_ARM = 1
    RUN_DISARM = 2
    RUN_START = 3
    RUN_STOP = 4
    SET_CONTROLMODE = 5
    SET_ENGINEMODE = 6
    SET_RUNDURATION = 7
    SET_IGNITERPREBURN = 8
    SET_IGNITERDURATION = 9
    SET_TARGETS = 10
    RUN_CALIBRATE_THRUST = 11
    CLOSE = 12
    STATE = 14
    DATA = 15

keymap = [
    {
        "topic": None,
        "id": Keys.SYNC,
        "payload": None,
    },
    {
        "topic": "run/arm",
        "id": Keys.RUN_ARM,
        "payload": None
    },
    {
        "topic": "run/disarm",
        "id": Keys.RUN_DISARM,
        "payload": None
    },
    {
        "topic": "run/start",
        "id": Keys.RUN_START,
        "payload": None
    },
    {
        "topic": "run/stop",
        "id": Keys.RUN_STOP,
        "payload": None
    },
    {
        "topic": "run/controlmode",
        "id": Keys.SET_CONTROLMODE,
        "payload": int
    },
    {
        "topic": "run/enginemode",
        "id": Keys.SET_ENGINEMODE,
        "payload": int
    },
    {
        "topic": "run/duration",
        "id": Keys.SET_RUNDURATION,
        "payload": int
    },
    {
        "topic": "run/igniter/preburn",
        "id": Keys.SET_IGNITERPREBURN,
        "payload": int
    },
    {
        "topic": "run/igniter/duration",
        "id": Keys.SET_IGNITERDURATION,
        "payload": int
    },
    {
        "topic": "run/keyframes",
        "id": Keys.SET_TARGETS,
        "payload": list,
        "type": int,
    },
    {
        "topic": "run/calibratethrust",
        "id": Keys.RUN_CALIBRATE_THRUST,
        "payload": None
    },
    {
        "topic": "run/state",
        "id": Keys.STATE,
        "payload": int
    },
    {
        "topic": "run/data",
        "id": Keys.DATA,
        "payload": list,
        "type": float
    }
]

def on_topic(topic, payload):
    for _dict in (x for x in keymap if x["topic"] == topic):
        _payload = json.loads(payload)
        return (_payload, _dict["payload"], _dict["id"].value)


def on_id(id_, payload):
    for _dict in (x for x in keymap if x["id"] == id_):
        return (payload, _dict["payload"], _dict["id"].value)
    return (payload, None, id_)

