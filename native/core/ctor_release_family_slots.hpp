// L5-RELEASE-FAMILY: native recovery of seventeen original startup constructor slots that share the
// `0xf590` (shared-str release) + `__cxa_atexit` (`0xf8f390`) shape. ELF sha256 d0723afb...5861d3; the
// constructor-table word of slot i is the u64 at file offset 0x2dda750 + (306 - i) * 8. Byte oracle:
// tools/lift/check_ctor_release_family_slots.py (decodes every body from the ELF and compares the whole
// table below row by row). NOTHING here executes, loads or calls an original RVA; vtable pointers,
// destructor RVAs and string addresses are IDENTITY TAGS only (the destructors are recorded for
// `__cxa_atexit`, never run).
//
// Three shapes (all bodies are straight-line, no branches except the stack-protector epilogue):
//
//  name_object (12 slots, 16 objects): the body builds a 0x20 B stack SharedStr view {text, len, 0, 0}
//      (ctl == nullptr) and stores into the 0x30 B BSS global G
//          G+0x00 = vptr (class identity tag, `.data.rel.ro` vtable + 0x10)
//          G+0x08 = {text, len}  G+0x18 = 0  G+0x20 = 0   (a borrowed SharedStr copy: ctl == nullptr)
//          G+0x28 = 0
//      then `call 0xf590` on the STACK view (control block null => the lifted release is an exact
//      no-op, E574), then `__cxa_atexit(dtor, G, &__dso_handle = 0x1604218)`. 371/405/470/472 build two
//      such objects back to back (the order is: stores, release, atexit, next object).
//  span_hash (434 435 438): the same stack view {text, len, 0, 0} is hashed with `0x161f80` (FNV-1a over
//      the span, seed 0x811c9dc5; the view of slot 435 object 0 is the empty span), `call 0xf590` on the
//      stack view (inert), and the result is stored in a BSS dword.
//  cstr_hash (431 447): `0xa2ddc0(text)` (path-folding FNV-1a, seed 0x811c9dc5, native
//      `fnv_cstr_a2ddc0`) stored in consecutive BSS dwords; slot 431 stores the literal 0 for its first
//      entry (const_zero). No f590 call in these two slots.
//
// Fail-closed contracts (result.ok == false, `blocked` names the step and stage):
//   * the f590 call is made through `ReleaseFamilyHooks::release_view` (null => the lifted
//     `release_inert_view_f590`); a release that is not an inert no-op (a control block present) means
//     the native model would be wrong about the original, so the fragment stops before `atexit`.
//     A last reference with no destroy/free contract is thus never reported as success.
//   * `__cxa_atexit` is recorded in the supplied `StartupAtexitLog`; a failed record stops the slot.
//   * an unknown slot number or a corrupt table range reports `unknown_slot` / `bad_table`.
// There are no unresolved callees: 0xf590, 0xf8f390, 0x161f80 and 0xa2ddc0 are all native. What stays
// outside: the destructor bodies, the vtable contents, and every consumer of the stored objects/hashes.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kReleaseFamilyDsoHandleRva = 0x1604218;
inline constexpr std::uint64_t kReleaseFamilyAtexitRva = 0xf8f390;
inline constexpr std::uint64_t kReleaseFamilyF590Rva = 0xf590;
inline constexpr std::uint64_t kReleaseFamilyFnvSpanRva = 0x161f80;
inline constexpr std::uint64_t kReleaseFamilyFnvCstrRva = 0xa2ddc0;
inline constexpr std::uint32_t kReleaseFamilyFnvBasis = 0x811c9dc5;
inline constexpr std::size_t kReleaseFamilySlotCount = 17;
inline constexpr std::size_t kReleaseFamilyStepCount = 56;
inline constexpr std::size_t kReleaseFamilyObjectCount = 16;   // name_object steps
inline constexpr std::size_t kReleaseFamilyDwordCount = 40;   // hash / const dword steps

