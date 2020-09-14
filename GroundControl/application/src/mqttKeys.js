export const MQTT = { 
    run: {
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
        disarm: 'run/disarm',
        start: 'run/start',
        stop: 'run/stop',
        status: 'run/status',
    },
    telemetry: {
        buffer: 'telemetry/buffer'
    }
}