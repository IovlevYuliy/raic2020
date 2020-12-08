#ifndef _INFLUENCE_MAP_HPP_
#define _INFLUENCE_MAP_HPP_

#include "common.hpp"

class InfluenceMap {
   public:
   vector<vector<float>> infMap;

    InfluenceMap() { }

    InfluenceMap(uint size) {
        infMap.resize(size);
        infMap.assign(size, vector<float>(size, 0.0));
    }

    InfluenceMap(InfluenceMap& copyFrom) {
        infMap.resize(copyFrom.infMap.size());

        for (uint i = 0; i < static_cast<uint>(copyFrom.infMap.size()); ++i) {
            infMap[0].resize(copyFrom.infMap[0].size());
            for (uint j = 0; j < static_cast<uint>(copyFrom.infMap[0].size()); ++j) {
                infMap[i][j] = copyFrom.infMap[i][j];
            }
        }
    }

    void add(InfluenceMap& add) {
        for (uint i = 0; i < static_cast<uint>(add.infMap.size()); ++i) {
            for (uint j = 0; j < static_cast<uint>(add.infMap[0].size()); ++j) {
                infMap[i][j] += add.infMap[i][j];
            }
        }
    }

    void sub(InfluenceMap& sub) {
        for (uint i = 0; i < static_cast<uint>(sub.infMap.size()); ++i) {
            for (uint j = 0; j < static_cast<uint>(sub.infMap[0].size()); ++j) {
                infMap[i][j] += sub.infMap[i][j];
            }
        }
    }
};

#endif
