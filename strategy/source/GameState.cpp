#include "GameState.hpp"

GameState::GameState() {}

GameState* GameState::state_ = nullptr;

GameState* GameState::getState() {
    if (state_ == nullptr) {
        state_ = new GameState();
    }

    return state_;
}

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

    createInfluenceMaps();
}

void GameState::restoreGameMap(const PlayerView& playerView) {
    // if (currentTick & 1) {
    //     return;
    // }

    mapSize = playerView.mapSize;

    gameMap.resize(mapSize);
    gameMap.assign(mapSize, vector<char>(mapSize, -1));

    for (auto& entry: playerView.entities) {
        fillGameMap(entry.position, entry.entityType);
    }
}

void GameState::fillGameMap(const Vec2Int& pos, EntityType type) {
    uint size = entityProperties[type].size;
    for (int i = pos.x; i < pos.x + size; ++i) {
        for (int j = pos.y; j < pos.y + size; ++j) {
            gameMap[i][j] = type;
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
                myBuilders.back().busy = false;
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
                enemySoldiers.back().distToBase = mapSize * 2;
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
    turretCount = 0;
    curBuilderCount = static_cast<uint>(myBuilders.size());

    for (auto& entry: myBuildings) {
        rangedBaseCount += (entry.entityType == EntityType::RANGED_BASE);
        builderBaseCount += (entry.entityType == EntityType::BUILDER_BASE);
        meleeBaseCount += (entry.entityType == EntityType::MELEE_BASE);
        turretCount += (entry.entityType == EntityType::TURRET);
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
    for (auto& entry: myBuildings) {
        for (auto& enemy: enemySoldiers) {
            auto res = getDistance(enemy, entry, entityProperties);
            enemy.distToBase = min(enemy.distToBase, static_cast<int>(res.first));
            distToBase = min(res.first, distToBase);
        }
    }
}

void GameState::createInfluenceMaps() {
    enemyAttackMap.clear(mapSize);
    enemyInfluence.clear(mapSize);
    myInfluence.clear(mapSize);

    for (auto& enemy: enemySoldiers) {
       enemyAttackMap.fillInfluence(
           enemy,
           entityProperties[enemy.entityType].size, // size
           entityProperties[enemy.entityType].attack->attackRange + 1, // attack range
           myId);
       enemyInfluence.fillInfluence(
           enemy,
           entityProperties[enemy.entityType].size,                     // size
           entityProperties[enemy.entityType].attack->attackRange + 1,  // attack range
           myId);
    }
    for (auto& entry : enemyBuildings) {
        if (isTurret(entry)) {
            enemyAttackMap.fillInfluence(
                entry,
                entityProperties[entry.entityType].size,                 // size
                entityProperties[entry.entityType].attack->attackRange + 1,  // attack range
                myId);
        }
        enemyInfluence.fillInfluence(
            entry,
            entityProperties[entry.entityType].size,  // size
            5,                                        // building range
            myId);
    }
    for (auto& entry : enemyBuilders) {
        enemyInfluence.fillInfluence(
            entry,
            entityProperties[entry.entityType].size,                     // size
            entityProperties[entry.entityType].attack->attackRange + 1,  // building range
            myId);
    }

    for (auto& entry: mySoldiers) {
        myInfluence.fillInfluence(
            entry,
            entityProperties[entry.entityType].size,                     // size
            entityProperties[entry.entityType].attack->attackRange + 1,  // attack range
            myId);
    }
    for (auto& entry: myBuildings) {
        if (isTurret(entry)) {
            myInfluence.fillInfluence(
                entry,
                entityProperties[entry.entityType].size,                     // size
                entityProperties[entry.entityType].attack->attackRange + 1,  // attack range
                myId);
        }
        myInfluence.fillInfluence(
            entry,
            entityProperties[entry.entityType].size,                     // size
            5,  // building range
            myId);
    }
    for (auto& entry : myBuilders) {
        myInfluence.fillInfluence(
            entry,
            entityProperties[entry.entityType].size,                     // size
            entityProperties[entry.entityType].attack->attackRange + 1,  // building range
            myId);
    }

    influenceMap = InfluenceMap(myInfluence);
    influenceMap.add(enemyInfluence);
}

int GameState::calcBuilders(Vec2Int& pos, int radius) {
    Vec2Int cur;
    int count = 0;
    for (int i = -radius; i <= radius; ++i) {
        for (int j = -radius; j <= radius; ++j) {
            cur.x = pos.x + i;
            cur.y = pos.y + j;
            if (!isOutOfMap(cur, mapSize)) {
                count += (gameMap[cur.x][cur.y] == EntityType::BUILDER_UNIT);
            }
        }
    }

    return count;
}

optional<Vec2Int> GameState::getStep(Entity& myEntity, Vec2Int& dest) {
    queue<Vec2Int> q;
    unordered_map<Vec2Int, Vec2Int> visited;
    q.push(myEntity.position);
    visited[myEntity.position] = myEntity.position;

    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, mapSize) || visited.count(to)) {
                continue;
            }

            if (to == dest) {
                visited[to] = v;
                while (visited[to] != myEntity.position) {
                    to = visited[to];
                }
                return to;
            } else if (gameMap[to.x][to.y] == -1) {
                visited[to] = v;
                q.push(to);
            }
        }
    }

    return {};
}

int GameState::getAlliesAround(Vec2Int& pos, int threshold) {
    int cnt = 0;
    for (int i = pos.x - threshold; i <= pos.x + threshold; ++i) {
        for (int j = pos.y - threshold; j <= pos.y + threshold; ++j) {
            Vec2Int cur(i, j);
            if (isOutOfMap(cur, mapSize)) {
                continue;
            }
            if (gameMap[i][j] == EntityType::RANGED_UNIT || gameMap[i][j] == EntityType::MELEE_UNIT) {
                cnt++;
            }
        }
    }
    return cnt;
}