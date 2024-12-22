precision highp float;

attribute vec3 a_movementMarker;
attribute vec2 a_textureCord;
attribute vec2 a_latLon;
attribute vec2 a_border_direction;
attribute float a_startAnimationElapsedTime;
attribute float a_invertAnimationUnit;
attribute float a_markerSize;

uniform float u_scale;
uniform mat4 u_matrix;
uniform vec3 u_axisLatitude;
uniform vec3 u_axisLongitude;
uniform vec3 u_pointOnSphere;
uniform float u_radius;
uniform float u_current_elapsed_time;

varying float startAnimationElapsedTime;
varying float invertAnimationUnit;

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

    vec3 markerDirectionSphere = rotationLongitude * rotationLatitude * u_pointOnSphere;

    // Compute the final location on the sphere
    vec3 markerPointLocation = markerDirectionSphere * u_radius;

    float borderWidth = 0.17;
    vec2 perpendicular = normalize(vec2(-a_movementMarker.y, a_movementMarker.x));

    bool isRightPointOnMarker = a_movementMarker.z == 1.0;
    bool isMarkersTwoPoints = a_movementMarker.z != 0.0;
    vec2 shiftPointToMarkerBorder = (isRightPointOnMarker ? perpendicular : -perpendicular) * (a_markerSize + borderWidth);
    vec2 shift = (a_movementMarker.xy + shiftPointToMarkerBorder) * u_scale;

    gl_PointSize = 20.0;
    gl_Position = u_matrix * vec4(markerPointLocation.xy + shift, markerPointLocation.z - u_radius, 1.0);

    startAnimationElapsedTime = a_startAnimationElapsedTime;
    invertAnimationUnit = a_invertAnimationUnit;
}