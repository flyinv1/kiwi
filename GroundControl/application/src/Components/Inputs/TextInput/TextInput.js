import React, { useState } from 'react';
import styles from './TextInput.module.scss';

const enterKeyCode = 13;

const TextInput = ({value, onChange, onSubmit, valid, disabled = false, width, ...props}) => {
    
    return (
        <input 
            type={"text"}
            value={value}
            disabled={disabled}
            onChange={(e) => {
                onChange(e.target.value);
            }}
            // onBlur={(e) => {
            //     onSubmit(e.target.value);
            // }}
            onKeyDown={(e) => {
                if (e.keyCode === enterKeyCode) {
                    onSubmit(e.target.value);
                    e.target.blur();
                }
            }}
            className={[
                styles.input,
                !valid && styles.invalid,
                disabled && styles.disabled
            ].join(" ")}
            style={{width: `${width || 256}px`}}
            {...props}
        />
    )
}

export default TextInput;