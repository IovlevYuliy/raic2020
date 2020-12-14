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
        { EntityType::RANGED_BASE, 4 },
        { EntityType::HOUSE, 1 },
        { EntityType::TURRET, 1 },
        { EntityType::WALL, 1 }
    };

    BuildingManager();

    optional<int> createBuilding(unordered_map<int, EntityAction>& actions, EntityType unitType, optional<Vec2Int> place);

    void repairBuildings(unordered_map<int, EntityAction>& actions);

    bool isFree(Vec2Int& pos, uint size);
    bool isForbidden(Vec2Int& pos);
    bool checkNeighbors(Vec2Int& pos, uint entrySize);

    vector<pair<Entity*, Vec2Int>> getNearestBuilders(Vec2Int pos, EntityType buildingType, uint count);
    pair<Vec2Int, Entity> getNearestBuilder(Entity& destEntity);
    optional<Vec2Int> findPlace(Vec2Int start, uint size, uint divider = 1);
    optional<Vec2Int> findTurretPlace(Vec2Int start, uint size);
    optional<Vec2Int> getPlace(EntityType type);
};

#endif
