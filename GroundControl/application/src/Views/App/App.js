import React, { useEffect, useState } from 'react';
import { useSelector, useDispatch } from 'react-redux';
import ConfigurationView from '../ConfigurationView/ConfigurationView';
import { useTopic, useClientStatus } from '../../Hooks/MQTTProvider';
import StreamView from '../StreamView/StreamView';
import HeaderView from '../HeaderView/HeaderView';
import styles from './App.module.scss';
import { MQTT } from 'mqttKeys.js'


const App = () => {    
    return(
        <div className={styles.appContainer}>
            <HeaderView/>
            <div className={styles.contentContainer}>
                <StreamView/>
                <ConfigurationView/>
            </div>
        </div>
    )
}

export default App;