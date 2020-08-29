const { } = require('../keys');
const {nanoid} = require('nanoid');

let socket;
let messageQueue;

async function openNewSocket() {
    return new Promise((resolve, reject) => {
        if (!socket) {
            const port = 80;
            socket = new WebSocket(`ws://localhost:${port}`);
            socket.onopen = () => {
                console.log("Client has connected to socket");
                resolve(true);
            }
            socket.onmessage = receiveMessage;
        } else {
            reject("Socket already open on port:", socket.port);
        }
    })
}

function receiveMessage(message) {
    try {
        const packet = JSON.parse(message.data);
        if (packet.id) {
            const messageIndex = messageQueue.findIndex(obj => obj.id === packet.id);
            const responseObject = messageQueue[messageIndex];
            if (responseObject.callback) try {
                responseObject.callback(packet);
            } catch(err) {
                console.log(err.message, packet);
            }
            messageQueue.splice(messageIndex, 1);
        } else {
            
        }
    } catch(err) {
        console.log(err);
    }
}

async function sendMessage(packet, callback) {
    packet["id"] = nanoid(10);
    socket.send(JSON.stringify(packet));
    if (callback) {
        return new Promise((resolve, reject) => {
            messageQueue.push({
                id: packet.id,
                payload: packet.payload,
                callback: (receivedPacket) => {
                    try {
                        callback(receivedPacket);
                        resolve(true);
                    } catch(err) {
                        reject("Callback failed with error:", err);
                    }
                }
            });
        })
    }
}

async function closeSocket() {
    return new Promise((resolve, reject) => {
        socket.close();
    })
}

function attachListener() {
    if (window.arcc.listeners[type]) {
        window.arcc.listeners[type].push({
            name: name,
            callback: callback
        });
        return true;
    } else { return false }
}

function removeListener(type, name) {
    const index = window.arcc.listeners[type]?.findIndex(listener => listener.name === name);
    window.arcc.listeners[type].splice(index, 1);
}

module.exports = {
    openNewSocket,
    sendMessage,
    closeSocket,
    attachListener,
    removeListener
}