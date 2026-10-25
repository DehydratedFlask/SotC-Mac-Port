// Fixture for the native registry owner: 0x2569d0's container construction, list-3 delegate
// registration through 0x1beeb0, and the byte-checked FIFO traversal of 0x1bdf30.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string_view>
#include <vector>

#include "core/registry_owner.hpp"
#include "core/app_info_install.hpp"
#include "core/init_1290.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {
int g_entries = 0;
int g_entry_frees = 0;
int g_delegates = 0;
int g_delegate_frees = 0;

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
    ++g_entry_frees;
    CHECK(tag == 2);
    std::free(ptr);
    return true;
}

void entry_free_tag2(void* ptr) {
    CHECK(entry_deallocate(ptr, 2));
}

void* delegate_allocate(std::size_t size) {
    ++g_delegates;
    void* p = std::calloc(1, size);
    CHECK(p != nullptr);
    return p;
}

void delegate_deallocate(void* ptr) {
    ++g_delegate_frees;
    std::free(ptr);
}

std::vector<std::string_view> entry_names(const AppInfoRegistry& registry) {
    std::vector<std::string_view> names;
    const List1bdeb0& list = registry.info_list();
    const auto* const head = reinterpret_cast<const Node1bdeb0*>(&list);
    for (const Node1bdeb0* node = list.next; node != head; node = node->next) {
        names.push_back({reinterpret_cast<const char*>(node->callable_10.value_00),
                         static_cast<std::size_t>(node->callable_10.value_08)});
    }
    return names;
}

void free_entry_nodes(AppInfoRegistry& registry) {
    List1bdeb0& list = registry.info_list();
    auto* const head = reinterpret_cast<Node1bdeb0*>(&list);
    Node1bdeb0* node = list.next;
    while (node != head) {
        Node1bdeb0* const next = node->next;
        std::free(node);
        ++g_entry_frees;
        node = next;
    }
    list.prev = head;
    list.next = head;
    list.count = 0;
}

void record_delegate(void* ctx, AppInfoRegistry&) {
    auto* const log = static_cast<std::vector<int>*>(ctx);
    log->push_back(1);
}

void record_delegate_two(void* ctx, AppInfoRegistry&) {
    auto* const log = static_cast<std::vector<int>*>(ctx);
    log->push_back(2);
}

void noop_vcall(void*) {}
void noop_insert_21cd0(void*, const HandlerRegistration1290&) {}

}  // namespace

