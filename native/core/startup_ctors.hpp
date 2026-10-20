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
#include "vec_e0b180.hpp"
#include "vec_e085e0.hpp"
#include "vec_e05320.hpp"
#include "vec_e026b0.hpp"
#include "vec_de1ad0.hpp"
#include "vec_ddef00.hpp"
#include "vec_de7fb0.hpp"
#include "vec_de6d20.hpp"
#include "vec_dfd5f0.hpp"
#include "vec_ddab00.hpp"
#include "vec_dda990.hpp"
#include "vec_dd41e0.hpp"
#include "vec_b52110.hpp"
#include "vec_b743c0.hpp"
#include "vec_dc2d50.hpp"
#include "vec_dbb280.hpp"
#include "vec_db93a0.hpp"
#include "vec_db5dd0.hpp"
#include "vec_db7e60.hpp"
#include "vec_db3a70.hpp"
#include "vec_db3960.hpp"
#include "vec_d7be60.hpp"
#include "vec_d7bc60.hpp"
#include "vec_d7b930.hpp"
#include "vec_d6e0f0.hpp"
#include "vec_d6dbf0.hpp"
#include "vec_d6d890.hpp"
#include "vec_d6d280.hpp"
#include "vec_d6a520.hpp"
#include "vec_d62b00.hpp"
#include "vec_d62690.hpp"
#include "vec_d623c0.hpp"
#include "vec_d5fbe0.hpp"
#include "vec_d47880.hpp"
#include "vec_d47420.hpp"
#include "vec_d47270.hpp"
#include "vec_d42b60.hpp"
#include "vec_d40ea0.hpp"
#include "vec_d405a0.hpp"
#include "vec_d40010.hpp"
#include "vec_d3b790.hpp"
#include "vec_d7c380.hpp"
#include "vec_d7c4d0.hpp"
#include "vec_d7c9d0.hpp"
#include "vec_d7e640.hpp"
#include "vec_d7fe40.hpp"
#include "vec_d7fe90.hpp"
#include "vec_d80e60.hpp"
#include "vec_d84ee0.hpp"
#include "vec_dac410.hpp"
#include "vec_d853a0.hpp"
#include "vec_d90310.hpp"
#include "vec_d94cb0.hpp"
#include "vec_dc4180.hpp"
#include "vec_dddc90.hpp"

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
    VecE0b180 vec_e0b180{};   // idx 49
    VecE085e0 vec_e085e0{};   // idx 51
    VecE05320 vec_e05320{};   // idx 52
    VecE026b0 vec_e026b0{};   // idx 53
    VecDfd5f0 vec_dfd5f0{};   // idx 54
    VecDe7fb0 vec_de7fb0{};   // idx 56
    VecDe6d20 vec_de6d20{};   // idx 57
    VecDe1ad0 vec_de1ad0{};   // idx 58
    VecDdef00 vec_ddef00{};   // idx 60
    VecDddc90 vec_dddc90{};   // idx 61
    VecDdab00 vec_ddab00{};   // idx 62
    VecDda990 vec_dda990{};   // idx 63
    VecDd41e0 vec_dd41e0{};   // idx 67
    VecDc4180 vec_dc4180{};   // idx 68
    VecDc2d50 vec_dc2d50{};   // idx 69
    VecDbb280 vec_dbb280{};   // idx 71
    VecDb93a0 vec_db93a0{};   // idx 72
    VecDb7e60 vec_db7e60{};   // idx 73
    VecDb5dd0 vec_db5dd0{};   // idx 74
    VecDb3a70 vec_db3a70{};   // idx 75
    VecDb3960 vec_db3960{};   // idx 76
    VecDac410 vec_dac410{};   // idx 77
    VecD94cb0 vec_d94cb0{};   // idx 78
    VecD90310 vec_d90310{};   // idx 79
    VecD853a0 vec_d853a0{};   // idx 83
    VecD84ee0 vec_d84ee0{};   // idx 84
    VecD80e60 vec_d80e60{};   // idx 85
    VecD7fe90 vec_d7fe90{};   // idx 86
    VecD7fe40 vec_d7fe40{};   // idx 87
    VecD7e640 vec_d7e640{};   // idx 88
    VecD7c9d0 vec_d7c9d0{};   // idx 89
    VecD7c4d0 vec_d7c4d0{};   // idx 90
    VecD7c380 vec_d7c380{};   // idx 91
    VecD7be60 vec_d7be60{};   // idx 92
    VecD7bc60 vec_d7bc60{};   // idx 93
    VecD7b930 vec_d7b930{};   // idx 94
    VecD6e0f0 vec_d6e0f0{};   // idx 96
    VecD6dbf0 vec_d6dbf0{};   // 0xd6dbf0 idx 97
    VecD6d890 vec_d6d890{};   // 0xd6d890 idx 98
    VecD6d280 vec_d6d280{};   // 0xd6d280 idx 99
    VecD6a520 vec_d6a520{};   // 0xd6a520 idx 100
    VecD62b00 vec_d62b00{};   // 0xd62b00 idx 101
    VecD62690 vec_d62690{};   // 0xd62690 idx 102
    VecD623c0 vec_d623c0{};   // 0xd623c0 idx 103
    VecD5fbe0 vec_d5fbe0{};   // 0xd5fbe0 idx 104
    VecD47880 vec_d47880{};   // 0xd47880 idx 106
    VecD47420 vec_d47420{};   // 0xd47420 idx 107
    VecD47270 vec_d47270{};   // 0xd47270 idx 108
    VecD42b60 vec_d42b60{};   // 0xd42b60 idx 109
    VecD40ea0 vec_d40ea0{};   // 0xd40ea0 idx 110
    VecD405a0 vec_d405a0{};   // 0xd405a0 idx 111
    VecD40010 vec_d40010{};   // 0xd40010 idx 112
    VecD3b790 vec_d3b790{};   // 0xd3b790 idx 113
    VecB743c0 vec_b743c0{};   // idx 282
    VecB52110 vec_b52110{};   // idx 290
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

// Recovered subset in original index order. Gaps (incl. 10–19 malloc, 21, 50) remain visible blockers.
std::span<const CtorSlot> recovered_constructors() noexcept;

}  // namespace sotc
