// CPU-01-STARTUP-SLOT-468: fixture for slot 468 / 0x3c3a90.
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/ctor_98a90_3be050.hpp"
#include "core/ctor_slot_468_3c3a90.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

std::vector<void*> g_blocks;
std::size_t g_secondary = 0;

void* entry_alloc(std::size_t size, std::uint32_t, std::uint32_t, std::uint32_t) {
    void* p = std::malloc(size);
    if (p != nullptr) g_blocks.push_back(p);
    return p;
}

bool entry_free(void* p, std::uint32_t) {
    std::free(p);
    for (void*& block : g_blocks) if (block == p) block = nullptr;
    return true;
}

void release_blocks() {
    for (void* p : g_blocks) std::free(p);
    g_blocks.clear();
}

bool view_is(const SharedStr& v, const char* text) {
    const std::size_t length = std::strlen(text);
    return v.data != nullptr && v.size == static_cast<std::int64_t>(length) && v.aux == 0 &&
           v.ctl == nullptr && std::memcmp(v.data, text, length) == 0;
}

bool rewind_secondary(void*, Slot468Command& command) noexcept {
    ++g_secondary;
    command.gap98 = 0xfeedfacecafebeefull;
    return true;
}

void reset_globals() {
    ctor_98a90_self_cell_2ef63f8() = nullptr;
    ctor_98a90_list_head_cell_2ef6400() = nullptr;
}

Slot468Result run_slot(Slot468Objects& objects, ConCmdRegistry& registry,
                       lane_m5::AppInfoQueue& queue, const Slot468Hooks& hooks = {}) {
    return init_slot468_3c3a90(objects, registry, queue, hooks);
}

void section_alone() {
    reset_globals();
    Slot468Objects objects{};
    ConCmdRegistry registry{};
    lane_m5::AppInfoQueue queue{};
    Slot468Hooks hooks{};
    hooks.rewind_secondary = &rewind_secondary;
    const Slot468Result r = run_slot(objects, registry, queue, hooks);
    CHECK(r.ok && r.canary_matched);
    CHECK(r.apps == 3 && r.entries == 5 && r.commands == 10);
    CHECK(r.app_registrations == 3 && r.entry_inserts == 0 && r.command_registrations == 10);
    CHECK(r.atexit_records == 18);
    CHECK(r.rewind_reserved && r.rewind_inline_push && !r.rewind_grow_path);
    CHECK(r.rewind_registered && r.rewind_secondary_called && g_secondary == 1);
    CHECK(registry.pending.size() == 10);
    CHECK(registry.atexit_log.size() == 18);

    // App-info delegate entries: push-front chain, names, delegate identity tags.
    CHECK(queue.head == &objects.apps[2]);
    CHECK(objects.apps[2].next == &objects.apps[1] && objects.apps[1].next == &objects.apps[0]);
    CHECK(objects.apps[0].next == nullptr);
    for (std::size_t i = 0; i < kSlot468AppCount; ++i) {
        const Slot468AppSpec& spec = kSlot468Apps[i];
        const lane_m5::AppInfoQueueEntry& entry = objects.apps[i];
        CHECK(entry.registered == 1);
        CHECK(view_is(entry.name, spec.name));
        CHECK(entry.delegate.object == reinterpret_cast<std::uint64_t>(&entry.delegate));
        CHECK(entry.delegate.fn == lane_m5::kDelegateInvokerRva);
        CHECK(entry.delegate.adj == 0 && entry.delegate.handler == spec.handler);
    }

    // Scripted entries: three shared tail pointers, identity tags, push-front list.
    CHECK(ctor_98a90_list_head_cell_2ef6400() == &objects.entries[4]);
    for (std::size_t i = 0; i < kSlot468EntryCount; ++i) {
        const Slot468EntrySpec& spec = kSlot468Entries[i];
        const ScriptedEntry& entry = objects.entries[i];
        for (std::size_t g = 0; g < 3; ++g) {
            CHECK(entry.groups[g].self == reinterpret_cast<std::uint64_t>(&entry) + 0x88);
            CHECK(entry.groups[g].fn == spec.fns[g]);
            CHECK(entry.groups[g].zero10 == 0 && entry.groups[g].flag18 == 0);
        }
        CHECK(view_is(entry.name, spec.name));
        CHECK(entry.tail88 == 0);
        if (i == 0) CHECK(entry.next == nullptr);
        else CHECK(entry.next == &objects.entries[i - 1]);
    }

    // Ten commands: nine ordinary, one with the 0x50-byte pause token.
    for (std::size_t i = 0; i < kSlot468CommandCount; ++i) {
        const Slot468CommandSpec& spec = kSlot468Commands[i];
        const Slot468Command& command = objects.commands[i];
        CHECK(command.base.vptr == kSlot468CommandVptr);
        CHECK(command.base.registered == 1 && command.base.flags == kSlot468CommandFlags);
        CHECK(view_is(command.base.name, spec.name));
        CHECK(view_is(command.base.description, spec.description));
        CHECK(command.tail0 == reinterpret_cast<std::uint64_t>(&command.tail0));
        CHECK(command.tail8 == kSlot468CommandTail8 && command.tail10 == 0);
        CHECK(command.tail18 == spec.callback);
        CHECK(command.gap98 == (spec.rewind ? 0xfeedfacecafebeefull : 0));
        if (spec.rewind) {
            CHECK(command.vec_begin == &objects.rewind_token);
            CHECK(command.vec_end == &objects.rewind_token + 1);
            CHECK(command.vec_cap == &objects.rewind_token + 1);
        } else {
            CHECK(command.vec_begin == nullptr && command.vec_end == nullptr && command.vec_cap == nullptr);
        }
    }

    const Slot468Token& token = objects.rewind_token;
    CHECK(token.kind == 0);
    CHECK(view_is(token.text, "pause"));
    CHECK(token.empty.data == reinterpret_cast<const std::uint8_t*>(kSlot468TokenEmptyData));
    CHECK(token.empty.size == 0 && token.empty.aux == 0 && token.empty.ctl == nullptr);
    CHECK(token.flag48 == 1);

    // Atexit body order: three app entries, five scripted entries, ten commands.
    for (std::size_t i = 0; i < 3; ++i) CHECK(registry.atexit_log.at(i).dtor_rva == kSlot468AppDtor);
    for (std::size_t i = 3; i < 8; ++i) CHECK(registry.atexit_log.at(i).dtor_rva == kSlot468EntryDtor);
    for (std::size_t i = 8; i < 18; ++i) CHECK(registry.atexit_log.at(i).dtor_rva == kSlot468CommandDtor);
}

