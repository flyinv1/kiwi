import Line from './Line';
import Color from './Core/Color';
import { limits } from './Core/Limits';

class Grid {

    constructor(xInterval, yInterval, xmin = -1, xmax = 1, ymin = -1, ymax = 1) {
        this.majorColor = new Color(0.8, 0.8, 0.8, 1.0);
        this.minorColor = new Color(0.2, 0.9, 0.9, 1.0);

        this._x = [];
        this._y = [];

        this.xInterval = xInterval;
        this.yInterval = yInterval;

        this.setLimits(xmin, xmax, ymin, ymax);
    }

    setLimits(xmin, xmax, ymin, ymax) {

        const { scale, origin } = limits(xmin, xmax, ymin, ymax);

        // round to nearest power of 10
        const mod_x = Math.pow(10, Math.ceil(Math.log10(1 / scale.x)));

        const xIntervals = Math.ceil((xmax - xmin) / this.xInterval);
        const yIntervals = Math.ceil((ymax - ymin) / this.yInterval);

        this._x = [];
        this._y = [];

        for (let i = 0; i < xIntervals + 1; i++) {
            let line = new Line(2);
            line.color = this.majorColor;
            line.scale = scale;
            line.origin = origin;
            line.buffer = new Float32Array([ 
                xmin - xmin % this.xInterval + i * this.xInterval, 
                ymin, 
                xmin - xmin % this.xInterval + i * this.xInterval, 
                ymax
            ]);
            this._x.push(line);
        }

        for (let i = 0; i < yIntervals + 1; i++) {
            let line = new Line(2);
            line.color = this.majorColor;
            line.scale = scale;
            line.origin = origin;
            line.buffer = new Float32Array([
                xmin,
                ymin - ymin % this.yInterval + i * this.yInterval,
                xmax,
                ymin - ymin % this.yInterval + i * this.yInterval
            ]);
            this._y.push(line);
        }

    }
}

export default Grid;
