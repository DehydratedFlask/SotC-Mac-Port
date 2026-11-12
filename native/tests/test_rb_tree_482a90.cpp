// CPU-01-AOT-CFG-SCALE-01-BATCH26: fixture for rb_tree_482a90. Test-only scaffolding: the RB
// insert below is NOT lifted from the image (no 0x482a90-adjacent insert exists in this TU); it
// builds valid trees so the lifted erase can be exercised against the ordering, colour and
// black-height invariants, and against the header leftmost/root/rightmost references.
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <vector>

#include "core/rb_tree_482a90.hpp"

#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #c); std::exit(1); } } while (0)

using namespace sotc;

namespace {

std::size_t g_allocations = 0;
std::size_t g_releases = 0;
std::vector<std::size_t> g_sizes;

void* recording_alloc(std::size_t bytes) noexcept {
    ++g_allocations;
    g_sizes.push_back(bytes);
    return std::malloc(bytes);
}

void recording_release(void* p) noexcept {
    ++g_releases;
    std::free(p);
}

RbEraseHooks hooks() { return RbEraseHooks{recording_alloc, recording_release}; }

struct Node : RbEraseNode {
    int key;
};

Node* make_node(int key) {
    Node* n = static_cast<Node*>(std::malloc(sizeof(Node)));
    std::memset(n, 0, sizeof(Node));
    n->key = key;
    return n;
}

bool less(const RbEraseNode* a, const RbEraseNode* b) {
    return static_cast<const Node*>(a)->key < static_cast<const Node*>(b)->key;
}

void rotate_left(RbEraseTree& t, RbEraseNode* x) noexcept {
    RbEraseNode* const nil = t.header;
    RbEraseNode* const y = x->right;
    x->right = y->left;
    if (y->left != nil) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nil) nil->parent = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void rotate_right(RbEraseTree& t, RbEraseNode* x) noexcept {
    RbEraseNode* const nil = t.header;
    RbEraseNode* const y = x->left;
    x->left = y->right;
    if (y->right != nil) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == nil) nil->parent = y;
    else if (x == x->parent->right) x->parent->right = y;
    else x->parent->left = y;
    y->right = x;
    x->parent = y;
}

void insert_fixup(RbEraseTree& t, RbEraseNode* z) noexcept {
    RbEraseNode* const nil = t.header;
    while (z->parent->color == kRbRed) {
        RbEraseNode* const p = z->parent;
        RbEraseNode* const g = p->parent;
        if (p == g->left) {
            RbEraseNode* const u = g->right;
            if (u->color == kRbRed) {
                p->color = kRbBlack;
                u->color = kRbBlack;
                g->color = kRbRed;
                z = g;
            } else {
                if (z == p->right) { z = p; rotate_left(t, z); }
                z->parent->color = kRbBlack;
                z->parent->parent->color = kRbRed;
                rotate_right(t, z->parent->parent);
            }
        } else {
            RbEraseNode* const u = g->left;
            if (u->color == kRbRed) {
                p->color = kRbBlack;
                u->color = kRbBlack;
                g->color = kRbRed;
                z = g;
            } else {
                if (z == p->left) { z = p; rotate_right(t, z); }
                z->parent->color = kRbBlack;
                z->parent->parent->color = kRbRed;
                rotate_left(t, z->parent->parent);
            }
        }
        (void)nil;
    }
    t.header->parent->color = kRbBlack;
}

void insert(RbEraseTree& t, int key) {
    Node* const fresh = make_node(key);
    RbEraseNode* const z = fresh;
    RbEraseNode* y = t.header;
    RbEraseNode* x = t.header->parent;
    while (x != t.header) {
        y = x;
        x = less(z, x) ? x->left : x->right;
    }
    z->parent = y;
    if (y == t.header) t.header->parent = z;
    else if (less(z, y)) y->left = z;
    else y->right = z;
    z->left = z->right = t.header;
    z->color = kRbRed;
    z->nil = 0;
    ++t.size;
    insert_fixup(t, z);
    t.header->left = t.header->parent;
    while (t.header->left->left != t.header) t.header->left = t.header->left->left;
    t.header->right = t.header->parent;
    while (t.header->right->right != t.header) t.header->right = t.header->right->right;
    t.header->parent->parent = t.header;
}

int verify_node(const RbEraseTree& t, RbEraseNode* n, long lo, long hi, std::vector<int>& order) {
    RbEraseNode* const nil = t.header;
    if (n == nil) return 1;
    const long key = static_cast<Node*>(n)->key;
    CHECK(key > lo && key < hi);
    if (n->color == kRbRed) {
        CHECK(n->left->color == kRbBlack && n->right->color == kRbBlack);
    }
    const int left = verify_node(t, n->left, lo, key, order);
    order.push_back(static_cast<int>(key));
    const int right = verify_node(t, n->right, key, hi, order);
    CHECK(left == right);
    return left + (n->color == kRbBlack ? 1 : 0);
}

