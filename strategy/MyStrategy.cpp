#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() {
    unitManager = shared_ptr<UnitManager>(new UnitManager());
    buildingManager = shared_ptr<BuildingManager>(new BuildingManager());
    attackManager = shared_ptr<AttackManager>(new AttackManager());
}

void MyStrategy::calcPopulationStats() {
    totalPopulation = 0;
    usedPopulation = 0;
    curBuilderCount = 0;
    rangedBaseCount = 0;
    builderBaseCount = 0;

    for (auto& entry : myEntities) {
        totalPopulation += entityProperties[entry.entityType].populationProvide;
        usedPopulation += entityProperties[entry.entityType].populationUse;
        curBuilderCount += isBuilder(entry);
        rangedBaseCount += (entry.entityType == EntityType::RANGED_BASE);
        builderBaseCount += (entry.entityType == EntityType::BUILDER_BASE);
    }
}

void MyStrategy::parsePlayerView(const PlayerView& playerView) {
    getSplittedEntities(playerView);

    entityProperties = playerView.entityProperties;
    buildingManager->entityProperties = entityProperties;
    attackManager->entityProperties = entityProperties;
    unitManager->entityProperties = entityProperties;

    attackManager->getAims(enemyEntities, myEntities);

    currentTick = playerView.currentTick;
    restoreGameMap(playerView);

    calcPopulationStats();
    unitManager->setPopulation(totalPopulation, usedPopulation, curBuilderCount);
    unitManager->currentTick = currentTick;

    for (auto& pl : playerView.players) {
        if (pl.id == playerView.myId) {
            resources = pl.resource;
        }
    }
}

void MyStrategy::restoreGameMap(const PlayerView& playerView) {
    // if (playerView.currentTick & 1) {
    //     return;
    // }

    gameMap.resize(playerView.mapSize);
    gameMap.assign(playerView.mapSize, vector<char>(playerView.mapSize, -1));

    for (auto& entry: playerView.entities) {
        uint size = entityProperties[entry.entityType].size;
        for (int i = entry.position.x; i < entry.position.x + size; ++i) {
            for (int j = entry.position.y; j < entry.position.y + size; ++j) {
                gameMap[i][j] = entry.entityType;
            }
        }
    }
}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface) {
    // const clock_t total_begin_time = clock();

    parsePlayerView(playerView);

    unordered_map<int, EntityAction> actions;
    unitManager->createUnits(myEntities, gameMap, actions, EntityType::BUILDER_UNIT);
    unitManager->createUnits(myEntities, gameMap, actions, EntityType::RANGED_UNIT);

    for (auto& entry : myEntities) {
        if (isUnit(entry)) {
            attackManager->goToAttack(entry, gameMap, actions);
        }
    }

    if (totalPopulation - usedPopulation < 2) {
        buildingManager->createBuilding(myEntities, gameMap, actions, EntityType::HOUSE);
    }

    if (resources > entityProperties[EntityType::RANGED_BASE].cost &&
            rangedBaseCount < MAX_RANGED_BASE) {
        buildingManager->createBuilding(myEntities, gameMap, actions, EntityType::RANGED_BASE);
    }

    if (resources > entityProperties[EntityType::BUILDER_BASE].cost &&
            builderBaseCount < MAX_BUILDER_BASE) {
        buildingManager->createBuilding(myEntities, gameMap, actions, EntityType::BUILDER_BASE);
    }

    buildingManager->repairBuildings(myEntities, actions);

    // totalGameTime += float(clock() - total_begin_time) / CLOCKS_PER_SEC;
    // cerr << "Total game time " << totalGameTime << endl;

    return Action(actions);
}

void MyStrategy::getSplittedEntities(const PlayerView& playerView, EntityType type) {
    myEntities.clear();
    enemyEntities.clear();
    for (auto& entry : playerView.entities) {
        if (entry.playerId && *entry.playerId == playerView.myId) {
            myEntities.push_back(entry);
            myEntities.back().busy = false;
        }
        if (entry.playerId && *entry.playerId != playerView.myId) {
            enemyEntities.push_back(entry);
        }
    }
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface) {
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}