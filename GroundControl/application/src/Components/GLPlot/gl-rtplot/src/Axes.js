import Line from './Line';
import Color from './Core/Color';
import { limits } from './Core/Limits';

export default class Axes {

    constructor() {
        this.color = new Color(0.6, 0.6, 0.6, 1.0);

        this.x = new Line(2)
        this.y = new Line(2)

        this.x.color = this.color;
        this.y.color = this.color;

        this.x.buffer = new Float32Array([-1, 0, 1, 0]);
        this.y.buffer = new Float32Array([0, -1, 0, 1]);
    }

    setLimits(xmin, xmax, ymin, ymax) {

        const { scale, origin } = limits(xmin, xmax, ymin, ymax);

        this.x.color = this.color;
        this.x.scale = scale;
        this.x.origin = origin;
        this.x.buffer = new Float32Array([ xmin, 0, xmax, 0 ]);

        this.y.color = this.color;
        this.y.scale = scale;
        this.y.origin = origin;
        this.y.buffer = new Float32Array([ 0, ymin, 0, ymax ]);

    }

    setScale(scale) {
        this.x.scale = scale;
        this.y.scale = scale;
    }

    setOrigin(origin) {
        this.x.origin = origin;
        this.y.origin = origin;
    }

    setColor(color) {
        this.color = color;
        this.x.color = color;
        this.y.color = color;
    }



}
