import React, { useEffect, useState, useCallback, useRef, useMemo, useReducer } from 'react';
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

const StreamView = ({shouldAnimate = true}) => {

    const bufferTopic = useTopic(MQTT.use.data);

    // const [ shouldAnimate, setShouldAnimate ] = useState(false);

    const [, forceUpdate] = useReducer(x => x + 1, 0);

    const buffer = useMemo(() => {
        let _databuffer
        try {
            _databuffer = JSON.parse(bufferTopic.payload);
            _databuffer = _databuffer?.map(value => {
                // Please just rerender everytime there is data jeez
                const _l = value + Math.random() / 1000000;
                return _l
            })
            // console.log(_databuffer)
        } catch(err) {}
        return {
            chamber_pressure: _databuffer?.[0],
            upstream_pressure: _databuffer?.[1],
            downstream_pressure: _databuffer?.[2],
            thrust: _databuffer?.[3],
            propellant_mass: _databuffer?.[4],
            mass_flow: _databuffer?.[5],
            throttle_position: _databuffer?.[6],
            mission_elapsed_time: _databuffer?.[7],
            state_elapsed_time: _databuffer?.[8],
            delta_time: _databuffer?.[9],
        }
    }, [bufferTopic])

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
                        newData={buffer.propellant_mass}
                    />
                </div>
                <div className={styles.right}>
                    <PressureStream 
                        animate={shouldAnimate}
                        newData={[buffer.chamber_pressure, buffer.upstream_pressure, buffer.downstream_pressure]}
                    />
                </div>
            </div>
            <div className={styles.bottom}>
                <VoltageStream 
                    animate={shouldAnimate}
                    newData={buffer.mass_flow}
                />
                <ThrottleStream
                    animate={shouldAnimate}
                    newData={buffer.throttle_position}
                />
            </div>
        </div>
    )
}

export default StreamView;