enum class ReleaseFamilyKind : std::uint8_t {
    name_object,
    span_hash,
    cstr_hash,
    const_zero,
};

// One construction step (checker-pinned against the ELF bytes).
struct ReleaseFamilyStep {
    ReleaseFamilyKind kind;
    std::uint16_t slot;          // original constructor-table index
    std::uint64_t global_rva;    // original BSS address (object start, or the destination dword)
    std::uint64_t aux_rva;       // name_object: vptr identity tag (vtable + 0x10); otherwise 0
    const char* text;            // name / hashed string ("" for const_zero)
    std::int64_t length;         // view length (== strlen(text))
    std::uint64_t dtor_rva;      // name_object: destructor recorded for __cxa_atexit (never run); else 0
    std::uint32_t seed;          // hash steps: FNV seed (0x811c9dc5); otherwise 0
    std::uint64_t text_rva;      // original .rodata address of the string (identity only)
};

// Per-slot extent into kReleaseFamilySteps.
struct ReleaseFamilySlot {
    std::uint16_t slot;
    std::uint64_t rva;           // body start (constructor-table word)
    std::uint32_t size;          // body size in bytes
    std::uint32_t first;         // first step index
    std::uint32_t count;         // step count
};

inline constexpr std::array<ReleaseFamilySlot, kReleaseFamilySlotCount> kReleaseFamilySlots{{
    {371, 0x70cdc0, 285, 0, 2},
    {372, 0x70a040, 165, 2, 1},
    {405, 0x5e7790, 309, 3, 2},
    {431, 0x523a00, 416, 5, 27},
    {433, 0x517850, 178, 32, 1},
    {434, 0x511c00, 174, 33, 2},
    {435, 0x510210, 180, 35, 2},
    {438, 0x4fbd80, 113, 37, 1},
    {447, 0x4c3410, 151, 38, 8},
    {470, 0x389a00, 311, 46, 2},
    {472, 0x3583c0, 289, 48, 2},
    {481, 0x2fd8f0, 178, 50, 1},
    {483, 0x2ef810, 178, 51, 1},
    {484, 0x2eee10, 178, 52, 1},
    {485, 0x2ee5e0, 178, 53, 1},
    {487, 0x2cd060, 178, 54, 1},
    {529, 0x14c030, 178, 55, 1},
}};

