import React, { useEffect, useState } from 'react';
import { useSelector, useDispatch } from 'react-redux';
import ConfigurationView from '../ConfigurationView/ConfigurationView';
import ControlView from '../ControlView/ControlView';
import { useTopic, useClientStatus } from '../../Hooks/MQTTProvider';
import StreamView from '../StreamView/StreamView';
import HeaderView from '../HeaderView/HeaderView';
import styles from './App.module.scss';


const App = () => {

    const { status } = useClientStatus();
    
    return(
        <div className={styles.appContainer}>
            <HeaderView status={status}/>
            <div className={styles.contentContainer}>
                <StreamView/>
                <ConfigurationView/>
            </div>
        </div>
    )
}

export default App;