//
// Created by Artem on 03.12.2024.
//

#ifndef TUSA_ANDROID_GRID_H
#define TUSA_ANDROID_GRID_H

#include <vector>
#include <map>
#include "GridNode.h"
#include "GridElement.h"


struct Grid {
public:
    void init(int width, int height, int widthCellsCount, int heightCellsCount);

    bool insert(Box& box, int& checks);
    void clean();
private:
    int widthCellsCount;
    int heightCellsCount;
    int widthCellSize;
    int heightCellSize;
    std::vector<GridNode*> nodes;
    std::vector<GridElement> elements;

    std::unordered_map<uint64_t, Box> boxes;
    std::unordered_map<uint16_t, uint64_t> toBoxId;
    uint16_t increment;
};


#endif //TUSA_ANDROID_GRID_H
