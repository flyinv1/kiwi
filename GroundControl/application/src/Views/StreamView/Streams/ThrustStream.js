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
            ymax: 60
        },
        grid: {
            xInterval: 0.5,
            yInterval: 5
        },
        axes: true
    },
    series: {
        thrust: {
            id: 'thrust',
            name: 'Thrust',
            color: Color.fromHex(Themes.palette.slate[0]),
            duration: 5,
            points: 300
        },
    }
}

const ThrustStream = ({animate, newData}) => {

    const bufferData = useMemo(() => {
        return { 
            thrust: newData
        }
    }, [newData])

    return(
        <div className={styles.container}>
            <div className={styles.header}>
                <h3>Thrust</h3>
                <span>N</span>
            </div>
            <GLPlot
                className={styles.thrustPlot}
                configuration={configuration}
                animate={animate}
                newData={bufferData}
            />
        </div>
    )
}

export default ThrustStream;