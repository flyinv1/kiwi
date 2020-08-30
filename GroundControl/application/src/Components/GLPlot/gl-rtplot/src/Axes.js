import Line from './Line';
import Color from './Core/Color';

export default function Axes(color = new Color(0.8, 0.8, 0.8, 1.0)) {
    this.grid = null;

    this.x = {
        line: new Line(color, 2)
    };

    this.y = {
        line: new Line(color, 2)
    };

    this.x.line.buffer = new Float32Array([ -1, 0, 1, 0 ]);
    this.y.line.buffer = new Float32Array([ 0, -1, 0, 1 ]);
}
