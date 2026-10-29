#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot545Rva = 0xB2EC0;
inline constexpr std::size_t kSlot545Size = 0x295;
inline constexpr std::uint64_t kSlot545MatrixHelperRva = 0xAB860;
inline constexpr std::uint64_t kSlot545DtorRva = 0xB0A10;
inline constexpr std::size_t kSlot545StateSize = 0x2F8;
inline constexpr std::size_t kSlot545ObjectOffset = 0x120;
inline constexpr std::size_t kSlot545ObjectSize = 0x1D0;
inline constexpr std::size_t kSlot545BasisOffset = 0xC8;
inline constexpr std::size_t kSlot545ScalarOffset = 0xE8;

using Slot545Matrix = std::array<float, 16>;  // column-major element order from 0xab860

// Host-owned image of the sparse static writes rooted at original RVA 0x2ed37f8.
// Offsets follow the pinned stores; unnamed bytes stay opaque. Links inside this storage point
// into this host array, never back into the original ELF image.
struct alignas(16) Slot545State {
    std::array<std::byte, kSlot545StateSize> bytes{};
};
static_assert(offsetof(Slot545State, bytes) == 0);
static_assert(sizeof(std::array<std::byte, kSlot545StateSize>) == kSlot545StateSize);

struct Slot545ObjectView {
    std::byte* bytes = nullptr;
};

[[nodiscard]] Slot545ObjectView slot545_object_view(Slot545State& state) noexcept;

enum class Slot545Error : std::uint8_t {
    none,
    missing_atexit_log,
    atexit_record_failed,
    stack_guard_mismatch,
};

struct Slot545Result {
    bool ok = false;
    bool atexit_recorded = false;
    Slot545Error error = Slot545Error::none;
};

enum class Slot545DtorFailure : std::uint8_t {
    none,
    missing_object,
    first_release,
    second_release,
};

struct Slot545DtorResult {
    bool ok = true;
    std::size_t release_attempts = 0;
    Slot545DtorFailure failure = Slot545DtorFailure::none;
};

// Scalar implementation of the pinned AVX dataflow, with per-lane multiply/add order preserved.
// Inputs are snapshotted before output stores, so output may alias either input.
void slot545_matrix_multiply(Slot545Matrix& out, const Slot545Matrix& left,
                             const Slot545Matrix& right) noexcept;

// Ordinary native model of the verified slot body. The guard values represent the observed
// stack-cookie comparison. A missing host log or guard mismatch is fail-visible.
[[nodiscard]] Slot545Result init_slot545_b2ec0(Slot545State& state, StartupAtexitLog* log,
                                                std::uint64_t entry_guard,
                                                std::uint64_t exit_guard) noexcept;

// Models the callback's two conditional tag-2 releases. It never invokes the recorded original
// destructor RVA. A missing or false host allocator hook stops at the failing release stage.
[[nodiscard]] Slot545DtorResult destroy_slot545_object(
    Slot545ObjectView object, const AllocDispatchContract& allocator) noexcept;

}  // namespace sotc
