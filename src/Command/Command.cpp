#include "Command/Command.h"

#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>

#include "Entry/Entry.h"

namespace lk::command {

void registerReloadCommand() {
    auto& command = ll::command::CommandRegistrar::getInstance(false).getOrCreateCommand(
        "stweaks",
        "Manage STweaks.",
        CommandPermissionLevel::Admin
    );

    command.overload().text("reload").execute([](CommandOrigin const&, CommandOutput& output) {
        if (!Entry::getInstance().reloadConfig()) {
            output.error("Could not reload STweaks configuration; the previous configuration remains active.");
            return;
        }

        output.success("STweaks configuration reloaded.");
    });
}

} // namespace lk::command
