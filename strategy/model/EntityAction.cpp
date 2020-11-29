#include "EntityAction.hpp"

EntityAction::EntityAction() { }

EntityAction::EntityAction(
    std::optional<MoveAction> moveAction,
    std::optional<BuildAction> buildAction,
    std::optional<AttackAction> attackAction,
    std::optional<RepairAction> repairAction) :
        moveAction(moveAction), buildAction(buildAction),
        attackAction(attackAction), repairAction(repairAction) { }

EntityAction::EntityAction(
    std::optional<MoveAction> moveAction,
    std::optional<AttackAction> attackAction) : moveAction(moveAction), attackAction(attackAction) { }

EntityAction EntityAction::readFrom(InputStream& stream) {
    EntityAction result;
    if (stream.readBool()) {
        result.moveAction = MoveAction::readFrom(stream);
    }

    if (stream.readBool()) {
        result.buildAction = BuildAction::readFrom(stream);
    }

    if (stream.readBool()) {
        result.attackAction = AttackAction::readFrom(stream);
    }

    if (stream.readBool()) {
        result.repairAction = RepairAction::readFrom(stream);
    }

    return result;
}
void EntityAction::writeTo(OutputStream& stream) const {
    if (moveAction) {
        stream.write(true);
        moveAction.value().writeTo(stream);
    } else {
        stream.write(false);
    }
    if (buildAction) {
        stream.write(true);
        buildAction.value().writeTo(stream);
    } else {
        stream.write(false);
    }
    if (attackAction) {
        stream.write(true);
        attackAction.value().writeTo(stream);
    } else {
        stream.write(false);
    }
    if (repairAction) {
        stream.write(true);
        repairAction.value().writeTo(stream);
    } else {
        stream.write(false);
    }
}
