import React from 'react';
import styles from './HeaderView.module.scss';

const HeaderView = ({status}) => {
    return(
        <div className={styles.container}>
            <div className={styles.title}>Kiwi Ground Control</div>
            <StatusIndicator status={status}/>
        </div>
    )
}

const StatusIndicator = ({status}) => {

    const statusStyle = {
        'connected': styles.connected,
        'closed': styles.closed,
        '': null
    }

    return(
        <div className={[statusStyle[status] || '', styles.statusIndicator].join(' ')}>
            {status}
        </div>
    )
}

export default HeaderView