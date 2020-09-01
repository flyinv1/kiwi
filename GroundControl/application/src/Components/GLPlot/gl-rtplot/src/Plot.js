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
        _computeLimits();
    }

    this.resize = () => {};

    this.setAxes = (axes) => {
        _axes = axes;
        _axes.x.line = this.renderer.add(_axes.x);
        _axes.y.line = this.renderer.add(_axes.y);
        _axes.setLimits(_limits.xmin, _limits.xmax, _limits.ymin, _limits.ymax);
    };

    this.setGrid = (grid) => {
        _grid = grid;
        _grid._x.map(line => this.renderer.add(line));
        _grid._y.map(line => this.renderer.add(line));
        _grid.setLimits(_limits.xmin, _limits.xmax, _limits.ymin, _limits.ymax);
    }

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
            this.renderer.render(_axes.x);
            this.renderer.render(_axes.y);
        }
        if (_grid) {
            _grid._x.map(line => { this.renderer.render(line) })
            _grid._y.map(line => { this.renderer.render(line) })
        }
        for (let series of Object.values(this.series)) {
            this.renderer.render(series);
        }
    };

    this.dispose = () => {
        this.renderer.disposeContext();
    };

    let _computeLimits = () => {

        const { xmin, xmax, ymin, ymax } = _limits;

        // The line shader computes the origin before scaling to fit the clipped region
        // Therefore the origin is scaled to the plot space - not the clip space
        // E.x. for x limits [0, 20], the origin offset is 10, not 0.5

        _axes.setLimits(xmin, xmax, ymin, ymax);
        _grid.setLimits(xmin, xmax, ymin, ymax);

        const { _origin, _scale } = limits(xmin, xmax, ymin, ymax);

        Object.values(this.series).map(series => {
            series.origin = _origin;
            series.scale = _scale;
        })
    }

}

export default Plot;
