#ifndef _ATTACK_MANAGER_HPP_
#define _ATTACK_MANAGER_HPP_

#include "common.hpp"
#include "GameState.hpp"

class AttackManager {
public:
    GameState* state;
    vector<Vec2Int> corners;

    AttackManager();

    void goToAttack(Entity& myEntity, unordered_map<int, EntityAction>& actions);
    void goToResources(Entity& myEntity, unordered_map<int, EntityAction>& actions);

    Entity* getNearestAlly(Entity& myEntity);

    optional<Vec2Int> getStep(Entity& myEntity, Vec2Int& dest);

    pair<Entity*,Entity*> getTargets(Entity& myEntity);

    bool troopIsReady(Entity& myEntity);

    void tryToHealRangers(Entity& myEntity, unordered_map<int, EntityAction>& actions);

    bool canAttackTurret(Entity& turret);

    void simEnemyAttack(Entity& enemy);

    bool killOwnUnits(Entity& myEntity, unordered_map<int, EntityAction>& actions);
};

#endif
