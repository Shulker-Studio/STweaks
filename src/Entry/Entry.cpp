#include "Entry/Entry.h"

#include "Command/Command.h"
#include "Config/Config.h"
#include "Feature/Features.h"
#include "Rule/Rules.h"
#include "ll/api/mod/RegisterHelper.h"

namespace lk {

Entry& Entry::getInstance() {
    static Entry instance;
    return instance;
}

bool Entry::load() {
    getSelf().getLogger().debug("Loading...");
    return reloadConfig();
}

bool Entry::reloadConfig() {
    if (!config::load(getSelf())) return false;

    rules::initialize(config::gConfig, getSelf().getLogger());
    getSelf().getLogger().info("STweaks configuration reloaded.");
    return true;
}

bool Entry::enable() {
    getSelf().getLogger().debug("Enabling...");
    if (!features::enableAll()) return false;

    command::registerReloadCommand();
    return true;
}

bool Entry::disable() {
    getSelf().getLogger().debug("Disabling...");
    features::disableAll();
    return true;
}

} // namespace lk

LL_REGISTER_MOD(lk::Entry, lk::Entry::getInstance());
