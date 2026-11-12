// CPU-01-AOT-CFG-SCALE-01-BATCH21 (slot 446): fixture for the red-black named registry 0x3be2d0 / 0x3c0840 / 0x3c31b0 / 0x3c3710 /
// 0x3c3470 / 0x3c3930. The allocator thunks are test doubles that poison and log every block; ordering is strcasecmp (0xf8f500).
// The tree is checked against the red-black invariants and a differential std::map reference after every operation.
#include "l4_test_util.hpp"

#include <algorithm>
#include <cstring>
#include <map>
#include <random>
#include <string>
#include <strings.h>
#include <vector>

#include "core/rb_registry_3be2d0.hpp"

namespace {
using namespace l4;

struct Counters {
    std::size_t allocs = 0;
    std::size_t frees = 0;
    std::size_t fail_after = ~static_cast<std::size_t>(0);   // allocations allowed before the next one returns null
    std::size_t cmp_calls = 0;
} g;

void* test_allocate(std::size_t size, std::uint32_t tag, std::uint32_t align, std::uint32_t flags) {
    CHECK(tag == 1 && align == 8 && flags == 1);   // 0x17070 constants
    if (g.allocs >= g.fail_after) return nullptr;
    ++g.allocs;
    void* p = std::malloc(size);
    std::memset(p, kPoison, size);
    return p;
}
bool test_deallocate(void* p, std::uint32_t tag) {
    CHECK(tag == 0 || tag == 1);
    ++g.frees;
    std::free(p);
    return true;
}
int counting_cmp(const char* a, const char* b) {
    ++g.cmp_calls;
    return ::strcasecmp(a, b);
}
RbHooks hooks(bool counting = false) {
    RbHooks h{};
    h.alloc.allocate = &test_allocate;
    h.alloc.deallocate = &test_deallocate;
    if (counting) h.strcasecmp_import = &counting_cmp;
    return h;
}

// A caller-owned key: text, refcount 1 held by the caller (nodes add their own references).
struct Key {
    std::string text;
    std::int64_t ctl = 1;
    SharedStr view() { return SharedStr{reinterpret_cast<const std::uint8_t*>(text.c_str()), static_cast<std::int64_t>(text.size()), 0, &ctl}; }
};

struct Lt {
    bool operator()(const std::string& a, const std::string& b) const { return ::strcasecmp(a.c_str(), b.c_str()) < 0; }
};

void collect(const RbMap& m, std::vector<const RbNode*>& out) {
    const RbNode* n = m.head->left;
    while (n != m.head) {
        out.push_back(n);
        if (n->right->isnil == 0) {
            n = n->right;
            while (n->left->isnil == 0) n = n->left;
        } else {
            const RbNode* p = n->parent;
            while (p->isnil == 0 && n == p->right) { n = p; p = p->parent; }
            n = p;
        }
    }
}

void destroy_all(RbMap& m, const RbHooks& h) {
    std::vector<const RbNode*> nodes;
    collect(m, nodes);
    for (const RbNode* n : nodes) CHECK(rb_node_destroy_3c3930(const_cast<RbNode*>(n), h));
    CHECK(free_170b0(m.head, h.alloc).ok);
    m = RbMap{};
}

void section_constants() {
    CHECK(kRbRegisterEntryRva == 0x3be2d0 && kRbRegisterEntrySize == 192);
    CHECK(kRbIndexRva == 0x3c0840 && kRbIndexSize == 368);
    CHECK(kRbHintInsertRva == 0x3c31b0 && kRbHintInsertSize == 692);
    CHECK(kRbUniqueInsertRva == 0x3c3710 && kRbUniqueInsertSize == 544);
    CHECK(kRbInsertAtRva == 0x3c3470 && kRbInsertAtSize == 662);
    CHECK(kRbNodeDestroyRva == 0x3c3930 && kRbNodeDestroySize == 32);
    CHECK(kRbStrcasecmpPltRva == 0xf8f500);
    CHECK(kRbHeadNodeBytes == 0x30 && kRbDataNodeBytes == 0x48 && sizeof(RbNode) == 0x48);
    CHECK(kRbMaxSize == 0x038e38e38e38e38dull && kRbEntryKeyOffset == 0x60);
}

void section_empty_and_first_insert() {
    g = Counters{};
    const RbHooks h = hooks();
    RbMap m{};
    CHECK(rb_map_init_empty(m, h));
    CHECK(m.head->left == m.head && m.head->parent == m.head && m.head->right == m.head);
    CHECK(m.head->color == 1 && m.head->isnil == 1 && m.size == 0);
    CHECK(rb_check_invariants(m).ok);
    Key k{"Alpha"};
    const RbIndexResult r = rb_index_3c0840(m, k.view(), h);
    CHECK(r.ok && r.created && r.slot != nullptr && *r.slot == nullptr);
    CHECK(m.size == 1 && m.head->parent->color == 1 && m.head->left == m.head->parent && m.head->right == m.head->parent);
    CHECK(k.ctl == 2);                       // the node's copy took a reference (lock incq)
    CHECK(m.head->parent->key.data == k.view().data && m.head->parent->key.ctl == &k.ctl);
    CHECK(m.head->parent->mapped == nullptr && m.head->parent->color == 1 && m.head->parent->isnil == 0);
    // found: case-insensitive, no allocation, no new node
    Key k2{"ALPHA"};
    const std::size_t allocs = g.allocs;
    const RbIndexResult r2 = rb_index_3c0840(m, k2.view(), h);
    CHECK(r2.ok && !r2.created && r2.slot == r.slot && g.allocs == allocs && m.size == 1 && k2.ctl == 1);
    destroy_all(m, h);
    CHECK(k.ctl == 1);                       // node destroy released exactly the node's reference
    CHECK(g.allocs == g.frees);
}

void section_differential(const std::vector<std::string>& order, const char* label) {
    g = Counters{};
    const RbHooks h = hooks();
    RbMap m{};
    CHECK(rb_map_init_empty(m, h));
    std::vector<Key> keys(order.size());
    for (std::size_t i = 0; i < order.size(); ++i) keys[i].text = order[i];
    std::map<std::string, int, Lt> ref;
    for (std::size_t i = 0; i < order.size(); ++i) {
        const bool fresh = ref.find(order[i]) == ref.end();
        const RbIndexResult r = rb_index_3c0840(m, keys[i].view(), h);
        if (!r.ok) { std::fprintf(stderr, "index failed (%s) at %zu\n", label, i); std::exit(1); }
        CHECK(r.created == fresh);
        ref.emplace(order[i], 1);
        const RbCheck c = rb_check_invariants(m);
        if (!c.ok) { std::fprintf(stderr, "invariants broken (%s) after %zu inserts\n", label, i + 1); std::exit(1); }
        CHECK(c.count == ref.size() && m.size == ref.size());
    }
    std::vector<const RbNode*> nodes;
    collect(m, nodes);
    CHECK(nodes.size() == ref.size());
    std::size_t i = 0;
    for (const auto& kv : ref) {
        CHECK(::strcasecmp(reinterpret_cast<const char*>(nodes[i]->key.data), kv.first.c_str()) == 0);
        ++i;
    }
    // black height of a tree of n nodes is at most log2(n + 1) + 1
    std::size_t bound = 1;
    while ((std::size_t{1} << bound) < ref.size() + 1) ++bound;
    CHECK(static_cast<std::size_t>(rb_check_invariants(m).black_height) <= bound + 1);
    destroy_all(m, h);
    CHECK(g.allocs == g.frees);
    for (const Key& k : keys) CHECK(k.ctl == 1);   // every node reference was returned
}

void section_orders() {
    std::vector<std::string> asc, desc, rnd, mixed;
    for (int i = 0; i < 300; ++i) {
        char b[24];
        std::snprintf(b, sizeof b, "node%04d", i);
        asc.push_back(b);
    }
    desc.assign(asc.rbegin(), asc.rend());
    rnd = asc;
    std::mt19937 gen(0x5071c);
    std::shuffle(rnd.begin(), rnd.end(), gen);
    for (std::size_t i = 0; i < rnd.size(); ++i) {
        std::string s = rnd[i];
        if (i % 3 == 0) std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        mixed.push_back(s);
        if (i % 5 == 0) mixed.push_back(rnd[i]);   // case-folded duplicate of an existing key
    }
    section_differential(asc, "ascending");
    section_differential(desc, "descending");
    section_differential(rnd, "random");
    section_differential(mixed, "mixed-case-duplicates");
    // zig-zag: outermost-in, which exercises both inner-rotation (zig-zag) cases
    std::vector<std::string> zig;
    for (int i = 0; i < 100; ++i) {
        char a[24], b[24];
        std::snprintf(a, sizeof a, "z%04d", i);
        std::snprintf(b, sizeof b, "z%04d", 199 - i);
        zig.push_back(a);
        zig.push_back(b);
    }
    section_differential(zig, "zigzag");
    for (int seed = 0; seed < 25; ++seed) {
        std::vector<std::string> v;
        for (int i = 0; i < 40 + seed; ++i) v.push_back("k" + std::to_string((i * 7919 + seed * 104729) % 97));
        std::mt19937 g2(seed);
        std::shuffle(v.begin(), v.end(), g2);
        section_differential(v, "seeded");
    }
}

// Every hint-insert branch (rb_hint_insert_3c31b0) and both unique-insert modes, driven with every possible hint node.
void section_hints() {
    const RbHooks h = hooks();
    const int n = 16;
    for (int hint_slot = -1; hint_slot <= n; ++hint_slot) {      // -1 = head, 0..n-1 = a real node
        for (int probe = 0; probe < 2 * n + 1; ++probe) {         // odd probes fall between/around the existing even keys
            g = Counters{};
            RbMap m{};
            CHECK(rb_map_init_empty(m, h));
            std::vector<Key> keys;
            keys.reserve(2 * n + 8);
            for (int i = 0; i < n; ++i) {
                char b[16];
                std::snprintf(b, sizeof b, "k%03d", i * 2);
                keys.push_back(Key{b});
                CHECK(rb_index_3c0840(m, keys.back().view(), h).ok);
            }
            std::vector<const RbNode*> nodes;
            collect(m, nodes);
            RbNode* hint = hint_slot < 0 || hint_slot >= n ? m.head : const_cast<RbNode*>(nodes[static_cast<std::size_t>(hint_slot)]);
            char b[16];
            std::snprintf(b, sizeof b, "K%03d", probe);             // upper case on purpose: strcasecmp ordering
            keys.push_back(Key{b});
            Key& dk = keys.back();
            const bool exists = (probe % 2) == 0 && probe / 2 < n;
            const std::size_t before = m.size;
            auto make_node = [&]() {
                const AllocWrapperResult a = alloc_17070(kRbDataNodeBytes, h.alloc);
                CHECK(a.ok);
                RbNode* nn = static_cast<RbNode*>(a.pointer);
                std::memset(static_cast<void*>(nn), 0, sizeof *nn);
                nn->left = nn->parent = nn->right = m.head;
                nn->key = dk.view();
                ++dk.ctl;
                return nn;
            };
            RbNode* nn = make_node();
            const std::size_t frees0 = g.frees;
            const RbInsertResult r = rb_hint_insert_3c31b0(m, dk.view(), nn, hint, h);
            CHECK(r.ok && r.node != nullptr);
            if (exists) {
                CHECK(!r.inserted && r.node != nn && m.size == before && g.frees == frees0 + 1 && dk.ctl == 1);
            } else {
                CHECK(r.node == nn && m.size == before + 1 && g.frees == frees0 && dk.ctl == 2);
            }
            CHECK(rb_check_invariants(m).ok);
            // the same key again with the same hint: always the unique-insert duplicate path
            const std::int64_t ctl1 = dk.ctl;
            RbNode* n2 = make_node();
            const std::size_t frees1 = g.frees;
            const std::size_t size1 = m.size;
            const RbInsertResult r2 = rb_hint_insert_3c31b0(m, dk.view(), n2, hint, h);
            CHECK(r2.ok && r2.node == r.node && !r2.inserted && m.size == size1);
            CHECK(g.frees == frees1 + 1 && dk.ctl == ctl1);   // 0x170b0 ran once; 0xf590 returned the duplicate's reference
            CHECK(rb_check_invariants(m).ok);
            destroy_all(m, h);
            CHECK(g.allocs == g.frees);
        }
    }
}

void section_unique_modes() {
    const RbHooks h = hooks();
    for (std::uint8_t mode = 0; mode <= 1; ++mode) {
        g = Counters{};
        RbMap m{};
        CHECK(rb_map_init_empty(m, h));
        std::vector<Key> keys;
        keys.reserve(64);
        std::mt19937 gen(7 + mode);
        std::vector<int> order(30);
        for (int i = 0; i < 30; ++i) order[static_cast<std::size_t>(i)] = i * 3;
        std::shuffle(order.begin(), order.end(), gen);
        for (int v : order) {
            char b[16];
            std::snprintf(b, sizeof b, "u%03d", v);
            keys.push_back(Key{b});
            Key& k = keys.back();
            const AllocWrapperResult a = alloc_17070(kRbDataNodeBytes, h.alloc);
            CHECK(a.ok);
            RbNode* nn = static_cast<RbNode*>(a.pointer);
            std::memset(static_cast<void*>(nn), 0, sizeof *nn);
            nn->left = nn->parent = nn->right = m.head;
            nn->key = k.view();
            ++k.ctl;
            const RbInsertResult r = rb_unique_insert_3c3710(m, mode, k.view(), nn, h);
            CHECK(r.ok && r.inserted && r.node == nn);
            CHECK(rb_check_invariants(m).ok);
        }
        // duplicates of every key, in both modes
        for (std::size_t i = 0; i < keys.size(); ++i) {
            Key& k = keys[i];
            const AllocWrapperResult a = alloc_17070(kRbDataNodeBytes, h.alloc);
            RbNode* nn = static_cast<RbNode*>(a.pointer);
            std::memset(static_cast<void*>(nn), 0, sizeof *nn);
            nn->left = nn->parent = nn->right = m.head;
            nn->key = k.view();
            ++k.ctl;
            const std::size_t size = m.size;
            const RbInsertResult r = rb_unique_insert_3c3710(m, mode, k.view(), nn, h);
            if (mode == 0) {
                CHECK(r.ok && !r.inserted && r.node != nn && m.size == size);
                CHECK(k.ctl == 2);
            } else {
                // dl != 0 descends left on equality (lower-bound position), so the predecessor is strictly smaller: an equal key is linked
                CHECK(r.ok && r.inserted && r.node == nn && m.size == size + 1);
                CHECK(k.ctl == 3);
                CHECK(rb_check_invariants(m).ok);
                CHECK(nn == r.node && nn->left != nullptr);
            }
        }
        destroy_all(m, h);
        CHECK(g.allocs == g.frees);
    }
}

void section_register() {
    g = Counters{};
    const RbHooks h = hooks(true);
    RbMap m{};
    CHECK(rb_map_init_empty(m, h));
    struct Entry {
        std::uint8_t before[0x60];
        SharedStr key;
        std::uint8_t after[0x30];
    };
    static_assert(offsetof(Entry, key) == kRbEntryKeyOffset);
    Key ka{"Alpha"}, kb{"beta"}, ka2{"ALPHA"};
    Entry a{}, b{}, a2{};
    a.key = ka.view();
    b.key = kb.view();
    a2.key = ka2.view();
    RbRegisterResult r = rb_register_entry_3be2d0(m, &a, h);
    CHECK(r.ok && r.stored && r.created_node && !r.already_registered && m.size == 1);
    CHECK(g.cmp_calls == 0);   // an empty tree compares nothing
    r = rb_register_entry_3be2d0(m, &b, h);
    CHECK(r.ok && r.stored && r.created_node && m.size == 2 && g.cmp_calls > 0);
    // the same name (case-insensitive) with a different object: found with mapped != 0, nothing changes
    const std::size_t allocs = g.allocs;
    r = rb_register_entry_3be2d0(m, &a2, h);
    CHECK(r.ok && r.already_registered && !r.stored && m.size == 2 && g.allocs == allocs);
    const RbIndexResult look = rb_index_3c0840(m, ka.view(), h);
    CHECK(look.ok && !look.created && *look.slot == &a);
    // a node whose mapped slot is still 0 is reused, not recreated
    *look.slot = nullptr;
    r = rb_register_entry_3be2d0(m, &a2, h);
    CHECK(r.ok && r.stored && !r.created_node && !r.already_registered && m.size == 2);
    CHECK(*rb_index_3c0840(m, ka.view(), h).slot == &a2);
    CHECK(rb_check_invariants(m).ok);
    destroy_all(m, h);
    CHECK(g.allocs == g.frees);
}

void section_failures() {
    // allocation failure at the node: index reports !ok and leaves the tree untouched
    {
        g = Counters{};
        const RbHooks h = hooks();
        RbMap m{};
        CHECK(rb_map_init_empty(m, h));          // allocation 1
        g.fail_after = 1;
        Key k{"x"};
        const RbIndexResult r = rb_index_3c0840(m, k.view(), h);
        CHECK(!r.ok && m.size == 0 && k.ctl == 1 && rb_check_invariants(m).ok);
        g.fail_after = ~static_cast<std::size_t>(0);
        destroy_all(m, h);
        CHECK(g.allocs == g.frees);
    }
    // head allocation failure
    {
        g = Counters{};
        g.fail_after = 0;
        const RbHooks h = hooks();
        RbMap m{};
        CHECK(!rb_map_init_empty(m, h));
    }
    // the size bound: the node is destroyed first (0x3c3930), then the original throws; the model fails
    {
        g = Counters{};
        const RbHooks h = hooks();
        RbMap m{};
        CHECK(rb_map_init_empty(m, h));
        Key k{"bound"};
        const AllocWrapperResult a = alloc_17070(kRbDataNodeBytes, h.alloc);
        RbNode* nn = static_cast<RbNode*>(a.pointer);
        std::memset(static_cast<void*>(nn), 0, sizeof *nn);
        nn->key = k.view();
        ++k.ctl;
        m.size = kRbMaxSize - 1;
        // at size == max - 1 the insert still succeeds ...
        const RbInsertResult ok = rb_insert_at_3c3470(m, true, m.head, nn, h);
        CHECK(ok.ok && m.size == kRbMaxSize);
        // ... and at size == max it fails after destroying the node
        const AllocWrapperResult a2 = alloc_17070(kRbDataNodeBytes, h.alloc);
        RbNode* n2 = static_cast<RbNode*>(a2.pointer);
        std::memset(static_cast<void*>(n2), 0, sizeof *n2);
        n2->key = k.view();
        ++k.ctl;
        const std::size_t frees = g.frees;
        const RbInsertResult bad = rb_insert_at_3c3470(m, true, nn, n2, h);
        CHECK(!bad.ok && g.frees == frees + 1 && m.size == kRbMaxSize && k.ctl == 2);
        CHECK(free_170b0(nn, h.alloc).ok);
        CHECK(free_170b0(m.head, h.alloc).ok);
    }
    // the last reference: node destroy frees the key data (tag 0), the control block (tag 1) and the node (tag 1)
    {
        g = Counters{};
        const RbHooks h = hooks();
        const AllocWrapperResult a = alloc_17070(kRbDataNodeBytes, h.alloc);
        RbNode* nn = static_cast<RbNode*>(a.pointer);
        std::memset(static_cast<void*>(nn), 0, sizeof *nn);
        char* text = static_cast<char*>(std::malloc(8));
        std::strcpy(text, "own");
        auto* ctl = static_cast<std::int64_t*>(std::malloc(8));
        *ctl = 1;
        nn->key = SharedStr{reinterpret_cast<const std::uint8_t*>(text), 3, 0, ctl};
        const std::size_t frees = g.frees;
        CHECK(rb_node_destroy_3c3930(nn, h));
        CHECK(g.frees == frees + 3);
        g.allocs = g.frees;   // the data/control blocks were malloc'd by this test, not by 0x17070
    }
}

}  // namespace

int main() {
    section_constants();
    section_empty_and_first_insert();
    section_orders();
    section_hints();
    section_unique_modes();
    section_register();
    section_failures();
    std::puts("rb_registry_3be2d0 ok");
    return 0;
}
