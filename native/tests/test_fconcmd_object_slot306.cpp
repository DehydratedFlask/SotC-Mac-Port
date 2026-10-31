// CPU-01-SLOT306-CONCMD-OBJECT-01: fixture for the slot-306 console-command object (vptr 0x1594dd0,
// flags 2, destructor 0x4b0, invoker thunk 0x1990, registration through vtable slot 2), its
// destructor chain (0x4b0 -> 0x20b50 -> manager erase 0x213b0) and the behavioural manager model.
//
// Sections:
//   1. constants, layout and the three observed sites
//   2. construction of each site: exact stores, untouched bytes, step order, FIFO pending list
//   3. fail-closed construction (null strings, zero handler, bad index) leaves everything untouched
//   4. handlers are identities only: construction never calls one; the thunk and the bound call
//   5. non-null manager path: insert-if-absent, missing contract
//   6. destructor chain: list unlink, guard, view releases, vector walk, storage free, fail-closed
//   7. manager erase through the destructor (including the key-only erase of a duplicate name)
//   8. whole slot-306 command block: three objects constructed, then destroyed in reverse order
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <utility>
#include <vector>

#include "core/concmd_base.hpp"
#include "core/fconcmd_object_slot306.hpp"

namespace {

int g_checks = 0;

#define CHECK(c)                                                                                  \
    do {                                                                                          \
        ++g_checks;                                                                               \
        if (!(c)) {                                                                               \
            std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c);                      \
            std::exit(1);                                                                         \
        }                                                                                         \
    } while (0)

using namespace sotc;

constexpr std::uint8_t kPoison = 0xA5;

template <class T>
void poison(T& obj) {
    std::memset(static_cast<void*>(&obj), kPoison, sizeof obj);
}

template <class T>
std::uint64_t q(const T& obj, std::size_t off) {
    std::uint64_t v;
    std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(&obj) + off, 8);
    return v;
}

template <class T>
void check_untouched(const T& obj, std::initializer_list<std::pair<std::size_t, std::size_t>> written) {
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(&obj);
    for (std::size_t off = 0; off < sizeof obj; ++off) {
        bool is_written = false;
        for (const auto& [begin, size] : written) {
            if (off >= begin && off < begin + size) {
                is_written = true;
                break;
            }
        }
        if (!is_written && bytes[off] != kPoison) {
            std::fprintf(stderr, "untouched byte +0x%zx changed to 0x%02x\n", off, bytes[off]);
            std::exit(1);
        }
    }
}

// Everything the constructor writes, with the pending link (+0x08..+0x17) included.
constexpr std::pair<std::size_t, std::size_t> kWrittenAfterRegister[] = {
    {0x00, 0x08}, {0x08, 0x10}, {0x18, 0x01}, {0x1c, 0x04}, {0x20, 0x40}, {0x60, 0x38}};

std::vector<ConCmdListNode*> pending_nodes(const ConCmdRegistry& r) {
    std::vector<ConCmdListNode*> v;
    if (r.pending.head.next == nullptr) return v;
    for (ConCmdListNode* n = r.pending.head.next; n != &r.pending.head; n = n->next) v.push_back(n);
    return v;
}

ConCmdBase* owner_of(ConCmdListNode* n) {
    return reinterpret_cast<ConCmdBase*>(reinterpret_cast<std::uint8_t*>(n) - offsetof(ConCmdBase, pending));
}

// ---- handler / release counters -------------------------------------------------------------------------
int g_handler_calls = 0;
std::uint64_t g_handler_tag = 0;
ConCmdCommand* g_handler_cmd = nullptr;
void* g_handler_args = nullptr;

int fake_handler(std::uint64_t tag, ConCmdCommand& cmd, void* args) noexcept {
    ++g_handler_calls;
    g_handler_tag = tag;
    g_handler_cmd = &cmd;
    g_handler_args = args;
    return 7;
}

