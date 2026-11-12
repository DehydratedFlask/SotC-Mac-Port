// Focused fixture for the five formerly hooked neighbours of 0xa54920. The tree nodes below are
// stack/heap fixtures only: they exercise the lifted 0xa58490 release and 0xa57d40 erase against
// valid RB links, without executing any original RVA.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

#include "core/savedata_family.hpp"

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

void set_status(std::uint8_t* base, std::uint32_t index, std::int32_t value) {
    std::uint8_t* slot = base + kSavedataSlotBaseOffset + index * kSavedataSlotStride;
    std::memcpy(slot + kSavedataSlotStatusOffset, &value, sizeof value);
}

void fill_statuses(std::uint8_t* base, std::int32_t value) {
    for (std::uint32_t i = 0; i < kSavedataSlotCount; ++i) {
        set_status(base, i, value);
    }
}

void section_slot_alloc() {
    std::array<std::uint8_t, 0x600> buffer{};
    std::uint8_t* base = buffer.data();
    fill_statuses(base, 1);
    set_status(base, 6, 0);

    std::uint32_t cursor = 5;
    const SavedataSlotAllocResult r = savedata_slot_alloc_a55050(base, &cursor);
    CHECK(r.ok);
    CHECK(r.found);
    CHECK(r.index == 6);
    CHECK(r.slot == base + kSavedataSlotBaseOffset + 6 * kSavedataSlotStride);
    CHECK(r.cursor_before == 5);
    CHECK(r.cursor_after == 7);
    CHECK(cursor == 7);
    CHECK(r.refusal == SavedataFamilyRefusal::none);

    // Wrap: slot 0 occupied, slot 1 free.
    fill_statuses(base, 1);
    set_status(base, 1, 0);
    cursor = 7;
    const SavedataSlotAllocResult wrap = savedata_slot_alloc_a55050(base, &cursor);
    CHECK(wrap.ok);
    CHECK(wrap.found);
    CHECK(wrap.index == 1);
    CHECK(wrap.slot == base + kSavedataSlotBaseOffset + 1 * kSavedataSlotStride);
    CHECK(cursor == 2);

    // Full: fallback is slot 0, cursor unchanged.
    fill_statuses(base, 1);
    cursor = 3;
    const SavedataSlotAllocResult full = savedata_slot_alloc_a55050(base, &cursor);
    CHECK(full.ok);
    CHECK(!full.found);
    CHECK(full.index == 0);
    CHECK(full.slot == base + kSavedataSlotBaseOffset);
    CHECK(full.cursor_after == 3);
    CHECK(cursor == 3);

    const SavedataSlotAllocResult null_parent = savedata_slot_alloc_a55050(nullptr, &cursor);
    CHECK(!null_parent.ok);
    CHECK(null_parent.refusal == SavedataFamilyRefusal::null_argument);
    const SavedataSlotAllocResult null_cursor = savedata_slot_alloc_a55050(base, nullptr);
    CHECK(!null_cursor.ok);
    CHECK(null_cursor.refusal == SavedataFamilyRefusal::null_argument);
}

static std::vector<void*> g_freed;
static bool g_dealloc_ok = true;

bool deallocate_hook(void* p, std::uint32_t tag) noexcept {
    (void)tag;
    g_freed.push_back(p);
    return g_dealloc_ok;
}

AllocDispatchContract dealloc_contract() {
    return AllocDispatchContract{nullptr, deallocate_hook};
}

void clear_links(RbEraseNode& n, RbEraseNode* nil) {
    n.left = nil;
    n.right = nil;
    n.parent = nil;
    n.color = kRbBlack;
    n.nil = 0;
}

