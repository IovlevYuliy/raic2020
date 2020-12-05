#ifndef _GAME_STATE_HPP_
#define _GAME_STATE_HPP_

#include "common.hpp"

class GameState {
public:
    unordered_map<EntityType, EntityProperties> entityProperties;

    vector<Entity> myBuilders;
    vector<Entity> mySoldiers;
    vector<Entity> myBases;
    vector<Entity> myBuildings;

    vector<Entity> enemySoldiers;
    vector<Entity> enemyBuilders;
    vector<Entity> enemyBuildings;

    vector<vector<char> >gameMap;
    uint mapSize;

    uint totalPopulation;
    uint usedPopulation;
    uint curBuilderCount;
    uint curRangerCount;
    uint curMeleeCount;

    uint rangedBaseCount;
    uint builderBaseCount;
    uint meleeBaseCount;

    uint rangerCost;
    uint meleeCost;
    uint builderCost;

    uint remainingResources;
    uint myResources;
    uint currentTick;

    uint distToBase;

    GameState();

    void parsePlayerView(const PlayerView& playerView);

    void restoreGameMap(const PlayerView& playerView);

    void splitEntities(const PlayerView& playerView);

    void calcTargets();

    void calcPopulationStats();
};

#endif
