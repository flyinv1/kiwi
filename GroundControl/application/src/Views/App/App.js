import React, { useEffect, useState } from 'react';
import { useSelector, useDispatch } from 'react-redux';
import ConfigurationPanel from '../ConfigurationPanel/ConfigurationPanel';
import ControlPanel from '../ControlPanel/ControlPanel';
import { useTopic, useClientStatus } from '../../Hooks/MQTTProvider';


const App = () => {

    const clientStatus = useClientStatus()
    const test = useTopic('test');

    console.log(clientStatus, test);

    return(
        <div>
            <ConfigurationPanel />
            <ControlPanel />
        </div>
    )
}

export default App;