#include "UnitManager.hpp"

UnitManager::UnitManager() {}

UnitManager::UnitManager(GameState& state_) {
    state = &state_;
}

void UnitManager::createUnits(unordered_map<int, EntityAction>& actions, EntityType unitType, bool force) {
    for (auto& entry : state->myBases) {
        if (entry.entityType == EntityType::BUILDER_BASE && unitType == EntityType::BUILDER_UNIT) {
            createBuilder(entry, actions, force);
        }

        if (entry.entityType == EntityType::RANGED_BASE && unitType == EntityType::RANGED_UNIT) {
            createRanger(entry, actions);
        }
    }
}

void UnitManager::createBuilder(Entity& builderBase, unordered_map<int, EntityAction>& actions, bool force) {
    if ((state->currentTick > 400 &&
            state->curBuilderCount >= state->totalPopulation * MAX_BUILDERS_PERCENTAGE) ||
            state->curBuilderCount >= MAX_BUILDERS || !state->resourcesExist) {
        actions[builderBase.id] = EntityAction();
        return;
    }

    uint baseSize = state->entityProperties[EntityType::BUILDER_BASE].size;

    actions[builderBase.id] = EntityAction(
        {},  // move
        BuildAction(
            EntityType::BUILDER_UNIT,
            Vec2Int(builderBase.position.x + baseSize, builderBase.position.y)));
}

void UnitManager::createRanger(Entity& rangerBase, unordered_map<int, EntityAction>& actions) {
    uint baseSize = state->entityProperties[EntityType::RANGED_BASE].size;

    actions[rangerBase.id] = EntityAction(
        {},  // move
        BuildAction(
            EntityType::RANGED_UNIT,
            Vec2Int(rangerBase.position.x + baseSize, rangerBase.position.y)
        )
    );
}