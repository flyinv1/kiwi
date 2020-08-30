import React, { useEffect, useState, useCallback } from 'react';
import GLPlot from '../../Components/GLPlot/GLPlot';
import { useTopic } from '../../Hooks/MQTTProvider';
import styles from './StreamView.module.scss';

const StreamView = (props) => {

    const {payload} = useTopic('test');

    useEffect(() => {
        console.log("updated")
    }, [payload])

    return(
        <div>
            <GLPlot 
                className={styles.plot}
                buffer={[0, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3]}
                config={{
                    points: 20,
                    streams: 5,
                    axes: true,
                    duration: 0
                }}
                isLive={true}
                contextAttributes={{
                    antialias: true,
                    alpha: true
                }}
                layout={{
                    scale: 1,
                    pixelRatio: 1,
                }}
            />
        </div>
    )
}

export default StreamView;