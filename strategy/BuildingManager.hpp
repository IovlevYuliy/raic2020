#ifndef _BUILDING_MANAGER_HPP_
#define _BUILDING_MANAGER_HPP_

#include "common.hpp"
#include "GameState.hpp"

class BuildingManager {
public:
    GameState* state;

    BuildingManager();
    BuildingManager(GameState& state_);

    optional<int> createBuilding(unordered_map<int, EntityAction>& actions, EntityType unitType);

    void repairBuildings(unordered_map<int, EntityAction>& actions);

    bool isForbidden(Vec2Int& pos);
    bool checkNeighbors(Vec2Int& pos, uint entrySize);

    pair<Vec2Int, Entity> getNearestBuilder(Entity& destEntity);
    optional<Vec2Int> findPlace(Vec2Int start, uint size);
};

#endif
