#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <algorithm>
#include <cmath>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "model/Model.hpp"

using namespace std;

const float MAX_BUILDERS_PERCENTAGE = 0.4;
const int MAX_TURRET = 25;
const int MAX_RANGED_BASE = 1;
const int MAX_BUILDER_BASE = 1;
const int MAX_BUILDERS = 50;
const int BUILDER_DELAY_TICKS = 10;
const int DEFENSE_THRESHOLD = 20;
const int RESOURCE_THRESHOLD = 100;

const int dx[4] = {0, 0, 1, -1};
const int dy[4] = {1, -1, 0, 0};

inline bool isOutOfMap(Vec2Int& pos, uint mapSize) {
    return pos.x < 0 || pos.x >= mapSize || pos.y < 0 || pos.y >= mapSize;
}

inline bool isBase(EntityType type) {
    return type == EntityType::BUILDER_BASE ||
        type == EntityType::MELEE_BASE ||
        type == EntityType::RANGED_BASE;
}

inline bool isBuilding(EntityType type) {
    return type == EntityType::BUILDER_BASE ||
        type == EntityType::MELEE_BASE ||
        type == EntityType::RANGED_BASE ||
        type == EntityType::HOUSE ||
        type == EntityType::TURRET ||
        type == EntityType::WALL;
}

inline bool isTurret(const Entity& entry) {
    return entry.entityType == EntityType::TURRET;
}

inline bool isRepairingBuilding(const Entity& entry) {
    return entry.entityType == EntityType::BUILDER_BASE ||
        entry.entityType == EntityType::HOUSE ||
        entry.entityType == EntityType::RANGED_BASE ||
        entry.entityType == EntityType::TURRET;
}

inline bool isBuilder(const Entity& entry) {
    return entry.entityType == EntityType::BUILDER_UNIT;
}

inline bool isRanger(const Entity& entry) {
    return entry.entityType == EntityType::RANGED_UNIT;
}

inline bool isMelee(const Entity& entry) {
    return entry.entityType == EntityType::MELEE_UNIT;
}

inline bool isUnit(const Entity& entry) {
    return isBuilder(entry) || isRanger(entry) || isMelee(entry);
}

inline bool isUnit(const EntityType type) {
    return type == EntityType::BUILDER_UNIT || type == EntityType::RANGED_UNIT || type == EntityType::MELEE_UNIT;
}

inline vector<Vec2Int> getBuildingBorder(Vec2Int& pos, int size) {
    vector<Vec2Int> borders;
    if (size == 1) {
        borders.push_back(pos);
        return borders;
    }

    for (uint i = pos.x; i < pos.x + size; ++i) {
        borders.push_back(Vec2Int(i, pos.y - 1));
        borders.push_back(Vec2Int(i, pos.y + size));
    }

    for (uint i = pos.y; i < pos.y + size; ++i) {
        borders.push_back(Vec2Int(pos.x - 1, i));
        borders.push_back(Vec2Int(pos.x + size, i));
    }

    return borders;
}

inline vector<Vec2Int> getBorder(Vec2Int pos, int size) {
    vector<Vec2Int> borders;
    if (size == 1) {
        borders.push_back(pos);
        return borders;
    }

    for (uint i = pos.x; i < pos.x + size; ++i) {
        borders.push_back(Vec2Int(i, pos.y));
        borders.push_back(Vec2Int(i, pos.y + size - 1));
    }

    for (uint i = pos.y + 1; i < pos.y + size - 1; ++i) {
        borders.push_back(Vec2Int(pos.x, i));
        borders.push_back(Vec2Int(pos.x + size - 1, i));
    }

    return borders;
}

inline pair<uint, Vec2Int> getDistance(Entity& e1, Entity& e2, unordered_map<EntityType, EntityProperties>& entityProperties) {
    vector<Vec2Int> e1Borders = getBorder(e1.position, entityProperties[e1.entityType].size);
    vector<Vec2Int> e2Borders = getBorder(e2.position, entityProperties[e2.entityType].size);

    uint minDist = 1e9;
    Vec2Int destVec;
    for (uint i = 0; i < (uint)e1Borders.size(); ++i) {
        for (uint j = 0; j < (uint)e2Borders.size(); ++j) {
            uint d = e1Borders[i].dist(e2Borders[j]);
            if (d < minDist) {
                minDist = d;
                destVec = e2Borders[j];
            }
        }
    }

    return make_pair(minDist, destVec);
}

inline pair<uint, Vec2Int> getDistance(Entity& e1, Vec2Int& pos, EntityType type, unordered_map<EntityType, EntityProperties>& entityProperties) {
    vector<Vec2Int> e1Borders = getBorder(e1.position, entityProperties[e1.entityType].size);
    vector<Vec2Int> e2Borders = getBorder(pos, entityProperties[type].size);

    uint minDist = 1e9;
    Vec2Int destVec;
    for (uint i = 0; i < (uint)e1Borders.size(); ++i) {
        for (uint j = 0; j < (uint)e2Borders.size(); ++j) {
            uint d = e1Borders[i].dist(e2Borders[j]);
            if (d < minDist) {
                minDist = d;
                destVec = e2Borders[j];
            }
        }
    }

    return make_pair(minDist, destVec);
}

inline pair<uint, pair<Vec2Int, Entity*>> getNearestTarget(Entity& myEntiry, vector<Entity>& enemies, unordered_map<EntityType, EntityProperties>& entityProperties) {
    uint minDist = 1e9;
    Vec2Int destVec;
    Entity* target;
    for (auto& enemy : enemies) {
        auto res = getDistance(myEntiry, enemy, entityProperties);
        if (res.first < minDist) {
            minDist = res.first;
            destVec = res.second;
            target = &enemy;
        }
    }

    return make_pair(minDist, make_pair(destVec, target));
}

#endif