import line_frag from './Line/frag.glsl';
import line_vert from './Line/vert.glsl';

let LineShader = {
    FRAGMENT: line_frag,
    VERTEX: line_vert
};

export {
    LineShader,
}