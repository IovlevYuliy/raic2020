#ifndef _UNIT_BUILDER_MANAGER_HPP_
#define _UNIT_BUILDER_MANAGER_HPP_

#include "model/Model.hpp"
#include "common.hpp"

class UnitManager {
public:
    uint totalPopulation;
    uint usedPopulation;
    uint curBuilderCount;

    UnitManager();

    void setPopulation(uint total, uint used, uint builderCount) {
        totalPopulation = total;
        usedPopulation = used;
        curBuilderCount = builderCount;
    }

    void createUnits(vector<Entity>& myEntities,
        unordered_map<int, EntityAction>& actions,
        EntityType unitType,
        bool force = false);

    void createBuilder(Entity& builderBaser,
        unordered_map<int, EntityAction>& actions,
        bool force = false);

    void createRanger(Entity& rangerBase,
        unordered_map<int, EntityAction>& actions);
};

#endif