std::vector<void*> g_destroyed;
std::vector<void*> g_freed_control;
std::vector<void*> g_freed_storage;
void destroy_hook(void* object, int flags) noexcept {
    CHECK(flags == 0);
    g_destroyed.push_back(object);
}
void free_control_hook(void* control) noexcept { g_freed_control.push_back(control); }
void free_storage_hook(void* storage) noexcept { g_freed_storage.push_back(storage); }
void reset_hooks() {
    g_destroyed.clear();
    g_freed_control.clear();
    g_freed_storage.clear();
}

FConCmdDtorHooks full_hooks() {
    FConCmdDtorHooks h{};
    h.manager_erase = &FConCmdManagerModel::erase_hook;
    h.shared.destroy = &destroy_hook;
    h.shared.free_control = &free_control_hook;
    h.free_storage = &free_storage_hook;
    return h;
}

void test_constants_and_sites() {
    CHECK(kFConCmdVptr == 0x1594dd0 && kFConCmdVtableStart == 0x1594dc0);
    CHECK(kFConCmdVptr == kFConCmdVtableStart + 0x10);
    CHECK(kFConCmdVtableSlots[0] == 0x4b0 && kFConCmdVtableSlots[2] == kConCmdRegisterRva);
    CHECK(kFConCmdDtorRva == 0x4b0 && kFConCmdInvokeThunkRva == 0x1990);
    CHECK(kFConCmdFlags == 2 && kConCmdBaseVptr == 0x1594d90);
    CHECK(sizeof(ConCmdCommand) == 0xa0 && sizeof(FConCmdDelegate) == 0x20 && sizeof(FConCmdArgRecord) == 0x50);
    CHECK(offsetof(ConCmdCommand, delegate) == 0x60 && offsetof(ConCmdCommand, args_begin) == 0x80);
    CHECK(offsetof(FConCmdDelegate, handler) == 0x18);   // the thunk reads [delegate_object + 0x18]

    CHECK(kSlot306CommandSites.size() == 3);
    const std::uint64_t handlers[3] = {0xa5bf20, 0xa5bf50, 0xa5c020};
    const std::uint64_t objects[3] = {0x32f9f68, 0x32fa008, 0x32fa0a8};
    for (std::size_t i = 0; i < 3; ++i) {
        const Slot306CommandSite& s = kSlot306CommandSites[i];
        CHECK(std::strlen(s.name) == s.name_length);
        CHECK(std::strlen(s.description) == s.description_length);
        CHECK(s.handler_rva == handlers[i] && s.object_rva == objects[i]);
        CHECK(s.description_rva == s.name_rva + s.name_length + 1);   // the description follows the name
        CHECK(s.ctor_call_rva > s.name_call_rva && s.register_call_rva > s.ctor_call_rva &&
              s.atexit_call_rva > s.register_call_rva);
    }
    CHECK(kSlot306CommandSites[1].object_rva - kSlot306CommandSites[0].object_rva == kFConCmdObjectSize);
    CHECK(kSlot306CommandSites[2].object_rva - kSlot306CommandSites[1].object_rva == kFConCmdObjectSize);
    CHECK(std::strcmp(kSlot306CommandSites[0].name, "sndx_reloadData") == 0);
    CHECK(std::strcmp(kSlot306CommandSites[1].name, "sndx_categoryMuteAll") == 0);
    CHECK(std::strcmp(kSlot306CommandSites[2].name, "sndx_categoryFilterAll") == 0);
}

