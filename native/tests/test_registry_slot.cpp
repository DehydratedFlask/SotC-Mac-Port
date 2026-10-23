// Fixture for the registry-owner slot replace (0x256f98..0x256fd2): construct-before-destroy,
// previous teardown, slot replacement, and the missing-teardown failure surface.
#include <cstdio>
#include <cstdlib>

#include "core/registry_slot.hpp"
#include "core/app_info_install.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_entries = 0;
int g_frees = 0;
int g_delegates = 0;

void* entry_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    ++g_entries;
    (void)tag;
    (void)align;
    (void)flags;
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

bool entry_deallocate(void* ptr, std::uint32_t tag) {
    ++g_frees;
    CHECK(tag == 2);
    std::free(ptr);
    return true;
}

void free_tag2(void* ptr) {
    CHECK(entry_deallocate(ptr, 2));
}

void* delegate_allocate(std::size_t size) {
    ++g_delegates;
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

void delegate_deallocate(void* ptr) {
    std::free(ptr);
}

}  // namespace

int main() {
    CHECK(kRegistrySlotRva == 0x2ee68c8);
    CHECK(kRegistrySlotOwnerBytes == 0x60);

    RegistrySlotHooks hooks{};
    hooks.delegates.alloc = delegate_allocate;
    hooks.delegates.dealloc = delegate_deallocate;
    hooks.dtor.free_tag2 = free_tag2;

    Insert1bdb50Hooks entry_hooks{};
    entry_hooks.alloc.allocate = entry_allocate;
    entry_hooks.alloc.deallocate = entry_deallocate;

    // ---- 1. First install: no previous owner, no teardown needed. ----
    RegistrySlot slot;
    CHECK(slot.get() == nullptr);
    const auto first = slot.install(hooks);
    CHECK(first.ok);
    CHECK(first.created);
    CHECK(!first.replaced_previous);
    CHECK(slot.get() != nullptr);
    CHECK(slot.replace_count() == 1);
    CHECK(slot.get()->registry().entry_count() == 0);

    // ---- 2. Populate the owner, then replace it: the previous nodes are torn down. ----
    {
        AppInfoProductDelegate product{};
        product.values.build_version = "b";
        product.insert = entry_hooks;
        RegistryOwner* const old = slot.get();
        CHECK(old->add_delegate({&product, install_app_info_product_entry}).ok);
        CHECK(slot.get()->registry().add_string_entry("k", "v", 1, entry_hooks).ok);
        g_entries = 0;
        g_frees = 0;
        g_delegates = 0;

        const auto second = slot.install(hooks);
        CHECK(second.ok);
        CHECK(second.created);
        CHECK(second.replaced_previous);
        CHECK(second.freed_previous_nodes == 2);  // one list-A entry + one list-C delegate
        CHECK(second.previous_release_failures == 0);
        CHECK(slot.replace_count() == 2);
        CHECK(slot.get() != old);
        CHECK(slot.get()->registry().entry_count() == 0);
        CHECK(slot.get()->delegate_count() == 0);
        CHECK(g_frees == 2);  // both old nodes freed by the container dtor
    }

    // ---- 3. Missing teardown on replace: the existing owner is untouched. ----
    {
        RegistrySlotHooks bad = hooks;
        bad.dtor.free_tag2 = nullptr;
        RegistryOwner* const before = slot.get();
        const int before_count = slot.replace_count();
        const auto failed = slot.install(bad);
        CHECK(!failed.ok);
        CHECK(!failed.created);
        CHECK(slot.get() == before);
        CHECK(slot.replace_count() == before_count);
    }

    // ---- 4. First install may omit the teardown hook (nothing to tear down). ----
    {
        RegistrySlot fresh;
        RegistrySlotHooks no_dtor{};
        no_dtor.delegates = hooks.delegates;
        const auto installed = fresh.install(no_dtor);
        CHECK(installed.ok);
        CHECK(fresh.get() != nullptr);
    }

    // ---- 5. The slot replacement is where the app-info delegate gets registered natively. ----
    {
        AppInfoProductDelegate product{};
        product.values.build_version = "b";
        product.values.camera_available = false;
        product.insert = entry_hooks;
        CHECK(slot.get()->add_delegate({&product, install_app_info_product_entry}).ok);
        AppInfoGameDelegate game{"Test Game", entry_hooks};
        CHECK(slot.get()->add_delegate({&game, install_app_info_game_entry_delegate}).ok);
        CHECK(slot.get()->dispatch().ok);
        CHECK(product.last.ok);
        CHECK(game.last.ok);
        CHECK(slot.get()->registry().entry_count() == 4);
    }

    return 0;
}
