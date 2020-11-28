#include "UnitManager.hpp"

UnitManager::UnitManager() {}

void UnitManager::createUnit(vector<Entity>& myEntities,
        unordered_map<int, EntityAction>& actions, EntityType unitType) {
    switch (unitType) {
    case EntityType::BUILDER_UNIT:
        createBuilder(myEntities, actions);
        break;
    case EntityType::RANGED_UNIT:
        createRanger(myEntities, actions);
        break;
    default:
        break;
    }
}

void UnitManager::createBuilder(vector<Entity>& myEntities, unordered_map<int, EntityAction>& actions) {
    uint curBuilderCount = 0;
    for (auto& entry : myEntities) {
        curBuilderCount += isBuilder(entry);
    }

    Entity& builderBase = *find_if(myEntities.begin(), myEntities.end(), [](const Entity& entry) {
        return entry.entityType == EntityType::BUILDER_BASE;
    });

    if (curBuilderCount >= totalPopulation * MAX_BUILDERS_PERCENTAGE) {
        actions[builderBase.id] = EntityAction();
        return;
    }

    shared_ptr<BuildAction> buildAction(new BuildAction(
        EntityType::BUILDER_UNIT,
        Vec2Int(builderBase.position.x - 1, builderBase.position.y)
    ));

    actions[builderBase.id] = EntityAction(buildAction);
}

void UnitManager::createRanger(vector<Entity>& myEntities,
        unordered_map<int, EntityAction>& actions) {
    Entity& rangerBase = *find_if(myEntities.begin(), myEntities.end(), [](const Entity& entry) {
        return entry.entityType == EntityType::RANGED_BASE;
    });

    shared_ptr<BuildAction> buildAction(new BuildAction(
        EntityType::RANGED_UNIT,
        Vec2Int(rangerBase.position.x - 1, rangerBase.position.y)
    ));

    actions[rangerBase.id] = EntityAction(buildAction);
}