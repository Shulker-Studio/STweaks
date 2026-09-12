add_rules("mode.debug", "mode.release")

add_repositories("levimc-repo https://github.com/LiteLDev/xmake-repo.git")

option("target_type")
    set_default("server")
    set_showmenu(true)
    set_values("server", "client")
option_end()

-- add_requires("levilamina x.x.x") for a specific version
-- add_requires("levilamina develop") to use develop version
-- please note that you should add bdslibrary yourself if using dev version
add_requires("levilamina 26.32.*", {configs = {target_type = get_config("target_type")}})

add_requires("levibuildscript")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("STweaks") -- Change this to your mod name.
    add_rules("@levibuildscript/linkrule")
    if is_plat("windows") then
        add_defines("NOMINMAX", "UNICODE")
        set_exceptions("none") -- To avoid conflicts with /EHa.
        add_cxflags( "/EHa", "/utf-8", "/W4", "/w44265", "/w44289", "/w44296", "/w45263", "/w44738", "/w45204")
        add_cxflags(
            "/EHs",
            "-Wno-microsoft-cast",
            "-Wno-invalid-offsetof",
            "-Wno-c++2b-extensions",
            "-Wno-microsoft-include",
            "-Wno-overloaded-virtual",
            "-Wno-ignored-qualifiers",
            "-Wno-missing-field-initializers",
            "-Wno-potentially-evaluated-expression",
            "-Wno-pragma-system-header-outside-header",
            {tools = {"clang_cl"}}
        )
        set_toolchains("clang-cl")
    end
    add_packages("levilamina")
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")
    add_headerfiles("src/**.h")
    add_files("src/**.cpp")
    add_files("src/**.rc")
    set_configdir("$(builddir)/config")
    add_configfiles("src/(Entry/Version.h.in)")
    add_includedirs("src", "$(builddir)/config")
    if is_config("target_type", "server") then
    --  add_includedirs("src-server")
    --  add_files("src-server/**.cpp")
    else
    --  add_includedirs("src-client")
    --  add_files("src-client/**.cpp")
    end

    on_load(function (target)
        import("core.base.json")

        local tooth = json.loadfile("tooth.json")
        local modVersion = tooth and tooth["version"]
        if type(modVersion) ~= "string" or not modVersion:match("^%d+%.%d+%.%d+$") then
            raise("tooth.json version must use major.minor.patch format")
        end

        local major, minor, patch = modVersion:match("^(%d+)%.(%d+)%.(%d+)$")
        local buildMetadata = os.iorun("git rev-parse --short HEAD"):gsub("\n", "")
        local versionString = modVersion .. "+" .. buildMetadata
        target:set("version", modVersion)
        target:set("configvar", "STWEAKS_VERSION_MAJOR", major)
        target:set("configvar", "STWEAKS_VERSION_MINOR", minor)
        target:set("configvar", "STWEAKS_VERSION_PATCH", patch)
        target:set("configvar", "STWEAKS_VERSION_STRING", versionString)
        target:add("rules", "@levibuildscript/modpacker", {modVersion = versionString})
    end)
