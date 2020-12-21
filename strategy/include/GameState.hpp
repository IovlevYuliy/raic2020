#ifndef _GAME_STATE_HPP_
#define _GAME_STATE_HPP_

#include "InfluenceMap.hpp"
#include "common.hpp"

class GameState {
private:
    static GameState* state_;
    GameState();
public:
    unordered_map<EntityType, EntityProperties> entityProperties;

    vector<Entity> myBuilders;
    vector<Entity> mySoldiers;
    vector<Entity> myBases;
    vector<Entity> myBuildings;

    vector<Entity> enemySoldiers;
    vector<Entity> enemyBuilders;
    vector<Entity> enemyBuildings;

    vector<vector<char>> gameMap;
    InfluenceMap enemyAttackMap;
    InfluenceMap enemyInfluence;
    InfluenceMap myInfluence;
    InfluenceMap influenceMap;

    uint mapSize;

    uint totalPopulation;
    uint usedPopulation;
    uint curBuilderCount;
    uint curRangerCount;
    uint curMeleeCount;

    uint rangedBaseCount;
    uint builderBaseCount;
    uint meleeBaseCount;
    uint turretCount;

    uint rangerCost;
    uint meleeCost;
    uint builderCost;

    uint remainingResources;
    int myResources;
    uint currentTick;

    uint distToBase;

    int myId;

    bool isFinal;
    int MAX_TURRET = 25;
    int MAX_RANGED_BASE = 1;
    int MAX_BUILDER_BASE = 1;
    int MAX_BUILDERS = 60;
    int MAX_RANGERS = 40;

    static GameState* getState();

    void parsePlayerView(const PlayerView& playerView);

    void restoreGameMap(const PlayerView& playerView);

    void createInfluenceMaps();

    void fillGameMap(const Vec2Int& pos, EntityType type);

    void splitEntities(const PlayerView& playerView);

    void calcTargets();

    void calcPopulationStats();

    int calcBuilders(Vec2Int& pos, int radius);

    optional<Vec2Int> getStep(Entity& myEntity, Vec2Int& dest);

    int getAlliesAround(Vec2Int& pos, int threshold);
};

#endif