void section_tree_release() {
    RbEraseNode header{};
    header.left = &header;
    header.parent = &header;
    header.right = &header;
    header.color = kRbBlack;
    header.nil = 1;

    RbEraseNode n1{}, n2{}, n3{};
    for (RbEraseNode* n : {&n1, &n2, &n3}) clear_links(*n, &header);
    n2.left = &n1;
    n2.right = &n3;
    n1.parent = &n2;
    n3.parent = &n2;

    g_freed.clear();
    g_dealloc_ok = true;
    const SavedataTreeReleaseResult r = savedata_tree_release_a58490(nullptr, &n2, dealloc_contract());
    CHECK(r.ok);
    CHECK(r.refusal == SavedataFamilyRefusal::none);
    CHECK(r.released == 3);
    CHECK(g_freed.size() == 3);
    // 0xa58490 order: recurse right, free node, then left.
    CHECK(g_freed[0] == &n3);
    CHECK(g_freed[1] == &n2);
    CHECK(g_freed[2] == &n1);

    // The sentinel has +0x19 set and returns before any allocator use, so no contract is needed.
    g_freed.clear();
    const SavedataTreeReleaseResult sentinel = savedata_tree_release_a58490(nullptr, &header, {});
    CHECK(sentinel.ok);
    CHECK(sentinel.released == 0);
    CHECK(g_freed.empty());

    const SavedataTreeReleaseResult null_node = savedata_tree_release_a58490(nullptr, nullptr, dealloc_contract());
    CHECK(!null_node.ok);
    CHECK(null_node.refusal == SavedataFamilyRefusal::null_argument);

    const SavedataTreeReleaseResult missing = savedata_tree_release_a58490(nullptr, &n2, {});
    CHECK(!missing.ok);
    CHECK(missing.refusal == SavedataFamilyRefusal::hook_missing);
    CHECK(missing.released == 0);

    // A failing deallocation stops the walk and reports the partial count.
    g_freed.clear();
    g_dealloc_ok = false;
    const SavedataTreeReleaseResult failed = savedata_tree_release_a58490(nullptr, &n2, dealloc_contract());
    CHECK(!failed.ok);
    CHECK(failed.refusal == SavedataFamilyRefusal::hook_rejected);
    CHECK(failed.released == 0);
    CHECK(g_freed.size() == 1 && g_freed[0] == &n3);
    g_dealloc_ok = true;
}

void section_tree_erase() {
    const RbEraseHooks hooks{
        [](std::size_t bytes) noexcept -> void* { return std::malloc(bytes); },
        [](void* p) noexcept { std::free(p); },
    };
    RbEraseTree tree{};
    CHECK(rb_tree_init(tree, 0x38, hooks));

    auto* node = static_cast<RbEraseNode*>(hooks.allocate(sizeof(RbEraseNode)));
    CHECK(node != nullptr);
    std::memset(node, 0, sizeof *node);
    node->left = tree.header;
    node->right = tree.header;
    node->parent = tree.header;
    node->color = kRbBlack;
    node->nil = 0;
    tree.header->parent = node;
    tree.header->left = node;
    tree.header->right = node;
    tree.size = 1;

    RbEraseNode* const next = savedata_tree_erase_a57d40(tree, node, hooks);
    CHECK(next == tree.header);
    CHECK(tree.size == 0);
    CHECK(tree.header->parent == tree.header);
    CHECK(tree.header->left == tree.header);
    CHECK(tree.header->right == tree.header);
    rb_tree_release(tree, hooks);

    // The sentinel is rejected without mutation, as 0x482a90 rejects it.
    RbEraseTree empty{};
    CHECK(rb_tree_init(empty, 0x38, hooks));
    CHECK(savedata_tree_erase_a57d40(empty, empty.header, hooks) == nullptr);
    CHECK(empty.size == 0);
    rb_tree_release(empty, hooks);
}

struct ClearObject {
    std::uint64_t unused;
    RbEraseNode* header;   // +0x8: a57c32 loads this pointer, not an embedded header
    std::uint64_t zero;    // +0x10: a57d11
};
static_assert(offsetof(ClearObject, header) == kSavedataTreeHeaderOffset);
static_assert(offsetof(ClearObject, zero) == kSavedataTreeSizeOffset);

