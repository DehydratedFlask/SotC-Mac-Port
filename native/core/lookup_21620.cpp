// CPU-01-INIT-21620-LOOKUP-01 — native `bpe::ConCmdManager::FindConCmd` (`0x21620`).
// See lookup_21620.hpp for the byte evidence, the two paths and the host-only additions.
//
// Nothing here executes, relocates or dereferences an original RVA: every original address in this
// file is an identity tag. The collections are the caller's (`ConCmdRegistry`), and the manager
// object is read through caller-supplied memory laid out at the pinned offsets.

#include "lookup_21620.hpp"

#include <cstddef>
#include <cstring>

namespace sotc {
namespace {

[[nodiscard]] void* load_ptr(const void* base, std::size_t offset) noexcept {
    void* value = nullptr;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof(value));
    return value;
}

[[nodiscard]] std::uint64_t load_u64(const void* base, std::size_t offset) noexcept {
    std::uint64_t value = 0;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof(value));
    return value;
}

// `end = manager + 8` (0x21695) and `element = link - 8` (0x21768/0x217aa): both are byte offsets.
[[nodiscard]] void* byte_offset(void* base, std::ptrdiff_t delta) noexcept {
    return static_cast<std::uint8_t*>(base) + delta;
}

[[nodiscard]] constexpr int ascii_fold(unsigned char c) noexcept {
    return (c >= 'A' && c <= 'Z') ? static_cast<int>(c) + 32 : static_cast<int>(c);
}

// The retail argument is a `bpe::String` *at an address* (0x91227 passes `this+8`, 0x53327d passes a
// stack block), so the adapters read the four fields out of the caller's memory instead of assuming a
// properly typed object lives there.
[[nodiscard]] SharedStr load_shared_str(const void* descriptor) noexcept {
    SharedStr view{};
    std::memcpy(&view, descriptor, sizeof(view));
    return view;
}

int compare_with(const FindConCmdHooks& hooks, const void* lhs, const void* rhs) noexcept {
    const auto* a = static_cast<const char*>(lhs);
    const auto* b = static_cast<const char*>(rhs);
    return hooks.compare != nullptr ? hooks.compare(hooks.host, a, b) : concmd_strcasecmp(a, b);
}

// The retail body discards both pthread return values; the host model refuses to continue unlocked
// (host-only addition, recorded in the header).
int lock_with(const FindConCmdHooks& hooks, void* handle) noexcept {
    return hooks.lock != nullptr ? hooks.lock(hooks.host, handle) : critsec_lock_at(handle);
}

int unlock_with(const FindConCmdHooks& hooks, void* handle) noexcept {
    return hooks.unlock != nullptr ? hooks.unlock(hooks.host, handle) : critsec_unlock_at(handle);
}

struct InstalledContext {
    ConCmdRegistry* registry = nullptr;
    FindConCmdHooks hooks{};
    bool installed = false;
};

[[nodiscard]] InstalledContext& installed_context() noexcept {
    static InstalledContext context{};
    return context;
}

}  // namespace

int concmd_strcasecmp(const char* lhs, const char* rhs) noexcept {
    const auto* a = reinterpret_cast<const unsigned char*>(lhs);
    const auto* b = reinterpret_cast<const unsigned char*>(rhs);
    for (;;) {
        // C-locale `strcasecmp`: ASCII fold, compare as unsigned char, only the sign is meaningful.
        const int ca = a != nullptr ? ascii_fold(*a) : 0;
        const int cb = b != nullptr ? ascii_fold(*b) : 0;
        if (ca != cb) {
            return ca - cb;
        }
        if (ca == 0) {
            return 0;
        }
        ++a;  // never reached with a null operand: ca == cb == 0 returns above
        ++b;
    }
}

