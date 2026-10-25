// Fixture for 0xf1a0: exact size/capacity/control fields, empty-string sharing, allocation tags,
// failure cleanup, and the existing 0xf590 release.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/shared_release_f590.hpp"
#include "core/shared_str_ctor_f1a0.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
struct AllocRecord {
    std::size_t size;
    std::uint32_t tag;
    std::uint32_t align;
    std::uint32_t flags;
    void* ptr;
};

std::vector<AllocRecord> g_allocs;
std::vector<std::pair<void*, std::uint32_t>> g_frees;
std::size_t g_fail_at = 0;
int g_destroyed = 0;

void* allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    if (g_fail_at != 0 && g_allocs.size() + 1 == g_fail_at) {
        return nullptr;
    }
    void* p = std::malloc(size);
    CHECK(p != nullptr);
    g_allocs.push_back({size, tag, align, flags, p});
    return p;
}

bool deallocate(void* ptr, std::uint32_t tag) {
    g_frees.emplace_back(ptr, tag);
    std::free(ptr);
    return true;
}

void destroy(void* object, int) {
    ++g_destroyed;
    std::free(object);
}

void free_control(void* control) { std::free(control); }

void reset() {
    g_allocs.clear();
    g_frees.clear();
    g_fail_at = 0;
    g_destroyed = 0;
}
}  // namespace

int main() {
    CHECK(kSharedStrCtorF1a0Rva == 0xf1a0);
    CHECK(kSharedStrCtorF1a0Size == 151);
    CHECK(kSharedStrCtorF1a0EndRva == 0xf237);
    CHECK(kSharedStrResizeF3c0Size == 459);
    CHECK(sizeof(SharedStr) == 0x20);

    SharedStrCtorHooks hooks{};
    hooks.alloc.allocate = allocate;
    hooks.alloc.deallocate = deallocate;

    // ---- 1. Non-empty: size 5, align4(9)-1 = 7, data tag 0, control tag 1. ----
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "hello", hooks));
        CHECK(str.size == 5);
        CHECK(str.aux == 7);
        CHECK(str.data != nullptr);
        CHECK(std::memcmp(str.data, "hello", 6) == 0);
        CHECK(str.ctl != nullptr);
        CHECK(*str.ctl == 1);
        CHECK(g_allocs.size() == 2);
        CHECK(g_allocs[0].size == 8 && g_allocs[0].tag == 0 && g_allocs[0].align == 8 &&
              g_allocs[0].flags == 0);
        CHECK(g_allocs[1].size == 8 && g_allocs[1].tag == 1 && g_allocs[1].align == 8 &&
              g_allocs[1].flags == 1);

        // The existing 0xf590 release consumes the ctor's control block.
        SharedHolderF590 holder{};
        holder.object = const_cast<std::uint8_t*>(str.data);
        holder.control = reinterpret_cast<std::atomic<std::uint64_t>*>(str.ctl);
        SharedReleaseF590Hooks release{};
        release.destroy = destroy;
        release.free_control = free_control;
        const auto result = shared_release_f590(holder, release);
        CHECK(result.ok);
        CHECK(result.had_control);
        CHECK(result.decremented);
        CHECK(result.was_last);
        CHECK(result.destroyed);
        CHECK(result.freed);
        CHECK(g_destroyed == 1);
    }

    // ---- 2. Empty: the shared zero byte, no allocation. ----
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "", hooks));
        CHECK(str.data == empty_string_f730().data);
        CHECK(str.size == 0 && str.aux == 0 && str.ctl == nullptr);
        CHECK(g_allocs.empty());
        CHECK(init_shared_str_f1a0(str, nullptr, hooks));
        CHECK(str.data == empty_string_f730().data);
        CHECK(g_allocs.empty());
    }

    // ---- 3. Data allocation failure: false, nothing allocated. ----
    {
        reset();
        g_fail_at = 1;
        SharedStr str{};
        CHECK(!init_shared_str_f1a0(str, "abc", hooks));
        CHECK(g_allocs.empty());
        CHECK(g_frees.empty());
    }

    // ---- 4. Control allocation failure: data returned through the tag-0 deallocator. ----
    {
        reset();
        g_fail_at = 2;
        SharedStr str{};
        CHECK(!init_shared_str_f1a0(str, "abc", hooks));
        CHECK(g_allocs.size() == 1);
        CHECK(g_frees.size() == 1);
        CHECK(g_frees[0].second == 0);
        CHECK(g_frees[0].first == g_allocs[0].ptr);
    }

    // ---- 5. Missing allocator is a visible failure. ----
    {
        reset();
        SharedStrCtorHooks empty{};
        SharedStr str{};
        CHECK(!init_shared_str_f1a0(str, "abc", empty));
    }

    return 0;
}
