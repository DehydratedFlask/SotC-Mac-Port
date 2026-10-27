// CPU-01-SLOT306-CATEGORY-VARS-01: native model of the `sndxCategory_*` console-variable block
// that sound-engine init builds (the 0xa5d990 loop inside the 0xa5cd90 engine-init body; private
// listing local/re/evidence/startup-slot-306-handlers-20261026/dis/a5cd90_engine_init.dis).
// NOTHING here executes, loads or calls an original RVA; every original address is an IDENTITY TAG.
//
// Observed contract (STATIC_ANALYSIS_VERIFIED; byte-pinned by tools/lift/check_sndx_category_vars.py):
//   * Signature: 0xa5d990(engine, const char** category_names) — one loop, i = 0..31
//     (`cmpq $0x20`), two ConCmdBoolVar constructions per iteration.
//   * Names: formatted with the variadic bpe::String::Format 0x10a90 (modelled here with snprintf,
//     a native stand-in): "sndxCategory_Mute_%d" (format {0x12c4fe6, len 20}) and
//     "sndxCategory_Filter_%d" ({0x12c4ffb, len 22}) with the loop index. Both vars: value false,
//     default false, flags 1, description the empty literal "" {0x12c4d1a, 0}, functor tag
//     0x15d2060 — the same inline static-bool shape as slot 306 (init_bool_var_vector).
//   * Vectors: Mute at engine+0x8bd0/{+0x08 end}/{+0x10 cap}, Filter at engine+0x72a8/+0x72b0/
//     +0x72b8; element stride 0xc8; a var is constructed at the CURRENT end, then end += 0xc8.
//     When end == cap the original grows through 0xa62bc0 (which also constructs the element);
//     this model splits that into a grow hook (reallocation only, contents preserved) followed
//     by the same in-place construction — identical end state. The growth policy inside
//     0xa62bc0 is NOT lifted; the hook owns it.
//   * Registration: 0x20d60 per var (the shared pending-list register). NO __cxa_atexit record:
//     the vectors own the vars and the engine teardown (0xa5cb30) destroys them.
//   * Conditional menu toggles: when category_names[i] is non-null and its first byte is not 0,
//     the loop formats a caption "%d - %s" ({0x12c5012, len 7}, index + the category name) and
//     builds TWO stack menu toggles through 0x26ea0 (which registers via the 0x268b0 tail):
//     parents "Category Mute" ({0x12c4d2f, 13}) and "Category Filters" ({0x12c4ddd, 16}), targets
//     the two fresh vars, flags 0x30; the stack items are then destroyed by DIRECT 0x272d0 calls
//     (temporary lifetime — the menu list owns the registered copies). This module exposes that
//     as an explicit per-index hook with the byte facts documented; the menu path itself belongs
//     to the debug-menu modules.
//   * A "Unnamed Categories" ({0x12c501a}) menu-group block at 0xa5ddec is gated by a stack flag
//     (-0x731 & 1) that the visible body never sets nonzero; it is NOT modelled here (open).
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"
#include "shared_str_ctor_f1a0.hpp"
#include "shared_str_release_f590.hpp"
#include "sndx_command_handlers.hpp"  // shared vector/stride/offset constants of the sndx block

