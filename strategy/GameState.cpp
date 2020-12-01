#include "GameState.hpp"

GameState::GameState() {}

void GameState::parsePlayerView(const PlayerView& playerView) {
    entityProperties = playerView.entityProperties;
    currentTick = playerView.currentTick;

    restoreGameMap(playerView);
    splitEntities(playerView);

    calcPopulationStats();

    for (auto& pl : playerView.players) {
        if (pl.id == playerView.myId) {
            resources = pl.resource;
        }
    }
}

void GameState::restoreGameMap(const PlayerView& playerView) {
    // if (currentTick & 1) {
    //     return;
    // }

    mapSize = playerView.mapSize;

    gameMap.resize(mapSize);
    gameMap.assign(mapSize, vector<char>(mapSize, -1));

    for (auto& entry: playerView.entities) {
        uint size = entityProperties[entry.entityType].size;
        for (int i = entry.position.x; i < entry.position.x + size; ++i) {
            for (int j = entry.position.y; j < entry.position.y + size; ++j) {
                gameMap[i][j] = entry.entityType;
            }
        }
    }
}

void GameState::splitEntities(const PlayerView& playerView) {
    myEntities.clear();
    myBases.clear();

    enemyBuildings.clear();
    others.clear();

    resourcesExist = false;
    for (auto& entry : playerView.entities) {
        if (entry.playerId && *entry.playerId == playerView.myId) {
            myEntities.push_back(entry);
            myEntities.back().busy = false;
            if (isBase(entry.entityType)) {
                myBases.push_back(entry);
            }
        }
        if (entry.playerId && *entry.playerId != playerView.myId) {
            if (isBuilding(entry.entityType)) {
                enemyBuildings.push_back(entry);
            } else {
                others.push_back(entry);
            }
        }
        resourcesExist = resourcesExist || (entry.entityType == EntityType::RESOURCE);
    }
}

void GameState::calcPopulationStats() {
    totalPopulation = 0;
    usedPopulation = 0;
    curBuilderCount = 0;
    rangedBaseCount = 0;
    builderBaseCount = 0;
    meleeBaseCount = 0;

    for (auto& entry : myEntities) {
        totalPopulation += entityProperties[entry.entityType].populationProvide;
        usedPopulation += entityProperties[entry.entityType].populationUse;
        curBuilderCount += isBuilder(entry);
        rangedBaseCount += (entry.entityType == EntityType::RANGED_BASE);
        builderBaseCount += (entry.entityType == EntityType::BUILDER_BASE);
        meleeBaseCount += (entry.entityType == EntityType::MELEE_BASE);
    }
}
