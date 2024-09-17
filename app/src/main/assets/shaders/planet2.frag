precision highp float;

uniform sampler2D u_tile;
varying vec2 v_uv;
uniform float u_x_dragged;

const float M_PI = 3.1415926535897932384626433832795;

float sinh(float x) {
    return (exp(x) - exp(-x)) / 2.0;
}

void main() {
    vec2 uv = v_uv;
    vec2 shiftedUV = uv + vec2(u_x_dragged, 0.0);
    gl_FragColor = texture2D(u_tile, shiftedUV);
}