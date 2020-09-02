import Renderer from './Renderer/Renderer';
import StreamLine from './StreamLine';
import Line from './Line';
import { limits } from './Core/Limits';

/**
 * @param {HTMLCanvasElement} canvas
 * @param {Object} properties Webgl renderer properties
 * @description Creates a new instance of Plot
 */

function Plot(canvas, properties) {
    let _axes = null;
    let _grid = null;

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


    this.series = {};
    this.renderer = renderer;

    this.renderer.resize();

    this.addSeries = (name, series) => {
        this.series[name] = this.renderer.add(series);
        this.layout();
    }

    this.resize = () => {
        this.renderer.resize();
    };

    this.setAxes = (axes) => {
        _axes = axes;
        this.layout();
    };

    this.setGrid = (grid) => {
        _grid = grid;
        this.layout();
    }

    this.setXLimit = (xmin, xmax) => {
        _limits = {
            ..._limits,
            xmin: xmin,
            xmax: xmax
        }
        this.layout();
    };

    this.setYLimit = (ymin, ymax) => {
        _limits = {
            ..._limits,
            ymin: ymin,
            ymax: ymax
        }
        this.layout();
    }

    this.setLimits = (xmin, xmax, ymin, ymax) => {
        _limits = {
            xmin: xmin,
            xmax: xmax,
            ymin: ymin,
            ymax: ymax,
        };
        this.layout();
    }

    this.render = () => {
        if (_grid) {
            _grid._x.map(line => { this.renderer.render(line) })
            _grid._y.map(line => { this.renderer.render(line) })
        }
        if (_axes) {
            this.renderer.render(_axes.x);
            this.renderer.render(_axes.y);
        }
        for (let series of Object.values(this.series)) {
            this.renderer.render(series);
        }
    };

    this.dispose = () => {
        // this.renderer.disposeContext();
    };

    this.layout = () => {
        const { xmin, xmax, ymin, ymax } = _limits;
        const { origin, scale } = limits(xmin, xmax, ymin, ymax);

        if (_axes) {
            _axes.setLimits(xmin, xmax, ymin, ymax);
            _axes.x.line = this.renderer.add(_axes.x);
            _axes.y.line = this.renderer.add(_axes.y);
        }

        if (_grid) {
            _grid.setLimits(xmin, xmax, ymin, ymax);
            _grid._x.map(line => this.renderer.add(line));
            _grid._y.map(line => this.renderer.add(line));
        }

        Object.values(this.series).map(series => {
            series.origin = origin;
            series.scale = scale;
        })
    }

}

export default Plot;
