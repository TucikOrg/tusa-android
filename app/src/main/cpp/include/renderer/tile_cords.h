//
// Created by Artem on 09.04.2024.
//

#ifndef TUSA_ANDROID_TILE_CORDS_H
#define TUSA_ANDROID_TILE_CORDS_H

#include <iostream>
#include "map/tile.h"

class TileCords {
public:
    TileCords(int tileX, int tileY, int tileZ) : tileX(tileX), tileY(tileY), tileZ(tileZ) {
        key = std::to_string(tileX) + std::to_string(tileY) + std::to_string(tileZ);
    }
    TileCords() = default;
private:
    int tileX;
    int tileY;
    int tileZ;

    float longitudeRad = 0;
    float latitudeRad = 0;

    float z;
    float y;

    Tile* drawGeometry = nullptr;
    bool hasLatitudeAndLongitudeRad = false;
    std::string key;

public:

    void setY(float y) {
        this->y = y;
    }

    void setZ(float z) {
        this->z = z;
    }

    int getY() {
        return y;
    }

    int getZ() {
        return z;
    }

    bool hasLatitudeAndLongitude() {
        return hasLatitudeAndLongitudeRad;
    }

    Tile* getDrawGeometry() {
        return drawGeometry;
    }

    float getLongitudeRad() {
        return longitudeRad;
    }

    float getLatitudeRad() {
        return latitudeRad;
    }

    void setLatitudeAndLongitudeRad(float latitudeRad, float longitudeRad, bool has) {
        this->latitudeRad = latitudeRad;
        this->longitudeRad = longitudeRad;
        hasLatitudeAndLongitudeRad = has;
    }

    std::string getKey() {
        return key;
    }

    int getTileX() {
        return tileX;
    }

    int getTileY() {
        return tileY;
    }

    int getTileZ() {
        return tileZ;
    }

    void setData(Tile* tile) {
        drawGeometry = tile;
    }

    std::string toString() {
        return "(" + std::to_string(tileX) + ", " + std::to_string(tileY) + ", " + std::to_string(tileZ) + ")";
    }

    bool is(TileCords otherTile) {
        return otherTile.tileX == tileX && otherTile.tileZ == tileZ && otherTile.tileY == tileY;
    }

    bool cover(TileCords otherTile) {
        if (tileZ > otherTile.tileZ) {
            return false;
        }

        // Коэффициент масштаба между уровнями зума
        int scale = 1 << (otherTile.tileZ - tileZ);

        // Преобразуем координаты тайла1 на уровень зума тайла2
        int transformed_x1 = tileX * scale;
        int transformed_y1 = tileY * scale;

        // Проверяем, покрывает ли тайл1 тайл2
        return (transformed_x1 <= otherTile.tileX && otherTile.tileX < transformed_x1 + scale) &&
               (transformed_y1 <= otherTile.tileY && otherTile.tileY < transformed_y1 + scale);

    }

    void clear() {
        drawGeometry = nullptr;
    }

    bool isEmpty() {
        return drawGeometry == nullptr;
    }
};


#endif //TUSA_ANDROID_TILE_CORDS_H
