#include "ctor_40_1ab550.hpp"

#include <cstring>

namespace sotc {
namespace {

alignas(8) std::uint8_t g_base_vt_1598c30[8]{};
alignas(8) std::uint8_t g_derived_vt_1598ae0[8]{};
void* g_cell_2ee6778 = nullptr;

}  // namespace

void* ctor40_base_vtable_1598c30() noexcept { return g_base_vt_1598c30; }
void* ctor40_derived_vtable_1598ae0() noexcept { return g_derived_vt_1598ae0; }
void*& ctor40_cell_2ee6778() noexcept { return g_cell_2ee6778; }

void init_base_1b9250(void* obj) noexcept {
    if (obj == nullptr) {
        return;
    }
    auto* b = static_cast<std::uint8_t*>(obj);
    void* vt = ctor40_base_vtable_1598c30();
    std::memcpy(b, &vt, sizeof(void*));
    b[0x08] = 0;
    b[0x09] = 0;
    b[0x0a] = 1;
    b[0x0b] = 0;
    std::uint32_t one = 1;
    std::uint32_t zero = 0;
    std::memcpy(b + 0x0c, &one, 4);
    std::memcpy(b + 0x10, &zero, 4);
    std::memset(b + 0x20, 0, 16);
    std::memset(b + 0x2c, 0, 16);
    g_cell_2ee6778 = obj;
}

void init_ctor_40_1ab550(void* obj, const Ctor40_1ab550Hooks& hooks) noexcept {
    if (obj == nullptr) {
        return;
    }
    // OBS: call 0x1b9250
    init_base_1b9250(obj);
    // OBS: overwrite vtable with derived 0x1598ae0
    void* dvt = ctor40_derived_vtable_1598ae0();
    std::memcpy(obj, &dvt, sizeof(void*));
    if (hooks.plt_f901a0 != nullptr) {
        hooks.plt_f901a0();
    }
    if (hooks.plt_f901c0 != nullptr) {
        hooks.plt_f901c0();
    }
}

}  // namespace sotc
