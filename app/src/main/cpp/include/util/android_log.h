//
// Created by Artem on 07.02.2024.
//

#ifndef TUSA_ANDROID_LOG_H
#define TUSA_ANDROID_LOG_H

#include <math.h>
#include <cmath>
#include <vector>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <locale>
#include "csscolorparser/csscolorparser.h"
#define LOG_TAG "GL_ARTEM"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FLOAT(x) static_cast<float>(x)
#define DEG2RAD(a)   ((a) / (180 / M_PI))
#define RAD2DEG(a)   ((a) * (180 / M_PI))

class Utils {
public:
    static std::wstring stringToWstring(const std::string& str) {
        std::setlocale(LC_ALL, "en_US.UTF-8"); // Set locale for UTF-8
        size_t size = mbstowcs(nullptr, str.c_str(), 0) + 1;
        std::wstring wstr(size, L'\0');
        mbstowcs(&wstr[0], str.c_str(), size);
        wstr.pop_back(); // Remove null terminator
        return wstr;
    }

    std::string wstringToString(const std::wstring& wstr) {
        std::setlocale(LC_ALL, "en_US.UTF-8"); // Set locale for UTF-8
        size_t size = wcstombs(nullptr, wstr.c_str(), 0) + 1;
        std::string str(size, '\0');
        wcstombs(&str[0], wstr.c_str(), size);
        str.pop_back(); // Remove null terminator
        return str;
    }

    static float normalizeXTile(float infTile, int n) {
        return infTile < 0 ? fmod(fmod(infTile, n) + n, n) : fmod(infTile, n);
    }

    static std::string floatToString(float value, int precision) {
        std::ostringstream out;
        out << std::fixed << std::setprecision(precision) << value;
        return out.str();
    }

    static std::wstring floatToWString(float value, int precision) {
        std::wostringstream out;
        out << std::fixed << std::setprecision(precision) << value;
        return out.str();
    }

    static std::string generateRandomColor() {
        // Generate random RGB values
        int r = std::rand() % 256;
        int g = std::rand() % 256;
        int b = std::rand() % 256;

        // Return color as a string in the format "rgb(r, g, b)"
        return "rgb(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ")";
    }

    // Функция для округления до определенного количества знаков после запятой
    static float round(float value, int precision) {
        float scale = std::pow(10.0f, precision);
        return std::round(value * scale) / scale;
    }

    static float fract(float num) {
        return num - std::floor(num);  // Using floor to get the integer part
    }

    static float EPSG4326_to_EPSG3857_latitude(float epsg4326Lat) {
        return log(tan(epsg4326Lat) + 1.0 / cos(epsg4326Lat));
    }

    static float EPSG3857_to_EPSG4326_latitude(float epsg3857Lat) {
        return atan(sinh(epsg3857Lat));
    }

    static float EPSG4326_to_EPSG3857_longitude(float epsg4326Lon) {
        return epsg4326Lon;
    }
};

class CommonUtils {
public:
    static std::vector<GLfloat> toOpenGlColor(CSSColorParser::Color color) {
        GLfloat red   = static_cast<GLfloat>(color.r) / 255;
        GLfloat green = static_cast<GLfloat>(color.g) / 255;
        GLfloat blue  = static_cast<GLfloat>(color.b) / 255;
        GLfloat alpha = static_cast<GLfloat>(color.a);
        return { red, green, blue, alpha };
    }

    static float longitudeToFlatCameraZ(float longitude, float planetRadius) {
        return -(longitude / M_PI) * planetRadius;
    }

    static float latitudeToFlatCameraY(float latitude, float planetRadius) {
        return -(CommonUtils::latitudeToTile(0, latitude) * 2 - 1) * planetRadius;
    }

    static float randomFloatInRange(float min, float max) {
        return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
    }

    static float latitudeToTile(int zoom, float latitude) {
        return pow(2, zoom) * (1 - (std::log(std::tan(latitude) + (1 / std::cos(latitude))) / M_PI)) / 2;
    }

    static float tileLongitude(float xTile, int n) {
        return xTile / n * 2 * M_PI - M_PI;
    }

    static float tileLatitude(float yTile, int n) {
        float y = M_PI * (1 - 2 * yTile / n);
        return std::atan(std::sinh(y));
    }

