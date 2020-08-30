attribute vec2 uv;
uniform mat2 u_scale;
uniform vec2 u_origin;

void main(void) {
    gl_Position = vec4(u_scale * (uv + u_origin), 0.0, 1.0);
}

