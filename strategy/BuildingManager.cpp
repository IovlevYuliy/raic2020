#include "BuildingManager.hpp"

BuildingManager::BuildingManager() {}

void BuildingManager::createBuilding(vector<Entity>& myEntities, vector<vector<char> >& gameMap,
        unordered_map<int, EntityAction>& actions, EntityType buildingType) {
    uint size = entityProperties[buildingType].size;

    auto& builderBase = *find_if(myEntities.begin(), myEntities.end(), [](const Entity& entry){
        return entry.entityType == EntityType::BUILDER_BASE;
    });
    auto builder = getNearestBuilder(myEntities, builderBase).second;

    if (!builder.playerId) {
        return;
    }

    auto foundPlace = findPlace(gameMap, builder.position, size);
    if (foundPlace.has_value()) {
        cerr << "place for building " << foundPlace.value().x << ' ' << foundPlace.value().y << endl;

        actions[builder.id] = EntityAction(
            MoveAction(foundPlace.value(), true,false),
            BuildAction(buildingType, foundPlace.value())
        );
    }
}

void BuildingManager::repairBuildings(vector<Entity>& myEntities,
        unordered_map<int, EntityAction>& actions) {
    vector<Entity>buildingsForRepair;
    for (auto& entry : myEntities) {
        if (isRepairingBuilding(entry) &&
                entry.health < entityProperties[entry.entityType].maxHealth) {
            buildingsForRepair.push_back(entry);
        }
    }

    for (auto& entry : buildingsForRepair) {
        auto res = getNearestBuilder(myEntities, entry);
        if (res.first.x != -1) {
            actions[res.second.id] = EntityAction(
                MoveAction(res.first, true,false),
                {}, // build
                {}, // attack
                RepairAction(entry.id)
            );
        }
    }
}

pair<Vec2Int, Entity> BuildingManager::getNearestBuilder(vector<Entity>& myEntities, Entity& destEntity) {
    uint minDist = 1e9;
    Vec2Int pos(-1, -1);
    Entity foundBuilder;
    for (auto& entry : myEntities) {
        if (!isBuilder(entry)) {
            continue;
        }
        auto r = getDistance(entry, destEntity, entityProperties);
        if (r.first < minDist) {
            minDist = r.first;
            pos = r.second;
            foundBuilder = entry;
        }
    }

    return make_pair(pos, foundBuilder);
}

optional<Vec2Int> BuildingManager::findPlace(vector<vector<char> >& gameMap, Vec2Int start, uint size) {
    queue<Vec2Int> q;
    q.push(start);

    uint mapSize = (uint)gameMap.size();
    auto checkPlace = [&size, &gameMap, &mapSize](Vec2Int& s) {
        for (uint i = s.x; i < s.x + size; ++i) {
            for (uint j = s.y; j < s.y + size; ++j) {
                if (i >= mapSize || j >= mapSize || gameMap[i][j] != -1)
                    return false;
            }
        }
        return true;
    };

    unordered_set<Vec2Int> visited;
    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();
        visited.insert(v);

        bool ok = checkPlace(v) && checkNeighbors(v, size, gameMap);
        if (ok) {
            return v;
        }

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, mapSize) || gameMap[to.x][to.y] != -1 || visited.count(to)) {
                continue;
            }
            q.push(to);
        }
    }

    return {};
}

bool BuildingManager::isForbidden(Vec2Int& pos, vector<vector<char>>& gameMap) {
    uint mapSize = (uint)gameMap.size();
    for (uint i = 0; i < 4; ++i) {
        uint nx = pos.x + dx[i];
        uint ny = pos.y + dy[i];
        if (nx >= 0 && nx < mapSize && ny >= 0 && ny < mapSize &&
                isBase((EntityType)gameMap[nx][ny])) {
            return true;
        }
    }
    return false;
}

bool BuildingManager::checkNeighbors(Vec2Int& pos, uint entrySize, vector<vector<char>>& gameMap) {
    bool ok = true;
    Vec2Int cur;
    for (uint i = 0; i < entrySize; ++i) {
        cur.x = pos.x + i;
        cur.y = pos.y;
        ok = ok & (!isForbidden(cur, gameMap));
        cur.y = pos.y + entrySize - 1;
        ok = ok & (!isForbidden(cur, gameMap));
        cur.x = pos.x;
        cur.y = pos.y + i;
        ok = ok & (!isForbidden(cur, gameMap));
        cur.x = pos.x + entrySize - 1;
        ok = ok & (!isForbidden(cur, gameMap));
    }

    return ok;
}
