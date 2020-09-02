import React from 'react';
import styles from './Input.module.scss';

const Input = ({label, units, children, short = false, ...props}) => {
    return(
        <div className={styles.container}>
            <span className={styles.label}>{label}</span>
            <div 
                className={short ? styles.col : styles.row}>
                {children}
                <span className={styles.units}>{units}</span>
            </div>
        </div>
    )
};

export default Input;