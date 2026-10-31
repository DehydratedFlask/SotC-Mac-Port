#include "lookup_1fc80.hpp"

#include <cstring>
#include <strings.h>

namespace sotc {
namespace {

// OBS converts (end-begin) via sar3 + imul magic 0x86bca1af286bca1b (= /0x98).
// Native uses exact stride division for the same count.
[[nodiscard]] std::uint64_t elem_count(std::uintptr_t begin, std::uintptr_t end) noexcept {
    if (end < begin) {
        return 0;
    }
    return (end - begin) / kLookup1fc80Stride;
}

const char kEmpty[] = "";

}  // namespace

void* lookup_1fc80(void* table, const char* needle, const Lookup1fc80Hooks& hooks) noexcept {
    if (table == nullptr || needle == nullptr) {
        return nullptr;
    }
    auto* base = static_cast<std::uint8_t*>(table);
    void* begin_p = nullptr;
    void* end_p = nullptr;
    std::memcpy(&begin_p, base, sizeof(void*));
    std::memcpy(&end_p, base + 8, sizeof(void*));
    auto* begin = static_cast<std::uint8_t*>(begin_p);
    auto* end = static_cast<std::uint8_t*>(end_p);
    if (begin == end || begin == nullptr) {
        return nullptr;
    }

    const std::uint64_t n =
        elem_count(reinterpret_cast<std::uintptr_t>(begin), reinterpret_cast<std::uintptr_t>(end));

    auto cmp = [&](const char* a, const char* b) -> int {
        if (hooks.compare_f8f500 != nullptr) {
            return hooks.compare_f8f500(a, b);
        }
        return ::strcasecmp(a, b);
    };

    auto* cur = begin;
    for (std::uint64_t i = 0; i < n; ++i) {
        void* name_p = nullptr;
        std::memcpy(&name_p, cur + kLookup1fc80NameOff, sizeof(void*));
        const char* name = name_p != nullptr ? static_cast<const char*>(name_p) : kEmpty;
        if (cmp(name, needle) == 0) {
            return cur;
        }
        cur += kLookup1fc80Stride;
    }
    return nullptr;
}

}  // namespace sotc
