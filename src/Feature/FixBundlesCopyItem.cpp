#include <ll/api/memory/Hook.h>
#include <mc/world/actor/Hopper.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/block/actor/DropperBlockActor.h>

#include <string_view>

#include "Config/Config.h"
#include "Feature/Features.h"
#include "Feature/HookResult.h"

namespace lk::features {
namespace {

bool isMinecraftBundle(std::string_view itemType) {
    return itemType.starts_with("minecraft:") && itemType.ends_with("bundle");
}

LL_TYPE_INSTANCE_HOOK(
    HopperAddItemHook,
    ll::memory::HookPriority::Normal,
    Hopper,
    &Hopper::_tryMoveInItem,
    bool,
    ::BlockSource& region,
    ::Container&   container,
    ::ItemStack&   item,
    int            slot,
    int            face,
    int            itemCount
) {
    if (!config::gConfig.features.fixBundlesCopyItem) return origin(region, container, item, slot, face, itemCount);

    if (isMinecraftBundle(item.getTypeName())) return false;
    return origin(region, container, item, slot, face, itemCount);
}

LL_TYPE_STATIC_HOOK(
    DropperTryMoveInItemsHook,
    ll::memory::HookPriority::Normal,
    DropperBlockActor,
    &DropperBlockActor::_tryMoveInItemsAndDepleteStack,
    bool,
    ::Container& container,
    ::ItemStack& item,
    int          stackSizeLimit,
    int          slot,
    int          face
) {
    if (!config::gConfig.features.fixBundlesCopyItem) return origin(container, item, stackSizeLimit, slot, face);

    if (isMinecraftBundle(item.getTypeName())) return false;
    return origin(container, item, stackSizeLimit, slot, face);
}

bool gEnabled = false;

} // namespace

bool enableFixBundlesCopyItem() {
    if (gEnabled) return true;

    if (!hookSucceeded(HopperAddItemHook::hook())) return false;
    if (!hookSucceeded(DropperTryMoveInItemsHook::hook())) {
        HopperAddItemHook::unhook();
        return false;
    }
    gEnabled = true;
    return true;
}

void disableFixBundlesCopyItem() {
    if (!gEnabled) return;
    DropperTryMoveInItemsHook::unhook();
    HopperAddItemHook::unhook();
    gEnabled = false;
}

} // namespace lk::features
