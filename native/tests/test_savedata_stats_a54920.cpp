// Fixture for the native 0xa54920 model. Asserts the three counters, three 0x26ea0 toggles, the
// deferred 0x272d0 records, the real 0xa55050 0xb8 slot, the 0xa57c10/0xa58490 old-head cleanup,
// and the 0xa586a0 deque-map append. No guest byte is executed.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "core/ctor_slots_322_334.hpp"
#include "core/savedata_stats_a54920.hpp"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++g_failures;                                                        \
        }                                                                        \
    } while (0)

using namespace sotc;

struct Calls {
    int parent_slot_dtors = 0;
};

SavedataStatsName make_name(const char* prefix, std::uint32_t max_count, void* slot40 = nullptr,
                            void* slot48 = nullptr, void* slot50 = nullptr) {
    SavedataStatsName name{};
    name.prefix = prefix;
    name.max_count = max_count;
    name.parent_slot40 = slot40;
    name.parent_slot48 = slot48;
    name.parent_slot50 = slot50;
    return name;
}

SavedataStatsInDescriptor make_descriptor() {
    SavedataStatsInDescriptor in{};
    in.field00 = 0x1122334455667788ull;
    in.field08 = 0xA1A2A3A4A5A6A7A8ull;
    in.field10 = 0xB1B2B3B4B5B6B7B8ull;
    in.field18 = 0;
    in.vtable40 = reinterpret_cast<void*>(0x1590000);
    in.field48 = 0xC1C2C3C4C5C6C7C8ull;
    return in;
}

alignas(0x100) std::array<std::uint8_t, 0x100> g_block0{};
alignas(0x100) std::array<std::uint8_t, 0x100> g_block1{};
std::array<std::uint8_t*, 4> g_map_slots{};
alignas(16) std::array<std::uint8_t, 0x2000> g_pool{};
std::size_t g_pool_offset = 0;
std::size_t g_alloc_calls = 0;
std::size_t g_alloc_size = 0;
std::uint32_t g_alloc_tag = 0;
std::uint32_t g_alloc_align = 0;
std::uint32_t g_alloc_flags = 0;
void* g_last_alloc = nullptr;
bool g_alloc_ok = true;
std::vector<void*> g_frees;

void* test_alloc(std::size_t size, std::uint32_t tag, std::uint32_t align,
                 std::uint32_t flags) noexcept {
    ++g_alloc_calls;
    g_alloc_size = size;
    g_alloc_tag = tag;
    g_alloc_align = align;
    g_alloc_flags = flags;
    if (!g_alloc_ok || g_pool_offset + size > g_pool.size()) return nullptr;
    void* const out = g_pool.data() + g_pool_offset;
    g_pool_offset += (size + 0xF) & ~std::size_t{0xF};
    g_last_alloc = out;
    return out;
}

bool test_free(void* ptr, std::uint32_t tag) noexcept {
    (void)tag;
    g_frees.push_back(ptr);
    return true;
}

AllocDispatchContract test_contract() {
    return AllocDispatchContract{test_alloc, test_free};
}

void reset_alloc() {
    g_pool_offset = 0;
    g_alloc_calls = 0;
    g_alloc_size = 0;
    g_alloc_tag = 0;
    g_alloc_align = 0;
    g_alloc_flags = 0;
    g_last_alloc = nullptr;
    g_alloc_ok = true;
    g_frees.clear();
}

struct ParentStorage {
    alignas(16) std::array<std::uint8_t, 0x700> bytes{};
    std::uint32_t cursor = 0;

    SavedataStatsDescriptor* slot() {
        return reinterpret_cast<SavedataStatsDescriptor*>(
            bytes.data() + kSavedataSlotBaseOffset + 0 * kSavedataSlotStride);
    }

    SavedataDequeMap* map() {
        return reinterpret_cast<SavedataDequeMap*>(bytes.data() + kSavedataStatsParentMapOffset);
    }

    void init_map(bool full) {
        std::memset(map(), 0, sizeof *map());
        g_block0.fill(0);
        g_block1.fill(0);
        g_map_slots.fill(nullptr);
        g_map_slots[1] = g_block0.data();
        SavedataDequeMap& m = *map();
        m.map = g_map_slots.data();
        m.map_capacity = g_map_slots.size();
        m.start_cur = g_block0.data();
        m.start_first = g_block0.data();
        m.start_last = g_block0.data() + kSavedataBlockSize;
        m.start_node = &g_map_slots[1];
        m.finish_cur = full ? g_block0.data() + kSavedataBlockSize - sizeof(void*)
                            : g_block0.data();
        m.finish_first = g_block0.data();
        m.finish_last = g_block0.data() + kSavedataBlockSize;
        m.finish_node = &g_map_slots[1];
    }
};

