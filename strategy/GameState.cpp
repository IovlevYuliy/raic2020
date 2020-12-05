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
            myResources = pl.resource;
        }
    }

    calcTargets();
    sort(mySoldiers.begin(), mySoldiers.end(), [](Entity& x, Entity& y){
        return x.targets < y.targets;
    });
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
    myBuilders.clear();
    mySoldiers.clear();
    myBuildings.clear();
    myBases.clear();

    enemyBuildings.clear();
    enemySoldiers.clear();
    enemyBuilders.clear();

    remainingResources = 0;
    for (auto& entry : playerView.entities) {
        if (entry.playerId && *entry.playerId == playerView.myId) {
            if (isBuilding(entry.entityType)) {
                myBuildings.push_back(entry);

                if (isBase(entry.entityType)) {
                    myBases.push_back(entry);
                }
                continue;
            }

            if (isBuilder(entry)) {
                myBuilders.push_back(entry);
            } else {
                mySoldiers.push_back(entry);
            }
        }
        if (entry.playerId && *entry.playerId != playerView.myId) {
            if (isBuilding(entry.entityType)) {
                enemyBuildings.push_back(entry);
                continue;
            }
            if (isBuilder(entry)){
                enemyBuilders.push_back(entry);
            } else {
                enemySoldiers.push_back(entry);
            }
        }
        remainingResources += (entry.entityType == EntityType::RESOURCE);
    }
}

void GameState::calcPopulationStats() {
    totalPopulation = 0;
    usedPopulation = 0;
    rangedBaseCount = 0;
    builderBaseCount = 0;
    meleeBaseCount = 0;
    curRangerCount = 0;
    curMeleeCount = 0;
    curBuilderCount = static_cast<uint>(myBuilders.size());

    for (auto& entry: myBuildings) {
        rangedBaseCount += (entry.entityType == EntityType::RANGED_BASE);
        builderBaseCount += (entry.entityType == EntityType::BUILDER_BASE);
        meleeBaseCount += (entry.entityType == EntityType::MELEE_BASE);
        totalPopulation += entityProperties[entry.entityType].populationProvide;
    }

    for (auto& entry: mySoldiers) {
        curRangerCount += (entry.entityType == EntityType::RANGED_UNIT);
        curMeleeCount += (entry.entityType == EntityType::MELEE_UNIT);
    }

    usedPopulation += entityProperties[EntityType::BUILDER_UNIT].populationUse * curBuilderCount;
    usedPopulation += entityProperties[EntityType::RANGED_UNIT].populationUse * curRangerCount;
    usedPopulation += entityProperties[EntityType::MELEE_UNIT].populationUse * curMeleeCount;

    builderCost = entityProperties[EntityType::BUILDER_UNIT].initialCost + curBuilderCount;
    rangerCost = entityProperties[EntityType::RANGED_UNIT].initialCost + curRangerCount;
    meleeCost = entityProperties[EntityType::MELEE_UNIT].initialCost + curMeleeCount;
}

void GameState::calcTargets() {
    for (auto& entry: mySoldiers) {
        entry.targets = 0;
        uint attackRange = entityProperties[entry.entityType].attack->attackRange;
        for (auto& enemy: enemySoldiers) {
            if(entry.position.dist(enemy.position) <= attackRange) {
                entry.targets++;
            }
        }
        for (auto& enemy: enemyBuilders) {
            if(entry.position.dist(enemy.position) <= attackRange) {
                entry.targets++;
            }
        }
    }
    distToBase = 1e9;
    for (auto& entry: myBases) {
        for (auto& enemy: enemySoldiers) {
            auto res = getDistance(enemy, entry, entityProperties);
            distToBase = min(res.first, distToBase);
        }
    }
}
