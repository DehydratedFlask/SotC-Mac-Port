// Shared helpers of the L3-SLOTS-322-334 fixtures (tests/test_ctor_slot_322 .. 334). Header-only so
// each slot keeps its own test executable.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <utility>

#include "core/concmd_base.hpp"
#include "core/concmd_vars.hpp"
#include "core/debug_menu_item.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace famtest {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

// Every byte outside `written` must still hold the poison pattern: the original stores exactly the
// listed ranges and nothing else.
template <class T>
void check_untouched(const T& obj, std::initializer_list<std::pair<std::size_t, std::size_t>> written) {
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t off = 0; off < sizeof obj; ++off) {
        bool is_written = false;
        for (const auto& [begin, size] : written) {
            if (off >= begin && off < begin + size) { is_written = true; break; }
        }
        if (!is_written && bytes[off] != kPoison) {
            std::fprintf(stderr, "untouched byte +0x%zx changed to 0x%02x\n", off, bytes[off]);
            std::exit(1);
        }
    }
}

template <class T>
std::uint32_t dword_at(const T& obj, std::size_t off) {
    std::uint32_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 4);
    return v;
}

inline bool same_view(const SharedStr& v, const char* text, std::size_t len) {
    return v.data != nullptr && v.size == static_cast<std::int64_t>(len) && v.aux == 0 &&
           std::memcmp(v.data, text, len) == 0 && v.ctl == nullptr;
}

inline bool empty_view(const SharedStr& v) {
    return v.data != nullptr && v.size == 0 && v.data[0] == 0 && v.aux == 0 && v.ctl == nullptr;
}

// Bytes the shared bool-variable shape stores (concmd_vars.hpp).
inline void check_bool_untouched(const ConCmdBoolVar& v) {
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8},
                        {0x68, 1}, {0x69, 4}, {0x70, 0x20}, {0xa0, 8}, {0xb0, 8}, {0xb8, 8}, {0xc0, 1}});
}

inline void check_bool_var(const ConCmdBoolVar& v, const char* name, std::size_t name_len,
                           std::uint64_t fn_tag, std::uint8_t value, std::uint8_t def) {
    CHECK(v.base.vptr == kBoolVarFinalVptr);
    CHECK(v.value_ref == &v.value);
    for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
    for (std::uint8_t byte : v.zero70) CHECK(byte == 0);
    CHECK(v.fn_payload == 0 && v.fn_aux == 0 && v.fn_vptr == fn_tag);
    CHECK(v.base.registered == 1 && v.base.flags == 1);
    CHECK(v.value == value && v.default_value == def);
    CHECK(same_view(v.base.name, name, name_len));
    CHECK(empty_view(v.base.description));
    check_bool_untouched(v);
}

// Bytes a debug-menu item constructor stores.
inline void check_item_untouched(const DebugMenuItem& it) {
    check_untouched(it, {{0x00, 0x20}, {0x20, 1}, {0x28, 0x20}, {0x48, 4}, {0x50, 0x20}, {0x70, 0x20},
                         {0x90, 8}, {0x98, 4}, {0x9c, 4}, {0xa0, 4}});
}

// kind 2 = leaf item (0x26ea0 / 0x26db0), kind 1 = group (0x26c10); default range {1.0f, 10.0f}.
inline void check_leaf_item(const DebugMenuItem& it, const void* target, std::uint32_t flags,
                            const char* label, std::size_t label_len, const char* text, std::size_t text_len) {
    CHECK(it.target == target && it.flags == flags && it.kind == 2 && it.registered == 0);
    CHECK(dword_at(it, 0x98) == 0x3f800000u && dword_at(it, 0x9c) == 0x41200000u);
    CHECK(same_view(it.label, label, label_len) && same_view(it.text, text, text_len));
    CHECK(it.empty.data != nullptr && it.empty.size == 0 && it.empty.ctl == nullptr);
    check_item_untouched(it);
}

inline SharedStr view(const char* text, std::size_t len) {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(len), 0, nullptr};
}

inline bool parent_insert_called = false;
inline bool parent_insert(void*, DebugMenuItem&) noexcept { parent_insert_called = true; return true; }

}  // namespace famtest
