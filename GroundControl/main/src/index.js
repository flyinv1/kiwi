const { app, ipcMain } = require('electron');
const path = require('path');
const ApplicationManager = require('./ApplicationManager');

const webPreferences = {
    allowRunningInsecureContent: false,
    // contextIsolation: true,
    enableRemoteModule: false,
    nativeWindowOpen: false,
    nodeIntegration: false,
    nodeIntegrationWorker: false,
    nodeIntegrationInSubFrames: false,
    safeDialogs: true,
    // sandbox: true,
    webSecurity: true,
    webviewTag: false,
    preload: path.join(__dirname, 'preload.js')
}

let applicationManager = new ApplicationManager();
applicationManager.webPreferences = webPreferences;

app.on('ready', applicationManager.onReady);
app.on('will-finish-launching', applicationManager.onWillFinishLaunching);
app.on('window-all-closed', applicationManager.onWindowAllClosed);
app.on('activate', applicationManager.onActivate);
app.on('before-quit', applicationManager.onBeforeQuit);
// ipcMain.on()