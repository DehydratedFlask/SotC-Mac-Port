// Fixture for the row-21 owner constructor `0x4a690` (0x90 B object, pointer slot 0x2ea8978).
//
// The characterization is `local/re/evidence/owner-row21-4a690/report.md`; this fixture is the
// falsifiable side of it. It drives the drain of the global queue `*(0x2ea8980)` into the object's
// tree and asserts: every written byte and every untouched hole, the hook call order/arguments,
// the per-entry lower_bound + equivalent-key compare sequence (stored, probe) and (probe, stored),
// the final node count, the in-order key order, every root/link/colour value, the red-black
// invariants, the key control refcount acquires, duplicate (first-wins) behaviour, ten failure
// surfaces with their exact prefix, a second call, and the guard ring.
//
// Byte-exact ELF pins live in `tools/lift/check_fowner_ctor_4a690.py`.
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

#include "core/init_sub_3be930.hpp"
#include "core/owner_ctor_4a690.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

constexpr std::size_t kObjBytes = kOwnerCtor4A690ObjSize;
constexpr std::size_t kGuardBytes = 0x40;
constexpr std::uint8_t kFill = 0xCC;
constexpr std::uint8_t kGuardFill = 0xA5;
constexpr std::size_t kArenaNodes = 40;
constexpr std::uint8_t kNodeFill = 0x5E;
constexpr std::uint8_t kQueueFill = 0x77;

// ---------------------------------------------------------------------------------------------
// Host stand-ins: a node arena behind 0x17070, a queue behind *(0x2ea8980), and the four value
// hooks. Every hook records its arguments; nothing is synthesised.
// ---------------------------------------------------------------------------------------------
struct CompareCall {
    std::string stored;
    std::string probe;
};

struct Ctx {
    alignas(8) std::uint8_t arena[kArenaNodes * kOwnerRow21NodeSize];
    std::size_t arena_used = 0;
    std::size_t alloc_calls = 0;
    std::size_t alloc_sizes_ok = 0;
    std::size_t alloc_fail_at = 0;  // 1-based allocation index that returns nullptr

    alignas(8) std::uint8_t queue[kArenaNodes * kOwnerRow21QueueNodeSize];
    std::uint64_t controls[kArenaNodes];
    std::string texts[kArenaNodes];
    const OwnerRow21QueueNode* head = nullptr;
    std::size_t queue_head_calls = 0;
    std::size_t queue_head_fail_at = 0;  // 1-based read index that returns nullptr
    std::vector<const OwnerRow21QueueNode*> head_history;

    std::size_t stamp_calls = 0;
    std::size_t mutex_calls = 0;
    const void* mutex_at = nullptr;
    bool mutex_writes = true;
    std::uint64_t stamp_first = 0x1122334455667788ull;
    std::uint64_t stamp_second = 0x99AABBCCDDEEFF00ull;

    std::vector<CompareCall> compares;
};

Ctx g;

void reset_ctx() {
    std::memset(g.arena, kNodeFill, sizeof g.arena);
    g.arena_used = 0;
    g.alloc_calls = 0;
    g.alloc_sizes_ok = 0;
    g.alloc_fail_at = 0;
    std::memset(g.queue, kQueueFill, sizeof g.queue);
    for (std::uint64_t& control : g.controls) {
        control = 1;
    }
    for (std::string& text : g.texts) {
        text.clear();
    }
    g.head = nullptr;
    g.queue_head_calls = 0;
    g.queue_head_fail_at = 0;
    g.head_history.clear();
    g.stamp_calls = 0;
    g.mutex_calls = 0;
    g.mutex_at = nullptr;
    g.mutex_writes = true;
    g.compares.clear();
}

// 0x17070: new(0x48). Fails on demand so a mid-construction allocation failure is testable.
void* allocate(std::size_t size) {
    ++g.alloc_calls;
    if (size == kOwnerRow21NodeSize) {
        ++g.alloc_sizes_ok;
    }
    if (g.alloc_fail_at != 0 && g.alloc_calls == g.alloc_fail_at) {
        return nullptr;
    }
    if (g.arena_used + kOwnerRow21NodeSize > sizeof g.arena) {
        return nullptr;
    }
    void* const memory = g.arena + g.arena_used;
    g.arena_used += kOwnerRow21NodeSize;
    return memory;
}

// 0xf8f500: strcmp. Records both arguments in call order.
int compare(const char* stored, const char* probe) {
    CHECK(stored != nullptr);
    CHECK(probe != nullptr);
    g.compares.push_back(CompareCall{stored, probe});
    return std::strcmp(stored, probe);
}

bool stamp(std::uint64_t& first, std::uint64_t& second) {
    ++g.stamp_calls;
    first = g.stamp_first;
    second = g.stamp_second;
    return true;
}

