import React, { useEffect, useState } from 'react';
import { useSelector, useDispatch } from 'react-redux';
import ConfigurationView from '../ConfigurationView/ConfigurationView';
import ControlView from '../ControlView/ControlView';
import { useTopic, useClientStatus } from '../../Hooks/MQTTProvider';
import StreamView from '../StreamView/StreamView';


const App = () => {

    const { status } = useClientStatus();
    
    return(
        <div>
            { status }
            <ConfigurationView />
            <ControlView/>
            <StreamView/>
        </div>
    )
}

export default App;