precision lowp float;

attribute vec4 vertexPosition;
attribute vec2 a_uv;
attribute vec2 a_shift;

uniform float u_point_size;
uniform mat4 u_matrix;
uniform mat4 u_projection;

varying vec2 v_uv;

void main() {
    gl_PointSize = 30.0f;
    float h = u_point_size / 2.0f;
    vec2 shift = vec2(a_shift.x * h, a_shift.y * h);
    gl_Position = u_projection * (u_matrix * vertexPosition + vec4(shift, 0.0, 0.0));

    v_uv = a_uv;
}