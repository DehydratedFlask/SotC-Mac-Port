// Fixture for the allocator wrappers 0x17070 / 0x170b0 / 0x170c0.
#include <cstdio>
#include <cstdlib>

#include "core/alloc_wrappers_17070.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_alloc = 0;
int g_free = 0;
std::size_t g_size = 0;
std::uint32_t g_tag = 0, g_align = 0, g_flags = 0;
void* g_free_ptr = nullptr;
unsigned char g_buf[0x60];

void* alloc_hook(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    ++g_alloc;
    g_size = size; g_tag = tag; g_align = align; g_flags = flags;
    return size <= sizeof(g_buf) ? static_cast<void*>(g_buf) : nullptr;
}
bool free_hook(void* ptr, std::uint32_t tag) {
    ++g_free;
    g_free_ptr = ptr; g_tag = tag;
    return true;
}
}  // namespace

int main() {
    CHECK(kAlloc17070Rva == 0x17070 && kAlloc17070Size == 20);
    CHECK(kFree170b0Rva == 0x170b0 && kFree170b0Size == 10);
    CHECK(kFree170c0Rva == 0x170c0 && kFree170c0Size == 10);
    CHECK(kAllocDispatchRva == 0x12820 && kFreeDispatchRva == 0x12af0);

    AllocDispatchContract c{};
    c.allocate = alloc_hook;
    c.deallocate = free_hook;

    // 0x17070 passes (size, tag=1, align=8, flags=1).
    {
        g_alloc = 0;
        const auto r = alloc_17070(0x60, c);
        CHECK(r.ok);
        CHECK(r.dispatched);
        CHECK(g_alloc == 1);
        CHECK(g_size == 0x60 && g_tag == 1 && g_align == 8 && g_flags == 1);
        CHECK(r.pointer == static_cast<void*>(g_buf));
    }
    // Allocation failure is visible.
    {
        const auto r = alloc_17070(0x1000, c);
        CHECK(!r.ok);
        CHECK(r.dispatched);
        CHECK(r.pointer == nullptr);
    }
    // 0x170b0 -> tag 1, 0x170c0 -> tag 2.
    {
        int obj = 0;
        g_free = 0;
        CHECK(free_170b0(&obj, c).ok);
        CHECK(g_tag == 1 && g_free_ptr == &obj);
        CHECK(free_170c0(&obj, c).ok);
        CHECK(g_tag == 2);
        CHECK(g_free == 2);
    }
    // Null pointer is a no-op success, matching the dispatcher's own null check.
    {
        g_free = 0;
        const auto r = free_170b0(nullptr, c);
        CHECK(r.ok);
        CHECK(!r.dispatched);
        CHECK(g_free == 0);
    }
    // Missing contract is a visible failure.
    {
        AllocDispatchContract empty{};
        int obj = 0;
        CHECK(!alloc_17070(0x10, empty).ok);
        CHECK(!free_170b0(&obj, empty).ok);
    }

    std::printf("alloc_wrappers_17070: all checks passed (tag 1/2 dispatch, null no-op, visible failures)\n");
    return 0;
}
