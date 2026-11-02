// Fixture for CPU-01-CONCMD-MANAGER-210B0-01 (`0x210b0`, the ConCmd manager constructor + pending drain).
//
// SYNTHETIC ONLY. Every command name here ("alpha", "Beta", "gamma", "delta", "epsilon",
// "demo_alpha") is invented by the test; neither pinned retail descriptor is built or looked up, so no
// result from this file can be read as "the engine has command X". Nothing here is a boot, audio,
// save, engine-readiness or "the registry is populated at run time" claim.
//
// The map the drain inserts into is a FIXTURE-LOCAL stand-in for the unlifted writer `0x211d0`: one
// 0x48-byte node per command in the layout `0x21620` reads (left +0x00, right +0x08, key +0x20,
// value +0x40). It is not a model of `0x211d0`, of a red-black tree, or of what the retail container
// stores.
//
// Sections:
//   1. layout constants and static offsets
//   2. the constructor's write set over a poisoned 0x60-byte block
//   3. the FIFO drain: order, guard bypass, insert argument, re-emptied head, untouched nodes
//   4. idempotence: a second construction drains nothing; a later registration still queues
//   5. fail-visible cases: null block, refused CritSec, missing insert, three corrupt links
//   6. the general re-termination form (a first element whose prev points at the last node)
//   7. end to end: queue -> drain -> FindConCmd hit through the native `0x21620`
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/concmd_base.hpp"
#include "core/concmd_manager_210b0.hpp"
#include "core/lookup_21620.hpp"

#define CHECK(c)                                                                                   \
    do {                                                                                           \
        if (!(c)) {                                                                                \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                       \
            std::exit(1);                                                                          \
        }                                                                                          \
    } while (0)

namespace {

using namespace sotc;

constexpr std::uint8_t kPoison = 0xCD;

SharedStr key_of(const char* text) {
    SharedStr key{};
    key.data = reinterpret_cast<const std::uint8_t*>(text);
    key.size = static_cast<std::int64_t>(std::strlen(text));
    key.aux = 0;
    key.ctl = nullptr;
    return key;
}

// A command that is fully built but not yet registered.
void make_command(ConCmdBase& cmd, const char* name) {
    std::memset(static_cast<void*>(&cmd), 0, sizeof cmd);
    init_concmd_base_20af0(cmd, key_of(name), SharedStr{}, 1);
}

bool poison_window_intact(const ConCmdManagerStorage& storage, std::size_t offset, std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
        if (storage.bytes[offset + i] != kPoison) return false;
    }
    return true;
}

bool window_is_zero(const ConCmdManagerStorage& storage, std::size_t offset, std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
        if (storage.bytes[offset + i] != 0) return false;
    }
    return true;
}

// ---- fixture-local map stand-in (NOT a model of 0x211d0) ---------------------------------------
struct FixtureNode {
    FixtureNode* left;        // +0x00
    FixtureNode* right;       // +0x08
    std::uint8_t pad[0x10];   // +0x10 (the writer 0x211d0 fills the RB fields; the lookup never reads them)
    SharedStr key;            // +0x20
    void* value;              // +0x40
};
static_assert(sizeof(FixtureNode) == 0x48);
static_assert(offsetof(FixtureNode, key) == kLookup21620NodeKeyOffset);
static_assert(offsetof(FixtureNode, value) == kLookup21620NodeValueOffset);

struct FixtureMap {
    FixtureNode nodes[8]{};
    int count = 0;
    void* expected_manager = nullptr;
    ConCmdRegistry* registry = nullptr;
    int inserts = 0;
    bool manager_seen = false;
    bool cell_seen_null = true;
    ConCmdBase* order[8]{};

    // Descending ordering, exactly the polarity 0x21213/0x2121f and 0x216b3/0x216c1 use: a smaller
    // name lies in a greater name's right subtree.
    bool insert(ConCmdBase& cmd) {
        if (count >= 8) return false;
        FixtureNode& node = nodes[count];
        node.key = cmd.name;
        node.value = &cmd;
        auto* slot = reinterpret_cast<FixtureNode**>(static_cast<std::uint8_t*>(expected_manager) +
                                                    kConCmdManager210b0RootOffset);
        while (*slot != nullptr) {
            auto* existing = static_cast<FixtureNode*>(*slot);
            const char* existing_name = reinterpret_cast<const char*>(existing->key.data);
            const char* new_name = reinterpret_cast<const char*>(node.key.data);
            slot = concmd_strcasecmp(existing_name, new_name) < 0 ? &existing->left : &existing->right;
        }
        *slot = &node;
        order[count] = &cmd;
        ++count;
        ++inserts;
        return true;
    }
};

