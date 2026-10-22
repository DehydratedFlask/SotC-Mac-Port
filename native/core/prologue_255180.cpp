#include "prologue_255180.hpp"

#include <cstring>

namespace sotc {
namespace {

void* g_a30_cell_2dfd4b8 = nullptr;

void install_cell(void*& cell, void* neu, AllocatorFront& alloc,
                  void (*dtor)(void*)) noexcept {
    void* prev = cell;
    if (prev == neu) {
        return;
    }
    if (prev != nullptr) {
        if (dtor != nullptr) {
            dtor(prev);
        }
        alloc.delete_(prev);
    }
    cell = neu;
}

}  // namespace

void*& prologue_a30_cell_2dfd4b8() noexcept { return g_a30_cell_2dfd4b8; }

Prologue255180Result run_prologue_255180_early(AllocatorFront& alloc,
                                               const Prologue255180Hooks& hooks) {
    Prologue255180Result out{};
    if (hooks.ctor_a30_264e50 == nullptr) {
        return out;
    }

    // OBS 0x25519b..0x2551d3: alloc 0xa30 → ctor → store 0x2dfd4b8
    void* a30 = alloc.new_(kPrologueA30Bytes);
    if (a30 == nullptr) {
        return out;
    }
    std::memset(a30, 0, kPrologueA30Bytes);
    hooks.ctor_a30_264e50(a30);
    install_cell(g_a30_cell_2dfd4b8, a30, alloc, hooks.dtor_vslot08);
    out.a30_obj = a30;

    // OBS 0x2551d7 / 0x2551dc
    if (hooks.call_11e70 != nullptr) {
        hooks.call_11e70();
    }
    if (hooks.call_3df40 != nullptr) {
        hooks.call_3df40();
    }

    // OBS 0x2551e9..0x25523b: alloc 0x40 → 0x335a0 → store 0x2ea4718
    PrologueFlagConfig255180 cfg{};
    void* flag = alloc.new_(kFlagObject335a0Bytes);
    if (flag == nullptr) {
        return out;
    }
    init_flag_object_335a0(flag);
    install_cell(base_init_flag_cell_2ea4718(), flag, alloc, hooks.dtor_vslot08);
    out.flag_obj = flag;

    // OBS 0x255245: call 0x335d0(flag, &cfg)
    if (hooks.apply_335d0 != nullptr) {
        hooks.apply_335d0(flag, cfg);
    }

    // OBS 0x255252: call 0x357f0(flag, 1)
    (void)xchg_store_u32_357f0(flag, 1);
    std::memcpy(&out.flag_word_20, static_cast<std::uint8_t*>(flag) + kXchgStore357f0Offset, 4);

    // OBS 0x25525f: call 0x35770(flag, 1) — TLS; hook optional
    if (hooks.tls_35770 != nullptr) {
        hooks.tls_35770(flag, 1);
    }

    out.ok = (out.flag_word_20 == 1) && (out.a30_obj != nullptr) && (out.flag_obj != nullptr);
    return out;
}

}  // namespace sotc
