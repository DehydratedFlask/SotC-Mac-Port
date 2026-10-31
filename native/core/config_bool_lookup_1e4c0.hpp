#pragma once

// CPU-01-INIT-1E4C0-SHARED-LOOKUP-01: shared model for app-main 0x1e4c0 boolean config lookup.
// Two entry shapes (E524 + E1037):
//   * Config object table at obj+0x30..+0x38 (Init continue-B `call 0x1e4c0`)
//   * Command args vector as 16-byte {name_ptr, value_ptr} records (slot-306 sndx handlers)
// Value parsing only: strcasecmp "true"/"yes"/"false"/"no", else strtoul base 10 != 0 (E520).

#include <cstddef>
#include <cstdint>

#include "core/sndx_command_handlers.hpp"

namespace sotc {

inline constexpr std::size_t kConfigBoolLookup1e4c0BodySize = 0x140;
inline constexpr char kConfigBoolLookup1e4c0BodySha256[] =
    "1188f8517326e086907a871b96482be50005651deed17f9b55fb22c51c4afbc1";

inline constexpr std::size_t kConfigBoolEntrySize = 16;
inline constexpr std::size_t kConfigBoolTableBeginOffset = 0x30;
inline constexpr std::size_t kConfigBoolTableEndOffset = 0x38;

struct ConfigBoolEntry16 {
    const char* name = nullptr;
    const char* value = nullptr;
};

[[nodiscard]] bool parse_config_bool_cstr(const char* text) noexcept;
[[nodiscard]] int parse_config_bool_to_int01(const char* text) noexcept;

// E524: int config_bool_lookup(void* obj, const char* key, int index, int default_val)
[[nodiscard]] int config_bool_lookup_object(void* obj, const char* key, int index,
                                            int default_val) noexcept;

[[nodiscard]] bool config_bool_lookup_args_span(const SndxArgsSpan& args, std::size_t index,
                                                bool default_value) noexcept;

// Init call site at 0x53278e: key NULL, index 0, default 1.
[[nodiscard]] int config_bool_lookup_init_continue_b(void* config_object) noexcept;

bool sndx_get_bool_arg_native(void* /*host*/, const SndxArgsSpan& args, std::size_t index,
                              bool default_value) noexcept;

[[nodiscard]] SndxGetBoolArgHooks default_native_sndx_get_bool_arg_hooks() noexcept;

}  // namespace sotc
