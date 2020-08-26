const { BrowserWindow, app } = require("electron");

function ApplicationManager() {
    let window;

    // this.store = new Store(`${app.getPath('userData')}/kiwi/application.json`);

    const launch = () => {
        window = new BrowserWindow({
        });
    }

    this.onReady = async () => {

    }

    this.onWillFinishLaunching = async () => {

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