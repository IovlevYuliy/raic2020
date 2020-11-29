#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "DebugInterface.hpp"
#include "model/Model.hpp"
#include "BuildingManager.hpp"
#include "UnitManager.hpp"
#include "AttackManager.hpp"
#include "common.hpp"

class MyStrategy {
public:
    shared_ptr<UnitManager> unitManager;
    shared_ptr<BuildingManager> buildingManager;
    shared_ptr<AttackManager> attackManager;

    vector<Entity>myEntities;
    vector<Entity>enemyEntities;

    unordered_map<int, int> busyBuilders;
    unordered_map<EntityType, EntityProperties> entityProperties;
    uint totalPopulation;
    uint usedPopulation;
    uint curBuilderCount;
    uint rangedBaseCount;
    uint resources;
    vector<vector<char> >gameMap;

    MyStrategy();

    Action getAction(const PlayerView& playerView, DebugInterface* debugInterface);

    void restoreGameMap(const PlayerView& playerView);

    void parsePlayerView(const PlayerView& playerView);
    void getSplittedEntities(const PlayerView& playerView, EntityType type = EntityType::ALL);

    void calcPopulationStats();

    void debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface);
};

#endif