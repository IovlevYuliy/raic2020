#ifndef _ATTACK_MANAGER_HPP_
#define _ATTACK_MANAGER_HPP_

#include "model/Model.hpp"
#include "common.hpp"

class AttackManager {
public:
    unordered_map<EntityType, EntityProperties> entityProperties;
    vector<Entity> enemyBuildings;
    vector<Entity> others;
    vector<Entity> myBases;

    AttackManager();

    void goToAttack(Entity& myEntity, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions);
    void goToResources(Entity& myEntity, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions);

    optional<Vec2Int> needDefense();
    bool troopIsReady(Entity& myEntity, vector<vector<char>>& gameMap);
    void getAims(vector<Entity>& enemyEntities, vector<Entity>& myEntities);
};

#endif
