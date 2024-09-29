precision highp float;

uniform sampler2D u_tile;
varying vec2 v_tiles_uv;

const float M_PI = 3.1415926535897932384626433832795;

void main() {
    gl_FragColor = texture2D(u_tile, v_tiles_uv);
}