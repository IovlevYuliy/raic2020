#include "AttackManager.hpp"

AttackManager::AttackManager() {
    state = GameState::getState();
    corners = {
        Vec2Int(79, 79),
        Vec2Int(0, 79),
        Vec2Int(79, 0)
    };
}

void AttackManager::goToAttack(Entity& myEntity, unordered_map<int, EntityAction>& actions) {
    if (isBuilder(myEntity)) {
        if (state->remainingResources) {
            goToResources(myEntity, actions);
        } else {
            if (state->currentTick < 100) {
                actions[myEntity.id] = EntityAction(
                    MoveAction(Vec2Int(state->mapSize - 1, state->mapSize - 1), true, false));
                    return;
            }
            actions[myEntity.id] = EntityAction(
                MoveAction(Vec2Int(0, 0), true, false),
                AttackAction(
                    {},
                    AutoAttack(1, vector<EntityType>())));
        }
        return;
    }

    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;
    uint damage = state->entityProperties[myEntity.entityType].attack->damage;
    uint sightRange = state->entityProperties[myEntity.entityType].sightRange;
    Vec2Int corner(state->mapSize - 1, state->mapSize - 1);

    auto targets = getTargets(myEntity);
    if (targets.first) {
        targets.first->health -= damage;
        actions[myEntity.id] = EntityAction(
            {},
            AttackAction(targets.first->id, {})
        );
        return;
    }

    if (killOwnUnits(myEntity, actions)) {
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
            MoveAction(Vec2Int(15, 15), true, true));
        return;
    }

    if (state->currentTick < 400 && state->isFinal) {
        actions[myEntity.id] = EntityAction(
            MoveAction(Vec2Int(40, 40), true, true));
        return;
    }

    // cerr << "my position " << myEntity.position.x << ' ' << myEntity.position.y << endl;

    // if (targets.first) {
    //     cerr << "first target " << targets.first->position.x << ' ' << targets.first->position.y << endl;
    // }
    // if (targets.second) {
    //     cerr << "second target " << targets.second->position.x << ' ' << targets.second->position.y << endl;
    // }

    // int allyCount = state->getAlliesAround(myEntity.position, 3);
    // if (allyCount < 2) {
    //     auto ally = getNearestAlly(myEntity);
    //     if (ally) {
    //         actions[myEntity.id] = EntityAction(
    //             MoveAction(ally->position, true, true)
    //         );
    //         return;
    //     }
    // }

    if (targets.second) {
        if (isTurret(*targets.second) && targets.second->active) {
            auto res = getDistance(myEntity, *targets.second, state->entityProperties);
            if (res.first == attackRange + 1 && !canAttackTurret(*targets.second)) {
                actions[myEntity.id] = EntityAction();
                return;
            }
        }
        actions[myEntity.id] = EntityAction(
            MoveAction(targets.second->position, true, true));
        // auto step = getStep(myEntity, targets.second->position);
        // if (step) {
        //     actions[myEntity.id] = EntityAction(
        //         MoveAction(step.value(), true, true)
        //     );
        // } else  {
        //     actions[myEntity.id] = EntityAction();
        // }
        // } else {
        //     auto ally = getNearestAlly(myEntity);
        //     if (ally) {
        //         actions[myEntity.id] = EntityAction(
        //             MoveAction(ally->position, true, false));
        //     } else {
        //         actions[myEntity.id] = EntityAction(
        //             MoveAction(corner, true, false)
        //         );
        //     }
        // }
    } else {
        if (state->isFinal) {
            actions[myEntity.id] = EntityAction(
                MoveAction(corners[0], true, true),
                AttackAction(
                    {},
                    AutoAttack(attackRange, vector<EntityType>())
                )
            );

            return;
        }

        uint ind = 0;
        uint minDist = 1e9;
        for (uint i = 0; i < static_cast<uint>(corners.size()); ++i) {
            uint d = myEntity.position.dist(corners[i]);
            if (d < 10) {
                corners.erase(corners.begin() + i);
            }
        }

        if (corners.empty()) {
            actions[myEntity.id] = EntityAction(
                MoveAction(Vec2Int(rand() % state->mapSize, rand() % state->mapSize), true, false),
                AttackAction(
                    {},
                    AutoAttack(attackRange, vector<EntityType>())));
            return;
        }

        for (uint i = 0; i < static_cast<uint>(corners.size()); ++i) {
            uint d = myEntity.position.dist(corners[i]);

            if (d < minDist) {
                ind = i;
                minDist = d;
            }
        }

        actions[myEntity.id] = EntityAction(
            MoveAction(corners[ind], true, true),
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

        uint dist = getDistance(enemy, myEntity, state->entityProperties).first;

        if (enemy.health < minHp && dist <= attackRange) {
            minHp = enemy.health;
            targetInRange = &enemy;
        }

        if (2 * dist < minDist) {
            minDist = dist;
            nearestTarget = &enemy;
        }
    }

    if (targetInRange) {
        return make_pair(targetInRange, nearestTarget);
    }

    for (auto& enemy: state->enemyBuilders) {
        if (enemy.health <= 0) continue;

        uint dist = getDistance(enemy, myEntity, state->entityProperties).first;

        if (enemy.health < minHp && dist <= attackRange) {
            minHp = enemy.health;
            targetInRange = &enemy;
        }

        if (dist <= minDist) {
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

        if (enemy.health <= minHp && dist.first <= attackRange) {
            minHp = enemy.health;
            targetInRange = &enemy;
        }

        if (dist.first <= minDist) {
            minDist = dist.first;
            nearestTarget = &enemy;
        }
    }

    return make_pair(targetInRange, nearestTarget);
}

void AttackManager::goToResources(Entity& myEntity, unordered_map<int, EntityAction>& actions) {
    uint mapSize = state->mapSize;
    queue<Vec2Int> q;
    unordered_map<Vec2Int, Vec2Int> visited;
    q.push(myEntity.position);
    visited[myEntity.position] = myEntity.position;

    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;
    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, mapSize) || visited.count(to) || state->enemyAttackMap.getValue(to) < 0) {
                continue;
            }

            if (state->gameMap[to.x][to.y] == EntityType::RESOURCE && state->enemyAttackMap.getValue(v) >= 0) {
                visited[to] = v;
                while (visited[to] != myEntity.position) {
                    to = visited[to];
                }
                if (state->gameMap[to.x][to.y] == EntityType::RESOURCE) {
                    state->myResources++;
                    actions[myEntity.id] = EntityAction(
                        {},
                        AttackAction(state->idsMap[to.x][to.y], {})
                    );
                    return;
                }
                actions[myEntity.id] = EntityAction(
                    MoveAction(to, true, false),
                    AttackAction({}, AutoAttack(attackRange, vector<EntityType>()))
                );
                return;
            } else if (state->gameMap[to.x][to.y] == -1) {
                visited[to] = v;
                q.push(to);
            }
        }
    }

    Vec2Int v(myEntity.position.x, myEntity.position.y);
    for (uint i = 0; i < 4; ++i) {
        Vec2Int to(v.x + dx[i], v.y + dy[i]);
        if (isOutOfMap(to, mapSize) || state->enemyAttackMap.getValue(to) < 0 || state->gameMap[to.x][to.y] != -1) {
            continue;
        }
        actions[myEntity.id] = EntityAction(
            MoveAction(to, true, true),
            AttackAction({}, AutoAttack(attackRange, vector<EntityType>()))
        );
        return;
    }
    actions[myEntity.id] = EntityAction(
        MoveAction(Vec2Int(0, 0), true, true),
        AttackAction({}, AutoAttack(attackRange, vector<EntityType>()))
    );
}

