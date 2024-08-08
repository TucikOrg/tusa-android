precision lowp float;

uniform sampler2D u_texture;
varying vec2 v_tile_cords_unit_square;

void main() {
    float maxY = 1.25;
    vec2 st = v_tile_cords_unit_square.xy;
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(center, st);
    float alphaAvatar = 1.0 - step(0.5, dist);

    float avatarX_i = 0.5;
    float avatarY_i = 0.75;

    float b = sqrt(avatarY_i);
    float a = b / avatarX_i;
    float alphaArrowCut = step(maxY - st.y, avatarY_i);
    float alphaArrow = (1. - step(maxY - st.y, pow(a * st.x - b, 2.0))) * alphaArrowCut;
    float onlyArrowZone = alphaArrow - alphaAvatar;

    float alpha = alphaArrow + alphaAvatar;
    vec4 arrowColor = mix(vec4(0.0), vec4(vec3(1.), 1.), onlyArrowZone);
    vec4 color = mix(arrowColor, texture2D(u_texture, st), alphaAvatar);

    gl_FragColor = color;
}