import Line from './Line';
import Color from './Core/Color';

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

    setColor(color) {
        this.color = color;
        this.x.color = color;
        this.y.color = color;
    }



}
