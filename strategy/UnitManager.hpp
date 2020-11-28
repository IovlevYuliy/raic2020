#ifndef _UNIT_BUILDER_MANAGER_HPP_
#define _UNIT_BUILDER_MANAGER_HPP_

#include "model/Model.hpp"
#include "common.hpp"

class UnitManager {
public:
    uint totalPopulation;
    uint usedPopulation;

    UnitManager();

    void setPopulation(uint total, uint used) {
        totalPopulation = total;
        usedPopulation = used;
    }

    void createUnit(vector<Entity>& myEntities, unordered_map<int, EntityAction>& actions, EntityType unitType);

    void createBuilder(vector<Entity>& myEntities, unordered_map<int, EntityAction>& actions);
    void createRanger(vector<Entity>& myEntities, unordered_map<int, EntityAction>& actions);
};

#endif
