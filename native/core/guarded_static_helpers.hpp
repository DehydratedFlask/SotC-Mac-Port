// Native recovery of two small lazily-initialised-static helpers and the two constructors that call
// them (ELF sha256 d0723afb...5861d3; offline analysis only; names are ours, the original has none):
//
//   0x51230  : `xor eax,eax; xor edx,edx; ret`           -> returns the 16-byte pair {0, 0} in rax:rdx
//   0xa77960 : function-local static pair at 0x32fb190 (guard byte 0x32fb1a0): on first use calls
//              0x51230 and stores rax/rdx; always returns the address of the pair
//   0x78c760 : ctor, calls 0xa77960 and copies the 16 bytes with one vmovups to BSS cell 0x3256378
//   0x268d40 : function-local static byte at 0x2eeec88 (guard byte 0x2eeec90): on first use stores 0xff;
//              always returns the byte (al)
//   0x268ce0 : ctor, calls 0x268d40 and stores al to BSS cell 0x2eeebac
//
// The original guard is the Itanium C++ ABI thread-safe static guard: the two PLT thunks 0xf8f3d0 /
// 0xf8f3e0 resolve (by NID, checked with tools/lift/check_f78c760.py / check_f268ce0.py) to
// __cxa_guard_acquire / __cxa_guard_release, i.e. the C++ runtime's static-init protocol, NOT game
// behaviour. The native form therefore uses an ordinary C++ function-local static (the compiler emits
// the same thread-safe guard); the lift has no Sony-module dependency. The original statics are
// shared by several compiler-emitted copies of the same inline function (a static scan found writers
// only in such copies, all storing the same value); the native lift has one definition of each.
//
// Limits: the consumers of cells 0x3256378 and 0x2eeebac are not analysed; 0x2eeebac is also touched
// by other code (not analysed). The static pair/byte are exposed read-only because no code seen by the
// scan writes anything other than the initial value. Not registered in startup_ctors.
#pragma once
#include <cstdint>

namespace sotc {

struct Pair128 {
    std::uint64_t lo;   // original rax  -> cell 0x32fb190
    std::uint64_t hi;   // original rdx  -> cell 0x32fb198
};
static_assert(sizeof(Pair128) == 16);

// 0x51230. Pure; no state.
Pair128 pair_zero_51230() noexcept;

// 0xa77960. First call initialises the static from pair_zero_51230(); every call returns the same
// address. Thread-safe (compiler-emitted guard).
const Pair128& static_pair_a77960() noexcept;

// 0x268d40. First call initialises the static byte to 0xff; every call returns it.
std::uint8_t static_flag_268d40() noexcept;

// 0x78c760: copies the 16-byte static pair into `cell`. Unguarded in the original (rewrites each call).
void init_ctor_78c760(Pair128& cell) noexcept;

// 0x268ce0: stores the static flag byte into `cell`. Unguarded in the original.
void init_ctor_268ce0(std::uint8_t& cell) noexcept;

}  // namespace sotc
