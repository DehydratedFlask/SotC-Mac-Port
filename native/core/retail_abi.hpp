// Retail calling-convention contract for the statically resolved vtable targets.
//
// Every modelled target was lifted as an ordinary C++ function with host cells and hooks. Calling one
// from lifted guest code needs two extra facts: which SysV x86-64 argument registers the body reads,
// and whether it returns a value. This module records that contract per target so a dispatcher can
// marshal a guest call frame instead of guessing.
//
// Derivation: each row was read from the pinned body (which registers are read before being written).
// It is a **contract record**, not an implementation: the row says "this entry takes these registers",
// not "here is a callable native function with guest memory". A target whose body needs guest memory
// (any hook-driven body) still requires the host to supply cells; the frame only fixes the arguments.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

// SysV x86-64 integer argument order used by the lifted bodies.
enum class RetailArg : std::uint8_t { rdi = 0, rsi = 1, rdx = 2, rcx = 3, r8 = 4, r9 = 5 };
inline constexpr std::size_t kRetailArgCount = 6;

struct RetailFrame {
    std::uint64_t args[kRetailArgCount] = {0, 0, 0, 0, 0, 0};
    std::uint64_t rax = 0;  // return value slot

    [[nodiscard]] std::uint64_t get(RetailArg arg) const noexcept {
        return args[static_cast<std::size_t>(arg)];
    }
    void set(RetailArg arg, std::uint64_t value) noexcept {
        args[static_cast<std::size_t>(arg)] = value;
    }
};

struct RetailArgSpec {
    std::uint64_t rva = 0;
    std::uint8_t argument_count = 0;  // consecutive from rdi
    bool returns_value = false;       // body leaves something in rax
    const char* note = "";
};

// Sorted by rva. Argument counts come from the pinned bodies: the registers each body reads before
// writing them.
inline constexpr RetailArgSpec kRetailArgSpecs[] = {
    {0xc2e0, 2, true, "rdi=this, rsi=source; copies one qword"},
    {0xc740, 2, false, "rdi=this, rsi=source; installs a table and copies one qword"},
    {0x13d20, 2, false, "setter: rdi=this, esi=value; returns nothing"},
    {0x187c0, 1, false, "rdi=this; pre-call then tag-1 free; returns nothing"},
    {0x20d60, 1, false, "rdi=this; once-flag gate then tail call; returns nothing"},
    {0x358c0, 2, false, "rdi=this, rsi=source; installs a table and copies one qword"},
    {0x1bf710, 1, false, "rdi=this; pre-call then tag-1 free; returns nothing"},
    {0x2687b0, 1, false, "rdi=this; pre-call then tag-1 free; returns nothing"},
    {0xa40e20, 0, false, "retq: no argument is read"},
    {0xa460f0, 1, false, "rdi=this; teardown with one helper vcall; returns nothing"},
    {0xddf4b0, 1, false, "rdi=this; manager dispatch; returns nothing directly (tail call)"},
    {0xe16a00, 2, false, "rdi=this, rsi=argument; dual vcall, tail call"},
    {0xe19980, 5, true, "rdi=this, rsi=arg2, rdx=arg3, rcx=arg4, r8=arg5; returns rax"},
    {0xe1a5c0, 4, true, "rdi=this, rsi=key, edx=value; returns int status"},
    {0xe21420, 3, true, "rdi=this, rsi=ctx, rdx=name; returns int status"},
    {0xe24900, 2, false, "rdi=this, rsi=out; stores through rsi; returns status in eax"},
    {0xe26f90, 3, true, "rdi=this, rsi=arg2, edx=arg3; returns status in eax"},
};

inline constexpr std::size_t kRetailArgSpecCount =
    sizeof(kRetailArgSpecs) / sizeof(kRetailArgSpecs[0]);

// Returns the row for `rva`, or nullptr when the target has no recorded ABI contract.
[[nodiscard]] const RetailArgSpec* retail_arg_spec_for(std::uint64_t rva) noexcept;

// Convenience: does this target take at least `count` argument registers?
[[nodiscard]] bool retail_takes_at_least(std::uint64_t rva, std::uint8_t count) noexcept;

}  // namespace sotc
