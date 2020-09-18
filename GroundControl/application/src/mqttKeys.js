export const MQTT = {
    cmd: {
        arm: 'cmd/arm',
        disarm: 'cmd/disarm',
        start: 'cmd/start',
        stop: 'cmd/stop',
        throttle: {
            position: 'cmd/throttle/position',
            encoder: 'cmd/throttle/encoder'
        } 
    },
    set: {
        enginemode: 'set/enginemode',
        controlmode: 'set/controlmode',
        duration: 'set/duration',
        igniter: {
            voltage: 'set/igniter/voltage',
            preburn: 'set/igniter/preburn',
            duration: 'set/igniter/duration',
        },
        targets: 'set/targets',
    },
    use: {
        enginemode: 'get/enginemode',
        controlmode: 'get/controlmode',
        duration: 'get/duration',
        igniter: {
            voltage: 'get/igniter/voltage',
            preburn: 'get/igniter/preburn',
            duration: 'get/igniter/duration',
        },
        throttle: {
            position: 'get/throttle/position',
            encoder: 'get/throttle/encoder',
        },
        targets: 'get/targets',
        connected: 'get/connected',
        state: 'get/state',
        data: 'get/data'
    },
}