import React, { useState, useEffect } from 'react';
import styles from './Collapsible.module.scss';
import { ReactComponent as ArrowIcon } from 'Assets/Icons/arrow-down.svg';

const Collapsible = ({ target, disabled, children, header, footer, initialState, className, ...props }) => {
    const [ open, setOpen ] = useState(initialState);
    return (
        <div className={className}>
            {target(open, setOpen)}
            {children && header(open, setOpen)}
            {open && children}
            {children && footer(open, setOpen)}
        </div>
    );
};

const CollapsibleSection = ({ title, onClick, disabled, footer, header, ...props }) => {
    return (
        <Collapsible
            className={styles.sectionContainer}
            target={(open, setOpen) => (
                <div className={styles.sectionTarget} onClick={() => setOpen((prev) => !prev)}>
                    <span>{title}</span>
                    <ArrowIcon
                        className={styles.arrow}
                        style={{ transform: open ? 'rotate(0deg)' : 'rotate(-90deg)' }}
                    />
                </div>
            )}
            header={(open, setOpen) => {
                return open ? header === true ? <div className={styles.header} /> : header || '' : '';
            }}
            footer={(open, setOpen) => {
                return open ? footer === true ? <div className={styles.footer} /> : footer || '' : '';
            }}
            {...props}
        />
    );
};

const CollapsibleSubsection = ({ title, onClick, disabled, footer, header, ...props }) => {
    return (
        <Collapsible
            className={styles.subsectionContainer}
            target={(open, setOpen) => (
                <div className={styles.subsectionTarget} onClick={() => setOpen((prev) => !prev)}>
                    <ArrowIcon
                        className={styles.arrow}
                        style={{ transform: open ? 'rotate(0deg)' : 'rotate(-90deg)' }}
                    />
                    <span>{title}</span>
                </div>
            )}
            header={(open, setOpen) => {
                return open ? header === true ? <div className={styles.header} /> : header || '' : '';
            }}
            footer={(open, setOpen) => {
                return open ? footer === true ? <div className={styles.footer} /> : footer || '' : '';
            }}
            {...props}
        />
    );
};

export { Collapsible as default, CollapsibleSection, CollapsibleSubsection };