void check_command_fields(const ConCmdCommand& c, const Slot306CommandSite& s) {
    CHECK(c.base.vptr == 0x1594dd0);
    CHECK(c.base.registered == 1);
    CHECK(c.base.flags == 2);
    CHECK(c.base.name.data == reinterpret_cast<const std::uint8_t*>(s.name));
    CHECK(c.base.name.size == static_cast<std::int64_t>(s.name_length));
    CHECK(c.base.name.aux == 0 && c.base.name.ctl == nullptr);
    CHECK(c.base.description.data == reinterpret_cast<const std::uint8_t*>(s.description));
    CHECK(c.base.description.size == static_cast<std::int64_t>(s.description_length));
    CHECK(c.base.description.aux == 0 && c.base.description.ctl == nullptr);
    CHECK(q(c, 0x60) == reinterpret_cast<std::uint64_t>(&c) + 0x60);   // self address
    CHECK(q(c, 0x68) == 0x1990);
    CHECK(q(c, 0x70) == 0);
    CHECK(q(c, 0x78) == s.handler_rva);
    CHECK(q(c, 0x80) == 0 && q(c, 0x88) == 0 && q(c, 0x90) == 0);
}

void test_construction_each_site() {
    for (std::size_t i = 0; i < 3; ++i) {
        ConCmdRegistry registry;
        ConCmdCommand c;
        poison(c);
        FConCmdTrace trace;
        const FConCmdBuildResult r = init_slot306_command(i, c, registry, &trace);
        CHECK(r.ok);
        CHECK(r.vtable_register_slot_ok);
        CHECK(r.registration.ok && r.registration.path == ConCmdRegisterPath::pending_list);
        CHECK(r.registration.list_initialised);
        CHECK(r.views_inert && r.atexit_recorded && !r.handler_invoked);
        check_command_fields(c, kSlot306CommandSites[i]);
        check_untouched(c, {kWrittenAfterRegister[0], kWrittenAfterRegister[1], kWrittenAfterRegister[2],
                            kWrittenAfterRegister[3], kWrittenAfterRegister[4], kWrittenAfterRegister[5]});
        // +0x19..+0x1b and +0x98..+0x9f are never written.
        const auto* b = reinterpret_cast<const std::uint8_t*>(&c);
        CHECK(b[0x19] == kPoison && b[0x1a] == kPoison && b[0x1b] == kPoison);
        for (std::size_t k = 0x98; k < 0xa0; ++k) CHECK(b[k] == kPoison);

        // The queue holds exactly this node (head.next == &c.base.pending).
        CHECK(registry.pending.size() == 1 && registry.pending.first() == &c.base.pending);
        CHECK(c.base.pending.prev == &registry.pending.head && c.base.pending.next == &registry.pending.head);

        // Step order, byte-identical to the listing.
        const FConCmdEvent expected[] = {
            FConCmdEvent::name_view_f2f0,         FConCmdEvent::base_ctor_20af0,
            FConCmdEvent::vptr_and_delegate_store, FConCmdEvent::vector_zero,
            FConCmdEvent::register_via_vtable_slot2, FConCmdEvent::release_description_f590,
            FConCmdEvent::release_name_f590,      FConCmdEvent::atexit_4b0};
        CHECK(trace.count == 8);
        for (std::size_t k = 0; k < 8; ++k) CHECK(trace.events[k] == expected[k]);

        // atexit: dtor 0x4b0 on the object.
        CHECK(registry.atexit_log.size() == 1);
        CHECK(registry.atexit_log.at(0).dtor_rva == 0x4b0 && registry.atexit_log.at(0).object == &c);

        // Re-registration is the already-registered no-op (the vtable slot 2 guard).
        const ConCmdRegisterResult again = register_concmd_20d60(c.base, registry);
        CHECK(again.ok && again.path == ConCmdRegisterPath::already_registered);
        CHECK(registry.pending.size() == 1);
    }
}

void test_three_in_one_registry() {
    ConCmdRegistry registry;
    std::array<ConCmdCommand, 3> cmds;
    for (auto& c : cmds) poison(c);
    for (std::size_t i = 0; i < 3; ++i) {
        const FConCmdBuildResult r = init_slot306_command(i, cmds[i], registry);
        CHECK(r.ok);
        CHECK(r.registration.list_initialised == (i == 0));   // guarded init runs once
    }
    const auto nodes = pending_nodes(registry);
    CHECK(nodes.size() == 3);
    for (std::size_t i = 0; i < 3; ++i) CHECK(owner_of(nodes[i]) == &cmds[i].base);   // FIFO drain order
    CHECK(registry.atexit_log.size() == 3);
    for (std::size_t i = 0; i < 3; ++i) {
        CHECK(registry.atexit_log.at(i).dtor_rva == 0x4b0 && registry.atexit_log.at(i).object == &cmds[i]);
        check_command_fields(cmds[i], kSlot306CommandSites[i]);
    }
}

