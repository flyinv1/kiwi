import React from 'react';
import ReactDOM from 'react-dom';
import App from './Views/App/App';
import { Provider } from 'react-redux';
import { configureStore } from '@reduxjs/toolkit';
import MQTTClient from './Store/mqtt';
import MQTTProvider from './Hooks/MQTTProvider';
import { app } from './Store/reducer';
import './index.css';

const config = {
  host: '192.168.1.10',
  port: 1883
}

ReactDOM.render(
    <MQTTProvider config={config}>
      <App/>
    </MQTTProvider>,
  document.getElementById('root')
);
