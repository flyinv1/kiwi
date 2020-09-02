import React, { useState, useRef, useEffect } from 'react';
import styles from './DropdownSelect.module.scss';
import Dropdown from 'Components/Core/Dropdown/Dropdown';
import { ReactComponent as ArrowIcon } from 'Assets/Icons/arrow-down.svg';

const DropdownSelect = ({
    options = [],
    selected = -1,
    disabled = false,
    placeholder = 'Select',
    onSelect = () => {},
    label,
    className,
    ...props
}) => {
    const wrapper = useRef();

    return (
        <Dropdown className={[ styles.container, className ].join(' ')}>
            {(open, setOpen) => {
                return (
                    <div
                        tabIndex={0}
                        className={styles.wrapper}
                        ref={wrapper}
                        onClick={() =>
                            setOpen((prevOpen) => {
                                if (!disabled) return !prevOpen;
                            })}
                        onKeyDown={(e) => {
                            if (e.keyCode === 13 && !disabled) {
                                setOpen((prevOpen) => !prevOpen)
                            }
                        }}
                            
                            >
                        <div className={[ styles.selected, open && styles.selectedOpen ].join(' ')}>
                            {selected > -1 ? (
                                <span className={styles.selection}>{options[selected]}</span>
                            ) : (
                                <span className={styles.placeholder}>{placeholder}</span>
                            )}
                            <ArrowIcon />
                        </div>
                        {open &&
                        options.length > 0 && (
                            <div
                                className={styles.dropdown}
                                style={{
                                    top: Math.ceil(wrapper.current.offsetTop + wrapper.current.clientHeight),
                                    left: Math.floor(wrapper.current.offsetLeft),
                                    minWidth: wrapper.current.clientWidth
                                }}>
                                {label && <span>{label}</span>}
                                {options.map((option, i) => {
                                    return (
                                        <div
                                            className={styles.dropdownItem}
                                            key={i}
                                            onClick={() => {
                                                onSelect(i);
                                            }}>
                                            {option}
                                        </div>
                                    );
                                })}
                            </div>
                        )}
                    </div>
                );
            }}
        </Dropdown>
    );
};

export default DropdownSelect;
