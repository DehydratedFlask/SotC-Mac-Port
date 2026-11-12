// CPU-01-HELPER-CALLEE-MODELS-04: the BOOL argument-functor class tables that the bool console-variable constructor 0x274730 (and the inlined bool
// constructors of the debug-page slots) accept from their callers. Original ELF sha256 d0723afb...5861d3. Offline analysis only; nothing here
// executes an original RVA, and the table addresses are IDENTITY TAGS (original `.data.rel.ro` addresses kept as numbers).
//
// Same construction as helper_functor_tags.hpp (int / float families): a class table is interchangeable with the lifted one exactly when its four
// RELATIVE-relocation words are the four lifted slot bodies. The bool family is
//   slots {0xc2a0 copy, 0xc2c0 clone, 0xc2e0 move, 0xc2f0 bare-ret destroy}
//   copy (0xc2a0) and clone (0xc2c0) store payload (dst+0x10 = src+0x10) and the class tag 0x1594280 (dst+0x20); move (0xc2e0) copies the payload only.
// The table below is EXHAUSTIVE: every 8-byte-aligned group of four consecutive RELATIVE relocations in the image whose addends are exactly those
// four RVAs (143 tables). tools/lift/check_helper_274730.py recomputes it from the relocation table and requires equality in both directions.
// What a tag in this set proves: the functor-install sequence and the call site's `call [tag + 0x18]` behave as modelled. It does NOT prove which
// tag a given call site passes (the per-site checks pin that).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kBoolFunctorCopyRva = 0xc2a0;
inline constexpr std::uint64_t kBoolFunctorCloneRva = 0xc2c0;
inline constexpr std::uint64_t kBoolFunctorMoveRva = 0xc2e0;
inline constexpr std::uint64_t kBoolFunctorDestroyRva = 0xc2f0;     // bare ret
inline constexpr std::uint64_t kBoolFunctorClassTag = 0x1594280;    // stored at dst+0x20 by copy and clone (also the first family table)

inline constexpr std::size_t kBoolFunctorTagCount = 143;

inline constexpr std::array<std::uint64_t, kBoolFunctorTagCount> kBoolFunctorTags = {
    0x1594280, 0x15949f0, 0x1595048, 0x15951e0,
    0x1595640, 0x1595770, 0x15958b0, 0x1596898,
    0x1597680, 0x1598020, 0x1598330, 0x15985b8,
    0x1598810, 0x1598f50, 0x15991b0, 0x1599288,
    0x159aba0, 0x159b170, 0x159b488, 0x159b500,
    0x159ba88, 0x159bae0, 0x159bc00, 0x159bee0,
    0x159c318, 0x159c528, 0x159c758, 0x159c9a8,
    0x159ca60, 0x159e430, 0x15a52f8, 0x15a66d8,
    0x15a6eb0, 0x15a7260, 0x15a77b0, 0x15a7c50,
    0x15a93e8, 0x15a9850, 0x15aa860, 0x15adbd8,
    0x15b0fc8, 0x15b1558, 0x15b1828, 0x15b18c8,
    0x15b5198, 0x15b5750, 0x15b70d0, 0x15bab98,
    0x15bad48, 0x15bc748, 0x15bddb8, 0x15be810,
    0x15bfc10, 0x15c0798, 0x15c0c30, 0x15c14f0,
    0x15c1950, 0x15c1be8, 0x15c2400, 0x15c27d0,
    0x15c2c20, 0x15c4a50, 0x15c59a0, 0x15c6498,
    0x15c6fb0, 0x15c8248, 0x15c8470, 0x15c8db8,
    0x15c8e98, 0x15ca5a8, 0x15ca808, 0x15cade0,
    0x15caf18, 0x15cb168, 0x15cb188, 0x15cb1d8,
    0x15cb258, 0x15cb298, 0x15cbdc0, 0x15cc078,
    0x15cc1c8, 0x15cc2f8, 0x15cc448, 0x15cc468,
    0x15cc4e8, 0x15cc548, 0x15cc588, 0x15cc5c8,
    0x15cc650, 0x15cc670, 0x15cc690, 0x15cc6f8,
    0x15cc748, 0x15cc7b8, 0x15cc7d8, 0x15cc818,
    0x15cc858, 0x15cc998, 0x15cc9b8, 0x15cc9d8,
    0x15ccd10, 0x15ccff8, 0x15cd160, 0x15cd1a0,
    0x15cd1c0, 0x15cd200, 0x15cd220, 0x15cd9f8,
    0x15cdf50, 0x15cdfb0, 0x15ce050, 0x15ce098,
    0x15ce1a0, 0x15ce220, 0x15ce2d8, 0x15ce2f8,
    0x15ce390, 0x15ce3b0, 0x15ce3d0, 0x15ce460,
    0x15ce480, 0x15ce5d0, 0x15ce650, 0x15ce8e0,
    0x15ce900, 0x15ce920, 0x15cea30, 0x15ceba0,
    0x15cebc0, 0x15cebe0, 0x15cec20, 0x15cec90,
    0x15d1220, 0x15d1750, 0x15d17b0, 0x15d19b0,
    0x15d1c10, 0x15d1da0, 0x15d1dc0, 0x15d2060,
    0x15d2478, 0x15d5070, 0x15d5800,
};

// True when `tag` is one of the bool family class tables above.
[[nodiscard]] bool is_bool_functor_tag(std::uint64_t tag) noexcept;

}  // namespace sotc
