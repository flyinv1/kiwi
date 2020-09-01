import OBJECT from '../Core/Objects';

export default function Renderer(parameters) {

    let _canvas = parameters.canvas ? parameters.canvas : document.createElement('canvas');
    let _context = parameters.context ? parameters.context : null;

    let _attributes = {
        antialias: parameters.antialias !== undefined ? parameters.antialias : true,
        alpha: parameters.alpha !== undefined ? parameters.alpha : false,
        powerPreference: parameters.powerPreference !== undefined ? parameters.powerPreference : 'default',
        premultipliedAlpha: parameters.premultipliedAlpha !== undefined ? parameters.premultipliedAlpha : true,
        depth: parameters.depth !== undefined ? parameters.depth : false
    };

    let _gl;
    let _objects;
    let _displayWidth, _displayHeight, _aspect;
    let _pixelRatio = window.devicePixelRatio;

    try {
        _gl = !_context ? _canvas.getContext('webgl', _attributes) : _context;
        _gl.enable(_gl.DEPTH_TEST);
        _gl.depthFunc(_gl.LESS);
        _gl.clearColor(0.5, 0.0, 0.0, 1.0);
        _gl.clear(_gl.COLOR_BUFFER_BIT || _gl.DEPTH_BUFFER_BIT);
        _gl.viewport(
            0, 
            0,
            Math.floor(_gl.canvas.clientWidth * _pixelRatio),
            Math.floor(_gl.canvas.clientHeight * _pixelRatio)
        );
    } catch (error) {
        console.error('GLPlot initialization error: ', error.message);
        throw error;
    }

    this.attributes = _attributes

    /**
     * @description Update the canvas drawing buffer dimension according to screen-space canvas dimensions
     */
    this.resize = () => {
        _displayWidth = Math.floor(_gl.canvas.clientWidth * _pixelRatio);
        _displayHeight = Math.floor(_gl.canvas.clientHeight * _pixelRatio);
        _gl.canvas.width = _displayWidth;
        _gl.canvas.height = _displayHeight;
        _gl.viewport(0, 0, _displayWidth, _displayHeight);
    }

    /**
     * @description Returns true if the canvas screen-space dimensions have changed
     */
    this.shouldResize = () => {
        return (_gl.canvas.clientWidth * _pixelRatio !== _displayWidth || _gl.canvas.clientHeight * _pixelRatio !== _displayHeight);
    }

    /**
     * @description Set the drawing buffer dimensions manually
     * @param {Number} width drawing buffer width
     * @param {Number} height drawing buffer height
     */
    this.setDisplaySize = (width, height) => {
        _displayWidth = width;
        _displayWidth = height;
        _aspect = _displayWidth / _displayWidth;

        this.context.canvas.width = Math.floor(_displayWidth);
        this.context.canvas.height = Math.floor(_displayWidth);

        _gl.viewport(0, 0, _displayWidth, _displayWidth);
    };

    /**
     * @description Get the current drawing buffer size
     * @returns { width: Number, height: Number }
     */
    this.getSize = () => {
        return {
            displayWidth: _gl.canvas.width,
            displayHeight: _gl.canvas.height
        };
    };

    this.disposeContext = () => {
        const ext = _gl.getExtension('WebGL_lose_context');
        if (!ext) {
            console.warn('WebGL_lose_context extension unavailable, cannot manually dispose of current WebGL context');
            console.log('https://developer.mozilla.org/en-US/docs/Web/API/WEBGL_lose_context/loseContext');
            return;
        }
        ext.loseContext();
        _objects.map((object) => object.dispose());
    };

    this.add = (object) => {
        let _object;

        // poly?
        if (object.type === OBJECT.LINE) {
            _object = _addLine(object);
        }

        return _object;
    };

    this.render = (object) => {
        if (object.render) {

            _gl.useProgram(object.program);

            _gl.uniformMatrix2fv(
                _gl.getUniformLocation(object.program, 'u_scale'),
                false,
                new Float32Array([ object.scale.x, 0, 0, object.scale.y ])
            );

            _gl.uniform2fv(
                _gl.getUniformLocation(object.program, 'u_origin'),
                new Float32Array([ object.origin.x, object.origin.y ])
            );

            if (object.type === OBJECT.LINE) {
                _renderLine(object);
            }
        }
        if (object.children) {
            object.children.map((child) => this.render(child));
        }
    };

    this.clear = () => {
        _gl.clear(_gl.COLOR_BUFFER_BIT || _gl.DEPTH_BUFFER_BIT);
    };

    let _addLine = (object) => {

        object._vbuffer = _gl.createBuffer();

        _gl.bindBuffer(_gl.ARRAY_BUFFER, object._vbuffer);

        _gl.bufferData(_gl.ARRAY_BUFFER, object.buffer, _gl.STREAM_DRAW);

        let vertexShader = _gl.createShader(_gl.VERTEX_SHADER);
        _gl.shaderSource(
            vertexShader,
            `
            attribute vec2 uv;
            uniform mat2 u_scale;
            uniform vec2 u_origin;

            void main(void) {
                gl_Position = vec4(
                    u_scale * (uv - u_origin),
                    0.0, 
                    1.0
                );
            }
        `
        );
        _gl.compileShader(vertexShader);

        let fragmentShader = _gl.createShader(_gl.FRAGMENT_SHADER);
        _gl.shaderSource(
            fragmentShader,
            `
            precision mediump float;
            uniform highp vec4 u_color;
            
            void main(void) {
                gl_FragColor = u_color;
            }
            `
        );
        _gl.compileShader(fragmentShader);

        object.program = _gl.createProgram();

        _gl.attachShader(object.program, vertexShader);
        _gl.attachShader(object.program, fragmentShader);

        _gl.linkProgram(object.program);
        _gl.bindBuffer(_gl.ARRAY_BUFFER, object._vbuffer);

        object._coord = _gl.getAttribLocation(object.program, 'uv');
        
        _gl.vertexAttribPointer(object._coord, 2, _gl.FLOAT, false, 0, 0);
        _gl.enableVertexAttribArray(object._coord);

        object.instantiated = true;

        return object;
    }

    let _renderLine = (object) => {
        _gl.uniform4f(
            _gl.getUniformLocation(object.program, 'u_color'),
            object.color.r,
            object.color.g,
            object.color.b,
            object.color.a
        );

        _gl.bufferData(_gl.ARRAY_BUFFER, object.buffer, _gl.STREAM_DRAW);

        _gl.drawArrays(!object.close ? _gl.LINE_STRIP : _gl.LINE_LOOP, 0, object.bufferSize);
    }
}
