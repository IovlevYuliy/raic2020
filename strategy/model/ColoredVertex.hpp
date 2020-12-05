#ifndef _MODEL_COLORED_VERTEX_HPP_
#define _MODEL_COLORED_VERTEX_HPP_

#include "../stream/Stream.hpp"
#include "Color.hpp"
#include "Vec2Float.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <optional>

class ColoredVertex {
public:
    std::optional<Vec2Float> worldPos;
    Vec2Float screenOffset;
    Color color;
    ColoredVertex();
    ColoredVertex(std::optional<Vec2Float> worldPos, Vec2Float screenOffset, Color color);
    static ColoredVertex readFrom(InputStream& stream);
    void writeTo(OutputStream& stream) const;
};

#endif
