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

    unordered_map<int, EntityAction> actions;
    unitManager->createUnits(actions, EntityType::BUILDER_UNIT);
    unitManager->createUnits(actions, EntityType::RANGED_UNIT);

    for (auto& entry : state.myEntities) {
        if (isUnit(entry) || isTurret(entry)) {
            attackManager->goToAttack(entry, actions);
            continue;
        }
    }

    if (state.totalPopulation - state.usedPopulation < 6) {
        buildingManager->createBuilding(actions, EntityType::HOUSE);
    }

    if (state.resources > state.entityProperties[EntityType::RANGED_BASE].cost &&
            state.rangedBaseCount < MAX_RANGED_BASE && state.totalPopulation > 50 * state.rangedBaseCount) {
        buildingManager->createBuilding(actions, EntityType::RANGED_BASE);
    }

    if (state.resources > state.entityProperties[EntityType::BUILDER_BASE].cost &&
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