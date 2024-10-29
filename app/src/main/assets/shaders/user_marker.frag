precision highp float;

uniform sampler2D u_texture;
varying vec2 v_uv;

void main() {
    // Центральные координаты
    vec2 center = vec2(0.5, 0.5);
    vec2 size = vec2(0.5, 0.5);
    vec2 uv = abs(v_uv - center);
    float uRadius = 0.2;
    vec2 positionOfCornerCircle = size - vec2(uRadius, uRadius);

    // Расстояние до угла
    vec2 cornerPos = uv - positionOfCornerCircle;
    float cornerDistance = length(cornerPos);
    bool alphaB = cornerDistance > uRadius && cornerPos.x > 0.0 && cornerPos.y > 0.0;
    float alpha = alphaB ? 1.0 : 0.0;

    vec4 color = mix(texture2D(u_texture, v_uv), vec4(0.0), alpha);
    gl_FragColor = color;
}