int main() {
    CHECK(kRegistryOwnerConstructRva == 0x256f98);
    CHECK(kRegistryOwnerDispatchRva == 0x1bdf30);

    Insert1beeb0Alloc delegate_alloc{};
    delegate_alloc.alloc = delegate_allocate;
    delegate_alloc.dealloc = delegate_deallocate;

    Insert1bdb50Hooks entry_hooks{};
    entry_hooks.alloc.allocate = entry_allocate;
    entry_hooks.alloc.deallocate = entry_deallocate;

    // ---- 1. Delegate registration order and FIFO dispatch. ----
    {
        RegistryOwner owner(delegate_alloc);
        CHECK(owner.delegate_count() == 0);
        std::vector<int> log;
        CHECK(owner.add_delegate({&log, record_delegate}).ok);
        CHECK(owner.add_delegate({&log, record_delegate_two}).ok);
        CHECK(owner.delegate_count() == 2);
        CHECK(owner.registry().entry_count() == 0);

        const auto dispatched = owner.dispatch();
        CHECK(dispatched.ok);
        CHECK(dispatched.called == 2);
        CHECK(log.size() == 2);
        CHECK(log[0] == 1);  // oldest first
        CHECK(log[1] == 2);
        // Destructor frees both nodes through the seam.
    }
    CHECK(g_delegate_frees == 2);

    // ---- 2. The recovered install order: product info first, "Game" second. ----
    {
        g_entries = 0;
        g_entry_frees = 0;
        RegistryOwner owner(delegate_alloc);

        static constexpr std::string_view game = "Test Game";
        static constexpr std::string_view camera = "setCameraPos 1.00 2.00 3.00 4.00 5.00 6.00";
        AppInfoProductDelegate product{};
        product.values.build_version = "build-9";
        product.values.neo_mode = false;
        product.values.debug_memory_available = false;
        product.values.camera_available = true;
        product.values.camera_location = camera;
        product.insert = entry_hooks;
        AppInfoGameDelegate game_source{game, entry_hooks};

        const auto product_node =
            owner.add_delegate({&product, install_app_info_product_entry});
        CHECK(product_node.ok);
        // The stored node key maps the original {app,0x25a4d0,0,0} to the native adapter:
        // receiver = context, invoke = native function, adjust = 0, fn = 0 (native tag).
        CHECK(product_node.node->obj == reinterpret_cast<std::uintptr_t>(&product));
        CHECK(product_node.node->invoke ==
              reinterpret_cast<std::uintptr_t>(install_app_info_product_entry));
        CHECK(product_node.node->offset == 0);
        CHECK(product_node.node->fn == 0);
        CHECK(owner.add_delegate({&game_source, install_app_info_game_entry_delegate}).ok);
        const auto dispatched = owner.dispatch();
        CHECK(dispatched.ok);
        CHECK(dispatched.called == 2);
        CHECK(product.calls == 1);
        CHECK(product.last.ok);
        CHECK(product.last.inserted == 4);
        CHECK(game_source.calls == 1);
        CHECK(game_source.last.ok);
        CHECK(owner.registry().entry_count() == 5);

        // Product entries were inserted first, Game last; push-front reverses insertion.
        const std::vector<std::string_view> names = entry_names(owner.registry());
        CHECK(names.size() == 5);
        CHECK(names[0] == kAppInfoGameName);
        CHECK(names[1] == kAppInfoCameraName);
        CHECK(names[2] == kAppInfoDebugMemoryName);
        CHECK(names[3] == kAppInfoPlatformName);
        CHECK(names[4] == kAppInfoBuildVersionName);

        // The observed flag=1 purge erases nothing.
        ListDtor1bdeb0Hooks dtor_hooks{};
        dtor_hooks.free_tag2 = entry_free_tag2;
        const auto skipped = owner.registry().clear(dtor_hooks);
        CHECK(skipped.ok);
        CHECK(skipped.skipped == 5);
        CHECK(skipped.erased == 0);
        free_entry_nodes(owner.registry());
    }

    // ---- 2b. Real 0x1290 registration into the same list: its identity-only handler node is
    //          skipped by the native dispatcher instead of calling an original RVA. ----
    {
        g_entries = 0;
        g_entry_frees = 0;
        RegistryOwner owner(delegate_alloc);
        AppInfoProductDelegate product{};
        product.values.build_version = "b";
        product.insert = entry_hooks;
        CHECK(owner.add_delegate({&product, install_app_info_product_entry}).ok);

        Init1290Hooks init_hooks{};
        init_hooks.vcall_1594060_slot3 = noop_vcall;
        init_hooks.insert_21cd0 = noop_insert_21cd0;
        init_hooks.container_a = &owner.registry().container();
        init_hooks.alloc_a = delegate_alloc;
        int self = 0;
        const Init1290Result init = init_1290(&self, init_hooks);
        CHECK(init.ok);
        CHECK(init.insert_a_executed);
        CHECK(owner.delegate_count() == 2);

        const auto dispatched = owner.dispatch();
        CHECK(dispatched.ok);
        CHECK(dispatched.called == 1);
        CHECK(dispatched.skipped_identity == 1);
        CHECK(owner.registry().entry_count() == 3);
        free_entry_nodes(owner.registry());
    }

    // ---- 3. A null native fn stops the walk with a visible failure. ----
    {
        RegistryOwner owner(delegate_alloc);
        std::vector<int> log;
        CHECK(owner.add_delegate({&log, record_delegate}).ok);
        CHECK(owner.add_delegate({&log, nullptr}).ok);
        const auto dispatched = owner.dispatch();
        CHECK(!dispatched.ok);
        CHECK(dispatched.called == 1);
        CHECK(dispatched.failed_node != nullptr);
        CHECK(log.size() == 1);
    }

    // ---- 4. Missing allocator: add fails visibly and leaves the list untouched. ----
    {
        RegistryOwner owner(Insert1beeb0Alloc{});
        std::vector<int> log;
        const auto added = owner.add_delegate({&log, record_delegate});
        CHECK(!added.ok);
        CHECK(added.node == nullptr);
        CHECK(owner.delegate_count() == 0);
        CHECK(owner.dispatch().ok);
    }

    return 0;
}
