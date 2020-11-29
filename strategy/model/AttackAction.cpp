#include "AttackAction.hpp"

AttackAction::AttackAction() { }
AttackAction::AttackAction(std::optional<int> target, std::optional<AutoAttack> autoAttack) : target(target), autoAttack(autoAttack) { }
AttackAction AttackAction::readFrom(InputStream& stream) {
    AttackAction result;
    if (stream.readBool()) {
        result.target = stream.readInt();
    }
    if (stream.readBool()) {
        result.autoAttack = AutoAttack::readFrom(stream);
    }

    return result;
}
void AttackAction::writeTo(OutputStream& stream) const {
    if (target) {
        stream.write(true);
        stream.write(target.value());
    } else {
        stream.write(false);
    }
    if (autoAttack) {
        stream.write(true);
        autoAttack.value().writeTo(stream);
    } else {
        stream.write(false);
    }
}
