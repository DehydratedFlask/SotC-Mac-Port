#include "vslot_30_534130.hpp"

namespace sotc {
namespace {

std::int32_t g_boss_val = 0;
std::int32_t g_boss_mode = 0;
std::uint8_t g_boss_flag = 0;
std::int32_t g_stage = 0;

int clamp_nonneg(int v, int hi_exclusive_max) noexcept {
    // OBS: ebx = max(0, eax) via cmovns; then min(ebx, max) via cmovl with imm.
    int ebx = (v < 0) ? 0 : v;
    int eax = hi_exclusive_max;  // 0xf or 5
    if (ebx < hi_exclusive_max + 1) {
        eax = ebx;
    }
    return eax;
}

}  // namespace

std::int32_t& vslot30_cell_boss_val_325e8b4() noexcept { return g_boss_val; }
std::int32_t& vslot30_cell_boss_mode_325e8b8() noexcept { return g_boss_mode; }
std::uint8_t& vslot30_cell_boss_flag_2dcdf14() noexcept { return g_boss_flag; }
std::int32_t& vslot30_cell_stage_2dd9578() noexcept { return g_stage; }

Vslot30_534130Result vslot_30_534130(void* self, const Vslot30_534130Hooks& hooks) noexcept {
    Vslot30_534130Result out{};
    if (self == nullptr) {
        return out;
    }

    // OBS: call 0x255f60
    out.precheck_ran = true;
    bool pre = true;
    if (hooks.precheck_255f60 != nullptr) {
        pre = hooks.precheck_255f60(self);
    }
    out.precheck_ok = pre;
    if (!pre) {
        out.ok = false;
        return out;
    }

    void* table = static_cast<std::uint8_t*>(self) + 8;

    auto lookup = [&](const char* name) -> void* {
        ++out.lookups;
        if (hooks.lookup_1fc80 == nullptr) {
            return nullptr;
        }
        return hooks.lookup_1fc80(table, name);
    };
    auto parse = [&](void* node) -> int {
        ++out.parses;
        if (hooks.parse_1e5e0 == nullptr) {
            return 0;
        }
        return hooks.parse_1e5e0(node);
    };

    // boss / shadow_jumpToBoss
    void* node = lookup(kVslot30NameBoss);
    if (node == nullptr) {
        node = lookup(kVslot30NameJump);
    }
    if (node != nullptr) {
        int v = parse(node);
        g_boss_val = clamp_nonneg(v, 0xf);
        g_boss_mode = 0;
        g_boss_flag = 0;
    }

    // boss_outro
    node = lookup(kVslot30NameOutro);
    if (node != nullptr) {
        int v = parse(node);
        int clamped = clamp_nonneg(v, 0xf);
        g_boss_val = clamped + 1;  // OBS: inc eax
        g_boss_mode = 1;
        g_boss_flag = 0;
    }

    // stage — return true regardless of miss (OBS mov r14b,1 before test)
    node = lookup(kVslot30NameStage);
    if (node != nullptr) {
        int v = parse(node);
        g_stage = clamp_nonneg(v, 5);
    }

    out.ok = true;
    return out;
}

}  // namespace sotc