void test_fail_closed_construction() {
    ConCmdRegistry registry;
    ConCmdCommand c;
    poison(c);
    ConCmdCommand reference;
    poison(reference);

    CHECK(!init_slot306_command(3, c, registry).ok);
    CHECK(!init_slot306_command(99, c, registry).ok);

    FConCmdCommandSpec spec{"cmd", "desc", 4, 0x1234};
    FConCmdCommandSpec bad = spec;
    bad.name = nullptr;
    CHECK(!init_fconcmd_object(c, bad, registry).ok);
    bad = spec;
    bad.description = nullptr;
    CHECK(!init_fconcmd_object(c, bad, registry).ok);
    bad = spec;
    bad.handler_tag = 0;
    CHECK(!init_fconcmd_object(c, bad, registry).ok);

    CHECK(std::memcmp(&c, &reference, sizeof c) == 0);   // nothing written
    CHECK(registry.pending.size() == 0 && !registry.pending.guard_done && registry.atexit_log.size() == 0);

    // A custom spec builds the same shape (the model is not tied to the three sites).
    CHECK(init_fconcmd_object(c, spec, registry).ok);
    CHECK(c.base.vptr == 0x1594dd0 && c.delegate.handler == 0x1234 && c.delegate.fn == 0x1990);
    CHECK(c.delegate.object == reinterpret_cast<std::uint64_t>(&c.delegate));
}

void test_handlers_are_identities() {
    g_handler_calls = 0;
    ConCmdRegistry registry;
    std::array<ConCmdCommand, 3> cmds;
    for (std::size_t i = 0; i < 3; ++i) CHECK(init_slot306_command(i, cmds[i], registry).ok);
    CHECK(g_handler_calls == 0);   // construction never reaches a handler body

    int arg_cell = 0;
    FConCmdInvokeHooks hooks{};
    // No hook: fails closed, nothing called.
    FConCmdInvokeResult r = invoke_delegate_head_21dc6(cmds[1], &arg_cell, hooks);
    CHECK(!r.ok && g_handler_calls == 0);
    r = invoke_thunk_1990(cmds[1], &cmds[1].delegate, &arg_cell, hooks);
    CHECK(!r.ok && g_handler_calls == 0);

    hooks.handler = &fake_handler;
    r = invoke_delegate_head_21dc6(cmds[1], &arg_cell, hooks);
    CHECK(r.ok && !r.no_delegate && r.status == 7);
    CHECK(g_handler_calls == 1 && g_handler_tag == 0xa5bf50 && g_handler_cmd == &cmds[1] &&
          g_handler_args == &arg_cell);

    // The thunk only accepts this command's own delegate (it reads [rdi + 0x18]).
    r = invoke_thunk_1990(cmds[1], &cmds[0].delegate, &arg_cell, hooks);
    CHECK(!r.ok && g_handler_calls == 1);

    // Unmodelled delegate shapes fail closed.
    ConCmdCommand tweak = cmds[2];
    tweak.delegate.fn |= 1;   // virtual member pointer
    CHECK(!invoke_delegate_head_21dc6(tweak, &arg_cell, hooks).ok);
    tweak = cmds[2];
    tweak.delegate.fn = 0x1992;
    CHECK(!invoke_delegate_head_21dc6(tweak, &arg_cell, hooks).ok);
    tweak = cmds[2];
    tweak.delegate.adj = 8;
    CHECK(!invoke_delegate_head_21dc6(tweak, &arg_cell, hooks).ok);
    tweak = cmds[2];
    tweak.delegate.handler = 0;
    CHECK(!invoke_delegate_head_21dc6(tweak, &arg_cell, hooks).ok);
    CHECK(g_handler_calls == 1);

    // No delegate at all: status 1 without a call.
    tweak = cmds[2];
    tweak.delegate.object = 0;
    tweak.delegate.fn = 0;
    r = invoke_delegate_head_21dc6(tweak, &arg_cell, hooks);
    CHECK(r.ok && r.no_delegate && r.status == 1 && g_handler_calls == 1);
}

