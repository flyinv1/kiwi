import React from 'react';
import styles from './Checkbox.module.scss';
import { ReactComponent as CheckIcon } from 'Assets/Icons/check.svg';

const Checkbox = ({ on, onClick, disabled, ...props }) => {
    return (
        <div
            tabIndex={0}
            onClick={() => {
                if (!disabled && onClick) {
                    onClick();
                }
            }}
            onKeyDown={(e) => {
                if (e.keyCode === 13 && !disabled && onClick) {
                    onClick();
                    e.target.blur();
                }
            }}
            className={[ styles.container, on && styles.on ].join(' ')}>
            <CheckIcon className={styles.check} />
        </div>
    );
};

export default Checkbox;
