#include "UnitManager.hpp"

UnitManager::UnitManager() {
    state = GameState::getState();
}

void UnitManager::createUnits(unordered_map<int, EntityAction>& actions, EntityType unitType, bool force) {
    for (auto& entry : state->myBases) {
        if (entry.entityType == EntityType::BUILDER_BASE && unitType == EntityType::BUILDER_UNIT) {
            createBuilder(entry, actions, force);
        }

        if (entry.entityType == EntityType::RANGED_BASE && unitType == EntityType::RANGED_UNIT) {
            createRanger(entry, actions);
        }

        if (entry.entityType == EntityType::MELEE_BASE && unitType == EntityType::MELEE_UNIT) {
            createMelee(entry, actions);
        }
    }
}

void UnitManager::stop(unordered_map<int, EntityAction>& actions, EntityType unitType) {
    for (auto& entry : state->myBases) {
        if (entry.entityType == EntityType::BUILDER_BASE && unitType == EntityType::BUILDER_UNIT) {
            actions[entry.id] = EntityAction();
            continue;
        }
        if (entry.entityType == EntityType::RANGED_BASE && unitType == EntityType::RANGED_UNIT) {
            actions[entry.id] = EntityAction();
            continue;
        }

        if (entry.entityType == EntityType::MELEE_BASE && unitType == EntityType::MELEE_UNIT) {
            actions[entry.id] = EntityAction();
            continue;
        }
    }
}

void UnitManager::createBuilder(Entity& builderBase, unordered_map<int, EntityAction>& actions, bool force) {
    if ((state->currentTick > 800 && state->curBuilderCount >= 20) ||
            (state->currentTick > 700 && state->curBuilderCount >= 30) ||
            (state->currentTick > 500 && state->curBuilderCount >= 40) ||
            (state->currentTick > 400 && state->curBuilderCount >= 50) ||
            state->currentTick > 900 ||
            (state->rangedBaseCount < 1 && state->curBuilderCount >= 25) ||
            state->curBuilderCount >= MAX_BUILDERS ||
            state->distToBase <= 5 ||
            state->myResources < state->builderCost) {
        actions[builderBase.id] = EntityAction();
        return;
    }

    uint baseSize = state->entityProperties[EntityType::BUILDER_BASE].size;
    state->myResources -= state->builderCost;

    actions[builderBase.id] = EntityAction(
        {},  // move
        BuildAction(
            EntityType::BUILDER_UNIT,
            getPosition(builderBase)
        ));
}

void UnitManager::createRanger(Entity& rangerBase, unordered_map<int, EntityAction>& actions) {
    if (state->myResources < state->rangerCost) {
        actions[rangerBase.id] = EntityAction();
        return;
    }

    state->myResources -= state->rangerCost;
    actions[rangerBase.id] = EntityAction(
        {},  // move
        BuildAction(
            EntityType::RANGED_UNIT,
            getPosition(rangerBase)
        )
    );
}

void UnitManager::createMelee(Entity& meleeBase, unordered_map<int, EntityAction>& actions) {
    if (state->myResources < state->meleeCost) {
        actions[meleeBase.id] = EntityAction();
        return;
    }

    state->myResources -= state->meleeCost;
    actions[meleeBase.id] = EntityAction(
        {},  // move
        BuildAction(
            EntityType::MELEE_UNIT,
            getPosition(meleeBase)
        )
    );
}

Vec2Int UnitManager::getPosition(Entity& base) {
    uint baseSize = state->entityProperties[base.entityType].size;
    Vec2Int pos(base.position.x - 1, base.position.y - 1);
    auto borders = getBorder(pos, baseSize + 2);
    random_shuffle(borders.begin(), borders.end());
    for (auto vec: borders) {
        if (isOutOfMap(vec, state->mapSize) || state->gameMap[vec.x][vec.y] != -1 ||
            vec == Vec2Int(base.position.x - 1, base.position.y - 1) ||
            vec == Vec2Int(base.position.x - 1, base.position.y + baseSize) ||
            vec == Vec2Int(base.position.x + baseSize, base.position.y - 1) ||
            vec == Vec2Int(base.position.x + baseSize, base.position.y + baseSize)) {
                continue;
        }
        return vec;
    }

    return Vec2Int(base.position.x + baseSize, base.position.y);
}
