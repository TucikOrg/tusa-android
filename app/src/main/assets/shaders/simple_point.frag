precision lowp float;

uniform vec4 u_color;

void main() {
    float dist = distance(gl_PointCoord, vec2(0.5, 0.5));
    float intensity = 1.0 - step(0.5, dist);
    gl_FragColor = vec4(u_color.xyz, intensity);
}