void test_manager_path() {
    ConCmdRegistry registry;
    FConCmdManagerModel model;
    registry.manager.manager = &model;

    ConCmdCommand a;
    ConCmdCommand b;

    // Manager present but the contract is missing: visible failure.
    FConCmdBuildResult r = init_slot306_command(0, a, registry);
    CHECK(!r.ok && r.registration.path == ConCmdRegisterPath::manager && !r.registration.ok);

    registry.manager.insert = &FConCmdManagerModel::insert_hook;
    ConCmdRegistry registry2;
    registry2.manager = registry.manager;
    ConCmdCommand c0;
    ConCmdCommand c1;
    ConCmdCommand c2;
    CHECK(init_slot306_command(0, c0, registry2).ok);
    CHECK(init_slot306_command(1, c1, registry2).ok);
    CHECK(init_slot306_command(2, c2, registry2).ok);
    CHECK(registry2.pending.size() == 0 && !registry2.pending.guard_done);   // never queued
    CHECK(model.size() == 3);
    CHECK(model.find("sndx_reloadData") == &c0.base);
    CHECK(model.find("sndx_categoryMuteAll") == &c1.base);
    CHECK(model.find("sndx_categoryFilterAll") == &c2.base);
    CHECK(model.find("sndx_missing") == nullptr);

    // Duplicate name: first registration wins, nothing is overwritten.
    FConCmdCommandSpec dup{"SNDX_RELOADDATA", "other", 5, 0x77};
    CHECK(init_fconcmd_object(b, dup, registry2).ok);
    CHECK(model.size() == 3 && model.find("sndx_reloadData") == &c0.base);
    CHECK(model.find("SNDX_RELOADDATA") == &c0.base);
    CHECK(!model.failed());
}

void test_destructor_pending_list() {
    ConCmdRegistry registry;
    std::array<ConCmdCommand, 3> cmds;
    for (std::size_t i = 0; i < 3; ++i) CHECK(init_slot306_command(i, cmds[i], registry).ok);
    reset_hooks();
    const FConCmdDtorHooks hooks = full_hooks();

    // Middle node first.
    FConCmdDtorResult d = destroy_fconcmd_4b0(cmds[1], registry, hooks);
    CHECK(d.ok && d.records_released == 0 && !d.storage_freed);
    CHECK(d.base.unlinked && !d.base.manager_path && !d.base.list_initialised);
    CHECK(d.base.description_released && d.base.name_released && d.base.final_vptr == 0x1594d90);
    CHECK(cmds[1].base.vptr == 0x1594d90);   // the chain ends in the base destructor
    auto nodes = pending_nodes(registry);
    CHECK(nodes.size() == 2 && owner_of(nodes[0]) == &cmds[0].base && owner_of(nodes[1]) == &cmds[2].base);

    d = destroy_fconcmd_4b0(cmds[0], registry, hooks);
    CHECK(d.ok && d.base.unlinked);
    nodes = pending_nodes(registry);
    CHECK(nodes.size() == 1 && owner_of(nodes[0]) == &cmds[2].base);
    CHECK(registry.pending.head.next == &cmds[2].base.pending && cmds[2].base.pending.prev == &registry.pending.head);

    d = destroy_fconcmd_4b0(cmds[2], registry, hooks);
    CHECK(d.ok && d.base.unlinked);
    CHECK(registry.pending.size() == 0 && registry.pending.head.next == &registry.pending.head &&
          registry.pending.head.prev == &registry.pending.head);

    // Destroying an object that is not in the list is not an error (the walk simply finds nothing).
    d = destroy_fconcmd_4b0(cmds[2], registry, hooks);
    CHECK(d.ok && !d.base.unlinked);

    // A never-registered command on a fresh registry: the guarded init runs, nothing is unlinked.
    ConCmdRegistry fresh;
    ConCmdCommand lone;
    poison(lone);
    lone.base.vptr = 0;
    lone.base.name = SharedStr{};
    lone.base.description = SharedStr{};
    lone.args_begin = lone.args_end = lone.args_cap = nullptr;
    d = destroy_fconcmd_4b0(lone, fresh, hooks);
    CHECK(d.ok && d.base.list_initialised && !d.base.unlinked && fresh.pending.guard_done);
    CHECK(fresh.pending.head.next == &fresh.pending.head);
}

