import React, { useState, useMemo, useEffect } from 'react';
import styles from './ConfigurationView.module.scss';
import { Input, NumInput, Checkbox, Dropdown, DropdownSelect, ActivePanel } from '../../Components';
import { useTopic, usePublishJSON, useClientStatus } from '../../Hooks/MQTTProvider';
import MapValueInput from '../../Components/Inputs/MapValueInput/MapValueInput';
import { PrimaryButton, DestructiveButton } from '../../Components/Button/Button';
import { MQTT } from 'mqttKeys.js'

const clamp = (x, min, max) => Math.min(Math.max(x, min), max);

const ConfigurationView = () => {

    const publish = usePublishJSON()

    const [ throttlePosition, setThrottlePosition ] = useState(0);
    const [ encoderValue, setEncoderValue ] = useState(0);

    const [ controlMode, setControlMode ] = useState(-1);
    const [ engineMode, setEngineMode ] = useState(-1);
    const [ runDuration, setRunDuration ] = useState(0);
    const [ igniterVoltage, setIgniterVoltage ] = useState(0);
    const [ preIgnitionDuration, setPreIgnitionDuration ] = useState(0);
    const [ ignitionDuration, setIgnitionDuration ] = useState(0);

    const [ isArmed, setIsArmed ] = useState(false);
    const [ isRunning, setIsRunning ] = useState(false);

    const [ keyframeMap, setKeyFrameMap ] = useState(new Map());

    const clientStatus = useClientStatus();
    const mcuStatus = useTopic(MQTT.use.connected);
    const mcuState = useTopic(MQTT.use.state);

    const publishKeyFrameMap = () => {
        let _map = keyframeMap;
        let _arr = Array.from(_map);
        // console.log(_arr)
        // let _numArr = _arr.map(_subarr => [Number(_subarr[0]), Number(_subarr[1])]);
        let _numArr = _arr.reduce((_c, _subarr) => {
            _c.push(Number(_subarr[0]));
            _c.push(Number(_subarr[1]));
            return _c
        }, [])
        // console.log(_numArr);
        publish(MQTT.set.targets, _numArr);
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

    const mcuConnected = useMemo(() => {
        let connected = false;
        try {
            connected = JSON.parse(mcuStatus.payload);
        } catch(err) {}
        return connected;
    }, [mcuStatus])

    useEffect(() => {
        try {
            // Reset the GUI state to standby after test completion
            const state = JSON.parse(mcuState.payload);
            if (isRunning && state == 1) {
                setIsRunning(false);
                setIsArmed(false);
            }
        } catch(err) {

        }
    }, [ mcuState ])

    useEffect(() => {
        publishKeyFrameMap();
    }, [ keyframeMap ])

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
            const _published = publish(MQTT.cmd.stop);
            console.log('keypress -> published: ', _published)
            window.removeEventListener('keypress', windowListener)
        }
        if (isRunning) {
            window.addEventListener('keypress', windowListener);
        }
    }, [isRunning])
    
    return(
        <div className={styles.container}>
            <div className={styles.header}>
                Subsystem Control
            </div>
            <ActivePanel className={styles.control} disabled={isArmed || clientStatus.status !== 'connected' || !mcuConnected}>
                <Input label={'Throttle Position'}>
                    <NumInput
                        placeholder={0}
                        value={throttlePosition}
                        onChange={setThrottlePosition}
                        onSubmit={value => {
                            const _pos = clamp(value, 0, 660)
                            setThrottlePosition(_pos);
                            const _published = publish(MQTT.cmd.throttle.position, _pos);
                        }}
                    />
                </Input>
                <Input label={'Encoder Value'}>
                    <NumInput
                        placeholder={0}
                        value={encoderValue}
                        onChange={setEncoderValue}
                        onSubmit={value => {
                            const _val = clamp(value, 0, 660)
                            setEncoderValue(_val);
                            const _published = publish(MQTT.cmd.throttle.encoder, _val);
                        }}
                    />
                </Input>
            </ActivePanel>
            <div className={styles.header}>
                Test Configuration
            </div>
            <ActivePanel className={styles.form} disabled={isArmed || clientStatus.status !== 'connected' || !mcuConnected}>
                <Input label={'Engine Mode'}>
                    <DropdownSelect
                        options={["Cold", "Hot"]}
                        selected={engineMode}
                        placeholder={"Engine Mode"}
                        onSelect={(i) => {
                            setEngineMode(i);
                            const _published = publish(MQTT.set.enginemode, i);
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
                            const _published = publish(MQTT.set.controlmode, i);
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
                            const _published = publish(MQTT.set.duration, Number(value));
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
                                const _published = publish(MQTT.set.igniter.voltage, Number(value));
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
                                const _published = publish(MQTT.set.igniter.preburn, Number(value));
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
                                const _published = publish(MQTT.set.igniter.duration, Number(value));
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
                            const _published = isArmed ? publish(MQTT.cmd.disarm) : publish(MQTT.cmd.arm)
                            setIsArmed(_armed => !_armed)
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
                            const _published = publish(MQTT.cmd.start);
                            setIsRunning(true);
                        }}
                    >
                        Run test
                    </DestructiveButton>
                </div>
            </div>
        </div>
    )
}

export default ConfigurationView