import json

keymap = [
    {
        "topic": None,
        "id": 0,
        "payload": None,
    },
    {
        "topic": None,
        "id": 255,
        "payload": None
    },
    {
        "topic": "run/arm",
        "id": 1,
        "payload": None
    },
    {
        "topic": "run/arm",
        "id": 2,
        "payload": None
    },
    {
        "topic": "run/start",
        "id": 3,
        "payload": None
    },
    {
        "topic": "run/stop",
        "id": 4,
        "payload": None
    },
    {
        "topic": "run/controlmode",
        "id": 5,
        "payload": int
    },
    {
        "topic": "run/enginemode",
        "id": 6,
        "payload": int
    },
    {
        "topic": "run/duration",
        "id": 7,
        "payload": int
    },
    {
        "topic": "run/igniter/preburn",
        "id": 8,
        "payload": int
    },
    {
        "topic": "run/igniter/duration",
        "id": 9,
        "payload": int
    },
    {
        "topic": "run/keyframes",
        "id": 10,
        "payload": list,
    },
    {
        "topic": "run/calibratethrust",
        "id": 13,
        "payload": None
    },
    {
        "topic": "run/state",
        "id": 14,
        "payload": int
    }
]

def on_topic(topic, payload):
    for _dict in (x for x in keymap if x["topic"] == topic):
        _payload = json.loads(payload)
        return (_payload, _dict["payload"], _dict["id"])


def on_id(id_, payload):
    for _dict in (x for x in keymap if x["id"] == id_):
        return (payload, _dict["payload"], _dict["id"])

