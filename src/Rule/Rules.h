#pragma once

#include <string_view>

namespace ll::io {
class Logger;
}

class Block;
class BlockPos;
class IRandom;
class Vec3;
struct ResourceDrops;
struct ResourceDropsContext;

namespace lk::config {
struct Settings;
}

namespace lk::rules {

void initialize(config::Settings const& settings, ll::io::Logger& logger);

bool isMobPortalAllowed(std::string_view entityType);
bool isWitherSummonAllowed(int dimensionId, BlockPos const& pos);

void applyExplosionRules(int dimensionId, Vec3 const& pos, bool& breaking, bool& fire, float& damageScaling);
void appendLootDrops(
    ResourceDrops&              resourceDrops,
    Block const&                block,
    IRandom&                    random,
    ResourceDropsContext const& context
);

} // namespace lk::rules
