precision highp float;

uniform vec4 u_color;
uniform sampler2D u_tex;
uniform vec2 u_uv_offset;
uniform vec2 u_uv_scale;
varying vec2 v_tex_uv;

void main() {
    vec2 uv = v_tex_uv * u_uv_scale + u_uv_offset;
    float alpha = step(0.0, uv.x) - step(1.0, uv.x);

    gl_FragColor = mix(vec4(1.0, 1.0, 1.0, 1.0), texture2D(u_tex, uv), alpha);
}