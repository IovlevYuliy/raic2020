#include "AttackManager.hpp"

AttackManager::AttackManager() {}
AttackManager::AttackManager(GameState& state_) {
    state = &state_;
}

void AttackManager::goToAttack(Entity& myEntity, unordered_map<int, EntityAction>& actions) {
    if (isBuilder(myEntity)) {
        goToResources(myEntity, actions);
        return;
    }
    if (!troopIsReady(myEntity)) {
        return;
    }

    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;
    uint sightRange = state->entityProperties[myEntity.entityType].sightRange;
    auto defPosition = needDefense();
    if (defPosition) {
        if (defPosition.value().dist(myEntity.position) < 2 * DEFENSE_THRESHOLD) {
            actions[myEntity.id] = EntityAction(
                MoveAction(defPosition.value(), true, false),
                AttackAction(
                    {},
                    AutoAttack(sightRange, vector<EntityType>())
                )
            );
        }
        return;
    }

    auto target = getNearestTarget(myEntity, state->enemyBuildings, state->entityProperties);
    if (target.first <= attackRange && target.second.second->health > 0) {
        uint damage = state->entityProperties[myEntity.entityType].attack->damage;
        actions[myEntity.id] = EntityAction(
            {},
            AttackAction(target.second.second->id, {})
        );

        target.second.second->health -= damage;
        return;
    }

    if (target.first == 1e9 || target.second.second->health <= 0) {
        target = getNearestTarget(myEntity, state->others, state->entityProperties);
    }

    if (target.first != 1e9) {
        auto mvAction = MoveAction(target.second.first, true, false);
        auto attackAction = AttackAction(
            {},
            AutoAttack(attackRange, vector<EntityType>{EntityType::RANGED_UNIT, EntityType::MELEE_UNIT, EntityType::BUILDER_UNIT})
        );

        actions[myEntity.id] = EntityAction(mvAction, attackAction);
    } else {
        uint& mapSize = state->mapSize;
        Vec2Int corner(mapSize - 1, mapSize - 1);
        actions[myEntity.id] = EntityAction(
            MoveAction(Vec2Int(mapSize - 1, mapSize - 1), true, false)
        );
    }
}

bool AttackManager::troopIsReady(Entity& myEntity) {
    Vec2Int pos(myEntity.position.x, myEntity.position.y - 1);
    uint& mapSize = state->mapSize;
    if (isOutOfMap(pos, mapSize) ||
            state->gameMap[myEntity.position.x][myEntity.position.y - 1] != EntityType::RANGED_BASE) {
        return true;
    }

    uint baseSize = state->entityProperties[EntityType::RANGED_BASE].size;
    pos.y = myEntity.position.y;
    uint troopSize = 0;
    for(uint i = myEntity.position.x - baseSize; i < myEntity.position.x + baseSize; ++i) {
        pos.x = i;
        if (!isOutOfMap(pos, mapSize) && state->gameMap[pos.x][pos.y] == EntityType::RANGED_UNIT) {
            troopSize++;
        }
    }

    return troopSize >= 5;
}

optional<Vec2Int> AttackManager::needDefense() {
    for (auto& enemy : state->others) {
        for (auto& myBase : state->myBases) {
            auto res = getDistance(enemy, myBase, state->entityProperties);
            if (res.first < DEFENSE_THRESHOLD) {
                return enemy.position;
            }
        }
    }

    return {};
}

void AttackManager::goToResources(Entity& myEntity, unordered_map<int, EntityAction>& actions) {
    uint& mapSize = state->mapSize;
    queue<Vec2Int> q;
    unordered_set<Vec2Int> visited;
    q.push(myEntity.position);
    visited.insert(myEntity.position);

    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;
    uint it = 0;
    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();
        ++it;

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, mapSize) || visited.count(to)) {
                continue;
            }

            if (state->gameMap[to.x][to.y] == EntityType::RESOURCE) {
                actions[myEntity.id] = EntityAction(
                    MoveAction(to, true, false),
                    AttackAction(
                        {},
                        AutoAttack(attackRange, vector<EntityType>{})));

                return;
            } else if(state->gameMap[to.x][to.y] == -1) {
                visited.insert(to);
                q.push(to);
            }
        }
    }
}
