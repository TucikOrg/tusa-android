precision highp float;

attribute vec2 vertexPosition;
attribute vec2 a_textureCord;
attribute float a_skipLen;
attribute float a_pixelShift;
attribute vec2 a_symbolShift;

varying vec2 textureCord;

uniform mat4 u_matrix;
uniform mat4 u_projection;
uniform float u_startShift;
uniform float u_scale;
uniform float u_symbolScale;
uniform float u_elapsedTime;
uniform int u_pointsSize;
uniform vec2 u_path[50];

void main() {
    float skipLen = a_skipLen * u_scale + u_startShift;

    vec2 point;
    vec2 nextPoint;
    for (int i = 0; i < u_pointsSize - 1; i++) {
        point = u_path[i];
        nextPoint = u_path[i + 1];
        float dist = sqrt( pow(nextPoint.x - point.x, 2.0) + pow(nextPoint.y - point.y, 2.0) );
        if (skipLen > dist) {
            skipLen -= dist;
        } else break;
    }
    float fromPointShift = skipLen - a_pixelShift * u_scale / 2.0;

    vec2 tangent = normalize(nextPoint - point);
    vec2 orthogonal = vec2(-tangent.y, tangent.x);
    vec2 symbolScaleVec = vec2(vertexPosition.x == 0.0 ? 1.0 : -1.0, vertexPosition.y == 0.0 ? 1.0 : -1.0) * u_symbolScale;

    vec2 startLoc = (vertexPosition.x + a_symbolShift.x - symbolScaleVec.x) * tangent + (vertexPosition.y - a_symbolShift.y - symbolScaleVec.y) * orthogonal;
    startLoc *= u_scale;
    vec2 symbPos = startLoc + point + tangent * fromPointShift;
    gl_Position = u_projection * u_matrix * vec4(symbPos.xy, 0.0, 1.0);
    textureCord = a_textureCord;
}