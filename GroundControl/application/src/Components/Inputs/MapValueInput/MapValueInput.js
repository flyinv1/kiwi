import React, { useState } from 'react';
import styles from './MapValueInput.module.scss';
import NumInput from '../NumInput/NumInput';

const MapValueInput = ({valueMap, setMap, keyLabel, valueLabel}) => {

    const [ newRowKey, setNewRowKey ] = useState('');
    const [ newRowValue, setNewRowValue ] = useState('');

    const addMapRow = () => {
        if (!isNaN(newRowValue) && !isNaN(newRowValue)) {
            console.log('yes');
            setNewRowKey('');
            setNewRowValue('');
            setMap(newRowKey, newRowValue)
        }
    }

    const updateMapKey = (key) => {
        console.log(key);
    }

    const updateMapValue = (key, value) => {

    }

    return (
        <div className={styles.container}>
            <div className={styles.headerRow}>
                <div>{keyLabel}</div>
                <div>{valueLabel}</div>
            </div>
            {
                Array.from(valueMap.keys()).map((key, i) => { 
                    console.log(key)
                    return (
                        <div className={styles.row} key={key}>
                            <NumInput
                                className={styles.keyInput}
                                onChange={() => {}}
                                onSubmit={updateMapKey}
                                value={key}
                                width={96}
                                // className={styles.row}
                            />
                            <NumInput
                                className={styles.valueInput}
                                onChange={() => {}}
                                onSubmit={value => updateMapValue(key, value)}
                                value={valueMap.get(key)}
                                width={96}
                                // className={styles.row}
                            />
                        </div>
                    )
                })
            }
            <div className={styles.row}>
                <NumInput
                    value={newRowKey}
                    className={styles.keyInput}
                    onChange={setNewRowKey}
                    onSubmit={addMapRow}
                    width={96}
                />
                <NumInput
                    value={newRowValue}
                    className={styles.valueInput}
                    onChange={setNewRowValue}
                    onSubmit={addMapRow}
                />
            </div>
        </div>
    )
}

export default MapValueInput;