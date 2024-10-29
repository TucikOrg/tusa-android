precision highp float;

attribute vec4 vertexPosition;
attribute vec2 a_uv;

uniform float u_pointSize;
uniform mat4 u_matrix;

varying vec2 v_uv;

void main() {
    gl_PointSize = u_pointSize;
    gl_Position = u_matrix * vertexPosition;
    v_uv = a_uv;
}