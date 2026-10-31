// Measured prologue early+tail with lifted 0x3be050 + lifted 0x1ab550 → registry.
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_ctor_98a90_slice.hpp"
#include "core/container_1bdb10.hpp"
#include "core/ctor_40_1ab550.hpp"
#include "core/ctor_98a90_3be050.hpp"
#include "core/ensure_capacity_3c2f70.hpp"
#include "core/init_sub_3be930.hpp"
#include "core/input_table_551140.hpp"
#include "core/inputtable_names_fixture.hpp"
#include "core/mid_255340.hpp"
#include "core/singleton_f08_531d84.hpp"
#include "core/vslot_40_533100_gfx_2ee2d00.hpp"
#include "core/vslot_40_533100_header.hpp"
#include "core/vslot_40_533100_join.hpp"
#include "core/vslot_40_533100_miss.hpp"
#include "core/vslot_40_533100_post.hpp"
#include "core/platform_tag_147e20.hpp"
#include "core/resource_pool.hpp"
#include "core/vslot_40_533100_attach_common.hpp"
#include "core/vslot_40_533100_shadow_attach.hpp"
#include "core/vslot_40_533100_shadowmodels.hpp"
#include "core/vslot_40_533100_shadowskels.hpp"
#include "core/vslot_40_533100_skels_attach.hpp"
#include "core/vslot_40_533100_shadowanims.hpp"
#include "core/vslot_40_533100_anims_attach.hpp"
#include "core/vslot_40_533100_shadowbanks.hpp"
#include "core/vslot_40_533100_banks_attach.hpp"
#include "core/vslot_40_533100_shadowstreams.hpp"
#include "core/vslot_40_533100_streams_attach.hpp"
#include "core/vslot_40_533100_shadowrandomevents.hpp"
#include "core/vslot_40_533100_randomevents_attach.hpp"
#include "core/vslot_40_533100_shadowbossmusic.hpp"
#include "core/vslot_40_533100_bossmusic_attach.hpp"
#include "core/vslot_40_533100_cp3characters.hpp"
#include "core/vslot_40_533100_cp3characters_attach.hpp"
#include "core/vslot_40_533100_cp3shadowprecache.hpp"
#include "core/vslot_40_533100_cp3shadowprecache_attach.hpp"
#include "core/vslot_40_533100_tail_140d90.hpp"
#include "core/vslot_40_533100_stackchk.hpp"
#include "core/init_531d60_post_base_499f80.hpp"
#include "core/init_531d60_post_base_56dcb0.hpp"
#include "core/init_531d60_post_base_5c64c0.hpp"
#include "core/init_531d60_post_base_1290.hpp"
#include "core/init_531d60_post_base_56db70.hpp"
#include "core/init_531d60_post_base_5b3d00.hpp"
#include "core/init_531d60_post_base_5b3cf0.hpp"
#include "core/init_531d60_post_base_avx_bss.hpp"
#include "core/init_531d60_post_base_7f7220.hpp"
#include "core/init_531d60_post_base_56fad0.hpp"
#include "core/init_531d60_post_base_f90310.hpp"
#include "core/init_531d60_post_base_ymm_bss.hpp"
#include "core/init_531d60_post_base_ymm_intercell.hpp"
#include "core/init_531d60_post_base_xmm_seed.hpp"
#include "core/init_531d60_post_base_ymm_reload.hpp"
#include "core/init_531d60_post_base_26800_f590.hpp"
#include "core/init_531d60_post_base_sbj.hpp"
#include "core/init_531d60_post_base_sboj.hpp"
#include "core/init_531d60_post_base_52eb50.hpp"
#include "core/init_531d60_post_base_lea_vmovq.hpp"
#include "core/init_531d60_post_base_532ca0.hpp"
#include "core/init_531d60_post_base_vcall18.hpp"
#include "core/init_531d60_post_base_continue_a.hpp"
#include "core/init_531d60_post_base_1e4c0_u1.hpp"
#include "core/init_531d60_post_base_4c4950_u1.hpp"
#include "core/init_531d60_post_base_a52b70_u1.hpp"
#include "core/init_531d60_post_base_a599e0_u1.hpp"
#include "core/init_531d60_post_base_continue_b.hpp"
#include "core/init_531d60_post_base_continue_c.hpp"
#include "core/init_531d60_post_base_mov_edi_98_532b0c_u1.hpp"
#include "core/init_531d60_post_base_registry_tail.hpp"
#include "core/capture_a473b0.hpp"
#include "core/prologue_255180.hpp"
#include "core/prologue_gates_2552d4.hpp"
#include "core/registry_init_531d60.hpp"
#include "core/shared_release_f590.hpp"
#include "core/store_tsc_323d0.hpp"
#include "core/vslot_30_534130.hpp"

#define CHECK(c)                                                                         \
    do {                                                                                 \
        if (!(c)) {                                                                      \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);              \
            std::exit(1);                                                                \
        }                                                                                \
    } while (0)

void run_alt_5327bd_tests();
void run_alt_join_route_tests();

using namespace sotc;

namespace {

struct FakeHeap : IAllocator {
    FakeHeap(PageOwnerMap& o, std::uint32_t id_in) : owners(o), id(id_in) {}
    ~FakeHeap() override {
        for (void* p : live) {
            owners.remove(reinterpret_cast<std::uintptr_t>(p));
            std::free(p);
        }
    }
    void* allocate(std::size_t size, std::size_t align, const AllocDesc&) override {
        void* p = nullptr;
        if (posix_memalign(&p, align < sizeof(void*) ? sizeof(void*) : align, size) != 0) {
            return nullptr;
        }
        std::memset(p, 0xA5, size);
        CHECK(owners.add(reinterpret_cast<std::uintptr_t>(p), size, id) == PageOwnerMap::Status::ok);
        live.push_back(p);
        ++alloc_count;
        bytes += size;
        return p;
    }
    void deallocate(void* p, const AllocDesc&) override {
        for (std::size_t i = 0; i < live.size(); ++i) {
            if (live[i] == p) {
                owners.remove(reinterpret_cast<std::uintptr_t>(p));
                std::free(p);
                live.erase(live.begin() + static_cast<std::ptrdiff_t>(i));
                return;
            }
        }
        CHECK(false);
    }
    bool recover_from_failure() override { return false; }

    PageOwnerMap& owners;
    std::uint32_t id;
    std::vector<void*> live;
    std::uint64_t alloc_count = 0;
    std::uint64_t bytes = 0;
};

int g_nonnull = 0;
int g_f590 = 0;
int g_init_tb = 0;
int g_move = 0;
int g_ctor_a30 = 0;
int g_dtor_98a90 = 0;
int g_dtor_3b40 = 0;
int g_dtor_early = 0;
int g_mid_reg = 0;
int g_mid_b60 = 0;
int g_f08_ctor = 0;
int g_f08_dtor = 0;
int g_v40_ctor = 0;
int g_v40_525080 = 0;
int g_v40_2569d0 = 0;
int g_v40_21620 = 0;
int g_v40_ba40 = 0;
int g_v40_miss_release_override = 0;
int g_v40_42390 = 0;
int g_v40_2779a0 = 0;
int g_v40_join_release_override = 0;
int g_v40_join_element = 0;
int g_v40_6a5370 = 0;
int g_v40_6abcc0 = 0;
int g_v40_14c2f0 = 0;
int g_v40_shadow_load = 0;
int g_v40_14ccc0 = 0;
int g_v40_skels_notify = 0;
int g_v40_skels_14c2f0 = 0;
int g_v40_skels_load = 0;
int g_v40_skels_14ccc0 = 0;
int g_v40_anims_notify = 0;
int g_v40_anims_14c2f0 = 0;
int g_v40_anims_load = 0;
int g_v40_anims_14ccc0 = 0;
int g_v40_banks_notify = 0;
int g_v40_banks_14c2f0 = 0;
int g_v40_banks_load = 0;
int g_v40_banks_14ccc0 = 0;
int g_v40_streams_notify = 0;
int g_v40_streams_14c2f0 = 0;
int g_v40_streams_load = 0;
int g_v40_streams_14ccc0 = 0;
int g_v40_rand_notify = 0;
int g_v40_rand_14c2f0 = 0;
int g_v40_rand_load = 0;
int g_v40_rand_14ccc0 = 0;
int g_v40_boss_notify = 0;
int g_v40_boss_14c2f0 = 0;
int g_v40_boss_load = 0;
int g_v40_boss_14ccc0 = 0;
int g_v40_cp3_notify = 0;
int g_v40_cp3_14c2f0 = 0;
int g_v40_cp3_load = 0;
int g_v40_cp3_14ccc0 = 0;
int g_v40_csp_notify = 0;
int g_v40_csp_14c2f0 = 0;
int g_v40_csp_load = 0;
int g_v40_csp_14ccc0 = 0;
// 0xf1a0/0x14c2f0 key seam: tag 0 = string buffer, tag 1 = 8-byte control block.
std::uint64_t g_key_alloc_tag0 = 0;
std::uint64_t g_key_alloc_tag1 = 0;
std::uint64_t g_key_free_tag0 = 0;
std::uint64_t g_key_free_tag1 = 0;
std::uint64_t g_key_platform_tag_calls = 0;
std::uint64_t g_key_platform_tag_crmp = 0;
std::uint64_t g_key_platform_tag_calt = 0;
void* g_key_last_buffer = nullptr;
void* g_key_last_control = nullptr;
int g_v40_tail_140d90 = 0;
int g_v40_tail_release_override = 0;
int g_v40_tail_140d90_arg = -1;
void* g_v40_tail_140d90_slot = nullptr;
int g_post_base_499f80 = 0;
void* g_post_base_499f80_arg = nullptr;
int g_post_base_56dcb0 = 0;
int g_post_base_5c64c0 = 0;
int g_post_base_1290 = 0;
int g_post_base_56db70 = 0;
int g_post_base_5b3d00 = 0;
int g_post_base_5b3cf0 = 0;
int g_post_base_7f7220 = 0;
int g_post_base_56fad0 = 0;
int g_post_base_f90310 = 0;
int g_post_base_26800 = 0;
int g_post_base_f590 = 0;
int g_post_base_sbj_26800 = 0;
int g_post_base_sbj_f590 = 0;
int g_post_base_sboj_26800 = 0;
int g_post_base_sboj_f590 = 0;
int g_post_base_52eb50 = 0;
int g_post_base_532ca0 = 0;
int g_post_base_vcall18 = 0;
int g_post_base_532ca0_u2 = 0;
int g_post_base_vcall18_u2 = 0;
int g_post_base_532ca0_u3 = 0;
int g_post_base_vcall18_u3 = 0;
int g_post_base_532ca0_u4 = 0;
int g_post_base_vcall18_u4 = 0;
int g_post_base_532ca0_u5 = 0;
int g_post_base_vcall18_u5 = 0;
int g_post_base_532ed0 = 0;
int g_post_base_vcall18_532ed0 = 0;
int g_post_base_532ed0_u2 = 0;
int g_post_base_vcall18_532ed0_u2 = 0;
int g_post_base_17070 = 0;
int g_post_base_544fe0 = 0;
int g_post_base_17070_u2 = 0;
int g_post_base_53fd10 = 0;
int g_post_base_vcall38 = 0;
int g_post_base_a2caf0 = 0;
int g_post_base_a2ed00 = 0;
int g_post_base_f590_a2ed00 = 0;
int g_post_base_f590_a2ed00_u2 = 0;
int g_post_base_a2ed00_u2 = 0;
int g_post_base_f590_a2ed00_u3 = 0;
int g_post_base_f590_a2ed00_u4 = 0;
int g_post_base_a2ed00_u3 = 0;
int g_post_base_f590_a2ed00_u5 = 0;
int g_post_base_f590_a2ed00_u6 = 0;
int g_post_base_a2ed00_u4 = 0;
int g_post_base_f590_a2ed00_u7 = 0;
int g_post_base_f590_a2ed00_u8 = 0;
int g_post_base_a2ed00_u5 = 0;
int g_post_base_f590_a2ed00_u9 = 0;
int g_post_base_f590_a2ed00_u10 = 0;
int g_post_base_a2ed00_u6 = 0;
int g_post_base_f590_a2ed00_u11 = 0;
int g_post_base_f590_a2ed00_u12 = 0;
int g_post_base_a2ed00_u7 = 0;
int g_post_base_f590_a2ed00_u13 = 0;
int g_post_base_f590_a2ed00_u14 = 0;
int g_post_base_a2ea00_u1 = 0;
int g_post_base_a2e5d0_u1 = 0;
int g_post_base_a543b0_u1 = 0;
int g_post_base_1fc80_u1 = 0;
// E519 grouped continue 0x532736..0x53277f: per-call-site hooks + observed args.
int g_cont_a_a2e5d0 = 0;
int g_cont_a_a543b0 = 0;
int g_cont_a_1fc80 = 0;
void* g_cont_a_a2ea00_rbx = nullptr;
void* g_cont_a_a2e5d0_rbx = nullptr;
int g_cont_a_a2e5d0_esi = 0;
void* g_cont_a_a543b0_rdi = nullptr;
std::uintptr_t g_cont_a_a543b0_rsi = 0;
void* g_cont_a_1fc80_rdi = nullptr;
std::uintptr_t g_cont_a_1fc80_rsi_rva = 0;
void* g_cont_a_r13_obj = nullptr;
void* g_cont_a_cell_32f79a8 = nullptr;
void* g_cont_a_cell_32f84f8 = nullptr;
void* g_cont_a_rsp20 = nullptr;
alignas(8) std::uint8_t g_cont_a_obj[0x100]{};
alignas(8) std::uint8_t g_cont_a_rdi_marker[8]{};
alignas(8) std::uint8_t g_cont_a_rsp20_obj[0x10]{};
// E510 grouped continue B 0x53277f..0x5327b1: per-call-site hooks + observed args.
int g_cont_b_1e4c0 = 0;
int g_cont_b_4c4950 = 0;
void* g_cont_b_1e4c0_rdi = nullptr;
int g_cont_b_1e4c0_eax = 0;
std::uint8_t g_cont_b_4c4950_edi = 0;
void* g_cont_b_rax = nullptr;
void* g_cont_b_cell_32f84f8 = nullptr;
alignas(8) std::uint8_t g_cont_b_rax_obj[8]{};
// E511 grouped continue C 0x532904..0x53299d: per-call-site hooks + observed args.
int g_cont_c_a52b70 = 0;
int g_cont_c_a599e0 = 0;
int g_cont_c_a431e0 = 0;
int g_cont_c_vcall = 0;
void* g_cont_c_a52b70_rdi = nullptr;
std::uint8_t g_cont_c_a52b70_esi = 0;
void* g_cont_c_a599e0_rdi = nullptr;
std::uint8_t g_cont_c_a599e0_esi = 0;
void* g_cont_c_a431e0_rdi = nullptr;
std::uint8_t g_cont_c_a431e0_esi = 0;
int g_cont_c_vcall_esi[3]{};
void* g_cont_c_cell_32f8b80 = nullptr;
void* g_cont_c_cell_32f6d18 = nullptr;
void* g_cont_c_cell_2efe318 = nullptr;
void* g_cont_c_cell_2efe340 = nullptr;
std::uint8_t g_cont_c_flag1 = 0;
std::uint8_t g_cont_c_flag2 = 0;
alignas(8) std::uint8_t g_cont_c_obj_a[8]{};
alignas(8) std::uint8_t g_cont_c_obj_b[8]{};
int g_post_base_cell_2eff450_dtor = 0;
int g_post_base_cell_2eff448_dtor = 0;
void* g_cell_2eff450 = nullptr;
void* g_cell_2eff448 = nullptr;
void* g_lea_32f4fe0_r13 = nullptr;
void* g_lea_32f4fe0_cell = reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x32f4fe0));
alignas(8) std::uint8_t g_cell_2eff450_neu[8]{};
alignas(8) std::uint8_t g_cell_2eff448_neu[8]{};
alignas(8) std::uint8_t g_lea_vmovq_src[8]{};
alignas(8) std::uint8_t g_lea_vmovq_xmm0[8]{};
void* g_lea_vmovq_rbx_cell = reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x32f79a8));
void* g_lea_vmovq_rbx = nullptr;
alignas(16) std::uint8_t g_avx_bss_src[16]{};
alignas(16) std::uint8_t g_avx_bss_dst0[16]{};
alignas(16) std::uint8_t g_avx_bss_dst1[16]{};
alignas(32) std::uint8_t g_ymm_bss_src[32]{};
alignas(32) std::uint8_t g_ymm_bss_dst4278[0x30]{};
alignas(16) std::uint8_t g_ymm_bss_dst426c[12]{};
alignas(32) std::uint8_t g_ymm_bss_dst42dc[0x30]{};
alignas(4) std::uint8_t g_ymm_bss_dst4268[4]{};
alignas(32) std::uint8_t g_ymm_ic_dst42a8[0x30]{};
alignas(32) std::uint8_t g_ymm_ic_dst430c[0x30]{};
alignas(16) std::uint8_t g_xmm_seed_src[16]{};
alignas(4) std::uint8_t g_xmm_seed_dst42d8[4]{};
alignas(16) std::uint8_t g_xmm_seed_dst430c[0x24]{};
alignas(32) std::uint8_t g_ymm_reload_src430c[0x30]{};
alignas(32) std::uint8_t g_ymm_reload_dst433c[0x30]{};
alignas(8) std::uint8_t g_shadow_asset[0x40]{};
alignas(8) std::uint8_t g_skels_asset[0x40]{};
alignas(8) std::uint8_t g_anims_asset[0x40]{};
alignas(8) std::uint8_t g_banks_asset[0x40]{};
alignas(8) std::uint8_t g_streams_asset[0x40]{};
alignas(8) std::uint8_t g_rand_asset[0x40]{};
alignas(8) std::uint8_t g_boss_asset[0x40]{};
alignas(8) std::uint8_t g_cp3_asset[0x40]{};
alignas(8) std::uint8_t g_csp_asset[0x40]{};

void ctor_a30(void* obj) {
    ++g_ctor_a30;
    *static_cast<std::uint32_t*>(obj) = 0xA30A30A3u;
}
void dtor_early(void*) { ++g_dtor_early; }
void dtor_98a90(void*) { ++g_dtor_98a90; }
void dtor_3b40(void*) { ++g_dtor_3b40; }
void hook_nonnull(void*, void*) { ++g_nonnull; }
void hook_f590(void*) { ++g_f590; }
void hook_move(void*, void*) { ++g_move; }
std::uint64_t hook_init_tb() {
    ++g_init_tb;
    return 0xF8F8600000000001ULL;
}

