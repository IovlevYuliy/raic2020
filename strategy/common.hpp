#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <optional>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <functional>
#include <queue>
#include <ctime>

#include "model/Model.hpp"

using namespace std;

const float MAX_BUILDERS_PERCENTAGE = 0.4;
const int MAX_RANGED_BASE = 3;
const int MAX_BUILDERS = 30;
const int BUILDER_DELAY_TICKS = 10;
const int DEFENSE_THRESHOLD = 20;

const int dx[4] = {0, 0, 1, -1};
const int dy[4] = {1, -1, 0, 0};

inline bool isOutOfMap(Vec2Int& pos, uint mapSize) {
    return pos.x < 0 || pos.x >= mapSize || pos.y < 0 || pos.y > mapSize;
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
        type == EntityType::TURRET;
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

inline vector<Vec2Int> getEntityBorder(Entity& e, unordered_map<EntityType, EntityProperties>& entityProperties) {
    vector<Vec2Int> borders;
    uint sz = entityProperties[e.entityType].size;
    for (uint i = e.position.x; i < e.position.x + sz; ++i) {
        borders.push_back(Vec2Int(e.position.x, e.position.y));
        borders.push_back(Vec2Int(e.position.x, e.position.y + sz - 1));
    }

    for (uint i = e.position.y + 1; i < e.position.y + sz - 1; ++i) {
        borders.push_back(Vec2Int(e.position.x, e.position.y));
        borders.push_back(Vec2Int(e.position.x + sz - 1, e.position.y));
    }

    return borders;
}

inline pair<uint, Vec2Int> getDistance(Entity& e1, Entity& e2, unordered_map<EntityType, EntityProperties>& entityProperties) {
    vector<Vec2Int> e1Borders = getEntityBorder(e1, entityProperties);
    vector<Vec2Int> e2Borders = getEntityBorder(e2, entityProperties);

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

#endif