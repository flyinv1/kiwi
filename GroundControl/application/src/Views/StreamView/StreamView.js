import React, { useEffect, useState, useCallback, useRef, useMemo } from 'react';
import GLPlot from '../../Components/GLPlot/GLPlot';
import { useTopic, useClientStatus } from '../../Hooks/MQTTProvider';
import styles from './StreamView.module.scss';
import { Color, Themes } from '../../Components/GLPlot/gl-rtplot';
import PressureStream from './Streams/PressureStream';

const config = {
    contextAttributes: {
        antialias: true,
        alpha: true
    },
    layout: {
        limits: {
            xmin: 0,
            xmax: 10,
            ymin: -1,
            ymax: 1
        },
        grid: {
            xInterval: 1,
            yInterval: 0.1,
        },
        axes: true
    },
    series: {
        pressure_0: {
            name: 'pressure0',
            color: Color.fromHex(Themes.palette.umber[0]),
            duration: 10,
            points: 3600,
        }
    }
}

const StreamView = () => {

    const testTopic = useTopic('test');
    const connected = useClientStatus();

    const pressureData = useMemo(() => {
        return {
            pressure_0: new Float32Array([Number(testTopic.payload) || 0]),
        }
    }, [ testTopic ])

    return(
        <div className={styles.container}>
            <PressureStream/>
            <GLPlot
                className={styles.plot}
                newData={pressureData}
                configuration={config}
                animate={connected}
            />
        </div>
    )
}

export default StreamView;