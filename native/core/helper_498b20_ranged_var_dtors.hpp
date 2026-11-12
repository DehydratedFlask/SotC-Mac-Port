// CPU-01-SLOT408-PREREQS: native model of the five ranged console-variable destructors of the 0xc8 B variable objects built
// by the 0x49ca60 / 0x140b00 / 0x25ab0 family (ELF bodies pinned per instruction by tools/lift/check_helper_498b20.py).
//
//   store the final vptr at [obj]                                      ; class + 0x10 for four, a literal for 0x140010
//   rdi = obj + 0xa0 ; rax = [obj + 0xc0] ; call [rax + 0x18]          ; destroy slot of the caller functor at +0xa0
//   rdi = obj ; jmp 0x20b50                                            ; base console-variable destructor (native elsewhere)
//
// The functor destroy is an indirect call and the base
// dtor 0x20b50 is a separate native model, so both are HOOKS here (for the 0x49ca60 classes the functor destroy slots 0x49cc50 / 0x49ce50 are bare `ret`, pinned by check_helper_49ca60; the other classes' slots are NOT pinned here); the order (vptr store, functor destroy, base dtor) and
// the arguments (functor block = obj + 0xa0, its vtable tag = the qword at obj + 0xc0) are the pinned facts. The vptr
// values are guest class TAGS, never dereferenced. Dtors are never run by a startup slot (atexit identity tags).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kRangedVarDtorFnOffset = 0xa0;
inline constexpr std::size_t kRangedVarDtorFnVptrOffset = 0xc0;
inline constexpr std::size_t kRangedVarDtorDestroySlotOffset = 0x18;
inline constexpr std::uint64_t kRangedVarBaseDtorRva = 0x20b50;
inline constexpr std::size_t kRangedVarObjectSize = 0xc8;

struct RangedVarDtorRow {
    std::uint64_t rva;
    std::size_t size;
    std::uint64_t vptr;   // final vptr stored at [obj]
};

inline constexpr std::array<RangedVarDtorRow, 5> kRangedVarDtors = {{
    {0x498b20, 54, 0x15bfb30},   // class 0x15bfb20 + 0x10
    {0x498b60, 54, 0x15bfba8},   // class 0x15bfb98 + 0x10
    {0x23d20, 54, 0x1594fe0},    // class 0x1594fd0 + 0x10
    {0x13a090, 54, 0x1598250},   // class 0x1598240 + 0x10
    {0x140010, 50, 0x15982c8},   // literal, no +0x10
}};

struct RangedVarDtorHooks {
    void (*functor_destroy)(std::uint64_t fn_vtbl, void* functor_block) = nullptr;   // [[obj+0xc0]+0x18](obj+0xa0)
    void (*base_dtor)(void* object) = nullptr;                                       // 0x20b50
};

[[nodiscard]] const RangedVarDtorRow* find_ranged_var_dtor(std::uint64_t rva) noexcept;

// `object` must address kRangedVarObjectSize bytes with the functor vtable tag at +0xc0. Returns false (writing nothing)
// for an RVA outside the five or when a hook is missing.
[[nodiscard]] bool ranged_var_dtor_run(std::uint64_t rva, void* object, const RangedVarDtorHooks& hooks) noexcept;

}  // namespace sotc
