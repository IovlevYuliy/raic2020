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

    auto targets = getTargets(myEntity);
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

    if (state->currentTick < 200 && (targets.second->position.x > 40 || targets.second->position.y > 40)) {
        actions[myEntity.id] = EntityAction(
            MoveAction(Vec2Int(15, 15), true, false)
        );
        return;
    }

    // if (rand() % 5 == 0) {
    //     actions[myEntity.id] = EntityAction();
    //     return;
    // }

    if (targets.second) {
        bool breakThrough = myEntity.entityType == EntityType::MELEE_UNIT;
        actions[myEntity.id] = EntityAction(
            MoveAction(targets.second->position, true, breakThrough)
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

pair<Entity*, Entity*> AttackManager::getTargets(Entity& myEntity) {
    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;
    Entity* targetInRange = NULL;
    Entity* nearestTarget = NULL;
    uint minDist = 1e9;
    uint minHp = 1e9;

    for (auto& enemy: state->enemySoldiers) {
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

    for (auto& enemy: state->enemyBuilders) {
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

void AttackManager::goToResources(Entity& myEntity, unordered_map<int, EntityAction>& actions) {
    uint& mapSize = state->mapSize;
    queue<Vec2Int> q;
    unordered_map<Vec2Int, Vec2Int> visited;
    q.push(myEntity.position);
    visited[myEntity.position] = myEntity.position;

    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;
    uint it = 0;
    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();
        ++it;

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, mapSize) || visited.count(to) || state->infMap[to.x][to.y] < 0) {
                continue;
            }

            if (state->gameMap[to.x][to.y] == EntityType::RESOURCE && state->infMap[v.x][v.y] >= 0) {
                visited[to] = v;
                while (visited[to] != myEntity.position) {
                    to = visited[to];
                }
                if (state->gameMap[to.x][to.y] == EntityType::RESOURCE) {
                    state->myResources++;
                    actions[myEntity.id] = EntityAction(
                        {},
                        AttackAction(
                            {},
                            AutoAttack(attackRange, vector<EntityType>{EntityType::RESOURCE, EntityType::BUILDER_UNIT}))
                    );
                    return;
                }
                actions[myEntity.id] = EntityAction(MoveAction(to, true, false));
                return;
            } else if(state->gameMap[to.x][to.y] == -1) {
                visited[to] = v;
                q.push(to);
            }
        }
    }

    actions[myEntity.id] = EntityAction(
        {},
        AttackAction({}, AutoAttack(attackRange, vector<EntityType>()))
    );
}

Entity* AttackManager::getNearestAlly(Entity& myEntity) {
    uint minDist = 1e9;
    Entity* ally;
    for (auto& entry : state->mySoldiers) {
        if (entry.id == myEntity.id) {
            continue;
        }
        auto dist = myEntity.position.dist(entry.position);
        if (dist < minDist) {
            minDist = dist;
            ally = &entry;
        }
    }

    return ally;
}
