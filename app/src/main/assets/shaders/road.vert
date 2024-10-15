precision highp float;

attribute vec4 vertexPosition;
attribute vec4 a_perpendiculars;
attribute vec2 a_uv;

uniform float u_width;
uniform mat4 u_matrix;
uniform mat4 u_projection;
uniform float u_pointSize;

varying vec2 v_uv;

void main() {
    gl_PointSize = u_pointSize;
    float h = u_width / 2.0;
    float xd = a_perpendiculars.x * h;
    float yd = a_perpendiculars.y * h;
    gl_Position = u_projection * (u_matrix * vertexPosition + vec4(xd, yd, 0.0, 0.0));

    v_uv = a_uv;
}