#include "Vec2Int.hpp"

Vec2Int::Vec2Int() { }
Vec2Int::Vec2Int(int x, int y) : x(x), y(y) { }
Vec2Int Vec2Int::readFrom(InputStream& stream) {
    Vec2Int result;
    result.x = stream.readInt();
    result.y = stream.readInt();
    return result;
}
void Vec2Int::writeTo(OutputStream& stream) const {
    stream.write(x);
    stream.write(y);
}

uint Vec2Int::dist(const Vec2Int& to) const {
    return abs(x - to.x) + abs(y - to.y);
}

uint Vec2Int::sqrDist(const Vec2Int& to) const {
    return (x - to.x) * (x - to.x) + (y - to.y) * (y - to.y);
}

bool Vec2Int::operator ==(const Vec2Int& other) const {
    return x == other.x && y == other.y;
}

bool Vec2Int::operator !=(const Vec2Int& other) const {
    return !(*this == other);
}

bool Vec2Int::operator <(const Vec2Int& other) const {
    if (x != other.x) {
        return x < other.x;
    }

    return y < other.y;
}

size_t std::hash<Vec2Int>::operator ()(const Vec2Int& value) const {
    size_t result = 0;
    result ^= std::hash<int>{}(value.x) + 0x9e3779b9 + (result<<6) + (result>>2);
    result ^= std::hash<int>{}(value.y) + 0x9e3779b9 + (result<<6) + (result>>2);
    return result;
}
