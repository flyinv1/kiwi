import React, { useEffect, useState, useCallback, useRef, useMemo } from 'react';
import GLPlot from '../../Components/GLPlot/GLPlot';
import { useTopic } from '../../Hooks/MQTTProvider';
import styles from './StreamView.module.scss';
import { Color, Themes } from '../../Components/GLPlot/gl-rtplot';

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
        // axes: {
        //     color: Color.fromHex(Themes.palette.slate[3])
        // },
        axes: false
    },
    series: {
        pressure_0: {
            name: 'pressure0',
            color: Color.fromHex(Themes.palette.umber[0]),
            duration: 10,
            points: 3600,
        },
        pressure_1: {
            name: 'pressure1',
            color: Color.fromHex(Themes.palette.umber[1]),
            duration: 10,
            points: 3600
        }
    }
}

const StreamView = (props) => {

    const testTopic = useTopic('test');
    const testTopic2 = useTopic('test2');
    const testTopic3 = useTopic('test3');
    const testTopic4 = useTopic('test4');

    const pressureData = useMemo(() => {
        return {
            pressure_0: new Float32Array([Number(testTopic.payload) || 0]),
            pressure_1: new Float32Array([-1 * Number(testTopic2.payload) + Math.random() || 0])
        }
    }, [ testTopic, testTopic2, testTopic3, testTopic4 ])

    return(
        <div>
            <GLPlot
                className={styles.plot}
                newData={pressureData}
                configuration={config}
                animate={true}
            />
        </div>
    )
}

export default StreamView;