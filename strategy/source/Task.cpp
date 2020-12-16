#include "Task.hpp"

Task::Task() {}

Task::Task(optional<Vec2Int> pos_, EntityType type_, int ttl_) {
    pos = pos_;
    type = type_;
    ttl = ttl_;
    finished = false;
}
