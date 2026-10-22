// Private asset-only loader for slot 23 / RVA 0xe71990 name strings.
// Strings live in a gitignored fixture under local/fixtures/; never embed eboot or ship names.
#pragma once

#include "leaf_e71990.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace sotc {

inline constexpr char kSlot23NamesFixtureEnv[] = "SOTC_SLOT23_NAMES_FIXTURE";
inline constexpr char kSlot23NamesFixtureMagic[4] = {'S', '2', '3', 'N'};
inline constexpr std::uint32_t kSlot23NamesFixtureVersion = 1u;

struct Slot23NamesFixture {
    bool loaded = false;
    // Owned NUL-terminated copies; pointers in `name_address` alias these buffers.
    std::array<std::vector<char>, kLeafE71990Records> storage{};
    LeafE71990Inputs inputs{};
};

// Resolve fixture path: env SOTC_SLOT23_NAMES_FIXTURE, else a few repo-relative probes.
// Returns empty string if none exist.
[[nodiscard]] std::string resolve_slot23_names_fixture_path() noexcept;

// Load + hash-check records 0..12 against kLeafE71990Hash. Record 13 is the sentinel (hash 0).
// On failure leaves `out` cleared and returns false (no throw).
[[nodiscard]] bool load_slot23_names_fixture(const char* path, Slot23NamesFixture& out) noexcept;

// Convenience: resolve path, load, verify. False if missing/corrupt/hash mismatch.
[[nodiscard]] bool load_slot23_names_fixture_default(Slot23NamesFixture& out) noexcept;

}  // namespace sotc
