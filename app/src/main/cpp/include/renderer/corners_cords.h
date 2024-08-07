//
// Created by Artem on 14.07.2024.
//

#ifndef TUSA_ANDROID_CORNERS_CORDS_H
#define TUSA_ANDROID_CORNERS_CORDS_H


class CornersCords {
public:
    double leftTopLongitudeRad;
    double leftTopLatitudeRad;
    double rightBottomLongitudeRad;
    double rightBottomLatitudeRad;
    bool hasLeftTop;
    bool hasRightBottom;

    double topRightLongitudeRad;
    double topRightLatitudeRad;
    bool hasTopRight;

    float leftTopY;
    float leftTopZ;

    float rightBottomY;
    float rightBottomZ;
};


#endif //TUSA_ANDROID_CORNERS_CORDS_H