void section_counters() {
    SavedataStatsName n0 = make_name("save", 8);
    SavedataStatsName n1 = make_name("save", 99);   // ignored: second counter is zero
    SavedataStatsName n2 = make_name("save", 77);   // ignored: third counter is zero
    SavedataStatsInDescriptor in = make_descriptor();
    ParentStorage parent{};
    parent.init_map(false);
    SavedataStatsA54920Args args{};
    args.parent = parent.bytes.data();
    args.sibling_cursor = &parent.cursor;
    args.names = {&n0, &n1, &n2};
    args.descriptor = &in;

    const SavedataStatsA54920Result r = savedata_stats_a54920(args, {});
    CHECK(r.ok);
    CHECK(r.refusal == SavedataStatsRefusal::none);
    CHECK(r.descriptor == parent.slot());
    CHECK(r.sibling_slot_found);
    CHECK(r.sibling_slot_index == 0);
    CHECK(parent.cursor == 1);
    CHECK(r.concmd.pending.size() == 3);
    CHECK(r.menu.root.count == 3);
    CHECK(r.atexit_count == 3);

    // The listing passes 0x11/0x12/0x10 as the snprintf size (including NUL), so "save" is
    // truncated exactly as the original does: 16, 17 and 15 characters plus NUL.
    const char* expect[3] = {"save_save_max_co", "save_save_saves_u", "save_save_mem_u"};
    const std::uint32_t expect_max[3] = {8, 0, 0};
    for (int i = 0; i < 3; ++i) {
        CHECK(std::strcmp(r.names[i].data(), expect[i]) == 0);
        CHECK(r.max_counts[i] == expect_max[i]);
        CHECK(r.worker_ok[i]);
        CHECK(r.name_view_inert[i]);
        CHECK(r.description_view_inert[i]);
        CHECK(r.registered[i]);
        CHECK(r.parent_slot_replaced[i]);
        CHECK(r.counters[i].base.vptr == kSavedataStatsCounterVptr);
        CHECK(r.counters[i].value_ref == &r.counters[i].value);
        CHECK(r.counters[i].value == expect_max[i]);
        CHECK(r.counters[i].default_value == expect_max[i]);
        CHECK(r.counters[i].fn_vptr == kUnboundedIntFnTable);
        CHECK(std::strcmp(reinterpret_cast<const char*>(r.counters[i].base.name.data),
                          expect[i]) == 0);
    }
    CHECK(n0.parent_slot40 == &r.counters[0]);
    CHECK(n1.parent_slot48 == &r.counters[1]);
    CHECK(n2.parent_slot50 == &r.counters[2]);

    // Toggle buffers are 0xc/0xd/0x15 bytes: 11, 12 and 20 characters plus NUL.
    const char* text[3] = {"save Max Co", "save Saves U", "save Save Mem Used ("};
    for (int i = 0; i < 3; ++i) {
        CHECK(r.item_ctor_ok[i]);
        CHECK(r.item_registered[i]);
        CHECK(r.items[i].kind == 2);
        CHECK(r.items[i].flags == 0x30);
        CHECK(r.items[i].target == &r.counters[i]);
        CHECK(r.items[i].range_min == 1.0f);
        CHECK(r.items[i].range_max == 10.0f);
        CHECK(std::strcmp(reinterpret_cast<const char*>(r.items[i].label.data),
                          "Save System") == 0);
        CHECK(std::strcmp(reinterpret_cast<const char*>(r.items[i].text.data), text[i]) == 0);
    }
    CHECK(r.concmd.atexit_log.size() >= 3);
    const std::size_t first = r.concmd.atexit_log.size() - 3;
    for (int i = 0; i < 3; ++i) {
        CHECK(r.concmd.atexit_log.at(first + i).dtor_rva == 0x272d0);
        CHECK(r.concmd.atexit_log.at(first + i).object == &r.items[i]);
    }

    CHECK(r.descriptor->sentinel10 == 0x100000001ull);
    CHECK(r.descriptor->type1c == 2);
    CHECK(r.descriptor->head38 == &r.node);
    CHECK(r.descriptor_head_installed);
    CHECK(r.node.owner == &n2);
    CHECK(r.node.child == &r.child);
    CHECK(r.node.zero18 == 0);
    CHECK(r.node.index20 == 0xFFFFFFFFu);
    CHECK(r.child.self0 == &r.child && r.child.self8 == &r.child && r.child.self10 == &r.child);
    CHECK(r.child.one18 == 1 && r.child.one19 == 1);
    CHECK(r.descriptor->field48 == in.field00);
    CHECK(std::memcmp(r.descriptor->field50, &in.field08, 16) == 0);
    CHECK(r.descriptor->field60 == in.field18);
    CHECK(r.descriptor->fn90 == in.field48);
    CHECK(r.descriptor_vcall_count == 5);
    CHECK(r.parent_vector_appended);
    CHECK(!r.vector_grow_called);
    CHECK(parent.map()->finish_cur == g_block0.data() + sizeof(void*));
    CHECK(g_map_slots[1] == g_block0.data());
}

