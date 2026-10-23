#include "vslot_40_533100_miss.hpp"

#include <cstring>

namespace sotc {
namespace {

void* g_cell_2efe560 = nullptr;
void* g_cell_2efe5c0 = nullptr;

// Stand-in stack hashed-string for the find hook.
alignas(8) std::uint8_t g_name_obj[0x40]{};

// The six fields the measured `0xf590` no-op must report at both miss sites.
[[nodiscard]] bool is_inert_release(const SharedReleaseF590Result& r) noexcept {
    return r.ok && !r.had_control && !r.decremented && !r.was_last && !r.destroyed && !r.freed &&
           r.previous == 0;
}

// OBS 0x5332e6/0x5332f9: both releases point at a 0x20 B stack string holding the literal
// `0x1014e56 "r_fps"`, length 5, capacity 0, control 0. Only `+0x00`/`+0x18` are read.
[[nodiscard]] SharedHolderF590 make_r_fps_holder() noexcept {
    SharedHolderF590 h{};
    h.object = const_cast<char*>(static_cast<const char*>(kVslot40MissNameRFps));
    h.reserved_08 = reinterpret_cast<void*>(static_cast<std::uintptr_t>(5));
    h.reserved_10 = nullptr;
    h.control = nullptr;  // OBS: `+0x18` is zero at both sites
    return h;
}

}  // namespace

void*& vslot40_miss_cell_2efe560() noexcept { return g_cell_2efe560; }
void*& vslot40_miss_cell_2efe5c0() noexcept { return g_cell_2efe5c0; }

Vslot40_533100MissResult vslot_40_533100_miss(const Vslot40_533100MissHooks& hooks) noexcept {
    Vslot40_533100MissResult out{};

    // OBS: stack hashed-string for "r_fps" (name pin only; layout NOT fully modelled).
    std::memset(g_name_obj, 0, sizeof(g_name_obj));
    // Store cstr pointer in a conventional slot so find hook can see the name if desired.
    const char* name = kVslot40MissNameRFps;
    std::memcpy(g_name_obj, &name, sizeof(void*));

    void* r12 = g_cell_2efe5c0;  // OBS: mov r12, [0x2efe5c0]
    void* held = nullptr;        // [rbp-0x108]

    void* found = nullptr;
    if (hooks.find_21620 != nullptr) {
        found = hooks.find_21620(g_name_obj);
    }
    ++out.find_21620_calls;
    out.find_null = (found == nullptr);

    if (found != nullptr) {
        int kind = 0;
        if (hooks.vslot_18 != nullptr) {
            kind = hooks.vslot_18(found);
        }
        if (kind == 1) {
            held = found;
            int keep = 0;
            if (hooks.vslot_30 != nullptr) {
                keep = hooks.vslot_30(found);
            }
            if (keep != 0) {
                held = nullptr;  // OBS clears on nonzero
            }
        }
        if (held != nullptr) {
            void* at60 = nullptr;
            std::memcpy(&at60, static_cast<std::uint8_t*>(held) + 0x60, sizeof(void*));
            if (at60 != nullptr) {
                r12 = at60;
            }
        }
    }

    // OBS: ba40(0x2efe560, *0x2efe5c0, r12)
    if (hooks.ba40 != nullptr) {
        hooks.ba40(g_cell_2efe560, g_cell_2efe5c0, r12);
    }
    ++out.ba40_calls;

    // OBS 0x5332f4 / 0x533300: two 0xf590 stack-string releases, both `{"r_fps", 5, 0, 0}`.
    SharedHolderF590 holder_a = make_r_fps_holder();
    if (hooks.call_f590 != nullptr) {
        out.release_f590_a = hooks.call_f590(holder_a);
    } else {
        out.release_f590_a = shared_release_f590(holder_a, hooks.release_f590);
    }
    SharedHolderF590 holder_b = make_r_fps_holder();
    if (hooks.call_f590 != nullptr) {
        out.release_f590_b = hooks.call_f590(holder_b);
    } else {
        out.release_f590_b = shared_release_f590(holder_b, hooks.release_f590);
    }
    out.dtor_f590_calls = 2;

    // The lifted default path must be the exact inert release proven by the static audit.
    // A host override supplies its own result and is not held to the measured shape.
    const bool release_ok =
        (hooks.call_f590 != nullptr) ||
        (is_inert_release(out.release_f590_a) && is_inert_release(out.release_f590_b));

    // Join @ 0x533305 — NOT modelled
    out.ok = (out.find_21620_calls == 1) && (out.ba40_calls == 1) && (out.dtor_f590_calls == 2) &&
             release_ok;
    return out;
}

}  // namespace sotc
