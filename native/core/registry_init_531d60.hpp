// Init `0x531d60` registry fragment only (OBS 0x532b0c..0x532bf8):
//   obj = new_(0x98) via 0x17070
//   fill via 0x52a220(obj, 0x54dc10(), 0x54dc20()=275)
//   destroy previous *(0x2efd160) if any (free A/B/+0x40 vectors + delete_ object)
//   store obj into global cell modelling 0x2efd160
//
// ELF sha256 d0723afb...5861d3. Rest of Init (3890 B) is NOT modelled — no leaf-vec, not
// registered into recovered_constructors(). Listener *slot* dtors inside +0x40 groups are
// NOT walked (fill leaves listeners null; if a non-null begin is present, native frees the
// array storage only). Private I275 names remain fail-visible via the fill path.
#pragma once

#include "allocator_front.hpp"
#include "input_table_551140.hpp"
#include "init_531d60_post_base_call_17070_532b11_u1.hpp"
#include "init_531d60_post_base_call_54dc10_532b19_u1.hpp"
#include "init_531d60_post_base_mov_edi_98_532b0c_u1.hpp"
#include "init_531d60_post_base_mov_r14_rax_532b16_u1.hpp"
#include "init_531d60_post_base_call_54dc20_532b21_u1.hpp"
#include "init_531d60_post_base_mov_r15_rax_532b26_u1.hpp"
#include "init_531d60_post_base_mov_rbx_rax_532b1e_u1.hpp"
#include "registry_fill_52a220.hpp"
#include "registry_object_2efd160.hpp"

#include <atomic>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kRegistryInitObjectBytes = 0x98;
inline constexpr std::uint64_t kRegistryInit531d60FragmentPinnedRva = 0x532b29;

struct RegistryInit531d60Hooks {
    bool merge_mov_edi_98_532b0c_u1_ran = false;
    bool merge_call_17070_532b11_u1_ran = false;
    bool merge_mov_r14_rax_532b16_u1_ran = false;
    bool merge_call_54dc10_532b19_u1_ran = false;
    bool merge_mov_rbx_rax_532b1e_u1_ran = false;
    bool merge_call_54dc20_532b21_u1_ran = false;
    bool merge_mov_r15_rax_532b26_u1_ran = false;
};

enum class RegistryInit531d60Status : std::uint8_t {
    ok = 0,
    missing_names = 1,
    alloc_failed = 2,
    fill_failed = 3,
};

// Process-wide cell modelling original global 0x2efd160 (unique_ptr-like qword).
[[nodiscard]] std::atomic<RegistryObject2efd160*>& registry_cell_2efd160() noexcept;

// Destroy *cell (A/B/+0x40 array frees + delete_) and store nullptr. Safe if already null.
void destroy_registry_at_2efd160(AllocatorFront& alloc) noexcept;

// Alloc + fill + install. On missing_names/alloc/fill failure, cell is left unchanged
// (and any freshly allocated obj is destroyed). Requires name pointers on `table`.
[[nodiscard]] RegistryInit531d60Status init_registry_fragment_531d60(InputTable551140& table, AllocatorFront& alloc, const RegistryInit531d60Hooks& hooks = {});

}  // namespace sotc