// 0x3b310: writes its own 0x10 B region; the constructor must not touch +0x70..+0x80 itself.
bool mutex_init(void* at) {
    ++g.mutex_calls;
    g.mutex_at = at;
    if (g.mutex_writes) {
        for (std::size_t i = 0; i < 0x10; ++i) {
            static_cast<std::uint8_t*>(at)[i] = static_cast<std::uint8_t>(0x40 + i);
        }
    }
    return true;
}

const OwnerRow21QueueNode* queue_head() {
    ++g.queue_head_calls;
    g.head_history.push_back(g.head);
    if (g.queue_head_fail_at != 0 && g.queue_head_calls == g.queue_head_fail_at) {
        return nullptr;
    }
    return g.head;
}

void set_queue_head(const OwnerRow21QueueNode* next) { g.head = next; }

OwnerRow21Hooks make_hooks() {
    OwnerRow21Hooks hooks{};
    hooks.compare = &compare;
    hooks.allocate = &allocate;
    hooks.stamp = &stamp;
    hooks.mutex_init = &mutex_init;
    hooks.queue_head = &queue_head;
    hooks.set_queue_head = &set_queue_head;
    return hooks;
}

// ---------------------------------------------------------------------------------------------
// Queue builder: keys in list order, one 0x30 B entry each, `head` = the first one. The two middle
// value qwords carry a per-entry pattern so the verbatim 0x20 B copy is visible.
// ---------------------------------------------------------------------------------------------
void build_queue(const std::vector<const char*>& keys) {
    CHECK(keys.size() <= kArenaNodes);
    std::vector<const OwnerRow21QueueNode*> nodes(keys.size(), nullptr);
    for (std::size_t i = 0; i < keys.size(); ++i) {
        g.texts[i] = keys[i];
        auto* const node = reinterpret_cast<OwnerRow21QueueNode*>(
            g.queue + i * kOwnerRow21QueueNodeSize);
        std::memset(node, kQueueFill, kOwnerRow21QueueNodeSize);
        auto* const value = reinterpret_cast<std::uint8_t*>(node) + kOwnerRow21QueueValueOffset;
        const char* const text = g.texts[i].c_str();
        std::memcpy(value + kOwnerRow21KeyTextOffset, &text, sizeof text);
        for (std::size_t b = 0x08; b < 0x10; ++b) {
            value[b] = static_cast<std::uint8_t>(0xB0 + i);
        }
        for (std::size_t b = 0x10; b < 0x18; ++b) {
            value[b] = static_cast<std::uint8_t>(0xC0 + i);
        }
        void* const control = &g.controls[i];
        std::memcpy(value + kOwnerRow21KeyControlOffset, &control, sizeof control);
        nodes[i] = node;
    }
    for (std::size_t i = 0; i + 1 < nodes.size(); ++i) {
        auto* const node = const_cast<OwnerRow21QueueNode*>(nodes[i]);
        node->next = const_cast<OwnerRow21QueueNode*>(nodes[i + 1]);
    }
    if (!nodes.empty()) {
        auto* const last = const_cast<OwnerRow21QueueNode*>(nodes.back());
        last->next = nullptr;
    }
    g.head = nodes.empty() ? nullptr : nodes.front();
}

OwnerRow21QueueNode* queue_at(std::size_t index) {
    return reinterpret_cast<OwnerRow21QueueNode*>(g.queue + index * kOwnerRow21QueueNodeSize);
}

const char* entry_text(const OwnerRow21QueueNode& entry) {
    const char* text = nullptr;
    std::memcpy(&text, entry.value + kOwnerRow21KeyTextOffset, sizeof text);
    return text;
}

// ---------------------------------------------------------------------------------------------
// Tree inspection.
// ---------------------------------------------------------------------------------------------
struct Obj {
    alignas(8) std::uint8_t storage[kGuardBytes + kObjBytes + kGuardBytes];

    explicit Obj(std::uint8_t fill = kFill) { std::memset(storage, fill, sizeof storage); }
    [[nodiscard]] std::uint8_t* ptr() { return storage + kGuardBytes; }
    [[nodiscard]] const std::uint8_t* ptr() const { return storage + kGuardBytes; }
};

std::uint8_t* obj_bytes(Obj& buffer) { return buffer.ptr(); }
const std::uint8_t* obj_bytes(const Obj& buffer) { return buffer.ptr(); }

void check_guards(const Obj& buffer, std::uint8_t fill) {
    for (std::size_t i = 0; i < kGuardBytes; ++i) {
        CHECK(buffer.storage[i] == fill);
        CHECK(buffer.storage[kGuardBytes + kObjBytes + i] == fill);
    }
}

template <typename T>
T read_at(const std::uint8_t* base, std::size_t offset) {
    T value{};
    std::memcpy(&value, base + offset, sizeof value);
    return value;
}

const OwnerRow21Node* anchor_of(const std::uint8_t* object) {
    return read_at<const OwnerRow21Node*>(object, kOwnerRow21AnchorOffset);
}

