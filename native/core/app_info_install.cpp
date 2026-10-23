#include "app_info_install.hpp"

namespace sotc {
namespace {

enum Site : int {
    kSiteBuildVersion = 1,
    kSitePlatform = 2,
    kSiteDebugMemory = 3,
    kSiteCamera = 4,
};

bool insert_or_stop(AppInfoRegistry& registry, AppInfoInstallResult& out,
                    std::string_view name, std::string_view value,
                    const AppInfoInstallHooks& hooks, int site) noexcept {
    const Insert1bdb50Result inserted =
        registry.add_string_entry(name, value, kAppInfoEntryFlag, hooks.insert);
    if (!inserted.ok) {
        out.failed_site = site;
        return false;
    }
    ++out.inserted;
    return true;
}

}  // namespace

AppInfoInstallResult install_app_info_product_entries(
    AppInfoRegistry& registry, const AppInfoProductValues& values,
    const AppInfoInstallHooks& hooks) noexcept {
    AppInfoInstallResult out{};
    out.platform_value =
        values.neo_mode ? kAppInfoPlatformNeo : kAppInfoPlatformStandard;

    if (!insert_or_stop(registry, out, kAppInfoBuildVersionName, values.build_version,
                        hooks, kSiteBuildVersion)) {
        return out;
    }
    if (!insert_or_stop(registry, out, kAppInfoPlatformName, out.platform_value, hooks,
                        kSitePlatform)) {
        return out;
    }
    if (!insert_or_stop(registry, out, kAppInfoDebugMemoryName,
                        values.debug_memory_available ? kAppInfoTrue : kAppInfoFalse, hooks,
                        kSiteDebugMemory)) {
        return out;
    }
    // The camera entry exists only when the owner's +0x2668 handle is non-null. The native caller
    // decides that and supplies the already-formatted text.
    if (values.camera_available) {
        if (!insert_or_stop(registry, out, kAppInfoCameraName, values.camera_location, hooks,
                            kSiteCamera)) {
            return out;
        }
    }

    out.ok = true;
    return out;
}

Insert1bdb50Result install_app_info_game_entry(AppInfoRegistry& registry,
                                               std::string_view game_name,
                                               const Insert1bdb50Hooks& hooks) noexcept {
    return registry.add_string_entry(kAppInfoGameName, game_name, kAppInfoEntryFlag, hooks);
}

void install_app_info_product_entry(void* ctx, AppInfoRegistry& registry) noexcept {
    auto* const source = static_cast<AppInfoProductDelegate*>(ctx);
    ++source->calls;
    source->last = install_app_info_product_entries(
        registry, source->values, AppInfoInstallHooks{source->insert});
}

void install_app_info_game_entry_delegate(void* ctx, AppInfoRegistry& registry) noexcept {
    auto* const source = static_cast<AppInfoGameDelegate*>(ctx);
    ++source->calls;
    source->last = install_app_info_game_entry(registry, source->game_name, source->insert);
}

}  // namespace sotc
