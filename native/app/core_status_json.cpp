// Byte-exact JSON formatting for the SoCGameCore line protocol. See core_status_json.hpp.
#include "core_status_json.hpp"

#include <cstdio>
#include <utility>

namespace soc::status_json {
namespace {

void key(std::string& out, std::string_view name) {
    out += '"';
    out += name;
    out += "\":";
}

void string_member(std::string& out, std::string_view name, std::string_view value) {
    key(out, name);
    out += '"';
    out += escape(value);
    out += '"';
}

void bool_member(std::string& out, std::string_view name, bool value) {
    key(out, name);
    out += value ? "true" : "false";
}

void u32_member(std::string& out, std::string_view name, std::uint32_t value) {
    key(out, name);
    out += std::to_string(value);
}

void u64_member(std::string& out, std::string_view name, std::uint64_t value) {
    key(out, name);
    out += std::to_string(value);
}

void optional_u32_member(std::string& out, std::string_view name, const std::optional<std::uint32_t>& value) {
    key(out, name);
    if (value) {
        out += std::to_string(*value);
    } else {
        out += "null";
    }
}

void append_ctor(std::string& out, const CtorSnapshot& ctor) {
    out += "\"ctor\":{";
    bool_member(out, "linked", ctor.linked);
    out += ',';
    u32_member(out, "original_slots", ctor.original_slots);
    out += ',';
    u32_member(out, "slots_registered", ctor.slots_registered);
    out += ',';
    u32_member(out, "slots_with_callback", ctor.slots_with_callback);
    out += ',';
    u32_member(out, "completed", ctor.completed);
    out += ',';
    bool_member(out, "run_ok", ctor.run_ok);
    out += ',';
    bool_member(out, "callback_failed", ctor.callback_failed);
    out += ',';
    u32_member(out, "blocked_on_rva", ctor.blocked_on_rva);
    out += ',';
    optional_u32_member(out, "first_unrecovered_index", ctor.first_unrecovered_index);
    if (ctor.first_unrecovered_rva.has_value()) {
        out += ',';
        u32_member(out, "first_unrecovered_rva", *ctor.first_unrecovered_rva);
    }
    out += '}';
}

}  // namespace

std::string escape(std::string_view text) {
    std::string out;
    out.reserve(text.size());
    for (const char raw : text) {
        const unsigned char c = static_cast<unsigned char>(raw);
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    char buf[8] = {};
                    std::snprintf(buf, sizeof buf, "\\u%04x", static_cast<unsigned>(c));
                    out += buf;
                } else {
                    out += raw;
                }
                break;
        }
    }
    return out;
}

std::string format_status(const StatusSnapshot& snapshot) {
    std::string out;
    out.reserve(512);
    out += '{';
    string_member(out, "schema", kStatusSchema);
    out += ',';
    u32_member(out, "abi", snapshot.abi);
    out += ',';
    string_member(out, "state", snapshot.state);
    out += ',';
    u32_member(out, "last_error", snapshot.last_error);
    out += ',';
    string_member(out, "last_error_name", snapshot.last_error_name);
    out += ',';
    bool_member(out, "assets_validated", snapshot.assets_validated);
    out += ',';
    bool_member(out, "assets_ready", snapshot.assets_ready);
    out += ',';
    bool_member(out, "engine_implemented", snapshot.engine_implemented);
    out += ',';
    append_ctor(out, snapshot.ctor);
    out += '}';
    return out;
}

std::string format_coverage(const CoverageSnapshot& snapshot) {
    std::string out;
    out.reserve(768);
    out += '{';
    string_member(out, "schema", kCoverageSchema);
    out += ',';
    u32_member(out, "abi", snapshot.abi);
    out += ',';
    bool_member(out, "engine_implemented", snapshot.engine_implemented);
    out += ",\"subsystems\":[";
    for (std::size_t i = 0; i != snapshot.subsystems.size(); ++i) {
        if (i != 0) out += ',';
        const CoverageRowSnapshot& row = snapshot.subsystems[i];
        out += '{';
        string_member(out, "name", row.name);
        out += ',';
        string_member(out, "status", row.status);
        out += ',';
        string_member(out, "detail", row.detail);
        out += '}';
    }
    out += "]}";
    return out;
}

std::string format_validate(const ValidateSnapshot& snapshot) {
    std::string out;
    out.reserve(768);
    out += '{';
    string_member(out, "schema", kValidateSchema);
    out += ',';
    bool_member(out, "ok", snapshot.ok);
    out += ',';
    u32_member(out, "error", snapshot.error);
    out += ',';
    string_member(out, "error_name", snapshot.error_name);
    out += ',';
    bool_member(out, "assets_ready", snapshot.assets_ready);
    out += ',';
    bool_member(out, "engine_ready", snapshot.engine_ready);
    out += ',';
    bool_member(out, "start_would_succeed", snapshot.start_would_succeed);
    out += ",\"archives\":[";
    for (std::size_t i = 0; i != snapshot.archives.size(); ++i) {
        if (i != 0) out += ',';
        const ArchiveSnapshot& row = snapshot.archives[i];
        out += '{';
        string_member(out, "name", row.name);
        out += ',';
        string_member(out, "status", row.status);
        out += ',';
        u64_member(out, "size_bytes", row.size_bytes);
        out += '}';
    }
    out += ']';
    out += ',';
    string_member(out, "detail", snapshot.detail);
    out += '}';
    return out;
}

std::string format_start(const StartSnapshot& snapshot) {
    std::string out;
    out.reserve(160);
    out += '{';
    string_member(out, "schema", kStartSchema);
    out += ',';
    bool_member(out, "ok", snapshot.ok);
    out += ',';
    u32_member(out, "error", snapshot.error);
    out += ',';
    string_member(out, "error_name", snapshot.error_name);
    out += '}';
    return out;
}

std::string format_stop(const StopSnapshot& snapshot) {
    std::string out;
    out.reserve(160);
    out += '{';
    string_member(out, "schema", kStopSchema);
    out += ',';
    bool_member(out, "ok", snapshot.ok);
    out += ',';
    u32_member(out, "error", snapshot.error);
    out += ',';
    string_member(out, "error_name", snapshot.error_name);
    out += '}';
    return out;
}

std::string with_request_id(std::string_view line, std::int64_t id) {
    if (line.empty() || line.front() != '{') {
        return std::string(line);
    }
    std::string out;
    out.reserve(line.size() + 24);
    out += "{\"id\":";
    out += std::to_string(id);
    const std::string_view body = line.substr(1);
    if (body.empty()) {
        out += '}';
        return out;
    }
    if (body.front() != '}') {
        out += ',';
    }
    out += body;
    return out;
}

}  // namespace soc::status_json
