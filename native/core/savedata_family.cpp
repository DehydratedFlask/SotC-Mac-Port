#include "savedata_family.hpp"

#include <algorithm>
#include <cstring>
#include <limits>

namespace sotc {

static_assert(kSavedataSlotCursorRva == 0x32F88A0, "cursor cell must stay pinned");
static_assert(kSavedataTreeEraseA57D40Size == kSavedataRbEraseDuplicateSize,
              "0xa57d40 and 0x482a90 must stay the same length");

namespace {

struct ReleaseWalk {
    const AllocDispatchContract* contract = nullptr;
    std::size_t released = 0;
};

// 0xa58490 loop body: recurse into +0x10, free this node, continue at +0x00. Both links are read
// before the free because the original reads them into registers first. A null link would fault
// in the original at the next +0x19 load, so the model stops with a failure instead.
bool release_walk(RbEraseNode* node, ReleaseWalk& walk) noexcept {
    while (node != nullptr && node->nil == 0) {
        RbEraseNode* const right = node->right;
        RbEraseNode* const left = node->left;
        if (right == nullptr || left == nullptr) return false;
        if (!release_walk(right, walk)) return false;
        if (!free_170b0(node, *walk.contract).ok) return false;
        ++walk.released;
        node = left;
    }
    return true;
}

}  // namespace

SavedataSlotAllocResult savedata_slot_alloc_a55050(std::uint8_t* parent,
                                                   std::uint32_t* cursor) noexcept {
    SavedataSlotAllocResult out{};
    if (parent == nullptr || cursor == nullptr) {
        out.refusal = SavedataFamilyRefusal::null_argument;
        return out;
    }

    out.cursor_before = *cursor;
    out.cursor_after = *cursor;

    for (std::uint32_t i = 0; i < kSavedataSlotCount; ++i) {
        const std::uint32_t index = (*cursor + i) % static_cast<std::uint32_t>(kSavedataSlotCount);
        std::uint8_t* slot = parent + kSavedataSlotBaseOffset + index * kSavedataSlotStride;
        std::int32_t status = 0;
        std::memcpy(&status, slot + kSavedataSlotStatusOffset, sizeof status);
        if (status == 0) {
            *cursor = index + 1;
            out.ok = true;
            out.slot = slot;
            out.index = index;
            out.found = true;
            out.cursor_after = *cursor;
            return out;
        }
    }

    // All eight occupied: the original returns the first slot without touching the cursor.
    out.ok = true;
    out.slot = parent + kSavedataSlotBaseOffset;
    out.index = 0;
    out.found = false;
    return out;
}

SavedataTreeReleaseResult savedata_tree_release_a58490(
    void* self, RbEraseNode* node, const AllocDispatchContract& contract) noexcept {
    (void)self;   // threaded through a58490's rdi for recursion; only the node is freed
    SavedataTreeReleaseResult out{};
    if (node == nullptr) {
        // The original reads +0x19 immediately (a584a0) and would fault on null.
        out.refusal = SavedataFamilyRefusal::null_argument;
        return out;
    }
    if (node->nil != 0) {
        // a584a0..a584a4: the sentinel returns before any allocator use.
        out.ok = true;
        return out;
    }
    if (contract.deallocate == nullptr) {
        out.refusal = SavedataFamilyRefusal::hook_missing;
        return out;
    }

    ReleaseWalk walk{&contract, 0};
    const bool complete = release_walk(node, walk);
    out.released = walk.released;
    if (!complete) {
        out.refusal = SavedataFamilyRefusal::hook_rejected;
        return out;
    }
    out.ok = true;
    return out;
}

RbEraseNode* savedata_tree_erase_a57d40(RbEraseTree& tree, RbEraseNode* node,
                                        const RbEraseHooks& hooks) noexcept {
    // Byte-identical body: 340/340 normalized instructions, same two call targets and the same
    // assert text (only the string address differs). One model, not a second rebalance.
    return rb_erase_482a90(tree, node, hooks);
}

SavedataTreeClearResult savedata_tree_clear_a57c10(
    void* self, const AllocDispatchContract& contract) noexcept {
    SavedataTreeClearResult out{};
    if (self == nullptr) {
        out.refusal = SavedataFamilyRefusal::null_argument;
        return out;
    }

    auto* object = static_cast<std::uint8_t*>(self);
    RbEraseNode* header = nullptr;
    std::memcpy(&header, object + kSavedataTreeHeaderOffset, sizeof header);   // a57c32
    if (header == nullptr) {
        out.refusal = SavedataFamilyRefusal::null_subtree;
        return out;
    }

    // a57ceb: rsi = [header+0x8], the tree root. 0xa58490(self, root). The preceding
    // load/compare at a57c37..a57c3f is tautological, so this cleanup path is the only entry.
    RbEraseNode* const root = header->parent;
    if (root == nullptr) {
        out.refusal = SavedataFamilyRefusal::null_subtree;
        return out;
    }
    if (root->nil == 0 && contract.deallocate == nullptr) {
        out.refusal = SavedataFamilyRefusal::hook_missing;
        return out;
    }

    const SavedataTreeReleaseResult released =
        savedata_tree_release_a58490(self, root, contract);
    ++out.release_calls;
    out.released_nodes = released.released;
    if (!released.ok) {
        // Do not pretend the cleanup completed or drop the header links after a partial release.
        out.refusal = released.refusal;
        return out;
    }

    // a57cf7..a57d0d stores to header+0x8, header+0x0 and header+0x10; a57d11 zeroes self+0x10.
    header->parent = header;
    header->left = header;
    header->right = header;
    const std::uint64_t zero = 0;
    std::memcpy(object + kSavedataTreeSizeOffset, &zero, sizeof zero);

    out.ok = true;
    out.sentinel_reset = true;
    return out;
}

SavedataGrowResult savedata_grow_a586a0(SavedataDequeMap* map, std::size_t requested, int mode,
                                         const AllocDispatchContract& contract) noexcept {
    SavedataGrowResult out{};
    if (map == nullptr) {
        out.refusal = SavedataFamilyRefusal::null_argument;
        return out;
    }

    // a586ba..a586da: start.node index, used node count and the used-node byte span.
    std::uint8_t** const map_base = map->map;
    std::uint8_t** const start_node = map->start_node;
    std::uint8_t** const finish_node = map->finish_node;
    if (map_base == nullptr || start_node == nullptr || finish_node == nullptr) {
        out.refusal = SavedataFamilyRefusal::invalid_layout;
        return out;
    }
    const auto base_addr = reinterpret_cast<std::uintptr_t>(map_base);
    const auto start_addr = reinterpret_cast<std::uintptr_t>(start_node);
    const auto finish_addr = reinterpret_cast<std::uintptr_t>(finish_node);
    if ((base_addr % sizeof(void*)) != 0 || (start_addr % sizeof(void*)) != 0 ||
        (finish_addr % sizeof(void*)) != 0 || start_node < map_base ||
        finish_node < start_node) {
        out.refusal = SavedataFamilyRefusal::invalid_layout;
        return out;
    }

    const auto front = static_cast<std::size_t>(start_node - map_base);
    const std::size_t used_nodes = static_cast<std::size_t>(finish_node - start_node) + 1;
    const std::size_t node_bytes = used_nodes * sizeof(void*);
    if (front > map->map_capacity || used_nodes > map->map_capacity - front) {
        out.refusal = SavedataFamilyRefusal::invalid_layout;
        return out;
    }
    if (start_node[0] == nullptr || finish_node[0] == nullptr) {
        out.refusal = SavedataFamilyRefusal::invalid_layout;
        return out;
    }

    out.used_nodes = used_nodes;
    out.new_capacity = map->map_capacity;
    std::uint8_t** new_start_node = nullptr;

    if (mode == 1 && front >= requested) {
        // a586ec..a58705: shift the used node pointers left by max(requested, front/2).
        const std::size_t shift = std::max(requested, front / 2);
        new_start_node = map_base + (front - shift);
        std::memmove(new_start_node, start_node, node_bytes);   // 0xf8f480 = memmove
        out.moved = true;
    } else {
        const std::size_t free_tail = map->map_capacity - front - used_nodes;
        if (mode == 0 && free_tail >= requested) {
            // a58722..a5872c: shift the used node pointers right by max(requested, free/2).
            const std::size_t shift = std::max(requested, free_tail / 2);
            new_start_node = start_node + shift;
            std::memmove(new_start_node, start_node, node_bytes);
            out.moved = true;
        } else {
            // a58746..a587c7: allocate a larger node-slot array, copy the used node pointers,
            // free the old base and commit the new base/capacity.
            const std::size_t maxv = std::max(map->map_capacity, requested);
            const std::size_t limit = std::numeric_limits<std::size_t>::max();
            if (maxv > limit - 2 || map->map_capacity > limit - maxv - 2 ||
                map->map_capacity + maxv + 2 > limit / sizeof(void*)) {
                out.refusal = SavedataFamilyRefusal::allocation_failed;
                return out;
            }
            const std::size_t new_capacity = map->map_capacity + maxv + 2;
            const AllocWrapperResult allocated =
                alloc_17090(new_capacity * sizeof(void*), contract);   // 0x17090
            if (!allocated.ok) {
                out.refusal = allocated.dispatched ? SavedataFamilyRefusal::allocation_failed
                                                   : SavedataFamilyRefusal::hook_missing;
                return out;
            }

            auto* const base = static_cast<std::uint8_t**>(allocated.pointer);
            const std::size_t front_gap = (mode == 0) ? requested : 0;
            if (front_gap > limit - front) {
                (void)free_170c0(base, contract);
                out.refusal = SavedataFamilyRefusal::allocation_failed;
                return out;
            }
            new_start_node = base + front + front_gap;
            std::memcpy(new_start_node, start_node, node_bytes);   // 0xf8f470 = memcpy
            out.copied = true;
            const AllocWrapperResult freed = free_170c0(map_base, contract);   // 0x170c0
            if (!freed.ok) {
                (void)free_170c0(base, contract);
                out.refusal = SavedataFamilyRefusal::hook_rejected;
                return out;
            }
            out.freed_old = true;
            map->map = base;
            map->map_capacity = new_capacity;
            out.allocated = true;
            out.new_capacity = new_capacity;
        }
    }

    map->start_node = new_start_node;
    map->finish_node = new_start_node + used_nodes - 1;
    map->start_first = new_start_node[0];
    map->start_last = map->start_first + kSavedataBlockSize;
    map->finish_first = new_start_node[used_nodes - 1];
    map->finish_last = map->finish_first + kSavedataBlockSize;

    out.node_bytes_moved = node_bytes;
    out.ok = true;
    return out;
}

}  // namespace sotc
