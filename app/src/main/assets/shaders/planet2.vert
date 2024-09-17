precision highp float;

uniform mat4 u_matrix;

attribute vec4 a_vertexPosition;
attribute vec2 a_uv;
uniform float u_animation;
uniform float u_x_dragged;

varying vec2 v_uv;

const float M_PI = 3.1415926535897932384626433832795;

vec3 flatToSphere(float longitude, float latitude, float r, vec3 flatPos)
{
    float x = r * cos(latitude) * sin(longitude);
    float y = r * sin(latitude);
    float z = r * cos(latitude) * cos(longitude);
    return vec3(x, y, z);
}

vec3 anglesToSphereCoord(vec2 a, float r)
{
    return vec3(
    r * sin(a.y) * sin(a.x),
    r * cos(a.y),
    r * sin(a.y) * cos(a.x)
    );
}

float sinh(float x) {
    return (exp(x) - exp(-x)) / 2.0;
}

void main() {

    float uv_x = a_uv.x;
    float uv_y = a_uv.y;

    float longitude = (uv_x * 2.0 * M_PI - M_PI);
    float latitude = atan(sinh(M_PI * -(1.0 - 2.0 * uv_y)));
    vec3 sphPos = flatToSphere(longitude, latitude, 1.0, a_vertexPosition.xyz);
    vec4 wrapPos = mix(a_vertexPosition, vec4(sphPos, 1.0), u_animation);


    gl_Position = u_matrix * wrapPos;

    v_uv = a_uv;
}