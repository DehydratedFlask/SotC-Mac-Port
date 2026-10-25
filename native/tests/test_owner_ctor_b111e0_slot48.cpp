// Fixture for owner vtable slot +0x48 (`0xb111e0`) and its two callees (`0x256970`, `0xa46dc0`).
//
// The recovered bodies never read `rdi` and store no vtable pointer, so this fixture pins the call
// order, the replace/destroy path, the `+0x178` flag store and every hook failure surface instead.
// Byte-exact sizes and the vtable slot wiring are pinned by
// `tools/lift/check_fowner_ctor_b111e0_b11210.py`.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/owner_ctor_b111e0_slot48.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

enum Step { kAlloc = 0, kConstruct = 1, kDestroy = 2, kFree = 3, kUserInit = 4, kShare = 5 };

std::vector<int> g_order;
alignas(16) std::byte g_objects[2][kOwnerSingletonObjectSize];
int g_object_next = 0;
std::size_t g_alloc_size = 0;
void* g_alloc_override = nullptr;
void* g_last_freed = nullptr;
void* g_last_destroyed = nullptr;
void* g_last_constructed = nullptr;
bool g_construct_ok = true;
std::int32_t g_priority = -1;
std::int32_t g_share_args[4] = {-1, -1, -1, -1};

void reset() {
    g_order.clear();
    g_object_next = 0;
    g_alloc_size = 0;
    g_alloc_override = nullptr;
    g_last_freed = nullptr;
    g_last_destroyed = nullptr;
    g_last_constructed = nullptr;
    g_construct_ok = true;
    g_priority = -1;
    for (auto& arg : g_share_args) {
        arg = -1;
    }
}

void* allocate(std::size_t size) {
    g_alloc_size = size;
    g_order.push_back(kAlloc);
    if (g_alloc_override != nullptr) {
        return g_alloc_override;
    }
    return g_objects[g_object_next++ % 2];
}

void deallocate(void* object) {
    g_order.push_back(kFree);
    g_last_freed = object;
}

bool construct(void* object) {
    g_order.push_back(kConstruct);
    g_last_constructed = object;
    return g_construct_ok;
}

void destroy(void* object) {
    g_order.push_back(kDestroy);
    g_last_destroyed = object;
}

void user_service_init(std::int32_t priority) {
    g_order.push_back(kUserInit);
    g_priority = priority;
    if (g_last_constructed != nullptr) {
        std::memset(g_last_constructed, 0xCC, kOwnerSingletonObjectSize);
    }
}

void share_utility_call(std::int32_t a1, std::int32_t a2, std::int32_t a3, std::int32_t a4) {
    g_order.push_back(kShare);
    g_share_args[0] = a1;
    g_share_args[1] = a2;
    g_share_args[2] = a3;
    g_share_args[3] = a4;
}

OwnerSlot48Hooks full_hooks() {
    OwnerSlot48Hooks hooks{};
    hooks.allocate = allocate;
    hooks.deallocate = deallocate;
    hooks.construct = construct;
    hooks.destroy = destroy;
    hooks.user_service_init = user_service_init;
    hooks.share_utility_call = share_utility_call;
    return hooks;
}

}  // namespace

