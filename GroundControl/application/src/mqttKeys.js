export const MQTT = { 
    run: {
        datarate: 'run/daqrate',
        enginemode: 'run/enginemode',
        controlmode: 'run/controlmode',
        duration: 'run/duration',
        igniter: {
            voltage: 'run/igniter/voltage',
            preburn: 'run/igniter/preburn',
            duration: 'run/igniter/duration',
        },
        keyframes: 'run/keyframes',
        arm: 'run/arm',
        start: 'run/start',
        status: 'run/status',
    },
    telemetry: {
        buffer: 'telemetry/buffer'
    }
}