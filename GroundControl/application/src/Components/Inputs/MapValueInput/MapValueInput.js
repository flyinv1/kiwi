import React, { useState } from 'react';
import styles from './MapValueInput.module.scss';
import NumInput from '../NumInput/NumInput';

const MapValueInput = ({valueMap, setMapValue, setMap, keyLabel, valueLabel}) => {

    const [ newRowKey, setNewRowKey ] = useState('');
    const [ newRowValue, setNewRowValue ] = useState('');

    const [ editingRowIndex, setEditingRowIndex ] = useState('');
    const [ editingRowKey, setEditingRowKey ] = useState('');
    const [ editingRowValue, setEditingRowValue ] = useState('');

    const addMapRow = () => {
        if (newRowValue !== '' && newRowValue !== '') {
            console.log(newRowKey);
            setMapValue(newRowKey, newRowValue)
            setNewRowKey('');
            setNewRowValue('');
        }
    }

    const updateMapKey = (key, newKey) => {
        console.log(typeof(newKey))

        let _map = valueMap;
        if (key !== newKey && newKey !== '') {
            _map.set(newKey, _map.get(key));
            _map.delete(key);
            setMap(_map);
        } else {
            if (newKey === '') {
                _map.delete(key);
                setMap(_map);
            }
        }
        setEditingRowIndex(-1);
    }

    const updateMapValue = (key, value) => {
        console.log(editingRowValue, value)
        if (value !== '') {
            setMapValue(key, value);
        }
        setEditingRowIndex(-1);
    }

    return (
        <div className={styles.container}>
            <div className={styles.headerRow}>
                <div>{keyLabel}</div>
                <div>{valueLabel}</div>
            </div>
            {
                Array.from(valueMap.keys()).map((key, i) => { 
                    return (
                        <div className={styles.row} key={key}>
                            <NumInput
                                className={styles.keyInput}
                                onChange={value => {
                                    setEditingRowIndex(i);
                                    setEditingRowKey(value);
                                    setEditingRowValue(valueMap.get(key));
                                }}
                                onSubmit={value => updateMapKey(key, value)}
                                value={(i === editingRowIndex) ? editingRowKey : key}
                                width={96}
                                // className={styles.row}
                            />
                            <NumInput
                                className={styles.valueInput}
                                onChange={value => {
                                    setEditingRowIndex(i);
                                    setEditingRowKey(key);
                                    setEditingRowValue(value)
                                }}
                                onSubmit={value => updateMapValue(key, value)}
                                value={(i === editingRowIndex) ? editingRowValue : valueMap.get(key)}
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