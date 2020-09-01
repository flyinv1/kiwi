import CoreObject from './Core/CoreObject';
import OBJECT from './Core/Objects';
import Color from './Core/Color';

class Line extends CoreObject {

	constructor(points) {
		super();
		this.color = new Color(0, 0, 0, 1.0);
		this.buffer = new Float32Array(2 * points);
		this.bufferSize = points;
		this.type = OBJECT.LINE
	}

	setX = (i, x) => {
		this.buffer[i * 2] = x;
	};

	setY = (i, y) => {
		this.buffer[i * 2 + 1] = y;
	};

	getX = (i) => this.buffer[i * 2];

	getY = (i) => this.buffer[i * 2 + 1];

	fillY(y) {
		for (let i = 0; i < this.bufferSize; i++) this.setY(i, y);
	}

	fill(x0, dx, y) {
		for (let i = 0; i < this.bufferSize; i++) {
			this.setX(i, x0 + dx * i);
			this.setY(i, y);
		}
	}

	/**
     * 
     * @param {Float32Array} data a fresh buffer of data
     */
	push(data) {
		const len = data.length;
		for (let i = 0; i < this.bufferSize - len; i++) {
			this.setY(i, this.getY(i + len));
		}
		for (let i = 0; i < len; i++) {
			this.setY(i + this.bufferSize - len, data[i]);
		}
	}
}

export default Line;
