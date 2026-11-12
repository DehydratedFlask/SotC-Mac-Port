// CPU-01-SLOT408-PREREQS: native model of 0xf7b0 (73 B, the `SharedStr` destructor named in startup slots 309 / 316 and the
// release startup slot 454 registers at 0x48432a). Its ELF body is the shared release 0xf590 byte for byte except the rel32
// displacements of its two allocator transfers (call 0x12af0 at +0x29, jmp 0x170b0 at +0x3d), so it has exactly the
// behaviour of shared_release_f590. tools/lift/check_helper_f7b0.py pins the body per instruction AND proves the twin
// relation (the only differing bytes are the low displacement bytes of those two transfers).
#pragma once

#include <cstddef>
#include <cstdint>

#include "core/shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kSharedReleaseTwinF7b0Rva = 0xf7b0;
inline constexpr std::size_t kSharedReleaseTwinF7b0Size = 73;

[[nodiscard]] SharedReleaseF590Result shared_release_f7b0(const SharedHolderF590& holder,
                                                          const SharedReleaseF590Hooks& hooks) noexcept;

}  // namespace sotc
