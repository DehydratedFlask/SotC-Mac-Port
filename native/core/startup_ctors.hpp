// Native replacement for the CRT constructor runner (original RVA 0x20): runs recovered constructors in the
// original list order and FAILS VISIBLY at the first constructor that has not been recovered.
#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <span>

#include "math_constants.hpp"
#include "scratch_reset.hpp"
#include "trig_constants.hpp"
#include "vec_neg036.hpp"
#include "xyz_mask_copy.hpp"
#include "clear_flag32.hpp"
#include "init_f4fd40.hpp"
#include "init_f4eba0.hpp"
#include "init_f4b540.hpp"
#include "vec_ef85e0.hpp"
#include "init_ea52f0.hpp"

namespace sotc {

// Native objects standing in for the original globals the recovered constructors write.
// math_b/c/d: clones of 0xf6cad0 at RVAs 0xf66640 / 0xf53de0 / 0xf3d9d0 (same bits, distinct BSS).
// xyz_mask_b: clone of the unguarded xyz-mask pattern at 0xf50d70 (distinct from 0xf30460's cell).
struct StartupState {
    MathConstants math{};
    MathConstants math_b{};
    MathConstants math_c{};
    MathConstants math_d{};
    TrigConstants trig{};
    VecNeg036 neg036{};
    XyzMaskCopy xyz_mask_copy{};
    XyzMaskCopy xyz_mask_b{};
    InitF4fd40 f4fd40{};
    InitF4eba0 f4eba0{};
    InitF4b540 f4b540{};
    VecEf85e0 vec_ef85e0{};   // idx 20 (peer lift; registered here)
    InitEa52f0 ea52f0{};      // idx 22
    std::array<std::byte, kScratchBytes> scratch{};
    std::atomic<std::uint64_t> scratch_slot{0};
    std::atomic<std::uint32_t> flag32{0};
};

struct CtorSlot {
    std::uint32_t original_rva;               // identity only; never used as an address
    std::uint32_t original_index;             // position in the original 573-entry list (runs 0 -> 572)
    void (*run)(StartupState&) noexcept;      // nullptr == not recovered yet
};

struct CtorRunResult {
    std::size_t completed = 0;
    std::uint32_t blocked_on_rva = 0;         // 0 when every slot ran
    bool ok() const noexcept { return blocked_on_rva == 0; }
};

CtorRunResult run_constructors(std::span<const CtorSlot> in_order, StartupState& state) noexcept;

// The recovered subset in original relative order. NOT a startup: 537 unrecovered constructors run between
// its two entries in the original, so a faithful run needs every slot between them filled first.
std::span<const CtorSlot> recovered_constructors() noexcept;

}  // namespace sotc
