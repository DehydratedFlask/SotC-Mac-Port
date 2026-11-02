// Fixture for CPU-01-INIT-21620-LOOKUP-01 (`0x21620` = bpe::ConCmdManager::FindConCmd).
//
// SYNTHETIC ONLY. Every name used here ("alpha", "Beta", ... "demo_alpha") is invented by the test.
// The two pinned retail descriptors (the 5-byte "r_fps" and the 14-byte debug-mode name at
// 0x1014ca3) are deliberately NOT looked up, so no result from this file can be read as "the engine
// finds X". Nothing here is a boot, audio, save or engine-readiness claim.
//
// Covered: both paths of the body, the case-insensitive comparator, the length-sensitive fallback,
// the length-blind manager map, one lock/unlock per primary call and none on the fallback, lock
// refusal failing closed, a null key failing closed, and the two retail call-site adapters driven
// through the existing modules (lazy_bind_91210 @ 0x91227 and vslot_40_533100_miss @ 0x53327d).

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/lazy_bind_91210.hpp"
#include "core/lookup_21620.hpp"
#include "core/vslot_40_533100_miss.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

using namespace sotc;

// ---- host objects laid out at the pinned offsets ------------------------------------------------
struct alignas(8) ManagerBuffer {
    std::uint8_t bytes[kLookup21620ManagerBytes]{};
};

struct alignas(8) NodeBuffer {
    std::uint8_t bytes[kLookup21620NodeBytes]{};
};

void store_ptr(void* base, std::size_t offset, void* value) {
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof(value));
}

void store_u64(void* base, std::size_t offset, std::uint64_t value) {
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof(value));
}

// A caller-side bpe::String over a literal: {data, size, aux = 0, ctl = 0} (both retail sites zero
// the last two fields).
SharedStr key_of(const char* text, std::uint64_t size_override = UINT64_MAX) {
    SharedStr key{};
    key.data = reinterpret_cast<const std::uint8_t*>(text);
    key.size = static_cast<std::int64_t>(size_override == UINT64_MAX ? std::strlen(text) : size_override);
    key.aux = 0;
    key.ctl = nullptr;
    return key;
}

// Fill a map node with the pinned key SharedStr at +0x20 and the value at +0x40.
void fill_node(NodeBuffer& node, const char* name, void* value) {
    const SharedStr key = key_of(name);
    std::memcpy(node.bytes + kLookup21620NodeKeyOffset, &key, sizeof(key));
    store_ptr(node.bytes, kLookup21620NodeValueOffset, value);
}

// The manager object as 0x210b0 builds it, minus the pending-list drain: an empty map (root null)
// and a live CritSec handle at +0x58. Returns false if the handle could not be initialised.
bool init_manager(ManagerBuffer& manager) {
    store_u64(manager.bytes, kLookup21620ManagerRootOffset, 0);
    return critsec_init_at(manager.bytes + kLookup21620ManagerMutexOffset) == 0;
}

// Descending ordering, as both the lookup (0x216c1) and the writer (0x20d80/0x211d0) descend:
// a smaller name sits in a greater name's right subtree.
//        d
//      /   \
//     f     b
//    / \   / \
//   g   e c   a
struct Tree {
    NodeBuffer d, f, b, g, e, c, a;
    void* values[7]{};

    Tree() {
        for (int i = 0; i < 7; ++i) {
            values[i] = reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x1000 + i));
        }
        fill_node(d, "d", values[0]);
        fill_node(f, "f", values[1]);
        fill_node(b, "b", values[2]);
        fill_node(g, "g", values[3]);
        fill_node(e, "e", values[4]);
        fill_node(c, "c", values[5]);
        fill_node(a, "a", values[6]);
        link(d, f.bytes, b.bytes);
        link(f, g.bytes, e.bytes);
        link(b, c.bytes, a.bytes);
    }

    static void link(NodeBuffer& parent, void* left, void* right) {
        store_ptr(parent.bytes, kLookup21620NodeLeftOffset, left);
        store_ptr(parent.bytes, kLookup21620NodeRightOffset, right);
    }

    void* root() { return d.bytes; }
};

