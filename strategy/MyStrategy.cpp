#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() {
    state = GameState::getState();

    unitManager = shared_ptr<UnitManager>(new UnitManager());
    buildingManager = shared_ptr<BuildingManager>(new BuildingManager());
    attackManager = shared_ptr<AttackManager>(new AttackManager());
}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface) {
    const clock_t total_begin_time = clock();
    state->parsePlayerView(playerView);

    unordered_map<int, EntityAction> actions;
    // if (debugInterface) {
    //     state->influenceMap.draw(debugInterface);
    // }

    finishTasks();
    buildingManager->repairBuildings(actions);

    if (state->curBuilderCount >= 20 && state->rangedBaseCount < MAX_RANGED_BASE) {
        state->myResources -= state->entityProperties[EntityType::RANGED_BASE].initialCost;
        tasks.push_back(Task(buildingManager->getPlace(EntityType::RANGED_BASE), EntityType::RANGED_BASE));
    }

    if (state->curBuilderCount >= 5 && state->myResources >= state->entityProperties[EntityType::HOUSE].initialCost &&
            state->totalPopulation - state->usedPopulation < 6) {
        state->myResources -= state->entityProperties[EntityType::HOUSE].initialCost;
        tasks.push_back(Task(buildingManager->getPlace(EntityType::HOUSE), EntityType::HOUSE));
    }

    if (state->currentTick % 4 == 0 &&
            state->rangedBaseCount &&
            state->myResources >= state->entityProperties[EntityType::TURRET].initialCost &&
            state->turretCount < MAX_TURRET &&
            state->turretCount < state->currentTick / 150 * 5) {
        auto foundPlace = buildingManager->getPlace(EntityType::TURRET);
        if (foundPlace) {
            state->myResources -= state->entityProperties[EntityType::TURRET].initialCost;
            tasks.push_back(Task(foundPlace, EntityType::TURRET));
        }
    }

    if (state->myResources >= state->entityProperties[EntityType::BUILDER_BASE].initialCost &&
            state->builderBaseCount < MAX_BUILDER_BASE) {
        state->myResources -= state->entityProperties[EntityType::BUILDER_BASE].initialCost;
        tasks.push_back(Task(buildingManager->getPlace(EntityType::BUILDER_BASE), EntityType::BUILDER_BASE));
    }

    // if (state->currentTick > 200 && !isWallCreated && state->myResources >= state->entityProperties[EntityType::WALL].initialCost) {
    //     state->myResources -= state->entityProperties[EntityType::WALL].initialCost;
    //     tasks.push_back(Task(buildingManager->getPlace(EntityType::WALL), EntityType::WALL));
    // }

    executeTasks(actions);

    for (auto& entry : state->mySoldiers) {
        attackManager->goToAttack(entry, actions);
    }
    for (auto& entry : state->myBuilders) {
        if (!entry.busy) {
            attackManager->goToAttack(entry, actions);
        }
    }
    for (auto& entry : state->myBuildings) {
        if (isTurret(entry)) {
            attackManager->goToAttack(entry, actions);
        }
    }

    createUnits(actions);

    // if (state->curBuilderCount >= 8 && state->currentTick < 200 && state->turretCount < 3) {
    //     unitManager->stop(actions, EntityType::MELEE_UNIT);
    //     unitManager->stop(actions, EntityType::RANGED_UNIT);
    //     unitManager->stop(actions, EntityType::BUILDER_UNIT);
    // }

    // if (state->myResources > state->entityProperties[EntityType::TURRET].initialCost &&
    //         state->turretCount < MAX_TURRET) {
    //     buildingManager->createBuilding(actions, EntityType::TURRET);
    // }

    // totalGameTime += float(clock() - total_begin_time) / CLOCKS_PER_SEC;
    // cerr << "Total game time " << totalGameTime << endl;

    return Action(actions);
}

void MyStrategy::createUnits(unordered_map<int, EntityAction>& actions) {
    unitManager->createUnits(actions, EntityType::BUILDER_UNIT);

    if (state->rangedBaseCount) {
        unitManager->createUnits(actions, EntityType::RANGED_UNIT);
        // if (state->rangerCost > 3 * state->meleeCost) {
        //     unitManager->stop(actions, EntityType::RANGED_UNIT);
        //     unitManager->createUnits(actions, EntityType::MELEE_UNIT);
        // } else {
        //     unitManager->stop(actions, EntityType::MELEE_UNIT);
        // }
    } else {
        unitManager->createUnits(actions, EntityType::MELEE_UNIT);
    }
}

void MyStrategy::finishTasks() {
    for (auto& task : tasks) {
        if (task.finished || task.ttl <= 0) {
            continue;
        }

        if (task.pos && state->gameMap[task.pos.value().x][task.pos.value().y] == task.type) {
            task.finished = true;
            continue;
        }
        if (task.pos) {
            state->fillGameMap(task.pos.value(), task.type);
        }
        task.ttl--;

        state->totalPopulation += state->entityProperties[task.type].populationProvide;
        state->myResources -= state->entityProperties[task.type].initialCost;
        if (task.type == EntityType::TURRET) {
            state->turretCount++;
        }
        if (task.type == EntityType::BUILDER_BASE) {
            state->builderBaseCount++;
        }
        if (task.type == EntityType::RANGED_BASE) {
            state->rangedBaseCount++;
        }
    }
}

void MyStrategy::executeTasks(unordered_map<int, EntityAction>& actions) {
    for (auto& task : tasks) {
        if (task.ttl <= 0 || task.finished) {
            continue;
        }

        if (state->myResources >= 0) {
            buildingManager->createBuilding(actions, task.type, task.pos);
        }
    }
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface) {
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}