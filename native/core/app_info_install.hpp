// Native model of the two recovered writers that populate the application-info registry list.
//
// 1. `0x25a4d0(owner, registry)` (1014 B, E562) installs FOUR entries, in this order, all with
//    flag = 1:
//      "Build Version"             -> the shared build-version string
//      "Platform Type"             -> "ps4 - NEO" / "ps4 - STANDARD" selected by `0x3f4e0`
//      "Is Debug Memory Available" -> "True" / "False" from `0x120c0` (always 0 in this build)
//      "Camera Location"           -> formatted "setCameraPos ..." text, only when the owner's
//                                     camera handle at `owner+0x2668` is non-null
//    `0x1bdb50` pushes at the front, so the list reads back newest-first:
//    Camera Location, Is Debug Memory Available, Platform Type, Build Version.
//
// 2. `0xb10(registry)` (139 B) inserts the game entry:
//      "Game" -> "Shadow of the Colossus"
//    `0xb10` is registered as a list-C delegate by `0x1290` and is dispatched before `0x25a4d0`,
//    so the observed insert order over a whole run is Game, Build Version, Platform Type,
//    Is Debug Memory Available, Camera Location.
//
// The holder is a 0x20 B refcounted string descriptor {data,size,capacity,control}; the inserter
// shares the buffer and takes one reference. This native model deliberately uses BORROWED views
// (capacity 0, control null) for every value: the entry then owns nothing, the destructor's
// `0xf590` calls are no-ops, and the caller must keep each `string_view` alive for the registry's
// lifetime. That is a stated contract, not a claim that the original never owns formatted values.
//
// NOT modelled: `0x3f4e0` (IsNeo), `0x120c0`, the camera handle chain
// (`0x1c70b0`/`0x1c0f40`/`0x216480`/`0x50ba0`), the `0x10a90` formatter, and the owning String
// implementation behind `*(0x2dfcf40+0x60)`. Callers supply the resulting text. The entry node,
// list linkage, counts and flags are the lifted `0x1bdb50` body. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "app_info_registry.hpp"

namespace sotc {

inline constexpr std::uint64_t kAppInfoInstallerRva = 0x25A4D0;
inline constexpr std::size_t kAppInfoInstallerSize = 1014;
inline constexpr std::uint64_t kAppInfoInstallerEndRva = 0x25A8C6;
inline constexpr std::uint64_t kAppInfoGameWriterRva = 0xB10;
inline constexpr std::size_t kAppInfoGameWriterSize = 139;
inline constexpr std::uint32_t kAppInfoEntryFlag = 1;

inline constexpr std::string_view kAppInfoGameName = "Game";
inline constexpr std::string_view kAppInfoBuildVersionName = "Build Version";
inline constexpr std::string_view kAppInfoPlatformName = "Platform Type";
inline constexpr std::string_view kAppInfoDebugMemoryName = "Is Debug Memory Available";
inline constexpr std::string_view kAppInfoCameraName = "Camera Location";
inline constexpr std::string_view kAppInfoPlatformNeo = "ps4 - NEO";
inline constexpr std::string_view kAppInfoPlatformStandard = "ps4 - STANDARD";
inline constexpr std::string_view kAppInfoTrue = "True";
inline constexpr std::string_view kAppInfoFalse = "False";

// Values the installer inserts. `camera_location` is read only when `camera_available` is true and
// must already contain the formatted pose text; the native model does not run the camera chain.
struct AppInfoProductValues {
    std::string_view build_version;
    bool neo_mode = false;
    bool debug_memory_available = false;
    bool camera_available = false;
    std::string_view camera_location;
};

struct AppInfoInstallHooks {
    Insert1bdb50Hooks insert{};
};

struct AppInfoInstallResult {
    bool ok = false;
    int inserted = 0;
    int failed_site = 0;            // 1..4, 0 when nothing failed
    std::string_view platform_value;  // selected constant, valid for the process lifetime
};

// Runs the four `0x25a4d0` insertions in their recovered order. Stops at the first visible insert
// failure (missing allocator or allocation failure) and reports where; entries already inserted
// stay in the list, exactly like the original's unguarded sequence would leave them.
[[nodiscard]] AppInfoInstallResult install_app_info_product_entries(
    AppInfoRegistry& registry, const AppInfoProductValues& values,
    const AppInfoInstallHooks& hooks) noexcept;

// Runs the single `0xb10` insertion. `game_name` is host-supplied so the native app does not bake
// the original product string into the binary; the recovered name/value pair is ("Game", the
// supplied text).
[[nodiscard]] Insert1bdb50Result install_app_info_game_entry(
    AppInfoRegistry& registry, std::string_view game_name,
    const Insert1bdb50Hooks& hooks) noexcept;

// Reusable native list-3 delegate payloads. The original registers the raw function pointers
// 0x25a4d0 and 0xb10; the native owner registers these adapters instead, so the dispatcher never
// calls an original RVA. Each adapter keeps the last result and call count for assertions.
struct AppInfoProductDelegate {
    AppInfoProductValues values{};
    Insert1bdb50Hooks insert{};
    AppInfoInstallResult last{};
    int calls = 0;
};

struct AppInfoGameDelegate {
    std::string_view game_name;
    Insert1bdb50Hooks insert{};
    Insert1bdb50Result last{};
    int calls = 0;
};

void install_app_info_product_entry(void* ctx, AppInfoRegistry& registry) noexcept;
void install_app_info_game_entry_delegate(void* ctx, AppInfoRegistry& registry) noexcept;

}  // namespace sotc
