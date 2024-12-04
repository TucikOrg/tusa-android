//
// Created by Artem on 03.12.2024.
//

#include "Grid.h"
#include "util/android_log.h"

void Grid::init(int width, int height, int widthCellsCount, int heightCellsCount) {
    this->widthCellsCount = widthCellsCount;
    this->heightCellsCount = heightCellsCount;
    widthCellSize = width / widthCellsCount;
    heightCellSize = height / heightCellsCount;
    nodes = std::vector<GridNode*>(widthCellsCount * heightCellsCount);
}

bool Grid::insert(Box& box, int& checks) {
    int startCellX = box.lb_x / widthCellSize;
    int endCellX = box.rt_x / widthCellSize;
    int startCellY = box.rt_y / heightCellSize;
    int endCellY = box.lb_y / heightCellSize;

    startCellX = std::clamp(startCellX, 0, widthCellsCount - 1);
    endCellX = std::clamp(endCellX, 0, widthCellsCount - 1);
    startCellY = std::clamp(startCellY, 0, heightCellsCount - 1);
    endCellY = std::clamp(endCellY, 0, heightCellsCount - 1);
    int checksCurrent = 0;

    // проверка пересечений
    for (int cellX = startCellX; cellX <= endCellX; cellX++) {
        for (int cellY = startCellY; cellY <= endCellY; cellY++) {
            int index = cellX + cellY * widthCellsCount;
            auto node = nodes[index];
            if (node == nullptr) continue;
            if (node->count >= 1) {

            }

            int next = node->elementIndex;
            while(next != 0) {
                auto element = elements[next - 1];
                auto& otherBox = boxes[toBoxId[element.forToBoxId]];
                bool intersects = otherBox.intersects(box);
                checksCurrent++;
                if (checks < checksCurrent) {
                    checks = checksCurrent;
                }
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

            toBoxId[increment] = box.titleId;
            elements.push_back(GridElement { 0, increment++ });
            if (node == nullptr) {
                nodes[index] = new GridNode {static_cast<uint16_t>(elements.size()), 1};
                continue;
            }

            int lastElementIndex = node->elementIndex;
            while (true) {
                auto element = elements[lastElementIndex - 1];
                if (element.next == 0) break;
                lastElementIndex = element.next;
            }
            elements[lastElementIndex - 1].next = elements.size();
            node->count++;
        }
    }
    return true;
}

void Grid::clean() {
    for (auto node : nodes) {
        if (node != nullptr) delete node;
    }
}
