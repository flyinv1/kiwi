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
        pressure0: {
            id: 'pressure0',
            name: 'Pressure 0',
            color: Color.fromHex(Themes.palette.umber[0]),
            duration: 5,
            points: 300
        },
        pressure1: {
            id: 'pressure1',
            name: 'Pressure 1',
            color: Color.fromHex(Themes.palette.umber[1]),
            duration: 5,
            points: 300
        },
    }
}

const PressureStream = ({animate, newData}) => {

    const streamData = useMemo(() => {
        let _data = newData || [0, 0];
        return {
            pressure0: _data[0],
            pressure1: _data[1]
        }
    }, [newData])

    return(
        <div className={styles.container}>
            <div className={styles.header}>
                <h2>Pressure</h2>
                <span>PSI</span>
            </div>
            <GLPlot
                className={styles.pressurePlot}
                configuration={configuration}
                animate={animate}
                newData={streamData}
            />
        </div>
    )
}

export default PressureStream;