void test_destructor_views_and_vector() {
    reset_hooks();
    ConCmdRegistry registry;
    ConCmdCommand c;
    CHECK(init_slot306_command(0, c, registry).ok);

    // Shared views on the name/description: counters 3 and 1.
    std::int64_t name_ctl = 3;
    std::int64_t desc_ctl = 1;
    c.base.name.ctl = &name_ctl;
    c.base.description.ctl = &desc_ctl;

    // Last reference without the release hooks: fails closed (description is released first).
    FConCmdDtorHooks none{};
    FConCmdDtorResult d = destroy_fconcmd_4b0(c, registry, none);
    CHECK(!d.ok && !d.base.description_released && !d.base.name_released);
    CHECK(desc_ctl == 0);   // the decrement happened, as in the original, before the missing callee

    desc_ctl = 1;
    name_ctl = 3;
    // Rebuild the registration so the unlink can be seen again.
    ConCmdRegistry registry2;
    ConCmdCommand c2;
    CHECK(init_slot306_command(1, c2, registry2).ok);
    c2.base.name.ctl = &name_ctl;
    c2.base.description.ctl = &desc_ctl;
    const FConCmdDtorHooks hooks = full_hooks();
    d = destroy_fconcmd_4b0(c2, registry2, hooks);
    CHECK(d.ok && d.base.unlinked);
    CHECK(name_ctl == 2 && desc_ctl == 0);
    CHECK(g_destroyed.size() == 1 && g_destroyed[0] == const_cast<std::uint8_t*>(c2.base.description.data));
    CHECK(g_freed_control.size() == 1 && g_freed_control[0] == &desc_ctl);

    // ---- vector walk: two records, +0x28 released before +0x08, per record in order -------------------
    reset_hooks();
    ConCmdRegistry registry3;
    ConCmdCommand c3;
    CHECK(init_slot306_command(2, c3, registry3).ok);
    FConCmdArgRecord* storage = new FConCmdArgRecord[4]();
    std::int64_t ctl[4] = {2, 1, 1, 1};   // rec0.first, rec0.second, rec1.first, rec1.second
    static const std::uint8_t cell[4] = {1, 2, 3, 4};
    for (int i = 0; i < 2; ++i) {
        storage[i].first.data = &cell[i * 2];
        storage[i].first.ctl = &ctl[i * 2];
        storage[i].second.data = &cell[i * 2 + 1];
        storage[i].second.ctl = &ctl[i * 2 + 1];
    }
    c3.args_begin = storage;
    c3.args_end = storage + 2;
    c3.args_cap = storage + 4;
    d = destroy_fconcmd_4b0(c3, registry3, hooks);
    CHECK(d.ok && d.records_released == 2 && d.storage_freed);
    CHECK(ctl[0] == 1 && ctl[1] == 0 && ctl[2] == 0 && ctl[3] == 0);
    // last references: rec0.second, rec1.second, rec1.first, in this order.
    CHECK(g_destroyed.size() == 3);
    CHECK(g_destroyed[0] == const_cast<std::uint8_t*>(&cell[1]));
    CHECK(g_destroyed[1] == const_cast<std::uint8_t*>(&cell[3]));
    CHECK(g_destroyed[2] == const_cast<std::uint8_t*>(&cell[2]));
    CHECK(g_freed_storage.size() == 1 && g_freed_storage[0] == storage);
    CHECK(c3.args_begin == storage && c3.args_end == storage + 2);   // pointers left unchanged

    // Missing storage-free hook: fail closed after the walk, base destructor not reached.
    ConCmdRegistry registry4;
    ConCmdCommand c4;
    CHECK(init_slot306_command(0, c4, registry4).ok);
    c4.args_begin = storage;
    c4.args_end = storage;   // empty but with storage: 0x170c0 is still called
    c4.args_cap = storage + 4;
    FConCmdDtorHooks no_free = hooks;
    no_free.free_storage = nullptr;
    d = destroy_fconcmd_4b0(c4, registry4, no_free);
    CHECK(!d.ok && !d.storage_freed && registry4.pending.size() == 1);   // still queued: base dtor skipped
    reset_hooks();
    d = destroy_fconcmd_4b0(c4, registry4, hooks);
    CHECK(d.ok && d.records_released == 0 && d.storage_freed && g_freed_storage.size() == 1);
    CHECK(registry4.pending.size() == 0);

    // Malformed vectors: nothing released, vptr untouched.
    ConCmdRegistry registry5;
    ConCmdCommand c5;
    CHECK(init_slot306_command(0, c5, registry5).ok);
    c5.args_begin = storage + 2;
    c5.args_end = storage;
    CHECK(!destroy_fconcmd_4b0(c5, registry5, hooks).ok);
    c5.args_begin = storage;
    c5.args_end = reinterpret_cast<FConCmdArgRecord*>(reinterpret_cast<std::uint8_t*>(storage) + 0x30);
    CHECK(!destroy_fconcmd_4b0(c5, registry5, hooks).ok);
    CHECK(c5.base.vptr == 0x1594dd0 && registry5.pending.size() == 1);

    delete[] storage;
}

