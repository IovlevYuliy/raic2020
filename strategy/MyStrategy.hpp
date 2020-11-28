#ifndef _MY_STRATEGY_HPP_
#define _MY_STRATEGY_HPP_

#include "DebugInterface.hpp"
#include "model/Model.hpp"
#include "common.hpp"
#include "UnitManager.hpp"
#include "BuildingManager.hpp"

class MyStrategy {
public:
    shared_ptr<UnitManager> unitManager;
    shared_ptr<BuildingManager> buildingManager;

    vector<Entity>myEntities;
    unordered_map<EntityType, EntityProperties> entityProperties;
    uint totalPopulation;
    uint usedPopulation;
    vector<vector<char> >gameMap;

    MyStrategy();

    Action getAction(const PlayerView& playerView, DebugInterface* debugInterface);

    void restoreGameMap(const PlayerView& playerView);

    void parsePlayerView(const PlayerView& playerView);
    vector<Entity> getMyEntities(const PlayerView& playerView, EntityType type = EntityType::ALL);

    void calcPopulationStats();

    void debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface);
};

#endif