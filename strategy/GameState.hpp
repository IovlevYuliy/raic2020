#ifndef _GAME_STATE_HPP_
#define _GAME_STATE_HPP_

#include "common.hpp"

class GameState {
public:
    unordered_map<EntityType, EntityProperties> entityProperties;

    vector<Entity> myEntities;
    vector<Entity> myBases;

    vector<Entity> others;
    vector<Entity> enemyBuildings;

    vector<vector<char> >gameMap;
    uint mapSize;

    uint totalPopulation;
    uint usedPopulation;
    uint curBuilderCount;

    uint rangedBaseCount;
    uint builderBaseCount;
    uint meleeBaseCount;

    uint remainingResources;
    uint myResources;
    uint currentTick;

    GameState();

    void parsePlayerView(const PlayerView& playerView);

    void restoreGameMap(const PlayerView& playerView);

    void splitEntities(const PlayerView& playerView);

    void calcPopulationStats();
};

#endif
