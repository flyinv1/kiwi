class CoreObject {
	_program = 0;
	_vbuffer = 0;
	_coord = 0;
	buffer = [];
	bufferSize = 0;
	renderable = true;
	name = null;

	constructor() {
		let _origin = {
			x: 0,
			y: 0
		};

		let _scale = {
			x: 1,
			y: 1
		};

		this.setScale = (scale) => {
			_scale = scale;
		};
		this.scale = () => _scale;

		this.setOrigin = (origin) => {
			_origin = origin;
		};
		this.origin = () => _origin;
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
