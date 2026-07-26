#pragma once

namespace lk::features {

bool enableAll();
void disableAll();

bool enableFixBundlesCopyItem();
void disableFixBundlesCopyItem();

bool enableFixChunkCopyItem();
void disableFixChunkCopyItem();

bool enableForbidSilverfish();
void disableForbidSilverfish();

bool enablePortalMobPolicy();
void disablePortalMobPolicy();

bool enableWitherRestriction();
void disableWitherRestriction();

bool enableExplosionProtection();
void disableExplosionProtection();

bool enableConfiguredLoot();
void disableConfiguredLoot();

} // namespace lk::features
