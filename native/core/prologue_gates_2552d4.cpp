#include "prologue_gates_2552d4.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_gate_flag_obj[0x68]{};
alignas(8) std::uint8_t g_gate_vtbl_obj[0x10]{};
alignas(8) std::uint8_t g_debug_obj[kPrologueDebugObjBytes]{};
std::uint8_t g_gate_flag_byte = 1;  // OBS non-zero → skip vslot+0x48 (al=0)
std::uint8_t g_debug_state_byte = 0;
bool g_objs_inited = false;

void ensure_objs() noexcept {
    if (g_objs_inited) {
        return;
    }
    std::memset(g_gate_flag_obj, 0, sizeof(g_gate_flag_obj));
    std::memset(g_gate_vtbl_obj, 0, sizeof(g_gate_vtbl_obj));
    std::memset(g_debug_obj, 0, sizeof(g_debug_obj));
    void* flag_ptr = &g_gate_flag_byte;
    std::memcpy(g_gate_flag_obj + 0x60, &flag_ptr, sizeof(void*));
    void* state_ptr = &g_debug_state_byte;
    std::memcpy(g_debug_obj + 0x60, &state_ptr, sizeof(void*));
    // +0x6a/+0x70/+0x78 already 0 → empty ba40 path
    g_objs_inited = true;
}

}  // namespace

std::uint8_t& prologue_gate_flag_byte() noexcept {
    ensure_objs();
    return g_gate_flag_byte;
}

std::uint8_t& prologue_debug_state_byte() noexcept {
    ensure_objs();
    return g_debug_state_byte;
}

void* prologue_debug_obj_2eec840() noexcept {
    ensure_objs();
    return g_debug_obj;
}

void init_ba40_empty(void* debug_obj, std::uint8_t* state, std::uint8_t* gate_byte) noexcept {
    if (debug_obj == nullptr || state == nullptr || gate_byte == nullptr) {
        return;
    }
    auto* o = static_cast<std::uint8_t*>(debug_obj);
    // OBS: if [rcx+0x6a]==0 → dl=*r9; else complex — NOT modelled here (caller gates).
    if (o[0x6a] != 0) {
        return;
    }
    const std::uint8_t dl = *gate_byte;
    const std::uint8_t al = *state;
    if (dl == al) {
        return;
    }
    *state = dl;
    void* begin = nullptr;
    void* end_or_tag = nullptr;
    std::memcpy(&begin, o + 0x70, sizeof(void*));
    std::memcpy(&end_or_tag, o + 0x78, sizeof(void*));
    // Listeners non-null → NOT modelled (no callback invoke).
    (void)begin;
    (void)end_or_tag;
}

PrologueGates2552d4Result run_prologue_gates_2552d4(void* a30,
                                                    const PrologueGates2552d4Hooks& hooks) noexcept {
    PrologueGates2552d4Result out{};
    ensure_objs();

    // OBS 0x2552d4..0x2552f5: gate flag → optional vslot+0x48 → al
    void* flag_ptr = nullptr;
    std::memcpy(&flag_ptr, g_gate_flag_obj + 0x60, sizeof(void*));
    std::uint8_t al = 0;
    if (flag_ptr != nullptr && *static_cast<std::uint8_t*>(flag_ptr) == 0) {
        out.took_vslot_48 = true;
        bool v = false;
        if (hooks.vslot_48_2dfcf40 != nullptr) {
            v = hooks.vslot_48_2dfcf40();
        }
        al = static_cast<std::uint8_t>(v ? 0 : 1);  // OBS: xor al, 1
    } else {
        al = 0;
    }
    out.gate_byte = al;

    // OBS 0x2552f7..0x255309: ba40(debug, state, &al)
    void* state_ptr = nullptr;
    std::memcpy(&state_ptr, g_debug_obj + 0x60, sizeof(void*));
    const std::uint8_t state_before =
        (state_ptr != nullptr) ? *static_cast<std::uint8_t*>(state_ptr) : 0;
    if (hooks.ba40 != nullptr) {
        hooks.ba40(g_debug_obj, static_cast<std::uint8_t*>(state_ptr), &out.gate_byte);
    } else {
        init_ba40_empty(g_debug_obj, static_cast<std::uint8_t*>(state_ptr), &out.gate_byte);
    }
    out.ba40_ran = true;
    if (state_ptr != nullptr) {
        out.state_updated = (*static_cast<std::uint8_t*>(state_ptr) != state_before);
    }

    // OBS 0x25530e..0x255315: vcall a30+0x10
    if (a30 != nullptr) {
        if (hooks.a30_vslot_10 != nullptr) {
            hooks.a30_vslot_10(a30);
            out.a30_vslot_called = true;
        }
    }

    // Stack canary 0x255318.. — NOT modelled
    out.ok = out.ba40_ran;
    return out;
}

}  // namespace sotc
