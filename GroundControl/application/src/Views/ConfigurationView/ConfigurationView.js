import React, { useState } from 'react';
import styles from './ConfigurationView.module.scss';
import { Input, NumInput, Checkbox, Dropdown, DropdownSelect } from '../../Components';
import { useTopic, usePublishJSON } from '../../Hooks/MQTTProvider';

const ConfigurationView = () => {

    const publish = usePublishJSON()

    const [ controlMode, setControlMode ] = useState(-1);
    const [ engineMode, setEngineMode ] = useState(-1);
    const [ runDuration, setRunDuration ] = useState(0);
    const [ igniterVoltage, setIgniterVoltage ] = useState(0);
    const [ preIgnitionDuration, setPreIgnitionDuration ] = useState(0)

    return(
        <div className={styles.container}>
            <Input label={'DAQ Rate'} units={'Hz'}>
                <NumInput 
                    placeholder={0}
                    onChange={() => {}}
                    onSubmit={() => {}}
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
            <Input label={'Run Duration'}>
                <NumInput
                    placeholder={0}
                    value={runDuration}
                    onChange={setRunDuration}
                    onSubmit={value => {
                        console.log(value)
                        setRunDuration(value);
                        const _published = publish('run/duration', value);
                    }}
                />
            </Input>
            { (engineMode === 1) && 
                <Input label={'Igniter Voltage'}>
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
            }
            { (engineMode === 1) &&
                <Input label={'Pre-Ignition'}>
                    <NumInput
                        placeholder={0}
                        value={preIgnitionDuration}
                        onChange={setPreIgnitionDuration}
                        onSubmit={value => {
                            setPreIgnitionDuration(value);
                            const _published = publish('run/igniter/pre', value);
                        }}
                    />
                </Input>
            }
            <Input label={'Ignition Duration'}>
                <NumInput
                    placeholder={0}
                    onChange={() => {}}
                    onSubmit={() => {}}
                />
            </Input>
        </div>
    )
}

export default ConfigurationView