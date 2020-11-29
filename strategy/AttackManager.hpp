#ifndef _ATTACK_MANAGER_HPP_
#define _ATTACK_MANAGER_HPP_

#include "model/Model.hpp"
#include "common.hpp"

class AttackManager {
public:
    unordered_map<EntityType, EntityProperties> entityProperties;
    vector<Entity> turrets;
    vector<Entity> bases;
    vector<Entity> others;

    AttackManager();

    void goToAttack(Entity& myEntity, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions);
    void goToResources(Entity& myEntity, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions);
    void getAims(vector<Entity>& enemyEntities);
};

#endif
