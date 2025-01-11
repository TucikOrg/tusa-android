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
    float alphaAnimation = abs(invertAnimationUnit - clamp((u_current_elapsed_time - startAnimationElapsedTime) / animationTime, 0.0, 1.0));

    // параметры рисования
    float uRadius = 0.45;

    // это чтобы сделать закругленные края у автарки
    // Центральные координаты
    vec2 center = vec2(0.5, 0.5);
    vec2 size = vec2(0.5, 0.5);
    vec2 uv = abs(corners_coords - center);

    vec2 positionOfCornerCircle = size - vec2(uRadius, uRadius);

    // Расстояние до угла
    vec2 cornerPos = uv - positionOfCornerCircle;
    float cornerDistance = length(cornerPos);
    bool alphaCornersRoundedB = (cornerDistance <= uRadius && cornerPos.x >= 0.0 && cornerPos.y >= 0.0);
    bool alphaInnerSpaceB = (uv.x < positionOfCornerCircle.x || uv.y < positionOfCornerCircle.y) && (uv.y < size.x && uv.x < size.y);
    bool alphaRoundedB = alphaCornersRoundedB || alphaInnerSpaceB; // закругление углов

    // это аватар
    float alphaRounded = alphaRoundedB ? 1.0 : 0.0;
    vec4 colorFirst = vec4(texture2D(u_text, textureCord).rgb, alphaRounded * alphaAnimation);

    // это фон, обертка для аватара
    float bgAlpha = alphaRoundedB ? 1.0 : 0.0;
    vec4 colorBg = vec4(u_color, bgAlpha * alphaAnimation);

    gl_FragColor = mix(colorFirst, colorBg, u_drawColorMix);
}