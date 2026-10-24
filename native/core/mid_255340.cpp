#include "mid_255340.hpp"

namespace sotc {
namespace {

alignas(8) std::uint8_t g_obj_2dfd1d0[0x10]{};
alignas(8) std::uint8_t g_obj_2eecde8[0x10]{};

// Stand-in temps for dtor/cleanup hook arguments when bodies are not modelled.
alignas(8) std::uint8_t g_temp_a[8]{};
alignas(8) std::uint8_t g_temp_b[8]{};
alignas(8) std::uint8_t g_menu_a[8]{};
alignas(8) std::uint8_t g_menu_b[8]{};
alignas(8) std::uint8_t g_menu_c[8]{};
alignas(8) std::uint8_t g_menu_d[8]{};
alignas(8) std::uint8_t g_menu_e[8]{};

}  // namespace

void* mid_255340_obj_2dfd1d0() noexcept { return g_obj_2dfd1d0; }
void* mid_255340_obj_2eecde8() noexcept { return g_obj_2eecde8; }

Mid255340Result mid_255340(void* self, const Mid255340Hooks& hooks) noexcept {
    Mid255340Result out{};
    if (self == nullptr) {
        return out;
    }
    auto* base = static_cast<std::uint8_t*>(self);

    // OBS: vcall [vt+0x50]
    if (hooks.vslot_50 != nullptr) {
        hooks.vslot_50(self);
        ++out.vslot_50_calls;
    }

    auto reg26ea0 = [&](void* attach, const char* title, const char* item) {
        if (hooks.register_26ea0 != nullptr) {
            hooks.register_26ea0(attach, title, item);
        }
        ++out.register_26ea0_calls;
        // OBS: two temp hashed-string dtors after each 26ea0
        if (hooks.dtor_f590 != nullptr) {
            hooks.dtor_f590(g_temp_a);
            hooks.dtor_f590(g_temp_b);
        }
        out.dtor_f590_calls += 2;
    };
    auto reg26b60 = [&](const char* title) {
        if (hooks.register_26b60 != nullptr) {
            hooks.register_26b60(title);
        }
        ++out.register_26b60_calls;
        if (hooks.dtor_f590 != nullptr) {
            hooks.dtor_f590(g_temp_a);
        }
        ++out.dtor_f590_calls;
    };
    auto cleanup = [&](void* obj) {
        if (hooks.cleanup_272d0 != nullptr) {
            hooks.cleanup_272d0(obj);
        }
        ++out.cleanup_272d0_calls;
    };

    // Block order matches OBS 0x25536c..0x2555e1
    reg26ea0(base + kMid255340Off2688, kMid255340NameDebugGame, kMid255340NameToggleCam);
    reg26ea0(g_obj_2dfd1d0, kMid255340NameDebugGame, kMid255340NameMirror);
    reg26b60(kMid255340NameDebug);
    reg26ea0(base + kMid255340Off2868, kMid255340NameDebug, kMid255340NameFixedTs);
    reg26ea0(g_obj_2eecde8, kMid255340NameDebug, kMid255340NameDeltaTol);

    // OBS: five 0x272d0 cleanups (menus constructed on stack)
    cleanup(g_menu_a);
    cleanup(g_menu_b);
    cleanup(g_menu_c);
    cleanup(g_menu_d);
    cleanup(g_menu_e);

    // Stack-chk — NOT modelled
    out.ok = (out.register_26ea0_calls == 4) && (out.register_26b60_calls == 1) &&
             (out.cleanup_272d0_calls == 5) && (out.dtor_f590_calls == 9);
    return out;
}

}  // namespace sotc