void test_manager_erase_through_destructor() {
    reset_hooks();
    ConCmdRegistry registry;
    FConCmdManagerModel model;
    registry.manager.manager = &model;
    registry.manager.insert = &FConCmdManagerModel::insert_hook;

    std::array<ConCmdCommand, 3> cmds;
    for (std::size_t i = 0; i < 3; ++i) CHECK(init_slot306_command(i, cmds[i], registry).ok);
    CHECK(model.size() == 3);

    // The manager is non-null but the erase hook is missing: ok == false, entry kept.
    FConCmdDtorHooks missing = full_hooks();
    missing.manager_erase = nullptr;
    FConCmdDtorResult d = destroy_fconcmd_4b0(cmds[0], registry, missing);
    CHECK(!d.ok && d.base.manager_path && !d.base.manager_erased && model.size() == 3);
    CHECK(cmds[0].base.vptr == 0x1594d90);   // the vptr store precedes the manager check

    const FConCmdDtorHooks hooks = full_hooks();
    d = destroy_fconcmd_4b0(cmds[0], registry, hooks);
    CHECK(d.ok && d.base.manager_path && d.base.manager_erased && !d.base.unlinked && d.base.list_initialised);
    CHECK(model.size() == 2 && model.find("sndx_reloadData") == nullptr);
    CHECK(model.find("sndx_categoryMuteAll") == &cmds[1].base);

    // Duplicate-name command: erase is by KEY ONLY, so destroying the duplicate removes the entry that
    // belongs to the first command (0x213b0 never compares the stored ConCmd pointer).
    ConCmdCommand dup;
    FConCmdCommandSpec spec{"SNDX_CATEGORYMUTEALL", "dup", 3, 0x99};
    CHECK(init_fconcmd_object(dup, spec, registry).ok);
    CHECK(model.find("sndx_categoryMuteAll") == &cmds[1].base);
    d = destroy_fconcmd_4b0(dup, registry, hooks);
    CHECK(d.ok && d.base.manager_erased);
    CHECK(model.find("sndx_categoryMuteAll") == nullptr && model.find("SNDX_CATEGORYMUTEALL") == nullptr &&
          model.size() == 1);

    // Erasing an absent key reports false from the hook but the destructor still completes.
    d = destroy_fconcmd_4b0(cmds[1], registry, hooks);
    CHECK(d.ok && !d.base.manager_erased);

    // The key copy holds a reference: insert acquires, erase releases.
    ConCmdRegistry registry2;
    FConCmdManagerModel model2(hooks.shared);
    registry2.manager.manager = &model2;
    registry2.manager.insert = &FConCmdManagerModel::insert_hook;
    std::int64_t ctl = 1;
    ConCmdCommand shared_cmd;
    FConCmdCommandSpec s2{"sndx_shared", "d", 1, 0x55};
    CHECK(init_fconcmd_object(shared_cmd, s2, registry2).ok);
    // Rebuild with a shared name so the key copy acquires: poke the control counter before inserting.
    shared_cmd.base.registered = 0;
    shared_cmd.base.name.ctl = &ctl;
    ConCmdRegistry registry3;
    FConCmdManagerModel model3(hooks.shared);
    registry3.manager.manager = &model3;
    registry3.manager.insert = &FConCmdManagerModel::insert_hook;
    CHECK(register_concmd_20d60(shared_cmd.base, registry3).ok);
    CHECK(ctl == 2 && model3.size() == 1);
    CHECK(FConCmdManagerModel::erase_hook(&model3, shared_cmd.base.name));
    CHECK(ctl == 1 && model3.size() == 0 && !model3.failed());
    // Last reference on erase without release hooks: the erase reports failure and the model flags it.
    FConCmdManagerModel model4;
    ConCmdRegistry registry4;
    registry4.manager.manager = &model4;
    registry4.manager.insert = &FConCmdManagerModel::insert_hook;
    shared_cmd.base.registered = 0;
    CHECK(register_concmd_20d60(shared_cmd.base, registry4).ok);
    CHECK(ctl == 2);
    ctl = 1;   // drop the caller's own reference
    CHECK(!FConCmdManagerModel::erase_hook(&model4, shared_cmd.base.name));
    CHECK(model4.failed() && ctl == 0);
}

