// CPU-01-AOT-CFG-SCALE-01-BATCH27: native model of the unmodelled functor-command constructor pair
// 0x6af60 (397 B, FDE [0x6af60, 0x6b0ed)) and 0x6b0f0 (352 B, FDE [0x6b0f0, 0x6b250)). Original
// ELF sha256 d0723afb...5861d3. Slot 462 calls 0x6af60 at 0x3e2c45 for the 0xd0-byte command at
// 0x2ef76a0; the pair was not first-class before this batch.
//
// ---- 0x6b0f0: base + type-erased functor tail (rdi=obj, rsi=name, rdx=spec, rcx=&low,
//      r8=&high, r9d=flags, [rbp+0x10]=functor view, [rbp+0x18]=description) ---------------
//   0x20af0(obj, name, description, flags)                      call at 0x6b126
//   vptr = 0x1595f08 (0x1595ef8 + 0x10)                          0x6b12b..0x6b141
//   +0x60 = spec                                                 0x6b144
//   +0x6c = byte 0 then dword *low                              0x6b148/0x6b153
//   +0x70 = byte 1                                               0x6b14c
//   +0x74 = byte 0 then dword *high                             0x6b156/0x6b161
//   +0x78 = byte 1                                               0x6b15a
//   +0x80..0x9f = 0                                              0x6b164
//   +0xc0 = 0x1596950, +0xb0 = 0, +0xc8 = 0                      0x6b16c..0x6b17e
//   functor view (+0x00 qword, +0x08/+0x10 qwords copied as 16 bytes, +0x18 qword):
//       +0x80 = view.q0, +0x88..0x97 = view.q8, +0x90 = view.q10, +0x98 = view.q18
//       0x6b189..0x6b1a6; if view.q18 != 0 then +0x80 = &+0x80 (self pointer, 0x6b1b2..0x6b1b9)
//   five indirect calls: view vtable [+0x40]+8 copy -> temp1; temp1 slot 0 clone -> temp2;
//       obj functor vtable [+0xc0] slot 0 construct +0xa0 from temp1; temp2 slot 0 assign +0xa0;
//       temp1 vtable [+0x18] destroy. 0x6b1d6, 0x6b1e7, 0x6b1fd, 0x6b209, 0x6b228. Their targets
//       are runtime contract (host hook); the native model keeps the observable stores.
//   0x6b20b..0x6b220: +0xc8 = view.tag48 (the old +0xc8 is saved and swapped into the temp).
//   canary 0x15fe518; fail 0xf8f3a0 at 0x6b24a.
//
// ---- 0x6af60: the outer wrapper (rdi=obj, rsi=name, rdx=spec, rcx=&low, r8=&high, r9d=flags,
//      [rbp+0x10]=view, [rbp+0x18]=description) ---------------------------------------
//   It copies the view to its own stack and performs the same five calls, then calls 0x6b0f0 at
//   0x6b093 with the view and description, destroys its temp, and finally:
//      vptr = 0x1596980 (0x1596970 + 0x10)                      0x6b0a9..0x6b0b7
//      +0x68 = dword [spec]                                     0x6b0ba/0x6b0bd
//      0x20d60(obj)                                             0x6b0c0
//   canary 0x15fe518; fail 0xf8f3a0 at 0x6b0e7.
//
// INFERENCE: the class is a console command with a type-erased functor tail; the image has no
// symbols. The 397 B body has three trailing nops after its stack-check branch (0x6b0ed..0x6b0ef).
// 0x20d60 is the already-lifted register-once entry (concmd_base). Vptr values are identity tags;
// the five type-erased calls are the only runtime contract and are never executed offline.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kConCmdFunctorInnerRva = 0x6b0f0;
inline constexpr std::size_t kConCmdFunctorInnerSize = 352;
inline constexpr std::uint64_t kConCmdFunctorOuterRva = 0x6af60;
inline constexpr std::size_t kConCmdFunctorOuterSize = 397;
inline constexpr std::uint64_t kConCmdFunctorInnerVptr = 0x1595f08;   // vtable 0x1595ef8
inline constexpr std::uint64_t kConCmdFunctorFinalVptr = 0x1596980;   // vtable 0x1596970
inline constexpr std::uint64_t kConCmdFunctorTagVptr = 0x1596950;
inline constexpr std::uint64_t kConCmdFunctorSpecRva = 0x2ea8ee8;     // .bss, zero word in this image
inline constexpr std::uint32_t kConCmdFunctorSpecWord = 0;
inline constexpr std::uint64_t kConCmdFunctorRegisterRva = 0x20d60;
inline constexpr std::uint64_t kConCmdFunctorCanaryCell = 0x15fe518;
inline constexpr std::uint64_t kConCmdFunctorStackChkFailRva = 0xf8f3a0;

