//
// Created by Artem on 19.12.2024.
//

#include "avatars/Grid.h"
#include "util/android_log.h"


void Avatars::Grid::init(int width, int height, int widthCellsCount, int heightCellsCount) {
    this->widthCellsCount = widthCellsCount;
    this->heightCellsCount = heightCellsCount;
    widthCellSize = width / widthCellsCount;
    heightCellSize = height / heightCellsCount;
    nodes = std::vector<GridNode*>(widthCellsCount * heightCellsCount);
}

bool Avatars::Grid::insert(Circle& circle) {
    int startCellX = (circle.x - circle.radius) / widthCellSize;
    int endCellX = (circle.x + circle.radius) / widthCellSize;
    int startCellY = (circle.y + circle.radius) / heightCellSize;
    int endCellY = (circle.y - circle.radius) / heightCellSize;

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

            int next = node->elementIndex;
            while(next != 0) {
                auto element = elements[next - 1];
                auto& otherCircle = circles[toBoxId[element.forToBoxId]];
                int dx = 0;
                int dy = 0;
                int intersectLength = 0;
                bool intersects = otherCircle.intersects(circle, dx, dy, intersectLength);
                checksCurrent++;
                next = element.next;
            }
        }
    }


    // вставка
    circles[circle.id] = circle;
    for (int cellX = startCellX; cellX <= endCellX; cellX++) {
        for (int cellY = startCellY; cellY <= endCellY; cellY++) {
            int index = cellX + cellY * widthCellsCount;
            auto node = nodes[index];

            toBoxId[increment] = circle.id;
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

void  Avatars::Grid::clean() {
    for (auto node : nodes) {
        if (node != nullptr) delete node;
    }
}