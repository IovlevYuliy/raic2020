#include "UnitManager.hpp"

UnitManager::UnitManager() {}

void UnitManager::createUnits(vector<Entity>& myEntities,
        unordered_map<int, EntityAction>& actions, EntityType unitType, bool force) {
    for (auto& entry : myEntities) {
        if (entry.entityType == EntityType::BUILDER_BASE && unitType == EntityType::BUILDER_UNIT) {
            createBuilder(entry, actions, force);
        }

        if (entry.entityType == EntityType::RANGED_BASE && unitType == EntityType::RANGED_UNIT) {
            createRanger(entry, actions);
        }
    }
}

void UnitManager::createBuilder(Entity& builderBase,
        unordered_map<int, EntityAction>& actions, bool force) {
    if ((!force && curBuilderCount > 10 && curBuilderCount >= totalPopulation * MAX_BUILDERS_PERCENTAGE) ||
            curBuilderCount >= MAX_BUILDERS) {
        actions[builderBase.id] = EntityAction();
        return;
    }

    curBuilderCount++;
    actions[builderBase.id] = EntityAction(
        {}, // move
        BuildAction(
            EntityType::BUILDER_UNIT,
            Vec2Int(builderBase.position.x - 1, builderBase.position.y)
        )
    );
}

void UnitManager::createRanger(Entity& rangerBase, unordered_map<int, EntityAction>& actions) {
    actions[rangerBase.id] = EntityAction(
        {}, // move
        BuildAction(
            EntityType::RANGED_UNIT,
            Vec2Int(rangerBase.position.x - 1, rangerBase.position.y)
        )
    );
}