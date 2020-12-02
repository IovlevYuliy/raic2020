#include "AttackManager.hpp"

AttackManager::AttackManager() {}
AttackManager::AttackManager(GameState& state_) {
    state = &state_;
}

void AttackManager::goToAttack(Entity& myEntity, unordered_map<int, EntityAction>& actions) {
    if (isBuilder(myEntity) && state->remainingResources) {
        goToResources(myEntity, actions);
        return;
    }

    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;
    uint damage = state->entityProperties[myEntity.entityType].attack->damage;
    uint sightRange = state->entityProperties[myEntity.entityType].sightRange;

    auto targets = getTarget(myEntity);
    if (targets.first) {
        targets.first->health -= damage;
        actions[myEntity.id] = EntityAction(
            {},
            AttackAction(targets.first->id, {})
        );
        return;
    }

    if (isTurret(myEntity)) {
        actions[myEntity.id] = EntityAction(
            {},
            AttackAction({}, AutoAttack(sightRange, vector<EntityType>()))
        );

        return;
    }

    if (rand() % 5 == 0) {
        actions[myEntity.id] = EntityAction();
        return;
    }

    if (targets.second) {
        actions[myEntity.id] = EntityAction(
            MoveAction(targets.second->position, true, false)
        );
    } else {
        uint mapSize = state->mapSize;
        Vec2Int corner(mapSize - 1, mapSize - 1);
        actions[myEntity.id] = EntityAction(
            MoveAction(corner, true, false),
            AttackAction(
                {},
                AutoAttack(attackRange, vector<EntityType>())
            )
        );
    }
}

pair<Entity*, Entity*> AttackManager::getTarget(Entity& myEntity) {
    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;
    Entity* targetInRange = NULL;
    Entity* nearestTarget = NULL;
    uint minDist = 1e9;
    uint minHp = 1e9;

    for (auto& enemy: state->others) {
        if (enemy.health <= 0) continue;

        uint dist = myEntity.position.dist(enemy.position);

        if (enemy.health < minHp && dist <= attackRange) {
            minHp = enemy.health;
            targetInRange = &enemy;
        }

        if (dist < minDist) {
            minDist = dist;
            nearestTarget = &enemy;
        }
    }

    if (targetInRange) {
        return make_pair(targetInRange, nearestTarget);
    }

    for (auto& enemy : state->enemyBuildings) {
        if (enemy.health <= 0) continue;

        auto dist = getDistance(myEntity, enemy, state->entityProperties);

        if (enemy.health < minHp && dist.first <= attackRange) {
            minHp = enemy.health;
            targetInRange = &enemy;
        }

        if (dist.first < minDist) {
            minDist = dist.first;
            nearestTarget = &enemy;
        }
    }

    return make_pair(targetInRange, nearestTarget);
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
                    MoveAction(to, true, true),
                    AttackAction(
                        {},
                        AutoAttack(attackRange, vector<EntityType>{EntityType::RESOURCE, EntityType::BUILDER_UNIT})));
                return;
            } else if(state->gameMap[to.x][to.y] == -1) {
                visited.insert(to);
                q.push(to);
            }
        }
    }
}
