import Renderer from './Renderer';

/**
 * @param {HTMLCanvasElement} canvas
 * @param {Object} properties Webgl renderer properties
 * @description Creates a new instance of Plot
 */

function Plot(canvas, properties) {
    let _axes;

    let renderer = new Renderer({
        canvas: canvas,
        ...properties
    });

    this.lines = [];
    this.axes = null;
    this.autoScale = false;

    this.renderer = renderer;

    this.addStream = (line) => {
        line = this.renderer.instantiateObject(line);
        this.lines.push(line);
    };

    this.setScale = (x, y) => {
        // this.renderer.setScale(x, y);
    };

    this.resize = () => {};

    this.attachAxes = (axes) => {
        _axes = axes;
        _axes.x.line = this.renderer.instantiateObject(_axes.x.line);
        _axes.y.line = this.renderer.instantiateObject(_axes.y.line);
    };

    this.render = () => {
        if (_axes) {
            this.renderer.renderObject(_axes.x.line);
            this.renderer.renderObject(_axes.y.line);
        }
        this.lines.map((line) => {
            this.renderer.renderObject(line);
        });
    };

    this.dispose = () => {
        this.lines = [];
        this.renderer.disposeContext();
    };
}

export default Plot;