FindConCmdResult find_concmd_21620(const SharedStr& name, ConCmdRegistry& registry,
                                   const FindConCmdHooks& hooks) noexcept {
    FindConCmdResult out{};
    const auto* const key_name = reinterpret_cast<const char*>(name.data);
    if (key_name == nullptr) {
        return out;  // host guard: the retail body would call strcasecmp with a null name
    }

    // 0x2164a: `movq [0x2dfcbc8],%r12` — the lock target, read before the lock.
    void* const lock_target = registry.manager.manager;
    if (lock_target != nullptr) {
        out.path = FindConCmdPath::manager_map;
        void* const handle = byte_offset(lock_target, static_cast<std::ptrdiff_t>(kLookup21620ManagerMutexOffset));
        const int lock_rc = lock_with(hooks, handle);
        if (lock_rc != 0) {
            out.lock_error = lock_rc;
            return out;  // failed, unlocked, no result
        }
        out.locked = true;

        // 0x2168e: `movq [0x2dfcbc8],%r15` — the cell is re-read under the lock. A host that swaps
        // the manager between the two reads gets the retail behaviour: lock the old, read the new.
        void* const base = registry.manager.manager;
        if (base == nullptr) {
            (void)unlock_with(hooks, handle);
            return out;  // host guard: 0x2168e would fault here
        }

        void* const end = byte_offset(base, static_cast<std::ptrdiff_t>(kLookup21620ManagerEndOffset));
        void* node = load_ptr(base, kLookup21620ManagerRootOffset);  // 0x21691
        void* current = end;                                        // 0x21699
        void* candidate = end;                                      // 0x216a5

        if (node != nullptr) {
            for (;;) {
                current = node;  // 0x216b0
                const void* const node_name = load_ptr(node, kLookup21620NodeKeyOffset);  // 0x216b3
                if (node_name == nullptr) {
                    (void)unlock_with(hooks, handle);
                    return out;  // host guard: corrupt collection
                }
                if (compare_with(hooks, node_name, key_name) < 0) {  // 0x216bf testl+jns: eax < 0 -> left
                    void* const left = load_ptr(node, kLookup21620NodeLeftOffset);  // 0x216c3
                    if (left != nullptr) {
                        node = left;
                        continue;
                    }
                    current = candidate;  // 0x217e1: the cold block re-enters the verify step
                    break;
                }
                void* const right = load_ptr(node, kLookup21620NodeRightOffset);  // 0x216d0
                candidate = node;                                                // 0x216d4
                if (right == nullptr) {
                    break;  // falls into verify with current == node
                }
                node = right;
            }
        }

        ConCmdBase* result = nullptr;
        if (current != end) {  // 0x216df
            const void* const current_name = load_ptr(current, kLookup21620NodeKeyOffset);  // 0x216e5
            if (current_name == nullptr) {
                (void)unlock_with(hooks, handle);
                return out;  // host guard: corrupt collection
            }
            // 0x216ec compares with the arguments swapped; 0x216f3 `js` means "key < node" -> miss.
            if (compare_with(hooks, key_name, current_name) >= 0) {
                result = static_cast<ConCmdBase*>(load_ptr(current, kLookup21620NodeValueOffset));
                out.status = FindConCmdStatus::hit;
            } else {
                out.status = FindConCmdStatus::miss;
            }
        } else {
            out.status = FindConCmdStatus::miss;  // 0x216df/0x216f8 both jump to 0x2170b
        }

        (void)unlock_with(hooks, handle);  // 0x21701 / 0x2170e — one unlock on every primary exit
        out.cmd = result;
        return out;
    }

    // ---- 0x2171b: manager absent -> the guarded static pending list -----------------------------
    out.path = FindConCmdPath::pending_list;
    out.list_initialised = ensure_concmd_pending_list(registry);  // 0x2171b..0x21755

    ConCmdListNode& head = registry.pending.head;
    void* const end = byte_offset(&head, -8);  // 0x21761: the sentinel "element" is head - 8
    void* link = head.next;                    // 0x2175a: `movq [0x2dfce50],%rax`
    if (link == nullptr) {
        out.status = FindConCmdStatus::failed;  // host guard: the initialised list is never headless
        return out;
    }
    void* element = byte_offset(link, -8);
    while (element != end) {
        const std::uint64_t entry_size = load_u64(element, kLookup21620PendingSizeOffset);  // 0x21790
        if (entry_size == static_cast<std::uint64_t>(name.size)) {
            const void* const entry_name = load_ptr(element, kLookup21620PendingNameOffset);  // 0x21796
            if (entry_name == nullptr) {
                out.status = FindConCmdStatus::failed;  // host guard: corrupt collection
                return out;
            }
            if (compare_with(hooks, entry_name, key_name) == 0) {  // 0x2179d/0x217a2
                out.cmd = static_cast<ConCmdBase*>(element);       // 0x217bc: the ConCmdBase itself
                out.status = FindConCmdStatus::hit;
                return out;
            }
        }
        void* const next = load_ptr(element, kLookup21620PendingNextOffset);  // 0x217a6
        if (next == nullptr) {
            out.status = FindConCmdStatus::failed;  // host guard: the retail list is circular
            return out;
        }
        element = byte_offset(next, -8);
    }

    out.status = FindConCmdStatus::miss;
    return out;
}

void* find_concmd_descriptor_hosted(void* host, void* key_descriptor) noexcept {
    auto* const context = static_cast<FindConCmdContext*>(host);
    if (context == nullptr || context->registry == nullptr || key_descriptor == nullptr) {
        return nullptr;
    }
    const SharedStr key = load_shared_str(key_descriptor);
    const FindConCmdResult result = find_concmd_21620(key, *context->registry, context->hooks);
    return result.status == FindConCmdStatus::hit ? static_cast<void*>(result.cmd) : nullptr;
}

bool install_concmd_registry(ConCmdRegistry& registry, const FindConCmdHooks& hooks) noexcept {
    InstalledContext& context = installed_context();
    context.registry = &registry;
    context.hooks = hooks;
    context.installed = true;
    return true;
}

void uninstall_concmd_registry() noexcept {
    InstalledContext& context = installed_context();
    context = InstalledContext{};
}

bool concmd_registry_installed() noexcept { return installed_context().installed; }

void* find_concmd_descriptor(void* key_descriptor) noexcept {
    InstalledContext& context = installed_context();
    if (!context.installed || context.registry == nullptr || key_descriptor == nullptr) {
        return nullptr;
    }
    const SharedStr key = load_shared_str(key_descriptor);
    const FindConCmdResult result = find_concmd_21620(key, *context.registry, context.hooks);
    return result.status == FindConCmdStatus::hit ? static_cast<void*>(result.cmd) : nullptr;
}

}  // namespace sotc