void hook_mid_26ea0(void*, const char*, const char*) { ++g_mid_reg; }
void hook_mid_26b60(const char*) { ++g_mid_b60; }
void hook_f08_ctor(void* obj) {
    ++g_f08_ctor;
    // Marker at +0x30 — OBS slots +0/+8 models, +0x10/+0x18 skels, +0x20/+0x28 anims,
    // +0x40/+0x48 banks, +0x50/+0x58 streams, +0x60/+0x68 randomevents,
    // +0x70/+0x78 bossmusic (marker sits in the gap before banks).
    std::memset(obj, 0, 0x80);
    *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(obj) + 0x30) = 0xF08F08F0u;
}
void hook_f08_dtor(void*) { ++g_f08_dtor; }
void hook_v40_ctor(void* obj) {
    ++g_v40_ctor;
    *static_cast<std::uint32_t*>(obj) = 0x53310040u;
}
void hook_v40_525080(void*) { ++g_v40_525080; }
void hook_v40_2569d0(void*, void*) { ++g_v40_2569d0; }
void* hook_v40_21620(void*) {
    ++g_v40_21620;
    return nullptr;  // measured empty → null find
}
void hook_v40_ba40(void*, void*, void*) { ++g_v40_ba40; }
SharedReleaseF590Result hook_v40_miss_release(SharedHolderF590& holder) {
    ++g_v40_miss_release_override;
    CHECK(holder.object != nullptr);
    CHECK(std::strcmp(static_cast<const char*>(holder.object), kVslot40MissNameRFps) == 0);
    CHECK(holder.reserved_08 == reinterpret_cast<void*>(static_cast<std::uintptr_t>(5)));
    CHECK(holder.reserved_10 == nullptr);
    CHECK(holder.control == nullptr);
    SharedReleaseF590Result rel{};
    rel.ok = true;
    return rel;
}
void hook_v40_42390(void*) { ++g_v40_42390; }
void hook_v40_2779a0(void* obj, void*, int) {
    ++g_v40_2779a0;
    *static_cast<std::uint32_t*>(obj) = 0x2779A001u;
}
SharedReleaseF590Result hook_v40_join_release(SharedHolderF590& holder) {
    ++g_v40_join_release_override;
    CHECK(holder.object != nullptr);
    CHECK(std::strcmp(static_cast<const char*>(holder.object), kVslot40JoinNamePosition) == 0);
    CHECK(holder.reserved_08 == reinterpret_cast<void*>(static_cast<std::uintptr_t>(8)));
    CHECK(holder.reserved_10 == nullptr);
    CHECK(holder.control == nullptr);
    SharedReleaseF590Result rel{};
    rel.ok = true;
    return rel;
}
void hook_v40_join_element(void*) { ++g_v40_join_element; }
void hook_v40_delete_array_noop(void*) {}
void hook_v40_delete_noop(void*) {}
void hook_v40_6a5370(void*) { ++g_v40_6a5370; }
void hook_v40_6abcc0(void*) { ++g_v40_6abcc0; }
void hook_v40_14c2f0(void* dst, const char* path) {
    ++g_v40_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40ShadowPath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_shadow_load(void* out_slot, void*, void*) {
    ++g_v40_shadow_load;
    void* p = g_shadow_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void hook_v40_skels_notify(void*) { ++g_v40_skels_notify; }
void hook_v40_skels_14c2f0(void* dst, const char* path) {
    ++g_v40_skels_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40ShadowskelsPath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_skels_load(void* out_slot, void*, void*) {
    ++g_v40_skels_load;
    void* p = g_skels_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void hook_v40_anims_notify(void*) { ++g_v40_anims_notify; }
void hook_v40_anims_14c2f0(void* dst, const char* path) {
    ++g_v40_anims_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40ShadowanimsPath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_anims_load(void* out_slot, void*, void*) {
    ++g_v40_anims_load;
    void* p = g_anims_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void hook_v40_banks_notify(void*) { ++g_v40_banks_notify; }
void hook_v40_banks_14c2f0(void* dst, const char* path) {
    ++g_v40_banks_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40ShadowbanksPath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_banks_load(void* out_slot, void*, void*) {
    ++g_v40_banks_load;
    void* p = g_banks_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void hook_v40_streams_notify(void*) { ++g_v40_streams_notify; }
void hook_v40_streams_14c2f0(void* dst, const char* path) {
    ++g_v40_streams_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40ShadowstreamsPath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_streams_load(void* out_slot, void*, void*) {
    ++g_v40_streams_load;
    void* p = g_streams_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void hook_v40_rand_notify(void*) { ++g_v40_rand_notify; }
void hook_v40_rand_14c2f0(void* dst, const char* path) {
    ++g_v40_rand_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40ShadowrandomeventsPath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_rand_load(void* out_slot, void*, void*) {
    ++g_v40_rand_load;
    void* p = g_rand_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void hook_v40_boss_notify(void*) { ++g_v40_boss_notify; }
void hook_v40_boss_14c2f0(void* dst, const char* path) {
    ++g_v40_boss_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40ShadowbossmusicPath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_boss_load(void* out_slot, void*, void*) {
    ++g_v40_boss_load;
    void* p = g_boss_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void hook_v40_cp3_notify(void*) { ++g_v40_cp3_notify; }
void hook_v40_cp3_14c2f0(void* dst, const char* path) {
    ++g_v40_cp3_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40Cp3charactersPath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_cp3_load(void* out_slot, void*, void*) {
    ++g_v40_cp3_load;
    void* p = g_cp3_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void hook_v40_csp_notify(void*) { ++g_v40_csp_notify; }
void hook_v40_csp_14c2f0(void* dst, const char* path) {
    ++g_v40_csp_14c2f0;
    CHECK(path != nullptr);
    CHECK(std::strcmp(path, kVslot40Cp3shadowprecachePath) == 0);
    std::memcpy(dst, &path, sizeof(void*));
}
void* hook_v40_csp_load(void* out_slot, void*, void*) {
    ++g_v40_csp_load;
    void* p = g_csp_asset;
    std::memcpy(out_slot, &p, sizeof(void*));
    return p;
}
void* hook_key_alloc(std::size_t size, std::uint32_t tag, std::uint32_t, std::uint32_t) {
    void* p = std::malloc(size);
    CHECK(p != nullptr);
    if (tag == 0) {
        ++g_key_alloc_tag0;
        g_key_last_buffer = p;
    } else if (tag == 1) {
        ++g_key_alloc_tag1;
        g_key_last_control = p;
    } else {
        CHECK(false);
    }
    return p;
}
bool hook_key_dealloc(void* p, std::uint32_t tag) {
    if (tag == 0) {
        ++g_key_free_tag0;
    } else if (tag == 1) {
        ++g_key_free_tag1;
    } else {
        CHECK(false);
    }
    std::free(p);
    return true;
}
std::uint32_t hook_key_platform_tag(std::uint32_t fourcc) {
    ++g_key_platform_tag_calls;
    if (fourcc == 0x63726d70u) {  // "crmp"
        ++g_key_platform_tag_crmp;
    } else if (fourcc == 0x63616c74u) {  // "calt"
        ++g_key_platform_tag_calt;
    } else {
        CHECK(false);
    }
    return platform_tag_147e20_default(fourcc);
}
void hook_key_destroy(void* p, int) { std::free(p); }
void hook_key_free_control(void* p) { std::free(p); }

struct SliceKeyDeallocator final : SharedStrDeallocatorF590 {
    void deallocate(const void* allocation, SharedStrDeallocationKindF590 kind) noexcept override {
        if (kind == SharedStrDeallocationKindF590::data) {
            ++g_key_free_tag0;
        } else {
            ++g_key_free_tag1;
        }
        std::free(const_cast<void*>(allocation));
    }
};
SliceKeyDeallocator g_pool_key_dealloc;
int g_pool_alloc = 0;
int g_pool_free = 0;
int g_pool_state_write = 0;
void* slice_pool_allocate(std::size_t size) {
    ++g_pool_alloc;
    void* const p = std::malloc(size);
    CHECK(p != nullptr);
    return p;
}
bool slice_pool_deallocate(void* ptr) {
    ++g_pool_free;
    std::free(ptr);
    return true;
}
bool slice_pool_state_write(ResourceWrapper&, std::uint32_t) {
    ++g_pool_state_write;
    return true;
}

template <typename Hooks>
void install_key_hooks(Hooks& h) {
    h.common.key_alloc.alloc.allocate = hook_key_alloc;
    h.common.key_alloc.alloc.deallocate = hook_key_dealloc;
    h.common.platform_tag = hook_key_platform_tag;
    h.common.release.destroy = hook_key_destroy;
    h.common.release.free_control = hook_key_free_control;
}

void check_key_release_miss(const SharedReleaseF590Result& kr) {
    CHECK(kr.ok);
    CHECK(kr.had_control);
    CHECK(kr.decremented);
    CHECK(kr.previous == 3);  // wrapper +0x70 copy and the pool's map-node copy
    CHECK(!kr.was_last);
    CHECK(!kr.destroyed);
    CHECK(!kr.freed);
}
void check_key_release(const SharedReleaseF590Result& kr) {
    CHECK(kr.ok);
    CHECK(kr.had_control);
    CHECK(kr.decremented);
    CHECK(kr.previous == 1);
    CHECK(kr.was_last);
    CHECK(kr.destroyed);
    CHECK(kr.freed);
}
void hook_v40_tail_140d90(void* slot_obj, int arg) {
    ++g_v40_tail_140d90;
    g_v40_tail_140d90_slot = slot_obj;
    g_v40_tail_140d90_arg = arg;
}
SharedReleaseF590Result hook_v40_tail_release(SharedHolderF590& holder) {
    ++g_v40_tail_release_override;
    CHECK(holder.object != nullptr);
    CHECK(std::strcmp(static_cast<const char*>(holder.object),
                      kVslot40Tail140d90NamePosition) == 0);
    CHECK(holder.reserved_08 == reinterpret_cast<void*>(static_cast<std::uintptr_t>(8)));
    CHECK(holder.reserved_10 == nullptr);
    CHECK(holder.control == nullptr);
    SharedReleaseF590Result rel{};
    rel.ok = true;
    return rel;
}
void hook_post_base_499f80(void* arg) {
    ++g_post_base_499f80;
    g_post_base_499f80_arg = arg;
}
void hook_post_base_56dcb0() { ++g_post_base_56dcb0; }
void hook_post_base_5c64c0() { ++g_post_base_5c64c0; }
void hook_post_base_1290() { ++g_post_base_1290; }
void hook_post_base_56db70() { ++g_post_base_56db70; }
void hook_post_base_5b3d00() { ++g_post_base_5b3d00; }
void hook_post_base_5b3cf0() { ++g_post_base_5b3cf0; }
void hook_post_base_7f7220() { ++g_post_base_7f7220; }
void hook_post_base_56fad0() { ++g_post_base_56fad0; }
void hook_post_base_f90310() { ++g_post_base_f90310; }
void hook_post_base_26800() { ++g_post_base_26800; }

// The 17 Init `0xf590` sites each construct a 32 B `{ptr,len,0,0}` stack temporary whose
// `+0x18` (control) word is zero, and the preceding callees (0x26800 / 0xa2ed00) only read
// that word. The real lift is therefore an exact no-op; the overrides below stand in for a
// host that supplies its own release contract.
void check_f590_release_noop(const SharedReleaseF590Result& rel) {
    CHECK(rel.ok);
    CHECK(!rel.had_control);
    CHECK(!rel.decremented);
    CHECK(!rel.was_last);
    CHECK(!rel.destroyed);
    CHECK(!rel.freed);
    CHECK(rel.previous == 0);
}

// Host override: checks the module built the measured zeroed holder, then returns a result
// only an override can produce (a zeroed holder can never report a control block).
SharedReleaseF590Result f590_override(SharedHolderF590& holder) {
    CHECK(holder.object == nullptr);
    CHECK(holder.reserved_08 == nullptr);
    CHECK(holder.reserved_10 == nullptr);
    CHECK(holder.control == nullptr);
    SharedReleaseF590Result rel{};
    rel.ok = true;
    rel.had_control = true;
    rel.decremented = true;
    rel.was_last = true;
    rel.previous = 1;
    rel.destroyed = true;
    rel.freed = true;
    return rel;
}

// A single-site 0xf590 module with default hooks: one call counted, real lift a no-op.
template <typename Result>
void check_init_f590_defaults(const Result& r) {
    CHECK(r.ok);
    CHECK(r.call_f590_count == 1);
    check_f590_release_noop(r.release);
}

SharedReleaseF590Result hook_post_base_f590(SharedHolderF590& holder) {
    ++g_post_base_f590;
    return f590_override(holder);
}
void hook_post_base_sbj_26800() { ++g_post_base_sbj_26800; }
SharedReleaseF590Result hook_post_base_sbj_f590(SharedHolderF590& holder) {
    ++g_post_base_sbj_f590;
    return f590_override(holder);
}
void hook_post_base_sboj_26800() { ++g_post_base_sboj_26800; }
SharedReleaseF590Result hook_post_base_sboj_f590(SharedHolderF590& holder) {
    ++g_post_base_sboj_f590;
    return f590_override(holder);
}
void hook_post_base_52eb50() { ++g_post_base_52eb50; }
void hook_post_base_532ca0() { ++g_post_base_532ca0; }
void hook_post_base_vcall18() { ++g_post_base_vcall18; }
void hook_post_base_532ca0_u2() { ++g_post_base_532ca0_u2; }
void hook_post_base_vcall18_u2() { ++g_post_base_vcall18_u2; }
void hook_post_base_532ca0_u3() { ++g_post_base_532ca0_u3; }
void hook_post_base_vcall18_u3() { ++g_post_base_vcall18_u3; }
void hook_post_base_532ca0_u4() { ++g_post_base_532ca0_u4; }
void hook_post_base_vcall18_u4() { ++g_post_base_vcall18_u4; }
void hook_post_base_532ca0_u5() { ++g_post_base_532ca0_u5; }
void hook_post_base_vcall18_u5() { ++g_post_base_vcall18_u5; }
void hook_post_base_532ed0() { ++g_post_base_532ed0; }
void hook_post_base_vcall18_532ed0() { ++g_post_base_vcall18_532ed0; }
void hook_post_base_532ed0_u2() { ++g_post_base_532ed0_u2; }
void hook_post_base_vcall18_532ed0_u2() { ++g_post_base_vcall18_532ed0_u2; }
void hook_post_base_17070() { ++g_post_base_17070; }
void hook_post_base_544fe0() { ++g_post_base_544fe0; }
void hook_post_base_17070_u2() { ++g_post_base_17070_u2; }
void hook_post_base_53fd10() { ++g_post_base_53fd10; }
void hook_post_base_vcall38() { ++g_post_base_vcall38; }
void hook_post_base_a2caf0() { ++g_post_base_a2caf0; }
void hook_post_base_a2ed00() { ++g_post_base_a2ed00; }
void hook_post_base_a2ed00_u2() { ++g_post_base_a2ed00_u2; }
SharedReleaseF590Result hook_post_base_f590_a2ed00(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00;
    return f590_override(holder);
}
SharedReleaseF590Result hook_post_base_f590_a2ed00_u2(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u2;
    return f590_override(holder);
}
SharedReleaseF590Result hook_post_base_f590_a2ed00_u3(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u3;
    return f590_override(holder);
}
SharedReleaseF590Result hook_post_base_f590_a2ed00_u4(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u4;
    return f590_override(holder);
}
void hook_post_base_a2ed00_u3() { ++g_post_base_a2ed00_u3; }
SharedReleaseF590Result hook_post_base_f590_a2ed00_u5(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u5;
    return f590_override(holder);
}
SharedReleaseF590Result hook_post_base_f590_a2ed00_u6(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u6;
    return f590_override(holder);
}
void hook_post_base_a2ed00_u4() { ++g_post_base_a2ed00_u4; }
SharedReleaseF590Result hook_post_base_f590_a2ed00_u7(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u7;
    return f590_override(holder);
}
SharedReleaseF590Result hook_post_base_f590_a2ed00_u8(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u8;
    return f590_override(holder);
}
void hook_post_base_a2ed00_u5() { ++g_post_base_a2ed00_u5; }
SharedReleaseF590Result hook_post_base_f590_a2ed00_u9(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u9;
    return f590_override(holder);
}
SharedReleaseF590Result hook_post_base_f590_a2ed00_u10(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u10;
    return f590_override(holder);
}
void hook_post_base_a2ed00_u6() { ++g_post_base_a2ed00_u6; }
SharedReleaseF590Result hook_post_base_f590_a2ed00_u11(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u11;
    return f590_override(holder);
}
SharedReleaseF590Result hook_post_base_f590_a2ed00_u12(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u12;
    return f590_override(holder);
}
void hook_post_base_a2ed00_u7() { ++g_post_base_a2ed00_u7; }
SharedReleaseF590Result hook_post_base_f590_a2ed00_u13(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u13;
    return f590_override(holder);
}
SharedReleaseF590Result hook_post_base_f590_a2ed00_u14(SharedHolderF590& holder) {
    ++g_post_base_f590_a2ed00_u14;
    return f590_override(holder);
}
int hook_cont_a_a2ea00(void* rbx) {
    ++g_post_base_a2ea00_u1;
    g_cont_a_a2ea00_rbx = rbx;
    return 0x5A;  // host stand-in for eax only
}
void hook_cont_a_a2e5d0(void* rbx, int esi) {
    ++g_post_base_a2e5d0_u1;
    ++g_cont_a_a2e5d0;
    g_cont_a_a2e5d0_rbx = rbx;
    g_cont_a_a2e5d0_esi = esi;
}
void hook_cont_a_a543b0(void* rdi, std::uintptr_t rsi) {
    ++g_post_base_a543b0_u1;
    ++g_cont_a_a543b0;
    g_cont_a_a543b0_rdi = rdi;
    g_cont_a_a543b0_rsi = rsi;
}
void hook_cont_a_1fc80(void* rdi, std::uintptr_t rsi_rva) {
    ++g_post_base_1fc80_u1;
    ++g_cont_a_1fc80;
    g_cont_a_1fc80_rdi = rdi;
    g_cont_a_1fc80_rsi_rva = rsi_rva;
}
int g_extra_slot0 = 0;

// E batch (continue_e — 0x5329ae..0x532b0c, save-data ctors):
int g_cont_e_a54920 = 0;
int g_cont_e_54b250 = 0;
void* g_cont_e_a54920_parent[3]{};
void* g_cont_e_a54920_name[3]{};
void* g_cont_e_a54920_desc[3]{};
void* g_cont_e_54b250_obj = nullptr;
std::uint8_t g_boss_flag_cell = 0x01;   // match E418 file value
void* g_cell_2eff450_value = nullptr;
void* g_cont_e_r13 = nullptr;

int g_alt_ac5260_calls = 0;
int g_alt_3e7e0_calls = 0;
int g_alt_4c4950_zero_calls = 0;
int g_alt_4c4950_one_calls = 0;
void hook_alt_ac5260(void*, void*, int) { ++g_alt_ac5260_calls; }
void hook_alt_3e7e0(int, int, int, int, const void*, const void*, void*) {
    ++g_alt_3e7e0_calls;
}
void hook_alt_4c4950(int edi) {
    if (edi == 0) ++g_alt_4c4950_zero_calls;
    if (edi == 1) ++g_alt_4c4950_one_calls;
}

void hook_cont_e_a54920(void* parent, void* name, void* descriptor) {
    int n = g_cont_e_a54920;
    if (n < 3) {
        g_cont_e_a54920_parent[n] = parent;
        g_cont_e_a54920_name[n] = name;
        g_cont_e_a54920_desc[n] = descriptor;
    }
    ++g_cont_e_a54920;
}

void hook_cont_e_54b250(void* obj, std::uint32_t, std::uint32_t) {
    ++g_cont_e_54b250;
    g_cont_e_54b250_obj = obj;
}

alignas(16) unsigned char g_slice_1290_pool[0x60];
int g_slice_1290_vcall = 0;

void* slice_1290_alloc(std::size_t bytes) {
    return (bytes == 0x60) ? static_cast<void*>(g_slice_1290_pool) : nullptr;
}
void hook_slice_1290_vcall(void*) { ++g_slice_1290_vcall; }
void hook_slice_1290_insert_b(void*, const HandlerRegistration1290&) {}

int g_cont_d_call = 0;
void* g_cont_d_rdi = nullptr;
const void* g_cont_d_cell_value = reinterpret_cast<const void*>(0x11223344);

void hook_cont_d_call(void* rdi) {
    ++g_cont_d_call;
    g_cont_d_rdi = rdi;
}

// Native 0xa473b0 contract mock for batch D: records the lift's service dispatches.
struct CaptureRecorderA473b0 final : CaptureServices {
    int live = 0;
    int screen_enable = 0;
    int screen_disable = 0;
    int video_calls = 0;
    std::uint32_t last_type = 0;
    std::uint32_t last_value = 0;
    std::size_t last_size = 0;
    bool last_enable_arg = false;
    CaptureServiceStatus game_live_streaming_enable(bool enable) override {
        ++live;
        last_enable_arg = enable;
        return CaptureServiceStatus::ok;
    }
    CaptureServiceStatus screenshot_enable() override {
        ++screen_enable;
        return CaptureServiceStatus::ok;
    }
    CaptureServiceStatus screenshot_disable() override {
        ++screen_disable;
        return CaptureServiceStatus::ok;
    }
    CaptureServiceStatus video_recording_set_info(std::uint32_t type, const void* value,
                                                  std::size_t size) override {
        ++video_calls;
        last_type = type;
        last_size = size;
        std::uint32_t v = 0;
        std::memcpy(&v, value, size < sizeof(v) ? size : sizeof(v));
        last_value = v;
        return CaptureServiceStatus::ok;
    }
};
CaptureRecorderA473b0 g_capture_a473b0;

void hook_extra_slot0(void*, std::uint8_t) { ++g_extra_slot0; }

int g_cont_b_1e4c0_value = 0xA6;  // host stand-in; 0xA6 keeps the non-boolean path

int hook_cont_b_1e4c0(void* rdi) {
    ++g_cont_b_1e4c0;
    g_cont_b_1e4c0_rdi = rdi;
    const int eax = g_cont_b_1e4c0_value;
    g_cont_b_1e4c0_eax = eax;
    return eax;
}
void hook_cont_b_4c4950(std::uint8_t edi) {
    ++g_cont_b_4c4950;
    g_cont_b_4c4950_edi = edi;
}
void hook_cont_c_a52b70(void* rdi, std::uint8_t esi) {
    ++g_cont_c_a52b70;
    g_cont_c_a52b70_rdi = rdi;
    g_cont_c_a52b70_esi = esi;
}
void hook_cont_c_a599e0(void* rdi, std::uint8_t esi) {
    ++g_cont_c_a599e0;
    g_cont_c_a599e0_rdi = rdi;
    g_cont_c_a599e0_esi = esi;
}
void hook_cont_c_a431e0(void* rdi, std::uint8_t esi) {
    ++g_cont_c_a431e0;
    g_cont_c_a431e0_rdi = rdi;
    g_cont_c_a431e0_esi = esi;
}
void hook_cont_c_vcall(void* /*rdi*/, std::uint8_t esi) {
    g_cont_c_vcall_esi[g_cont_c_vcall] = esi;
    ++g_cont_c_vcall;
}
void hook_post_base_cell_2eff450_dtor(void* /*prev*/) { ++g_post_base_cell_2eff450_dtor; }
void hook_post_base_cell_2eff448_dtor(void* /*prev*/) { ++g_post_base_cell_2eff448_dtor; }

void* loadp(const void* base, std::size_t off) {
    void* p = nullptr;
    std::memcpy(&p, static_cast<const std::uint8_t*>(base) + off, sizeof(void*));
    return p;
}

void reset_cells() {
    ctor_98a90_self_cell_2ef63f8() = nullptr;
    ctor_98a90_list_head_cell_2ef6400() = nullptr;
    store_tsc_guard_cell_2ea4700() = 0;
    store_tsc_global_cell_2ea4708() = 0;
}

}  // namespace

int main() {
    CHECK(kInitSub3be930Size == 826);
    CHECK(kInitBlock1d9c0Size == 73);
    CHECK(kInitSlotF180Size == 28);
    CHECK(kEnsureCapacity3c2f70Size == 313);
    CHECK(ensure_3c2f70_elem_count(0, 0xa800) == 0x100);

    AllocatorFront front;
    FakeHeap heap(front.owners(), 2);
    CHECK(front.set_heap(1, &heap));
    CHECK(front.set_heap(2, &heap));

    // Unit: sub-init 0x3be930 alone.
    {
        std::vector<std::uint8_t> region(0x98a28, 0xA5);
        init_sub_3be930(region.data());
        CHECK(region[0] == 0);
        void* vt = loadp(region.data(), 0x18);
        CHECK(vt == slot_vtable_2dd9f78());
        std::uint32_t tail_d = 0;
        std::memcpy(&tail_d, region.data() + 0x98a10, 4);
        CHECK(tail_d == 0);
        void* mid = loadp(region.data(), 0x10 + 0x30);
        CHECK(mid == region.data() + 0x10 + 0x58);
    }

    // Unit: ensure empty path alone.
    {
        std::uint8_t vec[0x20]{};
        EnsureCapacity3c2f70Hooks eh{};
        eh.elem_move_1da30 = hook_move;
        eh.elem_dtor_f590 = hook_f590;
        g_move = g_f590 = 0;
        const auto er = ensure_capacity_3c2f70(front, vec, 0x100, eh);
        CHECK(er.ok);
        CHECK(er.allocated);
        CHECK(er.buf != nullptr);
        CHECK(g_move == 0);
        CHECK(g_f590 == 0);
        void* b = loadp(vec, 0x08);
        void* e = loadp(vec, 0x10);
        void* c = loadp(vec, 0x18);
        CHECK(b == er.buf);
        CHECK(e == b);
        CHECK(static_cast<std::uint8_t*>(c) == static_cast<std::uint8_t*>(b) + 0xa800);
    }

    // Unit: ctor with lifted ensure (no override).
    {
        reset_cells();
        std::vector<std::uint8_t> obj(kPrologue98a90Bytes, 0);
        Ctor98a90_3be050Hooks ch{};
        ch.tsc_detail.init_timebase_f8f860 = hook_init_tb;
        ch.ensure_detail.elem_move_1da30 = hook_move;
        ch.elem_dtor_f590 = hook_f590;
        g_init_tb = g_move = g_f590 = 0;
        const auto cr = init_98a90_3be050(front, obj.data(), ch);
        CHECK(cr.ok);
        CHECK(cr.sub_3be930_ok);
        CHECK(cr.ensure_called);
        CHECK(cr.ensure_allocated);
        CHECK(cr.ensure_buf != nullptr);
        CHECK(g_move == 0);
        void* b = loadp(obj.data(), 0x28);
        void* c = loadp(obj.data(), 0x38);
        CHECK(b == cr.ensure_buf);
        CHECK(static_cast<std::uint8_t*>(c) == static_cast<std::uint8_t*>(b) + 0xa800);
        CHECK(cr.tsc_stored);
        CHECK(cr.buf_grown);
        CHECK(cr.list_walk_skipped_null);
    }

    // Unit: the four safe 0xf590 sites use the real inert release; a host override sees the
    // recovered holder shape (literal, length, zero capacity/control).
    {
        Vslot40_533100MissHooks mh{};
        mh.call_f590 = hook_v40_miss_release;
        g_v40_miss_release_override = 0;
        const auto mr = vslot_40_533100_miss(mh);
        CHECK(mr.ok);
        CHECK(mr.dtor_f590_calls == 2);
        CHECK(g_v40_miss_release_override == 2);
    }
    {
        std::vector<std::uint8_t> join_self(kVslot40JoinSlotOff + sizeof(void*), 0);
        Vslot40_533100JoinHooks jh{};
        jh.call_f590 = hook_v40_join_release;
        g_v40_join_release_override = 0;
        const auto jr = vslot_40_533100_join(front, join_self.data(), jh);
        CHECK(jr.ok);
        CHECK(jr.replaced);
        CHECK(jr.release_f590_count == 1);
        CHECK(jr.dtor_f590_count == 1);
        CHECK(jr.dtor_f590_element_count == 0);
        CHECK(g_v40_join_release_override == 1);
    }
    {
        // 0x5333e3 stays a per-element hook, separate from the stack-name release.
        alignas(8) std::uint8_t elements[2 * 0x50]{};
        alignas(8) std::uint8_t prev[0x20]{};
        void* begin = elements;
        void* end = elements + sizeof(elements);
        void* prev_ptr = prev;
        std::memcpy(prev + 0x00, &begin, sizeof(void*));
        std::memcpy(prev + 0x08, &end, sizeof(void*));
        std::vector<std::uint8_t> join_self(kVslot40JoinSlotOff + sizeof(void*), 0);
        std::memcpy(join_self.data() + kVslot40JoinSlotOff, &prev_ptr, sizeof(void*));
        Vslot40_533100JoinHooks jh{};
        jh.dtor_f590_element = hook_v40_join_element;
        jh.delete_array_170c0 = hook_v40_delete_array_noop;
        jh.delete_170b0 = hook_v40_delete_noop;
        g_v40_join_element = 0;
        const auto jr = vslot_40_533100_join(front, join_self.data(), jh);
        CHECK(jr.ok);
        CHECK(jr.destroyed_prev);
        CHECK(jr.dtor_f590_element_count == 2);
        CHECK(jr.dtor_f590_count == 3);  // stack release + 2 elements
        CHECK(g_v40_join_element == 2);
    }
    {
        std::vector<std::uint8_t> tail_self(kVslot40Tail140d90SlotOff + 4, 0);
        Vslot40_533100Tail140d90Hooks th{};
        th.call_f590 = hook_v40_tail_release;
        g_v40_tail_release_override = 0;
        const auto tr = vslot_40_533100_tail_140d90(tail_self.data(), th);
        CHECK(tr.ok);
        CHECK(tr.dtor_f590_count == 1);
        CHECK(g_v40_tail_release_override == 1);
    }

    reset_cells();

    InputTable551140 table{};
    init_input_table_551140(table);
    InputtableNamesFixture fix;
    CHECK(load_inputtable_names_fixture_default(fix));
    CHECK(apply_inputtable_names_fixture(table, fix));

    // Join @ self+0x3b60; cp3 attach @ +0x3cb8/+0x3cc0; csp attach @ +0x3cc8/+0x3cd0;
    // post-base 0x499f80 arg @ +0x3c70.
    constexpr std::size_t kAppBytesJoin = kVslot40JoinSlotOff + sizeof(void*);
    constexpr std::size_t kAppBytesCp3 =
        static_cast<std::size_t>(kVslot40Cp3charactersAttachFlagOff) + 1;
    constexpr std::size_t kAppBytesCsp =
        static_cast<std::size_t>(kVslot40Cp3shadowprecacheAttachFlagOff) + 1;
    constexpr std::size_t kAppBytesPost =
        kInit531d60PostBase499f80ArgOff + sizeof(void*);
    constexpr std::size_t kAppBytesNeeded =
        (kAppBytesJoin > kAppBytesCp3)
            ? ((kAppBytesJoin > kAppBytesCsp)
                   ? ((kAppBytesJoin > kAppBytesPost) ? kAppBytesJoin : kAppBytesPost)
                   : ((kAppBytesCsp > kAppBytesPost) ? kAppBytesCsp : kAppBytesPost))
            : ((kAppBytesCp3 > kAppBytesCsp)
                   ? ((kAppBytesCp3 > kAppBytesPost) ? kAppBytesCp3 : kAppBytesPost)
                   : ((kAppBytesCsp > kAppBytesPost) ? kAppBytesCsp : kAppBytesPost));
    constexpr std::size_t kAppBytes =
        (kPrologueAppMinBytes > kAppBytesNeeded) ? kPrologueAppMinBytes : kAppBytesNeeded;
    std::vector<std::uint8_t> app(kAppBytes, 0);
    std::array<std::uint8_t, 0x60> setter{};
    std::memset(setter.data(), 0xCC, setter.size());

    g_ctor_a30 = g_dtor_98a90 = g_dtor_3b40 = g_dtor_early = 0;
    g_nonnull = g_f590 = g_init_tb = g_move = g_mid_reg = g_mid_b60 = 0;
    g_f08_ctor = g_f08_dtor = g_v40_ctor = g_v40_525080 = g_v40_2569d0 = 0;
    g_v40_21620 = g_v40_ba40 = g_v40_miss_release_override = 0;
    g_v40_42390 = g_v40_2779a0 = g_v40_join_release_override = 0;
    g_v40_join_element = 0;
    g_v40_6a5370 = g_v40_6abcc0 = 0;
    g_v40_14c2f0 = g_v40_shadow_load = 0;
    g_v40_14ccc0 = 0;
    g_v40_skels_notify = g_v40_skels_14c2f0 = g_v40_skels_load = 0;
    g_v40_skels_14ccc0 = 0;
    g_v40_anims_notify = g_v40_anims_14c2f0 = g_v40_anims_load = 0;
    g_v40_anims_14ccc0 = 0;
    g_v40_banks_notify = g_v40_banks_14c2f0 = g_v40_banks_load = 0;
    g_v40_banks_14ccc0 = 0;
    g_v40_streams_notify = g_v40_streams_14c2f0 = g_v40_streams_load = 0;
    g_v40_streams_14ccc0 = 0;
    g_v40_rand_notify = g_v40_rand_14c2f0 = g_v40_rand_load = 0;
    g_v40_rand_14ccc0 = 0;
    g_v40_boss_notify = g_v40_boss_14c2f0 = g_v40_boss_load = 0;
    g_v40_boss_14ccc0 = 0;
    g_v40_cp3_notify = g_v40_cp3_14c2f0 = g_v40_cp3_load = 0;
    g_v40_cp3_14ccc0 = 0;
    g_v40_csp_notify = g_v40_csp_14c2f0 = g_v40_csp_load = 0;
    g_v40_csp_14ccc0 = 0;
    g_v40_tail_140d90 = g_v40_tail_release_override = 0;
    g_v40_tail_140d90_arg = -1;
    g_v40_tail_140d90_slot = nullptr;
    g_post_base_499f80 = 0;
    g_post_base_499f80_arg = nullptr;
    g_post_base_56dcb0 = 0;
    g_post_base_5c64c0 = 0;
    g_post_base_1290 = 0;
    g_post_base_56db70 = 0;
    g_post_base_5b3d00 = 0;
    g_post_base_5b3cf0 = 0;
    g_post_base_7f7220 = 0;
    g_post_base_56fad0 = 0;
    g_post_base_f90310 = 0;
    g_post_base_26800 = 0;
    g_post_base_f590 = 0;
    g_post_base_sbj_26800 = 0;
    g_post_base_sbj_f590 = 0;
    g_post_base_sboj_26800 = 0;
    g_post_base_sboj_f590 = 0;
    g_post_base_52eb50 = 0;
    g_post_base_532ca0 = 0;
    g_post_base_vcall18 = 0;
    g_post_base_532ca0_u2 = 0;
    g_post_base_vcall18_u2 = 0;
    g_post_base_532ca0_u3 = 0;
    g_post_base_vcall18_u3 = 0;
    g_post_base_532ca0_u4 = 0;
    g_post_base_vcall18_u4 = 0;
    g_post_base_532ca0_u5 = 0;
    g_post_base_vcall18_u5 = 0;
    g_post_base_532ed0 = 0;
    g_post_base_vcall18_532ed0 = 0;
    g_post_base_532ed0_u2 = 0;
    g_post_base_vcall18_532ed0_u2 = 0;
    g_post_base_17070 = 0;
    g_post_base_544fe0 = 0;
    g_post_base_17070_u2 = 0;
    g_post_base_53fd10 = 0;
    g_post_base_vcall38 = 0;
    g_post_base_a2caf0 = 0;
    g_post_base_a2ed00 = 0;
    g_post_base_f590_a2ed00 = 0;
    g_post_base_f590_a2ed00_u2 = 0;
    g_post_base_a2ed00_u2 = 0;
    g_post_base_f590_a2ed00_u3 = 0;
    g_post_base_f590_a2ed00_u4 = 0;
    g_post_base_a2ed00_u3 = 0;
    g_post_base_f590_a2ed00_u5 = 0;
    g_post_base_f590_a2ed00_u6 = 0;
    g_post_base_a2ed00_u4 = 0;
    g_post_base_f590_a2ed00_u7 = 0;
    g_post_base_f590_a2ed00_u8 = 0;
    g_post_base_a2ed00_u5 = 0;
    g_post_base_f590_a2ed00_u9 = 0;
    g_post_base_f590_a2ed00_u10 = 0;
    g_post_base_a2ed00_u6 = 0;
    g_post_base_f590_a2ed00_u11 = 0;
    g_post_base_f590_a2ed00_u12 = 0;
    g_post_base_a2ed00_u7 = 0;
    g_post_base_f590_a2ed00_u13 = 0;
    g_post_base_f590_a2ed00_u14 = 0;
    g_post_base_a2ea00_u1 = 0;
    g_post_base_a2e5d0_u1 = 0;
    g_post_base_a543b0_u1 = 0;
    g_post_base_1fc80_u1 = 0;
    g_cont_a_a2e5d0 = 0;
    g_cont_a_a543b0 = 0;
    g_cont_a_1fc80 = 0;
    g_cont_a_a2ea00_rbx = nullptr;
    g_cont_a_a2e5d0_rbx = nullptr;
    g_cont_a_a2e5d0_esi = 0;
    g_cont_a_a543b0_rdi = nullptr;
    g_cont_a_a543b0_rsi = 0;
    g_cont_a_1fc80_rdi = nullptr;
    g_cont_a_1fc80_rsi_rva = 0;
    g_cont_b_1e4c0 = 0;
    g_cont_b_4c4950 = 0;
    g_cont_b_1e4c0_rdi = nullptr;
    g_cont_b_1e4c0_eax = 0;
    g_cont_b_4c4950_edi = 0;
    g_cont_b_rax = nullptr;
    g_cont_b_cell_32f84f8 = nullptr;
    g_cont_c_a52b70 = 0;
    g_cont_c_a599e0 = 0;
    g_cont_c_a431e0 = 0;
    g_cont_c_vcall = 0;
    g_cont_c_a52b70_rdi = nullptr;
    g_cont_c_a52b70_esi = 0;
    g_cont_c_a599e0_rdi = nullptr;
    g_cont_c_a599e0_esi = 0;
    g_cont_c_a431e0_rdi = nullptr;
    g_cont_c_a431e0_esi = 0;
    g_cont_c_vcall_esi[0] = 0;
    g_cont_c_vcall_esi[1] = 0;
    g_cont_c_vcall_esi[2] = 0;
    g_cont_c_cell_32f8b80 = nullptr;
    g_cont_c_cell_32f6d18 = nullptr;
    g_cont_c_cell_2efe318 = nullptr;
    g_cont_c_cell_2efe340 = nullptr;
    g_cont_c_flag1 = 0;
    g_cont_c_flag2 = 0;
    // E509 host stand-ins: *[0x32f79a8] -> 0x100 B object with u64 0x1234 @ +0xf8;
    // *[0x32f84f8] -> rdi marker; [r13] and 0x20(%rsp) -> distinct markers.
    std::memset(g_cont_a_obj, 0x5C, sizeof(g_cont_a_obj));
    {
        const std::uint64_t k = 0x1234ull;
        std::memcpy(g_cont_a_obj + 0xf8, &k, sizeof(k));
    }
    std::memset(g_cont_a_rdi_marker, 0x7D, sizeof(g_cont_a_rdi_marker));
    std::memset(g_cont_a_rsp20_obj, 0x6E, sizeof(g_cont_a_rsp20_obj));
    g_cont_a_r13_obj = g_cont_a_obj;
    g_cont_a_cell_32f79a8 = g_cont_a_obj;
    g_cont_a_cell_32f84f8 = g_cont_a_rdi_marker;
    g_cont_a_rsp20 = g_cont_a_rsp20_obj;
    // E510 host stand-ins: rax (incoming from 0x1fc80 NOT modelled),
    // *[0x32f84f8] -> rdi marker, observed call args.
    std::memset(g_cont_b_rax_obj, 0x6A, sizeof(g_cont_b_rax_obj));
    g_cont_b_rax = g_cont_b_rax_obj;
    g_cont_b_cell_32f84f8 = g_cont_a_rdi_marker;
    // E511 host stand-ins: obj A / obj B objects, vtable + flag-source cells,
    // and resolved flag bytes (flag1 -> 0 => sil 1; flag2 -> non-zero => sil 0).
    std::memset(g_cont_c_obj_a, 0x2B, sizeof(g_cont_c_obj_a));
    std::memset(g_cont_c_obj_b, 0x2C, sizeof(g_cont_c_obj_b));
    g_cont_c_cell_32f8b80 = g_cont_c_obj_a;
    g_cont_c_cell_32f6d18 = g_cont_c_obj_b;
    g_cont_c_cell_2efe318 = g_cont_a_rdi_marker;   // vtable source stand-in
    g_cont_c_cell_2efe340 = g_cont_a_rsp20_obj;    // flag source stand-in
    g_cont_c_flag1 = 0;   // byte == 0 -> sete sil = 1
    g_cont_c_flag2 = 0x5C;  // byte != 0 -> sete sil = 0
    g_post_base_cell_2eff450_dtor = 0;
    g_post_base_cell_2eff448_dtor = 0;
    g_cell_2eff450 = nullptr;
    g_cell_2eff448 = nullptr;
    g_lea_32f4fe0_r13 = nullptr;
    std::memset(g_cell_2eff450_neu, 0xa5, sizeof(g_cell_2eff450_neu));
    std::memset(g_cell_2eff448_neu, 0xa6, sizeof(g_cell_2eff448_neu));
    g_lea_vmovq_rbx = nullptr;
    // OBS src at 0x15ffa38: packed u64 0x538530 (first qword); poison xmm dst.
    {
        const std::uint64_t k = 0x538530ull;
        std::memcpy(g_lea_vmovq_src, &k, sizeof(k));
        std::memset(g_lea_vmovq_xmm0, 0xcc, sizeof(g_lea_vmovq_xmm0));
    }
    // Distinctive src; poison destinations so the AVX BSS copy is observable.
    for (int i = 0; i < 16; ++i) {
        g_avx_bss_src[i] = static_cast<std::uint8_t>(0xA0 + i);
        g_avx_bss_dst0[i] = 0x11;
        g_avx_bss_dst1[i] = 0x22;
    }
    // OBS lane at 0x1014a80: (-1,0,0,0,1,0,0,0) f32; poison destinations.
    static const std::uint32_t kYmmLane[8] = {
        0xbf800000u, 0, 0, 0, 0x3f800000u, 0, 0, 0,
    };
    std::memcpy(g_ymm_bss_src, kYmmLane, sizeof(kYmmLane));
    std::memset(g_ymm_bss_dst4278, 0x11, sizeof(g_ymm_bss_dst4278));
    std::memset(g_ymm_bss_dst426c, 0x22, sizeof(g_ymm_bss_dst426c));
    std::memset(g_ymm_bss_dst42dc, 0x33, sizeof(g_ymm_bss_dst42dc));
    std::memset(g_ymm_bss_dst4268, 0x44, sizeof(g_ymm_bss_dst4268));
    std::memset(g_ymm_ic_dst42a8, 0x55, sizeof(g_ymm_ic_dst42a8));
    std::memset(g_ymm_ic_dst430c, 0x66, sizeof(g_ymm_ic_dst430c));
    // OBS lane at 0x1014be0: (0,1,0,0) f32
    static const std::uint32_t kXmmLane[4] = {0, 0x3f800000u, 0, 0};
    std::memcpy(g_xmm_seed_src, kXmmLane, sizeof(kXmmLane));
    std::memset(g_xmm_seed_dst42d8, 0x77, sizeof(g_xmm_seed_dst42d8));
    std::memset(g_xmm_seed_dst430c, 0x88, sizeof(g_xmm_seed_dst430c));
    for (int i = 0; i < 0x30; ++i) {
        g_ymm_reload_src430c[i] = static_cast<std::uint8_t>(0xB0 + i);
        g_ymm_reload_dst433c[i] = 0x99;
    }
    stack_guard_value_cell() = 0xC4A4A70000000001ULL;
    std::memset(g_shadow_asset, 0, sizeof(g_shadow_asset));
    std::memset(g_skels_asset, 0, sizeof(g_skels_asset));
    std::memset(g_anims_asset, 0, sizeof(g_anims_asset));
    std::memset(g_banks_asset, 0, sizeof(g_banks_asset));
    std::memset(g_streams_asset, 0, sizeof(g_streams_asset));
    std::memset(g_rand_asset, 0, sizeof(g_rand_asset));
    std::memset(g_boss_asset, 0, sizeof(g_boss_asset));
    std::memset(g_cp3_asset, 0, sizeof(g_cp3_asset));
    std::memset(g_csp_asset, 0, sizeof(g_csp_asset));
    vslot40_shadow_loader_cell_2ee2cf8() = nullptr;
    ctor40_cell_2ee6778() = nullptr;
    singleton_f08_cell_2efc9e0() = nullptr;
    vslot40_cell_2ef8c08() = nullptr;
    prologue_gate_flag_byte() = 1;  // skip vslot+0x48
    prologue_debug_state_byte() = 0;
    vslot30_cell_boss_val_325e8b4() = -1;
    vslot30_cell_boss_mode_325e8b8() = -1;
    vslot30_cell_boss_flag_2dcdf14() = 0xFF;
    vslot30_cell_stage_2dd9578() = -1;

    Prologue255180Hooks eh{};
    eh.ctor_a30_264e50 = ctor_a30;
    eh.dtor_vslot08 = dtor_early;

    Prologue255180TailHooks th{};
    // ctor_40_1ab550 null → lifted init_ctor_40_1ab550 (slice also forces null)
    th.dtor_98a90_3be390 = dtor_98a90;
    th.dtor_vslot08_3b40 = dtor_3b40;
    th.run_gates = true;  // E417 default

    Ctor98a90_3be050Hooks ch{};
    ch.elem_dtor_f590 = hook_f590;
    ch.list_walk_nonnull_body = hook_nonnull;
    ch.tsc_detail.init_timebase_f8f860 = hook_init_tb;
    ch.ensure_detail.elem_move_1da30 = hook_move;
    // sub_3be930 override null → lifted init_sub_3be930

    ResourcePool pool{&g_pool_key_dealloc};
    ResourcePoolHooks pool_hooks{};
    pool_hooks.allocate = slice_pool_allocate;
    pool_hooks.deallocate = slice_pool_deallocate;
    pool_hooks.state_write = slice_pool_state_write;
    auto install_pool = [&](auto& detail) {
        detail.common.pool = &pool;
        detail.common.pool_hooks = pool_hooks;
    };

    BaseInitialize255130Hooks bh{};
    // vslot_30 / mid / vslot_40 null → slice forces lifted 0x534130 + 0x255340 + header
    bh.mid_255340_detail.register_26ea0 = hook_mid_26ea0;
    bh.mid_255340_detail.register_26b60 = hook_mid_26b60;
    bh.vslot_40_detail.ctor_493230 = hook_v40_ctor;
    bh.vslot_40_post_detail.call_525080 = hook_v40_525080;
    bh.vslot_40_post_detail.call_2569d0 = hook_v40_2569d0;
    bh.vslot_40_miss_detail.find_21620 = hook_v40_21620;
    bh.vslot_40_miss_detail.ba40 = hook_v40_ba40;
    bh.vslot_40_join_detail.call_42390 = hook_v40_42390;
    bh.vslot_40_join_detail.ctor_2779a0 = hook_v40_2779a0;
    bh.vslot_40_gfx_detail.call_6a5370 = hook_v40_6a5370;
    bh.vslot_40_gfx_detail.call_6abcc0 = hook_v40_6abcc0;
    bh.vslot_40_shadow_detail.path_14c2f0 = hook_v40_14c2f0;
    bh.vslot_40_shadow_detail.load_vslot18 = hook_v40_shadow_load;
    install_key_hooks(bh.vslot_40_shadow_attach_detail);
    install_pool(bh.vslot_40_shadow_attach_detail);
    bh.vslot_40_shadowskels_detail.notify_95580 = hook_v40_skels_notify;
    bh.vslot_40_shadowskels_detail.path_14c2f0 = hook_v40_skels_14c2f0;
    bh.vslot_40_shadowskels_detail.load_vslot18 = hook_v40_skels_load;
    install_key_hooks(bh.vslot_40_skels_attach_detail);
    install_pool(bh.vslot_40_skels_attach_detail);
    bh.vslot_40_shadowanims_detail.notify_95580 = hook_v40_anims_notify;
    bh.vslot_40_shadowanims_detail.path_14c2f0 = hook_v40_anims_14c2f0;
    bh.vslot_40_shadowanims_detail.load_vslot18 = hook_v40_anims_load;
    install_key_hooks(bh.vslot_40_anims_attach_detail);
    install_pool(bh.vslot_40_anims_attach_detail);
    bh.vslot_40_shadowbanks_detail.notify_95580 = hook_v40_banks_notify;
    bh.vslot_40_shadowbanks_detail.path_14c2f0 = hook_v40_banks_14c2f0;
    bh.vslot_40_shadowbanks_detail.load_vslot18 = hook_v40_banks_load;
    install_key_hooks(bh.vslot_40_banks_attach_detail);
    install_pool(bh.vslot_40_banks_attach_detail);
    bh.vslot_40_shadowstreams_detail.notify_95580 = hook_v40_streams_notify;
    bh.vslot_40_shadowstreams_detail.path_14c2f0 = hook_v40_streams_14c2f0;
    bh.vslot_40_shadowstreams_detail.load_vslot18 = hook_v40_streams_load;
    install_key_hooks(bh.vslot_40_streams_attach_detail);
    install_pool(bh.vslot_40_streams_attach_detail);
    bh.vslot_40_shadowrandomevents_detail.notify_95580 = hook_v40_rand_notify;
    bh.vslot_40_shadowrandomevents_detail.path_14c2f0 = hook_v40_rand_14c2f0;
    bh.vslot_40_shadowrandomevents_detail.load_vslot18 = hook_v40_rand_load;
    install_key_hooks(bh.vslot_40_randomevents_attach_detail);
    install_pool(bh.vslot_40_randomevents_attach_detail);
    bh.vslot_40_shadowbossmusic_detail.notify_95580 = hook_v40_boss_notify;
    bh.vslot_40_shadowbossmusic_detail.path_14c2f0 = hook_v40_boss_14c2f0;
    bh.vslot_40_shadowbossmusic_detail.load_vslot18 = hook_v40_boss_load;
    install_key_hooks(bh.vslot_40_bossmusic_attach_detail);
    install_pool(bh.vslot_40_bossmusic_attach_detail);
    bh.vslot_40_cp3characters_detail.notify_95580 = hook_v40_cp3_notify;
    bh.vslot_40_cp3characters_detail.path_14c2f0 = hook_v40_cp3_14c2f0;
    bh.vslot_40_cp3characters_detail.load_vslot18 = hook_v40_cp3_load;
    install_key_hooks(bh.vslot_40_cp3characters_attach_detail);
    install_pool(bh.vslot_40_cp3characters_attach_detail);
    bh.vslot_40_cp3shadowprecache_detail.notify_95580 = hook_v40_csp_notify;
    bh.vslot_40_cp3shadowprecache_detail.path_14c2f0 = hook_v40_csp_14c2f0;
    bh.vslot_40_cp3shadowprecache_detail.load_vslot18 = hook_v40_csp_load;
    install_key_hooks(bh.vslot_40_cp3shadowprecache_attach_detail);
    install_pool(bh.vslot_40_cp3shadowprecache_attach_detail);
    bh.vslot_40_tail_140d90_detail.call_140d90 = hook_v40_tail_140d90;
    bh.vslot_40_tail_140d90_detail.slot =
        reinterpret_cast<LockedResourceRef*>(app.data() + kVslot40Tail140d90SlotOff);
    bh.vslot_40_tail_140d90_detail.pool_hooks = pool_hooks;

    SingletonF08Hooks sh{};
    sh.ctor_523bd0 = hook_f08_ctor;
    sh.dtor_525b40 = hook_f08_dtor;

    Init531d60PostBase499f80Hooks pb{};
    pb.call_499f80 = hook_post_base_499f80;
    Init531d60PostBase56dcb0Hooks pb56{};
    pb56.call_56dcb0 = hook_post_base_56dcb0;
    Init531d60PostBase5c64c0Hooks pb5c{};
    pb5c.call_5c64c0 = hook_post_base_5c64c0;
    Init531d60PostBase1290Hooks pb1290{};
    pb1290.call_1290 = hook_post_base_1290;
    Init531d60PostBase56db70Hooks pb56db{};
    pb56db.call_56db70 = hook_post_base_56db70;
    Init531d60PostBase5b3d00Hooks pb5b3{};
    pb5b3.call_5b3d00 = hook_post_base_5b3d00;
    Init531d60PostBase5b3cf0Hooks pb5b3c{};
    pb5b3c.call_5b3cf0 = hook_post_base_5b3cf0;
    Init531d60PostBaseAvxBssCells pb_avx{};
    pb_avx.src_1b1dd40 = g_avx_bss_src;
    pb_avx.dst_1b1dd00 = g_avx_bss_dst0;
    pb_avx.dst_1b1dd20 = g_avx_bss_dst1;
    Init531d60PostBase7f7220Hooks pb7f{};
    pb7f.call_7f7220 = hook_post_base_7f7220;
    Init531d60PostBase56fad0Hooks pb56fa{};
    pb56fa.call_56fad0 = hook_post_base_56fad0;
    Init531d60PostBaseF90310Hooks pbf9{};
    pbf9.call_f90310 = hook_post_base_f90310;
    Init531d60PostBaseYmmBssCells pb_ymm{};
    pb_ymm.src_1014a80 = g_ymm_bss_src;
    pb_ymm.dst_1604278 = g_ymm_bss_dst4278;
    pb_ymm.dst_160426c = g_ymm_bss_dst426c;
    pb_ymm.dst_16042dc = g_ymm_bss_dst42dc;
    pb_ymm.dst_1604268 = g_ymm_bss_dst4268;
    Init531d60PostBaseYmmIntercellCells pb_ic{};
    // Sources are E457 seed destinations (post-seed contents).
    pb_ic.src_1604278 = g_ymm_bss_dst4278;
    pb_ic.src_16042dc = g_ymm_bss_dst42dc;
    pb_ic.dst_16042a8 = g_ymm_ic_dst42a8;
    pb_ic.dst_160430c = g_ymm_ic_dst430c;
    Init531d60PostBaseXmmSeedCells pb_xmm{};
    pb_xmm.src_1014be0 = g_xmm_seed_src;
    pb_xmm.dst_16042d8 = g_xmm_seed_dst42d8;
    pb_xmm.dst_160430c = g_xmm_seed_dst430c;
    Init531d60PostBaseYmmReloadCells pb_yr{};
    pb_yr.src_160430c = g_ymm_reload_src430c;
    pb_yr.dst_160433c = g_ymm_reload_dst433c;
    Init531d60PostBase26800F590Hooks pb268{};
    pb268.call_26800 = hook_post_base_26800;
    pb268.call_f590 = hook_post_base_f590;
    Init531d60PostBaseSbjHooks pbsbj{};
    pbsbj.call_26800 = hook_post_base_sbj_26800;
    pbsbj.call_f590 = hook_post_base_sbj_f590;
    Init531d60PostBaseSbojHooks pbsboj{};
    pbsboj.call_26800 = hook_post_base_sboj_26800;
    pbsboj.call_f590 = hook_post_base_sboj_f590;
    Init531d60PostBase52eb50Hooks pb52{};
    pb52.call_52eb50 = hook_post_base_52eb50;
    Init531d60PostBaseLeaVmovqCells pb_lv{};
    pb_lv.src_15ffa38 = g_lea_vmovq_src;
    pb_lv.dst_xmm0 = g_lea_vmovq_xmm0;
    pb_lv.rbx_cell = g_lea_vmovq_rbx_cell;
    pb_lv.rbx_out = &g_lea_vmovq_rbx;
    Init531d60PostBase532ca0Hooks pb532{};
    pb532.call_532ca0 = hook_post_base_532ca0;
    Init531d60PostBaseVcall18Hooks pbvc{};
    pbvc.call_vslot18 = hook_post_base_vcall18;
    Init531d60PostBase532ca0U2Hooks pb532u2{};
    pb532u2.call_532ca0 = hook_post_base_532ca0_u2;
    Init531d60PostBaseVcall18U2Hooks pbvcu2{};
    pbvcu2.call_vslot18 = hook_post_base_vcall18_u2;
    Init531d60PostBase532ca0U3Hooks pb532u3{};
    pb532u3.call_532ca0 = hook_post_base_532ca0_u3;
    Init531d60PostBaseVcall18U3Hooks pbvcu3{};
    pbvcu3.call_vslot18 = hook_post_base_vcall18_u3;
    Init531d60PostBase532ca0U4Hooks pb532u4{};
    pb532u4.call_532ca0 = hook_post_base_532ca0_u4;
    Init531d60PostBaseVcall18U4Hooks pbvcu4{};
    pbvcu4.call_vslot18 = hook_post_base_vcall18_u4;
    Init531d60PostBase532ca0U5Hooks pb532u5{};
    pb532u5.call_532ca0 = hook_post_base_532ca0_u5;
    Init531d60PostBaseVcall18U5Hooks pbvcu5{};
    pbvcu5.call_vslot18 = hook_post_base_vcall18_u5;
    Init531d60PostBase532ed0Hooks pb532ed{};
    pb532ed.call_532ed0 = hook_post_base_532ed0;
    Init531d60PostBaseVcall18532ed0Hooks pbvced{};
    pbvced.call_vslot18 = hook_post_base_vcall18_532ed0;
    Init531d60PostBase532ed0U2Hooks pb532edu2{};
    pb532edu2.call_532ed0 = hook_post_base_532ed0_u2;
    Init531d60PostBaseVcall18532ed0U2Hooks pbvcedu2{};
    pbvcedu2.call_vslot18 = hook_post_base_vcall18_532ed0_u2;
    Init531d60PostBase17070Hooks pb170{};
    pb170.call_17070 = hook_post_base_17070;
    Init531d60PostBase544fe0Hooks pb544{};
    pb544.call_544fe0 = hook_post_base_544fe0;
    Init531d60PostBaseCell2eff450Cells pb450c{};
    pb450c.neu = g_cell_2eff450_neu;
    pb450c.cell = &g_cell_2eff450;
    Init531d60PostBaseCell2eff450Hooks pb450h{};
    pb450h.dtor_vslot70 = hook_post_base_cell_2eff450_dtor;
    Init531d60PostBase17070U2Hooks pb170u2{};
    pb170u2.call_17070 = hook_post_base_17070_u2;
    Init531d60PostBase53fd10Hooks pb53{};
    pb53.call_53fd10 = hook_post_base_53fd10;
    Init531d60PostBaseCell2eff448Cells pb448c{};
    pb448c.neu = g_cell_2eff448_neu;
    pb448c.cell = &g_cell_2eff448;
    Init531d60PostBaseCell2eff448Hooks pb448h{};
    pb448h.dtor_vslot70 = hook_post_base_cell_2eff448_dtor;
    Init531d60PostBaseVcall38Hooks pbv38{};
    pbv38.call_vslot38 = hook_post_base_vcall38;
    Init531d60PostBaseLea32f4fe0Cells pblea{};
    pblea.r13_cell = g_lea_32f4fe0_cell;
    pblea.r13_out = &g_lea_32f4fe0_r13;
    Init531d60PostBaseA2caf0Hooks pba2{};
    pba2.call_a2caf0 = hook_post_base_a2caf0;
    Init531d60PostBaseA2ed00Hooks pba2ed{};
    pba2ed.call_a2ed00 = hook_post_base_a2ed00;
    Init531d60PostBaseF590A2ed00Hooks pbf590a{};
    pbf590a.call_f590 = hook_post_base_f590_a2ed00;
    Init531d60PostBaseF590A2ed00U2Hooks pbf590u2{};
    pbf590u2.call_f590 = hook_post_base_f590_a2ed00_u2;
    Init531d60PostBaseA2ed00U2Hooks pba2edu2{};
    pba2edu2.call_a2ed00 = hook_post_base_a2ed00_u2;
    Init531d60PostBaseF590A2ed00U3Hooks pbf590u3{};
    pbf590u3.call_f590 = hook_post_base_f590_a2ed00_u3;
    Init531d60PostBaseF590A2ed00U4Hooks pbf590u4{};
    pbf590u4.call_f590 = hook_post_base_f590_a2ed00_u4;
    Init531d60PostBaseA2ed00U3Hooks pba2edu3{};
    pba2edu3.call_a2ed00 = hook_post_base_a2ed00_u3;
    Init531d60PostBaseF590A2ed00U5Hooks pbf590u5{};
    pbf590u5.call_f590 = hook_post_base_f590_a2ed00_u5;
    Init531d60PostBaseF590A2ed00U6Hooks pbf590u6{};
    pbf590u6.call_f590 = hook_post_base_f590_a2ed00_u6;
    Init531d60PostBaseA2ed00U4Hooks pba2edu4{};
    pba2edu4.call_a2ed00 = hook_post_base_a2ed00_u4;
    Init531d60PostBaseF590A2ed00U7Hooks pbf590u7{};
    pbf590u7.call_f590 = hook_post_base_f590_a2ed00_u7;
    Init531d60PostBaseF590A2ed00U8Hooks pbf590u8{};
    pbf590u8.call_f590 = hook_post_base_f590_a2ed00_u8;
    Init531d60PostBaseA2ed00U5Hooks pba2edu5{};
    pba2edu5.call_a2ed00 = hook_post_base_a2ed00_u5;
    Init531d60PostBaseF590A2ed00U9Hooks pbf590u9{};
    pbf590u9.call_f590 = hook_post_base_f590_a2ed00_u9;
    Init531d60PostBaseF590A2ed00U10Hooks pbf590u10{};
    pbf590u10.call_f590 = hook_post_base_f590_a2ed00_u10;
    Init531d60PostBaseA2ed00U6Hooks pba2edu6{};
    pba2edu6.call_a2ed00 = hook_post_base_a2ed00_u6;
    Init531d60PostBaseF590A2ed00U11Hooks pbf590u11{};
    pbf590u11.call_f590 = hook_post_base_f590_a2ed00_u11;
    Init531d60PostBaseF590A2ed00U12Hooks pbf590u12{};
    pbf590u12.call_f590 = hook_post_base_f590_a2ed00_u12;
    Init531d60PostBaseA2ed00U7Hooks pba2edu7{};
    pba2edu7.call_a2ed00 = hook_post_base_a2ed00_u7;
    Init531d60PostBaseF590A2ed00U13Hooks pbf590u13{};
    pbf590u13.call_f590 = hook_post_base_f590_a2ed00_u13;
    Init531d60PostBaseF590A2ed00U14Hooks pbf590u14{};
    pbf590u14.call_f590 = hook_post_base_f590_a2ed00_u14;
    Init531d60PostBaseA2ea00U1Hooks pba2eu1{};
    pba2eu1.incoming_rbx = g_cont_a_obj;
    pba2eu1.call_a2ea00 = hook_cont_a_a2ea00;
    Init531d60PostBaseA2e5d0U1Hooks pba2e5{};
    pba2e5.incoming_rbx = g_cont_a_obj;
    pba2e5.call_a2e5d0 = hook_cont_a_a2e5d0;
    Init531d60PostBaseA543b0U1Hooks pba543{};
    pba543.cell_32f79a8 = &g_cont_a_cell_32f79a8;
    pba543.cell_32f84f8 = &g_cont_a_cell_32f84f8;
    pba543.call_a543b0 = hook_cont_a_a543b0;
    Init531d60PostBase1fc80U1Hooks pb1fc{};
    pb1fc.rsp20_slot = &g_cont_a_rsp20;
    pb1fc.call_1fc80 = hook_cont_a_1fc80;
    Init531d60PostBase1e4c0U1Hooks pb1e4{};
    pb1e4.incoming_rax_standin = g_cont_b_rax;
    pb1e4.call_1e4c0 = hook_cont_b_1e4c0;
    Init531d60PostBase4c4950U1Hooks pb4c{};
    pb4c.call_4c4950 = hook_cont_b_4c4950;
    Init531d60PostBaseA52b70U1Hooks pba52u1{};
    Init531d60PostBaseContinueAHooks pbca{};
    Init531d60PostBaseContinueBHooks pbcb{};
    pbcb.cell_32f84f8 = &g_cont_b_cell_32f84f8;
    Init531d60PostBaseContinueCHooks pbcc{};
    pbcc.incoming_r14b = 0;  // fall-through path r14d = 0
    pbcc.incoming_r15 = g_cont_a_rdi_marker;  // stable init pointer stand-in
    pbcc.cell_32f8b80 = &g_cont_c_cell_32f8b80;
    pbcc.cell_32f6d18 = &g_cont_c_cell_32f6d18;
    pbcc.cell_2efe318 = &g_cont_c_cell_2efe318;
    pbcc.cell_2efe340 = &g_cont_c_cell_2efe340;
    pbcc.flag_byte_iter1 = &g_cont_c_flag1;
    pbcc.flag_byte_iter2 = &g_cont_c_flag2;
    pbcc.call_a52b70 = hook_cont_c_a52b70;
    pbcc.call_a599e0 = hook_cont_c_a599e0;
    pbcc.call_a431e0 = hook_cont_c_a431e0;
    pbcc.vcall_slot0 = hook_cont_c_vcall;
    Init531d60PostBaseContinueDHooks pbcd{};
    pbcd.cell_32f79a8 = &g_cont_d_cell_value;
    pbcd.call_a473b0 = hook_cont_d_call;         // fallback path (unused when services supplied)
    pbcd.capture_services = &g_capture_a473b0;  // run the lifted 0xa473b0 body
    // continue_e hooks (only used when the je is taken; supplied as stand-in values).
    g_cont_e_r13 = g_cont_a_rdi_marker;  // same parent stand-in as r15
    Init531d60PostBaseContinueEHooks pbce{};
    Init531d60PostBaseRegistryTailHooks pbreg{};
    pbreg.merge_mov_edi_98_532b0c_u1_ran = false;
    pbreg.merge_call_17070_532b11_u1_ran = false;
    pbce.incoming_r13 = g_cont_e_r13;
    pbce.incoming_r12b = 0xBB;
    pbce.boss_flag_2dcdf14 = &g_boss_flag_cell;
    pbce.call_a54920 = hook_cont_e_a54920;
    pbce.call_54b250 = hook_cont_e_54b250;
    pbce.cell_2eff450 = &g_cell_2eff450_value;
    // E545: supply a real 0x1290 configuration so the lifted body (with both insert bodies)
    // executes at the 0x531e03 call site instead of the call-site stub.
    Container1beeb0 init1290_cont{};
    init_container_1bdb10(init1290_cont);
    Init531d60PostBaseAlt5327bdHooks pbalt{};  // no gate supplied => alt path not entered
    Init1290Hooks ih{};
    ih.container_a = &init1290_cont;
    ih.alloc_a.alloc = slice_1290_alloc;
    ih.vcall_1594060_slot3 = hook_slice_1290_vcall;
    ih.insert_21cd0 = hook_slice_1290_insert_b;

    const std::uint64_t allocs_before = heap.alloc_count;
    const auto t0 = std::chrono::steady_clock::now();
    const auto r = run_appmain_init_ctor_98a90_slice(table, front, app.data(), nullptr, eh, th, ch,
                                                     bh, setter.data(), 0, 0x11, 0x22, 0x33, sh, pb,
                                                     pb56, pb5c, pb1290, pb56db, pb5b3, pb5b3c,
                                                     pb_avx, pb7f, pb56fa, pbf9, pb_ymm, pb_ic,
                                                     pb_xmm, pb_yr, pb268, pbsbj, pbsboj, pb52,
                                                     pb_lv, pb532, pbvc, pb532u2, pbvcu2, pb532u3,
                                                     pbvcu3, pb532u4, pbvcu4, pb532u5, pbvcu5,
                                                     pb532ed, pbvced, pb532edu2, pbvcedu2, pb170,
                                                     pb544, pb450c, pb450h, pb170u2, pb53, pb448c,
                                                     pb448h, pbv38, pblea, pba2, pba2ed, pbf590a,
                                                     pbf590u2, pba2edu2, pbf590u3, pbf590u4,
                                                     pba2edu3, pbf590u5, pbf590u6, pba2edu4,
                                                     pbf590u7, pbf590u8, pba2edu5, pbf590u9,
                                                     pbf590u10, pba2edu6, pbf590u11, pbf590u12,
                                                     pba2edu7, pbf590u13, pbf590u14, pba2eu1, pba2e5,
                                                     pba543, pb1fc, pb1e4, pb4c, pba52u1, {}, {}, pbca, pbcb, pbcc, pbcd, pbce, pbreg, pbalt,
                                                     ih);
    const auto t1 = std::chrono::steady_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

    // E545: the native 0x1290 ran in the chain; its A insert executed through 0x1beeb0.
    CHECK(r.init_1290_native);
    CHECK(r.init_1290.ok);
    CHECK(init1290_cont.count3 == 1);
    CHECK(r.init_1290.inserted_a.ok);
    CHECK(g_slice_1290_vcall == 1);
    CHECK(!r.post_base_1290.ok);  // stub path not taken

    CHECK(r.early.ok);
    CHECK(r.tail.ok);
    CHECK(r.ctor.ok);
    CHECK(r.ctor.sub_3be930_ok);
    CHECK(r.ctor.ensure_allocated);
    CHECK(r.ctor.tsc_stored);
    CHECK(r.ctor.list_walk_skipped_null);
    CHECK(r.ctor_40.ok);
    CHECK(r.ctor_40.derived_vt);
    CHECK(r.ctor_40.cell_self);
    CHECK(r.ctor_40.field_0a);
    CHECK(r.tail.gates.ok);
    CHECK(r.tail.gates.gate_byte == 0);
    CHECK(!r.tail.gates.took_vslot_48);
    CHECK(r.tail.gates.ba40_ran);
    CHECK(!r.tail.gates.state_updated);  // al==state==0
    // lifted vslot_30: lookups miss → cells untouched; still returns true
    CHECK(vslot30_cell_boss_val_325e8b4() == -1);
    CHECK(vslot30_cell_stage_2dd9578() == -1);
    // lifted mid 0x255340: 4x26ea0 + 1x26b60
    CHECK(g_mid_reg == 4);
    CHECK(g_mid_b60 == 1);
    // E420: 0xf08 singleton install before base
    CHECK(r.singleton.ok);
    CHECK(r.singleton.ctor_ran);
    CHECK(g_f08_ctor == 1);
    CHECK(g_f08_dtor == 0);  // first install, no previous
    CHECK(singleton_f08_cell_2efc9e0() == r.singleton.obj);
    CHECK(r.singleton.obj != nullptr);
    CHECK(*reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(r.singleton.obj) + 0x30) ==
          0xF08F08F0u);
    // E421: bounded 0x533100 header install
    CHECK(r.vslot_40.ok);
    CHECK(r.vslot_40.ctor_ran);
    CHECK(g_v40_ctor == 1);
    CHECK(vslot40_cell_2ef8c08() == r.vslot_40.obj);
    CHECK(*static_cast<std::uint32_t*>(r.vslot_40.obj) == 0x53310040u);
    // E422: post-header fragment (empty table → lookup miss; still ok)
    CHECK(r.vslot_40_post.ok);
    CHECK(g_v40_525080 == 1);
    CHECK(g_v40_2569d0 == 1);
    CHECK(r.vslot_40_post.lookup_count == 1);
    CHECK(!r.vslot_40_post.lookup_hit);
    // E423: miss path (r_fps) after lookup miss
    CHECK(r.vslot_40_miss.ok);
    CHECK(r.vslot_40_miss.find_null);
    CHECK(g_v40_21620 == 1);
    CHECK(g_v40_ba40 == 1);
    CHECK(r.vslot_40_miss.dtor_f590_calls == 2);
    check_f590_release_noop(r.vslot_40_miss.release_f590_a);
    check_f590_release_noop(r.vslot_40_miss.release_f590_b);
    // E424: join+ Position install at self+0x3b60
    CHECK(r.vslot_40_join.ok);
    CHECK(r.vslot_40_join.replaced);
    CHECK(g_v40_42390 == 1);
    CHECK(g_v40_2779a0 == 1);
    CHECK(r.vslot_40_join.release_f590_count == 1);
    CHECK(r.vslot_40_join.dtor_f590_count == 1);
    CHECK(r.vslot_40_join.dtor_f590_element_count == 0);
    check_f590_release_noop(r.vslot_40_join.release_f590);
    CHECK(r.vslot_40_join.pos_obj != nullptr);
    CHECK(*static_cast<std::uint32_t*>(r.vslot_40_join.pos_obj) == 0x2779A001u);
    CHECK(loadp(app.data(), kVslot40JoinSlotOff) == r.vslot_40_join.pos_obj);
    // E425: gfx-cell prelude on 0x2ee2d00 before shadowmodels
    CHECK(r.vslot_40_gfx.ok);
    CHECK(g_v40_6a5370 == 1);
    CHECK(g_v40_6abcc0 == 1);
    CHECK(r.vslot_40_gfx.marker_stored);
    CHECK(r.vslot_40_gfx.marker_value == kVslot40GfxMarker);
    CHECK(r.vslot_40_gfx.obj == vslot40_gfx_cell_2ee2d00());
    // E426: shadowmodels load-attempt
    CHECK(r.vslot_40_shadow.ok);
    CHECK(g_v40_14c2f0 == 1);
    CHECK(g_v40_shadow_load == 1);
    CHECK(r.vslot_40_shadow.loaded == g_shadow_asset);
    CHECK(r.vslot_40_shadow.refcount_bumped);
    CHECK(r.vslot_40_shadow.refcount_after == 1);
    CHECK(r.vslot_40_shadow.f08 == r.singleton.obj);
    // E427: attach fail-path + install into f08+0
    CHECK(r.vslot_40_shadow_attach.ok);
    CHECK(r.vslot_40_shadow_attach.get_ok);
    CHECK(r.vslot_40_shadow_attach.slot_assigned);
    CHECK(r.vslot_40_shadow_attach.lock_ok);
    CHECK(r.vslot_40_shadow_attach.wrapper != nullptr);
    CHECK(loadp(r.singleton.obj, 0) == r.vslot_40_shadow_attach.wrapper);
    CHECK(r.vslot_40_shadow_attach.refs_after == 1);
    CHECK(r.vslot_40_shadow_attach.deps_after == 1);
    CHECK(r.vslot_40_shadow_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_shadow_attach.key_release);
    CHECK(r.vslot_40_shadow_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_shadow_attach.key_size == 31);
    CHECK(r.vslot_40_shadow_attach.key_capacity == 31);
    CHECK(r.vslot_40_shadow_attach.key_hash == 0x2128cdb9u);
    // E428: shadowskels load-attempt (notify + path + vcall + 14ccb0)
    CHECK(r.vslot_40_shadowskels.ok);
    CHECK(g_v40_skels_notify == 1);
    CHECK(g_v40_skels_14c2f0 == 1);
    CHECK(g_v40_skels_load == 1);
    CHECK(r.vslot_40_shadowskels.loaded == g_skels_asset);
    CHECK(r.vslot_40_shadowskels.refcount_bumped);
    CHECK(r.vslot_40_shadowskels.refcount_after == 1);
    CHECK(r.vslot_40_shadowskels.f08 == r.singleton.obj);
    // E429: skels attach fail-path + install into f08+0x10
    CHECK(r.vslot_40_skels_attach.ok);
    CHECK(r.vslot_40_skels_attach.get_ok);
    CHECK(r.vslot_40_skels_attach.slot_assigned);
    CHECK(r.vslot_40_skels_attach.lock_ok);
    CHECK(r.vslot_40_skels_attach.wrapper != nullptr);
    CHECK(loadp(r.singleton.obj, 0x10) == r.vslot_40_skels_attach.wrapper);
    CHECK(r.vslot_40_skels_attach.refs_after == 1);
    CHECK(r.vslot_40_skels_attach.deps_after == 1);
    CHECK(r.vslot_40_skels_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_skels_attach.key_release);
    CHECK(r.vslot_40_skels_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_skels_attach.key_size == 30);
    CHECK(r.vslot_40_skels_attach.key_capacity == 31);
    CHECK(r.vslot_40_skels_attach.key_hash == 0xecf0a769u);
    // E430: shadowanims load-attempt (notify f08+0x10 + path + vcall + 14ccb0)
    CHECK(r.vslot_40_shadowanims.ok);
    CHECK(g_v40_anims_notify == 1);
    CHECK(g_v40_anims_14c2f0 == 1);
    CHECK(g_v40_anims_load == 1);
    CHECK(r.vslot_40_shadowanims.loaded == g_anims_asset);
    CHECK(r.vslot_40_shadowanims.refcount_bumped);
    CHECK(r.vslot_40_shadowanims.refcount_after == 1);
    CHECK(r.vslot_40_shadowanims.f08 == r.singleton.obj);
    // E431: anims attach fail-path + install into f08+0x20
    CHECK(r.vslot_40_anims_attach.ok);
    CHECK(r.vslot_40_anims_attach.get_ok);
    CHECK(r.vslot_40_anims_attach.slot_assigned);
    CHECK(r.vslot_40_anims_attach.lock_ok);
    CHECK(r.vslot_40_anims_attach.wrapper != nullptr);
    CHECK(loadp(r.singleton.obj, 0x20) == r.vslot_40_anims_attach.wrapper);
    CHECK(r.vslot_40_anims_attach.refs_after == 1);
    CHECK(r.vslot_40_anims_attach.deps_after == 1);
    CHECK(r.vslot_40_anims_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_anims_attach.key_release);
    CHECK(r.vslot_40_anims_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_anims_attach.key_size == 30);
    CHECK(r.vslot_40_anims_attach.key_capacity == 31);
    CHECK(r.vslot_40_anims_attach.key_hash == 0x2b44f9dbu);
    // E432: shadowbanks load-attempt (notify f08+0x20 + path + vcall + 14ccb0)
    CHECK(r.vslot_40_shadowbanks.ok);
    CHECK(g_v40_banks_notify == 1);
    CHECK(g_v40_banks_14c2f0 == 1);
    CHECK(g_v40_banks_load == 1);
    CHECK(r.vslot_40_shadowbanks.loaded == g_banks_asset);
    CHECK(r.vslot_40_shadowbanks.refcount_bumped);
    CHECK(r.vslot_40_shadowbanks.refcount_after == 1);
    CHECK(r.vslot_40_shadowbanks.f08 == r.singleton.obj);
    // E433: banks attach fail-path + install into f08+0x40
    CHECK(r.vslot_40_banks_attach.ok);
    CHECK(r.vslot_40_banks_attach.get_ok);
    CHECK(r.vslot_40_banks_attach.slot_assigned);
    CHECK(r.vslot_40_banks_attach.lock_ok);
    CHECK(r.vslot_40_banks_attach.wrapper != nullptr);
    CHECK(loadp(r.singleton.obj, 0x40) == r.vslot_40_banks_attach.wrapper);
    CHECK(r.vslot_40_banks_attach.refs_after == 1);
    CHECK(r.vslot_40_banks_attach.deps_after == 1);
    CHECK(r.vslot_40_banks_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_banks_attach.key_release);
    CHECK(r.vslot_40_banks_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_banks_attach.key_size == 36);
    CHECK(r.vslot_40_banks_attach.key_capacity == 39);
    CHECK(r.vslot_40_banks_attach.key_hash == 0x3b87dd56u);
    // E434: shadowstreams load-attempt (notify f08+0x40 + path + vcall + 14ccb0)
    CHECK(r.vslot_40_shadowstreams.ok);
    CHECK(g_v40_streams_notify == 1);
    CHECK(g_v40_streams_14c2f0 == 1);
    CHECK(g_v40_streams_load == 1);
    CHECK(r.vslot_40_shadowstreams.loaded == g_streams_asset);
    CHECK(r.vslot_40_shadowstreams.refcount_bumped);
    CHECK(r.vslot_40_shadowstreams.refcount_after == 1);
    CHECK(r.vslot_40_shadowstreams.f08 == r.singleton.obj);
    // E435: streams attach fail-path + install into f08+0x50
    CHECK(r.vslot_40_streams_attach.ok);
    CHECK(r.vslot_40_streams_attach.get_ok);
    CHECK(r.vslot_40_streams_attach.slot_assigned);
    CHECK(r.vslot_40_streams_attach.lock_ok);
    CHECK(r.vslot_40_streams_attach.wrapper != nullptr);
    CHECK(loadp(r.singleton.obj, 0x50) == r.vslot_40_streams_attach.wrapper);
    CHECK(r.vslot_40_streams_attach.refs_after == 1);
    CHECK(r.vslot_40_streams_attach.deps_after == 1);
    CHECK(r.vslot_40_streams_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_streams_attach.key_release);
    CHECK(r.vslot_40_streams_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_streams_attach.key_size == 38);
    CHECK(r.vslot_40_streams_attach.key_capacity == 39);
    CHECK(r.vslot_40_streams_attach.key_hash == 0xf2163cc4u);
    // E436: shadowrandomevents load-attempt (notify f08+0x50 + path + vcall + 14ccb0)
    CHECK(r.vslot_40_shadowrandomevents.ok);
    CHECK(g_v40_rand_notify == 1);
    CHECK(g_v40_rand_14c2f0 == 1);
    CHECK(g_v40_rand_load == 1);
    CHECK(r.vslot_40_shadowrandomevents.loaded == g_rand_asset);
    CHECK(r.vslot_40_shadowrandomevents.refcount_bumped);
    CHECK(r.vslot_40_shadowrandomevents.refcount_after == 1);
    CHECK(r.vslot_40_shadowrandomevents.f08 == r.singleton.obj);
    // E437: randomevents attach fail-path + install into f08+0x60
    CHECK(r.vslot_40_randomevents_attach.ok);
    CHECK(r.vslot_40_randomevents_attach.get_ok);
    CHECK(r.vslot_40_randomevents_attach.slot_assigned);
    CHECK(r.vslot_40_randomevents_attach.lock_ok);
    CHECK(r.vslot_40_randomevents_attach.wrapper != nullptr);
    CHECK(loadp(r.singleton.obj, 0x60) == r.vslot_40_randomevents_attach.wrapper);
    CHECK(r.vslot_40_randomevents_attach.refs_after == 1);
    CHECK(r.vslot_40_randomevents_attach.deps_after == 1);
    CHECK(r.vslot_40_randomevents_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_randomevents_attach.key_release);
    CHECK(r.vslot_40_randomevents_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_randomevents_attach.key_size == 43);
    CHECK(r.vslot_40_randomevents_attach.key_capacity == 43);
    CHECK(r.vslot_40_randomevents_attach.key_hash == 0x94a8da3fu);
    // E438: shadowbossmusic load-attempt (notify f08+0x60 + path + vcall + 14ccb0)
    CHECK(r.vslot_40_shadowbossmusic.ok);
    CHECK(g_v40_boss_notify == 1);
    CHECK(g_v40_boss_14c2f0 == 1);
    CHECK(g_v40_boss_load == 1);
    CHECK(r.vslot_40_shadowbossmusic.loaded == g_boss_asset);
    CHECK(r.vslot_40_shadowbossmusic.refcount_bumped);
    CHECK(r.vslot_40_shadowbossmusic.refcount_after == 1);
    CHECK(r.vslot_40_shadowbossmusic.f08 == r.singleton.obj);
    // E439: bossmusic attach fail-path + install into f08+0x70
    CHECK(r.vslot_40_bossmusic_attach.ok);
    CHECK(r.vslot_40_bossmusic_attach.get_ok);
    CHECK(r.vslot_40_bossmusic_attach.slot_assigned);
    CHECK(r.vslot_40_bossmusic_attach.lock_ok);
    CHECK(r.vslot_40_bossmusic_attach.wrapper != nullptr);
    CHECK(loadp(r.singleton.obj, 0x70) == r.vslot_40_bossmusic_attach.wrapper);
    CHECK(r.vslot_40_bossmusic_attach.refs_after == 1);
    CHECK(r.vslot_40_bossmusic_attach.deps_after == 1);
    CHECK(r.vslot_40_bossmusic_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_bossmusic_attach.key_release);
    CHECK(r.vslot_40_bossmusic_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_bossmusic_attach.key_size == 40);
    CHECK(r.vslot_40_bossmusic_attach.key_capacity == 43);
    CHECK(r.vslot_40_bossmusic_attach.key_hash == 0xbfa276e7u);
    // E440: cp3Characters load-attempt (notify f08+0x70 + path + vcall; no 14ccb0)
    CHECK(r.vslot_40_cp3characters.ok);
    CHECK(g_v40_cp3_notify == 1);
    CHECK(g_v40_cp3_14c2f0 == 1);
    CHECK(g_v40_cp3_load == 1);
    CHECK(r.vslot_40_cp3characters.loaded == g_cp3_asset);
    CHECK(r.vslot_40_cp3characters.f08 == r.singleton.obj);
    // E441: cp3Characters attach fail-path + install into self+0x3cb8
    CHECK(r.vslot_40_cp3characters_attach.ok);
    CHECK(r.vslot_40_cp3characters_attach.get_ok);
    CHECK(r.vslot_40_cp3characters_attach.slot_assigned);
    CHECK(r.vslot_40_cp3characters_attach.lock_ok);
    CHECK(r.vslot_40_cp3characters_attach.wrapper != nullptr);
    CHECK(loadp(app.data(), kVslot40Cp3charactersAttachSlotOff) == r.vslot_40_cp3characters_attach.wrapper);
    CHECK(r.vslot_40_cp3characters_attach.refs_after == 1);
    CHECK(r.vslot_40_cp3characters_attach.deps_after == 1);
    CHECK(r.vslot_40_cp3characters_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_cp3characters_attach.key_release);
    CHECK(r.vslot_40_cp3characters_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_cp3characters_attach.key_size == 43);
    CHECK(r.vslot_40_cp3characters_attach.key_capacity == 43);
    CHECK(r.vslot_40_cp3characters_attach.key_hash == 0x99117412u);
    // E442: cp3shadowprecache load-attempt (notify self+0x3cb8 + path + vcall; no 14ccb0)
    CHECK(r.vslot_40_cp3shadowprecache.ok);
    CHECK(g_v40_csp_notify == 1);
    CHECK(g_v40_csp_14c2f0 == 1);
    CHECK(g_v40_csp_load == 1);
    CHECK(r.vslot_40_cp3shadowprecache.loaded == g_csp_asset);
    CHECK(r.vslot_40_cp3shadowprecache.self == app.data());
    // E443: cp3shadowprecache attach fail-path + install into self+0x3cc8
    CHECK(r.vslot_40_cp3shadowprecache_attach.ok);
    CHECK(r.vslot_40_cp3shadowprecache_attach.get_ok);
    CHECK(r.vslot_40_cp3shadowprecache_attach.slot_assigned);
    CHECK(r.vslot_40_cp3shadowprecache_attach.lock_ok);
    CHECK(r.vslot_40_cp3shadowprecache_attach.wrapper != nullptr);
    CHECK(loadp(app.data(), kVslot40Cp3shadowprecacheAttachSlotOff) == r.vslot_40_cp3shadowprecache_attach.wrapper);
    CHECK(r.vslot_40_cp3shadowprecache_attach.refs_after == 1);
    CHECK(r.vslot_40_cp3shadowprecache_attach.deps_after == 0);
    CHECK(r.vslot_40_cp3shadowprecache_attach.state_after == 11);
    check_key_release_miss(r.vslot_40_cp3shadowprecache_attach.key_release);
    CHECK(r.vslot_40_cp3shadowprecache_attach.dtor_f590_count == 1);
    CHECK(r.vslot_40_cp3shadowprecache_attach.key_size == 39);
    CHECK(r.vslot_40_cp3shadowprecache_attach.key_capacity == 39);
    CHECK(r.vslot_40_cp3shadowprecache_attach.key_hash == 0x13a0d114u);
    // All nine 0x14c2f0 keys: one tag-0 buffer + one tag-1 control each, all released.
    CHECK(g_key_alloc_tag0 == 9);
    CHECK(g_key_alloc_tag1 == 9);
    CHECK(g_key_free_tag0 == 0);  // allocator-contract frees only fire on alloc failure
    CHECK(g_key_free_tag1 == 0);
    CHECK(g_key_platform_tag_calls == 9);
    CHECK(g_key_platform_tag_crmp == 7);
    CHECK(g_key_platform_tag_calt == 2);

    // A second attach with the same key takes the GetResource hit path: the wrapper is unchanged,
    // the slot assignment is a no-op, the caller's key is the last reference, and Lock is idempotent.
    {
        g_key_platform_tag_calls = 0;
        Vslot40_533100ShadowAttachHooks mh{};
        install_key_hooks(mh);
        install_pool(mh);
        const auto mr = vslot_40_533100_shadow_attach(mh);
        CHECK(mr.ok);
        CHECK(mr.wrapper == r.vslot_40_shadow_attach.wrapper);
        CHECK(mr.prev_slot == mr.wrapper);
        CHECK(mr.key_release.previous == 1);
        check_key_release(mr.key_release);
        CHECK(mr.refs_after == 1);
        CHECK(mr.deps_after == 1);  // Lock is idempotent
        CHECK(mr.state_after == 11);
        CHECK(g_key_platform_tag_calls == 1);
        CHECK(pool.size() == std::size_t{9});
    }
    // E444: 0x140d90 Lock(slot=self+0x3cc8, 9) + the inert 0xf590; stop before stackchk.
    CHECK(r.vslot_40_tail_140d90.ok);
    CHECK(r.vslot_40_tail_140d90.lock_ok);
    CHECK(r.vslot_40_tail_140d90.state_after == 9);
    CHECK(r.vslot_40_tail_140d90.deps_after == 1);
    CHECK(g_v40_tail_140d90 == 0);  // the production slot path, not the host hook
    CHECK(r.vslot_40_tail_140d90.dtor_f590_count == 1);
    check_f590_release_noop(r.vslot_40_tail_140d90.release_f590);
    CHECK(r.vslot_40_tail_140d90.arg == 9);
    // E445: stack-check success path (saved==live) through ret
    CHECK(r.vslot_40_stackchk.ok);
    CHECK(r.vslot_40_stackchk.returned);
    CHECK(!r.vslot_40_stackchk.took_fail_path);
    CHECK(r.vslot_40_stackchk.saved == stack_guard_value_cell());
    CHECK(r.vslot_40_stackchk.live == r.vslot_40_stackchk.saved);
    CHECK(g_move == 0);
    CHECK(g_nonnull == 0);
    CHECK(g_init_tb == 1);
    CHECK(r.base_ok);
    // E446: Init post-base 0x499f80(self+0x3c70) call-site; stop before 0x56dcb0
    CHECK(r.post_base_499f80.ok);
    CHECK(g_post_base_499f80 == 1);
    CHECK(g_post_base_499f80_arg ==
          static_cast<std::uint8_t*>(app.data()) + kInit531d60PostBase499f80ArgOff);
    CHECK(r.post_base_499f80.arg == g_post_base_499f80_arg);
    // E447: Init continue 0x56dcb0 call-site; stop before 0x56dca0
    CHECK(r.post_base_56dcb0.ok);
    CHECK(g_post_base_56dcb0 == 1);
    CHECK(r.post_base_56dcb0.call_56dcb0_count == 1);
    // E448: Init continue 0x5c64c0 call-site; stop before 0x1290
    CHECK(r.post_base_5c64c0.ok);
    CHECK(g_post_base_5c64c0 == 1);
    CHECK(r.post_base_5c64c0.call_5c64c0_count == 1);
    // E449/E545: the 0x1290 call site. When a native configuration is supplied (E545) the
    // lifted body runs instead of the stub, so the stub counters stay zero.
    CHECK(r.post_base_1290.ok == !r.init_1290_native);
    CHECK(g_post_base_1290 == (r.init_1290_native ? 0 : 1));
    CHECK(r.post_base_1290.call_1290_count == (r.init_1290_native ? 0 : 1));
    // E450: Init continue 0x56db70 call-site (ret-nop); stop before 0x5b3d00
    CHECK(r.post_base_56db70.ok);
    CHECK(g_post_base_56db70 == 1);
    CHECK(r.post_base_56db70.call_56db70_count == 1);
    // E451: Init continue 0x5b3d00 call-site (ret-nop); stop before 0x5b3cf0
    CHECK(r.post_base_5b3d00.ok);
    CHECK(g_post_base_5b3d00 == 1);
    CHECK(r.post_base_5b3d00.call_5b3d00_count == 1);
    // E452: Init continue 0x5b3cf0 call-site (ret-nop); stop before AVX BSS copy
    CHECK(r.post_base_5b3cf0.ok);
    CHECK(g_post_base_5b3cf0 == 1);
    CHECK(r.post_base_5b3cf0.call_5b3cf0_count == 1);
    // E453: Init continue AVX BSS copy; stop before 0x7f7220
    CHECK(r.post_base_avx_bss.ok);
    CHECK(r.post_base_avx_bss.store_count == 2);
    CHECK(std::memcmp(g_avx_bss_dst0, g_avx_bss_src, 16) == 0);
    CHECK(std::memcmp(g_avx_bss_dst1, g_avx_bss_src, 16) == 0);
    // E454: Init continue 0x7f7220 call-site; stop before 0x56fad0
    CHECK(r.post_base_7f7220.ok);
    CHECK(g_post_base_7f7220 == 1);
    CHECK(r.post_base_7f7220.call_7f7220_count == 1);
    // E455: Init continue 0x56fad0 call-site (ret-nop); stop before 0xf90310
    CHECK(r.post_base_56fad0.ok);
    CHECK(g_post_base_56fad0 == 1);
    CHECK(r.post_base_56fad0.call_56fad0_count == 1);
    // E456: Init continue 0xf90310 call-site (splash-hide PLT); stop before ymm
    CHECK(r.post_base_f90310.ok);
    CHECK(g_post_base_f90310 == 1);
    CHECK(r.post_base_f90310.call_f90310_count == 1);
    // E457: Init continue ymm BSS seed; stop before inter-cell ymm / 0x26800
    CHECK(r.post_base_ymm_bss.ok);
    CHECK(r.post_base_ymm_bss.store_count == 4);
    CHECK(std::memcmp(g_ymm_bss_dst4278, g_ymm_bss_src, 32) == 0);
    {
        std::uint64_t q = 0;
        std::memcpy(&q, g_ymm_bss_dst4278 + 0x20, 8);
        CHECK(q == 0x3f800000ull);
        std::memcpy(&q, g_ymm_bss_dst4278 + 0x28, 8);
        CHECK(q == 0);
        std::uint32_t d = 0;
        std::memcpy(&d, g_ymm_bss_dst426c + 0, 4);
        CHECK(d == 0xbf800000u);
        std::memcpy(&d, g_ymm_bss_dst426c + 4, 4);
        CHECK(d == 0x3f800000u);
        std::memcpy(&d, g_ymm_bss_dst426c + 8, 4);
        CHECK(d == 0x3f800000u);
        CHECK(std::memcmp(g_ymm_bss_dst42dc, g_ymm_bss_src, 32) == 0);
        std::memcpy(&d, g_ymm_bss_dst42dc + 0x20, 4);
        CHECK(d == 0x3f800000u);
        std::memcpy(&d, g_ymm_bss_dst42dc + 0x24, 4);
        CHECK(d == 0);
        std::memcpy(&d, g_ymm_bss_dst42dc + 0x28, 4);
        CHECK(d == 0);
        std::memcpy(&d, g_ymm_bss_dst42dc + 0x2c, 4);
        CHECK(d == 0);
        std::memcpy(&d, g_ymm_bss_dst4268, 4);
        CHECK(d == 0x3f800000u);
    }
    // E458: Init continue ymm inter-cell copy; stop before xmm / 0x26800
    CHECK(r.post_base_ymm_intercell.ok);
    CHECK(r.post_base_ymm_intercell.store_count == 2);
    CHECK(std::memcmp(g_ymm_ic_dst42a8, g_ymm_bss_dst4278, 0x30) == 0);
    CHECK(std::memcmp(g_ymm_ic_dst430c, g_ymm_bss_dst42dc, 0x30) == 0);
    // E459: Init continue xmm BSS seed; stop before ymm reload / 0x26800
    CHECK(r.post_base_xmm_seed.ok);
    CHECK(r.post_base_xmm_seed.store_count == 2);
    {
        std::uint32_t d = 0;
        std::memcpy(&d, g_xmm_seed_dst42d8, 4);
        CHECK(d == 0x3f800000u);
        std::memcpy(&d, g_xmm_seed_dst430c + 0, 4);
        CHECK(d == 0xbf800000u);
        std::memcpy(&d, g_xmm_seed_dst430c + 4, 4);
        CHECK(d == 0);
        std::memcpy(&d, g_xmm_seed_dst430c + 8, 4);
        CHECK(d == 0);
        CHECK(std::memcmp(g_xmm_seed_dst430c + 0xc, g_xmm_seed_src, 16) == 0);
        std::uint64_t q = 0;
        std::memcpy(&q, g_xmm_seed_dst430c + 0x1c, 8);
        CHECK(q == 0x3f80000000000000ull);
    }
    // E460: Init continue ymm reload→0x160433c; stop before 0x26800
    CHECK(r.post_base_ymm_reload.ok);
    CHECK(r.post_base_ymm_reload.store_count == 1);
    CHECK(std::memcmp(g_ymm_reload_dst433c, g_ymm_reload_src430c, 0x30) == 0);
    // E490..E509: the 18 Init 0xf590 sites now go through the shared release contract.
    // `f590_override` returns a result only a host override can produce, so `had_control`
    // proves the module called the override and stored its result.
    const auto check_f590_override = [](const SharedReleaseF590Result& rel) {
        CHECK(rel.ok);
        CHECK(rel.had_control);
        CHECK(rel.decremented);
        CHECK(rel.was_last);
        CHECK(rel.destroyed);
        CHECK(rel.freed);
        CHECK(rel.previous == 1);
    };
    // E461: Init continue 0x26800/0xf590 call-sites (bodies NOT modelled)
    CHECK(r.post_base_26800_f590.ok);
    CHECK(g_post_base_26800 == 1);
    CHECK(g_post_base_f590 == 1);
    CHECK(r.post_base_26800_f590.call_26800_count == 1);
    CHECK(r.post_base_26800_f590.call_f590_count == 1);
    check_f590_override(r.post_base_26800_f590.release);
    // E462: Init continue Shadow Boss Jump call-sites (bodies NOT modelled)
    CHECK(r.post_base_sbj.ok);
    CHECK(g_post_base_sbj_26800 == 1);
    CHECK(g_post_base_sbj_f590 == 1);
    CHECK(r.post_base_sbj.call_26800_count == 1);
    CHECK(r.post_base_sbj.call_f590_count == 1);
    check_f590_override(r.post_base_sbj.release);
    // E463: Init continue Outro Jump call-sites (bodies / 0x52eb50 NOT modelled)
    CHECK(r.post_base_sboj.ok);
    CHECK(g_post_base_sboj_26800 == 1);
    CHECK(g_post_base_sboj_f590 == 1);
    CHECK(r.post_base_sboj.call_26800_count == 1);
    CHECK(r.post_base_sboj.call_f590_count == 1);
    check_f590_override(r.post_base_sboj.release);
    // E464: Init continue 0x52eb50 call-site (trampoline body NOT modelled)
    CHECK(r.post_base_52eb50.ok);
    CHECK(g_post_base_52eb50 == 1);
    CHECK(r.post_base_52eb50.call_52eb50_count == 1);
    // E465: Init continue lea rbx / vmovq (stack prep / 0x532ca0 NOT modelled)
    CHECK(r.post_base_lea_vmovq.ok);
    CHECK(r.post_base_lea_vmovq.lea_count == 1);
    CHECK(r.post_base_lea_vmovq.load_count == 1);
    CHECK(g_lea_vmovq_rbx == g_lea_vmovq_rbx_cell);
    CHECK(std::memcmp(g_lea_vmovq_xmm0, g_lea_vmovq_src, 8) == 0);
    // E466: Init continue 0x532ca0 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_532ca0.ok);
    CHECK(g_post_base_532ca0 == 1);
    CHECK(r.post_base_532ca0.call_532ca0_count == 1);
    // E467: Init continue virtual call [rax+0x18] (callee NOT modelled)
    CHECK(r.post_base_vcall18.ok);
    CHECK(g_post_base_vcall18 == 1);
    CHECK(r.post_base_vcall18.call_vslot18_count == 1);
    // E468: Init continue second 0x532ca0 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_532ca0_u2.ok);
    CHECK(g_post_base_532ca0_u2 == 1);
    CHECK(r.post_base_532ca0_u2.call_532ca0_count == 1);
    // E469: Init continue second virtual call [rax+0x18] (callee NOT modelled)
    CHECK(r.post_base_vcall18_u2.ok);
    CHECK(g_post_base_vcall18_u2 == 1);
    CHECK(r.post_base_vcall18_u2.call_vslot18_count == 1);
    // E470: Init continue third 0x532ca0 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_532ca0_u3.ok);
    CHECK(g_post_base_532ca0_u3 == 1);
    CHECK(r.post_base_532ca0_u3.call_532ca0_count == 1);
    // E471: Init continue third virtual call [rax+0x18] (callee NOT modelled)
    CHECK(r.post_base_vcall18_u3.ok);
    CHECK(g_post_base_vcall18_u3 == 1);
    CHECK(r.post_base_vcall18_u3.call_vslot18_count == 1);
    // E472: Init continue fourth 0x532ca0 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_532ca0_u4.ok);
    CHECK(g_post_base_532ca0_u4 == 1);
    CHECK(r.post_base_532ca0_u4.call_532ca0_count == 1);
    // E473: Init continue fourth virtual call [rax+0x18] (callee NOT modelled)
    CHECK(r.post_base_vcall18_u4.ok);
    CHECK(g_post_base_vcall18_u4 == 1);
    CHECK(r.post_base_vcall18_u4.call_vslot18_count == 1);
    // E474: Init continue fifth 0x532ca0 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_532ca0_u5.ok);
    CHECK(g_post_base_532ca0_u5 == 1);
    CHECK(r.post_base_532ca0_u5.call_532ca0_count == 1);
    // E475: Init continue fifth virtual call [rax+0x18] (callee NOT modelled)
    CHECK(r.post_base_vcall18_u5.ok);
    CHECK(g_post_base_vcall18_u5 == 1);
    CHECK(r.post_base_vcall18_u5.call_vslot18_count == 1);
    // E476: Init continue first 0x532ed0 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_532ed0.ok);
    CHECK(g_post_base_532ed0 == 1);
    CHECK(r.post_base_532ed0.call_532ed0_count == 1);
    // E477: Init continue first 0x532ed0-series virtual call [rax+0x18] (callee NOT modelled)
    CHECK(r.post_base_vcall18_532ed0.ok);
    CHECK(g_post_base_vcall18_532ed0 == 1);
    CHECK(r.post_base_vcall18_532ed0.call_vslot18_count == 1);
    // E478: Init continue second 0x532ed0 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_532ed0_u2.ok);
    CHECK(g_post_base_532ed0_u2 == 1);
    CHECK(r.post_base_532ed0_u2.call_532ed0_count == 1);
    // E479: Init continue second 0x532ed0-series virtual call [rax+0x18] (callee NOT modelled)
    CHECK(r.post_base_vcall18_532ed0_u2.ok);
    CHECK(g_post_base_vcall18_532ed0_u2 == 1);
    CHECK(r.post_base_vcall18_532ed0_u2.call_vslot18_count == 1);
    // E480: Init continue first 0x17070 call-site (arg prep / body NOT modelled)
    CHECK(r.post_base_17070.ok);
    CHECK(g_post_base_17070 == 1);
    CHECK(r.post_base_17070.call_17070_count == 1);
    // E481: Init continue first 0x544fe0 call-site (mov-rbx prep / body NOT modelled)
    CHECK(r.post_base_544fe0.ok);
    CHECK(g_post_base_544fe0 == 1);
    CHECK(r.post_base_544fe0.call_544fe0_count == 1);
    // E482: Init continue lea r14 / cell-install @ 0x2eff450 (vslot70 body NOT modelled)
    CHECK(r.post_base_cell_2eff450.ok);
    CHECK(r.post_base_cell_2eff450.installed);
    CHECK(!r.post_base_cell_2eff450.destroyed_prev);
    CHECK(g_post_base_cell_2eff450_dtor == 0);
    CHECK(g_cell_2eff450 == g_cell_2eff450_neu);
    CHECK(r.post_base_cell_2eff450.dtor_vslot70_count == 0);
    // E483: Init continue second 0x17070 call-site (arg prep / body NOT modelled)
    CHECK(r.post_base_17070_u2.ok);
    CHECK(g_post_base_17070_u2 == 1);
    CHECK(r.post_base_17070_u2.call_17070_count == 1);
    // E484: Init continue first 0x53fd10 call-site (mov-rbx prep / body NOT modelled)
    CHECK(r.post_base_53fd10.ok);
    CHECK(g_post_base_53fd10 == 1);
    CHECK(r.post_base_53fd10.call_53fd10_count == 1);
    // E485: Init continue lea r14 / cell-install @ 0x2eff448 (vslot70 body NOT modelled)
    CHECK(r.post_base_cell_2eff448.ok);
    CHECK(r.post_base_cell_2eff448.installed);
    CHECK(!r.post_base_cell_2eff448.destroyed_prev);
    CHECK(g_post_base_cell_2eff448_dtor == 0);
    CHECK(g_cell_2eff448 == g_cell_2eff448_neu);
    CHECK(r.post_base_cell_2eff448.dtor_vslot70_count == 0);
    // E486: Init continue virtual call [rax+0x38] (callee body NOT modelled)
    CHECK(r.post_base_vcall38.ok);
    CHECK(g_post_base_vcall38 == 1);
    CHECK(r.post_base_vcall38.call_vslot38_count == 1);
    // E487: Init continue lea r13 pin @ 0x32f4fe0 (call 0xa2caf0 NOT modelled here)
    CHECK(r.post_base_lea_32f4fe0.ok);
    CHECK(r.post_base_lea_32f4fe0.lea_count == 1);
    CHECK(g_lea_32f4fe0_r13 == g_lea_32f4fe0_cell);
    // E488: Init continue first 0xa2caf0 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_a2caf0.ok);
    CHECK(g_post_base_a2caf0 == 1);
    CHECK(r.post_base_a2caf0.call_a2caf0_count == 1);
    // E489: Init continue first 0xa2ed00 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_a2ed00.ok);
    CHECK(g_post_base_a2ed00 == 1);
    CHECK(r.post_base_a2ed00.call_a2ed00_count == 1);
    // E490: Init continue first post-a2ed00 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00.ok);
    CHECK(g_post_base_f590_a2ed00 == 1);
    CHECK(r.post_base_f590_a2ed00.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00.release);
    // E491: Init continue second post-a2ed00 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u2.ok);
    CHECK(g_post_base_f590_a2ed00_u2 == 1);
    CHECK(r.post_base_f590_a2ed00_u2.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u2.release);
    // E492: Init continue second 0xa2ed00 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_a2ed00_u2.ok);
    CHECK(g_post_base_a2ed00_u2 == 1);
    CHECK(r.post_base_a2ed00_u2.call_a2ed00_count == 1);
    // E493: Init continue first post-a2ed00-u2 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u3.ok);
    CHECK(g_post_base_f590_a2ed00_u3 == 1);
    CHECK(r.post_base_f590_a2ed00_u3.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u3.release);
    // E494: Init continue second post-a2ed00-u2 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u4.ok);
    CHECK(g_post_base_f590_a2ed00_u4 == 1);
    CHECK(r.post_base_f590_a2ed00_u4.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u4.release);
    // E495: Init continue third 0xa2ed00 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_a2ed00_u3.ok);
    CHECK(g_post_base_a2ed00_u3 == 1);
    CHECK(r.post_base_a2ed00_u3.call_a2ed00_count == 1);
    // E496: Init continue first post-a2ed00-u3 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u5.ok);
    CHECK(g_post_base_f590_a2ed00_u5 == 1);
    CHECK(r.post_base_f590_a2ed00_u5.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u5.release);
    // E497: Init continue second post-a2ed00-u3 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u6.ok);
    CHECK(g_post_base_f590_a2ed00_u6 == 1);
    CHECK(r.post_base_f590_a2ed00_u6.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u6.release);
    // E498: Init continue fourth 0xa2ed00 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_a2ed00_u4.ok);
    CHECK(g_post_base_a2ed00_u4 == 1);
    CHECK(r.post_base_a2ed00_u4.call_a2ed00_count == 1);
    // E499: Init continue first post-a2ed00-u4 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u7.ok);
    CHECK(g_post_base_f590_a2ed00_u7 == 1);
    CHECK(r.post_base_f590_a2ed00_u7.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u7.release);
    // E500: Init continue second post-a2ed00-u4 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u8.ok);
    CHECK(g_post_base_f590_a2ed00_u8 == 1);
    CHECK(r.post_base_f590_a2ed00_u8.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u8.release);
    // E501: Init continue fifth 0xa2ed00 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_a2ed00_u5.ok);
    CHECK(g_post_base_a2ed00_u5 == 1);
    CHECK(r.post_base_a2ed00_u5.call_a2ed00_count == 1);
    // E502: Init continue first post-a2ed00-u5 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u9.ok);
    CHECK(g_post_base_f590_a2ed00_u9 == 1);
    CHECK(r.post_base_f590_a2ed00_u9.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u9.release);
    // E503: Init continue second post-a2ed00-u5 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u10.ok);
    CHECK(g_post_base_f590_a2ed00_u10 == 1);
    CHECK(r.post_base_f590_a2ed00_u10.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u10.release);
    // E504: Init continue sixth 0xa2ed00 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_a2ed00_u6.ok);
    CHECK(g_post_base_a2ed00_u6 == 1);
    CHECK(r.post_base_a2ed00_u6.call_a2ed00_count == 1);
    // E505: Init continue first post-a2ed00-u6 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u11.ok);
    CHECK(g_post_base_f590_a2ed00_u11 == 1);
    CHECK(r.post_base_f590_a2ed00_u11.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u11.release);
    // E506: Init continue second post-a2ed00-u6 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u12.ok);
    CHECK(g_post_base_f590_a2ed00_u12 == 1);
    CHECK(r.post_base_f590_a2ed00_u12.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u12.release);
    // E507: Init continue seventh 0xa2ed00 call-site (stack prep / body NOT modelled)
    CHECK(r.post_base_a2ed00_u7.ok);
    CHECK(g_post_base_a2ed00_u7 == 1);
    CHECK(r.post_base_a2ed00_u7.call_a2ed00_count == 1);
    // E508: Init continue first post-a2ed00-u7 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u13.ok);
    CHECK(g_post_base_f590_a2ed00_u13 == 1);
    CHECK(r.post_base_f590_a2ed00_u13.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u13.release);
    // E509: Init continue second post-a2ed00-u7 0xf590 call-site (mov-rdi prep / body NOT modelled)
    CHECK(r.post_base_f590_a2ed00_u14.ok);
    CHECK(g_post_base_f590_a2ed00_u14 == 1);
    CHECK(r.post_base_f590_a2ed00_u14.call_f590_count == 1);
    check_f590_override(r.post_base_f590_a2ed00_u14.release);
    // E510: Init continue first 0xa2ea00 call-site (9 B prep / body NOT modelled)
    CHECK(r.post_base_a2ea00_u1.ok);
    CHECK(g_post_base_a2ea00_u1 == 1);
    CHECK(r.post_base_a2ea00_u1.call_a2ea00_count == 1);
    CHECK(r.post_base_a2ea00_u1.eax_return == 0x5A);
    CHECK(g_cont_a_a2ea00_rbx == g_cont_a_obj);
    // E511: Init continue first 0xa2e5d0 call-site (5 B prep / body NOT modelled)
    CHECK(r.post_base_a2e5d0_u1.ok);
    CHECK(g_post_base_a2e5d0_u1 == 1);
    CHECK(r.post_base_a2e5d0_u1.call_a2e5d0_count == 1);
    CHECK(g_cont_a_a2e5d0 == 1);
    CHECK(g_cont_a_a2e5d0_rbx == g_cont_a_obj);
    CHECK(g_cont_a_a2e5d0_esi == 0x5A);  // mov esi,eax stand-in from E510 u1
    // E512: Init continue first 0xa543b0 call-site (32 B prep / body NOT modelled)
    CHECK(r.post_base_a543b0_u1.ok);
    CHECK(g_post_base_a543b0_u1 == 1);
    CHECK(r.post_base_a543b0_u1.call_a543b0_count == 1);
    CHECK(g_cont_a_a543b0 == 1);
    CHECK(g_cont_a_a543b0_rdi == g_cont_a_rdi_marker);  // [0x32f84f8]
    CHECK(r.post_base_a543b0_u1.rsi_known);
    CHECK(r.post_base_a543b0_u1.rsi_at_0x532760 == 0x1274);  // 0x1234 + 0x40
    CHECK(g_cont_a_a543b0_rsi == 0x1274);
    // E513: Init continue first 0x1fc80 call-site (16 B prep / body NOT modelled)
    CHECK(r.post_base_1fc80_u1.ok);
    CHECK(g_post_base_1fc80_u1 == 1);
    CHECK(r.post_base_1fc80_u1.call_1fc80_count == 1);
    CHECK(g_cont_a_1fc80 == 1);
    CHECK(g_cont_a_1fc80_rsi_rva == 0x1014d69);  // pinned data RVA
    CHECK(static_cast<std::uint8_t*>(g_cont_a_1fc80_rdi) == g_cont_a_rsp20_obj + 8);
    // E519: grouped Init continue 0x53277a..0x53277f — 2 units, je boundary only,
    CHECK(r.post_base_continue_a.ok);
    CHECK(r.post_base_continue_a.unit_executed ==
          kInit531d60PostBaseContinueAUnitCount);
    CHECK(kInit531d60PostBaseContinueAUnitCount == 2);
    for (int i = 0; i < kInit531d60PostBaseContinueAUnitCount; ++i) {
        CHECK(r.post_base_continue_a.unit_counts[static_cast<std::size_t>(i)] == 1);
    }
    CHECK(kInit531d60PostBaseContinueAUnits[0].rva == 0x53277a);
    CHECK(kInit531d60PostBaseContinueAUnits[1].rva == 0x53277d);
    CHECK(r.post_base_continue_a.call_count == 0);
    CHECK(r.post_base_continue_a.lea_count == 0);
    CHECK(r.post_base_continue_a.load_count == 0);
    CHECK(r.post_base_continue_a.alu_count == 1);
    CHECK(r.post_base_continue_a.branch_boundary_count == 1);
    CHECK(!r.post_base_continue_a.branch_outcome_modelled);
    CHECK(r.post_base_continue_a.end_rva == 0x53277f);
    CHECK(r.post_base_continue_a.branch_target_rva == 0x5327bd);

    // The 17 Init 0xf590 sites
    // zeroed 32 B holder, so each must report an exact no-op and still count one call site.
    {
        check_init_f590_defaults(init_531d60_post_base_26800_f590());
        check_init_f590_defaults(init_531d60_post_base_sbj());
        check_init_f590_defaults(init_531d60_post_base_sboj());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u2());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u3());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u4());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u5());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u6());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u7());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u8());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u9());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u10());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u11());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u12());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u13());
        check_init_f590_defaults(init_531d60_post_base_f590_a2ed00_u14());
        const Init531d60PostBaseA2ea00U1Result a2eu1_default{init_531d60_post_base_a2ea00_u1()};
        CHECK(a2eu1_default.ok);
        CHECK(a2eu1_default.call_a2ea00_count == 1);
        const Init531d60PostBaseA2e5d0U1Result a2e5_default{init_531d60_post_base_a2e5d0_u1()};
        CHECK(a2e5_default.ok);
        CHECK(a2e5_default.call_a2e5d0_count == 1);
        const Init531d60PostBaseA543b0U1Result a543_default{init_531d60_post_base_a543b0_u1()};
        CHECK(a543_default.ok);
        CHECK(a543_default.call_a543b0_count == 1);
        const Init531d60PostBase1fc80U1Result fc80_default{init_531d60_post_base_1fc80_u1()};
        CHECK(fc80_default.ok);
        CHECK(fc80_default.call_1fc80_count == 1);
        const Init531d60PostBase1e4c0U1Result e4_default{init_531d60_post_base_1e4c0_u1()};
        CHECK(e4_default.ok);
        CHECK(e4_default.call_1e4c0_count == 1);
        const Init531d60PostBase4c4950U1Result c49_default{init_531d60_post_base_4c4950_u1()};
        CHECK(c49_default.ok);
        CHECK(c49_default.call_4c4950_count == 1);
        const Init531d60PostBaseContinueAResult cont_a_default{init_531d60_post_base_continue_a()};
        CHECK(cont_a_default.ok);
        CHECK(cont_a_default.unit_counts[static_cast<std::size_t>(ContinueAUnit::test_rax_rax)] == 1);
    }
    // E514: Init continue first 0x1e4c0 call-site (15 B prep NOT modelled)
    CHECK(r.post_base_1e4c0_u1.ok);
    CHECK(r.post_base_1e4c0_u1.call_1e4c0_count == 1);
    CHECK(g_cont_b_1e4c0 == 1);
    CHECK(g_cont_b_1e4c0_rdi == g_cont_b_rax_obj);
    CHECK(g_cont_b_1e4c0_eax == 0xA6);
    // E515: Init continue first 0x4c4950 call-site (13 B prep NOT modelled)
    CHECK(r.post_base_4c4950_u1.ok);
    CHECK(r.post_base_4c4950_u1.call_4c4950_count == 1);
    CHECK(r.post_base_4c4950_u1.edi_known);
    CHECK(r.post_base_4c4950_u1.edi_arg == 0xA7);
    CHECK(g_cont_b_4c4950 == 1);
    CHECK(g_cont_b_4c4950_edi == 0xA7);
    // E515: grouped Init continue B 0x5327a5..0x5327b1 — 3 units, 0 call sites,
    // 1 ALU op, 0..1 load; je6 target pinned but outcome NOT modelled when eax non-boolean.
    CHECK(r.post_base_continue_b.ok);
    CHECK(r.post_base_continue_b.unit_executed ==
          kInit531d60PostBaseContinueBUnitCount);
    CHECK(kInit531d60PostBaseContinueBUnitCount == 3);
    for (int i = 0; i < kInit531d60PostBaseContinueBUnitCount; ++i) {
        CHECK(r.post_base_continue_b.unit_counts[static_cast<std::size_t>(i)] == 1);
    }
    CHECK(kInit531d60PostBaseContinueBUnits[0].rva == 0x5327a5);
    CHECK(kInit531d60PostBaseContinueBUnits[2].rva == 0x5327ab);
    CHECK(r.post_base_continue_b.call_count == 0);
    CHECK(r.post_base_continue_b.alu_count == 1);
    CHECK(r.post_base_continue_b.load_count == 1);
    CHECK(r.post_base_continue_b.branch_boundary_count == 1);
    CHECK(!r.post_base_continue_b.branch_outcome_modelled);
    // E524: a non-boolean 0xA6 leaves the je outcome unmodelled, so the merge
    // block still runs exactly as before (no silent branch claim).
    CHECK(!r.post_base_continue_b.branch_taken);
    CHECK(!r.init_branch_5328e0_taken);
    CHECK(!r.post_base_continue_c.preamble_ran);
    CHECK(r.post_base_continue_b.end_rva == 0x5327b1);
    CHECK(r.post_base_continue_b.branch_target_rva == 0x5328e0);
    CHECK(r.post_base_continue_b.eax_known);
    CHECK(r.post_base_continue_b.eax_from_1e4c0 == 0xA6);

    // Batch A alt: je 0x5327bd taken path — 70-unit alt module runs when
    // caller supplies hooks.  Not taken in the default full-slice run.
    CHECK(!r.init_branch_5327bd_taken);
    CHECK(!r.post_base_alt_5327bd.ok);
    CHECK(!r.post_base_alt_5327bd.reached_via_jmp);
    CHECK(!r.post_base_alt_5327bd.reached_via_preamble);
    // E516: merge first 0xa52b70 call-site (7 B prep NOT modelled)
    CHECK(r.post_base_a52b70_u1.ok);
    CHECK(r.post_base_a52b70_u1.call_a52b70_count == 1);
    CHECK(g_cont_c_a52b70 == 1);
    CHECK(g_cont_c_a52b70_rdi == g_cont_a_rdi_marker);
    CHECK(g_cont_c_a52b70_esi == 0);
    // E517: merge first 0xa599e0 call-site (54 B prep NOT modelled)
    CHECK(r.post_base_a599e0_u1.ok);
    CHECK(r.post_base_a599e0_u1.call_a599e0_count == 1);
    CHECK(r.post_base_a599e0_u1.vcall_count == 1);
    // E518: merge second 0xa431e0 call-site (47 B prep NOT modelled)
    CHECK(r.post_base_a431e0_u1.ok);
    CHECK(r.post_base_a431e0_u1.call_a431e0_count == 1);
    CHECK(r.post_base_a431e0_u1.vcall_count == 1);
    // E518: grouped Init continue C 0x53297f..0x53299d — 8 units, 0 direct calls,
    CHECK(r.post_base_continue_c.ok);
    CHECK(r.post_base_continue_c.unit_executed ==
          kInit531d60PostBaseContinueCUnitCount);
    CHECK(kInit531d60PostBaseContinueCUnitCount == 8);
    for (int i = 0; i < kInit531d60PostBaseContinueCUnitCount; ++i) {
        CHECK(r.post_base_continue_c.unit_counts[static_cast<std::size_t>(i)] == 1);
    }
    CHECK(kInit531d60PostBaseContinueCUnits[0].rva == 0x53297f);
    CHECK(kInit531d60PostBaseContinueCUnits[7].rva == 0x53299b);
    CHECK(r.post_base_continue_c.call_count == 0);
    CHECK(r.post_base_continue_c.vcall_count == 1);
    CHECK(r.post_base_continue_c.lea_count == 0);
    CHECK(r.post_base_continue_c.global_mov_count == 2);
    CHECK(r.post_base_continue_c.xor_count == 1);
    CHECK(r.post_base_continue_c.cmp_count == 1);
    CHECK(r.post_base_continue_c.sete_count == 0);
    CHECK(r.post_base_continue_c.branch_boundary_count == 1);
    CHECK(!r.post_base_continue_c.merge_branch_outcome_modelled);
    CHECK(!r.init_branch_5329ae_taken);
    CHECK(!r.init_stopped_at_5329ae);
    CHECK(!r.post_base_continue_c.branch_outcome_modelled);
    CHECK(r.post_base_continue_c.end_rva == 0x53299d);
    CHECK(r.post_base_continue_c.branch_target_rva == 0x5329ae);
    CHECK(g_cont_c_a599e0 == 1);
    CHECK(g_cont_c_a431e0 == 1);
    CHECK(g_cont_c_vcall == 3);

    // E519: batch D call-site in a473b0_u1; continue_d boundary @ 0x5329ae (0 units).
    CHECK(r.post_base_continue_d.ok);
    CHECK(r.post_base_continue_d.unit_executed == 0);
    CHECK(r.post_base_continue_d.call_count == 1);
    CHECK(r.post_base_continue_d.load_count == 1);
    CHECK(g_cont_d_call == 0);  // native body replaced the raw hook
    CHECK(r.post_base_continue_d.capture_native);
    CHECK(r.post_base_continue_d.capture.ok);
    CHECK(r.post_base_continue_d.capture.enable == false);  // xor esi,esi -> enable=false
    CHECK(g_capture_a473b0.live == 1);
    CHECK(g_capture_a473b0.last_enable_arg == false);
    CHECK(g_capture_a473b0.screen_enable == 0);
    CHECK(g_capture_a473b0.screen_disable == 1);
    CHECK(g_capture_a473b0.video_calls == 2);
    CHECK(r.post_base_continue_d.end_rva == 0x5329ae);

    // E1042 / CPU-01-INIT-DRIVER-JOIN-01: fall-through runs D then E (12 pinned units) before remainder.
    CHECK(r.post_base_continue_e.ok);
    CHECK(r.post_base_continue_e.unit_executed == kInit531d60PostBaseContinueEUnitCount);
    CHECK(kInit531d60PostBaseContinueEUnitCount == 0);
    CHECK(r.post_base_registry_tail.ok);
    CHECK(r.post_base_registry_tail.unit_executed == kInit531d60PostBaseRegistryTailUnitCount);
    CHECK(kInit531d60PostBaseRegistryTailUnitCount == 1);
    CHECK(r.post_base_registry_tail.mov_edi_count == 1);
    CHECK(r.post_base_registry_tail.call_17070_count == 1);
    CHECK(r.post_base_registry_tail.end_rva == 0x532b16);
    CHECK(g_cont_e_a54920 == 3);  // pbce hooks: three save-data ctor calls on D→E path
    CHECK(r.init_r12b_known);
    CHECK(r.init_r12b_out == 0xA7);  // continue-B bl(0xA6) xor 1 feeds continue-E
    CHECK(g_boss_flag_cell == 0xA7);

    // Hook fallback: without a CaptureServices interface the raw call_a473b0 hook runs.
    {
        g_cont_d_call = 0;
        Init531d60PostBaseContinueDHooks fallback{};
        const void* fallback_cell = g_cont_d_cell_value;
        fallback.cell_32f79a8 = &fallback_cell;
        fallback.call_a473b0 = hook_cont_d_call;  // no capture_services
        const auto fr = init_531d60_post_base_continue_d(fallback);
        CHECK(fr.ok);
        CHECK(!fr.capture_native);
        CHECK(g_cont_d_call == 1);
        CHECK(g_cont_d_rdi == const_cast<void*>(g_cont_d_cell_value));
    }

    // E546 shrunk continue_e direct: 0x532b09..0x532b0c (1 unit); block-1/2/3 + mov4b0_rax +
    // mov_rdi_rbx peels
    // live in a54920_u1 + vcall18_532a17 + a54920_532a62_u1 + vcall18_532a72_u1 +
    // vmovq_532a75_u1 + lea_5388e0_532a7d_u1 + lea_rsp470_532a84_u1 + lea_rsi_532a8c_u1 +
    // movq_rsp4a0_532a93_u1 + mov_rdi_r13_532a9f_u1 + lea_538870_532aa3_u1 +
    // lea_rbx_rsp490_532aaa_u1 + mov_rsp470_rdx_532ab2_u1 + vmovdqa_532aba_u1 +
    // mov_rsp488_rax_532ac3_u1 + mov_rsp4b0_rsi_532acb_u1 + lea_rsi_2efda50_532ad3_u1 +
    // mov_rsp4b8_rcx_532ada_u1 merges.
    {
        g_cont_e_a54920 = 0;
        g_cont_e_54b250 = 0;
        alignas(16) std::uint8_t cont_e_xmm0[16]{};
        Init531d60PostBaseContinueEHooks eh{};
        eh.incoming_r13 = g_cont_e_r13;
        eh.merge_a54920_u1_ran = true;
        eh.merge_vcall18_532a17_ran = true;
        eh.merge_a54920_532a62_u1_ran = true;
        eh.merge_vcall18_532a72_u1_ran = true;
        eh.merge_vmovq_532a75_u1_ran = true;
        eh.merge_lea_5388e0_532a7d_u1_ran = true;
        eh.merge_lea_rsp470_532a84_u1_ran = true;
        eh.merge_lea_rsi_15c8e18_532a8c_u1_ran = true;
        eh.merge_movq_rsp4a0_0_532a93_u1_ran = true;
        eh.merge_mov_rdi_r13_532a9f_u1_ran = true;
        eh.merge_lea_538870_532aa3_u1_ran = true;
        eh.merge_lea_rbx_rsp490_532aaa_u1_ran = true;
        eh.merge_mov_rsp470_rdx_532ab2_u1_ran = true;
        eh.merge_vmovdqa_rsp478_xmm0_532aba_u1_ran = true;
        eh.merge_mov_rsp488_rax_532ac3_u1_ran = true;
        eh.merge_mov_rsp4b0_rsi_532acb_u1_ran = true;
        eh.merge_lea_rsi_2efda50_532ad3_u1_ran = true;
        eh.merge_mov_rsp4b8_rcx_532ada_u1_ran = true;
        eh.call_a54920 = hook_cont_e_a54920;
        eh.call_54b250 = hook_cont_e_54b250;
        eh.cell_2eff450 = &g_cell_2eff450_value;
        const auto vq = init_531d60_post_base_vmovq_532a75_u1(
            Init531d60PostBaseVmovq532a75_U1Cells{.dst_xmm0 = cont_e_xmm0});
        CHECK(vq.ok);
        CHECK(vq.vmovq_count == 1);
        void* lea_rax_out = nullptr;
        const auto lr = init_531d60_post_base_lea_5388e0_532a7d_u1(
            Init531d60PostBaseLea5388e0_532a7d_U1Cells{.rax_out = &lea_rax_out});
        CHECK(lr.ok);
        CHECK(lr.lea_rip_count == 1);
        void* lea_rdx_out = nullptr;
        const auto lrr = init_531d60_post_base_lea_rsp470_532a84_u1(
            Init531d60PostBaseLeaRsp470_532a84_U1Cells{.rdx_out = &lea_rdx_out});
        CHECK(lrr.ok);
        CHECK(lrr.lea_rsp_count == 1);
        void* lea_rsi_out = nullptr;
        const auto lsr = init_531d60_post_base_lea_rsi_15c8e18_532a8c_u1(
            Init531d60PostBaseLeaRsi15c8e18_532a8c_U1Cells{.rsi_out = &lea_rsi_out});
        CHECK(lsr.ok);
        CHECK(lsr.lea_rip_count == 1);
        CHECK(lea_rsi_out == reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x15c8e18)));
        const auto mqr = init_531d60_post_base_movq_rsp4a0_0_532a93_u1();
        CHECK(mqr.ok);
        CHECK(mqr.movq_zero_count == 1);
        const auto mrr = init_531d60_post_base_mov_rdi_r13_532a9f_u1(
            Init531d60PostBaseMovRdiR13_532a9f_U1Cells{.incoming_r13 = g_cont_e_r13});
        CHECK(mrr.ok);
        CHECK(mrr.mov_rdi_r13_count == 1);
        CHECK(mrr.parent == g_cont_e_r13);
        void* lea_rcx_out = nullptr;
        const auto lcr = init_531d60_post_base_lea_538870_532aa3_u1(
            Init531d60PostBaseLea538870_532aa3_U1Cells{.rcx_out = &lea_rcx_out});
        CHECK(lcr.ok);
        CHECK(lcr.lea_rip_count == 1);
        CHECK(lea_rcx_out == reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x538870)));
        void* lea_rbx_out = nullptr;
        const void* cont_e_rsp =
            reinterpret_cast<const std::uint8_t*>(g_cont_e_a54920_desc) -
            static_cast<std::uintptr_t>(0x470);
        const auto lbr = init_531d60_post_base_lea_rbx_rsp490_532aaa_u1(
            Init531d60PostBaseLeaRbxRsp490_532aaa_U1Cells{
                .stack_rsp = cont_e_rsp, .rbx_out = &lea_rbx_out});
        CHECK(lbr.ok);
        CHECK(lbr.lea_rsp_count == 1);
        alignas(32) std::uint8_t cont_e_desc3[0x50]{};
        void* cont_e_desc3_ptr = cont_e_desc3;
        void* desc_head = nullptr;
        const auto mhr = init_531d60_post_base_mov_rsp470_rdx_532ab2_u1(
            Init531d60PostBaseMovRsp470Rdx_532ab2_U1Cells{
                .head_out = &desc_head, .rdx_in = cont_e_desc3_ptr});
        CHECK(mhr.ok);
        CHECK(mhr.mov_rsp470_rdx_count == 1);
        const auto vdr = init_531d60_post_base_vmovdqa_rsp478_xmm0_532aba_u1(
            Init531d60PostBaseVmovdqaRsp478Xmm0_532aba_U1Cells{
                .xmm0_in = cont_e_xmm0, .dest_out = cont_e_desc3 + 8});
        CHECK(vdr.ok);
        CHECK(vdr.vmovdqa_count == 1);
        void* mov488_out = nullptr;
        const auto m488 = init_531d60_post_base_mov_rsp488_rax_532ac3_u1(
            Init531d60PostBaseMovRsp488Rax_532ac3_U1Cells{
                .thunk_out = &mov488_out, .rax_in = lea_rax_out});
        CHECK(m488.ok);
        CHECK(m488.mov_rsp488_rax_count == 1);
        void* mov4b0_out = nullptr;
        const auto m4b0 = init_531d60_post_base_mov_rsp4b0_rsi_532acb_u1(
            Init531d60PostBaseMovRsp4b0_Rsi_532acb_U1Cells{
                .vtable_stack_out = &mov4b0_out, .rsi_in = lea_rsi_out});
        CHECK(m4b0.ok);
        CHECK(m4b0.mov_rsp4b0_rsi_count == 1);
        void* lea_name3_out = nullptr;
        const auto lnr = init_531d60_post_base_lea_rsi_2efda50_532ad3_u1(
            Init531d60PostBaseLeaRsi2efda50_532ad3_U1Cells{.rsi_out = &lea_name3_out});
        CHECK(lnr.ok);
        CHECK(lnr.lea_rip_count == 1);
        CHECK(lea_name3_out == reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2efda50)));
        void* mov4b8_out = nullptr;
        const auto m4b8 = init_531d60_post_base_mov_rsp4b8_rcx_532ada_u1(
            Init531d60PostBaseMovRsp4b8_Rcx_532ada_U1Cells{
                .thunk_out = &mov4b8_out, .rcx_in = lea_rcx_out});
        CHECK(m4b8.ok);
        CHECK(m4b8.mov_rsp4b8_rcx_count == 1);
        const auto er = init_531d60_post_base_continue_e(eh);
        CHECK(er.ok);
        CHECK(er.unit_executed == kInit531d60PostBaseContinueEUnitCount);
        CHECK(kInit531d60PostBaseContinueEUnitCount == 0);
        CHECK(kInit531d60PostBaseContinueEUnits.empty());
        CHECK(er.call_a54920_count == 1);
        CHECK(er.vcall_noop_count == 1);
        CHECK(er.vcall_54b250_count == 1);
        CHECK(er.stamp_count == 0);
        CHECK(er.lea_rip_count == 1);
        CHECK(er.vmovq_count == 0);
        CHECK(er.end_rva == 0x532b0c);
        CHECK(er.a54920_hook_supplied);
        CHECK(g_cont_e_a54920 == 1);
        CHECK(g_cont_e_a54920_name[0] ==
              reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x2efda50)));
        CHECK(g_cont_e_a54920_parent[0] == g_cont_e_r13);
        CHECK(g_cont_e_a54920_desc[0] != nullptr);
        {
            const auto* dsc = static_cast<const std::uint8_t*>(g_cont_e_a54920_desc[0]);
            void* vt = nullptr;
            std::memcpy(&vt, dsc + 0x40, sizeof(void*));
            CHECK(vt == reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x15c8e18)));
            void* sp = nullptr;
            std::memcpy(&sp, dsc + 0x20, sizeof(void*));
            CHECK(reinterpret_cast<std::uintptr_t>(sp) ==
                  reinterpret_cast<std::uintptr_t>(g_cont_e_a54920_desc[0]) + 0x20);
        }
        CHECK(g_cont_e_54b250 == 1);
        CHECK(g_cont_e_54b250_obj == g_cell_2eff450_value);

        g_cont_e_a54920 = 0;
        g_cont_e_54b250 = 0;
        Init531d60PostBaseContinueEHooks empty{};
        empty.merge_a54920_u1_ran = true;
        empty.merge_vcall18_532a17_ran = true;
        empty.merge_a54920_532a62_u1_ran = true;
        empty.merge_vcall18_532a72_u1_ran = true;
        empty.merge_vmovq_532a75_u1_ran = true;
        empty.merge_lea_5388e0_532a7d_u1_ran = true;
        empty.merge_lea_rsp470_532a84_u1_ran = true;
        empty.merge_lea_rsi_15c8e18_532a8c_u1_ran = true;
        empty.merge_movq_rsp4a0_0_532a93_u1_ran = true;
        empty.merge_mov_rdi_r13_532a9f_u1_ran = true;
        empty.merge_lea_538870_532aa3_u1_ran = true;
        empty.merge_lea_rbx_rsp490_532aaa_u1_ran = true;
        empty.merge_mov_rsp470_rdx_532ab2_u1_ran = true;
        empty.merge_vmovdqa_rsp478_xmm0_532aba_u1_ran = true;
        empty.merge_mov_rsp488_rax_532ac3_u1_ran = true;
        empty.merge_mov_rsp4b0_rsi_532acb_u1_ran = true;
        empty.merge_lea_rsi_2efda50_532ad3_u1_ran = true;
        empty.merge_mov_rsp4b8_rcx_532ada_u1_ran = true;
        empty.merge_a54920_532ae2_u1_ran = true;
        empty.merge_mov_rsp4b0_rax_532ae7_u1_ran = true;
        empty.merge_mov_rdi_rbx_532aef_u1_ran = true;
        empty.merge_vcall18_532af2_u1_ran = true;
        empty.merge_lea_rax_2eff450_532af5_u1_ran = true;
        empty.merge_xor_esi_532afc_u1_ran = true;
        empty.merge_mov_edx_2_532afe_u1_ran = true;
        empty.merge_mov_rdi_rax_mem_532b03_u1_ran = true;
        empty.merge_mov_rax_rdi_mem_532b06_u1_ran = true;
        empty.merge_vcall08_532b09_u1_ran = true;
        const auto er2 = init_531d60_post_base_continue_e(empty);
        CHECK(er2.ok);
        CHECK(er2.unit_executed == 0);
        CHECK(er2.call_a54920_count == 0);
        CHECK(er2.vcall_noop_count == 0);
        CHECK(er2.vcall_54b250_count == 0);
        CHECK(er2.stamp_count == 0);
        CHECK(!er2.a54920_hook_supplied);
        CHECK(g_cont_e_a54920 == 0);
        CHECK(g_cont_e_54b250 == 0);
    }

    CHECK(r.post_base_continue_c.flag1_known);
    CHECK(g_cont_c_a599e0_rdi == g_cont_c_obj_a);       // obj A
    CHECK(g_cont_c_a599e0_esi == 1);                    // flag1 byte == 0 -> sil 1
    CHECK(g_cont_c_a431e0 == 1);
    CHECK(g_cont_c_a431e0_rdi == g_cont_c_obj_b);       // obj B
    CHECK(g_cont_c_a431e0_esi == 0);                    // flag2 byte != 0 -> sil 0
    CHECK(g_cont_c_vcall == 3);
    CHECK(g_cont_c_vcall_esi[0] == 0);
    CHECK(g_cont_c_vcall_esi[1] == 0);
    CHECK(g_cont_c_vcall_esi[2] == 0);
    CHECK(r.post_base_continue_c.flag1_known);
    CHECK(r.post_base_continue_c.flag2_known);
    CHECK(r.post_base_continue_c.sil_iter1 == 1);
    CHECK(r.post_base_continue_c.sil_iter2 == 0);
    CHECK(r.remainder.trivial_ok);
    CHECK(r.remainder.registry.status == RegistryInit531d60Status::ok);
    CHECK(r.remainder.registry.entry_count == kInputTable551140Count);
    // + ensure 0xa800 alloc
    CHECK(heap.alloc_count - allocs_before >= 10);

    void* eb = loadp(r.tail.obj_98a90, 0x28);
    void* ec = loadp(r.tail.obj_98a90, 0x38);
    CHECK(eb != nullptr);
    CHECK(static_cast<std::uint8_t*>(ec) == static_cast<std::uint8_t*>(eb) + 0xa800);
    CHECK(r.tail.obj_40 != nullptr);
    CHECK(loadp(r.tail.obj_40, 0) == ctor40_derived_vtable_1598ae0());
    CHECK(ctor40_cell_2ee6778() == r.tail.obj_40);

    // E524 direct coverage: the 0x1e4c0 contract makes bl a proven boolean, so
    // the je outcome is modelled (bl == 0 => taken) and the slice stops at the
    // unmodelled 0x5328e0 preamble instead of entering the merge block.
    {
        Init531d60PostBase1e4c0U1Hooks u1{};
        u1.call_1e4c0 = hook_cont_b_1e4c0;
        auto run_b = [&](int val) {
            g_cont_b_1e4c0_value = val;
            const Init531d60PostBase1e4c0U1Result e4 = init_531d60_post_base_1e4c0_u1(u1);
            Init531d60PostBase4c4950U1Hooks u4{};
            u4.incoming_eax_from_1e4c0_u1 = e4.eax_return;
            u4.incoming_eax_known = e4.eax_known;
            (void)init_531d60_post_base_4c4950_u1(u4);
            Init531d60PostBaseContinueBHooks h{};
            h.incoming_eax_from_1e4c0_u1 = e4.eax_return;
            h.incoming_eax_known = e4.eax_known;
            return init_531d60_post_base_continue_b(h);
        };
        g_cont_b_1e4c0_value = 1;
        const auto b1 = run_b(1);
        CHECK(b1.ok);
        CHECK(b1.branch_outcome_modelled);
        CHECK(b1.bl == 1);
        CHECK(!b1.branch_taken);
        g_cont_b_1e4c0_value = 0;
        const auto b0 = run_b(0);
        CHECK(b0.ok);
        CHECK(b0.branch_outcome_modelled);
        CHECK(b0.bl == 0);
        CHECK(b0.branch_taken);
        // The non-boolean default keeps the outcome unmodelled.
        g_cont_b_1e4c0_value = 0xA6;
        const auto bx = run_b(0xA6);
        CHECK(!bx.branch_outcome_modelled && !bx.branch_taken);

        // Slice-level decision: not-taken and unknown both enter the merge; a
        // proven taken branch runs the 0x5328e0 preamble first.
        CHECK(enters_merge_after_continue_b(b1));
        CHECK(!enters_merge_after_continue_b(b0));
        CHECK(enters_merge_after_continue_b(bx));

        // E529 taken-path preamble: sete r14b from the resolved flag byte, fed
        // into the merge entry's movzx esi,r14b.
        std::uint8_t flag_zero = 0;
        std::uint8_t flag_one = 1;
        Init531d60PostBaseContinueCHooks ch{};
        ch.enter_via_5328e0 = true;
        ch.preamble_flag_byte = &flag_zero;
        ch.vcall_slot0 = hook_extra_slot0;
        const auto c0 = init_531d60_post_base_continue_c(ch);
        CHECK(c0.ok);
        CHECK(c0.preamble_ran);
        CHECK(c0.preamble_unit_executed == 8);
        CHECK(c0.preamble_call_count == 1);
        CHECK(c0.preamble_flag_known);
        CHECK(c0.preamble_r14b == 1);   // (byte == 0) => 1
        CHECK(c0.first_esi == 1);

        ch.preamble_flag_byte = &flag_one;
        const auto c1 = init_531d60_post_base_continue_c(ch);
        CHECK(c1.ok && c1.preamble_ran);
        CHECK(c1.preamble_r14b == 0);   // (byte != 0) => 0
        CHECK(c1.first_esi == 0);

        // E536: merge `je 0x5329ae` is taken when the resolved flag byte is 0, and the
        // slice must stop before the unmodelled 0x5329ae block instead of running the
        // fall-through.
        {
            std::uint8_t merge_zero = 0;
            Init531d60PostBaseContinueCHooks m{};
            m.merge_flag_byte = &merge_zero;
            const auto cm = init_531d60_post_base_continue_c(m);
            CHECK(cm.ok);
            CHECK(cm.merge_branch_outcome_modelled);
            CHECK(cm.merge_branch_taken);
            std::uint8_t merge_one = 1;
            m.merge_flag_byte = &merge_one;
            const auto cm1 = init_531d60_post_base_continue_c(m);
            CHECK(cm1.ok && cm1.merge_branch_outcome_modelled);
            CHECK(!cm1.merge_branch_taken);
            Init531d60PostBaseContinueCHooks mu{};
            const auto cmu = init_531d60_post_base_continue_c(mu);
            CHECK(!cmu.merge_branch_outcome_modelled && !cmu.merge_branch_taken);
        }

        // Missing hook data on the taken path fails visibly.
        Init531d60PostBaseContinueCHooks bad{};
        bad.enter_via_5328e0 = true;    // no flag byte, no slot-0 hook
        const auto cb = init_531d60_post_base_continue_c(bad);
        CHECK(!cb.ok);
        CHECK(!cb.preamble_ran);
    }

    std::printf(
        "appmain_init_ctor_98a90_slice: MEASURED lifted_3be050_header+sub3be930+ensure3c2f70+"
        "buf80+null_list+tsc323d0+ctor40_1ab550+gates2552d4+singleton_f08+vslot534130+mid255340+"
        "vslot533100_header+post1fc80+miss_rfps+join_pos+gfx2ee2d00+shadowmodels+attach+"
        "shadowskels+skels_attach+shadowanims+anims_attach+shadowbanks+banks_attach+"
        "shadowstreams+streams_attach+shadowrandomevents+randomevents_attach+shadowbossmusic+"
        "bossmusic_attach+cp3characters+cp3characters_attach+cp3shadowprecache+"
        "cp3shadowprecache_attach+tail_140d90+stackchk+post_base_499f80+post_base_56dcb0+"
        "post_base_5c64c0+post_base_1290+post_base_56db70+post_base_5b3d00+post_base_5b3cf0+"
        "post_base_avx_bss+post_base_7f7220+post_base_56fad0+post_base_f90310+"
        "post_base_ymm_bss+post_base_ymm_intercell+post_base_xmm_seed+"
        "post_base_ymm_reload+post_base_26800_f590+post_base_sbj+post_base_sboj+"
        "post_base_52eb50+post_base_lea_vmovq+post_base_532ca0+post_base_vcall18+"
        "post_base_532ca0_u2+post_base_vcall18_u2+post_base_532ca0_u3+"
        "post_base_vcall18_u3+post_base_532ca0_u4+post_base_vcall18_u4+"
        "post_base_532ca0_u5+post_base_vcall18_u5+post_base_532ed0+"
        "post_base_vcall18_532ed0+post_base_532ed0_u2+post_base_vcall18_532ed0_u2+"
        "post_base_17070+post_base_544fe0+post_base_cell_2eff450+"
        "post_base_17070_u2+post_base_53fd10+post_base_cell_2eff448+"
        "post_base_vcall38+post_base_lea_32f4fe0+post_base_a2caf0+"
        "post_base_a2ed00+post_base_f590_a2ed00+post_base_f590_a2ed00_u2+"
        "post_base_a2ed00_u2+post_base_f590_a2ed00_u3+"
        "post_base_f590_a2ed00_u4+post_base_a2ed00_u3+"
        "post_base_f590_a2ed00_u5+post_base_f590_a2ed00_u6+"
        "post_base_a2ed00_u4+post_base_f590_a2ed00_u7+"
        "post_base_f590_a2ed00_u8+post_base_a2ed00_u5+"
        "post_base_f590_a2ed00_u9+post_base_f590_a2ed00_u10+"
        "post_base_a2ed00_u6+post_base_f590_a2ed00_u11+"
        "post_base_f590_a2ed00_u12+post_base_a2ed00_u7+"
        "post_base_f590_a2ed00_u13+"
        "post_base_f590_a2ed00_u14+"
        "post_base_a2ea00_u1+"
        "post_base_a2e5d0_u1+"
        "post_base_a543b0_u1+"
        "post_base_1fc80_u1+"
        "post_base_1e4c0_u1+"
        "post_base_4c4950_u1+"
        "post_base_continue_a+"
        "early+tail+base+trivial+registry "
        "ok nested=%p ensure=%p obj40=%p f08=%p v40=%p pos=%p gfx=%p shadow=%p skels=%p "
        "anims=%p banks=%p streams=%p rand=%p boss=%p cp3=%p csp=%p post_hit=%d miss_null=%d "
        "marker=0x%x refc=%u fail=%d skels_refc=%u skels_fail=%d anims_refc=%u anims_fail=%d "
        "banks_refc=%u banks_fail=%d streams_refc=%u streams_fail=%d rand_refc=%u rand_fail=%d "
        "boss_refc=%u boss_fail=%d cp3_fail=%d csp_fail=%d t140=%d schk=%d pb499=%d pb56=%d "
        "pb5c=%d pb1290=%d pb56db=%d pb5b3=%d pb5b3c=%d pbavx=%d pb7f=%d pb56fa=%d pbf9=%d "
        "pbymm=%d pbic=%d pbxmm=%d pbyr=%d pb268=%d pbf590=%d pbsbj268=%d pbsbjf590=%d "
        "pbsboj268=%d pbsbojf590=%d pb52=%d pblv=%d pb532=%d pbvc=%d pb532u2=%d pbvcu2=%d "
        "pb532u3=%d pbvcu3=%d pb532u4=%d pbvcu4=%d pb532u5=%d pbvcu5=%d pb532ed=%d pbvced=%d "
        "pb532edu2=%d pbvcedu2=%d pb170=%d pb544=%d pb450=%d pb170u2=%d pb53=%d "
        "pb448=%d pbv38=%d pblea=%d pba2=%d pba2ed=%d pbf590a=%d pbf590u2=%d "
        "pba2edu2=%d pbf590u3=%d pbf590u4=%d pba2edu3=%d pbf590u5=%d pbf590u6=%d "
        "pba2edu4=%d pbf590u7=%d pbf590u8=%d pba2edu5=%d pbf590u9=%d "
        "pbf590u10=%d pba2edu6=%d pbf590u11=%d pbf590u12=%d "
        "pba2edu7=%d pbf590u13=%d pbf590u14=%d pba2eu1=%d pba2e5=%d pba543=%d pb1fc=%d pbca_units=%d pbcb_units=%d pbcc_units=%d "
        "gate_byte=%u mid_reg=%d tsc=0x%llx entries=%u "
        "alloc_ops=%llu duration_us=%lld "
        "allocator=tip_AllocatorFront+FakeHeap "
        "success_atexit/499f80_body/56dcb0_body/5c64c0_body/1290_body/"
        "7f7220_body/f90310_plt_body/CP10_stack/26800_body/f590_body/52eb50_body/"
        "532ca0_body/stack_prep/vcall18_callee/544fe0_body/mov_rbx_prep/17070_body/"
        "vslot70_callee/53fd10_body/vcall38_callee/a2caf0_body/a2ed00_body/"
        "14c2f0_body/6a5370_body/6abcc0_body/"
        "21620_body/2779a0_body/42390_body/525080_body/2569d0_body/26ea0/26b60/272d0/"
        "255f60/f8f3a0_body/140d90_body/ba40_listeners/plt/ctor523bd0_body/"
        "dtor525b40_body/ctor493230_body/ensure_migrate/nonnull=NOT_modelled "
        "pr5_heap_merge=GAP\n",
        r.ctor.nested_48, eb, r.tail.obj_40, r.singleton.obj, r.vslot_40.obj,
        r.vslot_40_join.pos_obj, r.vslot_40_gfx.obj, r.vslot_40_shadow.loaded,
        r.vslot_40_shadowskels.loaded, r.vslot_40_shadowanims.loaded,
        r.vslot_40_shadowbanks.loaded, r.vslot_40_shadowstreams.loaded,
        r.vslot_40_shadowrandomevents.loaded, r.vslot_40_shadowbossmusic.loaded,
        r.vslot_40_cp3characters.loaded, r.vslot_40_cp3shadowprecache.loaded,
        r.vslot_40_post.lookup_hit ? 1 : 0, r.vslot_40_miss.find_null ? 1 : 0,
        r.vslot_40_gfx.marker_value, r.vslot_40_shadow.refcount_after,
        r.vslot_40_shadow_attach.ok ? 1 : 0,
        r.vslot_40_shadowskels.refcount_after,
        r.vslot_40_skels_attach.ok ? 1 : 0,
        r.vslot_40_shadowanims.refcount_after,
        r.vslot_40_anims_attach.ok ? 1 : 0,
        r.vslot_40_shadowbanks.refcount_after,
        r.vslot_40_banks_attach.ok ? 1 : 0,
        r.vslot_40_shadowstreams.refcount_after,
        r.vslot_40_streams_attach.ok ? 1 : 0,
        r.vslot_40_shadowrandomevents.refcount_after,
        r.vslot_40_randomevents_attach.ok ? 1 : 0,
        r.vslot_40_shadowbossmusic.refcount_after,
        r.vslot_40_bossmusic_attach.ok ? 1 : 0,
        r.vslot_40_cp3characters_attach.ok ? 1 : 0,
        r.vslot_40_cp3shadowprecache_attach.ok ? 1 : 0,
        r.vslot_40_tail_140d90.call_140d90_count,
        r.vslot_40_stackchk.returned ? 1 : 0,
        r.post_base_499f80.call_499f80_count,
        r.post_base_56dcb0.call_56dcb0_count,
        r.post_base_5c64c0.call_5c64c0_count,
        r.post_base_1290.call_1290_count,
        r.post_base_56db70.call_56db70_count,
        r.post_base_5b3d00.call_5b3d00_count,
        r.post_base_5b3cf0.call_5b3cf0_count,
        r.post_base_avx_bss.store_count,
        r.post_base_7f7220.call_7f7220_count,
        r.post_base_56fad0.call_56fad0_count,
        r.post_base_f90310.call_f90310_count,
        r.post_base_ymm_bss.store_count,
        r.post_base_ymm_intercell.store_count,
        r.post_base_xmm_seed.store_count,
        r.post_base_ymm_reload.store_count,
        r.post_base_26800_f590.call_26800_count,
        r.post_base_26800_f590.call_f590_count,
        r.post_base_sbj.call_26800_count,
        r.post_base_sbj.call_f590_count,
        r.post_base_sboj.call_26800_count,
        r.post_base_sboj.call_f590_count,
        r.post_base_52eb50.call_52eb50_count,
        r.post_base_lea_vmovq.load_count,
        r.post_base_532ca0.call_532ca0_count,
        r.post_base_vcall18.call_vslot18_count,
        r.post_base_532ca0_u2.call_532ca0_count,
        r.post_base_vcall18_u2.call_vslot18_count,
        r.post_base_532ca0_u3.call_532ca0_count,
        r.post_base_vcall18_u3.call_vslot18_count,
        r.post_base_532ca0_u4.call_532ca0_count,
        r.post_base_vcall18_u4.call_vslot18_count,
        r.post_base_532ca0_u5.call_532ca0_count,
        r.post_base_vcall18_u5.call_vslot18_count,
        r.post_base_532ed0.call_532ed0_count,
        r.post_base_vcall18_532ed0.call_vslot18_count,
        r.post_base_532ed0_u2.call_532ed0_count,
        r.post_base_vcall18_532ed0_u2.call_vslot18_count,
        r.post_base_17070.call_17070_count,
        r.post_base_544fe0.call_544fe0_count,
        r.post_base_cell_2eff450.installed ? 1 : 0,
        r.post_base_17070_u2.call_17070_count,
        r.post_base_53fd10.call_53fd10_count,
        r.post_base_cell_2eff448.installed ? 1 : 0,
        r.post_base_vcall38.call_vslot38_count,
        r.post_base_lea_32f4fe0.lea_count,
        r.post_base_a2caf0.call_a2caf0_count,
        r.post_base_a2ed00.call_a2ed00_count,
        r.post_base_f590_a2ed00.call_f590_count,
        r.post_base_f590_a2ed00_u2.call_f590_count,
        r.post_base_a2ed00_u2.call_a2ed00_count,
        r.post_base_f590_a2ed00_u3.call_f590_count,
        r.post_base_f590_a2ed00_u4.call_f590_count,
        r.post_base_a2ed00_u3.call_a2ed00_count,
        r.post_base_f590_a2ed00_u5.call_f590_count,
        r.post_base_f590_a2ed00_u6.call_f590_count,
        r.post_base_a2ed00_u4.call_a2ed00_count,
        r.post_base_f590_a2ed00_u7.call_f590_count,
        r.post_base_f590_a2ed00_u8.call_f590_count,
        r.post_base_a2ed00_u5.call_a2ed00_count,
        r.post_base_f590_a2ed00_u9.call_f590_count,
        r.post_base_f590_a2ed00_u10.call_f590_count,
        r.post_base_a2ed00_u6.call_a2ed00_count,
        r.post_base_f590_a2ed00_u11.call_f590_count,
        r.post_base_f590_a2ed00_u12.call_f590_count,
        r.post_base_a2ed00_u7.call_a2ed00_count,
        r.post_base_f590_a2ed00_u13.call_f590_count,
        r.post_base_f590_a2ed00_u14.call_f590_count,
        r.post_base_a2ea00_u1.call_a2ea00_count,
        r.post_base_a2e5d0_u1.call_a2e5d0_count,
        r.post_base_a543b0_u1.call_a543b0_count,
        r.post_base_1fc80_u1.call_1fc80_count,
        r.post_base_continue_a.unit_executed,
        r.post_base_continue_b.unit_executed,
        r.post_base_continue_c.unit_executed,
        static_cast<unsigned>(r.tail.gates.gate_byte), g_mid_reg,
        static_cast<unsigned long long>(r.ctor.tsc_value), r.remainder.registry.entry_count,
        static_cast<unsigned long long>(heap.alloc_count - allocs_before),
        static_cast<long long>(us));

    // Release the nine persistent slots: each wrapper must die once, the map must empty, and the
    // pool's allocation/free counts must balance.
    {
        void* const f08 = r.singleton.obj;
        LockedResourceRef* const slots[9] = {
            reinterpret_cast<LockedResourceRef*>(static_cast<std::uint8_t*>(f08) + 0x00),
            reinterpret_cast<LockedResourceRef*>(static_cast<std::uint8_t*>(f08) + 0x10),
            reinterpret_cast<LockedResourceRef*>(static_cast<std::uint8_t*>(f08) + 0x20),
            reinterpret_cast<LockedResourceRef*>(static_cast<std::uint8_t*>(f08) + 0x40),
            reinterpret_cast<LockedResourceRef*>(static_cast<std::uint8_t*>(f08) + 0x50),
            reinterpret_cast<LockedResourceRef*>(static_cast<std::uint8_t*>(f08) + 0x60),
            reinterpret_cast<LockedResourceRef*>(static_cast<std::uint8_t*>(f08) + 0x70),
            reinterpret_cast<LockedResourceRef*>(app.data() + kVslot40Cp3charactersAttachSlotOff),
            reinterpret_cast<LockedResourceRef*>(app.data() + kVslot40Cp3shadowprecacheAttachSlotOff),
        };
        for (LockedResourceRef* const slot : slots) {
            CHECK(slot->release(pool_hooks));
        }
        CHECK(pool.size() == std::size_t{0});
        CHECK(g_pool_alloc == 9);
        CHECK(g_pool_free == 9);
        CHECK(g_key_free_tag0 == 9);  // nine map-node keys through the pool deallocator
        CHECK(g_key_free_tag1 == 9);
    }

    destroy_registry_at_2efd160(front);
    destroy_singleton_f08_at_2efc9e0(front, sh);
    destroy_vslot40_at_2ef8c08(front, bh.vslot_40_detail);

    // ================================================================
    //  Alt 5327bd standalone tests (added after main slice run)
    // ================================================================

    // alt_5327bd success path via jmp
    {
        Init531d60PostBaseAlt5327bdHooks h{};
        alignas(8) std::uint8_t gate_obj[0x70]{};
        gate_obj[0x60] = 0;
        h.gate_2dfce70 = gate_obj;
        std::uint8_t obj_3305568_buf[0x10]{};
        h.obj_3305568 = obj_3305568_buf;
        h.gate_check_supplied = true;
        h.gate_check_result = 0;
        h.float_compare_supplied = true;
        h.float_below_equal = false;

        auto r = init_531d60_post_base_alt_5327bd(h);
        CHECK(r.ok);
        CHECK(r.unit_executed == 70);
        CHECK(r.call_direct_count == 4);
        CHECK(r.vcall_count == 5);
        CHECK(r.lea_rip_count == 7);
        CHECK(r.mov_rip_count == 2);
        CHECK(r.float_compare_count == 1);
        CHECK(r.branch_count == 5);
        CHECK(r.setcc_count == 1);
        CHECK(r.reached_via_jmp);
        CHECK(!r.reached_via_preamble);
        CHECK(r.r12b_out == 0);
        CHECK(r.r14b_out == 0);
        CHECK(r.r12b_known);
        CHECK(r.r14b_known);
        for (int i = 0; i < 70; ++i)
            CHECK(r.unit_counts[i] == 1);
    }

    // alt_5327bd error path via gate failure
    {
        Init531d60PostBaseAlt5327bdHooks h{};
        h.gate_check_supplied = true;
        h.gate_check_result = 1;
        alignas(8) std::uint8_t cell340_obj[0x70]{};
        cell340_obj[0x60] = 0;
        h.cell_2efe340 = cell340_obj;

        auto r = init_531d60_post_base_alt_5327bd(h);
        CHECK(r.ok);
        CHECK(r.unit_executed == 70);
        CHECK(!r.reached_via_jmp);
        CHECK(r.reached_via_preamble);
        CHECK(r.r12b_out == 1);
        CHECK(r.r14b_out == 1);
    }

    // alt_5327bd float compare fail
    {
        Init531d60PostBaseAlt5327bdHooks h{};
        h.gate_check_supplied = true;
        h.gate_check_result = 0;
        h.float_compare_supplied = true;
        h.float_below_equal = true;
        alignas(8) std::uint8_t cell340_obj[0x70]{};
        cell340_obj[0x60] = 0xFF;
        h.cell_2efe340 = cell340_obj;

        auto r = init_531d60_post_base_alt_5327bd(h);
        CHECK(r.ok);
        CHECK(r.reached_via_preamble);
        CHECK(r.r14b_out == 0);

        cell340_obj[0x60] = 0;
        auto r2 = init_531d60_post_base_alt_5327bd(h);
        CHECK(r2.ok);
        CHECK(r2.r14b_out == 1);
    }

    // alt_5327bd empty hooks pin all units but prove neither route.
    {
        auto r = init_531d60_post_base_alt_5327bd();
        CHECK(r.ok);
        CHECK(r.unit_executed == 70);
        CHECK(!r.reached_via_jmp);
        CHECK(!r.reached_via_preamble);
        CHECK(!r.r12b_known);
        CHECK(!r.r14b_known);
        for (int i = 0; i < 70; ++i)
            CHECK(r.unit_counts[i] == 1);
    }

    // alt_5327bd exit stub
    {
        Init531d60PostBaseAlt5327bdResult r{};
        init_531d60_post_base_exit_5327b6(r);
        CHECK(r.exit_stub_active);
    }

    run_alt_5327bd_tests();
    run_alt_join_route_tests();
    std::puts("appmain_init_ctor_98a90_slice: all checks passed");
    return 0;
}

