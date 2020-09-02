import React, { useState } from 'react';
import styles from './NumInput.module.scss';

const enterKeyCode = 13;

const NumInput = ({value, onChange, onSubmit, valid, disabled = false, int, width, className, ...props}) => {
    
    return (
        <input 
            type={"number"}
            value={value}
            disabled={disabled}
            onChange={(e) => {
                onChange(e.target.value);
            }}
            onKeyDown={(e) => {
                if (e.keyCode === enterKeyCode) {
                    onSubmit(e.target.value);
                    e.target.blur();
                }
            }}
            // onBlur={(e) => {
            //     onSubmit(e.target.value);
            // }}
            className={[
                className,
                styles.input,
                !valid && styles.invalid,
                disabled && styles.disabled
            ].join(" ")}
            style={{width: `${width || 64}px`}}
            {...props}
        />
    )
}

export default NumInput;