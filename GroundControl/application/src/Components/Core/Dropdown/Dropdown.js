import React, { useEffect, useState, useRef } from 'react';

const Dropdown = ({ className, children, ...props }) => {
    let container = useRef();

    const [ open, setOpen ] = useState(false);

    useEffect(
        () => {
            let closeFunc = (e) => {
                if (!container.current.contains(e.target)) setOpen(false);
            };
            window.addEventListener('mousedown', closeFunc);
            return () => window.removeEventListener('mousedown', closeFunc);
        },
        [ container ]
    );

    return (
        <div className={className} ref={container}>
            {children(open, setOpen)}
        </div>
    );
};

export default Dropdown;
