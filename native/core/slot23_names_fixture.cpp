#include "slot23_names_fixture.hpp"

#include "name_hash.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace sotc {
namespace {

void clear_fixture(Slot23NamesFixture& out) noexcept {
    out.loaded = false;
    for (auto& row : out.storage) {
        row.clear();
    }
    out.inputs = {};
}

bool path_readable(const char* path) noexcept {
    if (path == nullptr || path[0] == '\0') {
        return false;
    }
    std::ifstream in(path, std::ios::binary);
    return static_cast<bool>(in);
}

}  // namespace

std::string resolve_slot23_names_fixture_path() noexcept {
    if (const char* env = std::getenv(kSlot23NamesFixtureEnv); env != nullptr && env[0] != '\0') {
        if (path_readable(env)) {
            return std::string(env);
        }
        return {};
    }
    // Probes for common CTest / checkout working directories (repo root or local/build/native).
    static constexpr const char* kCandidates[] = {
        "local/fixtures/slot23_e71990_names.bin",
        "../local/fixtures/slot23_e71990_names.bin",
        "../../local/fixtures/slot23_e71990_names.bin",
        "../../../local/fixtures/slot23_e71990_names.bin",
    };
    for (const char* c : kCandidates) {
        if (path_readable(c)) {
            return std::string(c);
        }
    }
    return {};
}

bool load_slot23_names_fixture(const char* path, Slot23NamesFixture& out) noexcept {
    clear_fixture(out);
    if (path == nullptr || path[0] == '\0') {
        return false;
    }
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return false;
    }
    char magic[4]{};
    std::uint32_t version = 0;
    std::uint32_t count = 0;
    in.read(magic, 4);
    in.read(reinterpret_cast<char*>(&version), 4);
    in.read(reinterpret_cast<char*>(&count), 4);
    if (!in || std::memcmp(magic, kSlot23NamesFixtureMagic, 4) != 0 || version != kSlot23NamesFixtureVersion ||
        count != kLeafE71990Records) {
        return false;
    }
    for (std::size_t i = 0; i != kLeafE71990Records; ++i) {
        std::uint32_t len = 0;
        in.read(reinterpret_cast<char*>(&len), 4);
        if (!in || len > 4096u) {
            clear_fixture(out);
            return false;
        }
        out.storage[i].assign(static_cast<std::size_t>(len) + 1u, '\0');
        if (len != 0) {
            in.read(out.storage[i].data(), static_cast<std::streamsize>(len));
            if (!in) {
                clear_fixture(out);
                return false;
            }
        }
        out.storage[i][len] = '\0';
        out.inputs.name_address[i] = static_cast<std::uint64_t>(
            reinterpret_cast<std::uintptr_t>(out.storage[i].data()));
        if (i < 13) {
            const std::uint32_t got = name_hash(out.storage[i].data());
            if (got != kLeafE71990Hash[i]) {
                clear_fixture(out);
                return false;
            }
        }
    }
    // Trailing bytes are ignored; require we consumed at least the declared payload.
    if (!in) {
        clear_fixture(out);
        return false;
    }
    out.loaded = true;
    return true;
}

bool load_slot23_names_fixture_default(Slot23NamesFixture& out) noexcept {
    const std::string path = resolve_slot23_names_fixture_path();
    if (path.empty()) {
        clear_fixture(out);
        return false;
    }
    return load_slot23_names_fixture(path.c_str(), out);
}

}  // namespace sotc
