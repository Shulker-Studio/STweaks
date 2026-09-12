// #include <ll/api/memory/Hook.h>
// #include <mc/world/actor/Hopper.h>
// #include <mc/world/actor/player/Player.h>
// #include <mc/world/level/BlockPos.h>
// #include <mc/world/level/BlockSource.h>
// #include <mc/world/level/ChunkPos.h>
// #include <mc/world/level/block/actor/ChestBlockActor.h>

// #include "Config/Config.h"
// #include "Feature/Features.h"
// #include "Feature/HookResult.h"

// namespace lk::features {
// namespace {

// bool shouldBlockOpen(ChestBlockActor const& chest, BlockSource const& region) {
//     if (chest.mLargeChestPaired == nullptr) {
//         return region.hasUntickedNeighborChunk(ChunkPos{chest.mPosition}, 0);
//     }

//     return region.hasUntickedNeighborChunk(ChunkPos{chest.mPosition}, 0)
//         || region.hasUntickedNeighborChunk(ChunkPos{chest.mLargeChestPairedPosition}, 0);
// }

// bool shouldBlockHopperPull(BlockSource& region, Vec3 const& pos) {
//     auto* blockActor = region.getBlockEntity(BlockPos{pos}.above());
//     if (blockActor == nullptr || blockActor->mType != BlockActorType::Chest) return false;

//     auto& chest = *static_cast<ChestBlockActor*>(blockActor);
//     return shouldBlockOpen(chest, region);
// }

// LL_TYPE_INSTANCE_HOOK(
//     StartOpenChestBlockActorHook,
//     ll::memory::HookPriority::Normal,
//     ChestBlockActor,
//     &ChestBlockActor::canOpen,
//     bool,
//     ::BlockSource& region
// ) {
//     if (!config::gConfig.features.fixChunkCopyItem) return origin(region);

//     if (shouldBlockOpen(*this, region)) return false;
//     return origin(region);
// }

// LL_TYPE_INSTANCE_HOOK(
//     ChestOpenByGuardHook,
//     ll::memory::HookPriority::High,
//     ChestBlockActor,
//     &ChestBlockActor::$openBy,
//     void,
//     ::Player& player
// ) {
//     if (config::gConfig.features.fixChunkCopyItem) {
//         auto& region = player.getDimensionBlockSource();
//         if (shouldBlockOpen(*this, region)) return;
//     }

//     origin(player);
// }

// LL_TYPE_INSTANCE_HOOK(
//     HopperTryPullInItemsFromAboveContainerHook,
//     ll::memory::HookPriority::Normal,
//     Hopper,
//     &Hopper::_tryPullInItemsFromAboveContainer,
//     bool,
//     ::BlockSource& region,
//     ::Container&   toContainer,
//     ::Vec3 const&  pos
// ) {
//     if (!config::gConfig.features.fixChunkCopyItem) return origin(region, toContainer, pos);

//     if (shouldBlockHopperPull(region, pos)) return false;
//     return origin(region, toContainer, pos);
// }

// bool gEnabled = false;

// } // namespace

// bool enableFixChunkCopyItem() {
//     if (gEnabled) return true;

//     if (!hookSucceeded(StartOpenChestBlockActorHook::hook())) return false;
//     if (!hookSucceeded(ChestOpenByGuardHook::hook())) {
//         StartOpenChestBlockActorHook::unhook();
//         return false;
//     }
//     if (!hookSucceeded(HopperTryPullInItemsFromAboveContainerHook::hook())) {
//         ChestOpenByGuardHook::unhook();
//         StartOpenChestBlockActorHook::unhook();
//         return false;
//     }
//     gEnabled = true;
//     return true;
// }

// void disableFixChunkCopyItem() {
//     if (!gEnabled) return;
//     HopperTryPullInItemsFromAboveContainerHook::unhook();
//     ChestOpenByGuardHook::unhook();
//     StartOpenChestBlockActorHook::unhook();
//     gEnabled = false;
// }

// } // namespace lk::features
