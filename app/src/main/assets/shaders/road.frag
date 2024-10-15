precision highp float;

uniform vec4 u_color;
uniform vec4 u_border_color;
uniform float u_border_factor;

varying vec2 v_uv;

void main() {
    vec3 borderColor = vec3(0.0, 0.0, 0.0);
    float borderAlpha = abs(step(1.0 - u_border_factor, v_uv.x) - step(v_uv.x, u_border_factor));
    gl_FragColor = mix(u_color, u_border_color, borderAlpha);
}