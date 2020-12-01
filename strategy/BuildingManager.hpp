#ifndef _BUILDING_MANAGER_HPP_
#define _BUILDING_MANAGER_HPP_

#include "common.hpp"
#include "GameState.hpp"


class BuildingManager {
public:
    GameState* state;
    unordered_map<EntityType, int> repairBuilderCount = {
        { EntityType::BUILDER_BASE, 3 },
        { EntityType::MELEE_BASE, 3 },
        { EntityType::RANGED_BASE, 3 },
        { EntityType::HOUSE, 2 },
        { EntityType::TURRET, 2 },
        { EntityType::WALL, 1 }
    };

    BuildingManager();
    BuildingManager(GameState& state_);

    optional<int> createBuilding(unordered_map<int, EntityAction>& actions, EntityType unitType);

    void repairBuildings(unordered_map<int, EntityAction>& actions);

    bool isFree(Vec2Int& pos, uint size);
    bool isForbidden(Vec2Int& pos);
    bool checkNeighbors(Vec2Int& pos, uint entrySize);

    vector<Entity*> getNearestBuilders(Vec2Int pos, uint count);
    pair<Vec2Int, Entity> getNearestBuilder(Entity& destEntity);
    optional<Vec2Int> findPlace(Vec2Int start, uint size);
};

#endif
