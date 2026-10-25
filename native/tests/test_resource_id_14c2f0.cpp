// Fixture for 0x14c2f0: "****" rewrite before FNV-1a-32, no-placeholder path, missing hook.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/fnv_span_161f80.hpp"
#include "core/resource_id_14c2f0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
std::uint32_t g_last_fourcc = 0;
int g_lookups = 0;
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

std::uint32_t platform_tag(std::uint32_t fourcc) {
    ++g_lookups;
    g_last_fourcc = fourcc;
    return 0x11223344u;
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
    CHECK(kResourceIdCtor14c2f0Rva == 0x14c2f0);
    CHECK(kResourceIdCtor14c2f0Size == 166);
    CHECK(kPlatformTagLookupRva == 0x147e20);

    SharedStrCtorHooks str_hooks{};
    str_hooks.alloc.allocate = allocate;
    str_hooks.alloc.deallocate = deallocate;
    ResourceIdCtorHooks hooks{};
    hooks.str = str_hooks;
    hooks.platform_tag = platform_tag;

    // ---- 1. "****" is patched before hashing; fourcc is the byte-swapped last four bytes. ----
    {
        g_lookups = 0;
        g_allocs = 0;
        const char* const literal = "$/Models/****/shadowmodels.crmp";
        HashedStr key{};
        CHECK(resource_id_ctor_14c2f0(key, literal, hooks));
        CHECK(g_lookups == 1);
        CHECK(g_last_fourcc == 0x63726d70u);  // "crmp" loaded with movbe
        CHECK(std::memcmp(key.str.data, "$/Models/", 9) == 0);
        CHECK(std::memcmp(key.str.data + 9, "\x44\x33\x22\x11", 4) == 0);
        CHECK(key.str.size == 31);
        CHECK(key.str.aux == 31);
        CHECK(key.gap04[0] == 0 && key.gap04[1] == 0 && key.gap04[2] == 0 && key.gap04[3] == 0);
        const ByteSpan span{key.str.data, key.str.size};
        CHECK(key.hash == fnv_span_161f80(span, kFnvOffsetBasis161f80));
        CHECK(g_allocs == 2);
        free_key(key);
    }

    // ---- 2. No placeholder: the literal is hashed unchanged. ----
    {
        g_lookups = 0;
        const char* const literal = "plain/path.crmp";
        HashedStr key{};
        CHECK(resource_id_ctor_14c2f0(key, literal, hooks));
        CHECK(g_lookups == 0);
        CHECK(std::memcmp(key.str.data, literal, std::strlen(literal) + 1) == 0);
        const ByteSpan span{key.str.data, key.str.size};
        CHECK(key.hash == fnv_span_161f80(span, kFnvOffsetBasis161f80));
        free_key(key);
    }

    // ---- 3. A "****" literal without a lookup fails before allocating. ----
    {
        g_allocs = 0;
        ResourceIdCtorHooks no_tag = hooks;
        no_tag.platform_tag = nullptr;
        HashedStr key{};
        CHECK(!resource_id_ctor_14c2f0(key, "$/Models/****/shadowmodels.crmp", no_tag));
        CHECK(g_allocs == 0);
    }

    // ---- 4. Empty literal shares the empty string. ----
    {
        HashedStr key{};
        CHECK(resource_id_ctor_14c2f0(key, "", hooks));
        CHECK(key.str.data == empty_string_f730().data);
        CHECK(key.str.size == 0 && key.str.aux == 0 && key.str.ctl == nullptr);
        CHECK(key.hash == kFnvOffsetBasis161f80);
    }

    return 0;
}
