#include "MyStrategy.hpp"
#include <exception>

MyStrategy::MyStrategy() {
    unitManager = shared_ptr<UnitManager>(new UnitManager(state));
    buildingManager = shared_ptr<BuildingManager>(new BuildingManager(state));
    attackManager = shared_ptr<AttackManager>(new AttackManager(state));
}

Action MyStrategy::getAction(const PlayerView& playerView, DebugInterface* debugInterface) {
    const clock_t total_begin_time = clock();
    state.parsePlayerView(playerView);

    if (debugInterface) {
        state.drawInfMap(debugInterface);
    }

    unordered_map<int, EntityAction> actions;
    unitManager->createUnits(actions, EntityType::BUILDER_UNIT);
    if (state.rangedBaseCount && state.meleeBaseCount) {
        if (state.rangerCost > 2 * state.meleeCost) {
            unitManager->stop(actions, EntityType::RANGED_UNIT);
            unitManager->createUnits(actions, EntityType::MELEE_UNIT);
        } else {
            unitManager->stop(actions, EntityType::MELEE_UNIT);
            unitManager->createUnits(actions, EntityType::RANGED_UNIT);
        }
    } else {
        unitManager->createUnits(actions, EntityType::RANGED_UNIT);
        unitManager->createUnits(actions, EntityType::MELEE_UNIT);
    }

    for (auto& entry : state.mySoldiers) {
        attackManager->goToAttack(entry, actions);
    }
    for (auto& entry : state.myBuilders) {
        attackManager->goToAttack(entry, actions);
    }
    for (auto& entry : state.myBuildings) {
        if (isTurret(entry)) {
            attackManager->goToAttack(entry, actions);
        }
    }

    if (state.totalPopulation - state.usedPopulation < 6) {
        buildingManager->createBuilding(actions, EntityType::HOUSE);
    }

    if (state.myResources > state.entityProperties[EntityType::RANGED_BASE].initialCost &&
            state.rangedBaseCount < MAX_RANGED_BASE) {
        buildingManager->createBuilding(actions, EntityType::RANGED_BASE);
    }

    if (state.myResources > state.entityProperties[EntityType::BUILDER_BASE].initialCost &&
            state.builderBaseCount < MAX_BUILDER_BASE) {
        buildingManager->createBuilding(actions, EntityType::BUILDER_BASE);
    }

    buildingManager->repairBuildings(actions);

    // totalGameTime += float(clock() - total_begin_time) / CLOCKS_PER_SEC;
    // cerr << "Total game time " << totalGameTime << endl;

    return Action(actions);
}

void MyStrategy::debugUpdate(const PlayerView& playerView, DebugInterface& debugInterface) {
    debugInterface.send(DebugCommand::Clear());
    debugInterface.getState();
}