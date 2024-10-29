precision highp float;

uniform mat4 u_matrix;

attribute vec4 a_vertexPosition;
attribute vec2 a_uv;

varying vec2 v_uv;

void main() {
    gl_Position = u_matrix * a_vertexPosition;
    v_uv = a_uv;
}