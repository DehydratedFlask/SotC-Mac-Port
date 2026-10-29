// CPU-01-STARTUP-SLOTS-343-356 (lane L4): shared construction shapes for the nine startup constructor
// slots 343 (0x7d7970), 344 (0x7d3810), 345 (0x7cec60), 347 (0x7c93a0), 348 (0x7c9150), 350 (0x7af0b0),
// 353 (0x7a14d0), 355 (0x7946d0) and 356 (0x78e2f0). Original ELF sha256 d0723afb...5861d3.
//
// Every shape below was read end to end from the pinned bytes (xcrun llvm-objdump -d) and is byte-pinned
// by tools/lift/check_ctor_slot_343_356_shapes.py. NOTHING here executes, loads or calls an original RVA.
// Vtable pointers, functor vtable tags and destructor RVAs are IDENTITY TAGS (original addresses kept as
// numbers); the destructors are recorded with the atexit log, never run.
//
// Shapes already native and reused unchanged: bool variable (core/concmd_vars init_bool_var_static),
// console command object (core/fconcmd_object_slot306 init_fconcmd_object), debug-menu toggle 0x26ea0
// (core/debug_menu_item), registration 0x268b0 (core/debug_menu_item).
//
// New shapes recovered here:
//   * 0x6aa00 (322 B)  UNBOUNDED float variable constructor: byte-identical in structure to the bounded
//        0xc300 constructor except that [this+0x70] (min engaged) and [this+0x78] (max engaged) are
//        stored 0 and only the low byte of the min/max slots is written (0x6c / 0x74); the class
//        functor vtable it stores first is 0x1596878 (whose four slots are the float-functor bodies
//        0xc4a0/0xc4c0/0xc4e0/0xc4f0), and the three functor calls leave [this+0xc0] = 0x1594318, exactly
//        as at 0xc300. this=rdi, name=rsi, value_ref=rdx, flags=ecx, args=r8, description=r9.
//   * the float functor vtables 0x15ce318 / 0x15ce260 / 0x15ce1c0 / 0x15cd240 (the caller argument
//        functors of slots 347 / 350 / 353 / 364) have the same four slot bodies as the lifted float
//        tables, so the 0xc300 / 0x6aa00 functor sequence is reproduced exactly (checker-pinned).
//   * 0x26db0 (0xef B)  debug-menu toggle constructor that takes its CAPTION from the target variable:
//        this=rdi, parent=rsi (a SharedStr view), target=rdx (a ConCmd variable), flags=ecx. View A
//        (+0x28) = *parent (+ lock inc ctl), kind = 2, view B (+0x50) = target->name (+0x20, +lock inc
//        ctl), view C = 0xf180, target pointer, range {1.0, 10.0} unless the target's virtual slots
//        [+0x18] == 1 && [+0x30] == 3 (then {0.1, 0.5}); then tail-calls 0x268b0(this, [0x2dfd4b8]).
//        Compared with 0x26ea0 it has no separate caption argument.
//   * the 24-byte {begin,end,cap} zero static with a free-on-destroy destructor 0x7d3af0 (slot 343).
//   * the 0x20-byte flag/atomic cells of slot 344.
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"

