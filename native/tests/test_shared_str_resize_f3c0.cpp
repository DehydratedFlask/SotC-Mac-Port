// Fixture for 0xf3c0 (SharedStr::resize) and 0xf6b0 (resize + NUL).
//
// Covers the four original paths, the exact allocator tags, the byte-verified return value
// (only path 4 returns true), the NUL write, and the native failure surfaces.
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <vector>

#include "core/shared_str_ctor_f1a0.hpp"
#include "core/shared_str_release_f590.hpp"
#include "core/shared_str_resize_f3c0.hpp"

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
int g_control_freed = 0;

void* allocate(std::size_t size, std::uint32_t tag, std::uint32_t align,
               std::uint32_t flags) {
    if (g_fail_at != 0 && g_allocs.size() + 1 == g_fail_at) {
        return nullptr;
    }
    void* p = std::malloc(size == 0 ? 1 : size);
    CHECK(p != nullptr);
    g_allocs.push_back({size, tag, align, flags, p});
    return p;
}

bool deallocate(void* ptr, std::uint32_t tag) {
    g_frees.emplace_back(ptr, tag);
    std::free(ptr);
    return true;
}

void reset() {
    g_allocs.clear();
    g_frees.clear();
    g_fail_at = 0;
    g_destroyed = 0;
    g_control_freed = 0;
}

// The 0xf590 release uses atomic_ref<int64_t>; the deallocator records the same tags the
// resize module uses (data 0, control 1).
class TestDeallocator final : public SharedStrDeallocatorF590 {
public:
    void deallocate(const void* allocation,
                    SharedStrDeallocationKindF590 kind) noexcept override {
        const auto tag = static_cast<std::uint32_t>(kind);
        g_frees.emplace_back(const_cast<void*>(allocation), tag);
        if (kind == SharedStrDeallocationKindF590::data) {
            ++g_destroyed;
        } else {
            ++g_control_freed;
        }
        std::free(const_cast<void*>(allocation));
    }
};

// Releases a holder whose control count is 1; checks both allocations are freed.
void release_holder(SharedStr& str) {
    CHECK(str.ctl != nullptr);
    const int destroyed_before = g_destroyed;
    const int control_before = g_control_freed;
    TestDeallocator deallocator;
    release_shared_str_f590(str, deallocator);
    CHECK(g_destroyed == destroyed_before + 1);
    CHECK(g_control_freed == control_before + 1);
    str = SharedStr{};
}

// Drops one reference without freeing (count must be > 1).
void release_once_no_free(const SharedStr& str) {
    const int destroyed_before = g_destroyed;
    const int control_before = g_control_freed;
    TestDeallocator deallocator;
    SharedStr copy = str;
    release_shared_str_f590(copy, deallocator);
    CHECK(g_destroyed == destroyed_before);
    CHECK(g_control_freed == control_before);
}
}  // namespace

