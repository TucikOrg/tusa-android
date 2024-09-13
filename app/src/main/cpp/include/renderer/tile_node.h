//
// Created by Artem on 30.07.2024.
//

#ifndef TUSA_ANDROID_TILE_NODE_H
#define TUSA_ANDROID_TILE_NODE_H


#include "tile_cords.h"
#include "map/tiles_storage.h"

class TileNode {
public:
    TileNode(TileCords tile): tile(tile) { }

    TileNode* parent = nullptr;
    TileNode* child00 = nullptr;
    TileNode* child01 = nullptr;
    TileNode* child10 = nullptr;
    TileNode* child11 = nullptr;

    TileCords tile;

    TileNode* searchAndCreate(TileCords tileCords, bool& isReplacement, TileNode* ifNoData, TilesStorage storage) {
        if (tile.is(tileCords)) {
            if (tile.isEmpty()) {

                // Cмотрим в хранилище если данных вектора нету в дереве
                auto tileData = storage.getTile(tile.getTileZ(), tile.getTileX(), tile.getTileY());
                if (tileData != nullptr) {
                    tile.setData(tileData);
                    return this;
                }

                isReplacement = true;
                if (ifNoData == nullptr) {
                    throw std::runtime_error("ifNoData is nullptr.");
                }
                return ifNoData;
            }
            return this;
        }

        TileNode* useIfNoData = nullptr;
        if (!tile.isEmpty()) {
            useIfNoData = this;
        } else {
            useIfNoData = ifNoData;
        }

        if (child00 == nullptr) {
            auto cord = TileCords(tile.getTileX() * 2, tile.getTileY() * 2, tile.getTileZ() + 1);
            child00 = new TileNode(cord);
            child00->parent = this;
        }
        if (child00->tile.cover(tileCords)) {
            return child00->searchAndCreate(tileCords, isReplacement, useIfNoData, storage);
        }

        if(child01 == nullptr) {
            auto cord = TileCords(tile.getTileX() * 2, tile.getTileY() * 2 + 1, tile.getTileZ() + 1);
            child01 = new TileNode(cord);
            child01->parent = this;
        }
        if (child01->tile.cover(tileCords)) {
            return child01->searchAndCreate(tileCords, isReplacement, useIfNoData, storage);
        }

        if(child10 == nullptr) {
            auto cord = TileCords(tile.getTileX() * 2 + 1, tile.getTileY() * 2, tile.getTileZ() + 1);
            child10 = new TileNode(cord);
            child10->parent = this;
        }
        if (child10->tile.cover(tileCords)) {
            return child10->searchAndCreate(tileCords, isReplacement, useIfNoData, storage);
        }

        if(child11 == nullptr) {
            auto cord = TileCords(tile.getTileX() * 2 + 1, tile.getTileY() * 2 + 1, tile.getTileZ() + 1);
            child11 = new TileNode(cord);
            child11->parent = this;
        }
        if (child11 != nullptr && child11->tile.cover(tileCords)) {
            return child11->searchAndCreate(tileCords, isReplacement, useIfNoData, storage);
        }

        isReplacement = true;
        return useIfNoData;
    }

    bool containsData() {
        return !tile.isEmpty();
    }
};


#endif //TUSA_ANDROID_TILE_NODE_H
