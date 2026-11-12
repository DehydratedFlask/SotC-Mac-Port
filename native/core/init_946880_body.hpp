// Native models for the Init post-base call chain entered at 0x946880.
//
// 0x946880 (353 B FDE, 0x946880..0x9469e1) is a spin-locked initializer. It queries
// 0x98a450 for a thread/user handle. A valid handle (!= -1 at result+0x50) selects the
// 0x944de0 branch; an invalid handle selects 0x9469f0. The observed Init calls pass
// values 1 and 0, use the 0x9469f0 branch, and return its status.
//
// 0x9469f0 (360 B FDE, 0x9469f0..0x946b58) locks a service handle, obtains a 0x10-byte
// record {u32 index; u32 pad; const char* name}, copies the name into two 0x40-byte
// fields at service+0x1d0 and service+0x240 with the 0x1a8-byte record stride, then
// releases the handle. Its record blob is obtained through 0x988400: first with a null
// buffer to obtain the size, then with the buffer to fill it.
//
// All direct calls are explicit hooks. A missing hook or a hook that refuses produces a
// named blocker with the observed arguments and the hook's verbatim detail; no plausible
// return is substituted. 0x944de0 itself is a named contract on this host (the observed
// query result selects 0x9469f0), not a claimed native body.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "init_post_call_trace.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit946880BodyRva = 0x946880;
inline constexpr std::size_t kInit946880BodySize = 353;
inline constexpr std::uint64_t kInit946880BodyEndRva = 0x9469e1;
inline constexpr std::uint64_t kInit946880LockRva = 0x2dd0a78;
inline constexpr std::uint64_t kInit946880ServiceRva = 0x32950a8;
inline constexpr std::uint64_t kInit946880LogFlagRva = 0x2dd0a30;
inline constexpr std::uint64_t kInit946880LogCounterRva = 0x32951ec;

inline constexpr std::uint64_t kInit9469f0BodyRva = 0x9469f0;
inline constexpr std::size_t kInit9469f0BodySize = 360;
inline constexpr std::uint64_t kInit9469f0BodyEndRva = 0x946b58;
inline constexpr std::uint64_t kInit944de0BodyRva = 0x944de0;
inline constexpr std::size_t kInit944de0BodySize = 399;
inline constexpr std::size_t kInit9469f0ServiceStride = 0x1a8;
inline constexpr std::size_t kInit9469f0ServiceIdOff = 0x1a8;
inline constexpr std::size_t kInit9469f0Name0Off = 0x1d0;
inline constexpr std::size_t kInit9469f0Name1Off = 0x240;
inline constexpr std::size_t kInit9469f0NameBytes = 0x40;
inline constexpr std::size_t kInit9469f0RecordIndexOff = 0x00;
inline constexpr std::size_t kInit9469f0RecordNameOff = 0x08;
inline constexpr std::size_t kInit9469f0RecordBytes = 0x10;
inline constexpr std::size_t kInit9469f0MaxRecordBytes = 0x1000;  // host safety bound
inline constexpr std::uintptr_t kInit9469f0DefaultNameRva = 0x12ab834;  // empty string

// 0x98a450 writes a larger record; 0x946880 only observes the qword at +0x50.
struct Init98a450Result {
    std::int32_t field50 = -1;  // 0x94690e compares the dword at result+0x50
};

struct Init9469f0Hooks {
    // 0x986f30(service_id, &handle)
    InitPostCallOutcome (*lock_service)(std::uint32_t service_id, void** handle) = nullptr;
    // 0x987720(handle)
    InitPostCallOutcome (*unlock_service)(void* handle) = nullptr;
    // 0x9882e0(handle, arg2), only when arg2 != nullptr
    InitPostCallOutcome (*get_name)(void* handle, const void* arg2) = nullptr;
    // 0x988400(handle, id, buffer, size); size != nullptr means query-only
    InitPostCallOutcome (*get_record)(void* handle, std::int32_t id, void* buffer,
                                      std::size_t* size) = nullptr;
    // 0x9556a0(dst, capacity, src)
    InitPostCallOutcome (*copy_name)(void* dst, std::size_t capacity, const char* src) = nullptr;
    // 0x9878d0(handle) / 0x987260(handle)
    InitPostCallOutcome (*release_a)(void* handle) = nullptr;
    InitPostCallOutcome (*release_b)(void* handle) = nullptr;
    // 0x947e30(status, line, format, arg); only on a negative 0x9882e0 result
    InitPostCallOutcome (*log_error)(std::int32_t status, std::int32_t line, const char* format,
                                     const void* arg) = nullptr;
};

struct Init9469f0Result {
    bool ok = false;
    const char* blocked = nullptr;
    std::int32_t status = -3;
    void* handle = nullptr;
    int get_record_calls = 0;
    int copy_calls = 0;
    bool default_name = false;
    std::uint32_t record_index = 0;
    std::array<char, kInit9469f0NameBytes> name0{};
    std::array<char, kInit9469f0NameBytes> name1{};
    InitPostCallLog call_log{};
};

struct Init946880Hooks {
    std::uint32_t* lock = nullptr;       // models 0x2dd0a78
    std::uint32_t service_id = 0;        // dword at [0x32950a8]
    bool log_flag = false;               // testb $0x20, [0x2dd0a30+1]
    std::uint32_t log_counter = 0;        // dword at 0x32951ec
    InitPostCallOutcome (*query_98a450)(std::uint32_t service_id,
                                        Init98a450Result* out) = nullptr;
    InitPostCallOutcome (*direct_944de0)(std::int32_t first, std::int32_t value, void* third,
                                         std::int32_t fourth) = nullptr;
    InitPostCallOutcome (*log_984700)(std::int32_t a, std::int32_t b, std::int32_t status,
                                      std::uint32_t message0, std::uint32_t message1,
                                      std::int32_t value) = nullptr;
    Init9469f0Hooks init_hooks{};
};

struct Init946880Result {
    bool ok = false;
    const char* blocked = nullptr;
    std::int32_t status = -3;
    bool lock_acquired = false;
    bool took_944de0 = false;
    bool took_9469f0 = false;
    int log_calls = 0;
    Init9469f0Result inner{};
    InitPostCallLog call_log{};
};

[[nodiscard]] Init9469f0Result run_init_9469f0_body(
    std::int32_t value, void* arg2, void* service_global, std::size_t service_size,
    const Init9469f0Hooks& hooks) noexcept;

[[nodiscard]] Init946880Result run_init_946880_body(
    std::int32_t value, void* service_global, std::size_t service_size,
    const Init946880Hooks& hooks) noexcept;

}  // namespace sotc
