#include "BuildingManager.hpp"

BuildingManager::BuildingManager() {}

void BuildingManager::createBuilding(vector<Entity>& myEntities, vector<vector<char> >& gameMap,
        unordered_map<int, EntityAction>& actions, EntityType buildingType) {
    uint size = entityProperties[buildingType].size;

    auto& builder = *find_if(myEntities.begin(), myEntities.end(), [](const Entity& entry){
        return entry.entityType == EntityType::BUILDER_UNIT;
    });

    auto foundPlace = findPlace(gameMap, builder.position, size);
    if (foundPlace.has_value()) {
        cerr << foundPlace.value().x << ' ' << foundPlace.value().y << endl;
        shared_ptr<BuildAction> buildAction(new BuildAction(
            buildingType,
            foundPlace.value()
        ));

        actions[builder.id] = EntityAction(buildAction);
    }
}

optional<Vec2Int> BuildingManager::findPlace(vector<vector<char> >& gameMap, Vec2Int start, uint size) {
    queue<Vec2Int> q;
    q.push(start);

    auto check = [size, &gameMap](Vec2Int& s) {
        for (uint i = s.x; i < s.x + size; ++i) {
            for (uint j = s.y; j < s.y + size; ++j) {
                if (gameMap[i][j] != -1)
                    return false;
            }
        }
        return true;
    };

    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();

        bool ok = check(v);
        if (ok) {
            return v;
        }

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (gameMap[to.x][to.y] == -1) {
                q.push(to);
            }
        }
    }

    return {};
}