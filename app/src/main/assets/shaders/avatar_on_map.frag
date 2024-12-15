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

varying float arrowMarkerHeightSize;

void main() {
    float animationTime = u_animationTime;
    //float alpha = abs(invertAnimationUnit - clamp((u_current_elapsed_time - startAnimationElapsedTime) / animationTime, 0.0, 1.0));
    //gl_FragColor = vec4(u_color, alpha); //texture2D(u_text, textureCord)


    // это чтобы сделать закругленные края у автарки
    // Центральные координаты
    vec2 center = vec2(0.5, 0.5);
    vec2 size = vec2(0.5, 0.5);
    vec2 uv = abs(corners_coords - center);
    float uRadius = 0.3;
    vec2 positionOfCornerCircle = size - vec2(uRadius, uRadius);

    float isArrowZoneAlpha = corners_coords.y < 0.0 ? 1.0 : 0.0;

    // Расстояние до угла
    vec2 cornerPos = uv - positionOfCornerCircle;
    float cornerDistance = length(cornerPos);
    bool alphaB = cornerDistance > uRadius && cornerPos.x > 0.0 && cornerPos.y > 0.0;
    float alphaRounded = mix(alphaB ? 0.0 : 1.0, 1.0, isArrowZoneAlpha); // альфа закрегленных углов

    vec3 resultColor = mix(texture2D(u_text, textureCord).rgb, u_color, u_drawColorMix);

    float x = corners_coords.x;
    float y = 1.0 - (corners_coords.y / -arrowMarkerHeightSize);
    float curveValue = pow(4.0 * x - 2.0, 2.0);
    float innerSpaceAlpha = y < curveValue ? 0.0 : 1.0;
    vec3 arrow = vec3(innerSpaceAlpha);

    float alpha = alphaRounded - (1.0 - innerSpaceAlpha) * isArrowZoneAlpha;
    vec4 colorFirst = vec4(mix(texture2D(u_text, textureCord).rgb, arrow, isArrowZoneAlpha), alpha);
    gl_FragColor = colorFirst;
    //gl_FragColor = mix(colorFirst, vec4(resultColor, mix(alphaB ? 0.0 : 1.0, 0.0, isArrowZoneAlpha)), u_drawColorMix);
}