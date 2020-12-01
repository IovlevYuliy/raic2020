#ifndef _ATTACK_MANAGER_HPP_
#define _ATTACK_MANAGER_HPP_

#include "common.hpp"
#include "GameState.hpp"

class AttackManager {
public:
    GameState* state;

    AttackManager();
    AttackManager(GameState& state_);

    void goToAttack(Entity& myEntity, unordered_map<int, EntityAction>& actions);
    void goToResources(Entity& myEntity, unordered_map<int, EntityAction>& actions);

    optional<Vec2Int> needDefense();
    bool troopIsReady(Entity& myEntity);
};

#endif
