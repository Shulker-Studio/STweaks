#include <ll/api/memory/Hook.h>
#include <mc/world/item/ItemStack.h>
#include <mc/world/level/block/Block.h>
#include <mc/world/level/block/BlockType.h>
#include <mc/world/level/block/ResourceDrops.h>
#include <mc/world/level/block/ResourceDropsContext.h>

#include "Config/Config.h"
#include "Feature/Features.h"
#include "Feature/HookResult.h"
#include "Rule/Rules.h"

namespace lk::features {
namespace {

LL_TYPE_INSTANCE_HOOK(
    GetResourceDropsHook,
    HookPriority::Normal,
    BlockType,
    &BlockType::getResourceDrops,
    ResourceDrops,
    ::Block const&                block,
    ::IRandom&                    random,
    ::ResourceDropsContext const& context
) {
    auto drops = origin(block, random, context);
    if (!config::gConfig.features.configuredLoot) return drops;

    rules::appendLootDrops(drops, block, random, context);
    return drops;
}

bool gEnabled = false;

} // namespace

bool enableConfiguredLoot() {
    if (gEnabled) return true;
    gEnabled = hookSucceeded(GetResourceDropsHook::hook());
    return gEnabled;
}

void disableConfiguredLoot() {
    if (!gEnabled) return;
    GetResourceDropsHook::unhook();
    gEnabled = false;
}

} // namespace lk::features
