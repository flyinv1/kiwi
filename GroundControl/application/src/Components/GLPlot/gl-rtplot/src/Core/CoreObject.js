import OBJECT from './Color'

class CoreObject {
	_program = 0;
	_vbuffer = 0;
	_coord = 0;
	buffer = [];
	bufferSize = 0;
	renderable = true;
	name = null;
	type = OBJECT.CORE;

	constructor() {
		this.scale = { x: 1, y: 1 };
		this.origin = { x: 0, y: 0};
	}

	dispose() {
		this._program = null;
		this._vbuffer = null;
		this._coord = null;
		this.buffer = null;
		return;
	}
}

export default CoreObject;
