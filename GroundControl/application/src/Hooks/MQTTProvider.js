import React, { useEffect, useState, createContext, useContext, useMemo, useCallback } from 'react';

const MQTTContext = createContext();


const MQTTProvider = ({ config, children }) => {

    const [ status, setStatus ] = useState('offline')
    const [ client, setClient ] = useState(null);
    const [ topics, setTopics ] = useState({});

    const updateTopic = useCallback(({ topic, message }) => {

        setTopics(prevTopics => {
            return {
                ...prevTopics,
                [topic]: message
            }
        })
    }, [])

    useEffect(() => {

        let mqttClient = window.kiwi.mqttConnectClient(config);
        setClient(mqttClient);
        mqttClient.on('connect', () => setStatus('connected'));
        mqttClient.on('reconnect', () => setStatus('reconnecting'));
        mqttClient.on('close', () => setStatus('closed'));
        mqttClient.on('offline', () => setStatus('offline'));
        mqttClient.on('message', (topic, message) => {
            updateTopic({
                topic: topic,
                message: message.toString()
            })
        })

        return () => {
            client.end();
        }

    }, [ config ])

    return(
        <MQTTContext.Provider value={{
            status,
            client,
            topics
        }}>
            {children}
        </MQTTContext.Provider>
    )
}

export const useClientStatus = () => {

    const { status } = useContext(MQTTContext)
    
    return {
        status
    };
}

export const useTopic = (topic) => {

    const { status, client, topics } = useContext(MQTTContext)

    useEffect(() => {
        if (client && status === 'connected') client.subscribe(topic);
    }, [status])

    return {
        topic,
        payload: topics[topic],
        status,
    }
}

export const usePublishJSON = () => {

    const { client } = useContext(MQTTContext);

    return (topic, payload) => {
            try {
                const _payload = JSON.stringify(payload);
                client.publish(topic, _payload)
                return true;
            } catch(err) {
                return false;
            }
        }
}

export default MQTTProvider