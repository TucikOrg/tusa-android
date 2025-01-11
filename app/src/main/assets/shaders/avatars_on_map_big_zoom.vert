precision highp float;

uniform vec2 u_movementTargetMarker;
uniform vec2 u_movementMarker;
uniform vec2 u_latLon;
uniform vec2 u_latLon_previous;
uniform float u_startAnimationLatLonTime;

attribute vec2 a_textureCord;
attribute vec2 a_border_direction;

uniform vec3 u_pointOnMap;
uniform vec3 u_pointOnMapPrevious;

uniform float u_animationLatLonTime;
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

uniform float u_targetMarkerSize;
uniform float u_movementStartAnimationTime;
uniform float u_invertAnimationUnit;
uniform float u_startAnimationElapsedTime;
uniform float u_startMarkerSizeAnimation;

uniform float u_startSelectionAnimationTime;
uniform float u_animationMarkerType;

uniform mat4 u_matrixPV_SCREEN;
uniform vec2 u_screenSize;

varying vec2 textureCord;
varying float startAnimationElapsedTime;
varying float invertAnimationUnit;
varying vec2 corners_coords;

varying float arrowMarkerHeightSize;

const float M_PI = 3.1415926535897932384626433832795;

vec3 getScreenPos(vec3 markerPointLocation) {
    vec4 pClip = u_matrix * vec4(markerPointLocation, 1.0);
    vec3 pNdc = vec3(pClip.x / pClip.w, pClip.y / pClip.w, pClip.z / pClip.w);
    float screenWidthT = u_screenSize.x;
    float screenHeightT = u_screenSize.y;
    float screenX = (pNdc.x + 1.0) * 0.5 * screenWidthT;
    float screenY = (1.0 - pNdc.y) * 0.5 * screenHeightT;
    vec3 screenPos = vec3(screenX, screenY, 0.0);
    return screenPos;
}

void main() {
    vec3 markerPointLocation = u_pointOnMap;
    vec3 previousMarkerPointLocation = u_pointOnMapPrevious;

    vec3 screenPos = getScreenPos(markerPointLocation);
    vec3 screenPosPrevious = getScreenPos(previousMarkerPointLocation);

    float animationLatLonTime = u_animationLatLonTime;
    float startAnimationLatLonTime = u_startAnimationLatLonTime;
    float animationLatLonProgress = clamp((u_current_elapsed_time - startAnimationLatLonTime) / animationLatLonTime, 0.0, 1.0);
    vec3 useScreenPos = mix(screenPosPrevious, screenPos, animationLatLonProgress);

    // параметры рисования
    float markerFloatSpeed = 3.0;
    float borderWidth = u_borderWidth;
    float squeezeMarkerStrength = 0.05;

    float startMarkerSizeAnimation = u_startMarkerSizeAnimation;
    float targetMarkerSize = u_targetMarkerSize;
    float markerSize = abs(a_border_direction.x);
    float markerSizeDelta = targetMarkerSize - markerSize;
    vec2 markerPointDirection = vec2(a_border_direction.x / abs(a_border_direction.x), a_border_direction.y / abs(a_border_direction.y));
    float currentMarkerSize = markerSize + markerSizeDelta * clamp((u_current_elapsed_time - startMarkerSizeAnimation) / u_markerSizeAnimationTime, 0.0, 1.0);
    vec2 useBorderDirection = vec2(markerPointDirection.x * currentMarkerSize, markerPointDirection.y * currentMarkerSize);


    // чтобы сделать границу вокруг аватара, нужно сдвинуть точку на границе на borderWidth
    // u_drawColorMix отвечает за то аватар это или нет
    vec2 shiftBorder = vec2(a_border_direction.x < 0.0 ? 1.0 : -1.0, a_border_direction.y < 0.0 ? 1.0 : -1.0) * borderWidth * (1.0 - u_drawColorMix);

    float startMovementTime = u_movementStartAnimationTime;
    float moveAnimationTime = u_movementAnimationTime;
    float movementProgress = clamp( (u_current_elapsed_time - startMovementTime) / moveAnimationTime, 0.0, 1.0);

    vec2 movementMarker = u_movementMarker;
    vec2 movementTargetMarker = u_movementTargetMarker;
    vec2 movementDelta = movementTargetMarker - movementMarker;
    vec2 currentMovement = movementMarker + movementDelta * movementProgress;

    // анимация выделения
    float selectedMarkerSquizeDelta = 0.15;
    float selectedAnimationSpeed = 4.0;
    float animationType = u_animationMarkerType;
    bool isMarkerSelectedAnimation = animationType == 1.0;
    float useTime = u_current_elapsed_time - u_startSelectionAnimationTime;
    float selectedAnimProgressX = abs(sin(useTime * selectedAnimationSpeed));
    float selectedAnimProgressY = abs(sin((useTime) * selectedAnimationSpeed));
    float markerSelectedAnimationX = 1.0 - selectedAnimProgressX * selectedMarkerSquizeDelta;
    float markerSelectedAnimationY = 1.0 - selectedAnimProgressY * selectedMarkerSquizeDelta;

    float animShiftXResult = isMarkerSelectedAnimation ? markerSelectedAnimationX : 1.0;
    float animShiftYResult = isMarkerSelectedAnimation ? markerSelectedAnimationY : 1.0;

    // куда двигать точку относительно места где аватар расположен
    vec2 sizingShift = (useBorderDirection + shiftBorder);
    sizingShift.x *= animShiftXResult;
    sizingShift.y *= animShiftYResult;
    vec2 shift = (sizingShift + currentMovement);

    gl_PointSize = 20.0;
    gl_Position = u_matrixPV_SCREEN * vec4(useScreenPos.xy + shift, useScreenPos.z, 1.0);

    startAnimationElapsedTime = u_startAnimationElapsedTime;
    textureCord = a_textureCord;
    invertAnimationUnit = u_invertAnimationUnit;

    float forRoundedCornersU = a_border_direction.x < 0.0 ? 0.0 : 1.0;
    float forRoundedCornersV = a_border_direction.y < 0.0 ? 0.0 : 1.0;
    corners_coords = vec2(forRoundedCornersU, forRoundedCornersV);
}