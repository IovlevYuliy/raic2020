#include "GameState.hpp"

GameState::GameState() {}

void GameState::parsePlayerView(const PlayerView& playerView) {
    entityProperties = playerView.entityProperties;
    currentTick = playerView.currentTick;

    restoreGameMap(playerView);
    splitEntities(playerView);

    calcPopulationStats();

    myId = playerView.myId;
    for (auto& pl : playerView.players) {
        if (pl.id == myId) {
            myResources = pl.resource;
        }
    }

    calcTargets();
    sort(mySoldiers.begin(), mySoldiers.end(), [](Entity& x, Entity& y){
        return x.targets < y.targets;
    });

    createInfluenceMap();
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

void GameState::createInfluenceMap() {
    infMap.resize(mapSize);
    infMap.assign(mapSize, vector<int>(mapSize, 0));
    for (auto& enemy: enemySoldiers) {
       fillInfluence(enemy);
    }
    for (auto& entry: mySoldiers) {
       fillInfluence(entry);
    }
    for (auto& entry: myBuildings) {
       fillInfluence(entry);
    }
    for (auto& entry: enemyBuildings) {
       fillInfluence(entry);
    }
}

void GameState::fillInfluence(Entity& entity) {
    if (isTurret(entity)) {
        int sz = entityProperties[entity.entityType].size;
        int range = entityProperties[entity.entityType].attack->attackRange;
        int sign = *entity.playerId == myId ? 1 : -1;
        Vec2Int pos;
        for (int i = entity.position.x - range; i <= entity.position.x + sz + range; ++i) {
            for (int j = entity.position.y - range; j <= entity.position.y + sz + range; ++j) {
                pos.x = i;
                pos.y = j;
                if (isOutOfMap(pos, mapSize)) {
                    continue;
                }
                int dist = min(abs(i - entity.position.x), abs(i - entity.position.x - sz + 1)) +
                           min(abs(j - entity.position.y), abs(j - entity.position.y - sz + 1));
                if (dist <= range) {
                    infMap[pos.x][pos.y] += sign * (range + 1 - dist);
                }
            }
        }
        return;
    }

    if (isBuilding(entity.entityType)) {
        int sz = entityProperties[entity.entityType].size;
        int range = 5;
        Vec2Int pos;
        for (int i = entity.position.x - range; i <= entity.position.x + sz + range; ++i) {
            for (int j = entity.position.y - range; j <= entity.position.y + sz + range; ++j) {
                pos.x = i;
                pos.y = j;
                if (isOutOfMap(pos, mapSize)) {
                    continue;
                }
                int dist = min(abs(i - entity.position.x), abs(i - entity.position.x - sz + 1)) +
                           min(abs(j - entity.position.y), abs(j - entity.position.y - sz + 1));
                if (dist <= range) {
                    infMap[pos.x][pos.y] ++;
                }
            }
        }
        return;
    }

    int range = entityProperties[entity.entityType].attack->attackRange + 1;
    int sign = *entity.playerId == myId ? 1 : -1;
    Vec2Int pos;
    for (int i = -range; i <= range; ++i) {
        for (int j = -(range - abs(i)); j <= (range - abs(i)); ++j) {
            pos.x = entity.position.x + i;
            pos.y = entity.position.y + j;
            if (!isOutOfMap(pos, mapSize)) {
                infMap[pos.x][pos.y] += sign * (range + 1 - abs(i) - abs(j));
            }
        }
    }
}

void GameState::drawInfMap(DebugInterface* debugInterface) {
    for (uint i = 0; i < mapSize; ++i) {
        for (uint j = 0; j < mapSize; ++j) {
            string s = to_string(infMap[i][j]);
            auto dc = DebugCommand::Add(shared_ptr<DebugData>(new DebugData::PlacedText(
                ColoredVertex(Vec2Float(i + 0.5, j + 0.5), Vec2Float(0, 0), Color(255, 0, 0, 1)), s, 0, 11)
            ));
            debugInterface->send(dc);
        }
    }
}
