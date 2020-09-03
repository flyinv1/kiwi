import React, { useEffect, useMemo } from 'react';
import { Color, Themes } from 'Components/GLPlot/gl-rtplot';
import styles from './Plot.module.scss';
import { MQTT } from 'mqttKeys.js';
import { useTopic } from '../../../Hooks/MQTTProvider';
import GLPlot from '../../../Components/GLPlot/GLPlot';

const PressureStream = ({}) => {

    const data = useTopic(MQTT.telemetry.buffer)

    const pressure = useMemo(() => {
        
    }, [data.payload])

    return(
        <div>
            <GLPlot
                className={styles.plotContainer}
                configuration={{

                }}
            />
        </div>
    )
}

export default PressureStream;