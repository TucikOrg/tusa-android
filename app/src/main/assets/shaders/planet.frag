precision lowp float;

uniform sampler2D u_tile_0;
varying vec2 v_tile_cords_unit_square;

void main() {
    gl_FragColor = texture2D(u_tile_0, v_tile_cords_unit_square) * vec4(0.91, 0.95, 0.95, 1.0);
}