FixtureMap* g_map = nullptr;

bool fixture_insert(void* manager, ConCmdBase& cmd) noexcept {
    if (g_map == nullptr || manager != g_map->expected_manager) return false;
    g_map->manager_seen = true;
    if (g_map->registry != nullptr) {
        g_map->cell_seen_null = g_map->cell_seen_null && g_map->registry->manager.manager == nullptr;
    }
    return g_map->insert(cmd);
}

// Records the drain order without building a map (sections 3-6).
struct DrainLog {
    ConCmdBase* order[8]{};
    int count = 0;
    bool cell_seen_null = true;
    bool manager_matched = true;
    ConCmdRegistry* registry = nullptr;
    void* expected_manager = nullptr;
    bool accept = true;

    ConCmdManagerCtorHooks hooks() {
        ConCmdManagerCtorHooks h{};
        h.host = this;
        h.register_body = [](void* host, ConCmdBase& cmd, void* manager,
                             ConCmdRegistry& registry) noexcept -> ConCmdRegisterResult {
            auto* self = static_cast<DrainLog*>(host);
            if (self->count < 8) self->order[self->count] = &cmd;
            ++self->count;
            self->manager_matched = self->manager_matched && manager == self->expected_manager;
            self->cell_seen_null = self->cell_seen_null && registry.manager.manager == nullptr;
            ConCmdRegisterResult out{};
            out.ok = self->accept;
            out.path = ConCmdRegisterPath::manager;
            return out;
        };
        return h;
    }
};

int refuse_critsec(void*, void*) noexcept { return 16; }  // EBUSY

}  // namespace

