import React, { useEffect, useState, useCallback, useRef, useMemo } from 'react';
import GLPlot from '../../Components/GLPlot/GLPlot';
import { useTopic, useClientStatus } from '../../Hooks/MQTTProvider';
import styles from './StreamView.module.scss';
import { Color, Themes } from '../../Components/GLPlot/gl-rtplot';
import {
    PressureStream,
    ThrustStream,
    MassStream,
    VoltageStream,
    ThrottleStream
} from './Streams';
import { MQTT } from 'mqttKeys.js';

const StreamView = () => {

    const bufferTopic = useTopic(MQTT.use.data);

    const armedTopic = useTopic(MQTT.use.state);

    // const [ shouldAnimate, setShouldAnimate ] = useState(false);

    const buffer = useMemo(() => {
        let _databuffer
        try {
            _databuffer = JSON.parse(bufferTopic.payload);
        } catch(err) {}
        return {
            pressure: _databuffer?.slice(0, 2),
            thrust: _databuffer?.[2],
            mass: _databuffer?.[3],
            voltage: _databuffer?.[4],
            throttle: _databuffer?.[5]
        }
    }, [bufferTopic])

    const shouldAnimate = useMemo(() => {
        // console.log(armedTopic)
        let _armed
        try {
            _armed = JSON.parse(armedTopic.payload);
        } catch(err) {
            _armed = (armedTopic.payload)
        }
        console.log(Boolean(_armed));
        return Boolean(_armed);
    }, [armedTopic.payload])

    return(
        <div className={styles.container}>
            <div className={styles.top}>
                <div className={styles.left}>
                    <ThrustStream 
                        animate={shouldAnimate}
                        newData={buffer.thrust}
                    />
                    <MassStream 
                        animate={shouldAnimate}
                        newData={buffer.mass}
                    />
                </div>
                <div className={styles.right}>
                    <PressureStream 
                        animate={shouldAnimate}
                        newData={buffer.pressure}
                    />
                </div>
            </div>
            <div className={styles.bottom}>
                <VoltageStream 
                    animate={shouldAnimate}
                    newData={buffer.voltage}
                />
                <ThrottleStream
                    animate={shouldAnimate}
                    newData={buffer.throttle}
                />
            </div>
        </div>
    )
}

export default StreamView;