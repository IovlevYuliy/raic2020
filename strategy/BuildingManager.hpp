#ifndef _BUILDING_MANAGER_HPP_
#define _BUILDING_MANAGER_HPP_

#include "common.hpp"
#include "model/Model.hpp"

class BuildingManager {
public:
    unordered_map<EntityType, EntityProperties> entityProperties;

    BuildingManager();

    void createBuilding(vector<Entity>& myEntities, vector<vector<char> >& gameMap,
        unordered_map<int, EntityAction>& actions, EntityType unitType);

    optional<Vec2Int> findPlace(vector<vector<char> >& gameMap, Vec2Int start, uint size);
};

#endif
