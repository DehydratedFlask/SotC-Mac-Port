#include "config_bool_lookup_1e4c0.hpp"

#include <cstdlib>
#include <cstring>
#include <strings.h>

namespace sotc {
namespace {

[[nodiscard]] const char* entry_value_or_null(const ConfigBoolEntry16* entry) noexcept {
    return (entry != nullptr) ? entry->value : nullptr;
}

[[nodiscard]] int bool_to_int01(bool v) noexcept { return v ? 1 : 0; }

[[nodiscard]] int parse_value_to_int01(const char* text) noexcept {
    return bool_to_int01(parse_config_bool_cstr(text));
}

}  // namespace

bool parse_config_bool_cstr(const char* text) noexcept {
    if (text == nullptr) {
        return false;
    }
    if (strcasecmp(text, "true") == 0 || strcasecmp(text, "yes") == 0) {
        return true;
    }
    if (strcasecmp(text, "false") == 0 || strcasecmp(text, "no") == 0) {
        return false;
    }
    char* end = nullptr;
    const unsigned long v = strtoul(text, &end, 10);
    if (end == text) {
        return false;
    }
    return v != 0;
}

int parse_config_bool_to_int01(const char* text) noexcept {
    return parse_value_to_int01(text);
}

int config_bool_lookup_object(void* obj, const char* key, int index, int default_val) noexcept {
    const int fallback = bool_to_int01(default_val != 0);
    if (obj == nullptr) {
        return fallback;
    }
    auto* base = static_cast<std::uint8_t*>(obj);
    ConfigBoolEntry16* begin = nullptr;
    ConfigBoolEntry16* end = nullptr;
    std::memcpy(&begin, base + kConfigBoolTableBeginOffset, sizeof(begin));
    std::memcpy(&end, base + kConfigBoolTableEndOffset, sizeof(end));
    if (begin == nullptr || end == nullptr || end < begin) {
        return fallback;
    }

    if (key != nullptr) {
        for (ConfigBoolEntry16* it = begin; it < end; ++it) {
            if (it->name != nullptr && strcasecmp(it->name, key) == 0) {
                return parse_value_to_int01(it->value);
            }
        }
    }

    const std::size_t count = static_cast<std::size_t>(end - begin);
    if (index < 0 || static_cast<std::size_t>(index) >= count) {
        return fallback;
    }
    const char* val = entry_value_or_null(begin + index);
    if (val == nullptr) {
        return fallback;
    }
    return parse_value_to_int01(val);
}

bool config_bool_lookup_args_span(const SndxArgsSpan& args, std::size_t index,
                                  bool default_value) noexcept {
    if (args.begin == nullptr || args.end == nullptr || args.end < args.begin) {
        return default_value;
    }
    const auto* begin = static_cast<const ConfigBoolEntry16*>(args.begin);
    const std::size_t count = args.record_count();
    if (index >= count) {
        return default_value;
    }
    const char* val = entry_value_or_null(begin + index);
    if (val == nullptr) {
        return default_value;
    }
    return parse_config_bool_cstr(val);
}

int config_bool_lookup_init_continue_b(void* config_object) noexcept {
    return config_bool_lookup_object(config_object, nullptr, 0, 1);
}

bool sndx_get_bool_arg_native(void* /*host*/, const SndxArgsSpan& args, std::size_t index,
                              bool default_value) noexcept {
    return config_bool_lookup_args_span(args, index, default_value);
}

SndxGetBoolArgHooks default_native_sndx_get_bool_arg_hooks() noexcept {
    SndxGetBoolArgHooks hooks{};
    hooks.lookup = &sndx_get_bool_arg_native;
    hooks.host = nullptr;
    return hooks;
}

}  // namespace sotc
