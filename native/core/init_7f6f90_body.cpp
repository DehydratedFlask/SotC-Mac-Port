#include "init_7f6f90_body.hpp"

#include <array>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <new>
#include <strings.h>

namespace sotc {
namespace {

constexpr char kStaffroll7f6f90PathFormat[] = "/app0/CP3Shadow/%s";
constexpr char kStaffroll7f6f90OpenMode[] = "rb";
constexpr char kStaffroll7f6f90FallbackFormat[] = "%s";
constexpr std::size_t kMaxFallbackTextBytes = 1u << 24;

constexpr std::array<const char*, kStaffroll7f6f90TokenCount> kStaffroll7f6f90Tokens = {
    "CENTER",
    "CENTER_BR",
    "RIGHT",
    "RIGHT_BR",
    "LEFT",
    "LEFT_BR",
    "BR",
    "SPACE",
    "SPACE_SCREEN",
    "SPACE_HALFSCREEN",
    "LAYOUTAREA",
    "FADEAREA",
    "LAYOUTBOX",
    "LINEHEIGHT",
    "FONTCOLOR",
    "ROLL_TIME",
    "FADE_TIME",
    "WAIT_BEFORE_FADE",
    "SHADOWFONT_OFFSET",
    "CENTER_FULL_SCREEN",
    "MAIN_HEADING",
    "SUB_HEADING",
    "SUB_HEADING_2",
    "COLUMN_HEADING_1",
    "COLUMN_HEADING_2",
    "COLUMN_HEADING_3",
    "COLUMN_HEADING_4",
    "COLUMN_1",
    "COLUMN_2",
    "COLUMN_3",
    "COLUMN_4",
    "COLUMN_END",
    "COPYRIGHT",
    "SPECIAL_THANKS",
    "BP_COMMAND_FORMAT",
    "STYLE",
    "TARGET",
};
static_assert(kStaffroll7f6f90Tokens.size() == kStaffroll7f6f90TokenCount);

InitPostCallOutcome executed_outcome(const char* detail) noexcept {
    InitPostCallOutcome o{};
    o.executed = true;
    std::snprintf(o.detail, sizeof o.detail, "%s", detail);
    return o;
}

void add_call(InitPostCallLog& log, std::uint64_t callee, const std::uint64_t* args,
              std::size_t arg_count, const InitPostCallOutcome& outcome) noexcept {
    if (log.count >= kInitPostCallMax) return;
    InitPostCallRecord& record = log.records[log.count++];
    record.callee_rva = callee;
    record.caller = "0x7f6f90";
    record.executed = outcome.executed;
    record.has_return = outcome.has_value;
    record.return_value = outcome.value;
    record.blocker = outcome.executed ? nullptr : outcome.blocker;
    record.arg_count = static_cast<std::uint8_t>(arg_count > kInitPostCallArgMax
                                                     ? kInitPostCallArgMax
                                                     : arg_count);
    for (std::size_t i = 0; i < record.arg_count; ++i) record.args[i] = args[i];
    std::memcpy(record.detail, outcome.detail, sizeof record.detail);
    record.detail[sizeof record.detail - 1] = '\0';
}

std::unique_ptr<char[]> allocate_text(std::size_t size) noexcept {
    if (size == static_cast<std::size_t>(-1)) return nullptr;
    return std::unique_ptr<char[]>(new (std::nothrow) char[size + 1]);
}

}  // namespace

Staffroll7f6f90Result run_staffroll_7f6f90_body(
    int mode, const char* string, const Staffroll7f6f90Hooks& hooks) noexcept {
    Staffroll7f6f90Result out{};
    out.mode = mode;

    if (mode < 0 || mode > 1) {
        out.blocked = "mode_out_of_range";
        return out;
    }
    if (string == nullptr) {
        out.blocked = "string_missing";
        return out;
    }
    if (hooks.counts == nullptr || hooks.entries == nullptr ||
        hooks.entry_capacity < kStaffroll7f6f90MaxEntries) {
        out.blocked = "state_missing";
        return out;
    }

    char path[kStaffroll7f6f90PathBytes]{};
    if (std::snprintf(path, sizeof path, kStaffroll7f6f90PathFormat, string) < 0) {
        out.blocked = "path_format_failed";
        return out;
    }

    std::FILE* file = nullptr;
    InitPostCallOutcome fopen_out{};
    if (hooks.fopen_f902a0 != nullptr) {
        fopen_out = hooks.fopen_f902a0(path, kStaffroll7f6f90OpenMode, &file);
    } else {
        file = std::fopen(path, kStaffroll7f6f90OpenMode);
        fopen_out = executed_outcome("libc fopen");
        fopen_out.has_value = true;
        fopen_out.value = file != nullptr ? 1u : 0u;
    }
    const std::uint64_t fopen_args[1] = {reinterpret_cast<std::uint64_t>(path)};
    add_call(out.call_log, 0xf902a0, fopen_args, 1, fopen_out);
    if (!fopen_out.executed) {
        out.blocked = "fopen_contract_refused";
        return out;
    }

    std::size_t text_bytes = 0;
    std::unique_ptr<char[]> text;
    if (file != nullptr) {
        out.used_fopen = true;
        if (std::fseek(file, 0, SEEK_END) != 0) {
            std::fclose(file);
            out.blocked = "fseek_end_failed";
            return out;
        }
        const long end = std::ftell(file);
        if (end < 0) {
            std::fclose(file);
            out.blocked = "ftell_failed";
            return out;
        }
        text_bytes = static_cast<std::size_t>(end);
        if (std::fseek(file, 0, SEEK_SET) != 0) {
            std::fclose(file);
            out.blocked = "fseek_set_failed";
            return out;
        }
        text = allocate_text(text_bytes);
        if (!text) {
            std::fclose(file);
            out.blocked = "alloc_failed";
            return out;
        }
        const std::size_t read = std::fread(text.get(), 1, text_bytes, file);
        text[read] = '\0';
        text_bytes = read;
        std::fclose(file);
    } else {
        if (hooks.load_56cdb0 == nullptr || hooks.size_56ceb0 == nullptr ||
            hooks.copy_56cec0 == nullptr || hooks.close_56ce60 == nullptr) {
            out.blocked = "fallback_contract_missing";
            return out;
        }
        char fallback_path[kStaffroll7f6f90PathBytes]{};
        if (std::snprintf(fallback_path, sizeof fallback_path, kStaffroll7f6f90FallbackFormat,
                          string) < 0) {
            out.blocked = "fallback_format_failed";
            return out;
        }
        int slot = -1;
        const std::uint64_t load_args[1] = {reinterpret_cast<std::uint64_t>(fallback_path)};
        const InitPostCallOutcome load =
            hooks.load_56cdb0(fallback_path, -1, &slot);
        add_call(out.call_log, 0x56cdb0, load_args, 1, load);
        if (!load.executed) {
            out.blocked = "load_56cdb0_refused";
            return out;
        }

        std::size_t size = 0;
        const InitPostCallOutcome size_out = hooks.size_56ceb0(slot, &size);
        const std::uint64_t size_args[1] = {static_cast<std::uint64_t>(slot)};
        add_call(out.call_log, 0x56ceb0, size_args, 1, size_out);
        if (!size_out.executed) {
            out.blocked = "size_56ceb0_refused";
            return out;
        }
        if (size > kMaxFallbackTextBytes) {
            out.blocked = "fallback_size_too_large";
            return out;
        }

        text = allocate_text(size);
        if (!text) {
            out.blocked = "alloc_failed";
            return out;
        }
        const std::uint64_t copy_args[2] = {static_cast<std::uint64_t>(slot),
                                            static_cast<std::uint64_t>(size)};
        const InitPostCallOutcome copy =
            hooks.copy_56cec0(slot, text.get(), size);
        add_call(out.call_log, 0x56cec0, copy_args, 2, copy);
        if (!copy.executed) {
            out.blocked = "copy_56cec0_refused";
            return out;
        }
        text[size] = '\0';
        text_bytes = size;

        const InitPostCallOutcome close = hooks.close_56ce60(slot);
        const std::uint64_t close_args[1] = {static_cast<std::uint64_t>(slot)};
        add_call(out.call_log, 0x56ce60, close_args, 1, close);
        if (!close.executed) {
            out.blocked = "close_56ce60_refused";
            return out;
        }
    }

    out.text = std::move(text);
    out.text_bytes = text_bytes;
    for (char* p = out.text.get(); p < out.text.get() + text_bytes; ++p) {
        if (*p == '\r' || *p == '\n') *p = '\0';
    }

    char* begin = out.text.get();
    char* end = begin + text_bytes;
    if (text_bytes >= 3 && static_cast<unsigned char>(begin[0]) == 0xef &&
        static_cast<unsigned char>(begin[1]) == 0xbb &&
        static_cast<unsigned char>(begin[2]) == 0xbf) {
        begin += 3;
    }

    while (begin < end) {
        const unsigned char byte = static_cast<unsigned char>(*begin);
        std::size_t advance = 1;
        if (byte != 0) {
            bool skip = std::isspace(byte) != 0 || std::iscntrl(byte) != 0;
            if (hooks.is_skip_f90220 != nullptr) {
                bool hook_skip = false;
                const InitPostCallOutcome ctype = hooks.is_skip_f90220(byte, &hook_skip);
                if (!ctype.executed) {
                    out.blocked = "ctype_contract_refused";
                    return out;
                }
                skip = hook_skip;
            }
            if (!skip) {
                const std::size_t remainder = std::strlen(begin);
                advance = remainder + 1;
                if (remainder >= 2 && begin[0] == '/' && begin[1] == '/') {
                    begin += advance;
                    continue;
                }
                char* comma = std::strchr(begin, ',');
                if (comma == nullptr) {
                    out.blocked = "missing_comma";
                    return out;
                }
                *comma = '\0';
                int index = -1;
                for (std::size_t i = 0; i < kStaffroll7f6f90Tokens.size(); ++i) {
                    if (::strcasecmp(begin, kStaffroll7f6f90Tokens[i]) == 0) {
                        index = static_cast<int>(i);
                        break;
                    }
                }
                const int count = hooks.counts[mode];
                if (count < 0 || static_cast<std::size_t>(count) >= hooks.entry_capacity) {
                    out.blocked = "entry_capacity";
                    return out;
                }
                Staffroll7f6f90Entry& entry =
                    hooks.entries[static_cast<std::size_t>(mode) * hooks.entry_capacity +
                                  static_cast<std::size_t>(count)];
                entry.index = index;
                entry.reserved = 0;
                entry.rest = comma + 1;
                hooks.counts[mode] = count + 1;
            }
        }
        begin += advance;
    }

    out.entries = hooks.counts[mode];
    out.ok = true;
    return out;
}

}  // namespace sotc