namespace sotc {

// ---- identity tags (never called) ----------------------------------------------------------------
inline constexpr std::uint64_t kSndxCategoriesBuilderRva = 0xA5D990;   // inside engine init 0xa5cd90
inline constexpr std::uint64_t kSndxCategoriesEngineInitRva = 0xA5CD90;
inline constexpr std::uint64_t kSndxNameFormatRva = 0x10A90;          // bpe::String::Format
inline constexpr std::uint64_t kSndxVectorGrowRva = 0xA62BC0;
inline constexpr std::uint64_t kSndxCategoryVarFnVtbl = 0x15D2060;     // same functor tag as slot 306
inline constexpr std::uint64_t kSndxMuteFormatRva = 0x12C4FE6;         // "sndxCategory_Mute_%d"
inline constexpr std::uint64_t kSndxFilterFormatRva = 0x12C4FFB;      // "sndxCategory_Filter_%d"
inline constexpr std::uint64_t kSndxCaptionFormatRva = 0x12C5012;    // "%d - %s"
inline constexpr std::uint64_t kSndxMuteParentRva = 0x12C4D2F;        // "Category Mute"
inline constexpr std::uint64_t kSndxFilterParentRva = 0x12C4DDD;      // "Category Filters"
inline constexpr std::uint64_t kSndxUnnamedGroupRva = 0x12C501A;      // "Unnamed Categories"
inline constexpr std::uint64_t kSndxEmptyDescRva = 0x12C4D1A;         // ""
inline constexpr std::size_t kSndxCategoryCount = 32;
inline constexpr std::size_t kSndxCategoryNameCapacity = 32;  // "sndxCategory_Filter_31" = 21 + NUL
inline constexpr std::size_t kSndxMuteVectorCapOffset = 0x8BE0;   // engine+0x8bd0 vector, cap slot
inline constexpr std::size_t kSndxFilterVectorCapOffset = 0x72B8;  // engine+0x72a8 vector, cap slot

enum class SndxCategoryVectorId : std::uint8_t { mute, filter };

// The builder appends to the shared SndxCategoryVectorSpan (see sndx_command_handlers.hpp): the
// {begin, end} walk range the handlers read, plus the capacity end the 0xa62bc0 growth hook owns.

// Reads the two vector spans out of an engine object at the pinned offsets (engine+0x8bd0..+0x8be0
// and engine+0x72a8..+0x72b8). The engine object itself stays opaque.
[[nodiscard]] SndxCategoryVectorSpan sndx_category_vectors_from_engine(
    const void* engine, SndxCategoryVectorId id) noexcept;

struct SndxCategoryHooks {
    // 0xf1a0 storage for the formatted names (data tag 0 / control tag 1), like the slot-306
    // separator strings. A missing allocator fails closed at the first name.
    SharedStrCtorHooks name_alloc{};
    // The 0xf590 release of the loop's stack reference on each owned name. Required whenever a
    // name owns storage (always, here); a null deallocator fails the construction.
    SharedStrDeallocatorF590* name_dealloc = nullptr;
    // 0xa62bc0 growth (reallocation only): must give the span capacity for at least one more
    // 0xc8 slot, preserving [begin, end). Called whenever end == cap before an append.
    bool (*grow)(void* host, SndxCategoryVectorId id, SndxCategoryVectorSpan& span) = nullptr;
    // The conditional per-index menu toggles (see the header comment for the byte facts).
    void (*menu_toggle)(void* host, std::size_t index, const char* name, void* mute_var,
                       void* filter_var) = nullptr;
    void* host = nullptr;
    [[nodiscard]] bool complete() const noexcept {
        return name_alloc.alloc.allocate != nullptr && name_alloc.alloc.deallocate != nullptr &&
               name_dealloc != nullptr && grow != nullptr;
    }
};

enum class SndxCategoriesStage : std::uint8_t {
    ok,
    hooks_missing,
    span_not_empty,      // entry vectors must be empty: this model owns the whole fill
    grow_failed,          // the grow hook refused or returned false
    name_alloc_failed,    // 0xf1a0 could not build the formatted name
    name_overflow,        // defensive: snprintf would truncate (the original cannot fail here)
    construct_failed,     // the bool-var construction or its registration failed
};

struct SndxCategoriesResult {
    bool ok = false;
    SndxCategoriesStage stage = SndxCategoriesStage::ok;
    std::size_t failed_index = kSndxCategoryCount;
    std::size_t mute_built = 0;
    std::size_t filter_built = 0;
    std::size_t grow_calls = 0;
    std::size_t menu_toggles = 0;   // hook invocations for enabled category names
};

// Runs the observed loop: 32 Mute + 32 Filter vars appended to `vectors`, registered into
// `registry`, with the per-index menu hook fired for every enabled `category_names[i]`.
// `category_names` may be null (no toggles). Stops at the first failure and names the stage and
// the index; a partial run is never reported as success.
[[nodiscard]] SndxCategoriesResult build_sndx_category_vars(
    SndxCategoryVectorSpan& mute, SndxCategoryVectorSpan& filter,
    const char* const* category_names, ConCmdRegistry& registry,
    const SndxCategoryHooks& hooks) noexcept;

// Formats one category name exactly as the model does (exposed for the test/checker contract).
[[nodiscard]] bool sndx_format_category_name(char (&buffer)[kSndxCategoryNameCapacity],
                                             SndxCategoryVectorId id, std::size_t index) noexcept;

}  // namespace sotc