// --- Alt 5327bd standalone tests ---

void alt_case_1() {
    Init531d60PostBaseAlt5327bdHooks h{};
    // Gate stand-in: byte at +0x60 == 0 means gate open.
    alignas(8) std::uint8_t gate_obj[0x70]{};
    gate_obj[0x60] = 0;
    h.gate_2dfce70 = gate_obj;

    // Object for vtable dispatch.
    std::uint8_t obj_3305568_buf[0x10]{};
    h.obj_3305568 = obj_3305568_buf;

    // Gate check: supplied, byte is 0 → gate open.
    h.gate_check_supplied = true;
    h.gate_check_result = 0;

    // Float compare: above constant → path continues.
    h.float_compare_supplied = true;
    h.float_below_equal = false;

    auto r = init_531d60_post_base_alt_5327bd(h);
    CHECK(r.ok);
    CHECK(r.unit_executed == 70);
    CHECK(r.call_direct_count == 4);
    CHECK(r.vcall_count == 5);
    CHECK(r.lea_rip_count == 7);
    CHECK(r.mov_rip_count == 2);
    CHECK(r.float_compare_count == 1);
    CHECK(r.branch_count == 5);
    CHECK(r.setcc_count == 1);
    CHECK(r.reached_via_jmp);
    CHECK(!r.reached_via_preamble);
    CHECK(r.r12b_out == 0);
    CHECK(r.r14b_out == 0);
    CHECK(r.r12b_known);
    CHECK(r.r14b_known);
    for (int i = 0; i < 70; ++i)
        CHECK(r.unit_counts[i] == 1);
}

