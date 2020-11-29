#ifndef _BUILDING_MANAGER_HPP_
#define _BUILDING_MANAGER_HPP_

#include "common.hpp"
#include "model/Model.hpp"

class BuildingManager {
public:
    unordered_map<EntityType, EntityProperties> entityProperties;

    BuildingManager();

    optional<int> createBuilding(vector<Entity>& myEntities, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions, EntityType unitType);

    vector<int> repairBuildings(vector<Entity>& myEntities,
        unordered_map<int, EntityAction>& actions);

    bool isForbidden(Vec2Int& pos, vector<vector<char>>& gameMap);
    bool checkNeighbors(Vec2Int& pos, uint entrySize, vector<vector<char>>& gameMap);

    pair<Vec2Int, Entity> getNearestBuilder(vector<Entity>& myEntities, Entity& destEntity);
    optional<Vec2Int> findPlace(vector<vector<char>>& gameMap, Vec2Int start, uint size);
};

#endif
