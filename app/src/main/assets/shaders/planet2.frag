precision lowp float;

uniform sampler2D u_tile;
uniform float u_uv_shift_u;

varying vec2 v_tiles_uv;

const float M_PI = 3.1415926535897932384626433832795;

void main() {
    vec2 uv = v_tiles_uv;
    gl_FragColor = texture2D(u_tile, uv);
}