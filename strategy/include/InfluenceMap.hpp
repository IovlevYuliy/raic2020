#ifndef _INFLUENCE_MAP_HPP_
#define _INFLUENCE_MAP_HPP_

#include "common.hpp"
#include "DebugInterface.hpp"
#include "GameState.hpp"

class InfluenceMap {
   public:
   vector<vector<float>> infMap;
   uint mapSize;

   InfluenceMap() {}

   InfluenceMap(uint size) {
       infMap.resize(size);
       infMap.assign(size, vector<float>(size, 0.0));
    }

    InfluenceMap(InfluenceMap& copyFrom) {
        mapSize = static_cast<uint>(copyFrom.infMap.size());
        infMap.resize(mapSize);
        for (uint i = 0; i < mapSize; ++i) {
            infMap[i].resize(mapSize);
            for (uint j = 0; j < mapSize; ++j) {
                infMap[i][j] = copyFrom.infMap[i][j];
            }
        }
    }

    void clear(uint size) {
        infMap.resize(size);
        infMap.assign(size, vector<float>(size, 0.0));
        mapSize = size;
    }

    void add(InfluenceMap& add) {
        for (uint i = 0; i < static_cast<uint>(add.infMap.size()); ++i) {
            for (uint j = 0; j < static_cast<uint>(add.infMap[i].size()); ++j) {
                infMap[i][j] += add.infMap[i][j];
            }
        }
    }

    void sub(InfluenceMap& sub) {
        for (uint i = 0; i < static_cast<uint>(sub.infMap.size()); ++i) {
            for (uint j = 0; j < static_cast<uint>(sub.infMap[i].size()); ++j) {
                infMap[i][j] += sub.infMap[i][j];
            }
        }
    }

    void fillInfluence(Entity& entity, int sz, int range, int myId) {
        if (isTurret(entity)) {
            int sign = *entity.playerId == myId ? 1 : -1;
            int add = (entity.health + 4) / 5;
            Vec2Int pos;
            for (int i = entity.position.x - range; i <= entity.position.x + sz + range; ++i) {
                for (int j = entity.position.y - range; j <= entity.position.y + sz + range; ++j) {
                    pos.x = i;
                    pos.y = j;
                    if (isOutOfMap(pos, mapSize)) {
                        continue;
                    }
                    int dist = min(abs(i - entity.position.x), abs(i - entity.position.x - sz + 1)) +
                               min(abs(j - entity.position.y), abs(j - entity.position.y - sz + 1));
                    if (dist <= range) {
                        infMap[pos.x][pos.y] += sign * add;
                    }
                }
            }
            return;
        }

        if (isBuilding(entity.entityType)) {
            Vec2Int pos;
            int sign = *entity.playerId == myId ? 1 : -1;
            for (int i = entity.position.x - range; i <= entity.position.x + sz + range; ++i) {
                for (int j = entity.position.y - range; j <= entity.position.y + sz + range; ++j) {
                    pos.x = i;
                    pos.y = j;
                    if (isOutOfMap(pos, mapSize)) {
                        continue;
                    }
                    int dist = min(abs(i - entity.position.x), abs(i - entity.position.x - sz + 1)) +
                               min(abs(j - entity.position.y), abs(j - entity.position.y - sz + 1));
                    if (dist <= range) {
                        infMap[pos.x][pos.y] += sign;
                    }
                }
            }
            return;
        }

        int sign = *entity.playerId == myId ? 1 : -1;
        int add = (entity.health + 4) / 5;
        Vec2Int pos;
        for (int i = -range; i <= range; ++i) {
            for (int j = -(range - abs(i)); j <= (range - abs(i)); ++j) {
                pos.x = entity.position.x + i;
                pos.y = entity.position.y + j;
                if (!isOutOfMap(pos, mapSize)) {
                    infMap[pos.x][pos.y] += sign * add;
                }
            }
        }
    }

    float getValue(Vec2Int& pos) {
        return infMap[pos.x][pos.y];
    }

    float getValue(uint x, uint y) {
        return infMap[x][y];
    }

    float getRegionInfluence(Vec2Int& pos, int range) {
        Vec2Int cur;
        int sum = 0;
        for (int i = -range; i <= range; ++i) {
            for (int j = -(range - abs(i)); j <= (range - abs(i)); ++j) {
                cur.x = pos.x + i;
                cur.y = pos.y + j;
                if (!isOutOfMap(cur, mapSize)) {
                    sum += infMap[cur.x][cur.y];
                }
            }
        }

        return sum;
    }

    void draw(DebugInterface* debugInterface) {
        for (uint i = 0; i < mapSize - 1; ++i) {
            for (uint j = 0; j < mapSize - 1; ++j) {
                auto dc = DebugCommand::Add(shared_ptr<DebugData>(new DebugData::Primitives(
                    vector<ColoredVertex>{
                        ColoredVertex(Vec2Float(i, j), Vec2Float(0, 0), getColor(infMap[i][j])),
                        ColoredVertex(Vec2Float(i, j + 1), Vec2Float(0, 0), getColor(infMap[i][j + 1])),
                        ColoredVertex(Vec2Float(i + 1, j + 1), Vec2Float(0, 0), getColor(infMap[i + 1][j + 1])),
                        ColoredVertex(Vec2Float(i, j), Vec2Float(0, 0), getColor(infMap[i][j])),
                        ColoredVertex(Vec2Float(i + 1, j), Vec2Float(0, 0), getColor(infMap[i + 1][j])),
                        ColoredVertex(Vec2Float(i + 1, j + 1), Vec2Float(0, 0), getColor(infMap[i + 1][j + 1]))
                    },
                    PrimitiveType::TRIANGLES)));
                debugInterface->send(dc);
            }
        }
    }

    Color getColor(float val) {
        if (val == 0) {
            return Color(0, 0, 0, 0);
        }
        if (val > 0) {
            return Color(0, val, 0, 0.3);
        }

        return Color(-val, 0, 0, 0.3);
    }
};

#endif
