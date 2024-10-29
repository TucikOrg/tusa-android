precision highp float;

uniform vec4 u_color;
uniform float u_radius;

varying vec2 v_uv;

void main() {
    vec2 center = vec2(0.5);
    float dist = distance(v_uv, center) * 2.0;
    float alpha = smoothstep(1.0, 0.0, dist);
    gl_FragColor = vec4(vec3(1.0), alpha);
}