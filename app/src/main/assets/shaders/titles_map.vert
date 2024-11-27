precision highp float;

attribute vec4 vertexPosition;
attribute vec2 a_textureCord;
attribute vec2 a_latLon;

varying vec2 textureCord;

uniform mat4 u_matrix;
uniform mat4 u_projection;
uniform vec3 u_axisLatitude;
uniform vec3 u_axisLongitude;
uniform vec3 u_pointOnSphere;
uniform float u_radius;

void main() {
    mat3 rotationLatitude = mat3(
        cos(-a_latLon.x) + u_axisLatitude.x * u_axisLatitude.x * (1.0 - cos(-a_latLon.x)),
        u_axisLatitude.x * u_axisLatitude.y * (1.0 - cos(-a_latLon.x)) - u_axisLatitude.z * sin(-a_latLon.x),
        u_axisLatitude.x * u_axisLatitude.z * (1.0 - cos(-a_latLon.x)) + u_axisLatitude.y * sin(-a_latLon.x),

        u_axisLatitude.y * u_axisLatitude.x * (1.0 - cos(-a_latLon.x)) + u_axisLatitude.z * sin(-a_latLon.x),
        cos(-a_latLon.x) + u_axisLatitude.y * u_axisLatitude.y * (1.0 - cos(-a_latLon.x)),
        u_axisLatitude.y * u_axisLatitude.z * (1.0 - cos(-a_latLon.x)) - u_axisLatitude.x * sin(-a_latLon.x),

        u_axisLatitude.z * u_axisLatitude.x * (1.0 - cos(-a_latLon.x)) - u_axisLatitude.y * sin(-a_latLon.x),
        u_axisLatitude.z * u_axisLatitude.y * (1.0 - cos(-a_latLon.x)) + u_axisLatitude.x * sin(-a_latLon.x),
        cos(-a_latLon.x) + u_axisLatitude.z * u_axisLatitude.z * (1.0 - cos(-a_latLon.x))
    );

    // Longitude rotation (create a rotation matrix)
    mat3 rotationLongitude = mat3(
        cos(a_latLon.y) + u_axisLongitude.x * u_axisLongitude.x * (1.0 - cos(a_latLon.y)),
        u_axisLongitude.x * u_axisLongitude.y * (1.0 - cos(a_latLon.y)) - u_axisLongitude.z * sin(a_latLon.y),
        u_axisLongitude.x * u_axisLongitude.z * (1.0 - cos(a_latLon.y)) + u_axisLongitude.y * sin(a_latLon.y),

        u_axisLongitude.y * u_axisLongitude.x * (1.0 - cos(a_latLon.y)) + u_axisLongitude.z * sin(a_latLon.y),
        cos(a_latLon.y) + u_axisLongitude.y * u_axisLongitude.y * (1.0 - cos(a_latLon.y)),
        u_axisLongitude.y * u_axisLongitude.z * (1.0 - cos(a_latLon.y)) - u_axisLongitude.x * sin(a_latLon.y),

        u_axisLongitude.z * u_axisLongitude.x * (1.0 - cos(a_latLon.y)) - u_axisLongitude.y * sin(a_latLon.y),
        u_axisLongitude.z * u_axisLongitude.y * (1.0 - cos(a_latLon.y)) + u_axisLongitude.x * sin(a_latLon.y),
        cos(a_latLon.y) + u_axisLongitude.z * u_axisLongitude.z * (1.0 - cos(a_latLon.y))
    );

    // Compute marker direction in sphere
    vec3 markerDirectionSphere = rotationLongitude * (rotationLatitude * u_pointOnSphere);

    // Compute the final location on the sphere
    vec3 markerPointLocation = markerDirectionSphere * u_radius;
    
    gl_Position = u_projection * u_matrix * (vertexPosition + vec4(markerPointLocation.xyz, 0.0));
    textureCord = a_textureCord;
}