const OwnerRow21Node* root_of(const std::uint8_t* object) { return anchor_of(object)->parent; }

std::size_t size_of(const std::uint8_t* object) {
    return read_at<std::size_t>(object, kOwnerRow21SizeOffset);
}

const char* key_of(const OwnerRow21Node* node) {
    return read_at<const char*>(node->key, kOwnerRow21KeyTextOffset);
}

void in_order(const OwnerRow21Node* node, std::vector<const OwnerRow21Node*>& out) {
    if (node->is_anchor != 0) {
        return;
    }
    in_order(node->left, out);
    out.push_back(node);
    in_order(node->right, out);
}

std::vector<const OwnerRow21Node*> in_order(const std::uint8_t* object) {
    std::vector<const OwnerRow21Node*> out;
    in_order(root_of(object), out);
    return out;
}

std::string key_or_anchor(const OwnerRow21Node* node) {
    return node->is_anchor != 0 ? std::string("A") : std::string(key_of(node));
}

// Canonical shape: one entry per node in pre-order (`key:colour(parent,left,right)`), then the
// anchor line (`A:colour(parent,left,right)`). An empty link prints `A`.
std::string shape(const std::uint8_t* object) {
    std::string out;
    std::vector<const OwnerRow21Node*> stack;
    const OwnerRow21Node* const anchor = anchor_of(object);
    if (anchor->parent != anchor) {
        stack.push_back(anchor->parent);
    }
    while (!stack.empty()) {
        const OwnerRow21Node* const node = stack.back();
        stack.pop_back();
        out += key_or_anchor(node);
        out += ':';
        out += (node->colour == kOwnerRow21Black ? 'B' : 'R');
        out += '(';
        out += key_or_anchor(node->parent);
        out += ',';
        out += key_or_anchor(node->left);
        out += ',';
        out += key_or_anchor(node->right);
        out += ") ";
        if (node->right->is_anchor == 0) {
            stack.push_back(node->right);
        }
        if (node->left->is_anchor == 0) {
            stack.push_back(node->left);
        }
    }
    out += "A:";
    out += (anchor->colour == kOwnerRow21Black ? 'B' : 'R');
    out += '(';
    out += key_or_anchor(anchor->parent);
    out += ',';
    out += key_or_anchor(anchor->left);
    out += ',';
    out += key_or_anchor(anchor->right);
    out += ')';
    return out;
}

std::string compare_log() {
    std::string out;
    for (const CompareCall& call : g.compares) {
        if (!out.empty()) {
            out += '|';
        }
        out += call.stored;
        out += ':';
        out += call.probe;
    }
    return out;
}

// Red-black invariants plus the parent/left/right consistency and the in-order ordering.
bool rb_invariants(const OwnerRow21Node* anchor) {
    if (anchor->parent == anchor) {
        return anchor->left == anchor && anchor->right == anchor;
    }
    if (anchor->parent->colour != kOwnerRow21Black || anchor->parent->parent != anchor) {
        return false;
    }
    std::vector<const OwnerRow21Node*> nodes;
    in_order(anchor->parent, nodes);
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        const OwnerRow21Node* const node = nodes[i];
        if (i > 0 && std::strcmp(key_of(nodes[i - 1]), key_of(node)) >= 0) {
            return false;  // in-order must be strictly ascending
        }
        if (node->is_anchor != 0 || node->mapped == nullptr) {
            return false;
        }
        if (node->left->is_anchor == 0 && node->left->parent != node) {
            return false;
        }
        if (node->right->is_anchor == 0 && node->right->parent != node) {
            return false;
        }
        if (node->colour == kOwnerRow21Red &&
            (node->left->colour == kOwnerRow21Red || node->right->colour == kOwnerRow21Red)) {
            return false;
        }
    }
    if (nodes.empty() || anchor->left != nodes.front() || anchor->right != nodes.back()) {
        return false;
    }
    struct Frame {
        const OwnerRow21Node* node;
        int black;
    };
    std::vector<Frame> stack;
    stack.push_back(Frame{anchor->parent, 0});
    int leaf_black = -1;
    while (!stack.empty()) {
        const Frame frame = stack.back();
        stack.pop_back();
        const int black = frame.black + (frame.node->colour == kOwnerRow21Black ? 1 : 0);
        for (const OwnerRow21Node* child : {frame.node->left, frame.node->right}) {
            if (child->is_anchor != 0) {
                if (leaf_black < 0) {
                    leaf_black = black;
                } else if (leaf_black != black) {
                    return false;
                }
            } else {
                stack.push_back(Frame{child, black});
            }
        }
    }
    return leaf_black >= 0;
}

// Stages of the written prefix: 0 = head + both 0xf180 slots, 1 = + the stamp pair and the tag,
// 3 = + the zeroed tree handle tail at +0x80. (+0x70..+0x80 is hook-owned and never written.)
bool object_offset_written(std::size_t offset, std::size_t stage) {
    if (offset < 0x58) {
        return true;
    }
    if (offset < 0x6C) {
        return stage >= 1;
    }
    if (offset < 0x80) {
        return false;  // the +0x68 tag's hole and the mutex region
    }
    return stage >= 3;
}

