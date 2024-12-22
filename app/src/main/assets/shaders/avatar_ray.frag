precision highp float;

uniform vec3 u_color;
uniform float u_current_elapsed_time;
uniform float u_animationTime;
uniform float u_drawColorMix;

varying float startAnimationElapsedTime;
varying float invertAnimationUnit;

void main() {
    float animationTime = u_animationTime;
    float alphaAnimation = abs(invertAnimationUnit - clamp((u_current_elapsed_time - startAnimationElapsedTime) / animationTime, 0.0, 1.0));

    gl_FragColor = vec4(u_color, alphaAnimation);
}