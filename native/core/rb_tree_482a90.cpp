// CPU-01-AOT-CFG-SCALE-01-BATCH26 (RB erase 0x482a90): see rb_tree_482a90.hpp.
#include "rb_tree_482a90.hpp"

#include <new>

namespace sotc {
namespace {

void* rb_alloc(std::size_t bytes, const RbEraseHooks& hooks) noexcept {
    if (hooks.allocate) return hooks.allocate(bytes);
    return ::operator new(bytes, std::nothrow);
}

void rb_release(void* p, const RbEraseHooks& hooks) noexcept {
    if (!p) return;
    if (hooks.release) {
        hooks.release(p);
    } else {
        ::operator delete(p);
    }
}

void rotate_left(RbEraseTree& t, RbEraseNode* x) noexcept {
    RbEraseNode* const nil = t.header;
    RbEraseNode* const y = x->right;
    x->right = y->left;
    if (y->left != nil) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == nil) {
        nil->parent = y;                  // x was the root
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void rotate_right(RbEraseTree& t, RbEraseNode* x) noexcept {
    RbEraseNode* const nil = t.header;
    RbEraseNode* const y = x->left;
    x->left = y->right;
    if (y->right != nil) y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == nil) {
        nil->parent = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

RbEraseNode* minimum(RbEraseNode* n, RbEraseNode* nil) noexcept {
    while (n->left != nil) n = n->left;
    return n;
}

RbEraseNode* maximum(RbEraseNode* n, RbEraseNode* nil) noexcept {
    while (n->right != nil) n = n->right;
    return n;
}

std::size_t destroy_subtree(RbEraseNode* n, RbEraseNode* nil, const RbEraseHooks& hooks) noexcept {
    if (n == nil) return 0;
    std::size_t count = destroy_subtree(n->left, nil, hooks);
    count += destroy_subtree(n->right, nil, hooks);
    rb_release(n, hooks);
    return count + 1;
}

}  // namespace

bool rb_tree_init(RbEraseTree& tree, std::size_t header_bytes, const RbEraseHooks& hooks) noexcept {
    void* const mem = rb_alloc(header_bytes, hooks);
    if (!mem) return false;
    RbEraseNode* const header = static_cast<RbEraseNode*>(mem);
    header->left = header;
    header->right = header;
    header->parent = header;
    header->color = kRbBlack;
    header->nil = 1;
    tree.header = header;
    tree.size = 0;
    return true;
}

void rb_tree_release(RbEraseTree& tree, const RbEraseHooks& hooks) noexcept {
    if (!tree.header) return;
    (void)rb_clear_482960(tree, hooks);
    rb_release(tree.header, hooks);
    tree.header = nullptr;
    tree.size = 0;
}

RbEraseNode* rb_next(const RbEraseTree& tree, RbEraseNode* z) noexcept {
    RbEraseNode* const nil = tree.header;
    if (!z || z == nil) return nil;
    if (z->right != nil) return minimum(z->right, nil);
    RbEraseNode* p = z->parent;
    while (p != nil && z == p->right) {
        z = p;
        p = p->parent;
    }
    return p;
}

RbEraseNode* rb_erase_482a90(RbEraseTree& tree, RbEraseNode* z, const RbEraseHooks& hooks) noexcept {
    RbEraseNode* const nil = tree.header;
    if (!z || z->nil) return nullptr;     // 0x482aa5 -> 0x482f33 abort path

    RbEraseNode* const next = rb_next(tree, z);
    RbEraseNode* y = z;
    RbEraseNode* x = nil;
    RbEraseNode* xp = z->parent;

    // 0x482b08..0x482c23: choose the physical node and its replacement.
    if (z->left == nil) {
        x = z->right;
        xp = z->parent;
    } else if (z->right == nil) {
        x = z->left;
        xp = z->parent;
    } else {
        y = minimum(z->right, nil);
        x = y->right;
    }

    // 0x482b49 / 0x482b74: the header's leftmost / rightmost references are updated before the
    // transplant, exactly where the original checks `header->left == z` / `header->right == z`.
    // The header's root is `header->parent` (+0x08), not `header->right`.
    if (nil->left == z) nil->left = (z->right == nil) ? z->parent : minimum(x, nil);
    if (nil->right == z) nil->right = (z->left == nil) ? z->parent : maximum(x, nil);

    if (y != z) {
        // 0x482ba6: transplant y into z's place.
        z->left->parent = y;
        y->left = z->left;
        if (y != z->right) {
            xp = y->parent;
            if (x != nil) x->parent = y->parent;
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        } else {
            xp = y;
        }
        if (nil->parent == z) {
            nil->parent = y;
        } else if (z->parent->left == z) {
            z->parent->left = y;
        } else {
            z->parent->right = y;
        }
        y->parent = z->parent;
        const std::uint8_t colour = y->color;
        y->color = z->color;
        z->color = colour;
        y = z;
    } else {
        // 0x482c05: plain unlink.
        xp = y->parent;
        if (x != nil) x->parent = y->parent;
        if (nil->parent == z) {
            nil->parent = x;
        } else if (z->parent->left == z) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }
    }

    if (z->color == kRbBlack) {
        // 0x482c27..0x482f04: the four rebalance cases with visible nil nodes.
        while (x != nil->parent && (x == nil || x->color == kRbBlack)) {
            if (x == xp->left) {
                RbEraseNode* w = xp->right;
                if (w->color == kRbRed) {
                    w->color = kRbBlack;
                    xp->color = kRbRed;
                    rotate_left(tree, xp);
                    w = xp->right;
                }
                if (w->left->color == kRbBlack && w->right->color == kRbBlack) {
                    w->color = kRbRed;
                    x = xp;
                    xp = xp->parent;
                } else {
                    if (w->right->color == kRbBlack) {
                        w->left->color = kRbBlack;
                        w->color = kRbRed;
                        rotate_right(tree, w);
                        w = xp->right;
                    }
                    w->color = xp->color;
                    xp->color = kRbBlack;
                    w->right->color = kRbBlack;
                    rotate_left(tree, xp);
                    x = nil->parent;
                    xp = x->parent;
                }
            } else {
                RbEraseNode* w = xp->left;
                if (w->color == kRbRed) {
                    w->color = kRbBlack;
                    xp->color = kRbRed;
                    rotate_right(tree, xp);
                    w = xp->left;
                }
                if (w->right->color == kRbBlack && w->left->color == kRbBlack) {
                    w->color = kRbRed;
                    x = xp;
                    xp = xp->parent;
                } else {
                    if (w->left->color == kRbBlack) {
                        w->right->color = kRbBlack;
                        w->color = kRbRed;
                        rotate_left(tree, w);
                        w = xp->left;
                    }
                    w->color = xp->color;
                    xp->color = kRbBlack;
                    w->left->color = kRbBlack;
                    rotate_right(tree, xp);
                    x = nil->parent;
                    xp = x->parent;
                }
            }
        }
    }
    if (x != nil) x->color = kRbBlack;    // 0x482f04: `if (x) x->color = black`

    if (tree.size != 0) --tree.size;      // 0x482efc
    rb_release(z, hooks);                 // 0x482f08 -> 0x170b0
    return next;
}

std::size_t rb_clear_482960(RbEraseTree& tree, const RbEraseHooks& hooks) noexcept {
    if (!tree.header) return 0;
    RbEraseNode* const nil = tree.header;
    const std::size_t count = destroy_subtree(nil->parent, nil, hooks);  // 0x482a42 -> 0x483240
    nil->right = nil;   // 0x482a4c
    nil->left = nil;    // 0x482a55
    nil->parent = nil;  // 0x482a5d
    tree.size = 0;      // 0x482a61
    return count;
}

std::size_t rb_erase_all_loop(RbEraseTree& tree, const RbEraseHooks& hooks) noexcept {
    if (!tree.header) return 0;
    std::size_t count = 0;
    while (tree.header->parent != tree.header) {
        RbEraseNode* const victim = tree.header->left;   // 0x482987: leftmost
        if (!rb_erase_482a90(tree, victim, hooks)) break;
        ++count;
    }
    return count;
}

}  // namespace sotc
