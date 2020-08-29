const mqtt = require('mqtt')

// Insecurely expose the mqtt module to the React front-end
// This should instead call whitelisted methods to connect an mqtt client
window.kiwi = {
    mqtt: mqtt
}