void section_old_slots_and_self_link() {
    Calls calls{};
    SavedataStatsName n0 = make_name("player", 2, &calls);
    SavedataStatsName n1 = make_name("player", 2, nullptr, &calls);
    SavedataStatsName n2 = make_name("player", 2, nullptr, nullptr, &calls);
    SavedataStatsInDescriptor in = make_descriptor();
    in.field18 = 0xDEADBEEF;

    ParentStorage parent{};
    parent.init_map(false);
    SavedataStatsA54920Hooks hooks{};
    hooks.parent_slot_dtor = [](void* old) noexcept {
        auto* c = static_cast<Calls*>(old);
        ++c->parent_slot_dtors;
    };

    SavedataStatsA54920Args args{};
    args.parent = parent.bytes.data();
    args.sibling_cursor = &parent.cursor;
    args.names = {&n0, &n1, &n2};
    args.descriptor = &in;
    const SavedataStatsA54920Result r = savedata_stats_a54920(args, hooks);
    CHECK(r.ok);
    CHECK(calls.parent_slot_dtors == 3);
    CHECK(r.old_parent_slot_destroyed[0]);
    CHECK(r.old_parent_slot_destroyed[1]);
    CHECK(r.old_parent_slot_destroyed[2]);
    CHECK(n0.parent_slot40 == &r.counters[0]);
    CHECK(n1.parent_slot48 == &r.counters[1]);
    CHECK(n2.parent_slot50 == &r.counters[2]);
    CHECK(r.descriptor->field60 == 0xDEADBEEF);
    CHECK(r.descriptor->field48 == reinterpret_cast<std::uintptr_t>(&r.descriptor->field48));
    CHECK(r.descriptor_vcall_count == 5);
}

void section_grow_and_refusals() {
    SavedataStatsName n0 = make_name("save", 1);
    SavedataStatsName n1 = make_name("save", 1);
    SavedataStatsName n2 = make_name("save", 1);
    SavedataStatsInDescriptor in = make_descriptor();

    {
        ParentStorage parent{};
        parent.init_map(true);
        SavedataStatsA54920Args args{};
        args.parent = parent.bytes.data();
        args.sibling_cursor = &parent.cursor;
        args.names = {&n0, &n1, &n2};
        args.descriptor = &in;
        const SavedataStatsA54920Result r = savedata_stats_a54920(args, {});
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataStatsRefusal::vector_block_failed);
        CHECK(r.vector_grow_called);
    }
    {
        ParentStorage parent{};
        parent.init_map(true);
        // No front/back map space: the grow itself must allocate and has no contract.
        g_map_slots[0] = g_block0.data();
        parent.map()->map_capacity = 1;
        parent.map()->start_node = &g_map_slots[0];
        parent.map()->finish_node = &g_map_slots[0];
        SavedataStatsA54920Args args{};
        args.parent = parent.bytes.data();
        args.sibling_cursor = &parent.cursor;
        args.names = {&n0, &n1, &n2};
        args.descriptor = &in;
        const SavedataStatsA54920Result r = savedata_stats_a54920(args, {});
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataStatsRefusal::vector_grow_failed);
        CHECK(r.vector_grow_called);
    }
    {
        ParentStorage parent{};
        parent.init_map(true);
        reset_alloc();
        SavedataStatsA54920Args args{};
        args.parent = parent.bytes.data();
        args.sibling_cursor = &parent.cursor;
        args.names = {&n0, &n1, &n2};
        args.descriptor = &in;
        args.alloc = test_contract();
        const SavedataStatsA54920Result r = savedata_stats_a54920(args, {});
        CHECK(r.ok);
        CHECK(r.vector_grow_called);
        CHECK(r.vector_block_allocated);
        CHECK(r.parent_vector_appended);
        CHECK(parent.map()->finish_node == &g_map_slots[1]);
        CHECK(parent.map()->finish_first == g_last_alloc);
        CHECK(parent.map()->finish_last ==
              static_cast<std::uint8_t*>(g_last_alloc) + kSavedataBlockSize);
        CHECK(parent.map()->finish_cur == g_last_alloc);
        CHECK(*reinterpret_cast<SavedataStatsDescriptor**>(
                  g_block0.data() + kSavedataBlockSize - sizeof(void*)) == r.descriptor);
        CHECK(g_alloc_calls == 1);
        CHECK(g_alloc_size == kSavedataBlockSize);
        CHECK(g_alloc_tag == 1 && g_alloc_align == 8 && g_alloc_flags == 2);
        CHECK(g_frees.empty());
    }
    {
        ParentStorage parent{};
        parent.init_map(false);
        SavedataStatsA54920Args args{};
        args.parent = parent.bytes.data();
        args.names = {&n0, &n1, &n2};
        args.descriptor = &in;
        const SavedataStatsA54920Result r = savedata_stats_a54920(args, {});
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataStatsRefusal::null_cursor);
    }
    {
        SavedataStatsA54920Args args{};
        args.sibling_cursor = nullptr;
        args.names = {&n0, &n1, &n2};
        args.descriptor = &in;
        const SavedataStatsA54920Result r = savedata_stats_a54920(args, {});
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataStatsRefusal::null_parent);
    }
    {
        SavedataStatsA54920Args bad{};
        bad.names = {nullptr, &n1, &n2};
        bad.descriptor = &in;
        const SavedataStatsA54920Result r = savedata_stats_a54920(bad, {});
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataStatsRefusal::null_name);
    }
    {
        SavedataStatsA54920Args bad{};
        bad.names = {&n0, &n1, &n2};
        bad.descriptor = nullptr;
        const SavedataStatsA54920Result r = savedata_stats_a54920(bad, {});
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataStatsRefusal::null_descriptor);
    }
    {
        ParentStorage parent{};
        // Deliberately leave the deque fields zero; the slot allocator still succeeds.
        SavedataStatsA54920Args args{};
        args.parent = parent.bytes.data();
        args.sibling_cursor = &parent.cursor;
        args.names = {&n0, &n1, &n2};
        args.descriptor = &in;
        const SavedataStatsA54920Result r = savedata_stats_a54920(args, {});
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataStatsRefusal::invalid_parent_map);
    }
}

