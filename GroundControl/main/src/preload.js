const mqtt = require('mqtt')

window.kiwi = {
    mqttConnectClient: (config) => mqtt.connect(config)
}