int main() {
    // ---- 1. layout constants ----------------------------------------------------------------
    static_assert(kConCmdManager210b0StorageBytes == 0x60);
    static_assert(kConCmdManager210b0RootOffset == 0x18);
    static_assert(kConCmdManager210b0MutexOffset == 0x58);
    static_assert(kConCmdManager210b0SentinelDelta == -8);
    static_assert(offsetof(ConCmdBase, pending) == kConCmdManager210b0SelfLinkAOffset);
    static_assert(offsetof(ConCmdBase, name) == 0x20);

    CHECK(concmd_manager_qword(nullptr, 0x18) == 0);

    // ---- 2. the constructor's write set over a poisoned block --------------------------------
    {
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        std::memset(storage.bytes, kPoison, sizeof storage.bytes);

        const auto created = construct_concmd_manager_210b0(storage.address(), registry);
        CHECK(created.ok);
        CHECK(created.fail == ConCmdManagerCtorFail::none);
        CHECK(created.drained == 0);
        CHECK(created.register_failures == 0);
        CHECK(created.mutex_error == 0);
        CHECK(created.list_initialised);   // 0x210f7..0x21135 ran the guarded init
        CHECK(created.list_emptied);       // head == {&head, &head}

        const auto self = reinterpret_cast<std::uint64_t>(storage.bytes) +
                          kConCmdManager210b0SelfLinkAOffset;
        CHECK(concmd_manager_qword(storage.bytes, kConCmdManager210b0SelfLinkAOffset) == self);
        CHECK(concmd_manager_qword(storage.bytes, kConCmdManager210b0SelfLinkBOffset) == self);
        CHECK(concmd_manager_qword(storage.bytes, kConCmdManager210b0RootOffset) == 0);
        CHECK(window_is_zero(storage, 0x20, 0x10));   // 0x210d9 + the redundant 0x210df byte store
        CHECK(window_is_zero(storage, 0x38, 0x10));   // 0x210e4
        CHECK(window_is_zero(storage, 0x48, 0x08));   // 0x210ea
        // The body writes nothing else: these three windows keep the poison.
        CHECK(poison_window_intact(storage, 0x00, 0x08));
        CHECK(poison_window_intact(storage, 0x30, 0x08));
        CHECK(poison_window_intact(storage, 0x50, 0x08));
        CHECK(registry.pending.head.next == &registry.pending.head);
        CHECK(registry.pending.head.prev == &registry.pending.head);

        // The 8-byte handle at +0x58 is a live CritSec, as 0x210f2 built it.
        void* const handle = storage.bytes + kConCmdManager210b0MutexOffset;
        CHECK(critsec_lock_at(handle) == 0);
        CHECK(critsec_unlock_at(handle) == 0);
        CHECK(critsec_destroy_at(handle) == 0);
    }

    // ---- 3. the FIFO drain ------------------------------------------------------------------
    {
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        ConCmdBase alpha{};
        ConCmdBase beta{};
        ConCmdBase gamma{};
        make_command(alpha, "alpha");
        make_command(beta, "Beta");
        make_command(gamma, "gamma");

        // Startup order: the cell is still null, so each command queues itself (0x20e7f).
        CHECK(register_concmd_20d60(alpha, registry).ok);
        CHECK(register_concmd_20d60(beta, registry).ok);
        CHECK(register_concmd_20d60(gamma, registry).ok);
        CHECK(registry.pending.size() == 3);
        CHECK(alpha.registered == 1 && beta.registered == 1 && gamma.registered == 1);

        DrainLog log{};
        log.registry = &registry;
        log.expected_manager = storage.address();
        const auto created = construct_concmd_manager_210b0(storage.address(), registry, log.hooks());

        CHECK(created.ok);
        CHECK(created.fail == ConCmdManagerCtorFail::none);
        CHECK(created.drained == 3);
        CHECK(created.register_failures == 0);
        CHECK(log.count == 3);
        // Queue order, oldest first: 0x21136 starts at head.next and follows element->next.
        CHECK(log.order[0] == &alpha && log.order[1] == &beta && log.order[2] == &gamma);
        // 0x20d80's argument is the manager under construction, and the cell is still null (0x243f5
        // stores it only after 0x210b0 returns) — a cell-reading drain would re-queue everything.
        CHECK(log.manager_matched);
        CHECK(log.cell_seen_null);
        // `registered == 1` on every drained command proves the drain bypassed 0x20d60's guard.
        CHECK(alpha.registered == 1);

        CHECK(created.list_emptied);
        CHECK(registry.pending.head.next == &registry.pending.head);
        CHECK(registry.pending.head.prev == &registry.pending.head);
        CHECK(registry.pending.size() == 0);
        // The body never unlinks the nodes: only the head's two links were rewritten.
        CHECK(alpha.pending.prev == &registry.pending.head);
        CHECK(alpha.pending.next == &beta.pending);
        CHECK(beta.pending.prev == &alpha.pending);
        CHECK(beta.pending.next == &gamma.pending);
        CHECK(gamma.pending.next == &registry.pending.head);

        // ---- 4. a second construction finds nothing to drain ---------------------------------
        // The installer always allocates a fresh 0x60-byte block (0x243a1), so each construction
        // below gets its own object; the registry is what is shared.
        CHECK(critsec_destroy_at(storage.bytes + kConCmdManager210b0MutexOffset) == 0);

        ConCmdManagerStorage storage2{};
        DrainLog second{};
        second.registry = &registry;
        second.expected_manager = storage2.address();
        const auto again =
            construct_concmd_manager_210b0(storage2.address(), registry, second.hooks());
        CHECK(again.ok);
        CHECK(again.drained == 0);
        CHECK(second.count == 0);
        CHECK(!again.list_initialised);   // the guard byte is already set
        CHECK(again.list_emptied);
        CHECK(critsec_destroy_at(storage2.bytes + kConCmdManager210b0MutexOffset) == 0);

        // A command registered afterwards still queues and is drained by a later construction.
        ConCmdBase delta{};
        make_command(delta, "delta");
        CHECK(register_concmd_20d60(delta, registry).ok);
        CHECK(registry.pending.size() == 1);
        ConCmdManagerStorage storage3{};
        DrainLog third{};
        third.registry = &registry;
        third.expected_manager = storage3.address();
        const auto after =
            construct_concmd_manager_210b0(storage3.address(), registry, third.hooks());
        CHECK(after.ok && after.drained == 1 && third.order[0] == &delta);
        CHECK(registry.pending.size() == 0);

        CHECK(critsec_destroy_at(storage3.bytes + kConCmdManager210b0MutexOffset) == 0);
    }

    // ---- 5. fail-visible cases ---------------------------------------------------------------
    {
        ConCmdRegistry registry{};

        // 5a. no object memory: retail always has the `new(0x60)` block.
        const auto null_block = construct_concmd_manager_210b0(nullptr, registry);
        CHECK(!null_block.ok);
        CHECK(null_block.fail == ConCmdManagerCtorFail::null_manager);
        CHECK(null_block.drained == 0);
    }
    {
        // 5b. a refused CritSec handle is reported, and the drain still runs (retail continues).
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        ConCmdBase one{};
        make_command(one, "alpha");
        CHECK(register_concmd_20d60(one, registry).ok);

        DrainLog log{};
        log.registry = &registry;
        log.expected_manager = storage.address();
        ConCmdManagerCtorHooks hooks = log.hooks();
        hooks.critsec_init = &refuse_critsec;
        const auto created = construct_concmd_manager_210b0(storage.address(), registry, hooks);
        CHECK(!created.ok);
        CHECK(created.fail == ConCmdManagerCtorFail::mutex_init);
        CHECK(created.mutex_error == 16);
        CHECK(created.drained == 1);   // the drain is not skipped
        CHECK(registry.pending.size() == 0);
        // The handle was never filled, so it must not be destroyed.
    }
    {
        // 5c. a queue with no insert contract: every register call fails visibly.
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        ConCmdBase one{};
        ConCmdBase two{};
        make_command(one, "alpha");
        make_command(two, "Beta");
        CHECK(register_concmd_20d60(one, registry).ok);
        CHECK(register_concmd_20d60(two, registry).ok);
        CHECK(registry.manager.insert == nullptr);

        const auto created = construct_concmd_manager_210b0(storage.address(), registry);
        CHECK(!created.ok);
        CHECK(created.fail == ConCmdManagerCtorFail::register_refused);
        CHECK(created.drained == 2);
        CHECK(created.register_failures == 2);
        CHECK(created.list_emptied);   // the drain still ran and re-emptied the head
        CHECK(critsec_destroy_at(storage.bytes + kConCmdManager210b0MutexOffset) == 0);
    }
    {
        // 5d. corrupt head: head.next == null (0x21136 would fall into Register(nullptr, manager)).
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        registry.pending.guard_done = true;
        registry.pending.head.next = nullptr;
        registry.pending.head.prev = &registry.pending.head;

        const auto created = construct_concmd_manager_210b0(storage.address(), registry);
        CHECK(!created.ok);
        CHECK(created.fail == ConCmdManagerCtorFail::null_head_link);
        CHECK(created.drained == 0);
        CHECK(critsec_destroy_at(storage.bytes + kConCmdManager210b0MutexOffset) == 0);
    }
    {
        // 5e. a queued element whose forward link is null.
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        ConCmdBase one{};
        make_command(one, "alpha");
        registry.pending.guard_done = true;
        registry.pending.head.next = &one.pending;
        registry.pending.head.prev = &one.pending;
        one.pending.next = nullptr;
        one.pending.prev = &registry.pending.head;

        DrainLog log{};
        log.registry = &registry;
        log.expected_manager = storage.address();
        const auto created = construct_concmd_manager_210b0(storage.address(), registry, log.hooks());
        CHECK(!created.ok);
        CHECK(created.fail == ConCmdManagerCtorFail::null_drained_link);
        CHECK(created.drained == 1);   // the first element was still registered
        CHECK(critsec_destroy_at(storage.bytes + kConCmdManager210b0MutexOffset) == 0);
    }
    {
        // 5f. null predecessor link in the re-termination block (0x21198 -> store at null+8).
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        ConCmdBase one{};
        make_command(one, "alpha");
        registry.pending.guard_done = true;
        registry.pending.head.next = &one.pending;
        registry.pending.head.prev = &one.pending;
        one.pending.next = &registry.pending.head;
        one.pending.prev = nullptr;

        DrainLog log{};
        log.registry = &registry;
        log.expected_manager = storage.address();
        const auto created = construct_concmd_manager_210b0(storage.address(), registry, log.hooks());
        CHECK(!created.ok);
        CHECK(created.fail == ConCmdManagerCtorFail::null_tail_link);
        CHECK(created.drained == 1);
        CHECK(critsec_destroy_at(storage.bytes + kConCmdManager210b0MutexOffset) == 0);
    }

    // ---- 6. the general re-termination form --------------------------------------------------
    // A list whose first element's prev points at the last node: 0x21198 then yields the last
    // element, so 0x211a7 only re-writes last->next and head.prev and the list is NOT emptied. That
    // is the byte consequence of the pinned block; no 0x20d80-built list has this shape.
    {
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        ConCmdBase one{};
        ConCmdBase two{};
        make_command(one, "alpha");
        make_command(two, "Beta");
        registry.pending.guard_done = true;
        registry.pending.head.next = &one.pending;
        registry.pending.head.prev = &two.pending;
        one.pending.next = &two.pending;
        one.pending.prev = &two.pending;   // first->prev = last (not the head link)
        two.pending.next = &registry.pending.head;
        two.pending.prev = &one.pending;

        DrainLog log{};
        log.registry = &registry;
        log.expected_manager = storage.address();
        const auto created = construct_concmd_manager_210b0(storage.address(), registry, log.hooks());
        CHECK(created.ok);
        CHECK(created.drained == 2);
        CHECK(log.order[0] == &one && log.order[1] == &two);
        CHECK(!created.list_emptied);
        CHECK(registry.pending.head.next == &one.pending);   // unchanged
        CHECK(registry.pending.head.prev == &two.pending);   // unchanged
        CHECK(critsec_destroy_at(storage.bytes + kConCmdManager210b0MutexOffset) == 0);
    }

    // ---- 6b. the two re-termination stores are observable ------------------------------------
    // The first element's prev names a node that is NOT in the drained chain. 0x21198 yields it,
    // 0x211a7 must write `[tail + 8] = &head` and 0x211ab/0x211af must write `head.prev = tail link`.
    // The tail starts with a poisoned next and the head with a stale prev, so omitting either store
    // (or storing the head link instead of the tail link) is visible. This is the byte consequence of
    // the pinned block, not a shape any 0x20d80-built list has.
    {
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        ConCmdBase one{};
        ConCmdBase outside{};
        make_command(one, "alpha");
        make_command(outside, "Gamma");
        registry.pending.guard_done = true;
        registry.pending.head.next = &one.pending;
        registry.pending.head.prev = &one.pending;      // stale: the block must overwrite it
        one.pending.next = &registry.pending.head;
        one.pending.prev = &outside.pending;            // first->prev = a node outside the chain
        outside.pending.next = &outside.pending;        // poison: must become &head
        outside.pending.prev = &outside.pending;

        DrainLog log{};
        log.registry = &registry;
        log.expected_manager = storage.address();
        const auto created = construct_concmd_manager_210b0(storage.address(), registry, log.hooks());
        CHECK(created.ok);
        CHECK(created.drained == 1);
        CHECK(!created.list_emptied);
        CHECK(outside.pending.next == &registry.pending.head);   // 0x211a7: [tail+8] = &head
        CHECK(registry.pending.head.prev == &outside.pending);   // 0x211ab/0x211af: head.prev = tail link
        CHECK(registry.pending.head.next == &one.pending);       // head.next is not rewritten here
        CHECK(critsec_destroy_at(storage.bytes + kConCmdManager210b0MutexOffset) == 0);
    }

    // ---- 7. end to end: queue -> drain -> FindConCmd hit -------------------------------------
    {
        ConCmdRegistry registry{};
        ConCmdManagerStorage storage{};
        ConCmdBase alpha{};
        ConCmdBase beta{};
        make_command(alpha, "alpha");
        make_command(beta, "Beta");
        CHECK(register_concmd_20d60(alpha, registry).ok);
        CHECK(register_concmd_20d60(beta, registry).ok);

        FixtureMap map{};
        g_map = &map;
        map.expected_manager = storage.address();
        map.registry = &registry;
        registry.manager.insert = &fixture_insert;

        const auto created = construct_concmd_manager_210b0(storage.address(), registry);
        CHECK(created.ok);
        CHECK(created.drained == 2);
        CHECK(created.register_failures == 0);
        CHECK(map.inserts == 2);
        CHECK(map.manager_seen);
        CHECK(map.cell_seen_null);   // 0x243f5 installs the cell after 0x210b0 returns
        CHECK(map.order[0] == &alpha && map.order[1] == &beta);

        // 0x243f5: the installer stores the object into the cell afterwards.
        registry.manager.manager = storage.address();

        const auto hit_alpha = find_concmd_21620(key_of("alpha"), registry);
        CHECK(hit_alpha.status == FindConCmdStatus::hit);
        CHECK(hit_alpha.path == FindConCmdPath::manager_map);
        CHECK(hit_alpha.cmd == &alpha);
        const auto hit_beta = find_concmd_21620(key_of("beta"), registry);   // case-insensitive
        CHECK(hit_beta.status == FindConCmdStatus::hit && hit_beta.cmd == &beta);
        const auto miss = find_concmd_21620(key_of("gamma"), registry);
        CHECK(miss.status == FindConCmdStatus::miss);
        CHECK(miss.path == FindConCmdPath::manager_map);   // the drained list is not consulted

        g_map = nullptr;
        registry.manager.insert = nullptr;
        registry.manager.manager = nullptr;
        CHECK(critsec_destroy_at(storage.bytes + kConCmdManager210b0MutexOffset) == 0);
    }

    std::printf("OK concmd_manager_210b0: write set, FIFO drain through the register body with the "
                "cell still null, re-emptied head without unlinking, six fail-visible paths, the "
                "general re-termination form, and a FindConCmd hit after the drain\n");
    return 0;
}
