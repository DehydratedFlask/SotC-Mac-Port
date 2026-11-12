// CPU-01-HELPER-CALLEE-MODELS-04: native model of the two dword-valued, both-bounds-engaged ranged console-variable constructors
// 0x49ca60 and 0x49cc60 (352 B each, FDE 352; startup slot 451). Original ELF sha256 d0723afb...5861d3; every fact is byte-pinned by
// tools/lift/check_helper_49ca60.py. NOTHING here executes, loads or calls an original RVA: vptr, functor class and slot RVAs are IDENTITY TAGS.
//
// Both bodies are instruction-for-instruction the same as the float ranged constructor 0xc300 (init_ranged_float_var_c300, already native) except
// for the vptr table, the functor class constant and the two branch displacements (the checker re-derives that equivalence from the bytes), so the
// object layout is ConCmdRangedVar and the argument copy / functor install is install_ranged_fn_from_args.
//   SysV args: rdi = this, rsi = name (SharedStr*), rdx = value_ref (stored at +0x60), rcx = &min (dword), r8 = &max (dword), r9d = flags,
//   stack: [rbp+0x10] = RangedVarArgs*, [rbp+0x18] = description (SharedStr*, forwarded to 0x20af0).
//   0x20af0(this, name, description, flags) ; [this] = vptr table + 0x10 ; [+0x60] = value_ref ; dword [+0x6c] = *min ; byte [+0x70] = 1 ;
//   dword [+0x74] = *max ; byte [+0x78] = 1 ; ymm [+0x80] = 0 ; [+0xc0] = class ; [+0xb0] = 0 ; [+0xc8] = 0 ; then the install
//   (clone / copy / copy / copy / aux swap / bare-ret destroy) = net { [+0xb0] = args.fn_payload, [+0xc0] = class, [+0xc8] = args.fn_aux }
//   and args.q[0..3] -> [+0x80..+0x9f] with the q[3] self-reference rule.
// The callee does NOT store the value (+0xd0), does NOT register, and does NOT run __cxa_atexit: slot 451's call sites do all of that.
//
// Admission. The callee dereferences `[args.fn_vptr + 8]` (clone) and the class it stores. At both observed sites (0x49dbad, 0x49df7f) the caller's
// functor is the constructor's OWN class table (A = 0x15bfce8, B = 0x15bfd80), each = {copy, clone, move, bare-ret destroy} that store the class
// constant. Any other tag has no lifted slot and is refused with NOTHING written.
//
// NOT established: what the dword value means (int vs unsigned: both are plain dword copies); the consumers of the variable; the qword q[1..3] callbacks.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"
#include "helper_functor_tags.hpp"

namespace sotc {

enum class RangedDwordVariant : std::uint8_t { a_49ca60, b_49cc60 };

inline constexpr std::uint64_t kRangedDwordACtorRva = 0x49ca60;
inline constexpr std::uint64_t kRangedDwordBCtorRva = 0x49cc60;
inline constexpr std::size_t kRangedDwordCtorSize = 352;
inline constexpr std::uint64_t kRangedDwordAVptrTable = 0x15bfb20;   // [this] = table + 0x10
inline constexpr std::uint64_t kRangedDwordBVptrTable = 0x15bfb98;
inline constexpr std::uint64_t kRangedDwordAClass = 0x15bfce8;       // stored at +0xc0 ; copy / clone slots store the same constant
inline constexpr std::uint64_t kRangedDwordBClass = 0x15bfd80;
inline constexpr std::uint64_t kRangedDwordAFnCopyRva = 0x49cc00;    // class A slots: copy / clone / move / destroy(bare ret)
inline constexpr std::uint64_t kRangedDwordAFnCloneRva = 0x49cc20;
inline constexpr std::uint64_t kRangedDwordAFnMoveRva = 0x49cc40;
inline constexpr std::uint64_t kRangedDwordAFnDestroyRva = 0x49cc50;
inline constexpr std::uint64_t kRangedDwordBFnCopyRva = 0x49ce00;    // class B slots
inline constexpr std::uint64_t kRangedDwordBFnCloneRva = 0x49ce20;
inline constexpr std::uint64_t kRangedDwordBFnMoveRva = 0x49ce40;
inline constexpr std::uint64_t kRangedDwordBFnDestroyRva = 0x49ce50;

[[nodiscard]] constexpr std::uint64_t ranged_dword_vptr(RangedDwordVariant v) noexcept {
    return (v == RangedDwordVariant::a_49ca60 ? kRangedDwordAVptrTable : kRangedDwordBVptrTable) + 0x10;
}
[[nodiscard]] constexpr std::uint64_t ranged_dword_class(RangedDwordVariant v) noexcept {
    return v == RangedDwordVariant::a_49ca60 ? kRangedDwordAClass : kRangedDwordBClass;
}

struct RangedDwordVarResult {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;
};

// `min_value` / `max_value` are the four bytes at *rcx / *r8 (copied verbatim, never interpreted). Refusals write nothing:
//   unadmitted_arg_functor  args.fn_vptr is not the constructor's own class
//   wrong_helper            a null min / max pointer (the original dereferences both)
[[nodiscard]] RangedDwordVarResult helper_ranged_dword_var_49ca60(ConCmdRangedVar& var, RangedDwordVariant variant, const SharedStr& name,
                                                                  void* value_ref, const std::uint8_t* min_value,
                                                                  const std::uint8_t* max_value, std::uint32_t flags,
                                                                  const RangedVarArgs& args, const SharedStr& description) noexcept;

}  // namespace sotc
