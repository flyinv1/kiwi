import React, { useState, useMemo, useEffect } from 'react';
import styles from './ConfigurationView.module.scss';
import { Input, NumInput, Checkbox, Dropdown, DropdownSelect, ActivePanel } from '../../Components';
import { useTopic, usePublishJSON } from '../../Hooks/MQTTProvider';
import MapValueInput from '../../Components/Inputs/MapValueInput/MapValueInput';
import { PrimaryButton, DestructiveButton } from '../../Components/Button/Button';
import { MQTT } from 'mqttKeys.js'

const ConfigurationView = () => {

    const publish = usePublishJSON()

    const [ daqRate, setdaqRate ] = useState(0); 
    const [ controlMode, setControlMode ] = useState(-1);
    const [ engineMode, setEngineMode ] = useState(-1);
    const [ runDuration, setRunDuration ] = useState(0);
    const [ igniterVoltage, setIgniterVoltage ] = useState(0);
    const [ preIgnitionDuration, setPreIgnitionDuration ] = useState(0);
    const [ ignitionDuration, setIgnitionDuration ] = useState(0);

    // const [ validConfiguration, setValidConfiguration ] = useState(false);

    const [ isArmed, setIsArmed ] = useState(false);
    const [ isRunning, setIsRunning ] = useState(false);

    const [ keyframeMap, setKeyFrameMap ] = useState(new Map());

    const testStatusTopic = useTopic(MQTT.run.start);

    const publishKeyFrameMap = (map) => {
        
    }

    const updateKeyframeMapValue = (key, value) => {
        const _value = Math.min(Math.max(0, value), 100);
        let _map = new Map(keyframeMap.set(key, _value));
        let _sortedMap = new Map([..._map].sort((a, b) => {
            return Number(a[0]) > Number(b[0]) ? 1 : -1;
        }))
        setKeyFrameMap(_sortedMap);
    }

    const updateKeyframeMap = (map) => {
        let _sortedMap = new Map([...map].sort((a, b) => {
            return Number(a[0]) > Number(b[0]) ? 1 : -1;
        }))
        setKeyFrameMap(_sortedMap);
    }

    const validConfiguration = useMemo(() => {
        let _valid = false;
        if (engineMode === 0) {
            _valid = (controlMode >= 0 && runDuration > 0 && [...keyframeMap].length > 0)
        } else {
            _valid = (
                controlMode >= 0 &&
                runDuration > 0 && 
                igniterVoltage > 0 &&
                preIgnitionDuration >= 0 &&
                ignitionDuration > 0 &&
                [...keyframeMap].length > 0
            )
        }
        console.log('test', _valid);
        // setValidConfiguration(_valid);
        return (_valid);
    }, [ 
        controlMode,
        engineMode,
        runDuration,
        igniterVoltage,
        preIgnitionDuration,
        ignitionDuration,
        keyframeMap
    ])

    useEffect(() => {
        const windowListener = (e) => {
            setIsRunning(false);
        }
        window.addEventListener('keypress', windowListener);
        return(() => {
            window.removeEventListener('keypress', windowListener)
        })
    }, [isRunning])

    useEffect(() => {
        // console.log('test: ', testStatusTopic)
    }, [testStatusTopic])
    
    return(
        <div className={styles.container}>
            <div className={styles.header}>
                Test Configuration
            </div>
            <ActivePanel className={styles.form} disabled={isArmed}>
                {/* <Input label={'DAQ Rate'} units={'Hz'}>
                    <NumInput 
                        placeholder={0}
                        value={daqRate}
                        onChange={setdaqRate}
                        onSubmit={value => {
                            setdaqRate(value);
                            const _published = publish(MQTT.run.datarate, value);
                        }}
                    />
                </Input> */}
                {/* <Input label={'Subsample'} units={''}>
                    <Checkbox/>
                </Input> */}
                <Input label={'Engine Mode'}>
                    <DropdownSelect
                        options={["Cold", "Hot"]}
                        selected={engineMode}
                        placeholder={"Engine Mode"}
                        onSelect={(i) => {
                            setEngineMode(i);
                            const _published = publish(MQTT.run.enginemode, i);
                        }}
                        disabled={false}
                        className={styles.dropdown}
                    />
                </Input>
                <Input label={'Control Mode'}>
                    <DropdownSelect
                        options={["Open Loop", "Closed Loop"]}
                        selected={controlMode}
                        placeholder={"Control Mode"}
                        onSelect={(i) => {
                            setControlMode(i);
                            const _published = publish(MQTT.run.controlmode, i);
                        }}
                        disabled={false}
                        className={styles.dropdown}
                    />
                </Input>
                <Input label={'Run Duration'} units={'ms'}>
                    <NumInput
                        placeholder={0}
                        value={runDuration}
                        onChange={setRunDuration}
                        onSubmit={value => {
                            setRunDuration(value);
                            const _published = publish(MQTT.run.duration, value);
                        }}
                    />
                </Input>
                { (engineMode === 1) && <>
                    <Input label={'Igniter Voltage'} units={'V'}>
                        <NumInput
                            placeholder={0}
                            value={igniterVoltage}
                            onChange={setIgniterVoltage}
                            onSubmit={value => {
                                setIgniterVoltage(value);
                                const _published = publish(MQTT.run.igniter.voltage, value);
                            }}
                        />
                    </Input>
                    <Input label={'Pre-Ignition'} units={'ms'} >
                        <NumInput
                            placeholder={0}
                            value={preIgnitionDuration}
                            onChange={setPreIgnitionDuration}
                            onSubmit={value => {
                                setPreIgnitionDuration(value);
                                const _published = publish(MQTT.run.igniter.preburn, value);
                            }}
                        />
                    </Input>
                    <Input label={'Ignition Duration'} units={'ms'}>
                        <NumInput
                            placeholder={0}
                            value={ignitionDuration}
                            onChange={setIgnitionDuration}
                            onSubmit={value => {
                                setIgnitionDuration(value);
                                const _published = publish(MQTT.run.igniter.duration, value);
                            }}
                        />
                    </Input>
                </> }
                <div className={styles.keyframeContainer}>
                    <div className={styles.keyframeLabel}>Throttle Keyframes</div>
                    <MapValueInput
                        keyLabel={'MET (ms)'}
                        valueLabel={'Throttle (%)'}
                        valueMap={keyframeMap} 
                        setMapValue={updateKeyframeMapValue}
                        setMap={updateKeyframeMap}
                    />
                </div>
            </ActivePanel>
            <div className={styles.buttonContainer}>
                <ActivePanel className={styles.safeButtonContainer} disabled={isRunning}>
                    <PrimaryButton
                        disabled={!validConfiguration}
                        loading={false}
                        onClick={() => {
                            setIsArmed(_armed => !_armed)
                            const _published = publish(MQTT.run.arm, 1);
                        }}
                    >
                        { isArmed ? 'Disarm Engine' : 'Arm Engine' }
                    </PrimaryButton>
                </ActivePanel>
                <div className={styles.runButtonContainer}>
                    { isRunning && <p className={styles.cancel}>
                        Press any key to abort the test.
                    </p> }
                    <DestructiveButton
                        disabled={!isArmed || isRunning}
                        onClick={() => {
                            setIsRunning(true);
                            const _published = publish(MQTT.run.start, 1);
                        }}
                    >
                        Run Test
                    </DestructiveButton>
                </div>
            </div>
        </div>
    )
}

export default ConfigurationView