inline constexpr std::array<ReleaseFamilyStep, kReleaseFamilyStepCount> kReleaseFamilySteps{{
    {ReleaseFamilyKind::name_object, 371, 0x3245460, 0x159bb30, "r_WantsHDR_Game", 15, 0x4cba00, 0x0, 0x1279784},
    {ReleaseFamilyKind::name_object, 371, 0x3245490, 0x159bab8, "r_Wants4K_Game", 14, 0x4cb9e0, 0x0, 0x1279794},
    {ReleaseFamilyKind::name_object, 372, 0x3245420, 0x15976b0, "shadowDemoMode", 14, 0x8ef00, 0x0, 0x1279650},
    {ReleaseFamilyKind::name_object, 405, 0x30059b0, 0x159e1a0, "r_hardwareGamma", 15, 0x2fced0, 0x0, 0x12699a0},
    {ReleaseFamilyKind::name_object, 405, 0x30059e0, 0x159e1a0, "r_hardwareGamma_HDR", 19, 0x2fced0, 0x0, 0x12699b0},
    {ReleaseFamilyKind::const_zero, 431, 0x2efc970, 0x0, "", 0, 0x0, 0x0, 0x0},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc974, 0x0, "UI_LANGUAGE_ARABIC", 18, 0x0, 0x811c9dc5, 0x1012840},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc978, 0x0, "UI_LANGUAGE_CHINESE", 19, 0x0, 0x811c9dc5, 0x1012853},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc97c, 0x0, "UI_LANGUAGE_SIMPLIFIED_CHINESE", 30, 0x0, 0x811c9dc5, 0x1012867},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc980, 0x0, "UI_LANGUAGE_CZECH", 17, 0x0, 0x811c9dc5, 0x1012886},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc984, 0x0, "UI_LANGUAGE_DANISH", 18, 0x0, 0x811c9dc5, 0x1012898},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc988, 0x0, "UI_LANGUAGE_DUTCH", 17, 0x0, 0x811c9dc5, 0x10128ab},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc98c, 0x0, "UI_LANGUAGE_ENGLISH", 19, 0x0, 0x811c9dc5, 0x10128bd},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc990, 0x0, "UI_LANGUAGE_ENGLISH_UK", 22, 0x0, 0x811c9dc5, 0x10128d1},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc994, 0x0, "UI_LANGUAGE_FINNISH", 19, 0x0, 0x811c9dc5, 0x10128e8},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc998, 0x0, "UI_LANGUAGE_FRENCH", 18, 0x0, 0x811c9dc5, 0x10128fc},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc99c, 0x0, "UI_LANGUAGE_FRENCH_CANADIAN", 27, 0x0, 0x811c9dc5, 0x101290f},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9a0, 0x0, "UI_LANGUAGE_GERMAN", 18, 0x0, 0x811c9dc5, 0x101292b},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9a4, 0x0, "UI_LANGUAGE_GREEK", 17, 0x0, 0x811c9dc5, 0x101293e},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9a8, 0x0, "UI_LANGUAGE_ITALIAN", 19, 0x0, 0x811c9dc5, 0x1012950},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9ac, 0x0, "UI_LANGUAGE_JAPANESE", 20, 0x0, 0x811c9dc5, 0x1012964},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9b0, 0x0, "UI_LANGUAGE_KOREAN", 18, 0x0, 0x811c9dc5, 0x1012979},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9b4, 0x0, "UI_LANGUAGE_NORWEGIAN", 21, 0x0, 0x811c9dc5, 0x101298c},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9b8, 0x0, "UI_LANGUAGE_POLISH", 18, 0x0, 0x811c9dc5, 0x10129a2},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9bc, 0x0, "UI_LANGUAGE_PORTUGUESE", 22, 0x0, 0x811c9dc5, 0x10129b5},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9c0, 0x0, "UI_LANGUAGE_BRAZILIAN_PORTUGUESE", 32, 0x0, 0x811c9dc5, 0x10129cc},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9c4, 0x0, "UI_LANGUAGE_RUSSIAN", 19, 0x0, 0x811c9dc5, 0x10129ed},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9c8, 0x0, "UI_LANGUAGE_SPANISH", 19, 0x0, 0x811c9dc5, 0x1012a01},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9cc, 0x0, "UI_LANGUAGE_LATIN_AMERICAN_SPANISH", 34, 0x0, 0x811c9dc5, 0x1012a15},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9d0, 0x0, "UI_LANGUAGE_SWEDISH", 19, 0x0, 0x811c9dc5, 0x1012a38},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9d4, 0x0, "UI_LANGUAGE_TURKISH", 19, 0x0, 0x811c9dc5, 0x1012a4c},
    {ReleaseFamilyKind::cstr_hash, 431, 0x2efc9d8, 0x0, "UI_LANGUAGE_THAI", 16, 0x0, 0x811c9dc5, 0x1012a60},
    {ReleaseFamilyKind::name_object, 433, 0x2efc900, 0x15976b0, "shadowDemoMode", 14, 0x8ef00, 0x0, 0x1010670},
    {ReleaseFamilyKind::span_hash, 434, 0x2efc8f8, 0x0, "hdr_curWhiteLevel", 17, 0x0, 0x811c9dc5, 0x100fcc0},
    {ReleaseFamilyKind::span_hash, 434, 0x2efc8fc, 0x0, "hdr_maxWhiteLevel", 17, 0x0, 0x811c9dc5, 0x100fcd2},
    {ReleaseFamilyKind::span_hash, 435, 0x2efc8c8, 0x0, "", 0, 0x0, 0x811c9dc5, 0x100f8a0},
    {ReleaseFamilyKind::span_hash, 435, 0x2efc8cc, 0x0, "FilmGrain", 9, 0x0, 0x811c9dc5, 0x100f8a1},
    {ReleaseFamilyKind::span_hash, 438, 0x2efc2f0, 0x0, "logo_texture", 12, 0x0, 0x811c9dc5, 0x100d10e},
    {ReleaseFamilyKind::cstr_hash, 447, 0x2efa870, 0x0, "UI_PHOTO_MODE_CAMERA_FOCUS_PLAYER", 33, 0x0, 0x811c9dc5, 0x1002de0},
    {ReleaseFamilyKind::cstr_hash, 447, 0x2efa874, 0x0, "UI_PHOTO_MODE_CAMERA_FOCUS_HORSE", 32, 0x0, 0x811c9dc5, 0x1002e02},
    {ReleaseFamilyKind::cstr_hash, 447, 0x2efa878, 0x0, "UI_PHOTO_MODE_DOF_MODE_USE_GAME", 31, 0x0, 0x811c9dc5, 0x1002e23},
    {ReleaseFamilyKind::cstr_hash, 447, 0x2efa87c, 0x0, "UI_PHOTO_MODE_DOF_MODE_CUSTOM", 29, 0x0, 0x811c9dc5, 0x1002e43},
    {ReleaseFamilyKind::cstr_hash, 447, 0x2efa880, 0x0, "UI_PHOTO_MODE_TINT_LABEL", 24, 0x0, 0x811c9dc5, 0x1002e61},
    {ReleaseFamilyKind::cstr_hash, 447, 0x2efa884, 0x0, "UI_PHOTO_MODE_HIGHLIGHTS_LABEL", 30, 0x0, 0x811c9dc5, 0x1002e7a},
    {ReleaseFamilyKind::cstr_hash, 447, 0x2efa888, 0x0, "UI_PHOTO_MODE_MIDTONES_LABEL", 28, 0x0, 0x811c9dc5, 0x1002e99},
    {ReleaseFamilyKind::cstr_hash, 447, 0x2efa88c, 0x0, "UI_PHOTO_MODE_SHADOW_LABEL", 26, 0x0, 0x811c9dc5, 0x1002eb6},
    {ReleaseFamilyKind::name_object, 470, 0x2ef5530, 0x15ad1d0, "r_fullscreenBlurEnabled", 23, 0x389800, 0x0, 0xfe0ec8},
    {ReleaseFamilyKind::name_object, 470, 0x2ef5560, 0x159e1a0, "r_fullscreenBlurBlendWeight", 27, 0x2fced0, 0x0, 0xfe0ee0},
    {ReleaseFamilyKind::name_object, 472, 0x2ef5470, 0x15976b0, "arealoader_displayZoneInfo", 26, 0x8ef00, 0x0, 0xfdd814},
    {ReleaseFamilyKind::name_object, 472, 0x2ef54a0, 0x15976b0, "arealoader_enableMapLoading", 27, 0x8ef00, 0x0, 0xfdd82f},
    {ReleaseFamilyKind::name_object, 481, 0x2eefdf8, 0x159e1a0, "wwise_masterVolume", 18, 0x2fced0, 0x0, 0xfd5169},
    {ReleaseFamilyKind::name_object, 483, 0x2eefd00, 0x159e1b8, "disableInput", 12, 0x2c84f0, 0x0, 0xfd34f9},
    {ReleaseFamilyKind::name_object, 484, 0x2eefcd0, 0x159e1b8, "disableInput", 12, 0x2c84f0, 0x0, 0xfd333d},
    {ReleaseFamilyKind::name_object, 485, 0x2eefca0, 0x159e1b8, "disableInput", 12, 0x2c84f0, 0x0, 0xfd2e94},
    {ReleaseFamilyKind::name_object, 487, 0x2eefa40, 0x159e1b8, "showPathFinding", 15, 0x2c84f0, 0x0, 0xfce29b},
    {ReleaseFamilyKind::name_object, 529, 0x2ee39f0, 0x15976b0, "res_printAllFileLoads", 21, 0x8ef00, 0x0, 0xfa9e00},
}};

