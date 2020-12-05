#include "ColoredVertex.hpp"

ColoredVertex::ColoredVertex() { }
ColoredVertex::ColoredVertex(std::optional<Vec2Float> worldPos, Vec2Float screenOffset, Color color) : worldPos(worldPos), screenOffset(screenOffset), color(color) { }
ColoredVertex ColoredVertex::readFrom(InputStream& stream) {
    ColoredVertex result;
    if (stream.readBool()) {
        result.worldPos = Vec2Float::readFrom(stream);
    }
    result.screenOffset = Vec2Float::readFrom(stream);
    result.color = Color::readFrom(stream);
    return result;
}
void ColoredVertex::writeTo(OutputStream& stream) const {
    if (worldPos) {
        stream.write(true);
        worldPos.value().writeTo(stream);
    } else {
        stream.write(false);
    }
    screenOffset.writeTo(stream);
    color.writeTo(stream);
}
