#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "common.hpp"

class Task {
   public:
    optional<Vec2Int> pos;
    EntityType type;
    bool finished;

    int ttl;

    Task();

    Task(optional<Vec2Int> pos_, EntityType type_, int ttl_ = 50);
};

#endif
