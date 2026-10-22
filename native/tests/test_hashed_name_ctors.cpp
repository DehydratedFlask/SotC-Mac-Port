// Ctors 0x669450 / 0x842450 / 0x7dace0 / 0x6dde20: each stores caller-supplied name pointers and the
// 0x52a990 hash of the string into 16-byte records (name +0x00, hash +0x08, +0x0c never written).
// Independent oracle: the hash is recomputed with a separately written FNV (sign-extended bytes);
// the unwritten bytes are checked with a poisoned buffer; the null-name edge stores pointer 0, hash 0;
// calls are idempotent and overwrite stale cells. --dump <ctor> <s:hex>... feeds
// tools/lift/check_f<rva>.py (binary-vs-native tie).
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "core/hashed_name_ctors.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

namespace {
using sotc::NameHashCell;
constexpr std::uint32_t kPoison = 0xAAAAAAAAu;

std::uint32_t oracle(const char* s) {
    if (!s) return 0;
    std::uint64_t h = 0x811c9dc5ull;
    for (const unsigned char* p = reinterpret_cast<const unsigned char*>(s); *p; ++p) {
        const std::uint64_t ext = *p >= 0x80 ? (0xffffff00ull | *p) : *p;
        h = ((h ^ ext) * 0x01000193ull) & 0xffffffffull;
    }
    return static_cast<std::uint32_t>(h);
}

template <std::size_t N, class F>
void exercise(F ctor, const std::array<const char*, N>& names) {
    std::array<NameHashCell, N> cells;
    for (auto& c : cells) c = NameHashCell{reinterpret_cast<const char*>(0x1111), 0x2222u, kPoison};
    ctor(cells, names);
    for (std::size_t k = 0; k != N; ++k) {
        CHECK(cells[k].name == names[k]);
        CHECK(cells[k].hash == oracle(names[k]));
        CHECK(cells[k].unwritten == kPoison);          // +0x0c never written
    }
    const auto again = cells;
    ctor(cells, names);
    CHECK(std::memcmp(&again, &cells, sizeof cells) == 0);   // idempotent
}

template <std::size_t N, class F>
void run_all(F ctor) {
    static const char* const pool[] = {"alpha", "", "InSaveVolume", "\x80\xff tail", "a long name with spaces 12345"};
    for (unsigned seed = 0; seed != 5; ++seed) {
        std::array<const char*, N> names{};
        for (std::size_t k = 0; k != N; ++k) names[k] = pool[(seed + k * 2) % 5];
        exercise<N>(ctor, names);
    }
    std::array<const char*, N> with_null{};          // null names: pointer 0 and hash 0 stored
    exercise<N>(ctor, with_null);
}

template <std::size_t N, class F>
void dump(F ctor, int argc, char** argv, int first) {
    CHECK(argc - first == static_cast<int>(N));
    std::vector<std::string> store;
    std::array<const char*, N> names{};
    store.reserve(N);
    for (std::size_t k = 0; k != N; ++k) {
        const std::string a = argv[first + k];
        CHECK(a.rfind("s:", 0) == 0 && a.size() % 2 == 0);
        std::string s;
        for (std::size_t i = 2; i < a.size(); i += 2) s.push_back(static_cast<char>(std::stoi(a.substr(i, 2), nullptr, 16)));
        store.push_back(s);
        names[k] = store.back().c_str();
    }
    std::array<NameHashCell, N> cells;
    for (auto& c : cells) c = NameHashCell{nullptr, kPoison, kPoison};
    ctor(cells, names);
    std::printf("{\"count\":%zu,\"stride\":%zu,\"records\":[", N, sizeof(NameHashCell));
    for (std::size_t k = 0; k != N; ++k)
        std::printf("%s{\"name_is_input\":%s,\"hash\":%u,\"unwritten\":%u}", k ? "," : "",
                    cells[k].name == names[k] ? "true" : "false", cells[k].hash, cells[k].unwritten);
    std::printf("]}\n");
}
}  // namespace

int main(int argc, char** argv) {
    using namespace sotc;
    run_all<1>(init_ctor_669450);
    run_all<3>(init_ctor_842450);
    run_all<2>(init_ctor_7dace0);
    run_all<3>(init_ctor_6dde20);
    static_assert(sizeof(std::array<NameHashCell, 3>) == 48);

    if (argc > 2 && std::strcmp(argv[1], "--dump") == 0) {
        const std::string which = argv[2];
        if (which == "669450") dump<1>(init_ctor_669450, argc, argv, 3);
        else if (which == "842450") dump<3>(init_ctor_842450, argc, argv, 3);
        else if (which == "7dace0") dump<2>(init_ctor_7dace0, argc, argv, 3);
        else if (which == "6dde20") dump<3>(init_ctor_6dde20, argc, argv, 3);
        else CHECK(false);
    } else {
        std::puts("hashed_name_ctors: all checks passed");
    }
    return 0;
}
