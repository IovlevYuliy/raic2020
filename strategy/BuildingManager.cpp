#include "BuildingManager.hpp"

BuildingManager::BuildingManager() {}
BuildingManager::BuildingManager(GameState& state_) {
    state = &state_;
}

optional<int> BuildingManager::createBuilding(unordered_map<int, EntityAction>& actions, EntityType buildingType) {
    uint size = state->entityProperties[buildingType].size;

    auto& builderBase = *find_if(state->myBases.begin(), state->myBases.end(), [](const Entity& entry){
        return entry.entityType == EntityType::BUILDER_BASE;
    });

    auto builder = getNearestBuilder(builderBase).second;

    if (!builder.playerId) {
        return {};
    }

    auto foundPlace = findPlace(builder.position, size);
    if (foundPlace.has_value()) {
        actions[builder.id] = EntityAction(
            MoveAction(Vec2Int(foundPlace.value().x - 1, foundPlace.value().y), true,false),
            BuildAction(buildingType, foundPlace.value())
        );

        return builder.id;
    }

    return {};
}

void BuildingManager::repairBuildings(unordered_map<int, EntityAction>& actions) {
    vector<Entity> buildingsForRepair;
    for (auto& entry : state->myEntities) {
        if (isRepairingBuilding(entry) &&
                entry.health < state->entityProperties[entry.entityType].maxHealth) {
            buildingsForRepair.push_back(entry);
        }
    }

    for (auto& entry : buildingsForRepair) {
        auto res = getNearestBuilder(entry);
        if (res.first.x != -1) {
            actions[res.second.id] = EntityAction(
                MoveAction(Vec2Int(res.first.x - 1, res.first.y), true,false),
                {}, // build
                {}, // attack
                RepairAction(entry.id)
            );
        }
    }
}

pair<Vec2Int, Entity> BuildingManager::getNearestBuilder(Entity& destEntity) {
    Entity foundBuilder;
    uint minDist = 1e9;
    Vec2Int pos;
    bool avoidStuck = rand() % 50 == 0;

    for (uint i = 0; i < (uint)state->myEntities.size(); ++i) {
        if (!isBuilder(state->myEntities[i])) {
            continue;
        }
        auto res = getDistance(state->myEntities[i], destEntity, state->entityProperties);
        if (avoidStuck) {
            return make_pair(res.second, state->myEntities[i]);
        }

        if (res.first < minDist) {
            minDist = res.first;
            pos = res.second;
            foundBuilder = state->myEntities[i];
        }
    }

    return make_pair(pos, foundBuilder);
}

optional<Vec2Int> BuildingManager::findPlace(Vec2Int start, uint size) {
    uint& mapSize = state->mapSize;
    auto checkPlace = [&size, &st = state, &mapSize](Vec2Int& s) {
        for (uint i = s.x; i < s.x + size; ++i) {
            for (uint j = s.y; j < s.y + size; ++j) {
                if (i >= mapSize || j >= mapSize || st->gameMap[i][j] != -1)
                    return false;
            }
        }
        return true;
    };

    queue<Vec2Int> q;
    unordered_set<Vec2Int> visited;
    q.push(start);
    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();

        bool ok = checkPlace(v) && checkNeighbors(v, size);
        if (ok) {
            return v;
        }

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, mapSize) || state->gameMap[to.x][to.y] != -1 || visited.count(to)) {
                continue;
            }
            q.push(to);
            visited.insert(to);
        }
    }

    return {};
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
