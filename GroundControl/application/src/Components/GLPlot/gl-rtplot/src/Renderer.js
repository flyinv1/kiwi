import { LineShader } from './Shaders';

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

    try {
        _gl = !_context ? _canvas.getContext('webgl', _attributes) : _context;
        _gl.enable(_gl.DEPTH_TEST);
        _gl.depthFunc(_gl.LESS);
        _gl.clear(_gl.COLOR_BUFFER_BIT || _gl.DEPTH_BUFFER_BIT);
        _gl.viewport(0, 0, _canvas.width, _canvas.height);
    } catch (error) {
        console.error('GLPlot: ', error.message);
        throw error;
    }

    this.attributes = _attributes
    this.objects = [];
    this.maintainAspect = false;

    let _width, _height, _pixelRatio, _aspect;

    this.setSize = (width, height) => {
        _width = width;
        _height = height;

        _aspect = _width / _height;

        this.context.canvas.width = Math.floor(_width * _pixelRatio);
        this.context.canvas.height = Math.floor(_height * _pixelRatio);

        _gl.viewport(0, 0, _width * _pixelRatio, _height * _pixelRatio);
    };

    this.getSize = () => {
        return {
            _width,
            _height
        };
    };

    this.setPixelRatio = (pixelRatio) => {
        _pixelRatio = pixelRatio;
        this.setSize(_width, _height);
    };

    this.getPixelRatio = () => _pixelRatio;

    this.disposeContext = () => {
        const ext = _gl.getExtension('WebGL_lose_context');
        if (!ext) {
            console.warn('WebGL_lose_context extension unavailable, cannot manually dispose of current WebGL context');
            console.log('https://developer.mozilla.org/en-US/docs/Web/API/WEBGL_lose_context/loseContext');
            return;
        }
        ext.loseContext();
        this.objects.map((object) => object.dispose());
    };

    this.instantiateTree = (root) => {
        if (root.renderable) root = this.instantiateObject(root);
        if (root.children) {
            root.children.map((child) => {
                if (child.renderable) child = this.instantiateObject(child);
                child = this.instantiateTree(child);
            });
        }
        return root;
    };

    this.instantiateObject = (object) => {
        object._vbuffer = _gl.createBuffer();

        if (object.isLine) {
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
					gl_Position = vec4(u_scale * (uv + u_origin), 0.0, 1.0);
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
    };

    this.renderObject = (object) => {

        console.log("obj: ", object)

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

        console.log("error: ", _gl.getError())

        if (object.isLine) {
            _gl.uniform4f(
                _gl.getUniformLocation(object.program, 'u_color'),
                object.color.r,
                object.color.g,
                object.color.b,
                object.color.a
            );

            _gl.bufferData(_gl.ARRAY_BUFFER, object.buffer, _gl.STREAM_DRAW);

            _gl.drawArrays(!object.close ? _gl.LINE_STRIP : _gl.LINE_LOOP, 0, object.bufferSize);

            return;
        }
    };

    this.render = (root) => {

        console.log("root: ", root);

        root.children.map((child) => {
            if (child.renderable) this.renderObject(child);
            // FIXME : no transforms are passed down on render
            if (child.children) child.children.map((child) => this.render(child));
        });
    };

    this.clear = () => {
        _gl.clear(_gl.COLOR_BUFFER_BIT || _gl.DEPTH_BUFFER_BIT);
    };
}
