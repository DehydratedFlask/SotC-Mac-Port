// Measured Init path: prologue early + tail using lifted 0x3be050 (header+0x3be930+ensure+
// 0x80 grow+null-head list+tsc) + lifted 0x1ab550 (0x40 ctor) + debug gates 0x2552d4
// → 0xf08 singleton install → base (lifted vslot 0x534130 + mid 0x255340 +
// bounded 0x533100 header+post+miss+join+gfx+shadowmodels+attach+shadowskels+
// skels-attach+shadowanims+anims-attach+shadowbanks+banks-attach+shadowstreams+
// streams-attach+shadowrandomevents+randomevents-attach+shadowbossmusic+
// bossmusic-attach+cp3characters+cp3characters-attach+cp3shadowprecache) →
// registry. Tip AllocatorFront+FakeHeap.
// PR #5 cite-only.
// NOT modelled: ensure migrate / heavy menu helpers / 0xf8f860 PLT / non-null list;
// PLT bodies 0xf901a0/0xf901c0; stack-chk; ba40 listener invoke; 0x533100 hit /
// success atexit/next path; 0x14c2f0/0x6a5370/0x6abcc0/0x21620/0x2779a0/0x42390
// bodies; bodies of 0x523bd0 / 0x525b40 / 0x493230 / 0x525080 / 0x2569d0.
#pragma once

#include "allocator_front.hpp"
#include "appmain_init_remainder_slice.hpp"
#include "base_initialize_255130.hpp"
#include "ctor_40_1ab550.hpp"
#include "ctor_98a90_3be050.hpp"
#include "input_table_551140.hpp"
#include "prologue_255180.hpp"
#include "singleton_f08_531d84.hpp"
#include "vslot_40_533100_header.hpp"

#include <cstdint>

namespace sotc {

struct AppmainInitCtor40SliceSide {
    bool ok = false;
    bool derived_vt = false;
    bool cell_self = false;
    bool field_0a = false;
};

struct AppmainInitCtor98a90SliceResult {
    Prologue255180Result early{};
    Prologue255180TailResult tail{};
    Ctor98a90_3be050Result ctor{};
    AppmainInitCtor40SliceSide ctor_40{};
    SingletonF08Result singleton{};
    Vslot40_533100HeaderResult vslot_40{};
    Vslot40_533100PostResult vslot_40_post{};
    Vslot40_533100MissResult vslot_40_miss{};
    Vslot40_533100JoinResult vslot_40_join{};
    Vslot40_533100GfxResult vslot_40_gfx{};
    Vslot40_533100ShadowResult vslot_40_shadow{};
    Vslot40_533100ShadowAttachResult vslot_40_shadow_attach{};
    Vslot40_533100ShadowskelsResult vslot_40_shadowskels{};
    Vslot40_533100SkelsAttachResult vslot_40_skels_attach{};
    Vslot40_533100ShadowanimsResult vslot_40_shadowanims{};
    Vslot40_533100AnimsAttachResult vslot_40_anims_attach{};
    Vslot40_533100ShadowbanksResult vslot_40_shadowbanks{};
    Vslot40_533100BanksAttachResult vslot_40_banks_attach{};
    Vslot40_533100ShadowstreamsResult vslot_40_shadowstreams{};
    Vslot40_533100StreamsAttachResult vslot_40_streams_attach{};
    Vslot40_533100ShadowrandomeventsResult vslot_40_shadowrandomevents{};
    Vslot40_533100RandomeventsAttachResult vslot_40_randomevents_attach{};
    Vslot40_533100ShadowbossmusicResult vslot_40_shadowbossmusic{};
    Vslot40_533100BossmusicAttachResult vslot_40_bossmusic_attach{};
    Vslot40_533100Cp3charactersResult vslot_40_cp3characters{};
    Vslot40_533100Cp3charactersAttachResult vslot_40_cp3characters_attach{};
    Vslot40_533100Cp3shadowprecacheResult vslot_40_cp3shadowprecache{};
    bool base_ok = false;
    std::uint32_t flag_after_base = 0;
    AppmainInitRemainderSliceResult remainder{};
};

[[nodiscard]] AppmainInitCtor98a90SliceResult run_appmain_init_ctor_98a90_slice(
    InputTable551140& table, AllocatorFront& alloc, void* self, void* arg,
    const Prologue255180Hooks& early_hooks, const Prologue255180TailHooks& tail_hooks,
    const Ctor98a90_3be050Hooks& ctor_hooks, const BaseInitialize255130Hooks& base_hooks,
    void* setter_obj, std::uint8_t flag_in, std::uint8_t set_a, std::uint8_t set_b,
    std::uint8_t set_c, const SingletonF08Hooks& singleton_hooks = {});

}  // namespace sotc
