// CPU-01-HELPER-CALLEE-MODELS-01: the argument-functor class tables that the console-variable constructors
// 0xc580 (ranged int), 0x3a980 (unbounded int) and 0x6aa00 (unbounded float) accept from their callers.
// Original ELF sha256 d0723afb...5861d3. Offline analysis only; nothing here executes an original RVA, and the
// table addresses are IDENTITY TAGS (original `.data.rel.ro` addresses kept as numbers).
//
// Why this exists. Each of the three constructors installs the caller's argument functor through
// `call [caller_tag + 8]` (slot 1, "clone") and finishes with `call [caller_tag + 0x18]` at the call site
// (slot 3, "destroy"). The older native models therefore refused every tag they had not byte-read, and each
// new startup slot added one more tag to a private whitelist. The class tables are shared data: a table is
// interchangeable with the lifted one exactly when its four slots are the lifted bodies.
//
//   int family    slots {0xc720 copy, 0xc740 clone, 0xc760 move, 0xc770 bare-ret destroy}
//                 copy and clone store payload (dst+0x10 = src+0x10) and the class tag 0x15944a0 (dst+0x20)
//   float family  slots {0xc4a0, 0xc4c0, 0xc4e0, 0xc4f0}, the same four shapes, class tag 0x1594318
//
// The two tables below are EXHAUSTIVE: every 8-byte-aligned group of four consecutive RELATIVE relocations in
// the image whose addends are exactly one family's four slot RVAs (37 int tables, 68 float tables).
// tools/lift/check_helper_{c580,3a980,6aa00}.py recompute them from the relocation table and require equality
// in both directions, so a table cannot be added to or removed from these arrays without the ELF agreeing.
// What a tag in this set proves: the constructor's functor-install sequence and the call site's destroy call
// behave exactly as the lifted model. What it does NOT prove: that a given call site really passes that tag
// (the slot author pins the site; the three blocked sites 430/504/555 are pinned by the checkers).
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

// Why a helper refused (nothing is written when a helper refuses).
enum class HelperRefusal : std::uint8_t {
    none,
    unadmitted_arg_functor,   // the caller's functor class is not one of the lifted families
    unknown_target_class,     // 0x26db0: the target's class has no lifted slot 3 / slot 6 body
    wrong_helper,             // the request names a shape this helper does not model (e.g. bounded float at 0x6aa00)
};

inline constexpr std::uint64_t kIntFunctorCopyRva = 0xc720;
inline constexpr std::uint64_t kIntFunctorCloneRva = 0xc740;
inline constexpr std::uint64_t kIntFunctorMoveRva = 0xc760;
inline constexpr std::uint64_t kIntFunctorDestroyRva = 0xc770;      // bare ret
inline constexpr std::uint64_t kIntFunctorClassTag = 0x15944a0;     // stored at dst+0x20 by copy and clone

inline constexpr std::uint64_t kFloatFunctorCopyRva = 0xc4a0;
inline constexpr std::uint64_t kFloatFunctorCloneRva = 0xc4c0;
inline constexpr std::uint64_t kFloatFunctorMoveRva = 0xc4e0;
inline constexpr std::uint64_t kFloatFunctorDestroyRva = 0xc4f0;    // bare ret
inline constexpr std::uint64_t kFloatFunctorClassTag = 0x1594318;

inline constexpr std::size_t kIntFunctorTagCount = 37;
inline constexpr std::size_t kFloatFunctorTagCount = 68;

inline constexpr std::array<std::uint64_t, kIntFunctorTagCount> kIntFunctorTags = {
    0x15944a0, 0x1594c70, 0x1595200, 0x1595660,
    0x15958d0, 0x1595e60, 0x1596858, 0x15983e8,
    0x15985d8, 0x15990f8, 0x15992f0, 0x159b468,
    0x159b6a0, 0x159bf40, 0x159c548, 0x159c778,
    0x159fae0, 0x15a0a18, 0x15a2248, 0x15be7f0,
    0x15bfc30, 0x15c6f90, 0x15c8450, 0x15c8490,
    0x15c8e58, 0x15caf38, 0x15cb128, 0x15cb218,
    0x15cb278, 0x15cb2d8, 0x15cbd80, 0x15cdf90,
    0x15ce200, 0x15ce5f0, 0x15ce8c0, 0x15d18a0,
    0x15d1d80,
};

inline constexpr std::array<std::uint64_t, kFloatFunctorTagCount> kFloatFunctorTags = {
    0x1594318, 0x1596878, 0x1597530, 0x15981a0,
    0x1598408, 0x15987f0, 0x1598a50, 0x159b4a8,
    0x159ba68, 0x159bb00, 0x159bf00, 0x159c798,
    0x159c988, 0x15a66b8, 0x15aa880, 0x15adbf8,
    0x15b0e90, 0x15b5770, 0x15bfc50, 0x15c05f0,
    0x15c1bc8, 0x15c27f0, 0x15c8300, 0x15c8e78,
    0x15cb148, 0x15cb1f8, 0x15cb238, 0x15cb2b8,
    0x15cbda0, 0x15cc110, 0x15cc488, 0x15cc528,
    0x15cc568, 0x15cc5a8, 0x15cc618, 0x15cc7f8,
    0x15cc838, 0x15cc878, 0x15cc898, 0x15cc9f8,
    0x15cccf0, 0x15ccd30, 0x15cd018, 0x15cd180,
    0x15cd1e0, 0x15cd240, 0x15cda18, 0x15cdf18,
    0x15cdf70, 0x15ce0b8, 0x15ce1c0, 0x15ce1e0,
    0x15ce240, 0x15ce260, 0x15ce2b8, 0x15ce318,
    0x15ce3f0, 0x15ce440, 0x15ce4a0, 0x15ce610,
    0x15ce630, 0x15ceb80, 0x15cec00, 0x15d10f0,
    0x15d1770, 0x15d2040, 0x15d57e0, 0x15d5e78,
};

// True when `tag` is one of the int / float family class tables above.
[[nodiscard]] bool is_int_functor_tag(std::uint64_t tag) noexcept;
[[nodiscard]] bool is_float_functor_tag(std::uint64_t tag) noexcept;

}  // namespace sotc