void section_tree_clear() {
    ClearObject object{};
    RbEraseNode header{};
    object.header = &header;
    header.left = &header;
    header.parent = &header;
    header.right = &header;
    header.color = kRbBlack;
    header.nil = 1;

    RbEraseNode n1{}, n2{}, n3{};
    for (RbEraseNode* n : {&n1, &n2, &n3}) clear_links(*n, &header);
    n2.left = &n1;
    n2.right = &n3;
    n1.parent = &n2;
    n3.parent = &n2;
    header.left = &n1;
    header.parent = &n2;   // a57ceb reads [header+0x8]
    header.right = &n3;

    g_freed.clear();
    g_dealloc_ok = true;
    const SavedataTreeClearResult r = savedata_tree_clear_a57c10(&object, dealloc_contract());
    CHECK(r.ok);
    CHECK(r.refusal == SavedataFamilyRefusal::none);
    CHECK(r.release_calls == 1);
    CHECK(r.released_nodes == 3);
    CHECK(g_freed.size() == 3);
    CHECK(g_freed[0] == &n3);
    CHECK(g_freed[1] == &n2);
    CHECK(g_freed[2] == &n1);
    CHECK(r.sentinel_reset);
    CHECK(r.dead_traversal_unreachable);
    CHECK(header.left == &header);
    CHECK(header.parent == &header);
    CHECK(header.right == &header);
    // The body's last write is `[self+0x10] = 0`; it does not change the header pointer.
    CHECK(object.header == &header);
    CHECK(object.zero == 0);

    // Restore the links so the refusal branches are reached with a live root.
    header.left = &n1;
    header.parent = &n2;
    header.right = &n3;
    g_freed.clear();
    const SavedataTreeClearResult missing = savedata_tree_clear_a57c10(&object, {});
    CHECK(!missing.ok);
    CHECK(missing.refusal == SavedataFamilyRefusal::hook_missing);
    CHECK(missing.release_calls == 0);
    CHECK(g_freed.empty());

    ClearObject null_root{};
    RbEraseNode null_header{};
    null_root.header = &null_header;
    null_header.left = &null_header;
    null_header.right = &null_header;
    null_header.nil = 1;
    null_header.parent = nullptr;
    const SavedataTreeClearResult null_subtree =
        savedata_tree_clear_a57c10(&null_root, dealloc_contract());
    CHECK(!null_subtree.ok);
    CHECK(null_subtree.refusal == SavedataFamilyRefusal::null_subtree);

    const SavedataTreeClearResult null_self = savedata_tree_clear_a57c10(nullptr, dealloc_contract());
    CHECK(!null_self.ok);
    CHECK(null_self.refusal == SavedataFamilyRefusal::null_argument);

    // An empty sentinel reaches 0xa58490 and returns before the allocator, so the clearance
    // succeeds even without a host contract.
    ClearObject empty_object{};
    RbEraseNode empty_header{};
    empty_object.header = &empty_header;
    empty_header.left = &empty_header;
    empty_header.parent = &empty_header;
    empty_header.right = &empty_header;
    empty_header.color = kRbBlack;
    empty_header.nil = 1;
    g_freed.clear();
    const SavedataTreeClearResult empty = savedata_tree_clear_a57c10(&empty_object, {});
    CHECK(empty.ok);
    CHECK(empty.released_nodes == 0);
    CHECK(empty.sentinel_reset);
    CHECK(g_freed.empty());
    CHECK(empty_header.left == &empty_header);
    CHECK(empty_header.parent == &empty_header);
    CHECK(empty_header.right == &empty_header);
    CHECK(empty_object.zero == 0);
}

alignas(8) std::array<std::uint8_t, 1024> g_grow_pool{};
alignas(0x100) std::array<std::uint8_t, 0x100> g_blocks[4]{};
std::size_t g_grow_alloc_calls = 0;
std::size_t g_grow_alloc_size = 0;
std::uint32_t g_grow_alloc_tag = 0;
std::uint32_t g_grow_alloc_align = 0;
std::uint32_t g_grow_alloc_flags = 0;
void* g_grow_alloc_ptr = nullptr;
bool g_grow_alloc_ok = true;
std::vector<void*> g_grow_freed;
std::vector<std::uint32_t> g_grow_free_tags;
bool g_grow_free_ok = true;

void* grow_alloc(std::size_t size, std::uint32_t tag, std::uint32_t align,
                 std::uint32_t flags) noexcept {
    ++g_grow_alloc_calls;
    g_grow_alloc_size = size;
    g_grow_alloc_tag = tag;
    g_grow_alloc_align = align;
    g_grow_alloc_flags = flags;
    if (!g_grow_alloc_ok || size > g_grow_pool.size()) return nullptr;
    g_grow_alloc_ptr = g_grow_pool.data();
    return g_grow_alloc_ptr;
}

bool grow_free(void* ptr, std::uint32_t tag) noexcept {
    g_grow_freed.push_back(ptr);
    g_grow_free_tags.push_back(tag);
    return g_grow_free_ok;
}

AllocDispatchContract grow_contract() {
    return AllocDispatchContract{grow_alloc, grow_free};
}

void reset_grow_records() {
    g_grow_alloc_calls = 0;
    g_grow_alloc_size = 0;
    g_grow_alloc_tag = 0;
    g_grow_alloc_align = 0;
    g_grow_alloc_flags = 0;
    g_grow_alloc_ptr = nullptr;
    g_grow_alloc_ok = true;
    g_grow_free_ok = true;
    g_grow_freed.clear();
    g_grow_free_tags.clear();
}

