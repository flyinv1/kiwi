import CoreObject from './Core/CoreObject';

class Line extends CoreObject {
	color;
	isLine = true;
	close = false;

	constructor(color, points) {
		super();
		this.color = color;
		this.buffer = new Float32Array(2 * points);
		this.bufferSize = points;
		this.scale = { x: 1, y: 1 };
		this.origin = { x: 0, y: 0 };
	}

	setX = (i, x) => {
		this.buffer[i * 2] = x;
	};

	setY = (i, x) => {
		this.buffer[i * 2 + 1] = x;
	};

	getX = (i) => this.buffer[i * 2];

	getY = (i) => this.buffer[i * 2 + 1];

	linspace(x0, dx) {
		for (let i = 0; i < this.bufferSize; i++) this.setX(i, x0 + i * dx);
	}

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
