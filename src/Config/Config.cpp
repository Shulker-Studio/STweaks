#include "Config/Config.h"

#include <exception>
#include <filesystem>
#include <utility>

#include "ll/api/Config.h"
#include "ll/api/mod/Mod.h"

namespace lk::config {

Settings gConfig;

bool load(ll::mod::Mod& mod) {
    Settings loadedConfig;

    auto const path = mod.getConfigDir() / "config.json";
    try {
        std::filesystem::create_directories(path.parent_path());
        if (!ll::config::loadConfig(loadedConfig, path)) {
            mod.getLogger().info("Writing configuration defaults or upgrades to {}.", path.string());
            if (!ll::config::saveConfig(loadedConfig, path)) {
                mod.getLogger().error("Could not write configuration to {}.", path.string());
                return false;
            }
        }
    } catch (std::exception const& error) {
        mod.getLogger().error("Could not load configuration from {}: {}", path.string(), error.what());
        return false;
    }

    gConfig = std::move(loadedConfig);
    return true;
}
} // namespace lk::config