int main() {
    CHECK(kSharedStrResizeF3c0Rva == 0xf3c0);
    CHECK(kSharedStrResizeF3c0Size == 459);
    CHECK(kSharedStrResizeF3c0EndRva == 0xf58b);
    CHECK(kSharedStrTerminateF6b0Rva == 0xf6b0);
    CHECK(kSharedStrTerminateF6b0Size == 32);
    CHECK(kSharedStrTerminateF6b0EndRva == 0xf6d0);
    CHECK(std::strncmp(kSharedStrResizeF3c0Sha256, "980c6911", 8) == 0);
    CHECK(std::strncmp(kSharedStrTerminateF6b0Sha256, "bfaf9b09", 8) == 0);
    CHECK(kSharedStrResizeDataTag == 0 && kSharedStrResizeControlTag == 1);
    CHECK(kSharedStrResizeSmallLimit == 0x80 && kSharedStrResizeMediumLimit == 0x400);
    CHECK(kSharedStrResizeSmallAlign == 0x10 && kSharedStrResizeMediumAlign == 0x100 &&
          kSharedStrResizeLargeAlign == 0x400);

    SharedStrCtorHooks hooks{};
    hooks.alloc.allocate = allocate;
    hooks.alloc.deallocate = deallocate;

    // ---- 1. Path 1, canonical empty holder: data + control, align4 capacity. ----
    {
        reset();
        SharedStr str = empty_string_f730();
        const std::size_t alloc_base = g_allocs.size();
        // Byte-verified: path 1 allocates but returns 0 in the original (0xf430/0xf46b).
        CHECK(!resize_shared_str_f3c0(str, 5, hooks));
        CHECK(str.size == 5);
        CHECK(str.aux == 7);  // align4(5+4) = 8, capacity = 7
        CHECK(str.data != empty_string_f730().data);
        CHECK(str.ctl != nullptr && *str.ctl == 1);
        CHECK(g_allocs.size() == alloc_base + 2);
        CHECK(g_allocs[alloc_base].size == 8 && g_allocs[alloc_base].tag == 0 &&
              g_allocs[alloc_base].align == 8 && g_allocs[alloc_base].flags == 0);
        CHECK(g_allocs[alloc_base + 1].size == 8 && g_allocs[alloc_base + 1].tag == 1 &&
              g_allocs[alloc_base + 1].align == 8 && g_allocs[alloc_base + 1].flags == 1);
        CHECK(g_frees.empty());
        release_holder(str);
    }

    // ---- 2. Path 1 with a heap old buffer: prefix copied, old bytes untouched, no old free. ----
    {
        reset();
        auto* old = static_cast<std::uint8_t*>(std::malloc(8));
        CHECK(old != nullptr);
        std::memcpy(old, "abc", 4);
        SharedStr str{};
        str.data = old;
        str.size = 3;
        str.aux = 7;
        str.ctl = nullptr;
        CHECK(!resize_shared_str_f3c0(str, 10, hooks));
        CHECK(str.size == 10);
        CHECK(str.aux == 11);  // align4(10+4) = 12, capacity = 11
        CHECK(str.ctl != nullptr && *str.ctl == 1);
        CHECK(std::memcmp(str.data, "abc", 3) == 0);
        CHECK(std::memcmp(old, "abc", 4) == 0);
        CHECK(g_frees.empty());  // path 1 does not free the old data
        release_holder(str);
        std::free(old);
    }

    // ---- 3. Path 2, shared control (count 2): new data/control, old count -> 1, no old free. ----
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "abcdef", hooks));
        SharedStr old = str;
        std::atomic_ref<std::int64_t>(*old.ctl).fetch_add(1);
        CHECK(*str.ctl == 2);
        const auto* const old_data = str.data;
        auto* const old_ctl = str.ctl;
        const std::size_t alloc_base = g_allocs.size();
        const std::size_t free_base = g_frees.size();

        CHECK(!resize_shared_str_f3c0(str, 10, hooks));  // path 2 returns 0 in the original
        CHECK(str.size == 10);
        CHECK(str.aux == 11);
        CHECK(str.data != old_data);
        CHECK(str.ctl != old_ctl);
        CHECK(*str.ctl == 1);
        CHECK(*old_ctl == 1);  // old count decremented, not freed
        CHECK(std::memcmp(str.data, "abcdef", 6) == 0);
        CHECK(g_allocs.size() == alloc_base + 2);
        CHECK(g_allocs[alloc_base].size == 12 && g_allocs[alloc_base].tag == 0);
        CHECK(g_allocs[alloc_base + 1].size == 8 && g_allocs[alloc_base + 1].tag == 1);
        CHECK(g_frees.size() == free_base);  // old data and control still live

        // Release the remaining old reference manually: it must free both allocations.
        const int destroyed_before = g_destroyed;
        const int control_before = g_control_freed;
        release_holder(old);
        CHECK(g_destroyed == destroyed_before + 1);
        CHECK(g_control_freed == control_before + 1);
        release_holder(str);
    }

    // ---- 4. Path 3, capacity sufficient: only size changes, no allocation, returns false. ----
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "hello", hooks));
        const auto* const data = str.data;
        auto* const ctl = str.ctl;
        const std::int64_t aux = str.aux;
        const std::size_t alloc_base = g_allocs.size();
        const std::size_t free_base = g_frees.size();
        CHECK(!resize_shared_str_f3c0(str, 7, hooks));
        CHECK(str.size == 7);
        CHECK(str.data == data && str.ctl == ctl && str.aux == aux);
        CHECK(g_allocs.size() == alloc_base && g_frees.size() == free_base);
        CHECK(!resize_shared_str_f3c0(str, 2, hooks));
        CHECK(str.size == 2);
        CHECK(str.data == data && str.ctl == ctl && str.aux == aux);
        CHECK(g_allocs.size() == alloc_base && g_frees.size() == free_base);
        release_holder(str);
    }

    // ---- 5. Path 4, capacity short, all three size classes: data realloc, old free tag 0. ----
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "abc", hooks));
        auto* const ctl = str.ctl;

        const auto* const old_small = str.data;
        std::size_t alloc_base = g_allocs.size();
        std::size_t free_base = g_frees.size();
        CHECK(resize_shared_str_f3c0(str, 0x40, hooks));  // align16(0x50) = 0x50
        CHECK(str.size == 0x40);
        CHECK(str.aux == 0x4F);
        CHECK(str.ctl == ctl && *ctl == 1);
        CHECK(std::memcmp(str.data, "abc", 3) == 0);
        CHECK(g_allocs.size() == alloc_base + 1);
        CHECK(g_allocs[alloc_base].size == 0x50 && g_allocs[alloc_base].tag == 0);
        CHECK(g_frees.size() == free_base + 1);
        CHECK(g_frees[free_base].first == old_small && g_frees[free_base].second == 0);

        const auto* const old_medium = str.data;
        alloc_base = g_allocs.size();
        free_base = g_frees.size();
        CHECK(resize_shared_str_f3c0(str, 0x100, hooks));  // align256(0x200) = 0x200
        CHECK(str.size == 0x100);
        CHECK(str.aux == 0x1FF);
        CHECK(str.ctl == ctl && *ctl == 1);
        CHECK(std::memcmp(str.data, "abc", 3) == 0);
        CHECK(g_allocs.size() == alloc_base + 1);
        CHECK(g_allocs[alloc_base].size == 0x200 && g_allocs[alloc_base].tag == 0);
        CHECK(g_frees.size() == free_base + 1);
        CHECK(g_frees[free_base].first == old_medium && g_frees[free_base].second == 0);

        const auto* const old_large = str.data;
        alloc_base = g_allocs.size();
        free_base = g_frees.size();
        CHECK(resize_shared_str_f3c0(str, 0x401, hooks));  // align1024(0x801) = 0x800
        CHECK(str.size == 0x401);
        CHECK(str.aux == 0x7FF);
        CHECK(str.ctl == ctl && *ctl == 1);
        CHECK(std::memcmp(str.data, "abc", 3) == 0);
        CHECK(g_allocs.size() == alloc_base + 1);
        CHECK(g_allocs[alloc_base].size == 0x800 && g_allocs[alloc_base].tag == 0);
        CHECK(g_frees.size() == free_base + 1);
        CHECK(g_frees[free_base].first == old_large && g_frees[free_base].second == 0);
        release_holder(str);
    }

    // ---- 6. 0xf6b0: resize then NUL at data[size]. ----
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "hello", hooks));
        const auto* const data = str.data;
        const std::size_t alloc_base = g_allocs.size();
        CHECK(terminate_shared_str_f6b0(str, 4, hooks));  // path 3
        CHECK(str.size == 4);
        CHECK(str.data == data);
        CHECK(std::memcmp(str.data, "hell", 4) == 0);
        CHECK(str.data[4] == 0);
        CHECK(g_allocs.size() == alloc_base);
        release_holder(str);
    }
    {
        reset();
        SharedStr str = empty_string_f730();
        CHECK(terminate_shared_str_f6b0(str, 5, hooks));  // path 1
        CHECK(str.size == 5);
        CHECK(str.aux == 7);  // align4(5+4) = 8, capacity = 7
        CHECK(str.data[5] == 0);
        release_holder(str);
    }
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "abc", hooks));
        CHECK(terminate_shared_str_f6b0(str, 100, hooks));  // path 4
        CHECK(str.size == 100);
        CHECK(str.aux == 111);  // align16(100+0x10) = 112, capacity = 111
        CHECK(std::memcmp(str.data, "abc", 3) == 0);
        CHECK(str.data[100] == 0);
        release_holder(str);
    }

    // ---- 7. Failure surfaces. ----
    {
        reset();
        SharedStrCtorHooks empty{};
        SharedStr str = empty_string_f730();
        CHECK(!resize_shared_str_f3c0(str, 5, empty));
        CHECK(str.data == empty_string_f730().data && str.size == 0 && str.aux == 0 &&
              str.ctl == nullptr);
        CHECK(g_allocs.empty() && g_frees.empty());
    }
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "abc", hooks));
        const auto* const data = str.data;
        auto* const ctl = str.ctl;
        const std::int64_t aux = str.aux;
        SharedStrCtorHooks empty{};
        CHECK(!resize_shared_str_f3c0(str, 100, empty));
        CHECK(str.data == data && str.size == 3 && str.aux == aux && str.ctl == ctl);
        release_holder(str);
    }
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "abc", hooks));
        const auto* const data = str.data;
        auto* const ctl = str.ctl;
        const std::int64_t aux = str.aux;
        const std::size_t alloc_base = g_allocs.size();
        SharedStrCtorHooks alloc_only{};
        alloc_only.alloc.allocate = allocate;
        CHECK(!resize_shared_str_f3c0(str, 100, alloc_only));
        CHECK(str.data == data && str.size == 3 && str.aux == aux && str.ctl == ctl);
        CHECK(g_allocs.size() == alloc_base);
        release_holder(str);
    }
    {
        reset();
        SharedStr str = empty_string_f730();
        g_fail_at = 1;
        CHECK(!resize_shared_str_f3c0(str, 5, hooks));
        CHECK(str.data == empty_string_f730().data && str.size == 0 && str.aux == 0 &&
              str.ctl == nullptr);
        CHECK(g_allocs.empty() && g_frees.empty());
    }
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "abc", hooks));
        SharedStr old = str;
        std::atomic_ref<std::int64_t>(*old.ctl).fetch_add(1);
        const auto* const data = str.data;
        auto* const ctl = str.ctl;
        g_fail_at = g_allocs.size() + 1;
        CHECK(!resize_shared_str_f3c0(str, 10, hooks));
        CHECK(str.data == data && str.size == 3 && str.aux == 3 && str.ctl == ctl);
        CHECK(*ctl == 2);
        CHECK(g_frees.empty());
        release_once_no_free(str);  // 2 -> 1
        release_holder(old);        // 1 -> 0, frees the shared old pair
        str = SharedStr{};
    }
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "abc", hooks));
        const auto* const data = str.data;
        auto* const ctl = str.ctl;
        const std::int64_t aux = str.aux;
        g_fail_at = g_allocs.size() + 1;
        CHECK(!resize_shared_str_f3c0(str, 100, hooks));
        CHECK(str.data == data && str.size == 3 && str.aux == aux && str.ctl == ctl);
        CHECK(g_frees.empty());
        release_holder(str);
    }
    {
        reset();
        SharedStr str = empty_string_f730();
        g_fail_at = 2;  // data succeeds, control fails
        CHECK(!resize_shared_str_f3c0(str, 5, hooks));
        CHECK(str.data == empty_string_f730().data && str.size == 0 && str.aux == 0 &&
              str.ctl == nullptr);
        CHECK(g_allocs.size() == 1);
        CHECK(g_frees.size() == 1 && g_frees[0].second == 0 &&
              g_frees[0].first == g_allocs[0].ptr);
    }
    {
        reset();
        SharedStr str{};
        CHECK(init_shared_str_f1a0(str, "abc", hooks));
        SharedStr old = str;
        std::atomic_ref<std::int64_t>(*old.ctl).fetch_add(1);
        const auto* const data = str.data;
        auto* const ctl = str.ctl;
        g_fail_at = g_allocs.size() + 2;  // data succeeds, control fails
        CHECK(!resize_shared_str_f3c0(str, 10, hooks));
        CHECK(str.data == data && str.size == 3 && str.aux == 3 && str.ctl == ctl);
        CHECK(*ctl == 2);  // old reference untouched
        CHECK(g_frees.size() == 1 && g_frees[0].second == 0 &&
              g_frees[0].first == g_allocs.back().ptr);
        release_once_no_free(str);
        release_holder(old);
        str = SharedStr{};
    }

    return 0;
}