// The 0x30 B global written by the name_object shape.
struct ReleaseFamilyNameObject {
    std::uint64_t vptr = 0;      // +0x00 identity tag
    SharedStr name{};            // +0x08 borrowed view of the literal (ctl == nullptr)
    std::uint64_t tail = 0;      // +0x28 zero
};
static_assert(sizeof(ReleaseFamilyNameObject) == 0x30);

struct ReleaseFamilyAtexitNote {
    std::uint64_t dtor_rva = 0;
    const void* object = nullptr;
};

enum class ReleaseFamilyStage : std::uint8_t {
    none,
    unknown_slot,
    bad_table,
    view_release,       // the f590 call was not an inert no-op (or the hook refused)
    atexit_record,      // __cxa_atexit could not be recorded
};
[[nodiscard]] const char* release_family_stage_name(ReleaseFamilyStage stage) noexcept;

struct ReleaseFamilyResult {
    bool ok = false;                       // every step of the slot built, nothing blocked
    std::uint16_t slot = 0;
    std::uint32_t built_count = 0;         // steps completed before the first failure
    std::uint32_t releases = 0;            // f590 calls issued (all on stack views)
    std::uint32_t atexit_records = 0;
    std::uint32_t blocked_step = 0xffffffffu;   // table step index that stopped the slot
    ReleaseFamilyStage stage = ReleaseFamilyStage::none;
};

