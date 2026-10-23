// Fixture for 0x147e20: the 25-row table, exact/wildcard/uninitialised lookup, and the nine
// derived ResourceId hashes.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/platform_tag_147e20.hpp"
#include "core/resource_id_14c2f0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_allocs = 0;

void* allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    (void)tag;
    (void)align;
    (void)flags;
    ++g_allocs;
    void* p = std::malloc(size);
    CHECK(p != nullptr);
    return p;
}

bool deallocate(void* ptr, std::uint32_t tag) {
    (void)tag;
    std::free(ptr);
    return true;
}

std::uint32_t lookup(std::uint32_t fourcc) {
    return platform_tag_147e20(fourcc, PlatformRegistryState::kPopulated);
}

void free_key(HashedStr& key) {
    if (key.str.ctl != nullptr) {
        std::free(const_cast<std::uint8_t*>(key.str.data));
        std::free(key.str.ctl);
        key.str.data = nullptr;
        key.str.ctl = nullptr;
    }
}
}  // namespace

int main() {
    CHECK(kPlatformTagLookup147e20Rva == 0x147e20);
    CHECK(kPlatformTagLookup147e20Size == 208);
    CHECK(kPlatformDefaultKeySetterRva == 0x147e10);
    CHECK(kPlatformTagRowCount == 25);
    CHECK(kPlatformTagCmn == 0x6e6d635f);
    CHECK(kPlatformTagPs4 == 0x3473705f);
    CHECK(kPlatformWildcardFourcc == 0x2a2a2a2a);
    CHECK(fourcc_of("crmp") == 0x63726d70);
    CHECK(fourcc_of("****") == 0x2a2a2a2a);

    // ---- 1. Exact rows, wildcard and uninitialised fallback. ----
    CHECK(lookup(fourcc_of("crmp")) == kPlatformTagCmn);
    CHECK(lookup(fourcc_of("calt")) == kPlatformTagCmn);
    CHECK(lookup(fourcc_of("cfon")) == kPlatformTagPs4);
    CHECK(lookup(fourcc_of("cptx")) == kPlatformTagPs4);
    CHECK(lookup(fourcc_of("cctx")) == kPlatformTagPs4);
    CHECK(lookup(fourcc_of("****")) == kPlatformTagCmn);
    CHECK(lookup(fourcc_of("zzzz")) == kPlatformTagCmn);  // wildcard
    CHECK(platform_tag_147e20(fourcc_of("crmp"), PlatformRegistryState::kUninitialised) ==
          kPlatformTagPs4);

    // ---- 2. The nine patched literals hash to the derived values. ----
    struct Case {
        const char* literal;
        std::uint32_t expected;
    };
    const Case cases[] = {
        {"$/Models/****/shadowmodels.crmp", 0x2128cdb9},
        {"$/Models/****/shadowskels.crmp", 0xecf0a769},
        {"$/Models/****/shadowanims.crmp", 0x2b44f9db},
        {"$/sound/shadow/****/shadowbanks.crmp", 0x3b87dd56},
        {"$/sound/shadow/****/shadowstreams.crmp", 0xf2163cc4},
        {"$/sound/shadow/****/shadowrandomevents.crmp", 0x94a8da3f},
        {"$/sound/shadow/****/shadowbossmusic.crmp", 0xbfa276e7},
        {"$/characters/shadow/****/cp3Characters.calt", 0x99117412},
        {"$/cp3shadow/****/cp3shadowprecache.calt", 0x13a0d114},
    };
    SharedStrCtorHooks str_hooks{};
    str_hooks.alloc.allocate = allocate;
    str_hooks.alloc.deallocate = deallocate;
    ResourceIdCtorHooks hooks{};
    hooks.str = str_hooks;
    hooks.platform_tag = lookup;
    for (const Case& c : cases) {
        HashedStr key{};
        CHECK(resource_id_ctor_14c2f0(key, c.literal, hooks));
        CHECK(key.hash == c.expected);
        CHECK(std::memcmp(key.str.data, "$/Models/_cmn/", 14) == 0 ||
              std::memcmp(key.str.data, "$/sound/shadow/_cmn/", 20) == 0 ||
              std::memcmp(key.str.data, "$/characters/shadow/_cmn/", 25) == 0 ||
              std::memcmp(key.str.data, "$/cp3shadow/_cmn/", 17) == 0);
        free_key(key);
    }

    return 0;
}