int main() {
    CHECK(kOwnerSlot48Rva == 0xb111e0);
    CHECK(kOwnerSlot48Size == 0x19);
    CHECK(kOwnerSingletonEnsureRva == 0x256970);
    CHECK(kOwnerSingletonEnsureSize == 0x51);
    CHECK(kOwnerSingletonGlobalRva == 0x32f79a8);
    CHECK(kOwnerSingletonObjectSize == 0x210);
    CHECK(kOwnerSingletonActivateRva == 0xa46dc0);
    CHECK(kOwnerSingletonActivateSize == 0x65);
    CHECK(kOwnerSingletonFlagOffset == 0x178);
    CHECK(kOwnerUserServicePriority == 0x2bc);

    // ---- 1. First ensure: allocate, construct, store. ----
    reset();
    {
        OwnerSingletonCell cell;
        const OwnerSlot48Hooks hooks = full_hooks();
        CHECK(owner_singleton_ensure(cell, hooks));
        CHECK(g_alloc_size == kOwnerSingletonObjectSize);
        CHECK(cell.object == &g_objects[0]);
        CHECK(g_last_constructed == cell.object);
        CHECK(g_order == (std::vector<int>{kAlloc, kConstruct}));
        CHECK(g_last_destroyed == nullptr);
        CHECK(g_last_freed == nullptr);
    }

    // ---- 2. Replace: destroy then free the previous object, then store. ----
    reset();
    {
        OwnerSingletonCell cell;
        cell.object = &g_objects[1];
        const OwnerSlot48Hooks hooks = full_hooks();
        CHECK(owner_singleton_ensure(cell, hooks));
        CHECK(cell.object == &g_objects[0]);
        CHECK(g_last_destroyed == &g_objects[1]);
        CHECK(g_last_freed == &g_objects[1]);
        CHECK(g_order == (std::vector<int>{kAlloc, kConstruct, kDestroy, kFree}));
    }

    // ---- 3. Unchanged pointer: no destroy, no free, no second store. ----
    reset();
    {
        OwnerSingletonCell cell;
        cell.object = &g_objects[1];
        g_alloc_override = &g_objects[1];
        const OwnerSlot48Hooks hooks = full_hooks();
        CHECK(owner_singleton_ensure(cell, hooks));
        CHECK(cell.object == &g_objects[1]);
        CHECK(g_order == (std::vector<int>{kAlloc, kConstruct}));
        CHECK(g_last_destroyed == nullptr);
        CHECK(g_last_freed == nullptr);
    }

    // ---- 4. Construct failure: the new object is released, the cell is untouched. ----
    reset();
    {
        OwnerSingletonCell cell;
        cell.object = &g_objects[1];
        g_construct_ok = false;
        const OwnerSlot48Hooks hooks = full_hooks();
        CHECK(!owner_singleton_ensure(cell, hooks));
        CHECK(cell.object == &g_objects[1]);
        CHECK(g_last_freed == &g_objects[0]);
        CHECK(g_last_destroyed == nullptr);
        CHECK(g_order == (std::vector<int>{kAlloc, kConstruct, kFree}));
    }

    // ---- 5. Missing destroy with a live previous object: visible failure, new freed. ----
    reset();
    {
        OwnerSingletonCell cell;
        cell.object = &g_objects[1];
        OwnerSlot48Hooks hooks = full_hooks();
        hooks.destroy = nullptr;
        CHECK(!owner_singleton_ensure(cell, hooks));
        CHECK(cell.object == &g_objects[1]);
        CHECK(g_last_freed == &g_objects[0]);
        CHECK(g_order == (std::vector<int>{kAlloc, kConstruct, kFree}));
    }

    // ---- 6. Missing allocate: nothing runs. ----
    reset();
    {
        OwnerSingletonCell cell;
        OwnerSlot48Hooks hooks = full_hooks();
        hooks.allocate = nullptr;
        CHECK(!owner_singleton_ensure(cell, hooks));
        CHECK(cell.object == nullptr);
        CHECK(g_order.empty());
    }

    // ---- 7. Activate: priority, the four constants, the single +0x178 byte. ----
    reset();
    {
        std::byte object[kOwnerSingletonObjectSize];
        std::memset(object, 0xCC, sizeof(object));
        const OwnerSlot48Hooks hooks = full_hooks();
        CHECK(owner_singleton_activate(object, hooks));
        CHECK(g_order == (std::vector<int>{kUserInit, kShare}));
        CHECK(g_priority == kOwnerUserServicePriority);
        CHECK(g_share_args[0] == kOwnerShareUtilityArg0);
        CHECK(g_share_args[1] == kOwnerShareUtilityArg1);
        CHECK(g_share_args[2] == kOwnerShareUtilityArg2);
        CHECK(g_share_args[3] == kOwnerShareUtilityArg3);
        CHECK(kOwnerShareUtilityArg0 == 1);
        CHECK(kOwnerShareUtilityArg1 == 0x20000);
        CHECK(kOwnerShareUtilityArg2 == 0x2bc);
        CHECK(kOwnerShareUtilityArg3 == 0x3f);
        CHECK(static_cast<std::uint8_t>(object[kOwnerSingletonFlagOffset]) == 1);
        for (std::size_t i = 0; i < sizeof(object); ++i) {
            if (i != kOwnerSingletonFlagOffset) {
                CHECK(static_cast<std::uint8_t>(object[i]) == 0xCC);
            }
        }
    }

    // ---- 8. Activate failure surfaces: no call, no flag write. ----
    reset();
    {
        std::byte object[kOwnerSingletonObjectSize];
        std::memset(object, 0xCC, sizeof(object));
        const OwnerSlot48Hooks hooks = full_hooks();
        CHECK(!owner_singleton_activate(nullptr, hooks));
        CHECK(g_order.empty());

        OwnerSlot48Hooks missing_share = full_hooks();
        missing_share.share_utility_call = nullptr;
        CHECK(!owner_singleton_activate(object, missing_share));
        CHECK(g_order.empty());
        CHECK(static_cast<std::uint8_t>(object[kOwnerSingletonFlagOffset]) == 0xCC);
    }

    // ---- 9. Slot body: ensure then activate the cell's current object. ----
    reset();
    {
        OwnerSingletonCell cell;
        const OwnerSlot48Hooks hooks = full_hooks();
        CHECK(owner_slot48_run(cell, hooks));
        CHECK(cell.object == &g_objects[0]);
        CHECK(g_order == (std::vector<int>{kAlloc, kConstruct, kUserInit, kShare}));
        CHECK(g_last_constructed == cell.object);
        CHECK(static_cast<std::uint8_t*>(cell.object)[kOwnerSingletonFlagOffset] == 1);
    }

    // ---- 10. Slot body: a failing ensure stops before activate. ----
    reset();
    {
        OwnerSingletonCell cell;
        OwnerSlot48Hooks hooks = full_hooks();
        hooks.allocate = nullptr;
        CHECK(!owner_slot48_run(cell, hooks));
        CHECK(g_order.empty());
        CHECK(cell.object == nullptr);
    }

    return 0;
}
