// Native recovery of the two console-variable constructor shapes that the startup ConCmd slots
// 298 / 299 / 301 share (original ELF sha256 d0723afb...5861d3). Names are ours.
//
// 1. Bool variable (class vtable address point 0x1594218, constructor body inlined into the
//    static initialiser). Verified at TWO independent sites: 0xac9550 (slot 301, `rumbleEnabled`)
//    and the second block of 0xb0c7a0 (slot 299, `globalDebugTextAdjustFor4K`); the stores are
//    identical except for the functor vtable tag; the third site (0xa82170, slot 303 `ProfileCPU`)
//    differs only in tag 0x15d2478 and in storing value 0 / default 0. Layout (object size 0xc1; the value byte is the
//    adjacent global the +0x60 pointer references):
//      +0x00 base (ConCmdBase, 0x60 B)   +0x60 value_ref          +0x68 default (byte)
//      +0x69 dword 0                      +0x70..0x8f zero         +0x90..0x9f never written
//      +0xa0 qword 0 (functor payload)    +0xb0 functor vtable tag +0xb8 qword 0 (functor aux)
//      +0xc0 value (byte)
//    Atexit destructor 0x20b0 (54 B, 399 image users): sets vptr 0x15940b0, calls the functor
//    destroy slot `[[+0xb0]+0x18]` on `this+0x90`, then the base destructor 0x20b50.
//
// 2. Ranged variable (0xb0e060 for slot 298; 0xc300 for slot 299). Layout (object size 0xd4):
//      +0x00 base          +0x60 value_ref        +0x68 default (dword)
//      +0x6c min value     +0x70 min engaged      +0x74 max value   +0x78 max engaged
//      +0x80..0x9f copy of the caller's four argument qwords
//      +0xa0 functor (inline buffer 0x10 B, payload +0xb0, +0xb8 never written, vtable tag +0xc0,
//            aux +0xc8)                           +0xd0 value (dword)
//    The functor is a type-erased small callable. For the shaderTypeToLoad site its vtable
//    0x15d5960 has four slots, ALL lifted trivially: 0 = copy-construct {dst+0x10 = src+0x10;
//    dst+0x20 = 0x15d5960} (0xb0e1f0), 1 = clone (0xb0e210, same stores), 2 = move-assign payload
//    (0xb0e230), 3 = destroy = a bare `ret` (0xb0e240). That bare `ret` is the target of the
//    `call [rax+0x18]` that the earlier analysis called unresolved (R4). The vtable is read from
//    the ELF `.data.rel.ro` RELATIVE relocations 0x15d5960..0x15d5978.
//
// vptr / functor-vtable values are IDENTITY TAGS (original vtable address points); nothing in
// native code dispatches through them. The class destructors are pinned, never executed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "shared_str_release_f590.hpp"  // SharedStrDeallocatorF590 for init_bool_var_vector