void section_grow() {
    // Mode 1, front space: the current slot moves left by max(1, front/2).
    {
        std::array<std::uint8_t*, 6> slots{};
        for (std::size_t i = 0; i < slots.size(); ++i) slots[i] = g_blocks[i].data();
        SavedataDequeMap map{};
        map.map = slots.data();
        map.map_capacity = slots.size();
        map.start_node = &slots[3];
        map.finish_node = &slots[3];
        map.start_first = slots[3];
        map.start_last = slots[3] + kSavedataBlockSize;
        map.finish_first = slots[3];
        map.finish_last = slots[3] + kSavedataBlockSize;
        reset_grow_records();

        const SavedataGrowResult r = savedata_grow_a586a0(&map, 1, 1, grow_contract());
        CHECK(r.ok);
        CHECK(r.moved && !r.allocated && !r.copied && !r.freed_old);
        CHECK(r.used_nodes == 1);
        CHECK(map.map == slots.data() && map.map_capacity == slots.size());
        CHECK(map.start_node == &slots[2]);
        CHECK(map.finish_node == &slots[2]);
        CHECK(slots[2] == g_blocks[3].data());
        CHECK(map.start_first == g_blocks[3].data());
        CHECK(map.start_last == g_blocks[3].data() + kSavedataBlockSize);
        CHECK(map.finish_first == g_blocks[3].data());
        CHECK(g_grow_alloc_calls == 0 && g_grow_freed.empty());
    }

    // Mode 0, back space with two used slots: the used pair moves right by one.
    {
        std::array<std::uint8_t*, 6> slots{};
        for (std::size_t i = 0; i < slots.size(); ++i) slots[i] = g_blocks[i].data();
        SavedataDequeMap map{};
        map.map = slots.data();
        map.map_capacity = slots.size();
        map.start_node = &slots[2];
        map.finish_node = &slots[3];
        map.start_first = slots[2];
        map.start_last = slots[2] + kSavedataBlockSize;
        map.finish_first = slots[3];
        map.finish_last = slots[3] + kSavedataBlockSize;
        reset_grow_records();

        const SavedataGrowResult r = savedata_grow_a586a0(&map, 1, 0, grow_contract());
        CHECK(r.ok);
        CHECK(r.moved && !r.allocated);
        CHECK(r.used_nodes == 2);
        CHECK(map.start_node == &slots[3]);
        CHECK(map.finish_node == &slots[4]);
        CHECK(slots[3] == g_blocks[2].data());
        CHECK(slots[4] == g_blocks[3].data());
        CHECK(map.start_first == g_blocks[2].data());
        CHECK(map.finish_first == g_blocks[3].data());
        CHECK(g_grow_alloc_calls == 0 && g_grow_freed.empty());
    }

    // Mode 1, no front space: allocate 6 slots, copy one, free the old base.
    {
        std::array<std::uint8_t*, 2> slots{g_blocks[0].data(), g_blocks[1].data()};
        SavedataDequeMap map{};
        map.map = slots.data();
        map.map_capacity = slots.size();
        map.start_node = &slots[0];
        map.finish_node = &slots[0];
        map.start_first = slots[0];
        map.start_last = slots[0] + kSavedataBlockSize;
        map.finish_first = slots[0];
        map.finish_last = slots[0] + kSavedataBlockSize;
        reset_grow_records();

        const SavedataGrowResult r = savedata_grow_a586a0(&map, 1, 1, grow_contract());
        CHECK(r.ok);
        CHECK(r.allocated && r.copied && r.freed_old && !r.moved);
        CHECK(r.new_capacity == 6);
        CHECK(r.node_bytes_moved == sizeof(void*));
        auto* const base = reinterpret_cast<std::uint8_t**>(g_grow_pool.data());
        CHECK(map.map == base && map.map_capacity == 6);
        CHECK(map.start_node == base);
        CHECK(map.finish_node == base);
        CHECK(base[0] == g_blocks[0].data());
        CHECK(map.start_first == g_blocks[0].data());
        CHECK(g_grow_alloc_calls == 1);
        CHECK(g_grow_alloc_size == 6 * sizeof(void*));
        CHECK(g_grow_alloc_tag == 1 && g_grow_alloc_align == 8 && g_grow_alloc_flags == 2);
        CHECK(g_grow_freed.size() == 1 && g_grow_freed[0] == slots.data());
        CHECK(g_grow_free_tags.size() == 1 && g_grow_free_tags[0] == 2);
    }

    // Mode 0 allocation places the current slot after the requested front gap.
    {
        std::array<std::uint8_t*, 2> slots{g_blocks[0].data(), g_blocks[1].data()};
        SavedataDequeMap map{};
        map.map = slots.data();
        map.map_capacity = slots.size();
        map.start_node = &slots[0];
        map.finish_node = &slots[0];
        map.start_first = slots[0];
        map.start_last = slots[0] + kSavedataBlockSize;
        map.finish_first = slots[0];
        map.finish_last = slots[0] + kSavedataBlockSize;
        reset_grow_records();

        const SavedataGrowResult r = savedata_grow_a586a0(&map, 2, 0, grow_contract());
        CHECK(r.ok);
        CHECK(r.allocated && r.copied && r.freed_old);
        CHECK(r.new_capacity == 6);
        auto* const base = reinterpret_cast<std::uint8_t**>(g_grow_pool.data());
        CHECK(map.start_node == base + 2);
        CHECK(map.finish_node == base + 2);
        CHECK(base[2] == g_blocks[0].data());
        CHECK(map.start_first == g_blocks[0].data());
        CHECK(g_grow_alloc_size == 6 * sizeof(void*));
    }

    // Allocation failure leaves the map unchanged.
    {
        std::array<std::uint8_t*, 2> slots{g_blocks[0].data(), g_blocks[1].data()};
        SavedataDequeMap map{};
        map.map = slots.data();
        map.map_capacity = slots.size();
        map.start_node = &slots[0];
        map.finish_node = &slots[0];
        map.start_first = slots[0];
        map.start_last = slots[0] + kSavedataBlockSize;
        map.finish_first = slots[0];
        map.finish_last = slots[0] + kSavedataBlockSize;
        reset_grow_records();
        g_grow_alloc_ok = false;

        const SavedataGrowResult r = savedata_grow_a586a0(&map, 1, 1, grow_contract());
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataFamilyRefusal::allocation_failed);
        CHECK(map.map == slots.data() && map.map_capacity == slots.size());
        CHECK(map.start_node == &slots[0] && map.finish_node == &slots[0]);
        CHECK(g_grow_freed.empty());
    }

    // A failed old-base free is reported, and the new base is released again.
    {
        std::array<std::uint8_t*, 2> slots{g_blocks[0].data(), g_blocks[1].data()};
        SavedataDequeMap map{};
        map.map = slots.data();
        map.map_capacity = slots.size();
        map.start_node = &slots[0];
        map.finish_node = &slots[0];
        map.start_first = slots[0];
        map.start_last = slots[0] + kSavedataBlockSize;
        map.finish_first = slots[0];
        map.finish_last = slots[0] + kSavedataBlockSize;
        reset_grow_records();
        g_grow_free_ok = false;

        const SavedataGrowResult r = savedata_grow_a586a0(&map, 1, 1, grow_contract());
        CHECK(!r.ok);
        CHECK(r.refusal == SavedataFamilyRefusal::hook_rejected);
        CHECK(map.map == slots.data() && map.map_capacity == slots.size());
        CHECK(g_grow_freed.size() == 2);
        CHECK(g_grow_freed[0] == slots.data());
        CHECK(g_grow_freed[1] == g_grow_pool.data());
        CHECK(g_grow_free_tags[0] == 2 && g_grow_free_tags[1] == 2);
    }

    {
        std::array<std::uint8_t*, 2> slots{g_blocks[0].data(), g_blocks[1].data()};
        SavedataDequeMap map{};
        map.map = slots.data();
        map.map_capacity = slots.size();
        map.start_node = &slots[0];
        map.finish_node = &slots[0];
        map.start_first = slots[0];
        map.finish_first = slots[0];
        reset_grow_records();

        const SavedataGrowResult missing = savedata_grow_a586a0(&map, 1, 1, {});
        CHECK(!missing.ok);
        CHECK(missing.refusal == SavedataFamilyRefusal::hook_missing);

        const SavedataGrowResult null_map = savedata_grow_a586a0(nullptr, 1, 1, grow_contract());
        CHECK(!null_map.ok);
        CHECK(null_map.refusal == SavedataFamilyRefusal::null_argument);

        map.start_node = &slots[1];   // front = 1, used = 1, capacity = 1: not enough slots
        map.finish_node = &slots[1];
        map.map_capacity = 1;
        const SavedataGrowResult invalid = savedata_grow_a586a0(&map, 1, 1, grow_contract());
        CHECK(!invalid.ok);
        CHECK(invalid.refusal == SavedataFamilyRefusal::invalid_layout);
    }
}

}  // namespace

int main() {
    section_slot_alloc();
    section_tree_release();
    section_tree_erase();
    section_tree_clear();
    section_grow();
    if (g_failures != 0) {
        std::fprintf(stderr, "%d failure(s)\n", g_failures);
        return 1;
    }
    std::printf("savedata_family: all assertions passed (release order 3,2,1; erase delegated to 0x482a90)\n");
    return 0;
}
