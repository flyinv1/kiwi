import React, { useState, useMemo, useEffect } from 'react';
import styles from './ConfigurationView.module.scss';
import { Input, NumInput, Checkbox, Dropdown, DropdownSelect, ActivePanel } from '../../Components';
import { useTopic, usePublishJSON } from '../../Hooks/MQTTProvider';
import MapValueInput from '../../Components/Inputs/MapValueInput/MapValueInput';
import { PrimaryButton, DestructiveButton } from '../../Components/Button/Button';

const ConfigurationView = () => {

    const publish = usePublishJSON()

    const [ daqRate, setdaqRate ] = useState(0); 
    const [ controlMode, setControlMode ] = useState(-1);
    const [ engineMode, setEngineMode ] = useState(-1);
    const [ runDuration, setRunDuration ] = useState(0);
    const [ igniterVoltage, setIgniterVoltage ] = useState(0);
    const [ preIgnitionDuration, setPreIgnitionDuration ] = useState(0);
    const [ ignitionDuration, setIgnitionDuration ] = useState(0);

    const [ isArmed, setIsArmed ] = useState(false);
    const [ isRunning, setIsRunning ] = useState(false);

    const [ keyframeMap, setKeyFrameMap ] = useState(new Map());

    const testStatusTopic = useTopic('run/status');

    const updateKeyframeMap = (key, value) => {
        const _value = Math.min(Math.max(0, value), 100);
        let _map = new Map(keyframeMap.set(key, _value));
        let _sortedMap = new Map([..._map].sort((a, b) => {
            return a[0] > b[0] ? 1 : -1;
        }))
        setKeyFrameMap(_sortedMap);
    }

    useEffect(() => {

        const windowListener = (e) => {
            console.log(e);
            setIsRunning(false);
        }

        window.addEventListener('keypress', windowListener);

        console.log('isRunning')
        return(() => {
            window.removeEventListener('keypress', windowListener)
        })
    }, [isRunning])
    
    return(
        <div className={styles.container}>
            <div className={styles.header}>
                Test Configuration
            </div>
            <ActivePanel className={styles.form} disabled={isArmed}>
                <Input label={'DAQ Rate'} units={'Hz'}>
                    <NumInput 
                        placeholder={0}
                        value={daqRate}
                        onChange={setdaqRate}
                        onSubmit={value => {
                            setdaqRate(value);
                            const _published = publish('run/daqrate', value);
                        }}
                    />
                </Input>
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
                            const _published = publish('run/enginemode', i);
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
                            const _published = publish('run/controlmode', i);
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
                            const _published = publish('run/duration', value);
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
                                const _published = publish('run/igniter/voltage', value);
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
                                const _published = publish('run/igniter/preburn', value);
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
                                const _published = publish('run/igniter/duration', value);
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
                        setMap={updateKeyframeMap}
                    />
                </div>
            </ActivePanel>
            <div className={styles.buttonContainer}>
                <ActivePanel className={styles.safeButtonContainer} disabled={isRunning}>
                    <PrimaryButton
                        disabled={false}
                        loading={false}
                        onClick={() => {
                            setIsArmed(_armed => !_armed)
                            const _published = publish('run/arm');
                        }}
                    >
                        { isArmed ? 'Disarm Engine' : 'Arm Engine' }
                    </PrimaryButton>
                </ActivePanel>
                <div className={styles.runButtonContainer}>
                    { isRunning && <p className={styles.cancel}>
                        Press enter to abort the test.
                    </p> }
                    <DestructiveButton
                        disabled={!isArmed || isRunning}
                        onClick={() => {
                            setIsRunning(true);
                            const _published = publish('run/start');
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