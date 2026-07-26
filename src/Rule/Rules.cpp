#include "Rule/Rules.h"

#include <algorithm>
#include <regex>
#include <utility>

#include "Config/Config.h"
#include "ll/api/io/Logger.h"
#include "mc/deps/core/math/IRandom.h"
#include "mc/deps/core/math/Vec3.h"
#include "mc/world/item/ItemStack.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/block/ResourceDrops.h"
#include "mc/world/level/block/ResourceDropsContext.h"

namespace lk::rules {
namespace {

struct CompiledMatcher {
    std::string exact;
    std::regex  regex;
    bool        isRegex = false;
};

struct CompiledLootRule {
    config::LootRule             rule;
    std::vector<CompiledMatcher> matchers;
};

struct RuntimeRules {
    config::PortalMobPolicy            portalMobPolicy;
    bool                               hasPortalMobPolicy = false;
    std::vector<config::WitherRule>    witherRules;
    std::vector<config::ExplosionRule> explosionRules;
    std::vector<CompiledLootRule>      lootRules;
};

RuntimeRules gRules;

bool isValidBox(config::Position const& min, config::Position const& max) {
    return min.x <= max.x && min.y <= max.y && min.z <= max.z;
}

bool contains(config::Position const& min, config::Position const& max, float x, float y, float z) {
    return x >= min.x && x <= max.x && y >= min.y && y <= max.y && z >= min.z && z <= max.z;
}

template <class Rule>
void sortByPriority(std::vector<Rule>& rules) {
    std::stable_sort(rules.begin(), rules.end(), [](Rule const& left, Rule const& right) {
        return left.priority > right.priority;
    });
}

bool matches(CompiledMatcher const& matcher, std::string_view blockType) {
    if (!matcher.isRegex) return matcher.exact == blockType;

    return std::regex_match(blockType.begin(), blockType.end(), matcher.regex);
}

bool matchesAny(CompiledLootRule const& rule, std::string_view blockType) {
    return std::ranges::any_of(rule.matchers, [&blockType](CompiledMatcher const& matcher) {
        return matches(matcher, blockType);
    });
}

bool matchesConditions(config::LootConditions const& conditions, ResourceDropsContext const& context) {
    if (conditions.requirePlayerMining && context.mCause != ResourceDropsCause::PlayerMining) return false;
    return conditions.allowSilkTouch || !context.isUsingSilkTouch();
}

void addDrop(ResourceDrops& resourceDrops, config::LootDrop const& drop, IRandom& random) {
    int count = drop.minCount;
    if (drop.minCount != drop.maxCount) count += random.nextInt(drop.maxCount - drop.minCount + 1);

    ItemStack item;
    item.reinit(drop.item, count, 0);
    if (!item.isNull()) resourceDrops.mItems->emplace_back(std::move(item));
}

} // namespace

void initialize(config::Settings const& settings, ll::io::Logger& logger) {
    gRules = {};

    if (settings.portalMobPolicy.enabled
        && (settings.portalMobPolicy.mode == "allowlist" || settings.portalMobPolicy.mode == "denylist")) {
        gRules.portalMobPolicy    = settings.portalMobPolicy;
        gRules.hasPortalMobPolicy = !gRules.portalMobPolicy.entityTypes.empty();
    } else if (settings.portalMobPolicy.enabled) {
        logger.warn("Ignoring portal mob policy: mode must be 'allowlist' or 'denylist'.");
    }

    for (auto const& rule : settings.witherRules) {
        if (!rule.enabled) continue;
        if (!isValidBox(rule.min, rule.max)) {
            logger.warn("Ignoring wither rule '{}': min must not exceed max.", rule.id);
            continue;
        }
        gRules.witherRules.emplace_back(rule);
    }
    sortByPriority(gRules.witherRules);

    for (auto const& rule : settings.explosionRules) {
        if (!rule.enabled) continue;
        if (!isValidBox(rule.min, rule.max)) {
            logger.warn("Ignoring explosion rule '{}': min must not exceed max.", rule.id);
            continue;
        }
        gRules.explosionRules.emplace_back(rule);
    }
    sortByPriority(gRules.explosionRules);

    for (auto const& rule : settings.lootRules) {
        if (!rule.enabled) continue;
        if (rule.selection != "one" && rule.selection != "independent") {
            logger.warn("Ignoring loot rule '{}': selection must be 'one' or 'independent'.", rule.id);
            continue;
        }

        CompiledLootRule compiled{.rule = rule};
        bool             valid = true;
        for (auto const& matcher : rule.blockMatchers) {
            if (matcher.value.empty() || matcher.value.size() > 256) {
                logger.warn("Ignoring loot rule '{}': matcher values must contain 1 to 256 characters.", rule.id);
                valid = false;
                break;
            }

            if (matcher.type == "exact") {
                compiled.matchers.emplace_back(CompiledMatcher{.exact = matcher.value});
            } else if (matcher.type == "regex") {
                try {
                    compiled.matchers.emplace_back(
                        CompiledMatcher{
                            .regex   = std::regex{matcher.value, std::regex_constants::optimize},
                            .isRegex = true
                    }
                    );
                } catch (std::regex_error const& error) {
                    logger
                        .warn("Ignoring loot rule '{}': invalid regex '{}': {}", rule.id, matcher.value, error.what());
                    valid = false;
                    break;
                }
            } else {
                logger.warn("Ignoring loot rule '{}': unknown matcher type '{}'.", rule.id, matcher.type);
                valid = false;
                break;
            }
        }

        int totalChance = 0;
        for (auto const& drop : rule.drops) {
            if (drop.item.empty() || drop.chancePermille < 0 || drop.chancePermille > 1000 || drop.minCount < 1
                || drop.maxCount < drop.minCount || drop.maxCount > 64) {
                logger.warn("Ignoring loot rule '{}': one or more drops are invalid.", rule.id);
                valid = false;
                break;
            }
            totalChance += drop.chancePermille;
        }

        if (rule.selection == "one" && totalChance > 1000) {
            logger.warn("Ignoring loot rule '{}': one-selection chances exceed 1000 permille.", rule.id);
            valid = false;
        }

        if (!valid || compiled.matchers.empty() || rule.drops.empty()) {
            if (valid) logger.warn("Ignoring loot rule '{}': matchers and drops must not be empty.", rule.id);
            continue;
        }
        gRules.lootRules.emplace_back(std::move(compiled));
    }

    logger.info(
        "Loaded rules: {} explosion rules, {} wither rules, {} loot rules.",
        gRules.explosionRules.size(),
        gRules.witherRules.size(),
        gRules.lootRules.size()
    );
}

bool isMobPortalAllowed(std::string_view entityType) {
    if (!gRules.hasPortalMobPolicy) return true;

    const bool listed =
        std::ranges::find(gRules.portalMobPolicy.entityTypes, entityType) != gRules.portalMobPolicy.entityTypes.end();
    return gRules.portalMobPolicy.mode == "allowlist" ? listed : !listed;
}

bool isWitherSummonAllowed(int dimensionId, BlockPos const& pos) {
    for (auto const& rule : gRules.witherRules) {
        if (rule.dimensionId == dimensionId
            && contains(
                rule.min,
                rule.max,
                static_cast<float>(pos.x),
                static_cast<float>(pos.y),
                static_cast<float>(pos.z)
            )) {
            return rule.allowSummon;
        }
    }
    return true;
}

void applyExplosionRules(int dimensionId, Vec3 const& pos, bool& breaking, bool& fire, float& damageScaling) {
    for (auto const& rule : gRules.explosionRules) {
        if (rule.dimensionId != dimensionId || !contains(rule.min, rule.max, pos.x, pos.y, pos.z)) continue;

        if (rule.disableBlockBreaking) breaking = false;
        if (rule.disableFire) fire = false;
        if (rule.disableEntityDamage) damageScaling = 0.0f;
        return;
    }
}

void appendLootDrops(
    ResourceDrops&              resourceDrops,
    Block const&                block,
    IRandom&                    random,
    ResourceDropsContext const& context
) {
    const auto& blockType = block.getTypeName();
    for (auto const& rule : gRules.lootRules) {
        if (!matchesConditions(rule.rule.conditions, context) || !matchesAny(rule, blockType)) continue;

        if (rule.rule.selection == "one") {
            const int roll       = random.nextInt(1000);
            int       upperBound = 0;
            for (auto const& drop : rule.rule.drops) {
                upperBound += drop.chancePermille;
                if (roll < upperBound) {
                    addDrop(resourceDrops, drop, random);
                    break;
                }
            }
        } else {
            for (auto const& drop : rule.rule.drops) {
                if (random.nextInt(1000) < drop.chancePermille) addDrop(resourceDrops, drop, random);
            }
        }
    }
}

} // namespace lk::rules
