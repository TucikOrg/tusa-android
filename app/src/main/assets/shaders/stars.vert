attribute vec4 vertexPosition;
attribute float vertexPointSize;

uniform mat4 u_matrix;

void main() {
    gl_PointSize = vertexPointSize;
    gl_Position = u_matrix * vertexPosition;
}