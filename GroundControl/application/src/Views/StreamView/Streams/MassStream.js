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
            xmax: 30,
            ymin: 0,
            ymax: 20
        },
        grid: {
            xInterval: 5,
            yInterval: 2
        },
        axes: true
    },
    series: {
        mass: {
            id: 'mass',
            name: 'Mass',
            color: Color.fromHex(Themes.palette.slate[1]),
            duration: 30,
            points: 1800
        },
    }
}

const MassStream = ({animate, newData}) => {

    const streamData = useMemo(() => {
        return {
            mass: newData || 0,
        }
    }, [newData])

    return(
        <div className={styles.container}>
            <div className={styles.header}>
                <h3>Propellant Mass</h3>
                <span>kg</span>
            </div>
            <GLPlot
                className={styles.massPlot}
                configuration={configuration}
                animate={animate}
                newData={streamData}
            />
        </div>
    )
}

export default MassStream;