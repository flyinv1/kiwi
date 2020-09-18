import React, { useEffect, useMemo, useState } from 'react';
import { useClientStatus, useTopic } from '../../Hooks/MQTTProvider';
import styles from './HeaderView.module.scss';
import { MQTT } from 'mqttKeys.js'

const MCUSTATEMAP = {
    undefined: "NONE",
    0: "DISCONNECTED",
    1: "STANDBY",
    2: "ARMED",
    3: "RUNNING"
}

const HeaderView = () => {

    const [ mcuStatusString, setmcuStatusString ] = useState('offline');

    const mqttStatus = useClientStatus();
    const mcuStatus = useTopic(MQTT.use.connected);
    const mcuState = useTopic(MQTT.use.state);

    useEffect(() => {
        let connected = false;
        try {
            connected = JSON.parse(mcuStatus.payload);
        } catch(err) {
            console.log(err);
        }
        setmcuStatusString(connected ? 'connected' : 'offline')
    }, [mcuStatus]);
    

    return(
        <div className={styles.container}>
            <div className={styles.title}>Kiwi Ground Control</div>
            <div className={styles.statusContainer}>
                <StatusIndicator prefix={'MQTT'} status={mqttStatus.status}/>
                <StatusIndicator prefix={'MCU'} status={mcuStatusString}/>
            </div>
            <div className={styles.stateContainer}>
                <StateIndicator state={mcuState.payload}/>
            </div>
        </div>
    )
}

const StatusIndicator = ({prefix, status}) => {

    const statusStyle = {
        'connected': styles.connected,
        'reconnecting': styles.reconnecting,
        'closed': styles.closed,
        'offline': styles.offline
    }

    const style = useMemo(() => {
        return statusStyle[status]
    }, [status])

    return(
        <div className={[style || '', styles.statusIndicator].join(' ')}>
            {`${prefix}: ${status}`}
        </div>
    )
}

const StateIndicator = ({state}) => {

    return(
        <div className={[styles.stateIndicator].join(' ')}>
            {`Controller: ${MCUSTATEMAP[state]}`}
        </div>
    )
}

export default HeaderView