# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [26.32.0] - 2026-09-13

### Changed

- Updated the LeviLamina dependency to 26.32.

## [26.10.0] - 2026-07-27

### Added

- Added `config.json` for feature switches and gameplay rules.
- Added prioritized AABB rules for explosion protection and wither summoning.
- Added portal mob allowlist and denylist policies.
- Added configurable bonus loot rules with exact and regular-expression block matchers, mining conditions, probabilities, and random stack counts.
- Added the `/stweaks reload` administrator command to reload configuration and runtime rules without reloading the plugin.

### Fixed

- Prevented bundle movement through hoppers and droppers.
- Prevented silverfish spawning from infested blocks when enabled.
- Protected chest opening and hopper pulls when the related chunk is not ready, preventing chunk-copy item duplication.
- Correctly interpret successful PreLoader hook registration results.

### Changed

- Embedded Windows DLL file and product version metadata from `tooth.json` and the current Git revision.
