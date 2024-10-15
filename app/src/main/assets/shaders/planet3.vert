precision highp float;

uniform mat4 u_matrix;
uniform mat4 u_plane_matrix;
uniform mat4 u_sphere_matrix;
uniform float u_transition;
uniform vec2 u_cam_epsg3857;
uniform float u_planeSize;

attribute vec4 a_vertexPosition;
attribute vec2 a_planet_epsg3857;

attribute vec2 a_tex_uv;
varying vec2 v_tex_uv;

const float M_PI = 3.1415926535897932384626433832795;
const float M_PI_2 = M_PI / 2.0;
const float M_PI_4 = M_PI / 4.0;
const float M_PI_8 = M_PI / 8.0;
const float TWO_M_PI = 2.0 * M_PI;

float sinh(float x) {
    return (exp(x) - exp(-x)) / 2.0;
}

vec2 EPSG4326_TO_EPSG3857(vec2 epsg4326) {
    return vec2(log(tan(epsg4326.x) + 1.0 / cos(epsg4326.x)), epsg4326.y);
}

vec2 EPSG3857_TO_EPSG4326(vec2 epsg3857) {
    return vec2(atan(sinh(epsg3857.x)), epsg3857.y);
}

vec3 getSpherePoint(float epsg4326Latitude, float epsg4326Longitude, float radius)
{
    float x = radius * cos(epsg4326Latitude) * sin(epsg4326Longitude);
    float y = radius * sin(epsg4326Latitude);
    float z = radius * cos(epsg4326Latitude) * cos(epsg4326Longitude);
    return vec3(x, y, z);
}

void main() {
    vec2 epsg3857 = a_planet_epsg3857;
    vec2 epsg4326 = EPSG3857_TO_EPSG4326(epsg3857);
    float radius = u_planeSize / TWO_M_PI;

    vec4 sphereVertexPos = u_sphere_matrix * vec4(getSpherePoint(epsg4326.x, epsg4326.y, radius), 1.0) - vec4(0.0, 0.0, radius, 0.0);
    vec4 planeVertexPos = u_plane_matrix * a_vertexPosition;
    vec4 vertexPos = mix(planeVertexPos, sphereVertexPos, u_transition);
    gl_Position = u_matrix * vertexPos;

    v_tex_uv = a_tex_uv;
}