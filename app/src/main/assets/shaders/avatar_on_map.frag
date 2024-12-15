precision highp float;

uniform sampler2D u_text;
uniform vec3 u_color;
uniform float u_current_elapsed_time;
uniform float u_animationTime;

varying float startAnimationElapsedTime;
varying vec2 textureCord;
varying float invertAnimationUnit;

void main() {
    float animationTime = u_animationTime;
    float alpha = abs(invertAnimationUnit - clamp((u_current_elapsed_time - startAnimationElapsedTime) / animationTime, 0.0, 1.0));
    //gl_FragColor = vec4(u_color, alpha); //texture2D(u_text, textureCord)
    gl_FragColor = texture2D(u_text, textureCord);
}