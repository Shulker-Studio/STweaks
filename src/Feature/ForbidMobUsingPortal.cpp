#include <ll/api/memory/Hook.h>
#include <mc/world/actor/Mob.h>

#include "Config/Config.h"
#include "Feature/Features.h"
#include "Feature/HookResult.h"
#include "Rule/Rules.h"

namespace lk::features {
namespace {

LL_TYPE_INSTANCE_HOOK(
    MobChangeDimHook,
    ll::memory::HookPriority::Normal,
    Mob,
    &Mob::$canChangeDimensionsUsingPortal,
    bool
) {
    if (!config::gConfig.features.portalMobPolicy) return origin();

    if (!rules::isMobPortalAllowed(getTypeName())) return false;
    return origin();
}

bool gEnabled = false;

} // namespace

bool enablePortalMobPolicy() {
    if (gEnabled) return true;
    gEnabled = hookSucceeded(MobChangeDimHook::hook());
    return gEnabled;
}

void disablePortalMobPolicy() {
    if (!gEnabled) return;
    MobChangeDimHook::unhook();
    gEnabled = false;
}

} // namespace lk::features
