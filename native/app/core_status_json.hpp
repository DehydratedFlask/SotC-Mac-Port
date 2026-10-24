// Pure JSON formatting for the SoCGameCore line protocol (APP-01-SOCGAMECORE-EXE-01).
//
// A plain snapshot in, one JSON object out (no trailing newline). No I/O, no core ABI calls, no
// process state: the executable feeds measured facts in here and the unit test pins the bytes.
// `engine_implemented` is carried as a value and never assumed true by this layer.
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace soc::status_json {

// The original constructor table has 573 entries (indices 0..572). This is protocol identity,
// not a recoverable count: it is the denominator the native table is measured against.
inline constexpr std::uint32_t kOriginalConstructorSlots = 573u;

inline constexpr std::string_view kStatusSchema = "sotc.core.status.v1";
inline constexpr std::string_view kCoverageSchema = "sotc.core.coverage.v1";
inline constexpr std::string_view kValidateSchema = "sotc.core.validate.v1";
inline constexpr std::string_view kStartSchema = "sotc.core.start.v1";
inline constexpr std::string_view kStopSchema = "sotc.core.stop.v1";

struct CtorSnapshot {
    bool linked = false;
    std::uint32_t original_slots = kOriginalConstructorSlots;
    std::uint32_t slots_registered = 0;
    std::uint32_t slots_with_callback = 0;
    std::uint32_t completed = 0;
    bool run_ok = false;
    bool callback_failed = false;
    std::uint32_t blocked_on_rva = 0;
    // nullopt means "every original slot is represented" (or the table is not linked). The first
    // unrecovered index is always computed from the recovered table, never hard-coded by callers.
    std::optional<std::uint32_t> first_unrecovered_index{};
    // Optional protocol extension; emitted only when the build can measure it.
    std::optional<std::uint32_t> first_unrecovered_rva{};
};

struct StatusSnapshot {
    std::uint32_t abi = 0;
    std::string state;
    std::uint32_t last_error = 0;
    std::string last_error_name;
    bool assets_validated = false;
    bool assets_ready = false;
    bool engine_implemented = false;
    CtorSnapshot ctor{};
};

struct CoverageRowSnapshot {
    std::string name;
    std::string status;  // not_implemented | partial | implemented | not_linked
    std::string detail;
};

struct CoverageSnapshot {
    std::uint32_t abi = 0;
    bool engine_implemented = false;
    std::vector<CoverageRowSnapshot> subsystems;
};

struct ArchiveSnapshot {
    std::string name;
    std::string status;  // ok | missing | unreadable | bad_magic | unknown
    std::uint64_t size_bytes = 0;
};

struct ValidateSnapshot {
    bool ok = false;
    std::uint32_t error = 0;
    std::string error_name;
    bool assets_ready = false;
    bool engine_ready = false;
    bool start_would_succeed = false;
    std::vector<ArchiveSnapshot> archives;
    std::string detail;
};

struct StartSnapshot {
    bool ok = false;
    std::uint32_t error = 0;
    std::string error_name;
};

struct StopSnapshot {
    bool ok = false;
    std::uint32_t error = 0;
    std::string error_name;
};

// RFC 8259 string contents: `"` and `\` escaped, C0 controls as \b \f \n \r \t or \u00XX.
// Bytes >= 0x20 are copied through (the ABI text is UTF-8/ASCII in practice).
std::string escape(std::string_view text);

std::string format_status(const StatusSnapshot& snapshot);
std::string format_coverage(const CoverageSnapshot& snapshot);
std::string format_validate(const ValidateSnapshot& snapshot);
std::string format_start(const StartSnapshot& snapshot);
std::string format_stop(const StopSnapshot& snapshot);

// Insert `"id":<id>` as the first member of a JSON object line (serve protocol). A line that does
// not begin with '{' is returned unchanged; this layer never invents a request id.
std::string with_request_id(std::string_view line, std::int64_t id);

}  // namespace soc::status_json
