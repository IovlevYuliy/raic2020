#include "BuildingManager.hpp"

BuildingManager::BuildingManager() {}
BuildingManager::BuildingManager(GameState& state_) {
    state = &state_;
}

optional<int> BuildingManager::createBuilding(unordered_map<int, EntityAction>& actions, EntityType buildingType) {
    if(state->curBuilderCount < 1) {
        return {};
    }

    uint size = state->entityProperties[buildingType].size;
    optional<Vec2Int> foundPlace;
    if (buildingType == EntityType::TURRET) {
        foundPlace = findTurretPlace(Vec2Int(0, 0), size);
    } else {
        foundPlace = findPlace(Vec2Int(0, 0), size, 4);
    }

    if (foundPlace) {
        auto builders = getNearestBuilders(foundPlace.value(), repairBuilderCount[buildingType] + 1, size);
        for (uint i = 0; i < static_cast<uint>(builders.size()); ++i) {
            actions[builders[i].first->id] = EntityAction(
                MoveAction(builders[i].second, true,false),
                BuildAction(buildingType, foundPlace.value())
            );
        }
    }

    return {};
}

void BuildingManager::repairBuildings(unordered_map<int, EntityAction>& actions) {
    vector<Entity> buildingsForRepair;
    for (auto& entry : state->myBuildings) {
        if (isRepairingBuilding(entry) &&
                entry.health < state->entityProperties[entry.entityType].maxHealth) {
            buildingsForRepair.push_back(entry);
        }
    }

    for (auto& entry : buildingsForRepair) {
        int diffHp = state->entityProperties[entry.entityType].maxHealth - entry.health;
        int builderCount = min((diffHp + 4) / 5, repairBuilderCount[entry.entityType] + 1);
        auto builders = getNearestBuilders(entry.position, builderCount,
            state->entityProperties[entry.entityType].size);
        for (uint i = 0; i < static_cast<uint>(builders.size()); ++i) {
            actions[builders[i].first->id] = EntityAction(
                MoveAction(builders[i].second, true, false),
                {},  // build
                {},  // attack
                RepairAction(entry.id));
        }
    }
}

vector<pair<Entity*, Vec2Int>> BuildingManager::getNearestBuilders(Vec2Int pos, uint count, int size) {
    vector<pair<int, int>> q;
    for (uint i = 0; i < static_cast<uint>(state->myBuilders.size()); ++i) {
        uint sqrDist = pos.sqrDist(state->myBuilders[i].position);
        q.push_back(make_pair(sqrDist, i));
    }
    sort(q.begin(), q.end());

    count = min(count, static_cast<uint>(q.size()));
    vector<pair<Entity*, Vec2Int>> builders;
    auto borders = getBorder(pos, size);
    vector<bool> used(borders.size(), false);
    for (int i = 0; i < count; ++i) {
        if (i && q[i].first > 100) {
            break;
        }
        uint minDist = 1e9;
        uint ind = -1;
        for (uint j = 0; j < static_cast<uint>(borders.size()); ++j) {
            if (used[j] || isOutOfMap(borders[j], state->mapSize)) {
                continue;
            }
            auto dist = borders[j].dist(state->myBuilders[q[i].second].position);
            if (dist < minDist) {
                minDist = dist;
                ind = j;
            }
        }
        if (ind != -1) {
            used[ind] = true;
            builders.push_back(make_pair(&state->myBuilders[q[i].second], borders[ind]));
        }
    }

    return builders;
}

pair<Vec2Int, Entity> BuildingManager::getNearestBuilder(Entity& destEntity) {
    Entity foundBuilder;
    uint minDist = 1e9;
    Vec2Int pos;
    bool avoidStuck = rand() % 50 == 0;

    for (uint i = 0; i < static_cast<uint>(state->myBuilders.size()); ++i) {
        auto res = getDistance(state->myBuilders[i], destEntity, state->entityProperties);
        if (avoidStuck) {
            return make_pair(res.second, state->myBuilders[i]);
        }

        if (res.first < minDist) {
            minDist = res.first;
            pos = res.second;
            foundBuilder = state->myBuilders[i];
        }
    }

    return make_pair(pos, foundBuilder);
}

optional<Vec2Int> BuildingManager::findPlace(Vec2Int start, uint size, uint divider) {
    uint mapSize = state->mapSize;

    queue<Vec2Int> q;
    unordered_set<Vec2Int> visited;
    q.push(start);
    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();

        bool ok = isFree(v, size) && checkNeighbors(v, size);
        if (ok && v.x % divider == 0 && v.y % divider == 0) {
            return v;
        }

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, mapSize) || visited.count(to)) {
                continue;
            }
            q.push(to);
            visited.insert(to);
        }
    }

    return {};
}

bool BuildingManager::isFree(Vec2Int& pos, uint size) {
    uint mapSize = state->mapSize;

    for (uint i = pos.x; i < pos.x + size; ++i) {
        for (uint j = pos.y; j < pos.y + size; ++j) {
            if (i >= mapSize || j >= mapSize || state->gameMap[i][j] != -1 || state->infMap[i][j] < 0) {
                return false;
            }
        }
    }

    return true;
}

bool BuildingManager::isForbidden(Vec2Int& pos) {
    uint& mapSize = state->mapSize;
    for (uint i = 0; i < 4; ++i) {
        uint nx = pos.x + dx[i];
        uint ny = pos.y + dy[i];
        if (nx >= 0 && nx < mapSize && ny >= 0 && ny < mapSize &&
                isBuilding((EntityType)state->gameMap[nx][ny])) {
            return true;
        }
    }
    return false;
}

bool BuildingManager::checkNeighbors(Vec2Int& pos, uint entrySize) {
    bool ok = true;
    Vec2Int cur;
    for (uint i = 0; i < entrySize; ++i) {
        cur.x = pos.x + i;
        cur.y = pos.y;
        ok = ok & (!isForbidden(cur));
        cur.y = pos.y + entrySize - 1;
        ok = ok & (!isForbidden(cur));
        cur.x = pos.x;
        cur.y = pos.y + i;
        ok = ok & (!isForbidden(cur));
        cur.x = pos.x + entrySize - 1;
        ok = ok & (!isForbidden(cur));
    }

    return ok;
}

optional<Vec2Int> BuildingManager::findTurretPlace(Vec2Int start, uint size) {
    uint mapSize = state->mapSize;

    queue<Vec2Int> q;
    unordered_set<Vec2Int> visited;
    q.push(start);
    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();

        bool ok = isFree(v, size);
        if (ok && v.x > 5 && v.y > 5) {
            int regDang = state->getRegionInfluence(v, 5);
            if (regDang >= -2 && regDang <= 4) {
                return v;
            }
        }

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, mapSize) || visited.count(to)) {
                continue;
            }
            q.push(to);
            visited.insert(to);
        }
    }

    return {};
}