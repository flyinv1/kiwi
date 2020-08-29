const { BrowserWindow, app } = require("electron");

function ApplicationManager() {
    let window;

    // this.store = new Store(`${app.getPath('userData')}/kiwi/application.json`);

    const launch = () => {
        window = new BrowserWindow({
            width: 800,
            height: 1080,
            webPreferences: {
                ...this.webPreferences
            }
        });
        window.loadURL('http://localhost:3000/index.html')
    }

    this.onReady = async (event) => {
        launch();
    }

    this.onWillFinishLaunching = async () => {

    }

    this.onWindowAllClosed = async () => {

    }

    this.onActivate = async () => {
        
    }

    this.onBeforeQuit = async () => {

    }

    this.onUserQuit = async () => {
        app.quit();
    }

    this.onApplicationEvent = async (event, { channel, payload }) => {

    }

}

module.exports = ApplicationManager