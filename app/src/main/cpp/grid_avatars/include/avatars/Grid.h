//
// Created by Artem on 19.12.2024.
//

#ifndef TUSA_ANDROID_AVATARS_GRID_H
#define TUSA_ANDROID_AVATARS_GRID_H

#include <vector>
#include <map>
#include "GridNode.h"
#include "GridElement.h"
#include "Circle.h"


namespace Avatars {
    class Grid {
    public:
        void init(int width, int height, int widthCellsCount, int heightCellsCount);

        bool insert(Circle& circle);
        void clean();
    private:
        int widthCellsCount;
        int heightCellsCount;
        int widthCellSize;
        int heightCellSize;
        std::vector<GridNode*> nodes;
        std::vector<GridElement> elements;

        std::unordered_map<uint64_t, Circle> circles;
        std::unordered_map<uint16_t, uint64_t> toBoxId;
        uint16_t increment;
    };
}



#endif //TUSA_ANDROID_AVATARS_GRID_H