// The caller-built view consumed by 0x6b0f0/0x6af60. Slot 462 builds it at rsp+0x80..0xcf.
struct ConCmdFunctorView {
    std::uint64_t q0;          // +0x00
    std::uint64_t q8;          // +0x08
    std::uint64_t q10;         // +0x10
    std::uint64_t q18;         // +0x18; non-zero turns +0x80 into a self pointer
    std::uint8_t payload[0x20]; // +0x20..0x3f source of the first type-erased copy
    std::uint64_t vtable;      // +0x40 identity tag (0x15b18e8 at slot 462)
    std::uint64_t tag48;       // +0x48 stored at object +0xc8
};
static_assert(sizeof(ConCmdFunctorView) == 0x50 && offsetof(ConCmdFunctorView, payload) == 0x20 &&
              offsetof(ConCmdFunctorView, vtable) == 0x40 && offsetof(ConCmdFunctorView, tag48) == 0x48);

// 0x2ef76a0: the 0xd0-byte functor command built by 0x6af60.
struct ConCmdFunctor {
    ConCmdBase base;           // +0x00
    std::uint64_t spec;        // +0x60
    std::uint32_t spec_word;   // +0x68 (outer only)
    std::uint32_t low;         // +0x6c
    std::uint8_t one70;        // +0x70
    std::uint8_t gap71[3];
    std::uint32_t high;        // +0x74
    std::uint8_t one78;        // +0x78
    std::uint8_t gap79[7];
    std::uint64_t view0;       // +0x80
    std::uint64_t view8;       // +0x88
    std::uint64_t view10;      // +0x90
    std::uint64_t view18;      // +0x98
    std::uint64_t region[4];   // +0xa0..0xbf functor inline buffer (contract-written)
    std::uint64_t tag_c0;      // +0xc0 = 0x1596950
    std::uint64_t tag_c8;      // +0xc8 = view.tag48
};
static_assert(sizeof(ConCmdFunctor) == 0xd0 && offsetof(ConCmdFunctor, spec) == 0x60 &&
              offsetof(ConCmdFunctor, spec_word) == 0x68 && offsetof(ConCmdFunctor, low) == 0x6c &&
              offsetof(ConCmdFunctor, high) == 0x74 && offsetof(ConCmdFunctor, view0) == 0x80 &&
              offsetof(ConCmdFunctor, region) == 0xa0 && offsetof(ConCmdFunctor, tag_c0) == 0xc0 &&
              offsetof(ConCmdFunctor, tag_c8) == 0xc8);

// 0x6af60 reads only the first dword of its rdx spec; slot 462's spec is zero-fill .bss.
struct ConCmdFunctorSpec {
    const void* address = nullptr;
    std::uint32_t word = 0;
};

// The five type-erased calls in body order: 0 copy, 1 clone, 2 construct, 3 assign, 4 destroy.
// A null hook leaves the observable stores and reports transfers = 0 (the original targets are
// runtime state, exactly as in slot 454's functor contract).
struct ConCmdFunctorHooks {
    void* context = nullptr;
    bool (*transfer)(void* context, ConCmdFunctor& object, unsigned stage) noexcept = nullptr;
};

struct ConCmdFunctorResult {
    bool ok = false;
    bool registered = false;
    std::size_t transfers = 0;
};

// 0x6b0f0, without the outer vptr/spec-word/registration stage.
[[nodiscard]] ConCmdFunctorResult init_concmd_functor_6b0f0(
    ConCmdFunctor& object, const SharedStr& name, const SharedStr& description, std::uint32_t flags,
    std::uint32_t low, std::uint32_t high, const ConCmdFunctorSpec& spec,
    const ConCmdFunctorView& view, const ConCmdFunctorHooks& hooks = {}) noexcept;

// 0x6af60: 0x6b0f0 then vptr 0x1596980, +0x68 = spec.word, 0x20d60.
[[nodiscard]] ConCmdFunctorResult init_concmd_functor_6af60(
    ConCmdFunctor& object, const SharedStr& name, const SharedStr& description, std::uint32_t flags,
    std::uint32_t low, std::uint32_t high, const ConCmdFunctorSpec& spec,
    const ConCmdFunctorView& view, ConCmdRegistry& registry,
    const ConCmdFunctorHooks& hooks = {}) noexcept;

}  // namespace sotc
