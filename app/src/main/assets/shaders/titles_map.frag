precision highp float;

uniform sampler2D u_text;
uniform vec3 u_color;

varying vec2 textureCord;

void main() {
    //gl_FragColor = vec4(u_color, texture2D(u_text, textureCord).a);
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}