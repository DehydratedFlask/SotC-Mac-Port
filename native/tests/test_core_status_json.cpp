// Byte-level tests for the SoCGameCore JSON formatter (native/app/core_status_json.*).
// These pin exact protocol lines so the executable's output cannot drift silently.
#include <cstdio>
#include <cstdlib>
#include <string>

#include "core_status_json.hpp"

#define CHECK(condition)                                                                  \
    do {                                                                                  \
        if (!(condition)) {                                                               \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #condition);       \
            std::exit(1);                                                                 \
        }                                                                                 \
    } while (0)

namespace {

void check_equal(const std::string& actual, const std::string& expected, int line) {
    if (actual != expected) {
        std::fprintf(stderr, "FAIL %s:%d\n  expected: %s\n  actual:   %s\n", __FILE__, line, expected.c_str(),
                     actual.c_str());
        std::exit(1);
    }
}

#define CHECK_JSON(actual, expected) check_equal((actual), (expected), __LINE__)

void test_escape() {
    using soc::status_json::escape;
    CHECK(escape("") == "");
    CHECK(escape("plain ascii") == "plain ascii");
    CHECK(escape("quote\"") == "quote\\\"");
    CHECK(escape("back\\slash") == "back\\\\slash");
    CHECK(escape("\b\f\n\r\t") == "\\b\\f\\n\\r\\t");
    CHECK(escape(std::string("\x01\x1f", 2)) == "\\u0001\\u001f");
    CHECK(escape("caf\xc3\xa9") == "caf\xc3\xa9");  // UTF-8 bytes pass through
    CHECK(escape(std::string("a\0b", 3)) == "a\\u0000b");  // embedded NUL is still data
}

void test_status_full() {
    soc::status_json::StatusSnapshot status;
    status.abi = 65536;
    status.state = "failed";
    status.last_error = 4;
    status.last_error_name = "SOTC_ERR_ENGINE_NOT_IMPLEMENTED";
    status.assets_validated = true;
    status.assets_ready = false;
    status.engine_implemented = false;
    status.ctor.linked = true;
    status.ctor.original_slots = 573;
    status.ctor.slots_registered = 156;
    status.ctor.slots_with_callback = 156;
    status.ctor.completed = 23;
    status.ctor.run_ok = false;
    status.ctor.callback_failed = true;
    status.ctor.blocked_on_rva = 0xe71990;
    status.ctor.first_unrecovered_index = 152;

    CHECK_JSON(soc::status_json::format_status(status),
               R"({"schema":"sotc.core.status.v1","abi":65536,"state":"failed","last_error":4,"last_error_name":"SOTC_ERR_ENGINE_NOT_IMPLEMENTED","assets_validated":true,"assets_ready":false,"engine_implemented":false,"ctor":{"linked":true,"original_slots":573,"slots_registered":156,"slots_with_callback":156,"completed":23,"run_ok":false,"callback_failed":true,"blocked_on_rva":15145360,"first_unrecovered_index":152}})");
}

void test_status_null_index_and_optional_rva() {
    soc::status_json::StatusSnapshot status;
    status.abi = 65536;
    status.state = "idle";
    status.last_error_name = "SOTC_ERR_\"QUOTE\"\\SLASH";
    status.ctor.linked = true;
    status.ctor.original_slots = 573;
    status.ctor.slots_registered = 4;
    status.ctor.slots_with_callback = 4;
    status.ctor.completed = 4;
    status.ctor.run_ok = true;
    status.ctor.first_unrecovered_index.reset();
    status.ctor.first_unrecovered_rva = 4660;

    CHECK_JSON(soc::status_json::format_status(status),
               R"({"schema":"sotc.core.status.v1","abi":65536,"state":"idle","last_error":0,"last_error_name":"SOTC_ERR_\"QUOTE\"\\SLASH","assets_validated":false,"assets_ready":false,"engine_implemented":false,"ctor":{"linked":true,"original_slots":573,"slots_registered":4,"slots_with_callback":4,"completed":4,"run_ok":true,"callback_failed":false,"blocked_on_rva":0,"first_unrecovered_index":null,"first_unrecovered_rva":4660}})");

    // Index 0 is a real answer, not "absent"; it must serialize as 0.
    status.ctor.first_unrecovered_index = 0;
    status.ctor.first_unrecovered_rva.reset();
    const std::string line = soc::status_json::format_status(status);
    CHECK(line.find(R"("first_unrecovered_index":0})") != std::string::npos);
    CHECK(line.find("first_unrecovered_rva") == std::string::npos);
}