void section_previous_head() {
    SavedataStatsName n0 = make_name("save", 1);
    SavedataStatsName n1 = make_name("save", 1);
    SavedataStatsName n2 = make_name("save", 1);
    SavedataStatsInDescriptor in = make_descriptor();

    ParentStorage parent{};
    parent.init_map(false);
    SavedataStatsListNode old_node{};
    SavedataStatsListChild old_child{};
    old_node.child = &old_child;
    old_node.zero18 = 0;
    old_node.index20 = 0xFFFFFFFFu;
    old_child.self0 = &old_child;
    old_child.self8 = &old_child;
    old_child.self10 = &old_child;
    old_child.one18 = 1;
    old_child.one19 = 1;
    parent.slot()->head38 = &old_node;

    reset_alloc();
    SavedataStatsA54920Args args{};
    args.parent = parent.bytes.data();
    args.sibling_cursor = &parent.cursor;
    args.names = {&n0, &n1, &n2};
    args.descriptor = &in;
    args.alloc = test_contract();
    const SavedataStatsA54920Result r = savedata_stats_a54920(args, {});
    CHECK(r.ok);
    CHECK(r.previous_head_destroyed);
    CHECK(r.descriptor->head38 == &r.node);
    CHECK(g_frees.size() == 2);
    CHECK(g_frees[0] == &old_child);
    CHECK(g_frees[1] == &old_node);
    CHECK(old_child.self0 == &old_child && old_child.self8 == &old_child &&
          old_child.self10 == &old_child);
    CHECK(old_node.zero18 == 0);

    // The same cleanup without an allocator cannot free the old child/node.
    ParentStorage missing_parent{};
    missing_parent.init_map(false);
    SavedataStatsListNode missing_node{};
    SavedataStatsListChild missing_child{};
    missing_node.child = &missing_child;
    missing_child.self0 = &missing_child;
    missing_child.self8 = &missing_child;
    missing_child.self10 = &missing_child;
    missing_child.one18 = 1;
    missing_child.one19 = 1;
    missing_parent.slot()->head38 = &missing_node;
    SavedataStatsA54920Args missing_args{};
    missing_args.parent = missing_parent.bytes.data();
    missing_args.sibling_cursor = &missing_parent.cursor;
    missing_args.names = {&n0, &n1, &n2};
    missing_args.descriptor = &in;
    const SavedataStatsA54920Result missing = savedata_stats_a54920(missing_args, {});
    CHECK(!missing.ok);
    CHECK(missing.refusal == SavedataStatsRefusal::previous_free_failed);
    CHECK(!missing.previous_head_destroyed);
}

}  // namespace

int main() {
    section_counters();
    section_old_slots_and_self_link();
    section_grow_and_refusals();
    section_previous_head();
    if (g_failures != 0) {
        std::fprintf(stderr, "%d failure(s)\n", g_failures);
        return 1;
    }
    std::printf("savedata_stats_a54920: all assertions passed (real 0xa55050/0xa57c10/0xa586a0)\n");
    return 0;
}
