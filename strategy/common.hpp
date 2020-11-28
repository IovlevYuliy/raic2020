#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <optional>
#include <iostream>
#include <algorithm>
#include <functional>
#include <queue>

#include "model/Model.hpp"

using namespace std;

const float MAX_BUILDERS_PERCENTAGE = 0.4;
const int dx[4] = {0, 0, 1, -1};
const int dy[4] = {1, -1, 0, 0};

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

#endif