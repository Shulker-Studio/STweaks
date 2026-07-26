#include <ll/api/memory/Hook.h>
#include <mc/world/effect/InfestedMobEffect.h>

#include "Config/Config.h"
#include "Feature/Features.h"
#include "Feature/HookResult.h"

namespace lk::features {
namespace {

LL_TYPE_INSTANCE_HOOK(
    InfestedMobEffectOnActorHurtHook,
    ll::memory::HookPriority::Normal,
    InfestedMobEffect,
    &InfestedMobEffect::$onActorHurt,
    void,
    ::Actor&                   actor,
    int                        amplifier,
    ::ActorDamageSource const& source,
    float                      damage
) {
    if (config::gConfig.features.forbidSilverfish) return;

    return origin(actor, amplifier, source, damage);
}

bool gEnabled = false;

} // namespace

bool enableForbidSilverfish() {
    if (gEnabled) return true;
    gEnabled = hookSucceeded(InfestedMobEffectOnActorHurtHook::hook());
    return gEnabled;
}

void disableForbidSilverfish() {
    if (!gEnabled) return;
    InfestedMobEffectOnActorHurtHook::unhook();
    gEnabled = false;
}

} // namespace lk::features
