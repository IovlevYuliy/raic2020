#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() {
    unitManager = shared_ptr<UnitManager>(new UnitManager());
    buildingManager = shared_ptr<BuildingManager>(new BuildingManager());
}

void MyStrategy::calcPopulationStats() {
    totalPopulation = 0;
    usedPopulation = 0;
    for (auto& entry : myEntities) {
        totalPopulation += entityProperties[entry.entityType].populationProvide;
        usedPopulation += entityProperties[entry.entityType].populationUse;
    }
}

void MyStrategy::parsePlayerView(const PlayerView& playerView) {
    myEntities = getMyEntities(playerView);
    entityProperties = playerView.entityProperties;
    buildingManager->entityProperties = entityProperties;

    calcPopulationStats();
    unitManager->setPopulation(totalPopulation, usedPopulation);
    cerr << "population " << totalPopulation << ' ' << usedPopulation << endl;
}

void MyStrategy::restoreGameMap(const PlayerView& playerView) {
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
    parsePlayerView(playerView);

    unordered_map<int, EntityAction> actions;
    unitManager->createUnit(myEntities, actions, EntityType::BUILDER_UNIT);
    unitManager->createUnit(myEntities, actions, EntityType::RANGED_UNIT);

    for (auto& entry : myEntities) {
        shared_ptr<MoveAction> mvAction;
        shared_ptr<AttackAction> attackAction;

        auto properties = &playerView.entityProperties.find(entry.entityType)->second;
        if (isUnit(entry)) {
            mvAction = shared_ptr<MoveAction>(new MoveAction(
                Vec2Int(playerView.mapSize - 1, playerView.mapSize - 1),
                true,
                true
            ));

            attackAction = shared_ptr<AttackAction>(new AttackAction(
                NULL,
                shared_ptr<AutoAttack>(new AutoAttack(
                    properties->sightRange,
                    isBuilder(entry) ? vector<EntityType>{ EntityType::RESOURCE }: vector<EntityType>()
                ))
            ));


            actions[entry.id] = EntityAction(mvAction, attackAction);
        }

    }

    if (totalPopulation - usedPopulation == 0) {
        restoreGameMap(playerView);
        buildingManager->createBuilding(myEntities, gameMap, actions, EntityType::HOUSE);
    }

    return Action(actions);
}

vector<Entity> MyStrategy::getMyEntities(const PlayerView& playerView, EntityType type) {
    vector<Entity> myEntities;
    for (auto& entry : playerView.entities) {
        if (entry.playerId && *entry.playerId == playerView.myId) {
            myEntities.push_back(entry);
        }
    }

    return myEntities;
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface) {
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}