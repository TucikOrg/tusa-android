precision lowp float;

uniform vec4 u_color;
uniform float u_radius;

varying vec2 textureCord;

void main() {
    float lightForce = 1.0;
    vec2 center = vec2(0.0, 0.0);
    float dist = distance(textureCord, center);
    float planetAlpha = step(u_radius, dist);
    float lightMask = smoothstep(1.0, u_radius / 2.0, dist);
    float lightColor = lightMask * lightForce;

    float alpha = planetAlpha * lightMask;
    gl_FragColor = vec4(lightColor, lightColor, lightColor, alpha);
    //gl_FragColor = vec4(alpha, alpha, alpha, 1.0);
}