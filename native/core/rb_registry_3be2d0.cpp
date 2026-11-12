#include "rb_registry_3be2d0.hpp"

#include <atomic>
#include <cstring>
#include <strings.h>

#include "shared_str_release_f590.hpp"

namespace sotc {

namespace {

// 0xf590 on a SharedStr whose data is released with tag 0 and whose control block with tag 1 (0x170b0), through the allocator hooks.
class HookDeallocator final : public SharedStrDeallocatorF590 {
public:
    explicit HookDeallocator(const AllocDispatchContract& c) noexcept : c_(c) {}
    void deallocate(const void* allocation, SharedStrDeallocationKindF590 kind) noexcept override {
        if (c_.deallocate != nullptr) (void)c_.deallocate(const_cast<void*>(allocation), kind == SharedStrDeallocationKindF590::data ? 0u : 1u);
    }

private:
    AllocDispatchContract c_;
};

int cmp(const RbHooks& h, const std::uint8_t* a, const std::uint8_t* b) noexcept {
    const char* const x = reinterpret_cast<const char*>(a);
    const char* const y = reinterpret_cast<const char*>(b);
    return h.strcasecmp_import != nullptr ? h.strcasecmp_import(x, y) : ::strcasecmp(x, y);
}

bool is_nil(const RbNode* n) noexcept { return n->isnil != 0; }

}  // namespace

bool rb_map_init_empty(RbMap& map, const RbHooks& hooks) noexcept {
    const AllocWrapperResult a = alloc_17070(kRbHeadNodeBytes, hooks.alloc);
    if (!a.ok || a.pointer == nullptr) return false;
    RbNode* const head = static_cast<RbNode*>(a.pointer);
    head->left = head;
    head->parent = head;
    head->right = head;
    head->color = 1;
    head->isnil = 1;
    map.head = head;
    map.size = 0;
    return true;
}

// 0x3c3930
bool rb_node_destroy_3c3930(RbNode* node, const RbHooks& hooks) noexcept {
    HookDeallocator d(hooks.alloc);
    release_shared_str_f590(node->key, d);
    return free_170b0(node, hooks.alloc).ok;
}

// 0x3c3470 -- a literal lift: labels carry the original addresses; registers become locals.
RbInsertResult rb_insert_at_3c3470(RbMap& map, bool add_left, RbNode* where, RbNode* node, const RbHooks& hooks) noexcept {
    RbNode* A = nullptr;   // rax
    RbNode* P = nullptr;   // r11
    RbNode* G = nullptr;   // r10
    RbNode* C = nullptr;   // rcx
    RbNode* D = nullptr;   // rdx
    RbInsertResult out{};
    if (map.size >= kRbMaxSize) {
        (void)rb_node_destroy_3c3930(node, hooks);   // 0x3c36e9: 0x3c3930, then the throw through 0xf8fc10 (never returns)
        return out;
    }
    map.size += 1;
    node->parent = where;
    if (map.head == where) goto L34b1;
    if (!add_left) goto L34c2;
    where->left = node;
    A = map.head;
    if (A->left != where) goto L34d4;
    A->left = node;
    goto L34d4;
L34b1:
    where->parent = node;
    A = map.head;
    A->left = node;
    A = map.head;
    goto L34d0;
L34c2:
    where->right = node;
    A = map.head;
    if (A->right != where) goto L34d4;
L34d0:
    A->right = node;
L34d4:
    P = node->parent;
    if (P->color != 0) goto L36d5;
    C = node;
L34f0:
    G = P->parent;
    A = G->left;
    if (P == A) goto L3520;
    if (A->color == 0) goto L3540;
    if (C == P->left) goto L355e;
    A = P;
    P = C;
    goto L35e6;
L3520:
    A = G->right;
    if (A->color == 0) goto L3540;
    if (C == P->right) goto L3594;
    A = P;
    P = C;
    goto L3658;
L3540:
    P->color = 1;
    A->color = 1;
    A = C->parent;
    A = A->parent;
    A->color = 0;
    A = C->parent;
    P = A->parent;
    goto L36bb;
L355e:   // parent is a right child, node a left child, uncle black: rotate the parent right, then the grandparent left
    A = C->right;
    P->left = A;
    A = C->right;
    if (!is_nil(A)) {
        A->parent = P;
        G = P->parent;
    }
    C->parent = G;
    A = map.head;
    if (A->parent == P) goto L35d1;
    A = P->parent;
    if (A->right == P) goto L35d7;
    A->left = C;
    goto L35db;
L35d1:
    A->parent = C;
    goto L35db;
L35d7:
    A->right = C;
L35db:
    A = C;
    C->right = P;
    P->parent = C;
L35e6:
    A->color = 1;
    A = P->parent;
    A = A->parent;
    A->color = 0;
    A = P->parent;
    C = A->parent;
    D = C->right;
    A = D->left;
    C->right = A;
    A = D->left;
    if (!is_nil(A)) A->parent = C;
    D->parent = C->parent;
    A = map.head;
    if (A->parent == C) goto L3637;
    A = C->parent;
    if (A->left == C) goto L363d;
    A->right = D;
    goto L3640;
L3637:
    A->parent = D;
    goto L3640;
L363d:
    A->left = D;
L3640:
    D->left = C;
    goto L36b7;
L3594:   // parent is a left child, node a right child, uncle black: rotate the parent left, then the grandparent right
    A = C->left;
    P->right = A;
    A = C->left;
    if (!is_nil(A)) {
        A->parent = P;
        G = P->parent;
    }
    C->parent = G;
    A = map.head;
    if (A->parent == P) goto L3645;
    A = P->parent;
    if (A->left == P) goto L364b;
    A->right = C;
    goto L364e;
L3645:
    A->parent = C;
    goto L364e;
L364b:
    A->left = C;
L364e:
    A = C;
    C->left = P;
    P->parent = C;
L3658:
    A->color = 1;
    A = P->parent;
    A = A->parent;
    A->color = 0;
    A = P->parent;
    C = A->parent;
    D = C->left;
    A = D->right;
    C->left = A;
    A = D->right;
    if (!is_nil(A)) A->parent = C;
    D->parent = C->parent;
    A = map.head;
    if (A->parent == C) goto L36a9;
    A = C->parent;
    if (A->right == C) goto L36af;
    A->left = D;
    goto L36b3;
L36a9:
    A->parent = D;
    goto L36b3;
L36af:
    A->right = D;
L36b3:
    D->right = C;
L36b7:
    C->parent = D;
L36bb:
    C = P;
    P = C->parent;
    if (P->color == 0) goto L34f0;
L36d5:
    A = map.head;
    A = A->parent;
    A->color = 1;
    out.ok = true;
    out.node = node;
    out.inserted = true;
    return out;
}

// 0x3c3710 -- literal lift. `key` is the SharedStr passed by pointer in rcx (only its data pointer is read).
RbInsertResult rb_unique_insert_3c3710(RbMap& map, std::uint8_t mode, const SharedStr& key, RbNode* node, const RbHooks& hooks) noexcept {
    RbNode* head = nullptr;    // r12 (until 0x3c37d4 / 0x3c37fb overwrite its low byte use)
    RbNode* rbx = nullptr;
    RbNode* r14 = nullptr;
    RbNode* rcx = nullptr;
    RbNode* rax = nullptr;
    RbNode* rdx = nullptr;
    const std::uint8_t* kp = nullptr;
    bool al = false;
    std::uint8_t r12b = 0;
    RbInsertResult tmp{};
    RbInsertResult out{};

    head = map.head;
    rcx = head->parent;
    if (is_nil(rcx)) {
        r14 = head;
        rbx = head;
        goto L37e6;
    }
    kp = key.data;
    if (mode == 0) goto L37a0;
    do {   // 0x3c3770: lower-bound descent (go right while node.key < key)
        rbx = rcx;
        al = cmp(hooks, rbx->key.data, kp) < 0;
        rcx = al ? rbx->right : rbx->left;
    } while (!is_nil(rcx));
    al = !al;
    goto L37c5;
L37a0:
    do {   // upper-bound descent (go left while key < node.key)
        rbx = rcx;
        al = cmp(hooks, kp, rbx->key.data) < 0;
        rcx = al ? rbx->left : rbx->right;
    } while (!is_nil(rcx));
L37c5:
    if (al) goto L37df;
    r12b = 0;
    r14 = rbx;
    goto L3894;
L37df:
    r14 = rbx;
L37e6:
    if (rbx == head->left) goto L3809;
    if (!is_nil(rbx)) goto L382f;
    r14 = rbx->right;
    r12b = 1;
    goto L3894;
L3809:
    tmp = rb_insert_at_3c3470(map, true, rbx, node, hooks);
    if (!tmp.ok) return out;
    out.ok = true;
    out.node = tmp.node;
    out.inserted = true;
    return out;
L382f:   // r14 = predecessor of rbx
    rax = rbx->left;
    if (!is_nil(rax)) goto L3870;
    rax = rbx->parent;
    if (is_nil(rax)) goto L388e;
    rcx = rbx;
L3850:
    rdx = rax;
    if (rcx != rdx->left) goto L3882;
    rax = rdx->parent;
    r14 = rdx;
    rcx = rdx;
    if (!is_nil(rax)) goto L3850;
    goto L3885;
L3870:
    r14 = rax;
    rax = r14->right;
    if (!is_nil(rax)) goto L3870;
    r12b = 1;
    goto L3894;
L3882:
    rax = rdx;
L3885:
    r12b = 1;
    if (is_nil(rcx)) goto L3894;
L388e:
    r12b = 1;
    r14 = rax;
L3894:
    if (is_nil(r14)) return out;   // the original would read the key of the head node (uninitialised); unreachable for a valid tree
    if (cmp(hooks, r14->key.data, key.data) < 0) goto L38d5;
    // duplicate: 0xf590 on node+0x20, 0x170b0 on the node
    if (!rb_node_destroy_3c3930(node, hooks)) return out;
    out.ok = true;
    out.node = r14;
    out.inserted = false;
    return out;
L38d5:
    tmp = rb_insert_at_3c3470(map, r12b != 0, rbx, node, hooks);
    if (!tmp.ok) return out;
    out.ok = true;
    out.node = tmp.node;
    out.inserted = true;
    return out;
}

// 0x3c31b0 -- literal lift (stack arguments: the 24-byte hint iterator; only its node at +0x10 is read).
RbInsertResult rb_hint_insert_3c31b0(RbMap& map, const SharedStr& keyp, RbNode* node, RbNode* hint, const RbHooks& hooks) noexcept {
    RbNode* head = nullptr;
    RbNode* leftmost = nullptr;
    RbNode* r13 = hint;
    RbNode* r12 = nullptr;     // rightmost (0x3c3275)
    RbNode* rbx = nullptr;
    RbNode* r14 = nullptr;
    RbNode* rax = nullptr;
    RbNode* rcx = nullptr;
    RbNode* rdx = nullptr;
    const std::uint8_t* key = keyp.data;
    RbInsertResult res{};
    RbInsertResult out{};

    if (map.size == 0) {
        res = rb_insert_at_3c3470(map, true, map.head, node, hooks);   // 0x3c3236
        return res;
    }
    head = map.head;
    leftmost = head->left;
    if (r13 == leftmost) goto L3256;
    if (r13 == head) goto L3275;
    if (cmp(hooks, key, r13->key.data) >= 0) goto L330f;
    if (!is_nil(r13)) goto L32a4;
    r14 = r13->right;
    goto L32fb;
L3256:
    if (cmp(hooks, key, leftmost->key.data) >= 0) goto L3396;
    rbx = leftmost;
    goto L33d6;
L3275:
    r12 = head->right;
    if (cmp(hooks, r12->key.data, key) >= 0) goto L3396;
    return rb_insert_at_3c3470(map, false, r12, node, hooks);   // 0x3c3294
L32a4:   // key < hint.key, hint is a real node: r14 = predecessor of hint
    rax = r13->left;
    if (!is_nil(rax)) goto L32e0;
    rax = r13->parent;
    if (is_nil(rax)) goto L32f8;
    r14 = r13;
    rcx = r13;
    rdx = rax;
L32c1:
    if (rcx != rdx->left) goto L32ef;
    rax = rdx->parent;
    r14 = rdx;
    rcx = rdx;
    if (is_nil(rax)) goto L32f2;
    rdx = rax;
    goto L32c1;
L32e0:
    r14 = rax;
    rax = r14->right;
    if (!is_nil(rax)) goto L32e0;
    goto L32fb;
L32ef:
    rax = rdx;
L32f2:
    if (is_nil(rcx)) goto L32fb;
L32f8:
    r14 = rax;
L32fb:
    if (is_nil(r14)) return out;   // head key would be read (uninitialised) -- unreachable for a valid tree
    if (cmp(hooks, r14->key.data, key) < 0) goto L340f;
L330f:   // hint.key <= key?  (equal or greater goes to the full unique insert)
    if (cmp(hooks, r13->key.data, key) >= 0) goto L3396;
    rbx = r13;
    if (is_nil(r13)) goto L337c;
    rax = r13->right;
    if (!is_nil(rax)) goto L3370;
    rax = r13;
L3350:
    rbx = rax->parent;
    if (is_nil(rbx)) goto L337c;
    {
        const bool was_right = (rax == rbx->right);
        rax = rbx;
        if (was_right) goto L3350;
    }
    goto L337c;
L3370:
    rbx = rax;
    rax = rbx->left;
    if (!is_nil(rax)) goto L3370;
L337c:   // rbx = successor of the hint
    if (rbx == head) goto L33bf;
    if (cmp(hooks, key, rbx->key.data) < 0) goto L33bf;
L3396:
    return rb_unique_insert_3c3710(map, 0, keyp, node, hooks);
L33bf:
    rax = r13->right;
    if (!is_nil(rax)) goto L33d6;
    return rb_insert_at_3c3470(map, false, r13, node, hooks);   // 0x33c9
L33d6:
    return rb_insert_at_3c3470(map, true, rbx, node, hooks);
L340f:
    rax = r14->right;
    if (!is_nil(rax)) goto L342a;
    return rb_insert_at_3c3470(map, false, r14, node, hooks);   // 0x3419
L342a:
    return rb_insert_at_3c3470(map, true, r13, node, hooks);
}

// 0x3c0840
RbIndexResult rb_index_3c0840(RbMap& map, const SharedStr& key, const RbHooks& hooks) noexcept {
    RbIndexResult out{};
    RbNode* const head = map.head;
    RbNode* rbx = nullptr;
    RbNode* r14 = nullptr;
    RbNode* root = head->parent;
    if (is_nil(root)) {
        rbx = head;   // 0x3c0878
    } else {
        r14 = head;
        rbx = root;
        for (;;) {   // 0x3c0940: lower bound
            if (cmp(hooks, rbx->key.data, key.data) >= 0) {
                RbNode* const l = rbx->left;
                r14 = rbx;
                if (is_nil(l)) {
                    rbx = r14;
                    break;
                }
                rbx = l;
            } else {
                rbx = rbx->right;
                if (is_nil(rbx)) {
                    rbx = r14;
                    break;
                }
            }
        }
        if (rbx != head) {
            if (cmp(hooks, key.data, rbx->key.data) >= 0) {   // 0x3c098f jns -> found
                out.ok = true;
                out.slot = &rbx->mapped;
                return out;
            }
        }
    }
    // 0x3c087b: allocate and build the node
    const AllocWrapperResult a = alloc_17070(kRbDataNodeBytes, hooks.alloc);
    if (!a.ok || a.pointer == nullptr) return out;
    RbNode* const node = static_cast<RbNode*>(a.pointer);
    node->left = head;
    node->parent = head;
    node->right = head;
    node->color = 0;
    node->isnil = 0;
    node->key.data = key.data;
    node->key.size = key.size;
    node->key.aux = key.aux;
    node->key.ctl = key.ctl;
    if (key.ctl != nullptr) reinterpret_cast<std::atomic<std::int64_t>*>(key.ctl)->fetch_add(1, std::memory_order_seq_cst);   // lock incq
    node->mapped = nullptr;
    const RbInsertResult r = rb_hint_insert_3c31b0(map, node->key, node, rbx, hooks);
    if (!r.ok) return out;
    out.ok = true;
    out.slot = &r.node->mapped;
    out.created = r.inserted;
    return out;
}

// 0x3be2d0
RbRegisterResult rb_register_entry_3be2d0(RbMap& map, void* entry, const RbHooks& hooks) noexcept {
    RbRegisterResult out{};
    const SharedStr& key = *reinterpret_cast<const SharedStr*>(static_cast<std::byte*>(entry) + kRbEntryKeyOffset);
    RbNode* const head = map.head;
    RbNode* r15 = head;   // lower-bound candidate (r15 in 0x3be2d0 starts at the head: r13)
    RbNode* cur = head->parent;
    if (!is_nil(cur)) {
        RbNode* cand = head;   // r15 = r13 = head
        while (!is_nil(cur)) {
            if (cmp(hooks, cur->key.data, key.data) >= 0) {
                cand = cur;
                cur = cur->left;
            } else {
                cur = cur->right;
            }
        }
        r15 = cand;
        if (r15 != head && cmp(hooks, key.data, r15->key.data) < 0) r15 = head;   // cmovs
    }
    if (r15 != head && r15->mapped != nullptr) {
        out.ok = true;
        out.already_registered = true;
        return out;
    }
    const RbIndexResult idx = rb_index_3c0840(map, key, hooks);
    if (!idx.ok) return out;
    *idx.slot = entry;
    out.ok = true;
    out.stored = true;
    out.created_node = idx.created;
    return out;
}

RbCheck rb_check_invariants(const RbMap& map) noexcept {
    RbCheck out{};
    const RbNode* const head = map.head;
    if (head == nullptr || head->isnil != 1 || head->color != 1) return out;
    const RbNode* const root = head->parent;
    if (is_nil(root)) {
        out.ok = head->left == head && head->right == head && map.size == 0;
        return out;
    }
    if (root->color != 1 || root->parent != head) return out;
    std::uint64_t count = 0;
    bool ok = true;
    // iterative-safe recursion through a lambda stack is unnecessary for the test sizes; use explicit recursion
    struct Walk {
        static int go(const RbNode* n, const RbNode* lo, const RbNode* hi, std::uint64_t& cnt, bool& good) {
            if (n->isnil) return 1;
            ++cnt;
            if (n->left->isnil == 0 && n->left->parent != n) good = false;
            if (n->right->isnil == 0 && n->right->parent != n) good = false;
            if (n->color == 0 && ((n->left->isnil == 0 && n->left->color == 0) || (n->right->isnil == 0 && n->right->color == 0))) good = false;
            (void)lo;
            (void)hi;
            const int bl = go(n->left, lo, n, cnt, good);
            const int br = go(n->right, n, hi, cnt, good);
            if (bl != br) good = false;
            return bl + (n->color == 1 ? 1 : 0);
        }
    };
    out.black_height = Walk::go(root, nullptr, nullptr, count, ok);
    const RbNode* l = root;
    while (l->left->isnil == 0) l = l->left;
    const RbNode* r = root;
    while (r->right->isnil == 0) r = r->right;
    if (head->left != l || head->right != r || count != map.size) ok = false;
    out.count = count;
    out.ok = ok;
    return out;
}

}  // namespace sotc
