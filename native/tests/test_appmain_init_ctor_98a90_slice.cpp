// Measured prologue early+tail with lifted 0x3be050 + lifted 0x1ab550 → registry.
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/allocator_front.hpp"
#include "core/appmain_init_ctor_98a90_slice.hpp"
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
#include "core/prologue_255180.hpp"
#include "core/prologue_gates_2552d4.hpp"
#include "core/registry_init_531d60.hpp"
#include "core/store_tsc_323d0.hpp"
#include "core/vslot_30_534130.hpp"

#define CHECK(c)                                                                         \
    do {                                                                                 \
        if (!(c)) {                                                                      \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);              \
            std::exit(1);                                                                \
        }                                                                                \
    } while (0)

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
int g_v40_miss_f590 = 0;
int g_v40_42390 = 0;
int g_v40_2779a0 = 0;
int g_v40_join_f590 = 0;
int g_v40_6a5370 = 0;
int g_v40_6abcc0 = 0;
int g_v40_14c2f0 = 0;
int g_v40_shadow_load = 0;
int g_v40_14ccc0 = 0;
int g_v40_attach_f590 = 0;
int g_v40_skels_notify = 0;
int g_v40_skels_14c2f0 = 0;
int g_v40_skels_load = 0;
int g_v40_skels_14ccc0 = 0;
int g_v40_skels_attach_f590 = 0;
int g_v40_anims_notify = 0;
int g_v40_anims_14c2f0 = 0;
int g_v40_anims_load = 0;
int g_v40_anims_14ccc0 = 0;
int g_v40_anims_attach_f590 = 0;
int g_v40_banks_notify = 0;
int g_v40_banks_14c2f0 = 0;
int g_v40_banks_load = 0;
int g_v40_banks_14ccc0 = 0;
int g_v40_banks_attach_f590 = 0;
int g_v40_streams_notify = 0;
int g_v40_streams_14c2f0 = 0;
int g_v40_streams_load = 0;
int g_v40_streams_14ccc0 = 0;
int g_v40_streams_attach_f590 = 0;
int g_v40_rand_notify = 0;
int g_v40_rand_14c2f0 = 0;
int g_v40_rand_load = 0;
int g_v40_rand_14ccc0 = 0;
int g_v40_rand_attach_f590 = 0;
int g_v40_boss_notify = 0;
int g_v40_boss_14c2f0 = 0;
int g_v40_boss_load = 0;
int g_v40_boss_14ccc0 = 0;
int g_v40_boss_attach_f590 = 0;
int g_v40_cp3_notify = 0;
int g_v40_cp3_14c2f0 = 0;
int g_v40_cp3_load = 0;
int g_v40_cp3_14ccc0 = 0;
int g_v40_cp3_attach_f590 = 0;
int g_v40_csp_notify = 0;
int g_v40_csp_14c2f0 = 0;
int g_v40_csp_load = 0;
int g_v40_csp_14ccc0 = 0;
int g_v40_csp_attach_f590 = 0;
int g_v40_tail_140d90 = 0;
int g_v40_tail_140d90_f590 = 0;
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
void hook_v40_miss_f590(void*) { ++g_v40_miss_f590; }
void hook_v40_42390(void*) { ++g_v40_42390; }
void hook_v40_2779a0(void* obj, void*, int) {
    ++g_v40_2779a0;
    *static_cast<std::uint32_t*>(obj) = 0x2779A001u;
}
void hook_v40_join_f590(void*) { ++g_v40_join_f590; }
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
void hook_v40_14ccc0(void*) { ++g_v40_14ccc0; }
void hook_v40_attach_f590(void*) { ++g_v40_attach_f590; }
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
void hook_v40_skels_14ccc0(void*) { ++g_v40_skels_14ccc0; }
void hook_v40_skels_attach_f590(void*) { ++g_v40_skels_attach_f590; }
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
void hook_v40_anims_14ccc0(void*) { ++g_v40_anims_14ccc0; }
void hook_v40_anims_attach_f590(void*) { ++g_v40_anims_attach_f590; }
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
void hook_v40_banks_14ccc0(void*) { ++g_v40_banks_14ccc0; }
void hook_v40_banks_attach_f590(void*) { ++g_v40_banks_attach_f590; }
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
void hook_v40_streams_14ccc0(void*) { ++g_v40_streams_14ccc0; }
void hook_v40_streams_attach_f590(void*) { ++g_v40_streams_attach_f590; }
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
void hook_v40_rand_14ccc0(void*) { ++g_v40_rand_14ccc0; }
void hook_v40_rand_attach_f590(void*) { ++g_v40_rand_attach_f590; }
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
void hook_v40_boss_14ccc0(void*) { ++g_v40_boss_14ccc0; }
void hook_v40_boss_attach_f590(void*) { ++g_v40_boss_attach_f590; }
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
void hook_v40_cp3_14ccc0(void*) { ++g_v40_cp3_14ccc0; }
void hook_v40_cp3_attach_f590(void*) { ++g_v40_cp3_attach_f590; }
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
void hook_v40_csp_14ccc0(void*) { ++g_v40_csp_14ccc0; }
void hook_v40_csp_attach_f590(void*) { ++g_v40_csp_attach_f590; }
void hook_v40_tail_140d90(void* slot_obj, int arg) {
    ++g_v40_tail_140d90;
    g_v40_tail_140d90_slot = slot_obj;
    g_v40_tail_140d90_arg = arg;
}
void hook_v40_tail_140d90_f590(void*) { ++g_v40_tail_140d90_f590; }
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
void hook_post_base_f590() { ++g_post_base_f590; }
void hook_post_base_sbj_26800() { ++g_post_base_sbj_26800; }
void hook_post_base_sbj_f590() { ++g_post_base_sbj_f590; }
void hook_post_base_sboj_26800() { ++g_post_base_sboj_26800; }
void hook_post_base_sboj_f590() { ++g_post_base_sboj_f590; }
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
    g_v40_21620 = g_v40_ba40 = g_v40_miss_f590 = 0;
    g_v40_42390 = g_v40_2779a0 = g_v40_join_f590 = 0;
    g_v40_6a5370 = g_v40_6abcc0 = 0;
    g_v40_14c2f0 = g_v40_shadow_load = 0;
    g_v40_14ccc0 = g_v40_attach_f590 = 0;
    g_v40_skels_notify = g_v40_skels_14c2f0 = g_v40_skels_load = 0;
    g_v40_skels_14ccc0 = g_v40_skels_attach_f590 = 0;
    g_v40_anims_notify = g_v40_anims_14c2f0 = g_v40_anims_load = 0;
    g_v40_anims_14ccc0 = g_v40_anims_attach_f590 = 0;
    g_v40_banks_notify = g_v40_banks_14c2f0 = g_v40_banks_load = 0;
    g_v40_banks_14ccc0 = g_v40_banks_attach_f590 = 0;
    g_v40_streams_notify = g_v40_streams_14c2f0 = g_v40_streams_load = 0;
    g_v40_streams_14ccc0 = g_v40_streams_attach_f590 = 0;
    g_v40_rand_notify = g_v40_rand_14c2f0 = g_v40_rand_load = 0;
    g_v40_rand_14ccc0 = g_v40_rand_attach_f590 = 0;
    g_v40_boss_notify = g_v40_boss_14c2f0 = g_v40_boss_load = 0;
    g_v40_boss_14ccc0 = g_v40_boss_attach_f590 = 0;
    g_v40_cp3_notify = g_v40_cp3_14c2f0 = g_v40_cp3_load = 0;
    g_v40_cp3_14ccc0 = g_v40_cp3_attach_f590 = 0;
    g_v40_csp_notify = g_v40_csp_14c2f0 = g_v40_csp_load = 0;
    g_v40_csp_14ccc0 = g_v40_csp_attach_f590 = 0;
    g_v40_tail_140d90 = g_v40_tail_140d90_f590 = 0;
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

    BaseInitialize255130Hooks bh{};
    // vslot_30 / mid / vslot_40 null → slice forces lifted 0x534130 + 0x255340 + header
    bh.mid_255340_detail.register_26ea0 = hook_mid_26ea0;
    bh.mid_255340_detail.register_26b60 = hook_mid_26b60;
    bh.vslot_40_detail.ctor_493230 = hook_v40_ctor;
    bh.vslot_40_post_detail.call_525080 = hook_v40_525080;
    bh.vslot_40_post_detail.call_2569d0 = hook_v40_2569d0;
    bh.vslot_40_miss_detail.find_21620 = hook_v40_21620;
    bh.vslot_40_miss_detail.ba40 = hook_v40_ba40;
    bh.vslot_40_miss_detail.dtor_f590 = hook_v40_miss_f590;
    bh.vslot_40_join_detail.call_42390 = hook_v40_42390;
    bh.vslot_40_join_detail.ctor_2779a0 = hook_v40_2779a0;
    bh.vslot_40_join_detail.dtor_f590 = hook_v40_join_f590;
    bh.vslot_40_gfx_detail.call_6a5370 = hook_v40_6a5370;
    bh.vslot_40_gfx_detail.call_6abcc0 = hook_v40_6abcc0;
    bh.vslot_40_shadow_detail.path_14c2f0 = hook_v40_14c2f0;
    bh.vslot_40_shadow_detail.load_vslot18 = hook_v40_shadow_load;
    bh.vslot_40_shadow_attach_detail.release_14ccc0 = hook_v40_14ccc0;
    bh.vslot_40_shadow_attach_detail.dtor_f590 = hook_v40_attach_f590;
    bh.vslot_40_shadowskels_detail.notify_95580 = hook_v40_skels_notify;
    bh.vslot_40_shadowskels_detail.path_14c2f0 = hook_v40_skels_14c2f0;
    bh.vslot_40_shadowskels_detail.load_vslot18 = hook_v40_skels_load;
    bh.vslot_40_skels_attach_detail.release_14ccc0 = hook_v40_skels_14ccc0;
    bh.vslot_40_skels_attach_detail.dtor_f590 = hook_v40_skels_attach_f590;
    bh.vslot_40_shadowanims_detail.notify_95580 = hook_v40_anims_notify;
    bh.vslot_40_shadowanims_detail.path_14c2f0 = hook_v40_anims_14c2f0;
    bh.vslot_40_shadowanims_detail.load_vslot18 = hook_v40_anims_load;
    bh.vslot_40_anims_attach_detail.release_14ccc0 = hook_v40_anims_14ccc0;
    bh.vslot_40_anims_attach_detail.dtor_f590 = hook_v40_anims_attach_f590;
    bh.vslot_40_shadowbanks_detail.notify_95580 = hook_v40_banks_notify;
    bh.vslot_40_shadowbanks_detail.path_14c2f0 = hook_v40_banks_14c2f0;
    bh.vslot_40_shadowbanks_detail.load_vslot18 = hook_v40_banks_load;
    bh.vslot_40_banks_attach_detail.release_14ccc0 = hook_v40_banks_14ccc0;
    bh.vslot_40_banks_attach_detail.dtor_f590 = hook_v40_banks_attach_f590;
    bh.vslot_40_shadowstreams_detail.notify_95580 = hook_v40_streams_notify;
    bh.vslot_40_shadowstreams_detail.path_14c2f0 = hook_v40_streams_14c2f0;
    bh.vslot_40_shadowstreams_detail.load_vslot18 = hook_v40_streams_load;
    bh.vslot_40_streams_attach_detail.release_14ccc0 = hook_v40_streams_14ccc0;
    bh.vslot_40_streams_attach_detail.dtor_f590 = hook_v40_streams_attach_f590;
    bh.vslot_40_shadowrandomevents_detail.notify_95580 = hook_v40_rand_notify;
    bh.vslot_40_shadowrandomevents_detail.path_14c2f0 = hook_v40_rand_14c2f0;
    bh.vslot_40_shadowrandomevents_detail.load_vslot18 = hook_v40_rand_load;
    bh.vslot_40_randomevents_attach_detail.release_14ccc0 = hook_v40_rand_14ccc0;
    bh.vslot_40_randomevents_attach_detail.dtor_f590 = hook_v40_rand_attach_f590;
    bh.vslot_40_shadowbossmusic_detail.notify_95580 = hook_v40_boss_notify;
    bh.vslot_40_shadowbossmusic_detail.path_14c2f0 = hook_v40_boss_14c2f0;
    bh.vslot_40_shadowbossmusic_detail.load_vslot18 = hook_v40_boss_load;
    bh.vslot_40_bossmusic_attach_detail.release_14ccc0 = hook_v40_boss_14ccc0;
    bh.vslot_40_bossmusic_attach_detail.dtor_f590 = hook_v40_boss_attach_f590;
    bh.vslot_40_cp3characters_detail.notify_95580 = hook_v40_cp3_notify;
    bh.vslot_40_cp3characters_detail.path_14c2f0 = hook_v40_cp3_14c2f0;
    bh.vslot_40_cp3characters_detail.load_vslot18 = hook_v40_cp3_load;
    bh.vslot_40_cp3characters_attach_detail.release_14ccc0 = hook_v40_cp3_14ccc0;
    bh.vslot_40_cp3characters_attach_detail.dtor_f590 = hook_v40_cp3_attach_f590;
    bh.vslot_40_cp3shadowprecache_detail.notify_95580 = hook_v40_csp_notify;
    bh.vslot_40_cp3shadowprecache_detail.path_14c2f0 = hook_v40_csp_14c2f0;
    bh.vslot_40_cp3shadowprecache_detail.load_vslot18 = hook_v40_csp_load;
    bh.vslot_40_cp3shadowprecache_attach_detail.release_14ccc0 = hook_v40_csp_14ccc0;
    bh.vslot_40_cp3shadowprecache_attach_detail.dtor_f590 = hook_v40_csp_attach_f590;
    bh.vslot_40_tail_140d90_detail.call_140d90 = hook_v40_tail_140d90;
    bh.vslot_40_tail_140d90_detail.dtor_f590 = hook_v40_tail_140d90_f590;

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
                                                     pb544);
    const auto t1 = std::chrono::steady_clock::now();
    const auto us =
        std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

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
    CHECK(g_v40_miss_f590 == 2);
    // E424: join+ Position install at self+0x3b60
    CHECK(r.vslot_40_join.ok);
    CHECK(r.vslot_40_join.replaced);
    CHECK(g_v40_42390 == 1);
    CHECK(g_v40_2779a0 == 1);
    CHECK(g_v40_join_f590 == 1);
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
    CHECK(r.vslot_40_shadow_attach.took_fail_path);
    CHECK(r.vslot_40_shadow_attach.installed);
    CHECK(r.vslot_40_shadow_attach.slot_after == g_shadow_asset);
    CHECK(loadp(r.singleton.obj, 0) == g_shadow_asset);
    CHECK(g_v40_attach_f590 == 1);
    CHECK(g_v40_14ccc0 == 1);  // release loaded after install
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
    CHECK(r.vslot_40_skels_attach.took_fail_path);
    CHECK(r.vslot_40_skels_attach.installed);
    CHECK(r.vslot_40_skels_attach.slot_after == g_skels_asset);
    CHECK(loadp(r.singleton.obj, 0x10) == g_skels_asset);
    CHECK(g_v40_skels_attach_f590 == 1);
    CHECK(g_v40_skels_14ccc0 == 1);
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
    CHECK(r.vslot_40_anims_attach.took_fail_path);
    CHECK(r.vslot_40_anims_attach.installed);
    CHECK(r.vslot_40_anims_attach.slot_after == g_anims_asset);
    CHECK(loadp(r.singleton.obj, 0x20) == g_anims_asset);
    CHECK(g_v40_anims_attach_f590 == 1);
    CHECK(g_v40_anims_14ccc0 == 1);
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
    CHECK(r.vslot_40_banks_attach.took_fail_path);
    CHECK(r.vslot_40_banks_attach.installed);
    CHECK(r.vslot_40_banks_attach.slot_after == g_banks_asset);
    CHECK(loadp(r.singleton.obj, 0x40) == g_banks_asset);
    CHECK(g_v40_banks_attach_f590 == 1);
    CHECK(g_v40_banks_14ccc0 == 1);
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
    CHECK(r.vslot_40_streams_attach.took_fail_path);
    CHECK(r.vslot_40_streams_attach.installed);
    CHECK(r.vslot_40_streams_attach.slot_after == g_streams_asset);
    CHECK(loadp(r.singleton.obj, 0x50) == g_streams_asset);
    CHECK(g_v40_streams_attach_f590 == 1);
    CHECK(g_v40_streams_14ccc0 == 1);
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
    CHECK(r.vslot_40_randomevents_attach.took_fail_path);
    CHECK(r.vslot_40_randomevents_attach.installed);
    CHECK(r.vslot_40_randomevents_attach.slot_after == g_rand_asset);
    CHECK(loadp(r.singleton.obj, 0x60) == g_rand_asset);
    CHECK(g_v40_rand_attach_f590 == 1);
    CHECK(g_v40_rand_14ccc0 == 1);
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
    CHECK(r.vslot_40_bossmusic_attach.took_fail_path);
    CHECK(r.vslot_40_bossmusic_attach.installed);
    CHECK(r.vslot_40_bossmusic_attach.slot_after == g_boss_asset);
    CHECK(loadp(r.singleton.obj, 0x70) == g_boss_asset);
    CHECK(g_v40_boss_attach_f590 == 1);
    CHECK(g_v40_boss_14ccc0 == 1);
    // E440: cp3Characters load-attempt (notify f08+0x70 + path + vcall; no 14ccb0)
    CHECK(r.vslot_40_cp3characters.ok);
    CHECK(g_v40_cp3_notify == 1);
    CHECK(g_v40_cp3_14c2f0 == 1);
    CHECK(g_v40_cp3_load == 1);
    CHECK(r.vslot_40_cp3characters.loaded == g_cp3_asset);
    CHECK(r.vslot_40_cp3characters.f08 == r.singleton.obj);
    // E441: cp3Characters attach fail-path + install into self+0x3cb8
    CHECK(r.vslot_40_cp3characters_attach.ok);
    CHECK(r.vslot_40_cp3characters_attach.took_fail_path);
    CHECK(r.vslot_40_cp3characters_attach.installed);
    CHECK(r.vslot_40_cp3characters_attach.slot_after == g_cp3_asset);
    CHECK(loadp(app.data(), kVslot40Cp3charactersAttachSlotOff) == g_cp3_asset);
    CHECK(g_v40_cp3_attach_f590 == 1);
    CHECK(g_v40_cp3_14ccc0 == 1);
    // E442: cp3shadowprecache load-attempt (notify self+0x3cb8 + path + vcall; no 14ccb0)
    CHECK(r.vslot_40_cp3shadowprecache.ok);
    CHECK(g_v40_csp_notify == 1);
    CHECK(g_v40_csp_14c2f0 == 1);
    CHECK(g_v40_csp_load == 1);
    CHECK(r.vslot_40_cp3shadowprecache.loaded == g_csp_asset);
    CHECK(r.vslot_40_cp3shadowprecache.self == app.data());
    // E443: cp3shadowprecache attach fail-path + install into self+0x3cc8
    CHECK(r.vslot_40_cp3shadowprecache_attach.ok);
    CHECK(r.vslot_40_cp3shadowprecache_attach.took_fail_path);
    CHECK(r.vslot_40_cp3shadowprecache_attach.installed);
    CHECK(r.vslot_40_cp3shadowprecache_attach.slot_after == g_csp_asset);
    CHECK(loadp(app.data(), kVslot40Cp3shadowprecacheAttachSlotOff) == g_csp_asset);
    CHECK(g_v40_csp_attach_f590 == 1);
    CHECK(g_v40_csp_14ccc0 == 1);
    // E444: 0x140d90 call-site (self+0x3cc8, arg=9) + f590; stop before stackchk
    CHECK(r.vslot_40_tail_140d90.ok);
    CHECK(g_v40_tail_140d90 == 1);
    CHECK(g_v40_tail_140d90_f590 == 1);
    CHECK(g_v40_tail_140d90_arg == kVslot40Tail140d90Arg);
    CHECK(g_v40_tail_140d90_slot ==
          static_cast<std::uint8_t*>(app.data()) + kVslot40Tail140d90SlotOff);
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
    // E449: Init continue 0x1290 call-site; stop before 0x56db70
    CHECK(r.post_base_1290.ok);
    CHECK(g_post_base_1290 == 1);
    CHECK(r.post_base_1290.call_1290_count == 1);
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
    // E461: Init continue 0x26800/0xf590 call-sites (bodies NOT modelled)
    CHECK(r.post_base_26800_f590.ok);
    CHECK(g_post_base_26800 == 1);
    CHECK(g_post_base_f590 == 1);
    CHECK(r.post_base_26800_f590.call_26800_count == 1);
    CHECK(r.post_base_26800_f590.call_f590_count == 1);
    // E462: Init continue Shadow Boss Jump call-sites (bodies NOT modelled)
    CHECK(r.post_base_sbj.ok);
    CHECK(g_post_base_sbj_26800 == 1);
    CHECK(g_post_base_sbj_f590 == 1);
    CHECK(r.post_base_sbj.call_26800_count == 1);
    CHECK(r.post_base_sbj.call_f590_count == 1);
    // E463: Init continue Outro Jump call-sites (bodies / 0x52eb50 NOT modelled)
    CHECK(r.post_base_sboj.ok);
    CHECK(g_post_base_sboj_26800 == 1);
    CHECK(g_post_base_sboj_f590 == 1);
    CHECK(r.post_base_sboj.call_26800_count == 1);
    CHECK(r.post_base_sboj.call_f590_count == 1);
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
        "post_base_17070+post_base_544fe0+"
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
        "pb532edu2=%d pbvcedu2=%d pb170=%d pb544=%d "
        "gate_byte=%u mid_reg=%d tsc=0x%llx entries=%u "
        "alloc_ops=%llu duration_us=%lld "
        "allocator=tip_AllocatorFront+FakeHeap "
        "vslot533100_hit/success_atexit/499f80_body/56dcb0_body/5c64c0_body/1290_body/"
        "7f7220_body/f90310_plt_body/CP10_stack/26800_body/f590_body/52eb50_body/"
        "532ca0_body/stack_prep/vcall18_callee/544fe0_body/mov_rbx_prep/17070_body/"
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
        r.vslot_40_shadow_attach.took_fail_path ? 1 : 0,
        r.vslot_40_shadowskels.refcount_after,
        r.vslot_40_skels_attach.took_fail_path ? 1 : 0,
        r.vslot_40_shadowanims.refcount_after,
        r.vslot_40_anims_attach.took_fail_path ? 1 : 0,
        r.vslot_40_shadowbanks.refcount_after,
        r.vslot_40_banks_attach.took_fail_path ? 1 : 0,
        r.vslot_40_shadowstreams.refcount_after,
        r.vslot_40_streams_attach.took_fail_path ? 1 : 0,
        r.vslot_40_shadowrandomevents.refcount_after,
        r.vslot_40_randomevents_attach.took_fail_path ? 1 : 0,
        r.vslot_40_shadowbossmusic.refcount_after,
        r.vslot_40_bossmusic_attach.took_fail_path ? 1 : 0,
        r.vslot_40_cp3characters_attach.took_fail_path ? 1 : 0,
        r.vslot_40_cp3shadowprecache_attach.took_fail_path ? 1 : 0,
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
        static_cast<unsigned>(r.tail.gates.gate_byte), g_mid_reg,
        static_cast<unsigned long long>(r.ctor.tsc_value), r.remainder.registry.entry_count,
        static_cast<unsigned long long>(heap.alloc_count - allocs_before),
        static_cast<long long>(us));

    destroy_registry_at_2efd160(front);
    destroy_singleton_f08_at_2efc9e0(front, sh);
    destroy_vslot40_at_2ef8c08(front, bh.vslot_40_detail);
    std::puts("appmain_init_ctor_98a90_slice: all checks passed");
    return 0;
}
