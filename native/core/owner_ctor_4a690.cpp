#include "owner_ctor_4a690.hpp"

#include <atomic>
#include <cstring>
#include <type_traits>

#include "init_sub_3be930.hpp"

namespace sotc {
namespace {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

void put_bytes(void* base, std::size_t offset, const void* source, std::size_t size) noexcept {
    std::memcpy(static_cast<u8*>(base) + offset, source, size);
}

template <typename T>
void put(void* base, std::size_t offset, T value) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    std::memcpy(static_cast<u8*>(base) + offset, &value, sizeof value);
}

void put_pointer(void* base, std::size_t offset, const void* value) noexcept {
    static_assert(sizeof(void*) == sizeof(u64));
    std::memcpy(static_cast<u8*>(base) + offset, &value, sizeof value);
}

template <typename T>
T load(const void* base, std::size_t offset) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    T value{};
    std::memcpy(&value, static_cast<const u8*>(base) + offset, sizeof value);
    return value;
}

constexpr u8 kZero16[16] = {};

// Anchor of the tree owned by `object`: the pointer stored at tree handle + 0x08.
OwnerRow21Node* anchor_of(void* object) noexcept {
    return static_cast<OwnerRow21Node*>(
        load<void*>(object, kOwnerRow21AnchorOffset));
}

// +0x00 of a tree value: the text the compare hook receives. The original loads the qword and
// passes it straight to strcmp; a null pointer faults there (native guard: "null-key").
const char* value_text(const void* value) noexcept {
    return static_cast<const char*>(load<void*>(value, kOwnerRow21KeyTextOffset));
}

const char* node_text(const OwnerRow21Node* node) noexcept {
    return value_text(node->key);
}

// One lower_bound walk, shared by the caller's duplicate test (0x4a754..0x4a7b0) and by
// `0x4b3f0`'s own search (0x4b4f0..0x4b51e). Both walk the same nodes in the same order and call
// the compare hook with the same argument pair `(stored, probe)`.
//
//   compare(stored, probe) <  0  -> descend right (+0x10)       (probe is larger)
//   compare(stored, probe) >= 0  -> remember, descend left (+0x00)
//
// The result is the first node whose key is >= probe, or the anchor when there is none.
struct Search {
    OwnerRow21Node* position;
    const char* failed_at;
};

Search search_lower_bound(OwnerRow21Node* anchor, const char* probe,
                          const OwnerRow21Hooks& hooks) noexcept {
    // Both callers test the root for the anchor first (0x4a75f / 0x4b41e) and take the empty-tree
    // path without any comparison; the guard keeps the anchor's uninitialized value untouched.
    if (anchor->parent == anchor) {
        return {anchor, nullptr};
    }
    OwnerRow21Node* last_left = anchor;
    OwnerRow21Node* node = anchor->parent;
    for (;;) {
        const char* const stored = node_text(node);
        if (stored == nullptr) {
            return {anchor, "null-key"};
        }
        if (hooks.compare == nullptr) {
            return {anchor, "compare"};
        }
        const int order = hooks.compare(stored, probe);
        if (order < 0) {
            node = node->right;
            if (node->is_anchor != 0) {
                return {last_left, nullptr};
            }
        } else {
            last_left = node;
            node = node->left;
            if (node->is_anchor != 0) {
                return {last_left, nullptr};
            }
        }
    }
}

