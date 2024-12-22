precision highp float;

uniform vec2 u_movementTargetMarker[64];
uniform vec2 u_movementMarker[64];
uniform vec2 u_latLon[64];

attribute vec2 a_textureCord;
attribute vec2 a_border_direction;
attribute float a_positionInUniform;

uniform float u_scale;
uniform mat4 u_matrix;
uniform vec3 u_axisLatitude;
uniform vec3 u_axisLongitude;
uniform vec3 u_pointOnSphere;
uniform float u_radius;
uniform float u_drawColorMix;
uniform float u_current_elapsed_time;
uniform float u_borderWidth;
uniform float u_movementAnimationTime;
uniform float u_markerSizeAnimationTime;

uniform float u_targetMarkerSize[64];
uniform float u_movementStartAnimationTime[64];
uniform float u_invertAnimationUnit[64];
uniform float u_startAnimationElapsedTime[64];
uniform float u_startMarkerSizeAnimation[64];


varying vec2 textureCord;
varying float startAnimationElapsedTime;
varying float invertAnimationUnit;
varying vec2 corners_coords;

varying float arrowMarkerHeightSize;

const float M_PI = 3.1415926535897932384626433832795;


void main() {
    vec2 latLon = u_latLon[int(a_positionInUniform)];
    float latitude = latLon.x;
    float longitude = latLon.y;
    
    mat3 rotationLatitude = mat3(
    cos(-latitude) + u_axisLatitude.x * u_axisLatitude.x * (1.0 - cos(-latitude)),
    u_axisLatitude.x * u_axisLatitude.y * (1.0 - cos(-latitude)) - u_axisLatitude.z * sin(-latitude),
    u_axisLatitude.x * u_axisLatitude.z * (1.0 - cos(-latitude)) + u_axisLatitude.y * sin(-latitude),

    u_axisLatitude.y * u_axisLatitude.x * (1.0 - cos(-latitude)) + u_axisLatitude.z * sin(-latitude),
    cos(-latitude) + u_axisLatitude.y * u_axisLatitude.y * (1.0 - cos(-latitude)),
    u_axisLatitude.y * u_axisLatitude.z * (1.0 - cos(-latitude)) - u_axisLatitude.x * sin(-latitude),

    u_axisLatitude.z * u_axisLatitude.x * (1.0 - cos(-latitude)) - u_axisLatitude.y * sin(-latitude),
    u_axisLatitude.z * u_axisLatitude.y * (1.0 - cos(-latitude)) + u_axisLatitude.x * sin(-latitude),
    cos(-latitude) + u_axisLatitude.z * u_axisLatitude.z * (1.0 - cos(-latitude))
    );

    // Longitude rotation (create a rotation matrix)
    mat3 rotationLongitude = mat3(
    cos(longitude) + u_axisLongitude.x * u_axisLongitude.x * (1.0 - cos(longitude)),
    u_axisLongitude.x * u_axisLongitude.y * (1.0 - cos(longitude)) - u_axisLongitude.z * sin(longitude),
    u_axisLongitude.x * u_axisLongitude.z * (1.0 - cos(longitude)) + u_axisLongitude.y * sin(longitude),

    u_axisLongitude.y * u_axisLongitude.x * (1.0 - cos(longitude)) + u_axisLongitude.z * sin(longitude),
    cos(longitude) + u_axisLongitude.y * u_axisLongitude.y * (1.0 - cos(longitude)),
    u_axisLongitude.y * u_axisLongitude.z * (1.0 - cos(longitude)) - u_axisLongitude.x * sin(longitude),

    u_axisLongitude.z * u_axisLongitude.x * (1.0 - cos(longitude)) - u_axisLongitude.y * sin(longitude),
    u_axisLongitude.z * u_axisLongitude.y * (1.0 - cos(longitude)) + u_axisLongitude.x * sin(longitude),
    cos(longitude) + u_axisLongitude.z * u_axisLongitude.z * (1.0 - cos(longitude))
    );

    // Compute marker direction in sphere
    //vec3 markerDirectionSphere = rotationLongitude * (rotationLatitude * u_pointOnSphere);
    vec3 markerDirectionSphere = rotationLongitude * rotationLatitude * u_pointOnSphere;

    // Compute the final location on the sphere
    vec3 markerPointLocation = markerDirectionSphere * u_radius;

    // параметры рисования
    float markerFloatSpeed = 3.0;
    float borderWidth = u_borderWidth;
    float squeezeMarkerStrength = 0.05;

    float startMarkerSizeAnimation = u_startMarkerSizeAnimation[int(a_positionInUniform)];
    float targetMarkerSize = u_targetMarkerSize[int(a_positionInUniform)];
    float markerSize = abs(a_border_direction.x);
    float markerSizeDelta = targetMarkerSize - markerSize;
    vec2 markerPointDirection = vec2(a_border_direction.x / abs(a_border_direction.x), a_border_direction.y / abs(a_border_direction.y));
    float currentMarkerSize = markerSize + markerSizeDelta * clamp((u_current_elapsed_time - startMarkerSizeAnimation) / u_markerSizeAnimationTime, 0.0, 1.0);
    vec2 useBorderDirection = vec2(markerPointDirection.x * currentMarkerSize, markerPointDirection.y * currentMarkerSize);


    // чтобы сделать границу вокруг аватара, нужно сдвинуть точку на границе на borderWidth
    // u_drawColorMix отвечает за то аватар это или нет
    vec2 shiftBorder = vec2(a_border_direction.x < 0.0 ? -1.0 : 1.0, a_border_direction.y < 0.0 ? -1.0 : 1.0) * borderWidth * u_drawColorMix;

    float startMovementTime = u_movementStartAnimationTime[int(a_positionInUniform)];
    float moveAnimationTime = u_movementAnimationTime;
    float movementProgress = clamp( (u_current_elapsed_time - startMovementTime) / moveAnimationTime, 0.0, 1.0);

    vec2 movementMarker = u_movementMarker[int(a_positionInUniform)];
    vec2 movementTargetMarker = u_movementTargetMarker[int(a_positionInUniform)];
    vec2 movementDelta = movementTargetMarker - movementMarker;
    vec2 currentMovement = movementMarker + movementDelta * movementProgress;

    // куда двигать точку относительно места где аватар расположен
    vec2 shift = (useBorderDirection + shiftBorder + currentMovement) * u_scale;
    gl_PointSize = 20.0;
    gl_Position = u_matrix * vec4(markerPointLocation.xy + shift, markerPointLocation.z - u_radius, 1.0);

    startAnimationElapsedTime = u_startAnimationElapsedTime[int(a_positionInUniform)];
    textureCord = a_textureCord;
    invertAnimationUnit = u_invertAnimationUnit[int(a_positionInUniform)];

    float forRoundedCornersU = a_border_direction.x < 0.0 ? 0.0 : 1.0;
    float forRoundedCornersV = a_border_direction.y < 0.0 ? 0.0 : 1.0;
    corners_coords = vec2(forRoundedCornersU, forRoundedCornersV);
}