Entity* AttackManager::getNearestAlly(Entity& myEntity) {
    uint minDist = 1e9;
    Entity* ally = NULL;
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

void AttackManager::tryToHealRangers(Entity& myEntity, unordered_map<int, EntityAction>& actions) {
    for (auto& entry : state->mySoldiers) {
        if (entry.health != 5) {
            continue;
        }
        auto d = entry.position.dist(myEntity.position);
        if (d == 1) {
            entry.health++;
            actions[myEntity.id] = EntityAction(
                {}, // mode
                {},  // build
                {},  // attack
                RepairAction(entry.id)
            );
            return;
        }
    }
}

bool AttackManager::canAttackTurret(Entity& turret) {
    if (turret.health > 80 && turret.underAttack >= 5) {
        return true;
    }

    if (turret.health > 50 && turret.health <= 80 && turret.underAttack >= 4) {
        return true;
    }

    if (turret.health > 20 && turret.health < 50 && turret.underAttack >= 3) {
        return true;
    }

    if (turret.health > 10 && turret.health <= 20 && turret.underAttack >= 2) {
        return true;
    }

    if (turret.health <= 10 && turret.underAttack >= 1) {
        return true;
    }

    return false;
}

optional<Vec2Int> AttackManager::getStep(Entity& myEntity, Vec2Int& dest) {
    if (myEntity.position == dest) {
        return dest;
    }

    queue<Vec2Int> q;
    unordered_map<Vec2Int, Vec2Int> visited;
    q.push(myEntity.position);
    visited[myEntity.position] = myEntity.position;
    uint dist = myEntity.position.dist(dest);

    while (!q.empty()) {
        Vec2Int v = q.front();
        q.pop();

        for (uint i = 0; i < 4; ++i) {
            Vec2Int to(v.x + dx[i], v.y + dy[i]);
            if (isOutOfMap(to, state->mapSize) || visited.count(to)) {
                continue;
            }

            if (to == dest) {
                visited[to] = v;
                while (visited[to] != myEntity.position) {
                    to = visited[to];
                }
                if (state->myInfluence.getValue(to) > abs(state->enemyAttackMap.getValue(to))) {
                    return to;
                }
                return {};
            } else if (state->gameMap[to.x][to.y] == -1 || (dist >= 10 && state->gameMap[to.x][to.y] == EntityType::RANGED_UNIT)) {
                visited[to] = v;
                q.push(to);
            }
        }
    }

    return {};
}

void AttackManager::simEnemyAttack(Entity& enemy) {
    uint attackRange = state->entityProperties[enemy.entityType].attack->attackRange;
    uint damage = state->entityProperties[enemy.entityType].attack->damage;

    Entity* targetInRange = NULL;
    uint minHp = 1e9;

    for (auto& myEntry : state->mySoldiers) {
        if (myEntry.health <= 0) continue;

        uint dist = getDistance(enemy, myEntry, state->entityProperties).first;

        if (myEntry.health < minHp && dist <= attackRange) {
            minHp = myEntry.health;
            targetInRange = &myEntry;
        }
    }

    if (targetInRange) {
        targetInRange->health -= damage;
        return;
    }

    for (auto& myEntry : state->myBuilders) {
        if (myEntry.health <= 0) continue;

        uint dist = getDistance(enemy, myEntry, state->entityProperties).first;

        if (myEntry.health < minHp && dist <= attackRange) {
            minHp = myEntry.health;
            targetInRange = &myEntry;
        }
    }

    if (targetInRange) {
        targetInRange->health -= damage;
        return;
    }

    for (auto& myEntry : state->myBuildings) {
        if (myEntry.health <= 0) continue;

        auto dist = getDistance(myEntry, enemy, state->entityProperties);

        if (myEntry.health <= minHp && dist.first <= attackRange) {
            minHp = myEntry.health;
            targetInRange = &myEntry;
        }
    }

    if (targetInRange) {
        targetInRange->health -= damage;
        return;
    }
}

bool AttackManager::killOwnUnits(Entity& myEntity, unordered_map<int, EntityAction>& actions) {
    uint attackRange = state->entityProperties[myEntity.entityType].attack->attackRange;

    Entity* target = NULL;
    vector<Entity*> targets;
    for (auto& myEntry : state->mySoldiers) {
        if (myEntry.health > 0) continue;

        uint dist = getDistance(myEntry, myEntity, state->entityProperties).first;

        if (dist <= attackRange && !myEntry.isKilled) {
            target = &myEntry;
        }
        if (dist <= attackRange && myEntry.isKilled) {
            targets.push_back(&myEntry);
        }
    }

    if (target) {
        target->isKilled = true;
        actions[myEntity.id] = EntityAction(
            {},
            AttackAction(target->id, {}));
        return true;
    }

    for (auto& myEntry : state->myBuildings) {
        if (myEntry.health > 0) continue;

        uint dist = getDistance(myEntry, myEntity, state->entityProperties).first;

        if (dist <= attackRange && !myEntry.isKilled) {
            target = &myEntry;
        }
        if (dist <= attackRange && myEntry.isKilled) {
            targets.push_back(&myEntry);
        }
    }

    if (target) {
        target->isKilled = true;
        actions[myEntity.id] = EntityAction(
            {},
            AttackAction(target->id, {})
        );
        return true;
    }

    if (targets.empty()) {
        return false;
    }

    uint ind = rand() % targets.size();
    actions[myEntity.id] = EntityAction(
        {},
        AttackAction(targets[ind]->id, {}));

    return true;
}