struct ReleaseFamilyHooks {
    // `call 0xf590` on the stack view. Null => release_inert_view_f590 (exact no-op, false if a
    // control block were present). Returning false blocks the slot at stage view_release.
    bool (*release_view)(const SharedStr& view) noexcept = nullptr;
};

// All state written by the family, in table order. `objects[k]` is the k-th name_object step and
// `dwords[k]` the k-th hash/const step (see release_family_object_index / release_family_dword_index).
struct ReleaseFamilyState {
    std::array<ReleaseFamilyNameObject, kReleaseFamilyObjectCount> objects{};
    std::array<std::uint32_t, kReleaseFamilyDwordCount> dwords{};
    std::array<bool, kReleaseFamilyStepCount> built{};
    std::array<ReleaseFamilyAtexitNote, kReleaseFamilyObjectCount> atexit_notes{};
    std::array<ReleaseFamilyResult, kReleaseFamilySlotCount> results{};   // per slot, table order
};

// Index of `step` among the name_object / dword steps (ordinals into the two state arrays).
[[nodiscard]] std::size_t release_family_object_index(std::size_t step) noexcept;
[[nodiscard]] std::size_t release_family_dword_index(std::size_t step) noexcept;
// Position of `slot` in kReleaseFamilySlots, or kReleaseFamilySlotCount when it is not in the family.
[[nodiscard]] std::size_t release_family_slot_position(unsigned slot) noexcept;

// Runs the original constructor `slot` (371 372 405 431 433 434 435 438 447 470 472 481 483 484 485
// 487 529) natively. The result is also stored in `state.results`. The original is unconditional, so
// the first failing step stops the slot (`blocked_step`/`stage`); nothing after it is built.
[[nodiscard]] ReleaseFamilyResult init_release_family_slot(unsigned slot, ReleaseFamilyState& state,
                                                           StartupAtexitLog& atexit_log,
                                                           const ReleaseFamilyHooks& hooks) noexcept;

}  // namespace sotc
