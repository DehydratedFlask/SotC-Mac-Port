// Per-call trace shared by the three lifted post-base Init bodies.
//
// A record is emitted for every invocation the body actually makes. `executed_native` means the
// hook ran an ordinary native body and returned its observed result; `blocked` means the hook
// refused because a named input is absent and copied its diagnostic into `detail` verbatim.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kInitPostCallMax = 64;
inline constexpr std::size_t kInitPostCallDetailBytes = 224;
inline constexpr std::size_t kInitPostCallArgMax = 6;

struct InitPostCallOutcome {
    bool executed = false;
    const char* blocker = nullptr;
    bool has_value = false;
    std::uint64_t value = 0;
    char detail[kInitPostCallDetailBytes]{};
};

struct InitPostCallRecord {
    std::uint64_t callee_rva = 0;
    const char* caller = "";
    bool executed = false;
    std::uint8_t arg_count = 0;
    std::uint64_t args[kInitPostCallArgMax]{};
    bool has_return = false;
    std::uint64_t return_value = 0;
    const char* blocker = nullptr;
    char detail[kInitPostCallDetailBytes]{};
};

struct InitPostCallLog {
    InitPostCallRecord records[kInitPostCallMax]{};
    std::size_t count = 0;
};

}  // namespace sotc