namespace sotc {

// ---- float functor identity ----------------------------------------------------------------------------
// Caller argument functors of the lifted float sites. Their clone/copy/move/destroy slots are the float
// functor bodies, so the object's final functor vtable is 0x1594318 for every one of them.
inline constexpr std::uint64_t kFloatArgFnVtbl_347 = 0x15ce318;
inline constexpr std::uint64_t kFloatArgFnVtbl_350 = 0x15ce260;
inline constexpr std::uint64_t kFloatArgFnVtbl_353B = 0x15ce1c0;
inline constexpr std::uint64_t kFloatArgFnVtblSlot364 = 0x15cd240;  // slot-364 caller; byte-pinned to the same four bodies

inline constexpr std::uint64_t kUnboundedFloatCtorRva = 0x6aa00;
inline constexpr std::size_t kUnboundedFloatCtorSize = 0x142;
inline constexpr std::uint64_t kUnboundedFloatInitialFnVtbl = 0x1596878;   // stored at [this+0xc0] first
inline constexpr std::uint64_t kUnboundedFloatMidVptr = 0x1594128;         // 0x1594118 + 0x10

inline constexpr std::uint64_t kMenuToggleFromVarCtorRva = 0x26db0;
inline constexpr std::size_t kMenuToggleFromVarCtorSize = 0xef;   // code ends at 0x26e9f (jmp 0x268b0)

// True for the argument-functor vtables whose slot bodies are the lifted float functor bodies.
[[nodiscard]] bool is_known_float_arg_functor(std::uint64_t vtbl) noexcept;

// 0xc300 with the extended functor set (the shared init_ranged_float_var_c300 only accepts three
// vtables). Same stores. Returns false (nothing written) for an unknown argument functor.
[[nodiscard]] bool init_bounded_float_var_c300(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                               float min_value, float max_value, std::uint32_t flags,
                                               const RangedVarArgs& args, const SharedStr& description) noexcept;

// 0x6aa00. Returns false (nothing written) for an unknown argument functor.
[[nodiscard]] bool init_unbounded_float_var_6aa00(ConCmdRangedVar& var, const SharedStr& name, void* value_ref,
                                                  std::uint32_t flags, const RangedVarArgs& args,
                                                  const SharedStr& description) noexcept;

// One float-variable static-initialiser block: [obj] = 0x15942b0 ; constructor ; f590(description) ;
// destroy slot of the caller functor (bare ret) ; [obj] = 0x15942b0 ; value = default ; register ;
// f590(name) ; __cxa_atexit(0x20f0). `bounded` selects 0xc300 (min/max given) or 0x6aa00.
struct FloatVarSpec {
    const char* name;                 // borrowed, NUL-terminated
    const char* description;          // borrowed, NUL-terminated (empty at every observed site)
    bool bounded;
    std::uint32_t lo_bits;            // IEEE bits of the minimum (bounded only)
    std::uint32_t hi_bits;            // IEEE bits of the maximum (bounded only)
    std::uint32_t default_bits;       // initial value and default
    std::uint64_t arg_fn_vtbl;        // caller argument functor tag
    std::uint32_t flags;              // 1 at every site
};

struct FloatVarResult {
    bool ok = false;
    bool ctor_ok = false;             // known argument functor, constructor ran
    bool views_inert = false;         // both 0xf590 calls were exact no-ops
    bool functor_destroy_ok = false;  // [argfn + 0x18] is the bare-ret destroy slot
    ConCmdRegisterResult registration{};
    bool atexit_recorded = false;     // dtor 0x20f0 recorded, not run
};

[[nodiscard]] FloatVarResult init_float_var_static(ConCmdRangedVar& var, const FloatVarSpec& spec,
                                                   ConCmdRegistry& registry) noexcept;

// ---- 0x26db0 ----------------------------------------------------------------------------------------------
// Fails closed (false, nothing written) when the target's class is not the lifted bool class.
[[nodiscard]] bool init_menu_toggle_from_var_26db0(DebugMenuItem& item, const SharedStr& parent,
                                                   const ConCmdBoolVar& target, std::uint32_t flags) noexcept;

// ---- slot 343: 24-byte zero static + atexit(dtor) ----------------------------------------------------------
inline constexpr std::uint64_t kZeroVectorDtorRva = 0x7d3af0;   // `mov rdi,[rdi]; test; je ret; jmp 0x170c0`
inline constexpr std::size_t kZeroVectorDtorSize = 0xe;         // 0x7d3af0..0x7d3afd inclusive of the ret

struct ZeroVector24 {
    std::uint64_t begin;   // +0x00
    std::uint64_t end;     // +0x08
    std::uint64_t cap;     // +0x10
};
static_assert(sizeof(ZeroVector24) == 0x18);

// vmovups xmm0(0) -> [cell]; movq 0 -> [cell+0x10]; __cxa_atexit(0x7d3af0, cell, &dso).
[[nodiscard]] bool init_zero_vector24(ZeroVector24& cell, ConCmdRegistry& registry) noexcept;

// ---- slot 344: two 0x20-byte cells -------------------------------------------------------------------------
inline constexpr std::uint64_t kFlagScaleQword = 0x3f80000000000001ull;   // {dword 1, float 1.0}

struct FlagScaleCell {
    std::uint64_t flag_scale;     // +0x00 stored with `movq %rdx` (0x3f80000000000001)
    std::uint8_t gap08[8];        // +0x08 never written
    std::int32_t atomic_word;     // +0x10 stored with a locked `xchgl` of 0 (seq_cst store)
    std::uint8_t gap14[12];       // +0x14 never written
};
static_assert(sizeof(FlagScaleCell) == 0x20 && offsetof(FlagScaleCell, atomic_word) == 0x10);

// Two stores in the original order: qword, then the locked word store.
void init_flag_scale_cell(FlagScaleCell& cell) noexcept;

// ---- site-level helpers (the per-slot modules compose these) ---------------------------------------------------
// Bool-variable static initialiser with a NUL-terminated borrowed name and an empty borrowed description.
[[nodiscard]] BoolVarResult init_bool_var_site(ConCmdBoolVar& var, const char* name, std::uint64_t fn_tag,
                                               ConCmdRegistry& registry, std::uint8_t value,
                                               std::uint8_t default_value) noexcept;

struct MenuSiteResult {
    bool ok = false;
    bool ctor_ok = false;
    DebugMenuRegisterResult registration{};
    bool views_inert = false;       // the 0xf590 releases that follow the constructor were exact no-ops
    bool atexit_recorded = false;   // dtor 0x272d0 recorded, not run
};

// 0x26ea0 toggle: ctor(label, text, target, 0x30) ; (tail) 0x268b0 ; f590(text) ; f590(label) ; atexit 0x272d0.
[[nodiscard]] MenuSiteResult init_toggle_site_26ea0(DebugMenuItem& item, const ConCmdBoolVar& target,
                                                    const char* label, const char* text, ConCmdRegistry& registry,
                                                    DebugMenuRegistry& menu) noexcept;

// 0x26db0 toggle: ctor(parent, target, 0x30) ; (tail) 0x268b0 ; f590(parent) ; atexit 0x272d0.
[[nodiscard]] MenuSiteResult init_toggle_site_26db0(DebugMenuItem& item, const ConCmdBoolVar& target,
                                                    const char* parent, ConCmdRegistry& registry,
                                                    DebugMenuRegistry& menu) noexcept;

inline constexpr std::uint32_t kL4MenuItemFlags = 0x30;   // dword [item+0xa0] at every observed site

}  // namespace sotc