namespace sotc {

// ---- bool variable ---------------------------------------------------------------------------
inline constexpr std::uint64_t kBoolVarMidVptr = 0x15940b0;    // 0x15940a0 + 0x10 (during construction)
inline constexpr std::uint64_t kBoolVarFinalVptr = 0x1594218;  // 0x1594208 + 0x10
inline constexpr std::uint64_t kBoolVarDtorRva = 0x20b0;
inline constexpr std::size_t kBoolVarDtorSize = 0x36;

struct ConCmdBoolVar {
    ConCmdBase base;               // +0x00
    std::uint8_t* value_ref;       // +0x60
    std::uint8_t default_value;    // +0x68
    std::uint8_t zero69[4];        // +0x69 (dword store of 0)
    std::uint8_t gap6d[3];         // +0x6d never written
    std::uint8_t zero70[0x20];     // +0x70
    std::uint8_t gap90[0x10];      // +0x90 never written (functor inline buffer)
    std::uint64_t fn_payload;      // +0xa0
    std::uint8_t gapa8[8];         // +0xa8 never written
    std::uint64_t fn_vptr;         // +0xb0 functor vtable tag
    std::uint64_t fn_aux;          // +0xb8
    std::uint8_t value;            // +0xc0 the adjacent global the original's +0x60 points at
};
static_assert(offsetof(ConCmdBoolVar, value_ref) == 0x60 && offsetof(ConCmdBoolVar, default_value) == 0x68 &&
              offsetof(ConCmdBoolVar, zero69) == 0x69 && offsetof(ConCmdBoolVar, zero70) == 0x70 &&
              offsetof(ConCmdBoolVar, gap90) == 0x90 && offsetof(ConCmdBoolVar, fn_payload) == 0xa0 &&
              offsetof(ConCmdBoolVar, fn_vptr) == 0xb0 && offsetof(ConCmdBoolVar, fn_aux) == 0xb8 &&
              offsetof(ConCmdBoolVar, value) == 0xc0);

struct BoolVarResult {
    bool ok = false;                       // every stage below completed
    ConCmdRegisterResult registration{};
    bool views_inert = false;              // both 0xf590 calls were exact no-ops
    bool atexit_recorded = false;          // dtor 0x20b0 recorded, not run
};

// The shared static-initialiser shape (see header). `fn_vptr_tag`, `value` and `default_value` are
// the per-site constants besides the name: 0x15d5070 at slot 301 and 0x15d5800 at slot 299's second
// block both store value 1 / default 1 (the parameter defaults), while slot 303 (`ProfileCPU`,
// functor tag 0x15d2478) stores value 0 / default 0. `value` is the byte at +0xc0 (stored first),
// `default_value` the byte at +0x68 (stored second), exactly as the three bodies order them.
[[nodiscard]] BoolVarResult init_bool_var_static(ConCmdBoolVar& var, const SharedStr& name,
                                                 const SharedStr& description,
                                                 std::uint64_t fn_vptr_tag,
                                                 ConCmdRegistry& registry,
                                                 std::uint8_t value = 1,
                                                 std::uint8_t default_value = 1) noexcept;

// CPU-01-SLOT306-CATEGORY-VARS-01: the same construction without the __cxa_atexit record — for
// vector-owned bool vars (the engine-init 0xa5d990 category loop) whose teardown is the engine's,
// not atexit's. An OWNED name (ctl != nullptr, e.g. a formatted `sndxCategory_*` name built with
// the 0xf1a0 model) acquires a reference during construction and releases the caller's reference
// through the real 0xf590 here, so it requires `name_deallocator`; a borrowed name is released as
// an inert view and needs none.
[[nodiscard]] BoolVarResult init_bool_var_vector(ConCmdBoolVar& var, const SharedStr& name,
                                                 const SharedStr& description,
                                                 std::uint64_t fn_vptr_tag,
                                                 ConCmdRegistry& registry,
                                                 SharedStrDeallocatorF590* name_deallocator,
                                                 std::uint8_t value = 1,
                                                 std::uint8_t default_value = 1) noexcept;

// ---- ranged variable -------------------------------------------------------------------------
inline constexpr std::uint64_t kRangedEnumCtorRva = 0xb0e060;  // slot 298 constructor, 0x143 B
inline constexpr std::size_t kRangedEnumCtorSize = 0x143;
inline constexpr std::uint64_t kRangedEnumMidVptr = 0x15d5880; // 0x15d5870 + 0x10
inline constexpr std::uint64_t kRangedEnumFnVtbl = 0x15d5960;  // functor vtable of the site
inline constexpr std::uint64_t kRangedEnumFnCopyRva = 0xb0e1f0;    // slot 0
inline constexpr std::uint64_t kRangedEnumFnCloneRva = 0xb0e210;   // slot 1
inline constexpr std::uint64_t kRangedEnumFnMoveRva = 0xb0e230;    // slot 2
inline constexpr std::uint64_t kRangedEnumFnDestroyRva = 0xb0e240; // slot 3: bare ret

// The by-pointer argument block (r8) that 0xb0e060 copies into the object. Observed at the only
// call site: the four qwords are zero and the functor carries payload 0 / aux 0.
struct RangedVarArgs {
    std::uint64_t q[4];            // +0x00 .. +0x1f
    std::uint8_t fn_inline[0x10];  // +0x20 never read
    std::uint64_t fn_payload;      // +0x30
    std::uint64_t fn_gap38;        // +0x38 never read
    std::uint64_t fn_vptr;         // +0x40 functor vtable tag (selects the clone/copy slots)
    std::uint64_t fn_aux;          // +0x48
};
static_assert(sizeof(RangedVarArgs) == 0x50 && offsetof(RangedVarArgs, fn_payload) == 0x30 &&
              offsetof(RangedVarArgs, fn_vptr) == 0x40 && offsetof(RangedVarArgs, fn_aux) == 0x48);

struct ConCmdRangedVar {
    ConCmdBase base;               // +0x00
    void* value_ref;               // +0x60
    std::uint32_t default_value;   // +0x68
    std::uint8_t min_value[4];     // +0x6c
    std::uint8_t min_engaged;      // +0x70
    std::uint8_t gap71[3];         // +0x71 never written
    std::uint8_t max_value[4];     // +0x74
    std::uint8_t max_engaged;      // +0x78
    std::uint8_t gap79[7];         // +0x79 never written
    std::uint64_t args[4];         // +0x80 copy of RangedVarArgs::q
    std::uint8_t fn_inline[0x10];  // +0xa0 never written
    std::uint64_t fn_payload;      // +0xb0
    std::uint64_t gapb8;           // +0xb8 never written
    std::uint64_t fn_vptr;         // +0xc0 functor vtable tag
    std::uint64_t fn_aux;          // +0xc8
    std::uint32_t value;           // +0xd0 the adjacent global the original's +0x60 points at
};
static_assert(offsetof(ConCmdRangedVar, value_ref) == 0x60 && offsetof(ConCmdRangedVar, default_value) == 0x68 &&
              offsetof(ConCmdRangedVar, min_value) == 0x6c && offsetof(ConCmdRangedVar, min_engaged) == 0x70 &&
              offsetof(ConCmdRangedVar, max_value) == 0x74 && offsetof(ConCmdRangedVar, max_engaged) == 0x78 &&
              offsetof(ConCmdRangedVar, args) == 0x80 && offsetof(ConCmdRangedVar, fn_inline) == 0xa0 &&
              offsetof(ConCmdRangedVar, fn_payload) == 0xb0 && offsetof(ConCmdRangedVar, fn_vptr) == 0xc0 &&
              offsetof(ConCmdRangedVar, fn_aux) == 0xc8 && offsetof(ConCmdRangedVar, value) == 0xd0);

// 0xc300 (352 B; 680 image callers): the float variable constructor with BOTH bounds engaged.
// this=rdi, name=rsi, value_ref=rdx, &min=rcx, &max=r8, flags=r9d, args and description on the
// stack. Same layout as above with `min_value`/`max_value` = the two floats and both engaged
// bytes = 1. Its functor class vtable is 0x1594318 (slots 0xc4a0 copy / 0xc4c0 clone / 0xc4e0
// move / 0xc4f0 bare-ret destroy); the caller's argument functor uses 0x15d57e0, whose four slots
// are the same four bodies. Any other functor vtable is an unlifted contract.
inline constexpr std::uint64_t kRangedFloatCtorRva = 0xc300;
inline constexpr std::size_t kRangedFloatCtorSize = 0x160;
inline constexpr std::uint64_t kRangedFloatMidVptr = 0x1594128;    // 0x1594118 + 0x10
inline constexpr std::uint64_t kRangedFloatFinalVptr = 0x15942b0;  // 0x15942a0 + 0x10
inline constexpr std::uint64_t kRangedFloatFnVtbl = 0x1594318;
inline constexpr std::uint64_t kRangedFloatArgFnVtbl = 0x15d57e0;
// Third caller functor vtable (startup slot 306, the five `sndx_` floats): its four slot bodies are the
// same as 0x15d57e0 / 0x1594318 (byte-pinned by tools/lift/check_ctor_slot_306.py).
inline constexpr std::uint64_t kRangedFloatArgFnVtblSndx = 0x15d2040;
inline constexpr std::uint64_t kRangedFloatFnCopyRva = 0xc4a0;
inline constexpr std::uint64_t kRangedFloatFnCloneRva = 0xc4c0;
inline constexpr std::uint64_t kRangedFloatFnMoveRva = 0xc4e0;
inline constexpr std::uint64_t kRangedFloatFnDestroyRva = 0xc4f0;
inline constexpr std::uint64_t kRangedFloatDtorRva = 0x20f0;       // pinned, not executed
inline constexpr std::size_t kRangedFloatDtorSize = 0x36;

[[nodiscard]] bool init_ranged_float_var_c300(ConCmdRangedVar& var, const SharedStr& name,
                                              void* value_ref, float min_value, float max_value,
                                              std::uint32_t flags, const RangedVarArgs& args,
                                              const SharedStr& description) noexcept;

// The argument copy at +0x80 and the functor install shared by 0xb0e060 / 0xc300 / 0xc580 and the
// 0xa4f690 family: clone -> copy -> copy(this.fn) -> copy(this.fn) -> aux swap -> bare-ret destroy,
// collapsed to its net state (this.fn = {payload <- args.fn_payload, vptr = vtbl, aux <- args.fn_aux}
// and args q[0..3] with the q[3] self-pointer rule). `vtbl` is the class tag the install stores.
void install_ranged_fn_from_args(ConCmdRangedVar& var, const RangedVarArgs& args,
                                 std::uint64_t vtbl) noexcept;

// 0xb0e060 (this=rdi, name=rsi, value_ref=rdx, flags=ecx, args=r8, description=r9): the
// "enum"-flavoured constructor (both optional bounds disengaged). Returns false only when the
// caller supplied a functor vtable other than 0x15d5960, whose slot code is not lifted; in that
// case NOTHING is written (the original would run the unknown slot after most of the stores, so a
// partial object would be a fabrication).
[[nodiscard]] bool init_ranged_var_b0e060(ConCmdRangedVar& var, const SharedStr& name,
                                          void* value_ref, std::uint32_t flags,
                                          const RangedVarArgs& args,
                                          const SharedStr& description) noexcept;

}  // namespace sotc
