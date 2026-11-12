// CPU-01-STARTUP-SLOT-468: fixture for the scripted-entry helper 0x3c2a00.
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "core/rb_registry_3be2d0.hpp"
#include "core/scripted_entry_3c2a00.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

std::vector<void*> g_blocks;

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

SharedStr view(const char* text, std::size_t length) {
    return SharedStr{reinterpret_cast<const std::uint8_t*>(text), static_cast<std::int64_t>(length), 0,
                     nullptr};
}

bool view_is(const SharedStr& v, const char* text) {
    const std::size_t length = std::strlen(text);
    return v.data != nullptr && v.size == static_cast<std::int64_t>(length) && v.aux == 0 &&
           std::memcmp(v.data, text, length) == 0;
}

void section_lists() {
    ScriptedEntry entry;
    std::memset(&entry, 0xA5, sizeof entry);
    std::int64_t ctl = 7;
    const SharedStr name{reinterpret_cast<const std::uint8_t*>("scripted"), 8, 0, &ctl};
    const std::array<ScriptedEntryDescriptor, 3> descriptors{{
        {0x1111, 0x2222, 0, 0},
        {0x3333, 0x4444, 0, 1},
        {0x5555, 0x6666, 0, 0},
    }};
    ScriptedEntry* head = reinterpret_cast<ScriptedEntry*>(0x1234);
    const ScriptedEntryResult r =
        init_scripted_entry_3c2a00(entry, descriptors, name, head, nullptr);
    CHECK(r.ok && r.linked && !r.registry_present && !r.inserted && !r.already_present);
    CHECK(head == &entry && entry.next == reinterpret_cast<ScriptedEntry*>(0x1234));
    CHECK(ctl == 8);
    CHECK(entry.groups[0].self == 0x1111 && entry.groups[0].fn == 0x2222 && entry.groups[0].zero10 == 0);
    CHECK(entry.groups[1].self == reinterpret_cast<std::uint64_t>(&entry.groups[1]));
    CHECK(entry.groups[1].fn == 0x4444 && entry.groups[1].flag18 == 1);
    CHECK(entry.groups[2].self == 0x5555 && entry.groups[2].fn == 0x6666);
    CHECK(entry.tail88 == 0xA5A5A5A5A5A5A5A5ull);   // never written by 0x3c2a00
    CHECK(view_is(entry.name, "scripted"));
    CHECK(entry.name.ctl == &ctl);
}

void section_registry() {
    RbMap map{};
    RbHooks hooks{};
    hooks.alloc.allocate = &entry_alloc;
    hooks.alloc.deallocate = &entry_free;
    CHECK(rb_map_init_empty(map, hooks));

    ScriptedEntry one;
    ScriptedEntry two;
    std::memset(&one, 0, sizeof one);
    std::memset(&two, 0, sizeof two);
    const std::array<ScriptedEntryDescriptor, 3> descriptors{{
        {0, 0x10, 0, 0},
        {0, 0x20, 0, 0},
        {0, 0x30, 0, 0},
    }};
    ScriptedEntry* head = nullptr;
    const ScriptedEntryResult first =
        init_scripted_entry_3c2a00(one, descriptors, view("alpha", 5), head, &map, hooks);
    CHECK(first.ok && first.linked && first.registry_present && first.inserted);
    const ScriptedEntryResult second =
        init_scripted_entry_3c2a00(two, descriptors, view("beta", 4), head, &map, hooks);
    CHECK(second.ok && second.inserted && !second.already_present);
    CHECK(head == &two && two.next == &one && one.next == nullptr);
    CHECK(map.size == 2 && rb_check_invariants(map).ok);

    const RbIndexResult found = rb_index_3c0840(map, view("alpha", 5), hooks);
    CHECK(found.ok && !found.created && found.slot != nullptr && *found.slot == &one);
    const RbIndexResult existing = rb_index_3c0840(map, view("beta", 4), hooks);
    CHECK(existing.ok && !existing.created && existing.slot != nullptr && *existing.slot == &two);

    // A pre-populated non-null mapping is not overwritten.
    const SharedStr alpha = view("alpha", 5);
    RbIndexResult slot = rb_index_3c0840(map, alpha, hooks);
    CHECK(slot.ok && slot.slot != nullptr);
    void* old = *slot.slot;
    ScriptedEntry third;
    std::memset(&third, 0, sizeof third);
    const ScriptedEntryResult duplicate =
        init_scripted_entry_3c2a00(third, descriptors, alpha, head, &map, hooks);
    CHECK(duplicate.ok && duplicate.already_present && !duplicate.inserted);
    CHECK(*slot.slot == old);
    release_blocks();
}

void section_registry_failure() {
    RbMap map{};
    RbHooks hooks{};
    hooks.alloc.allocate = &entry_alloc;
    hooks.alloc.deallocate = &entry_free;
    CHECK(rb_map_init_empty(map, hooks));
    ScriptedEntry entry;
    std::memset(&entry, 0, sizeof entry);
    const std::array<ScriptedEntryDescriptor, 3> descriptors{{
        {0, 1, 0, 0}, {0, 2, 0, 0}, {0, 3, 0, 0},
    }};
    ScriptedEntry* head = nullptr;
    const ScriptedEntryResult r =
        init_scripted_entry_3c2a00(entry, descriptors, view("alpha", 5), head, &map, RbHooks{});
    CHECK(!r.ok && r.linked && r.registry_present && !r.inserted);
    CHECK(head == &entry && map.size == 0);
    release_blocks();
}

}  // namespace

int main() {
    section_lists();
    section_registry();
    section_registry_failure();
    std::puts("OK test_scripted_entry_3c2a00");
    return 0;
}
