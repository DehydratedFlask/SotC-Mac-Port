// Shared helpers for the lane L4 fixtures (startup constructor slots 343..356). Header-only; test-only.
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
#include "core/ctor_shapes_343_356.hpp"
#include "core/debug_menu_item.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace l4 {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

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
std::uint32_t dword(const T& obj, std::size_t off) {
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

inline void check_bool_common(const ConCmdBoolVar& v, std::uint64_t fn_tag, std::uint8_t value) {
    CHECK(v.base.vptr == kBoolVarFinalVptr);
    CHECK(v.value_ref == &v.value);
    for (std::uint8_t byte : v.zero69) CHECK(byte == 0);
    for (std::uint8_t byte : v.zero70) CHECK(byte == 0);
    CHECK(v.fn_payload == 0 && v.fn_aux == 0 && v.fn_vptr == fn_tag);
    CHECK(v.base.registered == 1 && v.base.flags == 1);
    CHECK(v.value == value && v.default_value == value);
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 1},
                        {0x69, 4}, {0x70, 0x20}, {0xa0, 8}, {0xb0, 8}, {0xb8, 8}, {0xc0, 1}});
    CHECK(empty_view(v.base.description));
}

inline void check_float_common(const ConCmdRangedVar& v, bool bounded, std::uint32_t lo_bits, std::uint32_t hi_bits,
                               std::uint32_t default_bits) {
    CHECK(v.base.vptr == kRangedFloatFinalVptr);                  // 0x15942b0
    CHECK(v.value_ref == &v.value);
    CHECK(v.base.registered == 1 && v.base.flags == 1);
    CHECK(v.value == default_bits && v.default_value == default_bits);
    CHECK(v.min_engaged == (bounded ? 1 : 0) && v.max_engaged == (bounded ? 1 : 0));
    if (bounded) {
        CHECK(dword(v, 0x6c) == lo_bits && dword(v, 0x74) == hi_bits);
    } else {
        CHECK(v.min_value[0] == 0 && v.max_value[0] == 0);
    }
    for (std::uint64_t a : v.args) CHECK(a == 0);
    CHECK(v.fn_payload == 0 && v.fn_aux == 0 && v.fn_vptr == kRangedFloatFnVtbl);   // 0x1594318
    CHECK(empty_view(v.base.description));
    const std::size_t lo_len = bounded ? 4 : 1;
    check_untouched(v, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 8}, {0x68, 4},
                        {0x6c, lo_len}, {0x70, 1}, {0x74, lo_len}, {0x78, 1}, {0x80, 0x20}, {0xb0, 8},
                        {0xc0, 8}, {0xc8, 8}, {0xd0, 4}});
}

inline void check_command(const ConCmdCommand& c, const char* name, const char* desc, std::size_t desc_len,
                          std::uint64_t handler) {
    CHECK(c.base.vptr == kFConCmdVptr && c.base.flags == 2 && c.base.registered == 1);
    CHECK(same_view(c.base.name, name, std::strlen(name)));
    CHECK(c.base.description.data != nullptr && c.base.description.size == static_cast<std::int64_t>(desc_len) &&
          std::memcmp(c.base.description.data, desc, desc_len) == 0 && c.base.description.ctl == nullptr);
    CHECK(c.delegate.object == reinterpret_cast<std::uint64_t>(&c.delegate));
    CHECK(c.delegate.fn == 0x1990 && c.delegate.adj == 0 && c.delegate.handler == handler);
    CHECK(c.args_begin == nullptr && c.args_end == nullptr && c.args_cap == nullptr);
    check_untouched(c, {{0x00, 8}, {0x08, 0x10}, {0x18, 1}, {0x1c, 4}, {0x20, 0x40}, {0x60, 0x20}, {0x80, 0x18}});
}

inline void check_item_untouched(const DebugMenuItem& it) {
    check_untouched(it, {{0x00, 0x20}, {0x20, 1}, {0x28, 0x20}, {0x48, 4}, {0x50, 0x20}, {0x70, 0x20},
                         {0x90, 8}, {0x98, 4}, {0x9c, 4}, {0xa0, 4}});
}

inline void check_atexit(const ConCmdRegistry& r, std::size_t i, std::uint64_t rva, const void* obj) {
    CHECK(i < r.atexit_log.size());
    CHECK(r.atexit_log.at(i).dtor_rva == rva && r.atexit_log.at(i).object == obj);
}

}  // namespace l4
