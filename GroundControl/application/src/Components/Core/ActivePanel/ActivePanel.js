import React from 'react';
import styles from './ActivePanel.module.scss';

const ActivePanel = ({ disabled, children, className, ...props }) => {
    return <div className={[ className, disabled ? styles.disabled : '' ].join(' ')}>{children}</div>;
};

export default ActivePanel;
