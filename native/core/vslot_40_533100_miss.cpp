#include "vslot_40_533100_miss.hpp"

#include <cstring>

namespace sotc {
namespace {

void* g_cell_2efe560 = nullptr;
void* g_cell_2efe5c0 = nullptr;

// Stand-in stack hashed-string / temps for dtor hooks.
alignas(8) std::uint8_t g_name_obj[0x40]{};
alignas(8) std::uint8_t g_temp_a[8]{};
alignas(8) std::uint8_t g_temp_b[8]{};

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

    // OBS: two 0xf590 stack dtors
    if (hooks.dtor_f590 != nullptr) {
        hooks.dtor_f590(g_temp_a);
        hooks.dtor_f590(g_temp_b);
    }
    out.dtor_f590_calls = 2;

    // Join @ 0x533305 — NOT modelled
    out.ok = (out.find_21620_calls == 1) && (out.ba40_calls == 1) && (out.dtor_f590_calls == 2);
    return out;
}

}  // namespace sotc
