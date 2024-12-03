//
// Created by Artem on 03.12.2024.
//

#include "Grid.h"

void Grid::init(int width, int height, int widthCellsCount, int heightCellsCount) {
    this->widthCellsCount = widthCellsCount;
    this->heightCellsCount = heightCellsCount;
    widthCellSize = width / widthCellsCount;
    heightCellSize = height / heightCellsCount;
    nodes = std::vector<GridNode*>(widthCellsCount * heightCellsCount);
}

bool Grid::insert(Box box) {
    int startCellX = box.lb_x / widthCellSize;
    int endCellX = box.rt_x / widthCellSize;
    int startCellY = box.rt_y / heightCellSize;
    int endCellY = box.lb_y / heightCellSize;

    startCellX = std::clamp(startCellX, 0, widthCellsCount - 1);
    endCellX = std::clamp(endCellX, 0, widthCellsCount - 1);
    startCellY = std::clamp(startCellY, 0, heightCellsCount - 1);
    endCellY = std::clamp(endCellY, 0, heightCellsCount - 1);

    // проверка пересечений
    for (int cellX = startCellX; cellX <= endCellX; cellX++) {
        for (int cellY = startCellY; cellY <= endCellY; cellY++) {
            int index = cellX + cellY * widthCellsCount;
            auto node = nodes[index];
            if (node == nullptr) continue;

            int next = node->elementIndex;
            while(next != -1) {
                auto element = elements[next];
                auto& otherBox = boxes[element.boxId];
                bool intersects = otherBox.intersects(box);
                if (intersects) return false;
                next = element.next;
            }
        }
    }

    // вставка
    boxes[box.titleId] = box;
    for (int cellX = startCellX; cellX <= endCellX; cellX++) {
        for (int cellY = startCellY; cellY <= endCellY; cellY++) {
            int index = cellX + cellY * widthCellsCount;
            auto node = nodes[index];

            elements.push_back(GridElement { -1, box.titleId });
            if (node == nullptr) {
                nodes[index] = new GridNode {static_cast<int>(elements.size() - 1)};
                continue;
            }

            int lastElementIndex = node->elementIndex;
            while (true) {
                auto element = elements[lastElementIndex];
                if (element.next == -1) break;
                lastElementIndex = element.next;
            }
            elements[lastElementIndex].next = elements.size() - 1;
        }
    }
    return true;
}

void Grid::clean() {
    for (auto node : nodes) {
        delete node;
    }
}
