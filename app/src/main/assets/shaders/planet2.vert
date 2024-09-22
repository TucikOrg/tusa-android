precision lowp float;

uniform mat4 u_matrix;
uniform mat4 u_projection;

attribute vec4 a_vertexPosition;
attribute vec2 a_planet_uv;
attribute vec2 a_tiles_uv;

uniform float u_transition;
uniform float u_uv_shift_u;
uniform float u_cam_angle;
uniform float u_cam_distance;
uniform float u_radius;
uniform float u_plane_camera_y;

varying vec2 v_tiles_uv;

const float M_PI = 3.1415926535897932384626433832795;
const float M_PI_2= M_PI / 2.0;
const float M_PI_4 = M_PI / 4.0;

vec3 flatToSphere(float longitude, float latitude, float radius)
{
    float x = radius * cos(latitude) * sin(longitude);
    float y = radius * sin(latitude);
    float z = radius * cos(latitude) * cos(longitude);
    return vec3(x, y, z);
}

float sinh(float x) {
    return (exp(x) - exp(-x)) / 2.0;
}

mat4 createViewMatrix(vec3 eye, vec3 target, vec3 up) {
    vec3 zAxis = normalize(eye - target);  // Forward vector
    vec3 xAxis = normalize(cross(up, zAxis)); // Right vector
    vec3 yAxis = cross(zAxis, xAxis);  // Recalculate up vector

    mat4 view = mat4(1.0);  // Initialize the view matrix as identity matrix

    // Set rotation part of the view matrix
    view[0][0] = xAxis.x;
    view[1][0] = xAxis.y;
    view[2][0] = xAxis.z;

    view[0][1] = yAxis.x;
    view[1][1] = yAxis.y;
    view[2][1] = yAxis.z;

    view[0][2] = zAxis.x;
    view[1][2] = zAxis.y;
    view[2][2] = zAxis.z;

    // Set translation part of the view matrix
    view[3][0] = -dot(xAxis, eye);
    view[3][1] = -dot(yAxis, eye);
    view[3][2] = -dot(zAxis, eye);

    return view;
}

vec3 createCamSphericalPosition(float angle, float distance) {
    float camX = 0.0;
    float camY = distance * sin(angle);
    float camZ = distance * cos(angle);
    return vec3(camX, camY, camZ);
}

vec3 createFlatCamPosition(float y, float radius, float camDistance) {
    //float y = radius * log(tan(M_PI_4 + angle / 2.0));

    float camX = 0.0;
    float camY = y;
    float camZ = camDistance;
    return vec3(camX, camY, camZ);
}

void main() {
    float uv_planet_x = a_planet_uv.x;
    float uv_planet_y = a_planet_uv.y;
    float radius = u_radius;
    float cameraDistance = u_cam_distance;
    float transition = u_transition;
    vec3 planeVertex = a_vertexPosition.xyz;
    float camAngle = u_cam_angle;
    float planeCameraY = u_plane_camera_y;

    // Convert UV to spherical coordinates
    float longitude = (uv_planet_x * 2.0 * M_PI - M_PI);
    float latitude = atan(sinh(M_PI * (1.0 - 2.0 * uv_planet_y)));
    vec3 sphereVertex = flatToSphere(longitude, latitude, radius);
    vec4 mixedVertex = vec4(mix(planeVertex, sphereVertex, transition), 1.0);

    // Calculate camera position
    // Camera Plane to Sphere transition
    vec3 sphericalCamPos = createCamSphericalPosition(camAngle, cameraDistance);
    vec3 planeCamPos = createFlatCamPosition(planeCameraY, radius, cameraDistance);

    vec3 sphericalTarget = vec3(0.0, 0.0, 0.0);
    vec3 planeTarget = vec3(planeCamPos.x, planeCamPos.y, 0.0);
    vec3 eye = mix(planeCamPos, sphericalCamPos, transition);
    vec3 target = mix(planeTarget, sphericalTarget, transition);
    vec3 up = vec3(0.0, 1.0, 0.0);

    mat4 viewMatrix = createViewMatrix(eye, target, up);
    gl_Position = u_projection * viewMatrix * mixedVertex;

    v_tiles_uv = a_tiles_uv;
}

