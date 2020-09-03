import React, { useEffect, useMemo } from 'react';
import { Color, Themes } from 'Components/GLPlot/gl-rtplot';
import styles from './Plot.module.scss';
import { MQTT } from 'mqttKeys.js';
import { useTopic } from '../../../Hooks/MQTTProvider';
import GLPlot from '../../../Components/GLPlot/GLPlot';

const configuration = {
    contextAttributes: {
        alpha: true,
        antialias: true,
    },
    layout: {
        limits: {
            xmin: 0,
            xmax: 5,
            ymin: 0,
            ymax: 1000
        },
        grid: {
            xInterval: 0.5,
            yInterval: 100
        },
        axes: true
    },
    series: {
        voltage: {
            id: 'voltage',
            name: 'Voltage',
            color: Color.fromHex(Themes.palette.slate[0]),
            duration: 5,
            points: 300
        },
    }
}

const PressureStream = ({animate}) => {

    const data = useTopic(MQTT.telemetry.buffer)

    const voltage = useMemo(() => {

    }, [data.payload])

    return(
        <div className={styles.container}>
            <h3>Voltage</h3>
            <GLPlot
                className={styles.voltagePlot}
                configuration={configuration}
                animate={animate}
                newData={null}
            />
        </div>
    )
}

export default PressureStream;