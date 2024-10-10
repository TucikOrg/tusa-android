attribute vec4 vertexPosition;

uniform float u_pointSize;
uniform mat4 u_matrix;

void main() {
    gl_PointSize = u_pointSize;
    gl_Position = u_matrix * vertexPosition;
}