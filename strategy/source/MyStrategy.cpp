#include <exception>
#include "MyStrategy.hpp"

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
        // state->myInfluence.drawColor(debugInterface);
    // }

    finishTasks();
    buildingManager->repairBuildings(actions);

    if (state->curBuilderCount >= 20 && state->rangedBaseCount < state->MAX_RANGED_BASE) {
        state->myResources -= state->entityProperties[EntityType::RANGED_BASE].initialCost;
        tasks.push_back(Task(buildingManager->getPlace(EntityType::RANGED_BASE), EntityType::RANGED_BASE));
    }

    if (state->curBuilderCount >= 5 && state->totalPopulation - state->usedPopulation < 9) {
        state->myResources -= state->entityProperties[EntityType::HOUSE].initialCost;
        tasks.push_back(Task(buildingManager->getPlace(EntityType::HOUSE), EntityType::HOUSE));
    }

    if (state->rangedBaseCount &&
            state->myResources >= state->entityProperties[EntityType::TURRET].initialCost &&
            state->turretCount < state->MAX_TURRET &&
            ((state->turretCount < state->currentTick / 140 * 4 && !state->isFinal) ||
            (state->turretCount < state->currentTick / 100 * 3 && state->isFinal))
    ) {
        auto foundPlace = buildingManager->getPlace(EntityType::TURRET);
        if (foundPlace) {
            state->myResources -= state->entityProperties[EntityType::TURRET].initialCost;
            tasks.push_back(Task(foundPlace, EntityType::TURRET));
        }
    }

    if (state->myResources >= state->entityProperties[EntityType::BUILDER_BASE].initialCost &&
            state->builderBaseCount < state->MAX_BUILDER_BASE) {
        state->myResources -= state->entityProperties[EntityType::BUILDER_BASE].initialCost;
        tasks.push_back(Task(buildingManager->getPlace(EntityType::BUILDER_BASE), EntityType::BUILDER_BASE));
    }

    // if (state->currentTick > 200 && !isWallCreated && state->myResources >= state->entityProperties[EntityType::WALL].initialCost) {
    //     state->myResources -= state->entityProperties[EntityType::WALL].initialCost;
    //     tasks.push_back(Task(buildingManager->getPlace(EntityType::WALL), EntityType::WALL));
    // }

    executeTasks(actions);

    // simulation of enemy attacks
    for (auto& entry : state->enemySoldiers) {
        if (entry.targets == 1) {
            attackManager->simEnemyAttack(entry);
        }
    }
    for (auto& entry : state->enemyBuildings) {
        if (isTurret(entry)) {
            attackManager->simEnemyAttack(entry);
        }
    }

    // my attacks
    for (auto& entry : state->mySoldiers) {
        attackManager->goToAttack(entry, actions);
    }
    for (auto& entry : state->myBuilders) {
        if (!entry.busy) {
            attackManager->goToAttack(entry, actions);
            attackManager->tryToHealRangers(entry, actions);
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
        if (!task.pos) {
            task.pos = buildingManager->getPlace(task.type);
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

        if (state->existResources >= state->entityProperties[task.type].initialCost) {
            if (!task.pos) {
                task.pos = buildingManager->getPlace(task.type);
            }
            if (task.pos) {
                state->fillGameMap(task.pos.value(), task.type);
                buildingManager->createBuilding(actions, task.type, task.pos);
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface) {
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}