struct CountingHooks {
    int locks = 0;
    int unlocks = 0;
    int compares = 0;
    int reject_lock_with = 0;

    FindConCmdHooks hooks() {
        FindConCmdHooks h{};
        h.host = this;
        h.compare = &CountingHooks::compare;
        h.lock = &CountingHooks::lock;
        h.unlock = &CountingHooks::unlock;
        return h;
    }

    void reset() {
        locks = unlocks = compares = 0;
    }

    static int compare(void* host, const char* lhs, const char* rhs) {
        ++static_cast<CountingHooks*>(host)->compares;
        return concmd_strcasecmp(lhs, rhs);
    }
    static int lock(void* host, void* handle) {
        auto* self = static_cast<CountingHooks*>(host);
        if (self->reject_lock_with != 0) {
            return self->reject_lock_with;
        }
        ++self->locks;
        return critsec_lock_at(handle);
    }
    static int unlock(void* host, void* handle) {
        ++static_cast<CountingHooks*>(host)->unlocks;
        return critsec_unlock_at(handle);
    }
};

}  // namespace

int main() {
    // ---- comparator shape ---------------------------------------------------------------------
    CHECK(concmd_strcasecmp("alpha", "alpha") == 0);
    CHECK(concmd_strcasecmp("ALPHA", "alpha") == 0);
    CHECK(concmd_strcasecmp("alpha", "alphb") < 0);
    CHECK(concmd_strcasecmp("b", "a") > 0);
    CHECK(concmd_strcasecmp("", "") == 0);

    // ---- pending-list (fallback) path ----------------------------------------------------------
    {
        ConCmdRegistry registry{};
        CHECK(registry.manager.manager == nullptr);

        const SharedStr alpha = key_of("alpha");
        const auto first = find_concmd_21620(alpha, registry);
        CHECK(first.status == FindConCmdStatus::miss);
        CHECK(first.path == FindConCmdPath::pending_list);
        CHECK(first.list_initialised);   // 0x2171b..0x21755 ran
        CHECK(!first.locked);            // no lock on this path
        CHECK(registry.pending.head.next == &registry.pending.head);

        const auto second = find_concmd_21620(alpha, registry);
        CHECK(second.status == FindConCmdStatus::miss);
        CHECK(!second.list_initialised);  // the guard byte is now set

        // Two registrations through the real register body, which also owns the same guard.
        ConCmdBase one{};
        ConCmdBase two{};
        const SharedStr one_name = key_of("alpha");
        const SharedStr two_name = key_of("Beta");
        init_concmd_base_20af0(one, one_name, SharedStr{}, 1);
        init_concmd_base_20af0(two, two_name, SharedStr{}, 1);
        CHECK(register_concmd_20d60(one, registry).ok);
        CHECK(register_concmd_20d60(two, registry).ok);

        const auto hit_one = find_concmd_21620(key_of("alpha"), registry);
        CHECK(hit_one.status == FindConCmdStatus::hit && hit_one.cmd == &one);
        CHECK(hit_one.path == FindConCmdPath::pending_list);

        // Case-insensitive on the name, but the length is part of the pending-list key.
        CHECK(find_concmd_21620(key_of("ALPHA"), registry).cmd == &one);
        CHECK(find_concmd_21620(key_of("alpha", 6), registry).status == FindConCmdStatus::miss);
        CHECK(find_concmd_21620(key_of("beta"), registry).cmd == &two);
        CHECK(find_concmd_21620(key_of("gamma"), registry).status == FindConCmdStatus::miss);

        // A rejected compare never runs when the length already differs (0x21790 precedes 0x2179d).
        CountingHooks counting{};
        const auto before = find_concmd_21620(key_of("alpha", 99), registry, counting.hooks());
        CHECK(before.status == FindConCmdStatus::miss);
        CHECK(counting.compares == 0);
        CHECK(counting.locks == 0 && counting.unlocks == 0);

        // A key without a name cannot be compared: fail closed, no path taken.
        SharedStr empty{};
        const auto bad_key = find_concmd_21620(empty, registry);
        CHECK(bad_key.status == FindConCmdStatus::failed);
        CHECK(bad_key.path == FindConCmdPath::none);
    }

    // ---- manager-map (primary) path ------------------------------------------------------------
    {
        ConCmdRegistry registry{};
        ManagerBuffer manager{};
        CHECK(init_manager(manager));
        Tree tree;
        store_ptr(manager.bytes, kLookup21620ManagerRootOffset, tree.root());
        registry.manager.manager = manager.bytes;

        CountingHooks counting{};

        const char* const names[7] = {"d", "f", "b", "g", "e", "c", "a"};
        for (int i = 0; i < 7; ++i) {
            counting.reset();
            const auto found = find_concmd_21620(key_of(names[i]), registry, counting.hooks());
            CHECK(found.status == FindConCmdStatus::hit);
            CHECK(found.path == FindConCmdPath::manager_map);
            CHECK(found.cmd == static_cast<ConCmdBase*>(tree.values[i]));
            CHECK(found.locked);
            CHECK(counting.locks == 1 && counting.unlocks == 1);
        }

        // Misses still unlock exactly once, and leave no result.
        counting.reset();
        const auto above = find_concmd_21620(key_of("x"), registry, counting.hooks());
        CHECK(above.status == FindConCmdStatus::miss && above.cmd == nullptr);
        CHECK(above.locked && counting.locks == 1 && counting.unlocks == 1);

        // "cc" traverses to the same candidate as "c" but is rejected by 0x216f3.
        counting.reset();
        const auto between = find_concmd_21620(key_of("cc"), registry, counting.hooks());
        CHECK(between.status == FindConCmdStatus::miss);
        CHECK(counting.unlocks == 1);

        // The manager path never reads the key length (0x216b3 reads only +0x00).
        const auto blind = find_concmd_21620(key_of("d", 99), registry, counting.hooks());
        CHECK(blind.status == FindConCmdStatus::hit && blind.cmd == static_cast<ConCmdBase*>(tree.values[0]));

        // An empty map (root null) falls straight through to the miss tail.
        store_u64(manager.bytes, kLookup21620ManagerRootOffset, 0);
        counting.reset();
        const auto empty_map = find_concmd_21620(key_of("d"), registry, counting.hooks());
        CHECK(empty_map.status == FindConCmdStatus::miss);
        CHECK(counting.unlocks == 1 && counting.compares == 0);

        // A refused lock fails closed: the traversal never runs.
        store_ptr(manager.bytes, kLookup21620ManagerRootOffset, tree.root());
        counting.reset();
        counting.reject_lock_with = 16;  // EBUSY
        const auto refused = find_concmd_21620(key_of("d"), registry, counting.hooks());
        CHECK(refused.status == FindConCmdStatus::failed);
        CHECK(!refused.locked);
        CHECK(refused.lock_error == 16);
        CHECK(counting.compares == 0);
        CHECK(counting.unlocks == 0);

        CHECK(critsec_destroy_at(manager.bytes + kLookup21620ManagerMutexOffset) == 0);
    }

    // ---- adapters ------------------------------------------------------------------------------
    {
        ConCmdRegistry registry{};
        ManagerBuffer manager{};
        CHECK(init_manager(manager));
        registry.manager.manager = nullptr;  // placeholder: registration runs on the pending list

        ConCmdBase cmd{};
        SharedStr name = key_of("demo_alpha");  // 10 characters, synthetic
        init_concmd_base_20af0(cmd, name, SharedStr{}, 1);
        // Registered while the manager cell is null, so the entry lands on the pending list
        // (0x20e7f) — the register body's manager path is a host contract with no default.
        CHECK(registry.manager.manager == nullptr);
        CHECK(register_concmd_20d60(cmd, registry).ok);

        // 0x53327d shape: {data, size} in caller memory, no host parameter.
        CHECK(!concmd_registry_installed());
        CHECK(find_concmd_descriptor(nullptr) == nullptr);
        CHECK(install_concmd_registry(registry));
        CHECK(concmd_registry_installed());
        CHECK(find_concmd_descriptor(&name) == &cmd);
        CHECK(find_concmd_descriptor(nullptr) == nullptr);
        SharedStr same_size_other_case = key_of("demo_alpHa", 10);  // case-insensitive -> hit
        CHECK(find_concmd_descriptor(&same_size_other_case) == &cmd);
        SharedStr other_size = key_of("demo_alpha", 11);
        CHECK(find_concmd_descriptor(&other_size) == nullptr);
        uninstall_concmd_registry();
        CHECK(!concmd_registry_installed());
        CHECK(find_concmd_descriptor(&name) == nullptr);

        // 0x91227 shape: host = FindConCmdContext*.
        FindConCmdContext context{&registry, FindConCmdHooks{}};
        CHECK(find_concmd_descriptor_hosted(&context, &name) == &cmd);
        CHECK(find_concmd_descriptor_hosted(nullptr, &name) == nullptr);
        CHECK(find_concmd_descriptor_hosted(&context, nullptr) == nullptr);

        // Through the real lazy-bind module: the descriptor sits at object+8 (0x91223).
        struct alignas(8) BindObject {
            std::uint8_t gap00[0x08]{};
            SharedStr key{};    // +0x08
            void* bound{};      // +0x28
            std::uint8_t gap30[0x10]{};
        };
        static_assert(offsetof(BindObject, key) == 0x08);
        static_assert(offsetof(BindObject, bound) == kLazyBind91210BoundOffset);
        static_assert(sizeof(BindObject) == 0x40);

        BindObject object{};
        object.key = name;
        CHECK(object.bound == nullptr);
        LazyBind91210Hooks bind{};
        bind.lookup_21620 = &find_concmd_descriptor_hosted;
        bind.vcall_18 = [](void*, void* sub) { return sub == nullptr ? 0 : 1; };
        bind.vcall_30 = [](void*, void*) { return 0; };
        bind.host = &context;
        const auto bound = run_lazy_bind_91210(&object, bind);
        CHECK(bound.ok && bound.stored && !bound.rolled_back && bound.returned == 1);
        CHECK(object.bound == &cmd);

        // Through the real vslot-miss module: its stand-in block carries the pinned {name, 5} key.
        CHECK(install_concmd_registry(registry));
        Vslot40_533100MissHooks miss{};
        miss.find_21620 = &find_concmd_descriptor;
        miss.vslot_18 = [](void* obj) { return obj == nullptr ? 0 : 1; };
        miss.vslot_30 = [](void*) { return 0; };
        const auto miss_result = vslot_40_533100_miss(miss);
        CHECK(miss_result.find_21620_calls == 1);
        uninstall_concmd_registry();  // fail closed again

        // With the manager cell set, the same key now takes the locked map path and misses, because
        // this fixture's manager owns an empty map: the pending list is not consulted.
        registry.manager.manager = manager.bytes;
        FindConCmdContext map_context{&registry, FindConCmdHooks{}};
        CHECK(find_concmd_descriptor_hosted(&map_context, &name) == nullptr);
        registry.manager.manager = nullptr;
        CHECK(find_concmd_descriptor_hosted(&map_context, &name) == &cmd);

        CHECK(critsec_destroy_at(manager.bytes + kLookup21620ManagerMutexOffset) == 0);
    }

    std::printf("OK lookup_21620 (FindConCmd): both paths, case-insensitive names, length-sensitive "
                "pending list, one lock/unlock per manager call, fail-closed lock and null key\n");
    return 0;
}
