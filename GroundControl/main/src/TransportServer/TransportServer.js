const ws = require('ws');

// The TransportServer provides a wrapper for electron server-client communication outside of IPCMain
// for high frequency data transmission using a websocket.
// This is used for all data transmission to the client application for real time plotting.

function TransportServer() {

    this.server = null;
    this.manager = new TransportManager()

    let messageQueue = [];

    this.initialize = (port = 8080, opts = {
        perMessageDeflate: false,
    }) => {
        try {

            this.server = new ws.Server({
                port: port,
                ...opts
            });

            this.server.on('connection', (socket) => {
                socket.onmessage = (data) => onSocketMessage(socket, data);
                socket.onclose = onSocketClose;
            });

            this.server.on('error', () => {

            });

            this.server.on('close', () => {

            });

        } catch(err) {

        }
    }

    const onSocketMessage = (socket, message) => {
        try {
            const packet = JSON.parse(message.data);
            if (packet.id) {
                onRegisteredPacket(packet);
            } else {
                this.manager.handlePacket(packet);
            }
        } catch(err) {
            
        }
    }

    const onRegisteredPacket = (packet) => {
        const index = messageQueue.findIndex(obj => obj.id === packet.id);
        const responseObject = messageQueue[index];
        if (responseObject.callback) try {
            responseObject.callback(packet);
        } catch(err) {
            console.log(err.message, packet);
        }
        messageQueue.splice(index, 1);
    }

    const onSocketClose = () => {

    }

}

module.exports = {
    default: TransportServer
}