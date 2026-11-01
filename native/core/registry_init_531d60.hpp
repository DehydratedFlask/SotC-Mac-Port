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
#include "init_531d60_post_base_mov_edx_eax_532b2c_u1.hpp"
#include "init_531d60_post_base_mov_rsi_rbx_532b29_u1.hpp"
#include "init_531d60_post_base_call_52a220_532b2e_u1.hpp"
#include "init_531d60_post_base_lea_rbx_2efd160_532b33_u1.hpp"
#include "init_531d60_post_base_cmp_r12_r14_532b44_u1.hpp"
#include "init_531d60_post_base_je_532bfb_532b47_u1.hpp"
#include "init_531d60_post_base_test_r12_r12_532b4d_u1.hpp"
#include "init_531d60_post_base_je_532bf8_532b50_u1.hpp"
#include "init_531d60_post_base_mov_r14_mem_rsp10_532b56_u1.hpp"
#include "init_531d60_post_base_mov_r13_mem_rsp40_532b5b_u1.hpp"
#include "init_531d60_post_base_mov_r14_mem_rsp48_532b60_u1.hpp"
#include "init_531d60_post_base_cmp_r13_r14_532b65_u1.hpp"
#include "init_531d60_post_base_je_532bba_532b68_u1.hpp"
#include "init_531d60_post_base_mov_r12_rbx_532b41_u1.hpp"
#include "init_531d60_post_base_mov_r13_15fe518_532b3a_u1.hpp"
#include "registry_fill_52a220.hpp"
#include "registry_object_2efd160.hpp"

#include <atomic>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kRegistryInitObjectBytes = 0x98;
inline constexpr std::uint64_t kRegistryInit531d60FragmentPinnedRva = 0x532b7b;

struct RegistryInit531d60Hooks {
    bool merge_mov_edi_98_532b0c_u1_ran = false;
    bool merge_call_17070_532b11_u1_ran = false;
    bool merge_mov_r14_rax_532b16_u1_ran = false;
    bool merge_call_54dc10_532b19_u1_ran = false;
    bool merge_mov_rbx_rax_532b1e_u1_ran = false;
    bool merge_call_54dc20_532b21_u1_ran = false;
    bool merge_mov_r15_rax_532b26_u1_ran = false;
    bool merge_mov_rsi_rbx_532b29_u1_ran = false;
    bool merge_mov_edx_eax_532b2c_u1_ran = false;
    bool merge_call_52a220_532b2e_u1_ran = false;
    bool merge_lea_rbx_2efd160_532b33_u1_ran = false;
    bool merge_mov_r13_15fe518_532b3a_u1_ran = false;
    bool merge_mov_r12_rbx_532b41_u1_ran = false;
    bool merge_cmp_r12_r14_532b44_u1_ran = false;
    bool merge_je_532bfb_532b47_u1_ran = false;
    bool merge_test_r12_r12_532b4d_u1_ran = false;
    bool merge_je_532bf8_532b50_u1_ran = false;
    bool merge_mov_r14_mem_rsp10_532b56_u1_ran = false;
    bool merge_mov_r13_mem_rsp40_532b5b_u1_ran = false;
    bool merge_mov_r14_mem_rsp48_532b60_u1_ran = false;
    bool merge_cmp_r13_r14_532b65_u1_ran = false;
    bool merge_je_532bba_532b68_u1_ran = false;
    bool merge_lea_rsi_rsp10_532b6a_u1_ran = false;
    bool merge_mov_rbx_r13_10_532b70_u1_ran = false;
    bool merge_mov_r15_r13_18_532b74_u1_ran = false;
    bool merge_cmp_r15_r11_532b78_u1_ran = false;
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