void test_coverage() {
    soc::status_json::CoverageSnapshot coverage;
    coverage.abi = 65536;
    coverage.engine_implemented = false;
    coverage.subsystems = {
        {"startup_constructors", "partial", "N of 573; \"quoted\" \\ path\ttab"},
        {"engine_boot", "not_implemented", "no engine"},
    };

    CHECK_JSON(soc::status_json::format_coverage(coverage),
               R"({"schema":"sotc.core.coverage.v1","abi":65536,"engine_implemented":false,"subsystems":[{"name":"startup_constructors","status":"partial","detail":"N of 573; \"quoted\" \\ path\ttab"},{"name":"engine_boot","status":"not_implemented","detail":"no engine"}]})");

    // The empty list is still a valid object (no trailing comma).
    coverage.subsystems.clear();
    CHECK_JSON(soc::status_json::format_coverage(coverage),
               R"({"schema":"sotc.core.coverage.v1","abi":65536,"engine_implemented":false,"subsystems":[]})");
}

void test_validate() {
    soc::status_json::ValidateSnapshot validate;
    validate.ok = false;
    validate.error = 3;
    validate.error_name = "SOTC_ERR_ASSETS_NOT_READY";
    validate.assets_ready = false;
    validate.engine_ready = false;
    validate.start_would_succeed = false;
    validate.archives = {
        {"cachedFiles.psarc", "missing", 0},
        {"pak0.psarc", "bad_magic", 18446744073709551615ull},
    };
    validate.detail = "line one\nline two\t\"x\" \\";

    CHECK_JSON(soc::status_json::format_validate(validate),
               R"({"schema":"sotc.core.validate.v1","ok":false,"error":3,"error_name":"SOTC_ERR_ASSETS_NOT_READY","assets_ready":false,"engine_ready":false,"start_would_succeed":false,"archives":[{"name":"cachedFiles.psarc","status":"missing","size_bytes":0},{"name":"pak0.psarc","status":"bad_magic","size_bytes":18446744073709551615}],"detail":"line one\nline two\t\"x\" \\"})");

    // Empty archive list and C0 control escaping.
    validate.archives.clear();
    validate.detail = std::string("bell:") + '\x07';
    CHECK_JSON(soc::status_json::format_validate(validate),
               R"({"schema":"sotc.core.validate.v1","ok":false,"error":3,"error_name":"SOTC_ERR_ASSETS_NOT_READY","assets_ready":false,"engine_ready":false,"start_would_succeed":false,"archives":[],"detail":"bell:\u0007"})");
}

void test_start_and_stop() {
    soc::status_json::StartSnapshot start;
    start.ok = false;
    start.error = 4;
    start.error_name = "SOTC_ERR_ENGINE_NOT_IMPLEMENTED";
    CHECK_JSON(soc::status_json::format_start(start),
               R"({"schema":"sotc.core.start.v1","ok":false,"error":4,"error_name":"SOTC_ERR_ENGINE_NOT_IMPLEMENTED"})");

    soc::status_json::StopSnapshot stop;
    stop.ok = true;
    stop.error = 0;
    stop.error_name = "SOTC_OK";
    CHECK_JSON(soc::status_json::format_stop(stop),
               R"({"schema":"sotc.core.stop.v1","ok":true,"error":0,"error_name":"SOTC_OK"})");
}

void test_request_id_prefix() {
    using soc::status_json::with_request_id;
    CHECK(with_request_id(R"({"ok":true})", 7) == R"({"id":7,"ok":true})");
    CHECK(with_request_id("{}", 0) == R"({"id":0})");
    CHECK(with_request_id("{", -3) == R"({"id":-3})");
    CHECK(with_request_id("", 1) == "");
    CHECK(with_request_id("not json", 1) == "not json");
    CHECK(with_request_id(R"({"a":{"b":1}})", 2) == R"({"id":2,"a":{"b":1}})");
}

}  // namespace

int main() {
    test_escape();
    test_status_full();
    test_status_null_index_and_optional_rva();
    test_coverage();
    test_validate();
    test_start_and_stop();
    test_request_id_prefix();
    std::puts("core_status_json: all protocol lines pinned");
    return 0;
}
