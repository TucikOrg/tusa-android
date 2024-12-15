precision highp float;

uniform sampler2D u_text;
uniform vec3 u_color;
uniform float u_current_elapsed_time;
uniform float u_animationTime;
uniform float u_drawColorMix;

varying float startAnimationElapsedTime;
varying vec2 textureCord;
varying float invertAnimationUnit;
varying vec2 corners_coords;

void main() {
    float animationTime = u_animationTime;
    //float alpha = abs(invertAnimationUnit - clamp((u_current_elapsed_time - startAnimationElapsedTime) / animationTime, 0.0, 1.0));
    //gl_FragColor = vec4(u_color, alpha); //texture2D(u_text, textureCord)


    // это чтобы сделать закругленные края у автарки
    // Центральные координаты
    vec2 center = vec2(0.5, 0.5);
    vec2 size = vec2(0.5, 0.5);
    vec2 uv = abs(corners_coords - center);
    float uRadius = 0.4;
    vec2 positionOfCornerCircle = size - vec2(uRadius, uRadius);

    // Расстояние до угла
    vec2 cornerPos = uv - positionOfCornerCircle;
    float cornerDistance = length(cornerPos);
    bool alphaB = cornerDistance > uRadius && cornerPos.x > 0.0 && cornerPos.y > 0.0;
    float alphaRounded = alphaB ? 0.0 : 1.0; // альфа закрегленных углов

    vec3 resultColor = mix(texture2D(u_text, textureCord).rgb, u_color, u_drawColorMix);
    gl_FragColor = vec4(resultColor, alphaRounded);
}