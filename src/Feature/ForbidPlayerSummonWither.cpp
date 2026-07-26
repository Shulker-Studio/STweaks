#include <ll/api/memory/Hook.h>
#include <mc/world/level/BlockPos.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/Level.h>
#include <mc/world/level/block/SkullBlock.h>
#include <mc/world/level/dimension/Dimension.h>

#include "Config/Config.h"
#include "Feature/Features.h"
#include "Feature/HookResult.h"
#include "Rule/Rules.h"

namespace lk::features {
namespace {

LL_TYPE_INSTANCE_HOOK(
    SkullBlockCheckMobSpawnHook,
    HookPriority::Normal,
    SkullBlock,
    &SkullBlock::checkMobSpawn,
    bool,
    ::Level&          level,
    ::BlockSource&    region,
    ::BlockPos const& pos
) {
    if (!config::gConfig.features.witherRestriction) return origin(level, region, pos);

    if (!rules::isWitherSummonAllowed(region.mDimension.getDimensionId().id, pos)) return false;
    return origin(level, region, pos);
}

bool gEnabled = false;

} // namespace

bool enableWitherRestriction() {
    if (gEnabled) return true;
    gEnabled = hookSucceeded(SkullBlockCheckMobSpawnHook::hook());
    return gEnabled;
}

void disableWitherRestriction() {
    if (!gEnabled) return;
    SkullBlockCheckMobSpawnHook::unhook();
    gEnabled = false;
}

} // namespace lk::features
