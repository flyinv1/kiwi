import Line from './Line';
import Color from './Core/Color';
import { limits } from './Core/Limits';

export default class Axes {

    constructor() {
        this.color = new Color(0.7, 0.7, 0.7, 1.0);

        this.x = new Line(2)
        this.y = new Line(2)

        this.x.color = this.color;
        this.y.color = this.color;

        this.x.buffer = new Float32Array([-1, 0, 1, 0]);
        this.y.buffer = new Float32Array([0, -1, 0, 1]);
    }

    setLimits(xmin, xmax, ymin, ymax) {

        const { _scale, _origin } = limits(xmin, xmax, ymin, ymax);

        this.x.scale = _scale;
        this.x.origin = _origin;

        this.y.scale = _scale;
        this.y.origin = _origin;
    }

    setColor(color) {
        this.color = color;
        this.x.color = color;
        this.y.color = color;
    }



}
