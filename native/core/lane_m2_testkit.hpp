// Test-only helpers for the lane M2 fixtures (startup constructor slots 389 393 403 407 413 415 419 421).
// Header-only; builds on tests/l4_test_util.hpp (CHECK, poison, check_untouched, same_view, ...).
#pragma once

#include "../tests/l4_test_util.hpp"
#include "lane_m2_shapes.hpp"

namespace sotc::lane_m2 {

using namespace l4;

// A bool variable as built by init_bool_var_site / init_bool_var_flagged: every store accounted for.
inline void check_bool_var(const ConCmdBoolVar& v, const char* name, std::uint64_t fn_tag, std::uint8_t value,
                           std::uint32_t flags) {
    CHECK(v.base.vptr == kBoolVarFinalVptr);
    CHECK(v.value_ref == &v.value);
    for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
    for (std::uint8_t byte : v.zero70) CHECK(byte == 0);
    CHECK(v.fn_payload == 0 && v.fn_aux == 0 && v.fn_vptr == fn_tag);
    CHECK(v.base.registered == 1 && v.base.flags == flags);
    CHECK(v.value == value && v.default_value == value);
    CHECK(same_view(v.base.name, name, std::strlen(name)));
    CHECK(empty_view(v.base.description));
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 1},
                        {0x69, 4}, {0x70, 0x20}, {0xa0, 8}, {0xb0, 8}, {0xb8, 8}, {0xc0, 1}});
}

// The 0x26db0 menu item built from a bool variable: parent path in +0x28, caption = the variable's name view.
inline void check_toggle_item(const DebugMenuItem& it, const ConCmdBoolVar& var, const char* parent) {
    CHECK(it.target == &var && it.flags == 0x30 && it.kind == 2 && it.registered == 0);
    CHECK(same_view(it.label, parent, std::strlen(parent)));
    CHECK(it.text.data == var.base.name.data && it.text.size == var.base.name.size && it.text.ctl == nullptr);
    CHECK(dword(it, 0x98) == 0x3f800000u && dword(it, 0x9c) == 0x41200000u);
    CHECK(empty_view(it.empty));
    check_item_untouched(it);
}

inline void check_var_atexit_and_pending(const ConCmdRegistry& creg, const ConCmdBoolVar& var) {
    CHECK(creg.pending.size() == 1 && creg.pending.first() == &var.base.pending);
    CHECK(creg.atexit_log.size() >= 1);
    check_atexit(creg, 0, 0x20b0, &var);
}

}  // namespace sotc::lane_m2
