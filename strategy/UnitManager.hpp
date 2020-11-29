#ifndef _UNIT_BUILDER_MANAGER_HPP_
#define _UNIT_BUILDER_MANAGER_HPP_

#include "model/Model.hpp"
#include "common.hpp"

class UnitManager {
public:
    unordered_map<EntityType, EntityProperties> entityProperties;

    uint totalPopulation;
    uint usedPopulation;
    uint curBuilderCount;

    UnitManager();

    void setPopulation(uint total, uint used, uint builderCount) {
        totalPopulation = total;
        usedPopulation = used;
        curBuilderCount = builderCount;
    }

    void createUnits(vector<Entity>& myEntities, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions,
        EntityType unitType,
        bool force = false);

    void createBuilder(Entity& builderBaser,
        unordered_map<int, EntityAction>& actions,
        bool force = false);

    void createRanger(Entity& rangerBase, vector<vector<char>>& gameMap,
        unordered_map<int, EntityAction>& actions);
};

#endif