void rotate_left(OwnerRow21Node* anchor, OwnerRow21Node* x) noexcept {
    OwnerRow21Node* const y = x->right;
    x->right = y->left;
    if (y->left->is_anchor == 0) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == anchor) {
        anchor->parent = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rotate_right(OwnerRow21Node* anchor, OwnerRow21Node* x) noexcept {
    OwnerRow21Node* const y = x->left;
    x->left = y->right;
    if (y->right->is_anchor == 0) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == anchor) {
        anchor->parent = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

// `_Lmost`/`_Rmost`: the anchor's left/right children are the extreme nodes. Recomputed from the
// root instead of being patched inside every rotation branch; same invariant, one place to audit.
void refresh_extremes(OwnerRow21Node* anchor) noexcept {
    OwnerRow21Node* node = anchor->parent;
    if (node->is_anchor != 0) {
        anchor->left = anchor;
        anchor->right = anchor;
        return;
    }
    while (node->left->is_anchor == 0) {
        node = node->left;
    }
    anchor->left = node;
    node = anchor->parent;
    while (node->right->is_anchor == 0) {
        node = node->right;
    }
    anchor->right = node;
}

// Red-black insert fixup (0x4c500's rebalance half): the new node is red, a red parent triggers
// recolour or one/two rotations, the root is black and the anchor's extremes track the tree.
void insert_fixup(OwnerRow21Node* anchor, OwnerRow21Node* node) noexcept {
    while (node->parent->colour == kOwnerRow21Red) {
        OwnerRow21Node* const parent = node->parent;
        OwnerRow21Node* const grand = parent->parent;
        if (parent == grand->left) {
            OwnerRow21Node* const uncle = grand->right;
            if (uncle->colour == kOwnerRow21Red) {
                parent->colour = kOwnerRow21Black;
                uncle->colour = kOwnerRow21Black;
                grand->colour = kOwnerRow21Red;
                node = grand;
                continue;
            }
            if (node == parent->right) {
                node = parent;
                rotate_left(anchor, node);
            }
            OwnerRow21Node* const top = node->parent;
            top->colour = kOwnerRow21Black;
            top->parent->colour = kOwnerRow21Red;
            rotate_right(anchor, top->parent);
        } else {
            OwnerRow21Node* const uncle = grand->left;
            if (uncle->colour == kOwnerRow21Red) {
                parent->colour = kOwnerRow21Black;
                uncle->colour = kOwnerRow21Black;
                grand->colour = kOwnerRow21Red;
                node = grand;
                continue;
            }
            if (node == parent->left) {
                node = parent;
                rotate_right(anchor, node);
            }
            OwnerRow21Node* const top = node->parent;
            top->colour = kOwnerRow21Black;
            top->parent->colour = kOwnerRow21Red;
            rotate_left(anchor, top->parent);
        }
    }
    anchor->parent->colour = kOwnerRow21Black;
    refresh_extremes(anchor);
}

// The value copy `0x4b45d..0x4b480`: 0x20 B verbatim, then `lock incq` on the control pointer the
// value carries at +0x18 (the acquire that keeps the SharedStr alive inside the tree).
void copy_value(OwnerRow21Node* node, const void* value) noexcept {
    std::memcpy(node->key, value, kOwnerRow21KeyBytes);
    node->mapped = nullptr;
    void* const control = load<void*>(node->key, kOwnerRow21KeyControlOffset);
    if (control != nullptr) {
        // The guest instruction tolerates any alignment; std::atomic_ref does not (report §10.4).
        auto* const counter = static_cast<u64*>(control);
        static_cast<void>(
            std::atomic_ref<u64>(*counter).fetch_add(1u, std::memory_order_seq_cst));
    }
}

struct Insert {
    void* mapped;           // &node->mapped, or the existing node's slot on an equal key
    bool inserted;
    const char* failed_at;
};

// `0x4b3f0` (`insert_unique(value)`, no hint) plus its `0x4c500` fixup:
//   search -> equivalent-key test -> new(0x48) -> link -> rebalance -> ++size -> &node->mapped.
// The body reaches the equal-key path only if its own duplicate test disagreed, which cannot
// happen; it is modelled anyway because the primitive is shared with other callers.
Insert insert_unique(void* object, const void* value, const OwnerRow21Hooks& hooks) noexcept {
    OwnerRow21Node* const anchor = anchor_of(object);
    const char* const probe = value_text(value);
    if (probe == nullptr) {
        return {nullptr, false, "null-key"};
    }
    const bool empty = (anchor->parent == anchor);
    const Search search = search_lower_bound(anchor, probe, hooks);
    if (search.failed_at != nullptr) {
        return {nullptr, false, search.failed_at};
    }
    if (search.position != anchor) {
        const char* const stored = node_text(search.position);
        if (stored == nullptr) {
            return {nullptr, false, "null-key"};
        }
        if (hooks.compare == nullptr) {
            return {nullptr, false, "compare"};
        }
        if (hooks.compare(probe, stored) >= 0) {
            return {&search.position->mapped, false, nullptr};
        }
    }

    if (hooks.allocate == nullptr) {
        return {nullptr, false, "node_alloc"};
    }
    void* const memory = hooks.allocate(kOwnerRow21NodeSize);
    if (memory == nullptr) {
        return {nullptr, false, "node_alloc"};
    }
    auto* const node = static_cast<OwnerRow21Node*>(memory);
    node->left = anchor;
    node->parent = anchor;
    node->right = anchor;
    node->colour = kOwnerRow21Red;
    node->is_anchor = 0;
    copy_value(node, value);

    if (empty) {
        anchor->parent = node;  // first node: the root
        anchor->left = node;
        anchor->right = node;
    } else if (search.position == anchor) {
        OwnerRow21Node* const last = anchor->right;  // probe is larger than every stored key
        last->right = node;
        node->parent = last;
    } else if (search.position->left->is_anchor != 0) {
        search.position->left = node;
        node->parent = search.position;
    } else {
        OwnerRow21Node* last = search.position->left;
        while (last->right->is_anchor == 0) {
            last = last->right;
        }
        last->right = node;
        node->parent = last;
    }

    insert_fixup(anchor, node);

    // 0x4c7d7 `inc r8` / 0x4c7da `mov [rsi+0x10], r8`: the tree's node count.
    std::size_t count = load<std::size_t>(object, kOwnerRow21SizeOffset);
    ++count;
    put<std::size_t>(object, kOwnerRow21SizeOffset, count);

    return {&node->mapped, true, nullptr};
}

OwnerRow21Hooks g_row21_hooks{};
bool g_row21_hooks_set = false;
const char* g_row21_last_failed_at = nullptr;

}  // namespace

OwnerRow21Result owner_ctor_4a690(void* object, const OwnerRow21Hooks& hooks) noexcept {
    if (object == nullptr) {
        return {false, "null-object", 0, 0};
    }
    if (reinterpret_cast<std::uintptr_t>(object) % alignof(u64) != 0) {
        return {false, "object-align", 0, 0};
    }

    auto* const o = static_cast<u8*>(object);

    // ---- 0x4a6a4..0x4a6b3: three head pointers zeroed (16 B store + one qword). --------------
    put_bytes(o, kOwnerRow21HeadOffset, kZero16, sizeof kZero16);
    put<u64>(o, kOwnerRow21SpareOffset, 0u);

    // ---- 0x4a6bc / 0x4a6c6: the two 0x20 B SharedStr slots, 0xf180 is already native. --------
    init_slot_f180(o + kOwnerRow21Shared0Offset);
    init_slot_f180(o + kOwnerRow21Shared1Offset);

    // ---- 0x4a6cb: the 16 B stamp pair, then the 0x7d03 tag. --------------------------------
    u64 stamp_first = 0;
    u64 stamp_second = 0;
    if (hooks.stamp == nullptr) {
        return {false, "stamp", 0, 0};
    }
    if (!hooks.stamp(stamp_first, stamp_second)) {
        return {false, "stamp", 0, 0};
    }
    put<u64>(o, kOwnerRow21Stamp0Offset, stamp_first);
    put<u64>(o, kOwnerRow21Stamp1Offset, stamp_second);
    put<u32>(o, kOwnerRow21TagOffset, kOwnerCtor4A690Tag);

    // ---- 0x4a6e8: mutex-style sub-object at +0x70 (hook; the body writes nothing there). ----
    if (hooks.mutex_init == nullptr) {
        return {false, "mutex_init", 0, 0};
    }
    if (!hooks.mutex_init(o + kOwnerRow21MutexOffset)) {
        return {false, "mutex_init", 0, 0};
    }

    // ---- 0x4a6f6: zero the tree handle tail (anchor pointer + count), before the allocation. -
    put_bytes(o, kOwnerRow21AnchorOffset, kZero16, sizeof kZero16);

    // ---- 0x4a700: new(0x48) anchor, self-linked, black and marked as the anchor. ------------
    if (hooks.allocate == nullptr) {
        return {false, "anchor_alloc", 0, 0};
    }
    void* const anchor_memory = hooks.allocate(kOwnerRow21NodeSize);
    if (anchor_memory == nullptr) {
        return {false, "anchor_alloc", 0, 0};
    }
    auto* const anchor = static_cast<OwnerRow21Node*>(anchor_memory);
    anchor->left = anchor;
    anchor->parent = anchor;
    anchor->right = anchor;
    anchor->colour = kOwnerRow21Black;
    anchor->is_anchor = 1;
    put_pointer(o, kOwnerRow21AnchorOffset, anchor);

    // ---- 0x4a72d: drain *(0x2ea8980) into the tree. ------------------------------------------
    if (hooks.queue_head == nullptr || hooks.set_queue_head == nullptr) {
        return {false, "queue", 0, 0};
    }
    const OwnerRow21QueueNode* entry = hooks.queue_head();
    if (entry == nullptr) {
        return {true, nullptr, 0, 0};
    }

    std::size_t inserted = 0;
    std::size_t duplicates = 0;
    for (;;) {
        const void* const value =
            reinterpret_cast<const u8*>(entry) + kOwnerRow21QueueValueOffset;
        const char* const probe = value_text(value);
        if (probe == nullptr) {
            return {false, "null-key", inserted, duplicates};
        }

        // ---- 0x4a739..0x4a7e7: lower_bound descent, then the equivalent-key test. ----------
        const Search search = search_lower_bound(anchor, probe, hooks);
        if (search.failed_at != nullptr) {
            return {false, search.failed_at, inserted, duplicates};
        }
        bool present = false;
        if (search.position != anchor) {
            const char* const stored = node_text(search.position);
            if (stored == nullptr) {
                return {false, "null-key", inserted, duplicates};
            }
            if (hooks.compare == nullptr) {
                return {false, "compare", inserted, duplicates};
            }
            // 0x4a7ca/0x4a7d1: the *swapped* argument order, probe first.
            present = hooks.compare(probe, stored) >= 0;
        }

        if (!present) {
            // ---- 0x4a7f3: insert_unique(this+0x78, &entry->value). -------------------------
            const Insert insert = insert_unique(object, value, hooks);
            if (insert.failed_at != nullptr) {
                return {false, insert.failed_at, inserted, duplicates};
            }
            // ---- 0x4a7ff: *mapped = entry. -------------------------------------------------
            put_pointer(insert.mapped, 0, entry);
            if (insert.inserted) {
                ++inserted;
            } else {
                ++duplicates;
            }
        } else {
            // ---- 0x4a7ed -> 0x4a805: equal key, no node, no store. -------------------------
            ++duplicates;
        }

        // ---- 0x4a802..0x4a809: re-read the head, advance it past this entry. --------------
        const OwnerRow21QueueNode* const current = hooks.queue_head();
        if (current == nullptr) {
            return {false, "queue", inserted, duplicates};
        }
        const OwnerRow21QueueNode* const next = current->next;
        hooks.set_queue_head(next);
        if (next == nullptr) {
            return {true, nullptr, inserted, duplicates};
        }
        entry = next;
    }
}

void owner_row21_set_hooks(const OwnerRow21Hooks& hooks) noexcept {
    g_row21_hooks = hooks;
    g_row21_hooks_set = true;
    g_row21_last_failed_at = nullptr;
}

bool owner_row21_member_ctor(std::size_t row, void* object, void* arg) noexcept {
    static_cast<void>(arg);  // row 21's ctor takes no constructor argument
    if (row != kOwnerMemberRow21) {
        g_row21_last_failed_at = "row";
        return false;
    }
    if (!g_row21_hooks_set) {
        g_row21_last_failed_at = "hooks";
        return false;
    }
    const OwnerRow21Result result = owner_ctor_4a690(object, g_row21_hooks);
    g_row21_last_failed_at = result.ok ? nullptr : result.failed_at;
    return result.ok;
}

const char* owner_row21_last_failed_at() noexcept { return g_row21_last_failed_at; }

}  // namespace sotc