void alt_case_2() {
    Init531d60PostBaseAlt5327bdHooks h{};
    h.gate_check_supplied = true;
    h.gate_check_result = 1;  // gate closed → jne taken → error path

    // Preamble: cell_2efe340 with byte +0x60 = 0 → sete r14b = 1
    alignas(8) std::uint8_t cell340_obj[0x70]{};
    cell340_obj[0x60] = 0;
    h.cell_2efe340 = cell340_obj;

    auto r = init_531d60_post_base_alt_5327bd(h);
    CHECK(r.ok);
    CHECK(r.unit_executed == 70);
    CHECK(!r.reached_via_jmp);
    CHECK(r.reached_via_preamble);
    CHECK(r.r12b_out == 1);
    CHECK(r.r14b_out == 1);  // flag was 0 → sete = 1
}

void alt_case_3() {
    Init531d60PostBaseAlt5327bdHooks h{};
    h.gate_check_supplied = true;
    h.gate_check_result = 0;     // gate open
    h.float_compare_supplied = true;
    h.float_below_equal = true;  // jbe taken → error path

    // Preamble: flag byte != 0 → sete = 0
    alignas(8) std::uint8_t cell340_obj[0x70]{};
    cell340_obj[0x60] = 0xFF;
    h.cell_2efe340 = cell340_obj;

    auto r = init_531d60_post_base_alt_5327bd(h);
    CHECK(r.ok);
    CHECK(r.reached_via_preamble);
    CHECK(r.r14b_out == 0);  // flag non-zero → sete = 0

    // Also test: flag == 0
    cell340_obj[0x60] = 0;
    auto r2 = init_531d60_post_base_alt_5327bd(h);
    CHECK(r2.ok);
    CHECK(r2.r14b_out == 1);
}

