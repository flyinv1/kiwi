import React, { useEffect, useMemo, useReducer } from 'react';
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
            xmax: 20,
            ymin: 0,
            ymax: 100
        },
        grid: {
            xInterval: 2,
            yInterval: 10
        },
        axes: true
    },
    series: {
        throttle: {
            id: 'throttle',
            name: 'Throttle',
            color: Color.fromHex(Themes.palette.umber[2]),
            duration: 20,
            points: 1200
        },
    }
}

const ThrottleStream = ({animate, newData}) => {

    const streamData = useMemo(() => {
        return {
            throttle: newData || 0,
        }
    }, [newData])

    return(
        <div className={styles.container}>
            <div className={styles.header}>
                <h3>Throttle</h3>
                <span>%</span>
            </div>
            <GLPlot
                className={styles.throttlePlot}
                configuration={configuration}
                animate={animate}
                newData={streamData}
            />
        </div>
    )
}

export default ThrottleStream;