void expect_prefix_bytes(const Obj& buffer, std::uint8_t stage) {
    const std::uint8_t* const obj = obj_bytes(buffer);
    for (std::size_t i = 0; i < kObjBytes; ++i) {
        if (i >= kOwnerRow21MutexOffset && i < kOwnerRow21AnchorOffset) {
            continue;
        }
        if (object_offset_written(i, stage)) {
            continue;
        }
        if (obj[i] != kFill) {
            std::fprintf(stderr, "FAIL prefix: +0x%zx got 0x%02x want fill 0x%02x (stage %u)\n",
                         i, obj[i], kFill, stage);
        }
        CHECK(obj[i] == kFill);
    }
}

// ---------------------------------------------------------------------------------------------
// 1. Layout constants and the ELF-pinned numbers.
// ---------------------------------------------------------------------------------------------
void section_constants() {
    CHECK(kOwnerCtor4A690Rva == 0x4A690);
    CHECK(kOwnerCtor4A690Size == 0x194);
    CHECK(kOwnerCtor4A690InsnCount == 104);
    CHECK(kOwnerCtor4A690PadNopCount == 4);
    CHECK(kOwnerCtor4A690ObjSize == 0x90);
    CHECK(kOwnerCtor4A690SlotRva == 0x2EA8978);
    CHECK(kOwnerCtor4A690QueueHeadRva == 0x2EA8980);
    CHECK(kOwnerCtor4A690Tag == 0x7D03);
    CHECK(kOwnerMemberRow21 == 21);
    CHECK(sizeof(OwnerRow21Node) == 0x48);
    CHECK(offsetof(OwnerRow21Node, left) == 0x00);
    CHECK(offsetof(OwnerRow21Node, parent) == 0x08);
    CHECK(offsetof(OwnerRow21Node, right) == 0x10);
    CHECK(offsetof(OwnerRow21Node, colour) == 0x18);
    CHECK(offsetof(OwnerRow21Node, is_anchor) == 0x19);
    CHECK(offsetof(OwnerRow21Node, key) == 0x20);
    CHECK(offsetof(OwnerRow21Node, mapped) == 0x40);
    CHECK(sizeof(OwnerRow21QueueNode) == 0x30);
    CHECK(offsetof(OwnerRow21QueueNode, value) == 0x08);
    CHECK(offsetof(OwnerRow21QueueNode, next) == 0x28);
    CHECK(kOwnerRow21TreeOffset + 0x08 == kOwnerRow21AnchorOffset);
    CHECK(kOwnerRow21TreeOffset + 0x10 == kOwnerRow21SizeOffset);
    CHECK(kOwnerRow21AnchorOffset + 0x08 == kOwnerRow21SizeOffset);
    CHECK(kOwnerRow21SizeOffset + 0x08 == kObjBytes);
    std::printf("PASS 1 constants: body 0x194/104 insns, object 0x90, node 0x48, queue 0x30\n");
}

