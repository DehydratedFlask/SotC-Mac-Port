// Native 0xa5d990 body: the SoundSystemSndx category console-variable + menu cluster.
//
// Original: 0xa5d990..0xa5e425, 2,709 B (0xa95). 500 instructions, 44 direct calls and one
// cold stack-check tail (`0xf8f3a0`, pinned and never called).
//
// The body is three regions, in order:
//   1. 0xa5da20..0xa5ddde — the 32-iteration loop that appends one `sndxCategory_Mute_%d` and one
//      `sndxCategory_Filter_%d` bool variable to the engine's two 0xc8-stride vectors
//      (engine+0x8bd0 / engine+0x72a8), registering each through 0x20d60, and — when
//      `category_names[i]` is non-null and non-empty — building the two per-index menu toggles.
//      This region is already native as `build_sndx_category_vars` (engine_category_vars.*), whose
//      byte facts and menu-toggle hook are established there; the model reuses it rather than
//      duplicating a second loop.
//   2. 0xa5ddec..0xa5e058 — the "Unnamed Categories" group block. It is gated by
//      `testb $0x1,-0x731(%rbp)`, and that byte is written only at the loop head (0xa5da20) from
//      `eax`, which is zero at that point (`xorl %eax,%eax` at 0xa5da17). The block is therefore
//      dead in the measured body; the model records `dead_unnamed_block` and never runs it.
//   3. 0xa5e05d..0xa5e3fc — a static menu cluster: two separators (0x26b60) and four console
//      command items (0x26f90) under the parent paths "Category Mute" and "Category Filters", all
//      with flags 0x30, all targeted at the opaque engine pointer 0x2dfcbd0, each registered
//      through 0x268b0 and then destroyed by a direct 0x272d0 call. The constructors and the
//      registration helper are the existing native models in fmenu_ctors_slot306.*; the direct
//      destructor call is a caller-owned hook here (0x272d0 is pinned, never executed).
//
// Nothing in this file executes or dereferences an original RVA. The 0x2dfcbd0 target is stored
// as an identity tag and never read. G1 PARTIAL: no boot, audio, save or Metal gate is claimed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "engine_category_vars.hpp"
#include "hashed_string_14c3a0.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kInitA5d990BodyRva = 0xA5D990;
inline constexpr std::size_t kInitA5d990BodySize = 0xA95;   // 2,709 B
inline constexpr std::uint64_t kInitA5d990BodyEndRva = 0xA5E425;

// The opaque pointer every static command item targets (0x2dfcbd0). Identity tag only.
inline constexpr std::uint64_t kInitA5d990MenuTargetRva = 0x2DFCBD0;
// The dead "Unnamed Categories" gate byte at rbp-0x731 (never set nonzero).
inline constexpr std::uint32_t kInitA5d990UnnamedGateMask = 0x1;

// Static cluster strings, pinned from the body's leaq operands.
inline constexpr std::uint64_t kInitA5d990MuteParentRva = 0x12C4D2F;      // "Category Mute"
inline constexpr std::size_t kInitA5d990MuteParentLen = 0xD;
inline constexpr std::uint64_t kInitA5d990FilterParentRva = 0x12C4DDD;    // "Category Filters"
inline constexpr std::size_t kInitA5d990FilterParentLen = 0x10;
inline constexpr std::uint64_t kInitA5d990MuteAllRva = 0x12C504D;         // "Mute All"
inline constexpr std::size_t kInitA5d990MuteAllLen = 0x8;
inline constexpr std::uint64_t kInitA5d990MuteAllCmdRva = 0x12C5056;      // "sndx_categoryMuteAll=true"
inline constexpr std::size_t kInitA5d990MuteAllCmdLen = 0x19;
inline constexpr std::uint64_t kInitA5d990UnmuteAllRva = 0x12C5070;       // "Unmute All"
inline constexpr std::size_t kInitA5d990UnmuteAllLen = 0xA;
inline constexpr std::uint64_t kInitA5d990UnmuteAllCmdRva = 0x12C507B;    // "sndx_categoryMuteAll=false"
inline constexpr std::size_t kInitA5d990UnmuteAllCmdLen = 0x1A;
inline constexpr std::uint64_t kInitA5d990FilterAllRva = 0x12C5096;       // "Filter All"
inline constexpr std::size_t kInitA5d990FilterAllLen = 0xA;
inline constexpr std::uint64_t kInitA5d990FilterAllCmdRva = 0x12C50A1;    // "sndx_categoryFilterAll=true"
inline constexpr std::size_t kInitA5d990FilterAllCmdLen = 0x1B;
inline constexpr std::uint64_t kInitA5d990UnfilterAllRva = 0x12C50BD;     // "Unfilter All"
inline constexpr std::size_t kInitA5d990UnfilterAllLen = 0xC;
inline constexpr std::uint64_t kInitA5d990UnfilterAllCmdRva = 0x12C50CA;  // "sndx_categoryFilterAll=false"
inline constexpr std::size_t kInitA5d990UnfilterAllCmdLen = 0x1C;
// The dead block's per-index caption format ("Category %d").
inline constexpr std::uint64_t kInitA5d990UnnamedCaptionRva = 0x12C5041;
inline constexpr std::size_t kInitA5d990UnnamedCaptionLen = 0xB;
inline constexpr std::uint64_t kInitA5d990UnnamedGroupRva = 0x12C501A;    // "Unnamed Categories"

inline constexpr std::size_t kInitA5d990StaticItems = 6;   // 2 separators + 4 command items
inline constexpr std::uint32_t kInitA5d990ItemFlags = 0x30;  // every static-cluster item

struct InitA5d990Hooks {
    // Opaque engine object (the same 0xa500 B SoundSystemSndx object 0xa5cd90 receives).
    void* engine = nullptr;
    // The caller's category-name table (32 entries). May be null: no per-index toggles.
    const char* const* category_names = nullptr;
    ConCmdRegistry* registry = nullptr;     // 0x20d60 registration for the 64 bool vars
    SndxCategoryHooks* categories = nullptr;
    DebugMenuRegistry* menu = nullptr;      // 0x268b0 registration for the static cluster
    StartupAtexitLog* atexit = nullptr;
    SharedStrCtorHooks separator_alloc{};   // the only allocating ctor (0x26b60 "MenuSeparator")
    // Direct 0x272d0 destruction of the six stack items. The model never executes 0x272d0.
    bool (*destroy_item)(void* host, DebugMenuItem& item) noexcept = nullptr;
    void* host = nullptr;
};

struct InitA5d990Result {
    bool ok = false;
    const char* blocked = nullptr;
    SndxCategoriesStage stage = SndxCategoriesStage::ok;
    std::size_t mute_built = 0;
    std::size_t filter_built = 0;
    std::size_t toggle_items = 0;
    int separators = 0;
    int commands = 0;
    int registered = 0;
    int destroys = 0;
    bool dead_unnamed_block = true;   // region 2 is unreachable; never executed
};

[[nodiscard]] InitA5d990Result run_init_a5d990_body(const InitA5d990Hooks& hooks) noexcept;

}  // namespace sotc
