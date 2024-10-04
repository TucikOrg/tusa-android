precision highp float;

uniform float u_pointSize;
uniform mat4 u_matrix;

attribute vec4 a_vertexPosition;

const float M_PI = 3.1415926535897932384626433832795;
const float M_PI_2 = M_PI / 2.0;
const float M_PI_4 = M_PI / 4.0;
const float M_PI_8 = M_PI / 8.0;
const float M_2_PI = 2.0 * M_PI;

void main() {
    gl_PointSize = u_pointSize;
    gl_Position = u_matrix * a_vertexPosition;
}