// ---------------------------------------------------------------------------------------------
// 2. Empty queue: the whole object minus the tree, the anchor, and the exact hook call order.
// ---------------------------------------------------------------------------------------------
void section_empty() {
    reset_ctx();
    Obj buffer;
    build_queue({});
    const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
    CHECK(result.ok);
    CHECK(result.failed_at == nullptr);
    CHECK(result.inserted == 0);
    CHECK(result.duplicates == 0);

    // Hook order: stamp once, mutex once at +0x70, one 0x48 allocation for the anchor.
    CHECK(g.stamp_calls == 1);
    CHECK(g.mutex_calls == 1);
    CHECK(g.mutex_at == obj_bytes(buffer) + kOwnerRow21MutexOffset);
    CHECK(g.alloc_calls == 1);
    CHECK(g.alloc_sizes_ok == 1);
    CHECK(g.compares.empty());  // no comparison on an empty tree
    CHECK(g.queue_head_calls == 1);
    CHECK(g.head == nullptr);

    const std::uint8_t* const obj = obj_bytes(buffer);
    for (std::size_t i = 0; i < 0x10; ++i) {
        CHECK(obj[i] == 0);
    }
    CHECK(read_at<std::uint64_t>(obj, 0x10) == 0);
    CHECK(read_at<std::uint64_t>(obj, 0x58) == g.stamp_first);
    CHECK(read_at<std::uint64_t>(obj, 0x60) == g.stamp_second);
    CHECK(read_at<std::uint32_t>(obj, kOwnerRow21TagOffset) == kOwnerCtor4A690Tag);
    CHECK(size_of(obj) == 0);

    // 0xf180: host-stand-in payload qword + 24 zero bytes, at +0x18 and +0x38.
    const std::uint64_t zeros[3] = {0, 0, 0};
    for (const std::size_t base : {kOwnerRow21Shared0Offset, kOwnerRow21Shared1Offset}) {
        CHECK(read_at<const void*>(obj, base) == slot_vtable_2dd9f78());
        CHECK(std::memcmp(obj + base + 0x08, zeros, 24) == 0);
    }

    // The anchor: self-linked, black, marked; its value/mapped region stays uninitialized.
    const OwnerRow21Node* const anchor = anchor_of(obj);
    CHECK(anchor == reinterpret_cast<const OwnerRow21Node*>(g.arena));
    CHECK(anchor->left == anchor);
    CHECK(anchor->parent == anchor);
    CHECK(anchor->right == anchor);
    CHECK(anchor->colour == kOwnerRow21Black);
    CHECK(anchor->is_anchor == 1);
    for (std::size_t i = kOwnerRow21KeyOffset; i < kOwnerRow21NodeSize; ++i) {
        CHECK(reinterpret_cast<const std::uint8_t*>(anchor)[i] == kNodeFill);
    }
    CHECK(shape(obj) == "A:B(A,A,A)");
    CHECK(rb_invariants(anchor));

    // Everything else keeps the fill, including the +0x6c..+0x70 hole; the mutex region holds
    // exactly what the hook wrote.
    for (std::size_t i = 0; i < kObjBytes; ++i) {
        if (i >= kOwnerRow21MutexOffset && i < kOwnerRow21AnchorOffset) {
            CHECK(obj[i] == static_cast<std::uint8_t>(0x40 + (i - kOwnerRow21MutexOffset)));
            continue;
        }
        if (object_offset_written(i, 3)) {
            continue;
        }
        CHECK(obj[i] == kFill);
    }
    check_guards(buffer, kFill);
    std::printf("PASS 2 empty queue: anchor self-linked/black, 0x90 object exact, hook order "
                "stamp/mutex/alloc\n");
}

// ---------------------------------------------------------------------------------------------
// 3. Twelve insertions that force recolours and both rotation directions.
// ---------------------------------------------------------------------------------------------
constexpr const char* kInsertKeys[] = {"m", "d", "p", "b", "f", "n", "r", "a", "c", "e", "g", "q"};
constexpr std::size_t kInsertCount = sizeof kInsertKeys / sizeof kInsertKeys[0];

// Derived from the twelve insertions and hand-checked against the CLRS fixup (the shape below
// is the textbook result for this order; `rb_invariants` re-verifies it generically).
const char* kInsertShape =
    "m:B(A,d,p) d:R(m,b,f) b:B(d,a,c) a:R(b,A,A) c:R(b,A,A) f:B(d,e,g) e:R(f,A,A) g:R(f,A,A) p:R(m,n,r) n:B(p,A,A) r:B(p,q,A) q:R(r,A,A) A:B(m,a,r)";
// Every compare hook call in order. Per entry: the caller's descent `(stored, probe)`, the
// caller's equivalent-key test `(probe, stored)` when a candidate exists, then `0x4b3f0`
// repeating the same two phases. Empty-tree entries produce no compares at all.
const char* kInsertCompares =
    "m:d|d:m|m:d|d:m|m:p|m:p|m:b|d:b|b:d|m:b|d:b|b:d|m:f|d:f|f:m|m:f|d:f|f:m|m:n|p:n|n:p|m:n|p:n|n:p|m:r|p:r|m:r|p:r|m:a|d:a|b:a|a:b|m:a|d:a|b:a|a:b|m:c|d:c|b:c|c:d|m:c|d:c|b:c|c:d|m:e|d:e|f:e|e:f|m:e|d:e|f:e|e:f|m:g|d:g|f:g|g:m|m:g|d:g|f:g|g:m|m:q|p:q|r:q|q:r|m:q|p:q|r:q|q:r";

