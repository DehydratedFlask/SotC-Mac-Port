// CPU-01-HELPER-CALLEE-MODELS-01: shared fixture helpers for tests/test_helper_*.cpp.
#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/concmd_vars.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace helper_test {

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) { std::memset(static_cast<void*>(&obj), kPoison, sizeof obj); }

template <class T>
bool all_poison(const T& obj) {
    const auto* b = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t i = 0; i < sizeof obj; ++i) if (b[i] != kPoison) return false;
    return true;
}

template <class T>
bool same_bytes(const T& a, const T& b) { return std::memcmp(&a, &b, sizeof(T)) == 0; }

inline sotc::SharedStr view(const char* text) {
    return sotc::SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(std::strlen(text)),
                           0, nullptr};
}

// A caller functor block as the original's static initialiser builds it: four zero qwords, payload and aux set so
// the install sequence is observable.
inline sotc::RangedVarArgs make_args(std::uint64_t tag, std::uint64_t payload = 0x1234, std::uint64_t aux = 0x77,
                                     std::uint64_t q0 = 0, std::uint64_t q3 = 0) {
    sotc::RangedVarArgs a{};
    std::memset(&a, 0, sizeof a);
    a.q[0] = q0;
    a.q[1] = 0x1111;
    a.q[2] = 0x2222;
    a.q[3] = q3;
    a.fn_payload = payload;
    a.fn_vptr = tag;
    a.fn_aux = aux;
    return a;
}

}  // namespace helper_test
