#include "core/guest_thunks.hpp"

#include <cstdio>
#include <cstring>
#include <map>
#include <mutex>
#include <strings.h>

namespace sotc {
namespace {

#include "core/guest_thunk_table.inc"

GuestConCmdRegisterHook g_concmd_hook = nullptr;
GuestPrintfHook g_printf_hook = nullptr;

// Contract-level model of the 0x20d80 path. The original walks a case-insensitive red-black
// set keyed by the string pointer at node+0x20, inserts through 0x211d0 (0x42d30 fixup), and stores
// the original node pointer at tree-node+0x40. Tree-node+0x18 is the color byte used by 0x42d30.
// A null container takes the separate 0x20e7f branch: it lazily initializes the global sentinel at
// 0x2dfce50 and appends the original node to that circular list, without looking at its key.
struct ConCmdModelNode {
    std::uint64_t left = 0;
    std::uint64_t right = 0;
    std::uint64_t parent = 0;
    std::uint64_t color = 0;  // 0 black, 1 red; same byte 0x42d30 tests at +0x18
    std::uint64_t key = 0;
    std::uint64_t value0 = 0;
    std::uint64_t value1 = 0;
    std::uint64_t value2 = 0;
    std::uint64_t payload = 0;
};

struct ConCmdModel {
    std::mutex lock;
    ConCmdModelNode* root = nullptr;
    std::size_t size = 0;
    std::size_t rotations = 0;
};

struct ConCmdGlobal {
    std::mutex lock;
    alignas(8) unsigned char sentinel[0x20] = {};
    bool initialized = false;
    std::size_t count = 0;
};

ConCmdModel& concmd_model(const void* container) {
    static std::map<std::uintptr_t, ConCmdModel> models;
    const auto key = reinterpret_cast<std::uintptr_t>(container);
    auto it = models.find(key);
    if (it == models.end()) it = models.try_emplace(key).first;
    return it->second;
}

ConCmdGlobal& concmd_global() {
    static ConCmdGlobal global;
    return global;
}

ConCmdModelNode* as_node(std::uint64_t address) {
    return reinterpret_cast<ConCmdModelNode*>(address);
}

bool is_red(const ConCmdModelNode* node) {
    return node != nullptr && node->color != 0;
}

void rotate_left(ConCmdModel& model, ConCmdModelNode* x) {
    ConCmdModelNode* y = as_node(x->right);
    if (y == nullptr) return;
    x->right = y->left;
    if (y->left != 0) as_node(y->left)->parent = reinterpret_cast<std::uint64_t>(x);
    y->parent = x->parent;
    if (x->parent == 0) {
        model.root = y;
    } else {
        auto* parent = as_node(x->parent);
        if (parent->left == reinterpret_cast<std::uint64_t>(x)) parent->left = reinterpret_cast<std::uint64_t>(y);
        else parent->right = reinterpret_cast<std::uint64_t>(y);
    }
    y->left = reinterpret_cast<std::uint64_t>(x);
    x->parent = reinterpret_cast<std::uint64_t>(y);
    ++model.rotations;
}

void rotate_right(ConCmdModel& model, ConCmdModelNode* x) {
    ConCmdModelNode* y = as_node(x->left);
    if (y == nullptr) return;
    x->left = y->right;
    if (y->right != 0) as_node(y->right)->parent = reinterpret_cast<std::uint64_t>(x);
    y->parent = x->parent;
    if (x->parent == 0) {
        model.root = y;
    } else {
        auto* parent = as_node(x->parent);
        if (parent->left == reinterpret_cast<std::uint64_t>(x)) parent->left = reinterpret_cast<std::uint64_t>(y);
        else parent->right = reinterpret_cast<std::uint64_t>(y);
    }
    y->right = reinterpret_cast<std::uint64_t>(x);
    x->parent = reinterpret_cast<std::uint64_t>(y);
    ++model.rotations;
}

void insert_fixup(ConCmdModel& model, ConCmdModelNode* z) {
    while (z->parent != 0 && is_red(as_node(z->parent))) {
        auto* parent = as_node(z->parent);
        auto* grand = as_node(parent->parent);
        if (grand == nullptr) break;
        if (parent == as_node(grand->left)) {
            auto* uncle = as_node(grand->right);
            if (is_red(uncle)) {
                parent->color = 0;
                uncle->color = 0;
                grand->color = 1;
                z = grand;
            } else {
                if (z == as_node(parent->right)) {
                    z = parent;
                    rotate_left(model, z);
                    parent = as_node(z->parent);
                    grand = parent == nullptr ? nullptr : as_node(parent->parent);
                    if (parent == nullptr || grand == nullptr) break;
                }
                parent->color = 0;
                grand->color = 1;
                rotate_right(model, grand);
            }
        } else {
            auto* uncle = as_node(grand->left);
            if (is_red(uncle)) {
                parent->color = 0;
                uncle->color = 0;
                grand->color = 1;
                z = grand;
            } else {
                if (z == as_node(parent->left)) {
                    z = parent;
                    rotate_right(model, z);
                    parent = as_node(z->parent);
                    grand = parent == nullptr ? nullptr : as_node(parent->parent);
                    if (parent == nullptr || grand == nullptr) break;
                }
                parent->color = 0;
                grand->color = 1;
                rotate_left(model, grand);
            }
        }
    }
    if (model.root != nullptr) model.root->color = 0;
}

void append_global(std::uint64_t node) {
    ConCmdGlobal& global = concmd_global();
    std::lock_guard<std::mutex> guard(global.lock);
    const std::uint64_t sentinel = reinterpret_cast<std::uint64_t>(global.sentinel);
    if (!global.initialized) {
        std::memset(global.sentinel, 0, sizeof(global.sentinel));
        std::memcpy(global.sentinel, &sentinel, sizeof(sentinel));
        std::memcpy(global.sentinel + 8, &sentinel, sizeof(sentinel));
        global.initialized = true;
    }
    std::uint64_t tail = 0;
    std::memcpy(&tail, global.sentinel + 8, sizeof(tail));
    const std::uint64_t node_link = node + 8;
    std::memcpy(reinterpret_cast<void*>(node + 0x10), &tail, sizeof(tail));
    std::memcpy(reinterpret_cast<void*>(node + 8), &sentinel, sizeof(sentinel));
    std::memcpy(global.sentinel + 8, &node_link, sizeof(node_link));
    std::memcpy(reinterpret_cast<void*>(tail), &node_link, sizeof(node_link));
    ++global.count;
}

std::uint64_t thunk_concmd_registry_insert(std::uint64_t node, std::uint64_t container,
                                           std::uint64_t, std::uint64_t,
                                           std::uint64_t, std::uint64_t) {
    if (node == 0) return 0;
    if (container == 0) {
        append_global(node);
        return 0;
    }
    auto* node_bytes = reinterpret_cast<std::uint8_t*>(node);
    std::uint64_t key_address = 0;
    std::memcpy(&key_address, node_bytes + 0x20, sizeof(key_address));
    if (key_address == 0) return 0;
    const char* key = reinterpret_cast<const char*>(key_address);
    ConCmdModel& model = concmd_model(reinterpret_cast<const void*>(container));
    std::lock_guard<std::mutex> guard(model.lock);

    ConCmdModelNode* parent = nullptr;
    ConCmdModelNode* current = model.root;
    while (current != nullptr) {
        const int order = ::strcasecmp(key, reinterpret_cast<const char*>(current->key));
        if (order == 0) return 0;  // duplicate: no size, root, rotation or node change
        parent = current;
        current = order < 0 ? as_node(current->left) : as_node(current->right);
    }

    auto* inserted = new ConCmdModelNode();
    inserted->key = key_address;
    std::memcpy(&inserted->value0, node_bytes + 0x28, sizeof(inserted->value0));
    std::memcpy(&inserted->value1, node_bytes + 0x30, sizeof(inserted->value1));
    std::memcpy(&inserted->value2, node_bytes + 0x38, sizeof(inserted->value2));
    inserted->payload = node;
    inserted->color = 1;
    inserted->parent = reinterpret_cast<std::uint64_t>(parent);
    if (parent == nullptr) {
        model.root = inserted;
    } else if (::strcasecmp(key, reinterpret_cast<const char*>(parent->key)) < 0) {
        parent->left = reinterpret_cast<std::uint64_t>(inserted);
    } else {
        parent->right = reinterpret_cast<std::uint64_t>(inserted);
    }
    ++model.size;
    insert_fixup(model, inserted);

    std::uint64_t root_address = reinterpret_cast<std::uint64_t>(model.root);
    std::memcpy(reinterpret_cast<void*>(container + 0x18), &root_address, sizeof(root_address));
    const std::uint64_t size = model.size;
    std::memcpy(reinterpret_cast<void*>(container + 0x28), &size, sizeof(size));
    return 0;
}

// 0x20d60: `cmpb $0,0x18(%rdi); je +1; ret` then flag store, manager reload, jmp 0x20d80.
std::uint64_t thunk_concmd_lazy_register(std::uint64_t self, std::uint64_t, std::uint64_t,
                                         std::uint64_t, std::uint64_t, std::uint64_t) {
    if (self == 0) return 0;
    auto* bytes = reinterpret_cast<std::uint8_t*>(self);
    if (bytes[0x18] != 0) return 0;
    bytes[0x18] = 1;
    if (g_concmd_hook != nullptr) g_concmd_hook(reinterpret_cast<void*>(self), nullptr);
    return 0;
}

// 0x9844b0: `lea "%s",%rdi; xor eax,eax; mov rdx,rsi; jmp printf@plt`.
std::uint64_t thunk_printf_string(std::uint64_t, std::uint64_t, std::uint64_t text,
                                  std::uint64_t, std::uint64_t, std::uint64_t) {
    const char* message = reinterpret_cast<const char*>(text);
    if (message == nullptr) return 0;
    if (g_printf_hook != nullptr) {
        g_printf_hook(message);
        return std::strlen(message);
    }
    return static_cast<std::uint64_t>(std::printf("%s", message));
}

constexpr GuestThunk kCustomThunks[] = {
    {0x20d60ULL, "concmd_lazy_register", thunk_concmd_lazy_register, 1},
    {0x20d80ULL, "concmd_registry_insert", thunk_concmd_registry_insert, 2},
    {0x9844b0ULL, "retail_printf_string", thunk_printf_string, 3},
};

}  // namespace

void guest_set_concmd_register_hook(GuestConCmdRegisterHook hook) noexcept {
    g_concmd_hook = hook;
}

void guest_set_printf_hook(GuestPrintfHook hook) noexcept {
    g_printf_hook = hook;
}

std::uint64_t guest_concmd_root_node(const void* container) noexcept {
    if (container == nullptr) return 0;
    return reinterpret_cast<std::uint64_t>(concmd_model(container).root);
}

std::size_t guest_concmd_registered_count(const void* container) noexcept {
    if (container == nullptr) return 0;
    return concmd_model(container).size;
}

std::size_t guest_concmd_rotation_count(const void* container) noexcept {
    if (container == nullptr) return 0;
    return concmd_model(container).rotations;
}

std::size_t guest_concmd_global_count() noexcept {
    return concmd_global().count;
}

const GuestThunk* guest_thunk_find(std::uint64_t rva) noexcept {
    for (const GuestThunk& thunk : kGuestLeafThunks) {
        if (thunk.rva == rva) return &thunk;
    }
    for (const GuestThunk& thunk : kCustomThunks) {
        if (thunk.rva == rva) return &thunk;
    }
    return nullptr;
}

std::size_t guest_thunk_count() noexcept {
    return sizeof(kGuestLeafThunks) / sizeof(kGuestLeafThunks[0]) +
           sizeof(kCustomThunks) / sizeof(kCustomThunks[0]);
}

bool guest_thunk_invoke(std::uint64_t rva, const std::uint64_t* args,
                        std::uint64_t* result) noexcept {
    const GuestThunk* thunk = guest_thunk_find(rva);
    if (thunk == nullptr) return false;
    std::uint64_t a[6] = {0, 0, 0, 0, 0, 0};
    for (std::uint8_t i = 0; i < thunk->arg_count && i < 6; ++i) {
        if (args != nullptr) a[i] = args[i];
    }
    const std::uint64_t value = thunk->fn(a[0], a[1], a[2], a[3], a[4], a[5]);
    if (result != nullptr) *result = value;
    return true;
}

const GuestDispatchSite& guest_dispatch_site() noexcept {
    return kGuestDispatchSite;
}

const std::uint64_t* guest_resolved_targets(std::size_t* count) noexcept {
    if (count != nullptr) *count = sizeof(kGuestResolvedTargets) / sizeof(kGuestResolvedTargets[0]);
    return kGuestResolvedTargets;
}

const std::uint8_t* guest_dispatch_vtable(std::size_t* size) noexcept {
    if (size != nullptr) *size = kGuestDispatchVtableSize;
    return kGuestDispatchVtable;
}

bool guest_dispatch_slot(const std::uint8_t* vtable, std::size_t vtable_size,
                         std::size_t slot_offset, const std::uint64_t* args,
                         std::uint64_t* result, std::uint64_t* target_rva) noexcept {
    if (vtable == nullptr || slot_offset + sizeof(std::uint64_t) > vtable_size) return false;
    std::uint64_t target = 0;
    std::memcpy(&target, vtable + slot_offset, sizeof(target));
    if (target_rva != nullptr) *target_rva = target;
    return guest_thunk_invoke(target, args, result);
}

}  // namespace sotc