    static double clamp(double n, double lower, double upper) {
        return std::max(lower, std::min(n, upper));
    }

    static float clampf(float n, float lower, float upper) {
        return std::max(lower, std::min(n, upper));
    }

    static void printGlError() {
        LOGI("Open gl error %s", getGLErrorString().c_str());
    }

    static bool compareFloats(float a, float b, float epsilon = 1e-5) {
        return std::fabs(a - b) < epsilon;
    }

    static float normalizeLongitudeRad(float rad) {
        float result = fmod(rad + M_PI, 2 * M_PI);
        if (result < 0) {
            result += 2 * M_PI;
        }
        return result - M_PI;
    }

    static void normalizePlane(float (&plane)[4]) {
        float A = plane[0];
        float B = plane[1];
        float C = plane[2];
        float D = plane[3];
        float length = sqrt(pow(A, 2) + pow(B, 2) + pow(C, 2));
        plane[0] = A / length;
        plane[1] = B / length;
        plane[2] = C / length;
        plane[3] = D / length;
    }

    static float calcDistanceFromPointToPlane(float plane[4], float point[3]) {
        float x0 = point[0];
        float y0 = point[1];
        float z0 = point[2];

        float A = plane[0];
        float B = plane[1];
        float C = plane[2];
        float D = plane[3];

        float distance =
                std::abs(A * x0 + B * y0 + C * z0 + D) /
                sqrt(pow(A, 2.0) + pow(B, 2.0) + pow(C, 2.0));
        return distance;
    }

    static int sign(int x) {
        return (x > 0) - (x < 0);
    }

    static int sign(float x) {
        return (x > 0) - (x < 0);
    }

    static int sign(double x) {
        return (x > 0) - (x < 0);
    }

    static std::string getGLErrorString() {
        switch (glGetError()) {
            case GL_NO_ERROR: return "GL_NO_ERROR";
            case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
            case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
            case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
            case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
            case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
            default: return "UNKNOWN_ERROR";
        }
    }

    static std::string formatFloat(float value) {
        char buffer[10]; // Buffer to hold the formatted string
        std::snprintf(buffer, sizeof(buffer), "%.1f", value); // Format the float to one decimal place
        return std::string(buffer); // Convert to std::string and return
    }

    static float latitudeToSphere(float latitudeRad) {
        float lowerStackRad = (float) DEG2RAD(-degLatitudeConstraint);
        float highStackRad = (float) DEG2RAD(degLatitudeConstraint);
        float stackRadClipped = std::max(lowerStackRad, std::min(latitudeRad, highStackRad));
        float stackCord = (std::log(std::tan(stackRadClipped) + 1.0 / std::cos(stackRadClipped)));
        return stackCord;
    }

    // преобразует угол в радианах в позицию от 0 до 1 сферы
    static float latitudeRadToY(float latitudeRad) {
        float lowerStackRad = (float) DEG2RAD(-degLatitudeConstraint);
        float highStackRad = (float) DEG2RAD(degLatitudeConstraint);
        float stackRadClipped = std::max(lowerStackRad, std::min(latitudeRad, highStackRad));
        float stackCord = (std::log(std::tan(stackRadClipped) + 1.0 / std::cos(stackRadClipped)));
        float stackCord_n = 0.5 - stackCord / (2.0f * M_PI);
        float y = std::min(std::max(0.0, 1.0 - stackCord_n), 1.0);
        return y;
    }

    static float latitudeRadToSphereLatitudeRad(float latitudeRad) {
        float lowerStackRad = (float) DEG2RAD(-degLatitudeConstraint);
        float highStackRad = (float) DEG2RAD(degLatitudeConstraint);
        float stackRadClipped = std::max(lowerStackRad, std::min(latitudeRad, highStackRad));
        float stackCord = (std::log(std::tan(stackRadClipped) + 1.0 / std::cos(stackRadClipped)));
        return stackCord;
    }

    // longitudeRad should be in -M_PI to M_PI
    static float longitudeRadToX(float longitudeRad) {
        return 0.5 + RAD2DEG(longitudeRad) / 360;
    }

    static float fract(float value) {
        return value - std::floor(value);
    }
private:
    //constexpr static const float degLatitudeConstraint = 85.0511f;
    constexpr static const float degLatitudeConstraint = 89.9f;
};

#endif //TUSA_ANDROID_LOG_H