void section_registry() {
    reset_globals();
    Slot468Objects objects{};
    ConCmdRegistry registry{};
    lane_m5::AppInfoQueue queue{};
    RbMap map{};
    RbHooks hooks{};
    hooks.alloc.allocate = &entry_alloc;
    hooks.alloc.deallocate = &entry_free;
    CHECK(rb_map_init_empty(map, hooks));
    ctor_98a90_self_cell_2ef63f8() = &map;
    Slot468Hooks slot_hooks{};
    slot_hooks.entry_hooks = hooks;
    const Slot468Result r = run_slot(objects, registry, queue, slot_hooks);
    CHECK(r.ok && r.entries == 5 && r.entry_inserts == 5);
    CHECK(map.size == 5 && rb_check_invariants(map).ok);
    for (std::size_t i = 0; i < kSlot468EntryCount; ++i) {
        const RbIndexResult found =
            rb_index_3c0840(map, objects.entries[i].name, hooks);
        CHECK(found.ok && !found.created && found.slot != nullptr && *found.slot == &objects.entries[i]);
    }
    release_blocks();
    reset_globals();
}

void section_failure() {
    reset_globals();
    Slot468Objects objects{};
    ConCmdRegistry registry{};
    lane_m5::AppInfoQueue queue{};
    RbMap map{};
    RbHooks hooks{};
    hooks.alloc.allocate = &entry_alloc;
    hooks.alloc.deallocate = &entry_free;
    CHECK(rb_map_init_empty(map, hooks));
    ctor_98a90_self_cell_2ef63f8() = &map;
    Slot468Hooks slot_hooks{};   // no entry allocator -> first 0x3c0840 fails visibly
    const Slot468Result r = run_slot(objects, registry, queue, slot_hooks);
    CHECK(!r.ok && r.apps == 3 && r.entries == 0 && r.commands == 0);
    CHECK(registry.pending.size() == 0);
    release_blocks();
    reset_globals();
}

}  // namespace

int main() {
    CHECK(kSlot468Rva == 0x3c3a90 && kSlot468Size == 4772);
    CHECK(sizeof(Slot468Command) == 0xa0 && offsetof(Slot468Objects, commands) == 0x3c0);
    section_alone();
    section_registry();
    section_failure();
    std::puts("OK test_ctor_slot_468");
    return 0;
}
