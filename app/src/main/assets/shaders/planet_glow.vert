attribute vec4 vertexPosition;
attribute vec2 a_textureCord;
uniform mat4 u_matrix;

uniform float u_pointSize;

varying vec2 textureCord;

void main() {
    gl_PointSize = u_pointSize;
    gl_Position = u_matrix * vertexPosition;
    textureCord = a_textureCord;
}