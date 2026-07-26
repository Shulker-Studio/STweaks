#include "Feature/Features.h"

namespace lk::features {
bool enableAll() {
    bool success = true;
    success      = enableFixChunkCopyItem() && success;
    success      = enableFixBundlesCopyItem() && success;
    success      = enableForbidSilverfish() && success;
    success      = enablePortalMobPolicy() && success;
    success      = enableWitherRestriction() && success;
    success      = enableExplosionProtection() && success;
    success      = enableConfiguredLoot() && success;

    if (!success) disableAll();
    return success;
}

void disableAll() {
    disableConfiguredLoot();
    disableExplosionProtection();
    disableWitherRestriction();
    disablePortalMobPolicy();
    disableForbidSilverfish();
    disableFixBundlesCopyItem();
    disableFixChunkCopyItem();
}

} // namespace lk::features