void alt_case_4() {
    // With no hooks, all static units pin but neither join route is known.
    auto r = init_531d60_post_base_alt_5327bd();
    CHECK(r.ok);
    CHECK(r.unit_executed == 70);
    CHECK(!r.reached_via_jmp);
    CHECK(!r.reached_via_preamble);
    CHECK(!r.r12b_known);
    CHECK(!r.r14b_known);
    for (int i = 0; i < 70; ++i)
        CHECK(r.unit_counts[i] == 1);
}

void alt_case_5() {
    Init531d60PostBaseAlt5327bdResult r{};
    init_531d60_post_base_exit_5327b6(r);
    CHECK(r.exit_stub_active);
}

void run_alt_5327bd_tests() {
    alt_case_1();
    alt_case_2();
    alt_case_3();
    alt_case_4();
    alt_case_5();
}

void run_alt_join_route_tests() {
    // Unknown continue-B eax must not be replaced by the E hook's default byte.
    {
        AllocatorFront route_front;
        FakeHeap route_heap(route_front.owners(), 2);
        CHECK(route_front.set_heap(1, &route_heap));
        CHECK(route_front.set_heap(2, &route_heap));
        InputTable551140 route_table{};
        init_input_table_551140(route_table);
        std::array<std::uint8_t, 0x60> route_setter{};
        Init531d60PostBaseContinueEHooks e{};
        e.incoming_r12b = 0xBB;
        e.boss_flag_2dcdf14 = &g_boss_flag_cell;
        g_boss_flag_cell = 0xCC;
        AppmainInitCtor98a90SliceResult out{};
        run_appmain_init_continue_driver_tail(
            out, route_table, route_front, route_setter.data(), 0, 0, 0, 0,
            {}, {}, {}, {}, e, {}, {}, {}, {}, {}, {}, {}, {});
        CHECK(out.post_base_continue_b.ok);
        CHECK(!out.post_base_continue_b.eax_known);
        CHECK(out.post_base_continue_c.ok);
        CHECK(out.post_base_continue_d.ok);
        CHECK(!out.init_r12b_known);
        CHECK(out.init_stopped_at_5329ae);
        CHECK(!out.post_base_continue_e.ok);
        CHECK(!out.remainder.trivial_ok);
        CHECK(g_boss_flag_cell == 0xCC);
    }

    // A known-open gate with no float-compare result proves neither join.
    {
        AllocatorFront route_front;
        FakeHeap route_heap(route_front.owners(), 2);
        CHECK(route_front.set_heap(1, &route_heap));
        CHECK(route_front.set_heap(2, &route_heap));
        InputTable551140 route_table{};
        init_input_table_551140(route_table);
        std::array<std::uint8_t, 0x60> route_setter{};
        Init531d60PostBaseAlt5327bdHooks alt{};
        alt.gate_check_supplied = true;
        alt.gate_check_result = 0;
        alt.call_ac5260 = hook_alt_ac5260;
        alt.call_3e7e0 = hook_alt_3e7e0;
        alt.call_4c4950 = hook_alt_4c4950;
        Init531d60PostBaseContinueEHooks e{};
        e.incoming_r12b = 0xBB;
        e.boss_flag_2dcdf14 = &g_boss_flag_cell;
        g_boss_flag_cell = 0xCC;
        AppmainInitCtor98a90SliceResult out{};
        run_appmain_init_continue_driver_tail(
            out, route_table, route_front, route_setter.data(), 0, 0, 0, 0,
            {}, {}, {}, {}, e, {}, alt, {}, {}, {}, {}, {}, {});
        CHECK(out.post_base_alt_5327bd.ok);
        CHECK(!out.post_base_alt_5327bd.reached_via_jmp);
        CHECK(!out.post_base_alt_5327bd.reached_via_preamble);
        CHECK(!out.post_base_alt_5327bd.r12b_known);
        CHECK(!out.post_base_alt_5327bd.r14b_known);
        CHECK(out.init_stopped_at_532904);
        CHECK(!out.post_base_continue_c.ok);
        CHECK(!out.post_base_continue_e.ok);
        CHECK(!out.remainder.trivial_ok);
        CHECK(g_boss_flag_cell == 0xCC);
        CHECK(g_alt_ac5260_calls == 1); // gate-open work precedes the unknown float branch
        CHECK(g_alt_3e7e0_calls == 0);
        CHECK(g_alt_4c4950_zero_calls == 0);
        CHECK(g_alt_4c4950_one_calls == 0);
    }

    // A failing float comparison proves the error route even when the earlier
    // gate result is unknown; do not dispatch gate-conditional work speculatively.
    {
        Init531d60PostBaseAlt5327bdHooks alt{};
        alt.float_compare_supplied = true;
        alt.float_below_equal = true;
        alt.call_ac5260 = hook_alt_ac5260;
        alt.call_3e7e0 = hook_alt_3e7e0;
        alt.call_4c4950 = hook_alt_4c4950;
        g_alt_ac5260_calls = 0;
        g_alt_3e7e0_calls = 0;
        g_alt_4c4950_zero_calls = 0;
        g_alt_4c4950_one_calls = 0;
        const auto out = init_531d60_post_base_alt_5327bd(alt);
        CHECK(out.ok);
        CHECK(out.reached_via_preamble);
        CHECK(out.r12b_known && out.r12b_out == 1);
        CHECK(!out.r14b_known);
        CHECK(g_alt_ac5260_calls == 0);
        CHECK(g_alt_3e7e0_calls == 0);
        CHECK(g_alt_4c4950_zero_calls == 0);
        CHECK(g_alt_4c4950_one_calls == 1);
    }

    // Success alt route jumps directly to 0x532904 and then follows C→D→E.
    {
        AllocatorFront route_front;
        FakeHeap route_heap(route_front.owners(), 2);
        CHECK(route_front.set_heap(1, &route_heap));
        CHECK(route_front.set_heap(2, &route_heap));
        InputTable551140 route_table{};
        init_input_table_551140(route_table);
        std::array<std::uint8_t, 0x60> route_setter{};
        std::array<std::uint8_t, 8> route_parent{};

        Init531d60PostBaseAlt5327bdHooks alt{};
        alt.gate_check_supplied = true;
        alt.gate_check_result = 0;
        alt.float_compare_supplied = true;
        alt.float_below_equal = false;
        alt.incoming_r13 = route_parent.data();
        Init531d60PostBaseContinueEHooks e{};
        e.incoming_r13 = nullptr;  // the alt route's r13 must reach E
        e.incoming_r12b = 0xBB;    // route's r12b must replace this stand-in
        e.boss_flag_2dcdf14 = &g_boss_flag_cell;
        e.call_a54920 = hook_cont_e_a54920;
        e.call_54b250 = hook_cont_e_54b250;
        e.cell_2eff450 = &g_cell_2eff450_value;

        g_cont_e_a54920 = 0;
        g_cont_e_54b250 = 0;
        g_boss_flag_cell = 0xAA;
        AppmainInitCtor98a90SliceResult out{};
        run_appmain_init_continue_driver_tail(
            out, route_table, route_front, route_setter.data(), 0, 0x11, 0x22, 0x33,
            {}, {}, {}, {}, e, {}, alt, {}, {}, {}, {}, {}, {});
        CHECK(out.post_base_alt_5327bd.ok);
        CHECK(out.post_base_alt_5327bd.reached_via_jmp);
        CHECK(out.post_base_alt_5327bd.r12b_known && out.post_base_alt_5327bd.r12b_out == 0);
        CHECK(out.post_base_continue_c.ok);
        CHECK(!out.post_base_continue_c.preamble_ran);
        CHECK(out.post_base_continue_d.ok);
        CHECK(out.post_base_continue_e.ok);
        CHECK(out.init_r12b_known && out.init_r12b_out == 0);
        CHECK(g_boss_flag_cell == 0);
        CHECK(g_cont_e_a54920 == 3);
        CHECK(g_cont_e_a54920_parent[0] == route_parent.data());
        CHECK(out.remainder.trivial_ok);
    }

    // Error alt route executes its preamble once, uses native 0x91210 binding,
    // then enters C at 0x532904 with the resulting r14b/r12b values.
    {
        AllocatorFront route_front;
        FakeHeap route_heap(route_front.owners(), 2);
        CHECK(route_front.set_heap(1, &route_heap));
        CHECK(route_front.set_heap(2, &route_heap));
        InputTable551140 route_table{};
        init_input_table_551140(route_table);
        std::array<std::uint8_t, 0x60> route_setter{};
        std::array<std::uint8_t, 8> route_parent{};
        std::array<std::uint8_t, 0x30> demo_object{};
        std::array<std::uint8_t, 0x68> bound_object{};
        std::uint8_t native_flag = 0;
        void* flag_cell = &native_flag;
        std::memcpy(bound_object.data() + 0x60, &flag_cell, sizeof(flag_cell));
        void* bound_ptr = bound_object.data();
        std::memcpy(demo_object.data() + kLazyBind91210BoundOffset, &bound_ptr,
                    sizeof(bound_ptr));

        Init531d60PostBaseAlt5327bdHooks alt{};
        alt.gate_check_supplied = true;
        alt.gate_check_result = 1;  // error path, so the alt preamble is executed
        alt.incoming_r13 = route_parent.data();
        Init531d60PostBaseContinueCHooks c{};
        c.use_native_lazy_bind_91210 = true;
        c.demo_object = demo_object.data();
        Init531d60PostBaseContinueEHooks e{};
        e.incoming_r13 = route_parent.data();
        e.incoming_r12b = 0xBB;
        e.boss_flag_2dcdf14 = &g_boss_flag_cell;
        e.call_a54920 = hook_cont_e_a54920;
        e.call_54b250 = hook_cont_e_54b250;
        e.cell_2eff450 = &g_cell_2eff450_value;

        g_cont_e_a54920 = 0;
        g_cont_e_54b250 = 0;
        g_boss_flag_cell = 0xAA;
        AppmainInitCtor98a90SliceResult out{};
        run_appmain_init_continue_driver_tail(
            out, route_table, route_front, route_setter.data(), 0, 0x11, 0x22, 0x33,
            {}, {}, c, {}, e, {}, alt, {}, {}, {}, {}, {}, {});
        CHECK(out.post_base_alt_5327bd.ok);
        CHECK(out.post_base_alt_5327bd.reached_via_preamble);
        CHECK(out.post_base_alt_5327bd.slot0_dispatch_count == 1);
        CHECK(out.post_base_alt_5327bd.lazy_bind_calls == 1);
        CHECK(out.post_base_alt_5327bd.lazy_bind_ok);
        CHECK(out.post_base_alt_5327bd.r14b_known && out.post_base_alt_5327bd.r14b_out == 1);
        CHECK(out.post_base_alt_5327bd.r12b_known && out.post_base_alt_5327bd.r12b_out == 1);
        CHECK(out.post_base_continue_c.ok);
        CHECK(!out.post_base_continue_c.preamble_ran);  // do not replay 0x5328e0
        CHECK(out.post_base_continue_c.first_esi == 1); // alt r14b reaches C
        CHECK(out.post_base_continue_c.lazy_bind_calls == 3); // a599e0+a431e0 u1 + C slot-0
        CHECK(out.init_branch_5329ae_taken);
        CHECK(out.post_base_continue_e.ok);
        CHECK(out.init_r12b_known && out.init_r12b_out == 1);
        CHECK(g_boss_flag_cell == 1);
        CHECK(g_cont_e_a54920 == 3);
        CHECK(g_cont_e_a54920_parent[0] == route_parent.data());
        CHECK(out.remainder.trivial_ok);
    }
}
