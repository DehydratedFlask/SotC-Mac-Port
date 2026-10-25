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

// OBS 0x2552b6..0x2552cd: vslot+0x08 only (no delete_).
void install_slot_vslot_only(void*& cell, void* neu, void (*dtor)(void*)) noexcept {
    void* prev = cell;
    if (prev == neu) {
        return;
    }
    if (prev != nullptr && dtor != nullptr) {
        dtor(prev);
    }
    cell = neu;
}

[[nodiscard]] void*& app_slot(void* self, std::size_t off) noexcept {
    return *reinterpret_cast<void**>(static_cast<std::uint8_t*>(self) + off);
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

Prologue255180TailResult run_prologue_255180_tail(AllocatorFront& alloc, void* self,
                                                  const Prologue255180TailHooks& hooks) {
    Prologue255180TailResult out{};
    const bool have_ctor = (hooks.ctor_98a90_3be050 != nullptr) || hooks.use_lifted_ctor_98a90;
    if (self == nullptr || !have_ctor) {
        return out;
    }

    // OBS 0x255264..0x25529a: alloc 0x98a90 → ctor 0x3be050 → store self+0x3b58
    void* big = alloc.new_(kPrologue98a90Bytes);
    if (big == nullptr) {
        return out;
    }
    std::memset(big, 0, kPrologue98a90Bytes);
    if (hooks.ctor_98a90_3be050 != nullptr) {
        hooks.ctor_98a90_3be050(big);
    } else {
        const auto cr = init_98a90_3be050(alloc, big, hooks.ctor_98a90_detail);
        if (!cr.ok) {
            alloc.delete_(big);
            return out;
        }
    }
    install_cell(app_slot(self, kPrologueAppSlot3b58), big, alloc, hooks.dtor_98a90_3be390);
    out.obj_98a90 = big;

    // OBS 0x2552a1..0x2552cd: alloc 0x40 → ctor 0x1ab550 (lifted) → store self+0x3b40
    void* small = alloc.new_(kPrologueTail40Bytes);
    if (small == nullptr) {
        return out;
    }
    std::memset(small, 0, kPrologueTail40Bytes);
    if (hooks.ctor_40_1ab550 != nullptr) {
        hooks.ctor_40_1ab550(small);
    } else {
        init_ctor_40_1ab550(small, hooks.ctor_40_detail);
    }
    install_slot_vslot_only(app_slot(self, kPrologueAppSlot3b40), small, hooks.dtor_vslot08_3b40);
    out.obj_40 = small;

    // OBS 0x2552d4..0x255318: debug gates + a30 vslot+0x10 (stack-chk NOT modelled)
    if (hooks.run_gates) {
        out.gates = run_prologue_gates_2552d4(g_a30_cell_2dfd4b8, hooks.gates_detail);
    }

    out.ok = (out.obj_98a90 != nullptr) && (out.obj_40 != nullptr) &&
             (!hooks.run_gates || out.gates.ok);
    return out;
}

}  // namespace sotc
