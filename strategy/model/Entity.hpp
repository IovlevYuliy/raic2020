#ifndef _MODEL_ENTITY_HPP_
#define _MODEL_ENTITY_HPP_

#include "../stream/Stream.hpp"
#include "EntityType.hpp"
#include "Vec2Int.hpp"
#include <memory>
#include <stdexcept>
#include <string>

class Entity {
public:
    int id;
    std::shared_ptr<int> playerId;
    EntityType entityType;
    Vec2Int position;
    int health;
    bool active;
    uint targets = 0;
    int distToBase;
    uint underAttack = 0;
    bool busy;
    bool isKilled = false;
    Entity();
    Entity(int id, std::shared_ptr<int> playerId, EntityType entityType, Vec2Int position, int health, bool active);
    static Entity readFrom(InputStream& stream);
    void writeTo(OutputStream& stream) const;
};

#endif
