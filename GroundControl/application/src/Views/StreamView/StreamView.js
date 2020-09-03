import React, { useEffect, useState, useCallback, useRef, useMemo } from 'react';
import GLPlot from '../../Components/GLPlot/GLPlot';
import { useTopic, useClientStatus } from '../../Hooks/MQTTProvider';
import styles from './StreamView.module.scss';
import { Color, Themes } from '../../Components/GLPlot/gl-rtplot';
import {
    PressureStream,
    ThrustStream,
    MassStream,
    VoltageStream
} from './Streams';

const StreamView = () => {

    return(
        <div className={styles.container}>
            <div className={styles.top}>
                <div className={styles.left}>
                    <ThrustStream 
                        animate={false}
                    />
                    <MassStream 
                        animate={false}
                    />
                </div>
                <PressureStream 
                    animate={false}
                />
            </div>
            <div className={styles.bottom}>
                <VoltageStream 
                    animate={false}
                />
            </div>
        </div>
    )
}

export default StreamView;