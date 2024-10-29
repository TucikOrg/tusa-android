precision highp float;

uniform vec4 u_color;
uniform float u_radius;

varying vec2 v_uv;

void main() {
    vec2 center = vec2(0.5);
    float alpha = distance(v_uv, center) * 2.0;
    gl_FragColor = vec4(vec3(0.0), alpha);
}