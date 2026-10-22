// Private asset-only loader for the 275 input-table (0x551140) name-string pairs.
// Strings live in a gitignored fixture under local/fixtures/; never embed eboot or ship names.
// Consumed by registry fill 0x52a220 (hash → sorted insert A/B). Fail-visible when missing.
#pragma once

#include "input_table_551140.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace sotc {

inline constexpr char kInputtableNamesFixtureEnv[] = "SOTC_INPUTTABLE_NAMES_FIXTURE";
inline constexpr char kInputtableNamesFixtureMagic[4] = {'I', '2', '7', '5'};
inline constexpr std::uint32_t kInputtableNamesFixtureVersion = 1u;
// 275 × {u32 hash_a=fnv_cstr_52a990(name_a), u32 hash_b=fnv_cstr_a2ddc0(name_b)} LE
inline constexpr char kInputtableNamesHashpairSha256[] =
    "a1aa059e47b80f1d450d46f47d0885858d04323299c8e01b6a3bd2425f329258";

struct InputtableNamePair {
    std::vector<char> storage_a;  // NUL-terminated
    std::vector<char> storage_b;
    const char* name_a = nullptr;
    const char* name_b = nullptr;
    std::uint32_t hash_a = 0;  // fnv_cstr_52a990
    std::uint32_t hash_b = 0;  // fnv_cstr_a2ddc0
};

struct InputtableNamesFixture {
    bool loaded = false;
    std::array<InputtableNamePair, kInputTable551140Count> pairs{};
};

[[nodiscard]] std::string resolve_inputtable_names_fixture_path() noexcept;

// Load + verify each pair's hashes against fnv_cstr_52a990 / fnv_cstr_a2ddc0.
// On failure leaves `out` cleared and returns false (no throw).
[[nodiscard]] bool load_inputtable_names_fixture(const char* path,
                                                 InputtableNamesFixture& out) noexcept;

[[nodiscard]] bool load_inputtable_names_fixture_default(InputtableNamesFixture& out) noexcept;

// Wire fixture pointers into an already numeric-inited table (names only; does not hash).
// Returns false if fixture not loaded.
[[nodiscard]] bool apply_inputtable_names_fixture(InputTable551140& table,
                                                  const InputtableNamesFixture& fix) noexcept;

}  // namespace sotc
