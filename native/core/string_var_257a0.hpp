// CPU-01-STRING-VAR-257A0-01: native recovery of the string-valued console-variable constructor 0x257a0
// (0x14c B, [0x257a0, 0x258ec)) and its inner constructor 0x258f0 (0x157 B, [0x258f0, 0x25a47)), used by
// startup slot 310 (0xa479d0, `titleid`). Original ELF sha256 d0723afb...5861d3; pinned by
// tools/lift/check_ctor_slot_310.py. Nothing here executes an original RVA: the vtable pointers, the
// destructor and the argument-functor class are identity tags.
//
// Object layout (0x148 B):
//   +0x00 ConCmdBase (0x60, built by 0x20af0)     +0x60 value_ref  = &value (obj+0x128)
//   +0x68 default SharedStr (0x20)                +0x88 byte 0, +0xa8 byte 0, +0xb0 byte 0, +0xd0 byte 0
//   +0xd8 args q[4] (copied from the caller's argument block; the self-pointer rule of 0xc580 applies)
//   +0xf8 functor object: payload +0x108, class tag +0x118, aux +0x120 (inline bytes +0xf8..+0x107 never written)
//   +0x128 value SharedStr (0x20), a COPY of the caller's string view (control counter incremented)
// 0x257a0(this, name, init_view): [this] = 0x1594ef0 (0x1594ee0 + 0x10, kept in a callee-saved register);
//   0x258f0(this, name, &this->value, flags 1, args = zero block + functor class 0x1595068, description view =
//   {0xf96388 (empty string), 0, 0, 0}); 0xf590(description); caller functor destroy (slot 3 = 0x25aa0, a
//   bare ret); [this] = 0x1594ef0; value = init_view (qword copies +0x128..+0x147, lock inc [ctl]);
//   if (this+0x68 != init_view) { 0xf590(this+0x68); default = init_view (copy, lock inc [ctl]) };
//   0x20d60(this).
// 0x258f0(this, name, value_ref, flags, args, description): 0x20af0(this, name, description, flags);
//   [this] = 0x1594f58 + 0x10 (transient: 0x257a0 overwrites it); [+0x60] = value_ref; 0xf180(this+0x68)
//   (empty view); bytes +0x88 / +0xa8 / +0xb0 / +0xd0 = 0; ymm [+0xd8] = 0; [+0x118] = 0x1595068;
//   [+0x108] = 0; [+0x120] = 0; args copy (q[1..2] as one xmm, q[0], q[3]; if q[3] != 0 then [+0xd8] =
//   this+0xd8); then the functor install through the class 0x1595068 slots (0x25a50 copy, 0x25a70 clone:
//   payload at +0x10 and the class tag at +0x20 are stored, nothing else) and swap(this.aux, args.aux).
// The class 0x1594ef0 answers the two virtual calls of the debug-menu toggle constructor 0x26ea0 with
// slot 3 = 1 (0x246a0) and slot 6 = 4 (0x24780): 6 != 3, so the toggle keeps the default range {1.0, 10.0}.
// Only the lifted functor class tag 0x1595068 is accepted (any other tag would run an unread clone slot):
// the model refuses with `false` and writes nothing. The consumers of the variable (title id) are NOT
// modelled; the destructor 0x23c90 and the deleting destructor 0x24610 (which releases +0x128) are tags.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kStringVarCtorRva = 0x257a0;
inline constexpr std::size_t kStringVarCtorSize = 0x14c;
inline constexpr std::uint64_t kStringVarInnerRva = 0x258f0;
inline constexpr std::size_t kStringVarInnerSize = 0x157;
inline constexpr std::uint64_t kStringVarFinalVptr = 0x1594ef0;       // 0x1594ee0 + 0x10
inline constexpr std::uint64_t kStringVarInnerVptr = 0x1594f68;       // 0x1594f58 + 0x10 (transient)
inline constexpr std::uint64_t kStringVarFnVtbl = 0x1595068;          // functor class tag
inline constexpr std::uint64_t kStringVarDtorRva = 0x23c90;           // class slot 0 (atexit target)
inline constexpr std::uint64_t kStringVarSlot3Rva = 0x246a0;          // mov eax,1; ret
inline constexpr std::uint64_t kStringVarSlot6Rva = 0x24780;          // mov eax,4; ret
inline constexpr std::uint32_t kStringVarSlot3Result = 1;
inline constexpr std::uint32_t kStringVarSlot6Result = 4;
inline constexpr std::uint32_t kStringVarFlags = 1;
inline constexpr std::uint64_t kStringVarEmptyDescRva = 0xf96388;     // the description view's data (a NUL byte)

struct ConCmdStringVar {
    ConCmdBase base;               // +0x00
    void* value_ref;               // +0x60
    SharedStr default_value;       // +0x68
    std::uint8_t byte88;           // +0x88
    std::uint8_t gap89[0x1f];      // +0x89 never written
    std::uint8_t bytea8;           // +0xa8
    std::uint8_t gapa9[7];         // +0xa9 never written
    std::uint8_t byteb0;           // +0xb0
    std::uint8_t gapb1[0x1f];      // +0xb1 never written
    std::uint8_t byted0;           // +0xd0
    std::uint8_t gapd1[7];         // +0xd1 never written
    std::uint64_t args[4];         // +0xd8
    std::uint8_t fn_inline[0x10];  // +0xf8 never written
    std::uint64_t fn_payload;      // +0x108
    std::uint64_t fn_gap110;       // +0x110 never written
    std::uint64_t fn_vptr;         // +0x118 functor class tag
    std::uint64_t fn_aux;          // +0x120
    SharedStr value;               // +0x128
};
static_assert(sizeof(ConCmdStringVar) == 0x148 && offsetof(ConCmdStringVar, value_ref) == 0x60 &&
              offsetof(ConCmdStringVar, default_value) == 0x68 && offsetof(ConCmdStringVar, byte88) == 0x88 &&
              offsetof(ConCmdStringVar, bytea8) == 0xa8 && offsetof(ConCmdStringVar, byteb0) == 0xb0 &&
              offsetof(ConCmdStringVar, byted0) == 0xd0 && offsetof(ConCmdStringVar, args) == 0xd8 &&
              offsetof(ConCmdStringVar, fn_payload) == 0x108 && offsetof(ConCmdStringVar, fn_vptr) == 0x118 &&
              offsetof(ConCmdStringVar, fn_aux) == 0x120 && offsetof(ConCmdStringVar, value) == 0x128);

// 0x258f0. `false` (nothing written) for an unlifted argument-functor tag.
[[nodiscard]] bool init_string_var_inner_258f0(ConCmdStringVar& var, const SharedStr& name, void* value_ref,
                                               std::uint32_t flags, const RangedVarArgs& args,
                                               const SharedStr& description) noexcept;

struct StringVarResult {
    bool ok = false;
    ConCmdRegisterResult registration{};
    bool views_inert = false;      // the 0xf590 calls on the description view and on the empty default were no-ops
};

// 0x257a0. `init_view` is copied twice (value and default); a non-null control counter is incremented twice.
[[nodiscard]] StringVarResult init_string_var_257a0(ConCmdStringVar& var, const SharedStr& name,
                                                    const SharedStr& init_view, ConCmdRegistry& registry) noexcept;

// The two virtual-slot results 0x26ea0 reads from a variable of class 0x1594ef0 (known only when the vptr
// is exactly that class).
[[nodiscard]] DebugMenuTarget string_var_menu_target(const ConCmdStringVar& var) noexcept;

}  // namespace sotc
