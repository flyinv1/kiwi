import Line from './Line';
import Color from './Core/Color';

class Grid {

    constructor(xIntervals, yIntervals) {
        this.majorColor = new Color(0.7, 0.7, 0.7, 1.0);
        this.minorColor = new Color(0.9, 0.9, 0.9, 1.0);

        this._x = [];
        this._y = [];

        for (let i = 0; i < xIntervals + 1; i++) {
            let line = new Line(2);
            line.color = this.majorColor;
            line.buffer = new Float32Array([ i / xIntervals, -1, i / xIntervals, 1 ]);
            this._x.push(line);
        }

        for (let i = 0; i < yIntervals + 1; i++) {
            let line = new Line(2);
            line.color = this.majorColor;
            line.buffer = new Float32Array([-1, i / yIntervals, 1, i / yIntervals]);
            this._y.push(line);
        }
    }

    setMinorIntervals(xMinorIntervals, yMinorIntervals) {
        for (let i = 0; i < xMinorIntervals + 1; i++) {
            let line = new Line(2);
            line.color = this.minorColor;
            line.buffer = new Float32Array([ i / xMinorIntervals, -1, i / xMinorIntervals, 1 ]);
            this._x.push(line);
        }
        for (let i = 0; i < yMinorIntervals + 1; i++) {
            let line = new Line(2);
            line.color = this.minorColor;
            line.buffer = new Float32Array([ -1,  i / yMinorIntervals, 1, i / yMinorIntervals ]);
            this._x.push(line);
        }
    }

    setLimits({xmin, xmax, ymin, ymax}) {

        const _delta = {
            x: xmax - xmin,
            y: ymax - ymin
        };

        const _origin = {
            x: xmin + _delta.x / 2,
            y: ymin + _delta.y / 2
        };

        const _scale = {
            x: 2 / _delta.x,
            y: 2 / _delta.y
        }

        this._x.map(line => {
            line.scale = _scale
            line.origin = _origin
        });

        this._y.map(line => {
            line.scale = _scale
            line.origin = _origin
        })

    }
}

export default Grid;