void section_insert() {
    reset_ctx();
    Obj buffer;
    build_queue(std::vector<const char*>(std::begin(kInsertKeys), std::end(kInsertKeys)));
    const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
    CHECK(result.ok);
    CHECK(result.inserted == kInsertCount);
    CHECK(result.duplicates == 0);

    const std::uint8_t* const obj = obj_bytes(buffer);
    CHECK(size_of(obj) == kInsertCount);
    CHECK(g.alloc_calls == kInsertCount + 1);
    CHECK(g.head == nullptr);  // fully drained
    CHECK(g.head_history.size() == kInsertCount + 1);

    // In-order keys == the sorted input.
    std::vector<const OwnerRow21Node*> nodes = in_order(obj);
    CHECK(nodes.size() == kInsertCount);
    std::vector<std::string> keys;
    for (const char* key : kInsertKeys) {
        keys.emplace_back(key);
    }
    std::sort(keys.begin(), keys.end());
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        CHECK(std::string(key_of(nodes[i])) == keys[i]);
    }

    // Every root/link/colour value, pinned; plus the red-black invariants.
    CHECK(shape(obj) == kInsertShape);
    CHECK(rb_invariants(anchor_of(obj)));

    // The compare sequence the body produces: the caller's descent, the caller's equivalent-key
    // test (probe first), then `0x4b3f0` repeating both.
    CHECK(compare_log() == kInsertCompares);
    for (std::size_t i = 0; i < kInsertCount; ++i) {
        CHECK(g.controls[i] == 2);  // one `lock incq` acquire per inserted entry
    }

    // Each mapped slot points at its own queue entry, in insertion order; the 0x20 B key copy is
    // verbatim, including the 0x08..0x18 pattern bytes.
    for (std::size_t i = 0; i < kInsertCount; ++i) {
        const OwnerRow21QueueNode& entry = *queue_at(i);
        const OwnerRow21Node* node = nullptr;
        for (const OwnerRow21Node* candidate : nodes) {
            if (std::strcmp(key_of(candidate), entry_text(entry)) == 0) {
                node = candidate;
            }
        }
        CHECK(node != nullptr);
        CHECK(node->mapped == &entry);
        const auto* const value =
            reinterpret_cast<const std::uint8_t*>(&entry) + kOwnerRow21QueueValueOffset;
        CHECK(std::memcmp(node->key, value, kOwnerRow21KeyBytes) == 0);
    }

    std::printf("PASS 3 insertions: %zu nodes, in-order keys, pinned shape/links/colours, "
                "%zu compares, refcount 1->2\n", nodes.size(), g.compares.size());
}

// ---------------------------------------------------------------------------------------------
// 4. Duplicate keys: the first entry wins, the duplicate is dropped, the queue still drains.
// ---------------------------------------------------------------------------------------------
void section_duplicates() {
    reset_ctx();
    Obj buffer;
    build_queue({"k", "b", "k", "m"});
    const OwnerRow21QueueNode* const first = queue_at(0);
    const OwnerRow21QueueNode* const third = queue_at(2);
    const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
    CHECK(result.ok);
    CHECK(result.inserted == 3);
    CHECK(result.duplicates == 1);
    CHECK(size_of(obj_bytes(buffer)) == 3);
    CHECK(g.head == nullptr);
    CHECK(g.alloc_calls == 4);  // anchor + 3 nodes, none for the duplicate

    const std::vector<const OwnerRow21Node*> nodes = in_order(obj_bytes(buffer));
    CHECK(nodes.size() == 3);
    CHECK(std::string(key_of(nodes[0])) == "b");
    CHECK(std::string(key_of(nodes[1])) == "k");
    CHECK(std::string(key_of(nodes[2])) == "m");
    CHECK(nodes[1]->mapped == first);  // first wins
    CHECK(nodes[1]->mapped != third);
    CHECK(g.controls[0] == 2);
    CHECK(g.controls[1] == 2);
    CHECK(g.controls[2] == 1);  // the duplicate was never copied
    CHECK(g.controls[3] == 2);
    CHECK(rb_invariants(anchor_of(obj_bytes(buffer))));
    std::printf("PASS 4 duplicates: 3 nodes + 1 dropped, first entry keeps the mapped slot, "
                "queue drained\n");
}

