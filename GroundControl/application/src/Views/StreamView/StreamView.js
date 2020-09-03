import React, { useEffect, useState, useCallback, useRef, useMemo } from 'react';
import GLPlot from '../../Components/GLPlot/GLPlot';
import { useTopic, useClientStatus } from '../../Hooks/MQTTProvider';
import styles from './StreamView.module.scss';
import { Color, Themes } from '../../Components/GLPlot/gl-rtplot';
import {
    PressureStream,
    ThrustStream,
    MassStream,
    VoltageStream
} from './Streams';
import { MQTT } from 'mqttKeys.js';

const StreamView = () => {

    const bufferTopic = useTopic(MQTT.telemetry.buffer);

    const buffer = useMemo(() => {
        let _databuffer
        try {
            _databuffer = JSON.parse(bufferTopic.payload);
        } catch(err) {}
        return {
            pressure: _databuffer?.slice(0, 2),
            thrust: _databuffer?.slice(2, 3),
            mass: _databuffer?.slice(3, 4),
            voltage: _databuffer?.slice(4, 5),
        }
    }, [bufferTopic])

    return(
        <div className={styles.container}>
            <div className={styles.top}>
                <div className={styles.left}>
                    <ThrustStream 
                        animate={true}
                        newData={buffer.thrust}
                    />
                    <MassStream 
                        animate={true}
                        newData={buffer.mass}
                    />
                </div>
                <PressureStream 
                    animate={true}
                    newData={buffer.pressure}
                />
            </div>
            <div className={styles.bottom}>
                <VoltageStream 
                    animate={true}
                    newData={buffer.voltage}
                />
            </div>
        </div>
    )
}

export default StreamView;