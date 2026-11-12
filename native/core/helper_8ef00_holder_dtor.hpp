// CPU-01-SLOT408-PREREQS: native model of the 23 byte "holder" destructor family (seven byte-identical-but-for-the-rip-disp
// twins; ELF bodies pinned per instruction by tools/lift/check_helper_8ef00.py).
//
//   lea rax, [rip + class 0x1597648] ; add rax, 0x10 ; mov [rdi], rax      ; vptr = class + 0x10 (= 0x1597658)
//   add rdi, 8 ; jmp 0xf590                                                ; shared release of the 0x20 B holder at +8
//
// The object is 0x28 B: the vptr at +0x00 and a `SharedHolderF590` at +0x08 (object +0x08, control +0x20). The destructor
// stores the vptr, then tail-calls the shared release. The four startup-slot dtors named in the survey as "unknown holder
// dtors" (0x34e820, 0x5b5470, 0x5b5490, 0x2c84f0) are twins of 0x8ef00 / 0x2fced0 / 0x2b5050, the three the generator
// already allowlists. The vptr is a guest class TAG (never dereferenced); the release is the native 0xf590 model.
// Nothing here executes an original RVA. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "core/shared_release_f590.hpp"

namespace sotc {

inline constexpr std::size_t kHolderDtorSize = 23;
inline constexpr std::uint64_t kHolderDtorClass = 0x1597648;
inline constexpr std::uint64_t kHolderDtorVptr = 0x1597658;     // class + 0x10
inline constexpr std::size_t kHolderDtorHolderOffset = 0x08;
inline constexpr std::size_t kHolderDtorObjectSize = 0x28;

// The seven twins, in ELF order of discovery.
inline constexpr std::array<std::uint64_t, 7> kHolderDtorTwins = {0x8ef00, 0x2fced0, 0x2b5050, 0x34e820, 0x5b5470, 0x5b5490, 0x2c84f0};

struct HolderObject28 {
    std::uint64_t vptr;          // +0x00  guest class tag
    SharedHolderF590 holder;     // +0x08  0x20 B shared holder (control at +0x20)
};
static_assert(sizeof(HolderObject28) == kHolderDtorObjectSize, "holder object is 0x28 B");

[[nodiscard]] bool is_holder_dtor_twin(std::uint64_t rva) noexcept;

// Stores the vptr and releases the holder. Returns the shared-release result (ok == false when a last-reference release
// lacks a destroy/free hook). All seven twins share this body, so `rva` only selects membership; an unknown RVA returns a
// default (ok == false) result and writes nothing.
[[nodiscard]] SharedReleaseF590Result holder_dtor_run(std::uint64_t rva, HolderObject28& object,
                                                      const SharedReleaseF590Hooks& hooks) noexcept;

}  // namespace sotc