// ---------------------------------------------------------------------------------------------
// 5. Failure surfaces: a named step, the exact prefix, and the queue head where it stopped.
// ---------------------------------------------------------------------------------------------
void section_failures() {
    // (a) missing stamp hook: only the head zeroes and the two 0xf180 slots exist.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"a"});
        OwnerRow21Hooks hooks = make_hooks();
        hooks.stamp = nullptr;
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), hooks);
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "stamp") == 0);
        CHECK(g.alloc_calls == 0);
        CHECK(g.head != nullptr);  // untouched
        expect_prefix_bytes(buffer, 0);
        CHECK(g.mutex_calls == 0);
    }
    // (b) missing mutex hook: stamp+tag written, nothing above +0x70.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"a"});
        OwnerRow21Hooks hooks = make_hooks();
        hooks.mutex_init = nullptr;
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), hooks);
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "mutex_init") == 0);
        expect_prefix_bytes(buffer, 1);
        CHECK(read_at<std::uint64_t>(obj_bytes(buffer), kOwnerRow21Stamp0Offset) == g.stamp_first);
        CHECK(g.alloc_calls == 0);
    }
    // (c) the anchor allocation fails: +0x80..+0x90 is already zeroed, no anchor, no queue read.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"a"});
        g.alloc_fail_at = 1;
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "anchor_alloc") == 0);
        CHECK(read_at<std::uint64_t>(obj_bytes(buffer), kOwnerRow21AnchorOffset) == 0);
        CHECK(read_at<std::uint64_t>(obj_bytes(buffer), kOwnerRow21SizeOffset) == 0);
        CHECK(g.queue_head_calls == 0);
        CHECK(g.compares.empty());
        expect_prefix_bytes(buffer, 3);
    }
    // (d) the queue hooks are missing: the anchor exists, nothing is drained.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"a"});
        OwnerRow21Hooks hooks = make_hooks();
        hooks.queue_head = nullptr;
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), hooks);
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "queue") == 0);
        CHECK(anchor_of(obj_bytes(buffer))->is_anchor == 1);
        CHECK(g.head != nullptr);
    }
    // (e) node allocation fails on the second entry: one node, head advanced once.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"m", "d", "p"});
        g.alloc_fail_at = 3;  // anchor, node 1, then fail
        const OwnerRow21QueueNode* const second = queue_at(1);
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "node_alloc") == 0);
        CHECK(result.inserted == 1);
        CHECK(size_of(obj_bytes(buffer)) == 1);
        CHECK(g.head == second);
        CHECK(g.controls[0] == 2);
        CHECK(g.controls[1] == 1);
        CHECK(shape(obj_bytes(buffer)) == "m:B(A,A,A) A:B(m,m,m)");
        CHECK(rb_invariants(anchor_of(obj_bytes(buffer))));
    }
    // (f) a null key on the second entry: one node, no second copy, head advanced once.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"m", "d"});
        auto* const second = queue_at(1);
        auto* const value = reinterpret_cast<std::uint8_t*>(second) + kOwnerRow21QueueValueOffset;
        const char* const null_text = nullptr;
        std::memcpy(value + kOwnerRow21KeyTextOffset, &null_text, sizeof null_text);
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "null-key") == 0);
        CHECK(result.inserted == 1);
        CHECK(g.head == second);
        CHECK(g.controls[1] == 1);
    }
    // (g) the post-insert head re-read returns null: one node, head not advanced.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"m", "d"});
        const OwnerRow21QueueNode* const first = queue_at(0);
        g.queue_head_fail_at = 2;  // entry 1 read, then the post-insert re-read of entry 1
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "queue") == 0);
        CHECK(result.inserted == 1);
        CHECK(g.head == first);  // 0x4a809 never ran for entry 1
        CHECK(g.controls[0] == 2);
        CHECK(size_of(obj_bytes(buffer)) == 1);
    }
    // (h) a missing compare hook with a non-empty tree.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"m", "d"});
        OwnerRow21Hooks hooks = make_hooks();
        hooks.compare = nullptr;
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), hooks);
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "compare") == 0);
        CHECK(result.inserted == 1);  // the first entry needed no comparison
        CHECK(g.head != nullptr);
    }
    // (i) missing allocate hook and missing queue accessors fail before any write of that stage.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"a"});
        OwnerRow21Hooks hooks = make_hooks();
        hooks.allocate = nullptr;
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), hooks);
        CHECK(!result.ok);
        CHECK(std::strcmp(result.failed_at, "anchor_alloc") == 0);
        expect_prefix_bytes(buffer, 3);
    }
    // (j) a misaligned object and a null object: nothing is written, no hook fires.
    {
        reset_ctx();
        Obj buffer;
        build_queue({"a"});
        const OwnerRow21Result null_result = owner_ctor_4a690(nullptr, make_hooks());
        CHECK(!null_result.ok);
        CHECK(std::strcmp(null_result.failed_at, "null-object") == 0);
        const OwnerRow21Result aligned = owner_ctor_4a690(buffer.storage + 1, make_hooks());
        CHECK(!aligned.ok);
        CHECK(std::strcmp(aligned.failed_at, "object-align") == 0);
        CHECK(g.stamp_calls == 0);
        CHECK(g.alloc_calls == 0);
        for (std::size_t i = 0; i < sizeof buffer.storage; ++i) {
            CHECK(buffer.storage[i] == kFill);
        }
    }
    std::printf("PASS 5 failures: 10 cases, named step, exact prefix, no fabricated node\n");
}

