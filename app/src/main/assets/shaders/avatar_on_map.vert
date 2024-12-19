precision highp float;

attribute vec4 vertexPosition;
attribute vec2 a_textureCord;
attribute vec2 a_latLon;
attribute vec2 a_border_direction;
attribute float a_startAnimationElapsedTime;
attribute float a_invertAnimationUnit;

uniform float u_scale;
uniform mat4 u_matrix;
uniform vec3 u_axisLatitude;
uniform vec3 u_axisLongitude;
uniform vec3 u_pointOnSphere;
uniform float u_radius;
uniform float u_drawColorMix;
uniform float u_current_elapsed_time;
uniform float u_borderWidth;
uniform float u_arrowHeight;

varying vec2 textureCord;
varying float startAnimationElapsedTime;
varying float invertAnimationUnit;
varying vec2 corners_coords;

varying float arrowMarkerHeightSize;

const float M_PI = 3.1415926535897932384626433832795;


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
    //vec3 markerDirectionSphere = rotationLongitude * (rotationLatitude * u_pointOnSphere);
    vec3 markerDirectionSphere = rotationLongitude * rotationLatitude * u_pointOnSphere;

    // Compute the final location on the sphere
    vec3 markerPointLocation = markerDirectionSphere * u_radius;

    // параметры рисования
    float markerFloatSpeed = 3.0;
    float arrowHeight = u_arrowHeight + sin(u_current_elapsed_time * markerFloatSpeed) * 0.1; arrowHeight = u_arrowHeight;
    float borderWidth = u_borderWidth;
    float squeezeMarkerStrength = 0.05;

    vec2 useBorderDirection = a_border_direction;

    float size = abs(useBorderDirection.x) * 2.0;
    float heightK = (size + arrowHeight) / size; // насколько высота стала больше
    vec2 arrowShift = vec2(0.0, a_border_direction.y < 0.0 ? 0.0 : arrowHeight); // увеличить прямоугольник чтобы еще вставить туда стрелку
    vec2 markerUp = vec2(0.0, abs(useBorderDirection.x));

    // чтобы сделать границу вокруг аватара, нужно сдвинуть точку на границе на borderWidth
    // u_drawColorMix отвечает за то аватар это или нет
    vec2 shiftBorder = vec2(a_border_direction.x < 0.0 ? -1.0 : 1.0, a_border_direction.y < 0.0 ? -1.0 : 1.0) * borderWidth * u_drawColorMix;

    vec2 shift = (useBorderDirection + shiftBorder + markerUp + arrowShift) * u_scale; // куда двигать точку относительно места где аватар расположен
    gl_PointSize = 20.0;
    gl_Position = u_matrix * vec4(markerPointLocation.xy + shift, markerPointLocation.z - u_radius, 1.0);

    float avatarPortion = 1.0 / 8.0;
    float increasedPorion = avatarPortion * heightK;
    float textCoordsDelta = increasedPorion - avatarPortion;
    vec2 textureShift = vec2(0.0, a_border_direction.y < 0.0 ? textCoordsDelta : 0.0);
    textureCord = a_textureCord + textureShift;
    startAnimationElapsedTime = a_startAnimationElapsedTime;
    invertAnimationUnit = a_invertAnimationUnit;

    arrowMarkerHeightSize = heightK - 1.0;
    corners_coords = vec2(a_border_direction.x < 0.0 ? 0.0 : 1.0, a_border_direction.y < 0.0 ? -arrowMarkerHeightSize : 1.0);
}