void test_whole_block_roundtrip() {
    reset_hooks();
    ConCmdRegistry registry;
    std::array<ConCmdCommand, 3> cmds;
    for (std::size_t i = 0; i < 3; ++i) CHECK(init_slot306_command(i, cmds[i], registry).ok);
    CHECK(registry.pending.size() == 3 && registry.atexit_log.size() == 3);

    // __cxa_atexit runs in reverse registration order: every recorded dtor is 0x4b0.
    const FConCmdDtorHooks hooks = full_hooks();
    for (std::size_t i = registry.atexit_log.size(); i-- > 0;) {
        const StartupAtexitEntry& e = registry.atexit_log.at(i);
        CHECK(e.dtor_rva == kFConCmdDtorRva);
        auto* cmd = const_cast<ConCmdCommand*>(static_cast<const ConCmdCommand*>(e.object));
        const FConCmdDtorResult d = destroy_fconcmd_4b0(*cmd, registry, hooks);
        CHECK(d.ok && d.base.unlinked);
    }
    CHECK(registry.pending.size() == 0);
    CHECK(g_destroyed.empty() && g_freed_control.empty() && g_freed_storage.empty());   // views were borrowed
    for (auto& c : cmds) CHECK(c.base.vptr == 0x1594d90);
}

}  // namespace

int main() {
    test_constants_and_sites();
    test_construction_each_site();
    test_three_in_one_registry();
    test_fail_closed_construction();
    test_handlers_are_identities();
    test_manager_path();
    test_destructor_pending_list();
    test_destructor_views_and_vector();
    test_manager_erase_through_destructor();
    test_whole_block_roundtrip();
    std::printf("fconcmd_object_slot306: %d checks passed\n", g_checks);
    return 0;
}
