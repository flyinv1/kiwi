import React, { Component } from 'react';
import PropTypes from 'prop-types';
import styles from './Button.module.scss';

const Button = ({ buttonStyle, className, children, onClick, disabled, loading, ...props }) => {
    return (
        <div
            onClick={() => {
                if (!disabled && onClick) onClick();
            }}
            className={[ styles.container, buttonStyle, className, disabled && styles.disabled ].join(' ')}>
            {children}
        </div>
    );
};

const PrimaryButton = ({ children, loading, loadingMessage, ...props }) => {
    return (
        <Button buttonStyle={styles.primary} {...props}>
            <p>{loading ? loadingMessage ? loadingMessage : 'loading...' : children}</p>
        </Button>
    );
};

const SecondaryButton = ({ children, loading, loadingMessage, ...props }) => {
    return (
        <Button buttonStyle={styles.secondary} {...props}>
            <p>{loading ? loadingMessage ? loadingMessage : 'loading...' : children}</p>
        </Button>
    );
};

const TextButton = ({ children, loading, loadingMessage, ...props }) => {
    return (
        <button className={styles.textButton} {...props}>
            <p>{loading ? loadingMessage ? loadingMessage : 'loading...' : children}</p>
        </button> 
    )
}

const DestructiveButton = ({ children, ...props }) => {
    return (
        <Button buttonStyle={styles.destructive} {...props}>
            <p>{children}</p>
        </Button>
    );
};

const IconButton = ({ children, on, ...props }) => {
    return (
        <Button className={[ styles.iconButton, on && styles.iconButtonOn ].join(' ')} {...props}>
            {children}
        </Button>
    );
};

export { Button as default, PrimaryButton, SecondaryButton, DestructiveButton, IconButton, TextButton };
