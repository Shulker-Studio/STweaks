#include <ll/api/memory/Hook.h>
#include <mc/world/level/BlockSource.h>
#include <mc/world/level/Explosion.h>

#include "Config/Config.h"
#include "Feature/Features.h"
#include "Feature/HookResult.h"
#include "Rule/Rules.h"

namespace lk::features {
namespace {

LL_TYPE_INSTANCE_HOOK(ExplodeHook, HookPriority::High, Explosion, &Explosion::explode, bool, ::IRandom& random) {
    if (!config::gConfig.features.explosionProtection) return origin(random);

    rules::applyExplosionRules(mRegion.getDimensionId().id, mPos.get(), mBreaking, mFire, mDamageScaling);
    return origin(random);
}

bool gEnabled = false;

} // namespace

bool enableExplosionProtection() {
    if (gEnabled) return true;
    gEnabled = hookSucceeded(ExplodeHook::hook());
    return gEnabled;
}

void disableExplosionProtection() {
    if (!gEnabled) return;
    ExplodeHook::unhook();
    gEnabled = false;
}

} // namespace lk::features
