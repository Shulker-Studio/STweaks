#pragma once

#include <string>
#include <vector>

namespace ll::mod {
class Mod;
}

namespace lk::config {

struct Position {
    int x = 0;
    int y = 0;
    int z = 0;
};

struct FeatureSettings {
    bool fixChunkCopyItem    = true;
    bool fixBundlesCopyItem  = true;
    bool forbidSilverfish    = true;
    bool portalMobPolicy     = true;
    bool witherRestriction   = true;
    bool explosionProtection = true;
    bool configuredLoot      = true;
};

struct PortalMobPolicy {
    bool        enabled = true;
    std::string mode    = "denylist";
    std::vector<std::string>
        entityTypes{"minecraft:iron_golem", "minecraft:guardian", "minecraft:wither", "minecraft:zombie_pigman"};
};

struct WitherRule {
    std::string id          = "overworld_protection";
    bool        enabled     = true;
    int         priority    = 100;
    int         dimensionId = 0;
    Position    min{-30'000'000, -64, -30'000'000};
    Position    max{30'000'000, 320, 30'000'000};
    bool        allowSummon = false;
};

struct ExplosionRule {
    std::string id          = "spawn_protection";
    bool        enabled     = true;
    int         priority    = 100;
    int         dimensionId = 0;
    Position    min{-8000, -64, -8000};
    Position    max{8000, 320, 8000};
    bool        disableBlockBreaking = true;
    bool        disableFire          = true;
    bool        disableEntityDamage  = false;
};

struct BlockMatcher {
    std::string type  = "exact";
    std::string value = "minecraft:diamond_ore";
};

struct LootConditions {
    bool requirePlayerMining = true;
    bool allowSilkTouch      = false;
};

struct LootDrop {
    std::string item           = "sks:shulker_coin";
    int         chancePermille = 230;
    int         minCount       = 1;
    int         maxCount       = 1;
};

struct LootRule {
    std::string               id      = "ore_bonus";
    bool                      enabled = true;
    std::vector<BlockMatcher> blockMatchers{
        {"regex", "^minecraft:.*_ore$"}
    };
    LootConditions        conditions{};
    std::string           selection = "one";
    std::vector<LootDrop> drops{
        {"sks:shulker_coin",         230, 1, 1},
        {"sks:diamond_shulker_coin", 19,  1, 1}
    };
};

struct Settings {
    int                        version = 1;
    FeatureSettings            features{};
    PortalMobPolicy            portalMobPolicy{};
    std::vector<WitherRule>    witherRules{{}};
    std::vector<ExplosionRule> explosionRules{{}};
    std::vector<LootRule>      lootRules{{}};
};

extern Settings gConfig;

bool load(ll::mod::Mod& mod);

} // namespace lk::config
