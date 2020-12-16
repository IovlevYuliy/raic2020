#ifndef _UNIT_BUILDER_MANAGER_HPP_
#define _UNIT_BUILDER_MANAGER_HPP_

#include "common.hpp"
#include "GameState.hpp"

class UnitManager {
public:
    GameState* state;

    UnitManager();

    void createUnits(unordered_map<int, EntityAction>& actions, EntityType unitType,
        bool force = false);

    void createBuilder(Entity& builderBase, unordered_map<int, EntityAction>& actions,
        bool force = false);

    void createRanger(Entity& rangerBase, unordered_map<int, EntityAction>& actions);

    void createMelee(Entity& meleeBase, unordered_map<int, EntityAction>& actions);

    void stop(unordered_map<int, EntityAction>& actions, EntityType unitType);

    Vec2Int getPosition(Entity& base);
};

#endif