void verify(const RbEraseTree& t, std::size_t expected_size) {
    CHECK(t.size == expected_size);
    CHECK(t.header->color == kRbBlack && t.header->nil == 1);
    RbEraseNode* const root = t.header->parent;
    if (root == t.header) {
        CHECK(t.size == 0);
        CHECK(t.header->left == t.header && t.header->parent == t.header);
        return;
    }
    CHECK(root->parent == t.header);
    CHECK(root->color == kRbBlack);
    std::vector<int> order;
    verify_node(t, root, -1L << 40, 1L << 40, order);
    CHECK(order.size() == expected_size);
    CHECK(std::is_sorted(order.begin(), order.end()));
    RbEraseNode* min = root;
    while (min->left != t.header) min = min->left;
    CHECK(t.header->left == min);
    RbEraseNode* max = root;
    while (max->right != t.header) max = max->right;
    CHECK(t.header->right == max);
}

void drain(std::vector<int>& keys, std::mt19937& rng, bool shuffled) {
    if (shuffled) std::shuffle(keys.begin(), keys.end(), rng);
}

void exercise(int count, int seed, bool shuffled) {
    g_allocations = g_releases = 0;
    g_sizes.clear();
    RbEraseTree t{};
    CHECK(rb_tree_init(t, 0x38, hooks()));
    CHECK(g_allocations == 1 && g_sizes.back() == 0x38);
    std::vector<int> keys;
    for (int i = 0; i < count; ++i) keys.push_back(i * 2 + 1);
    std::mt19937 rng(static_cast<unsigned>(seed));
    if (shuffled) std::shuffle(keys.begin(), keys.end(), rng);
    for (int key : keys) insert(t, key);
    verify(t, static_cast<std::size_t>(count));

    std::vector<int> order = keys;
    drain(order, rng, shuffled);
    for (int key : order) {
        Node* victim = nullptr;
        // find by BST search
        RbEraseNode* n = t.header->parent;
        while (n != t.header) {
            const int k = static_cast<Node*>(n)->key;
            if (k == key) { victim = static_cast<Node*>(n); break; }
            n = key < k ? n->left : n->right;
        }
        CHECK(victim != nullptr);
        RbEraseNode* const next = rb_erase_482a90(t, victim, hooks());
        CHECK(next == t.header || static_cast<Node*>(next)->key > key);
        --count;
        verify(t, static_cast<std::size_t>(count));
    }
    CHECK(t.size == 0);
    rb_tree_release(t, hooks());
    CHECK(t.header == nullptr);
}

void exercise_loop(int count, int seed) {
    RbEraseTree t{};
    CHECK(rb_tree_init(t, 0x38, hooks()));
    std::mt19937 rng(static_cast<unsigned>(seed));
    std::vector<int> keys;
    for (int i = 0; i < count; ++i) keys.push_back(i);
    std::shuffle(keys.begin(), keys.end(), rng);
    for (int k : keys) insert(t, k);
    CHECK(rb_erase_all_loop(t, hooks()) == static_cast<std::size_t>(count));
    CHECK(t.size == 0 && t.header->parent == t.header && t.header->left == t.header &&
          t.header->right == t.header);
    rb_tree_release(t, hooks());
}

}  // namespace

int main() {
    // Nil rejection: the original asserts at 0x482f33; the native model refuses without mutation.
    {
        RbEraseTree t{};
        CHECK(rb_tree_init(t, 0x38, hooks()));
        CHECK(rb_erase_482a90(t, t.header, hooks()) == nullptr);
        CHECK(t.size == 0);
        rb_tree_release(t, hooks());
    }
    for (int n : {0, 1, 2, 3, 4, 5, 8, 17, 64}) exercise(n, 100 + n, false);
    for (int n : {2, 3, 7, 16, 33, 100}) exercise(n, 200 + n, true);
    exercise_loop(64, 7);
    exercise_loop(255, 11);

    // Successor walk on a fixed tree.
    {
        RbEraseTree t{};
        CHECK(rb_tree_init(t, 0x38, hooks()));
        for (int k : {4, 2, 6, 1, 3, 5, 7}) insert(t, k);
        std::vector<int> seen;
        for (RbEraseNode* n = t.header->left; n != t.header; n = rb_next(t, n)) {
            seen.push_back(static_cast<Node*>(n)->key);
        }
        CHECK((seen == std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
        CHECK(rb_next(t, t.header->right) == t.header);
        rb_tree_release(t, hooks());
    }
    std::printf("rb_tree_482a90: erase/clear invariants hold (allocation sizes 0x38; loop erase 64/255)\n");
    return 0;
}
