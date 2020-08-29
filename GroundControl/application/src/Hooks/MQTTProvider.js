import React, { useEffect, useState, createContext, useContext, useMemo, useCallback } from 'react';

const MQTTContext = createContext();

const MQTTProvider = ({ config, children }) => {

    const [ status, setStatus ] = useState('offline')
    const [ client, setClient ] = useState(null);
    const [ topics, setTopics ] = useState([]);

    const updateTopic = useCallback(({ topic, message }) => {
        setTopics(prevTopics => {
            return {
                ...prevTopics,
                [topic]: message
            }
        })
    }, [])

    useEffect(() => {

        let mqttClient = window.kiwi.mqtt.connect(config);
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

    }, [])

    return(
        <MQTTContext.Provider value={{
            status,
            client,
            topics,
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

export default MQTTProvider