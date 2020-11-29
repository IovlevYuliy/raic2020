#include "AttackManager.hpp"

AttackManager::AttackManager() {}

void AttackManager::getAims(vector<Entity>& enemyEntities) {
    turrets.clear();
    bases.clear();
    others.clear();

    for (auto& entry : enemyEntities) {
        if (entry.entityType == EntityType::TURRET) {
            turrets.push_back(entry);
            continue;
        }

        if (isBase(entry.entityType)) {
            bases.push_back(entry);
            continue;
        }

        others.push_back(entry);
    }
}

void AttackManager::goToAttack(Entity& myEntity, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions) {
    if (isBuilder(myEntity)) {
        goToResources(myEntity, gameMap, actions);
        return;
    }

    uint mapSize = (uint)gameMap.size();

    Entity* aim = NULL;
    if (!turrets.empty()) {
        aim = &turrets[0];
    } else if (!bases.empty()){
        aim = &bases[0];
    } else if (!others.empty()) {
        aim = &others[0];
    }

    if (aim) {
        auto mvAction = MoveAction(aim->position, true, true);
        auto attackAction = AttackAction(
            {},
            AutoAttack(entityProperties[myEntity.entityType].sightRange, vector<EntityType>())
        );

        actions[myEntity.id] = EntityAction(mvAction, attackAction);
    } else {
        Vec2Int corner(mapSize - 1, mapSize - 1);
        actions[myEntity.id] = EntityAction(
            MoveAction(Vec2Int(mapSize - 1, mapSize - 1), true, false)
        );
    }
}

void AttackManager::goToResources(Entity& myEntity, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions) {
    uint mapSize = (uint)gameMap.size();
    queue<Vec2Int> q;
    unordered_set<Vec2Int> visited;
    q.push(myEntity.position);
    visited.insert(myEntity.position);

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

            if (gameMap[to.x][to.y] == EntityType::RESOURCE) {
                actions[myEntity.id] = EntityAction(
                    MoveAction(to, true, false),
                    AttackAction(
                        {},
                        AutoAttack(entityProperties[myEntity.entityType].sightRange,
                            vector<EntityType>{ EntityType::RESOURCE })
                    )
                );

                return;
            } else if(gameMap[to.x][to.y] == -1) {
                visited.insert(to);
                q.push(to);
            }
        }
    }
}

