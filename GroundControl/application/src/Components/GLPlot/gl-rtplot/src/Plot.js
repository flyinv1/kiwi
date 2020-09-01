import Renderer from './Renderer/Renderer';
import StreamLine from './StreamLine';
import Line from './Line';

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

    this.lines = []
    this.series = {};
    this.renderer = renderer;

    this.renderer.resize();

    this.addSeries = (name, series) => {
        this.series[name] = this.renderer.add(series);
        _computeLimits();
    }

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

    this.addAxes = (axes) => {
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
        this.renderer.render(_axes);
        for (let series of Object.values(this.series)) {
            this.renderer.render(series);
        }
    };

    this.dispose = () => {
        this.lines = [];
        this.lines.map((line) => line.dispose());
        this.renderer.disposeContext();
    };

    let _computeLimits = () => {

        const { xmin, xmax, ymin, ymax } = _limits;

        // The line shader computes the origin before scaling to fit the clipped region
        // Therefore the origin is scaled to the plot space - not the clip space
        // E.x. for x limits [0, 20], the origin offset is 10, not 0.5

        const _origin = {
            x: xmin + (xmax - xmin) / 2,
            y: ymin + (ymax - ymin) / 2
        }

        const _scale = {
            x: 2 / (xmax - xmin),
            y: 2 / (ymax - ymin)
        }
            
        if (_axes) _axes.setLimits(_limits, _origin, _scale);

        this.lines.map((line) => {
            line.origin = _origin;
            line.scale = _scale;
        })

        Object.values(this.series).map(series => {
            series.origin = _origin;
            series.scale = _scale;
        })
    }

}

export default Plot;