// ---------------------------------------------------------------------------------------------
// 6. Second call: a fresh anchor replaces +0x80, the previous anchor/tree is left untouched.
// ---------------------------------------------------------------------------------------------
void section_double_call() {
    reset_ctx();
    Obj buffer;
    build_queue({"m", "d"});
    CHECK(owner_ctor_4a690(obj_bytes(buffer), make_hooks()).ok);
    const OwnerRow21Node* const first_anchor = anchor_of(obj_bytes(buffer));
    std::vector<std::uint8_t> first_anchor_bytes(
        reinterpret_cast<const std::uint8_t*>(first_anchor),
        reinterpret_cast<const std::uint8_t*>(first_anchor) + kOwnerRow21NodeSize);
    CHECK(size_of(obj_bytes(buffer)) == 2);

    // Second call, empty queue: a second anchor is allocated and installed, the count resets.
    g.head = nullptr;
    g.compares.clear();
    const OwnerRow21Result second = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
    CHECK(second.ok);
    CHECK(second.inserted == 0);
    CHECK(g.compares.empty());  // empty queue, empty tree, no compares
    const OwnerRow21Node* const second_anchor = anchor_of(obj_bytes(buffer));
    CHECK(second_anchor != first_anchor);
    CHECK(second_anchor == reinterpret_cast<const OwnerRow21Node*>(
                               g.arena + 3 * kOwnerRow21NodeSize));
    CHECK(second_anchor->left == second_anchor);
    CHECK(second_anchor->parent == second_anchor);
    CHECK(second_anchor->right == second_anchor);
    CHECK(size_of(obj_bytes(buffer)) == 0);
    CHECK(shape(obj_bytes(buffer)) == "A:B(A,A,A)");
    // The orphaned anchor and its two nodes are byte-identical to before (leak documented).
    CHECK(std::memcmp(reinterpret_cast<const std::uint8_t*>(first_anchor),
                      first_anchor_bytes.data(), kOwnerRow21NodeSize) == 0);

    // A fresh object with a fresh queue: the new tree is built from the new entries only.
    reset_ctx();
    Obj buffer2;
    build_queue({"x", "y"});
    CHECK(owner_ctor_4a690(obj_bytes(buffer2), make_hooks()).ok);
    CHECK(size_of(obj_bytes(buffer2)) == 2);
    const std::vector<const OwnerRow21Node*> nodes = in_order(obj_bytes(buffer2));
    CHECK(nodes.size() == 2);
    CHECK(std::string(key_of(nodes[0])) == "x");
    CHECK(std::string(key_of(nodes[1])) == "y");
    CHECK(rb_invariants(anchor_of(obj_bytes(buffer2))));
    std::printf("PASS 6 second call: fresh anchor, count reset, previous anchor untouched\n");
}

// ---------------------------------------------------------------------------------------------
// 7. Guard ring and the hook-owned mutex region.
// ---------------------------------------------------------------------------------------------
void section_guard_ring() {
    reset_ctx();
    Obj buffer(kGuardFill);
    build_queue({"m", "d", "p"});
    g.mutex_writes = false;  // the hook writes nothing: the body must not write +0x70..+0x80
    CHECK(owner_ctor_4a690(obj_bytes(buffer), make_hooks()).ok);
    check_guards(buffer, kGuardFill);
    for (std::size_t i = kOwnerRow21MutexOffset; i < kOwnerRow21AnchorOffset; ++i) {
        CHECK(obj_bytes(buffer)[i] == kGuardFill);
    }
    CHECK(obj_bytes(buffer)[0] == 0);
    CHECK(obj_bytes(buffer)[kObjBytes - 1] == 0);  // +0x8f is inside the zeroed 16 B block
    CHECK(size_of(obj_bytes(buffer)) == 3);
    std::printf("PASS 7 guard ring: 0x40 bytes 0xa5 on both sides, +0x70..+0x80 untouched\n");
}

// ---------------------------------------------------------------------------------------------
// 8. Adapter for `owner_members`' row driver.
// ---------------------------------------------------------------------------------------------
void section_adapter() {
    // Before any hook installation the adapter refuses instead of calling a null hook.
    const bool before = owner_row21_member_ctor(kOwnerMemberRow21, nullptr, nullptr);
    CHECK(!before);
    CHECK(std::strcmp(owner_row21_last_failed_at(), "hooks") == 0);

    reset_ctx();
    Obj buffer;
    build_queue({"z"});
    owner_row21_set_hooks(make_hooks());
    CHECK(owner_row21_last_failed_at() == nullptr);
    CHECK(owner_row21_member_ctor(kOwnerMemberRow21, obj_bytes(buffer), nullptr));
    CHECK(owner_row21_last_failed_at() == nullptr);
    CHECK(size_of(obj_bytes(buffer)) == 1);
    CHECK(!owner_row21_member_ctor(22, obj_bytes(buffer), nullptr));
    CHECK(std::strcmp(owner_row21_last_failed_at(), "row") == 0);
    std::printf("PASS 8 adapter: row 21 drives the ctor, other rows and missing hooks refuse\n");
}

}  // namespace

int main() {
    section_constants();
    section_empty();
    if (std::getenv("SOTC_DUMP_4A690") != nullptr) {
        reset_ctx();
        Obj buffer;
        build_queue(std::vector<const char*>(std::begin(kInsertKeys), std::end(kInsertKeys)));
        const OwnerRow21Result result = owner_ctor_4a690(obj_bytes(buffer), make_hooks());
        std::printf("dump ok=%d inserted=%zu\nshape: %s\ncompares: %s\n", result.ok ? 1 : 0,
                    result.inserted, shape(obj_bytes(buffer)).c_str(), compare_log().c_str());
    }
    section_insert();
    section_duplicates();
    section_failures();
    section_double_call();
    section_guard_ring();
    section_adapter();
    std::printf("ALL 8 SECTIONS PASSED\n");
    return 0;
}
