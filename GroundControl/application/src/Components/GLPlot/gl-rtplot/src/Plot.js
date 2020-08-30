import Renderer from './Renderer';

/**
 * @param {HTMLCanvasElement} canvas
 * @param {Object} properties Webgl renderer properties
 * @description Creates a new instance of Plot
 */

function Plot(canvas, properties) {
    let _axes = null;
    let _limits = {
        xmin: -1,
        xmax: 1,
        ymin: -1,
        ymax: 1
    };

    let renderer = new Renderer({
        canvas: canvas,
        ...properties
    });

    this.lines = [];

    this.renderer = renderer;
    this.renderer.resize();

    this.addStream = (line) => {
        line = this.renderer.add(line);
        this.lines.push(line);
        _computeLimits();
    };

    this.addStreams = (lines) => {
        lines.map((line) => {
            line = this.renderer.add(line);
        })
        this.lines = [...this.lines, ...lines];
        _computeLimits();
    }

    this.resize = () => {};

    this.attachAxes = (axes) => {
        _axes = axes;
        _axes.x.line = this.renderer.add(_axes.x.line);
        _axes.y.line = this.renderer.add(_axes.y.line);
    };

    this.setXLimit = (xmin, xmax) => {
        _limits = {
            ..._limits,
            xmin: xmin,
            xmax: xmax
        }
        _computeLimits();
    };

    this.setYLimit = (ymin, ymax) => {
        _limits = {
            ..._limits,
            ymin: ymin,
            ymax: ymax
        }
        _computeLimits();
    }

    this.setLimits = (xmin, xmax, ymin, ymax) => {
        _limits = {
            xmin: xmin,
            xmax: xmax,
            ymin: ymin,
            ymax: ymax,
        };
        _computeLimits();
    }

    this.render = () => {
        if (_axes) {
            this.renderer.render(_axes.x.line);
            this.renderer.render(_axes.y.line);
        }
        this.lines.map((line) => {
            this.renderer.render(line);
        });
    };

    this.dispose = () => {
        this.lines = [];
        this.renderer.disposeContext();
    };

    let _computeLimits = () => {

        const { xmin, xmax, ymin, ymax } = _limits;
        console.log(xmin, xmax, ymin, ymax);

        const _origin = {
            x: 2 * xmin / (xmax - xmin) + 1,
            y: 2 * ymin / (ymax - xmin) + 1
        }

        const _scale = {
            x: 2 / (xmax - xmin),
            y: 2 / (ymax - ymin)
        }
            
        if (_axes) {

            _axes.x.line.buffer = new Float32Array([xmin, 0, xmax, 0]);
            _axes.y.line.buffer = new Float32Array([0, ymin, 0, ymax]);

            _axes.x.line.origin = _origin;
            _axes.y.line.origin = _origin;
            _axes.x.line.scale = { x: 1, y: 1 };
            _axes.y.line.scale = { x: 1, y: 1 };
        }
 
        this.lines.map((line) => {
            line.origin = _origin;
            line.scale = _scale;
        })
    }

}

export default Plot;
