#include "inputtable_names_fixture.hpp"

#include "fnv_cstr_161fb0.hpp"
#include "fnv_path_cstr_162020.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace sotc {
namespace {

void clear_fixture(InputtableNamesFixture& out) noexcept {
    out.loaded = false;
    for (auto& p : out.pairs) {
        p = {};
    }
}

bool path_readable(const char* path) noexcept {
    if (path == nullptr || path[0] == '\0') {
        return false;
    }
    std::ifstream in(path, std::ios::binary);
    return static_cast<bool>(in);
}

bool read_u32(std::ifstream& in, std::uint32_t& v) noexcept {
    in.read(reinterpret_cast<char*>(&v), 4);
    return static_cast<bool>(in);
}

bool read_bytes_nul(std::ifstream& in, std::uint32_t len, std::vector<char>& storage,
                    const char*& ptr) noexcept {
    if (len > 4096u) {
        return false;
    }
    storage.assign(static_cast<std::size_t>(len) + 1u, '\0');
    if (len != 0) {
        in.read(storage.data(), static_cast<std::streamsize>(len));
        if (!in) {
            return false;
        }
    }
    storage[len] = '\0';
    ptr = storage.data();
    return true;
}

}  // namespace

std::string resolve_inputtable_names_fixture_path() noexcept {
    if (const char* env = std::getenv(kInputtableNamesFixtureEnv); env != nullptr && env[0] != '\0') {
        if (path_readable(env)) {
            return std::string(env);
        }
        return {};
    }
    static constexpr const char* kCandidates[] = {
        "local/fixtures/inputtable_551140_names.bin",
        "../local/fixtures/inputtable_551140_names.bin",
        "../../local/fixtures/inputtable_551140_names.bin",
        "../../../local/fixtures/inputtable_551140_names.bin",
    };
    for (const char* c : kCandidates) {
        if (path_readable(c)) {
            return std::string(c);
        }
    }
    return {};
}

bool load_inputtable_names_fixture(const char* path, InputtableNamesFixture& out) noexcept {
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
    if (!read_u32(in, version) || !read_u32(in, count)) {
        return false;
    }
    if (std::memcmp(magic, kInputtableNamesFixtureMagic, 4) != 0 ||
        version != kInputtableNamesFixtureVersion || count != kInputTable551140Count) {
        return false;
    }
    for (std::size_t i = 0; i != kInputTable551140Count; ++i) {
        std::uint32_t len_a = 0;
        std::uint32_t len_b = 0;
        if (!read_u32(in, len_a) ||
            !read_bytes_nul(in, len_a, out.pairs[i].storage_a, out.pairs[i].name_a) ||
            !read_u32(in, len_b) ||
            !read_bytes_nul(in, len_b, out.pairs[i].storage_b, out.pairs[i].name_b)) {
            clear_fixture(out);
            return false;
        }
        const std::uint32_t ha = fnv_cstr_52a990(out.pairs[i].name_a);
        const std::uint32_t hb = fnv_cstr_a2ddc0(out.pairs[i].name_b);
        out.pairs[i].hash_a = ha;
        out.pairs[i].hash_b = hb;
    }
    if (!in) {
        clear_fixture(out);
        return false;
    }
    out.loaded = true;
    return true;
}

bool load_inputtable_names_fixture_default(InputtableNamesFixture& out) noexcept {
    const std::string path = resolve_inputtable_names_fixture_path();
    if (path.empty()) {
        clear_fixture(out);
        return false;
    }
    return load_inputtable_names_fixture(path.c_str(), out);
}

bool apply_inputtable_names_fixture(InputTable551140& table,
                                    const InputtableNamesFixture& fix) noexcept {
    if (!fix.loaded) {
        return false;
    }
    for (std::size_t i = 0; i != kInputTable551140Count; ++i) {
        table[i].name_a = fix.pairs[i].name_a;
        table[i].name_b = fix.pairs[i].name_b;
    }
    return true;
}

}  // namespace sotc
