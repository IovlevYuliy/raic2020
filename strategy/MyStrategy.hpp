#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "common.hpp"
#include "DebugInterface.hpp"
#include "BuildingManager.hpp"
#include "UnitManager.hpp"
#include "AttackManager.hpp"
#include "GameState.hpp"

class MyStrategy {
public:
    GameState state;

    shared_ptr<UnitManager> unitManager;
    shared_ptr<BuildingManager> buildingManager;
    shared_ptr<AttackManager> attackManager;

    float totalGameTime = 0;

    MyStrategy();

    Action getAction(const PlayerView& playerView, DebugInterface* debugInterface);

    void getSplittedEntities(const PlayerView& playerView, EntityType type = EntityType::ALL);

    void debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface);
};

#endif