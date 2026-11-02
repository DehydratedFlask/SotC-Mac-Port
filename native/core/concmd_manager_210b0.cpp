// CPU-01-CONCMD-MANAGER-210B0-01 — native `0x210b0` (manager constructor + pending drain).
// See concmd_manager_210b0.hpp for the byte evidence, the two-phase body and the host-only additions.
//
// Nothing here executes, relocates or dereferences an original RVA: every original address in this
// file is an identity tag. The manager object and the pending list are caller-supplied memory, read
// and written at the pinned offsets only.

#include "concmd_manager_210b0.hpp"

#include <cstddef>
#include <cstring>

namespace sotc {
namespace {

// The two link offsets inside the list element. 0x20d80 writes them and 0x2116b/0x21198 read them.
inline constexpr std::size_t kPendingNextOffset = 0x08;
inline constexpr std::size_t kPendingPrevOffset = 0x10;
static_assert(offsetof(ConCmdBase, pending) + offsetof(ConCmdListNode, next) == kPendingNextOffset);
static_assert(offsetof(ConCmdBase, pending) + offsetof(ConCmdListNode, prev) == kPendingPrevOffset);
static_assert(offsetof(ConCmdPendingList, head) == 0x00);

[[nodiscard]] void* load_ptr(const void* base, std::size_t offset) noexcept {
    void* value = nullptr;
    std::memcpy(&value, static_cast<const std::uint8_t*>(base) + offset, sizeof(value));
    return value;
}

void store_ptr(void* base, std::size_t offset, const void* value) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof(value));
}

void store_u64(void* base, std::size_t offset, std::uint64_t value) noexcept {
    std::memcpy(static_cast<std::uint8_t*>(base) + offset, &value, sizeof(value));
}

void store_byte(void* base, std::size_t offset, std::uint8_t value) noexcept {
    static_cast<std::uint8_t*>(base)[offset] = value;
}

void store_zero(void* base, std::size_t offset, std::size_t size) noexcept {
    std::memset(static_cast<std::uint8_t*>(base) + offset, 0, size);
}

// `element = link - 8` (0x21144/0x2116f/0x21186/0x2119c), with the body's `cmoveq` null collapse.
[[nodiscard]] void* element_of(void* link) noexcept {
    if (link == nullptr) return nullptr;
    return static_cast<std::uint8_t*>(link) + kConCmdManager210b0SentinelDelta;
}

int critsec_init_with(const ConCmdManagerCtorHooks& hooks, void* handle) noexcept {
    return hooks.critsec_init != nullptr ? hooks.critsec_init(hooks.host, handle) : critsec_init_at(handle);
}

ConCmdRegisterResult register_body_with(const ConCmdManagerCtorHooks& hooks, ConCmdBase& cmd,
                                        void* manager, ConCmdRegistry& registry) noexcept {
    return hooks.register_body != nullptr ? hooks.register_body(hooks.host, cmd, manager, registry)
                                          : register_concmd_body_20d80(cmd, manager, registry);
}

}  // namespace

ConCmdManagerCtorResult construct_concmd_manager_210b0(void* manager, ConCmdRegistry& registry,
                                                       const ConCmdManagerCtorHooks& hooks) noexcept {
    ConCmdManagerCtorResult out{};

    // Host guard: retail gets the block from `new(0x60)` and never sees null.
    if (manager == nullptr) {
        out.fail = ConCmdManagerCtorFail::null_manager;
        return out;
    }
    auto* const base = static_cast<std::uint8_t*>(manager);

    // ---- 0x210c9..0x210ea: eight stores and nothing else -----------------------------------------
    const auto self = reinterpret_cast<std::uint64_t>(base) + kConCmdManager210b0SelfLinkAOffset;
    store_u64(base, kConCmdManager210b0SelfLinkAOffset, self);   // 0x210c9
    store_u64(base, kConCmdManager210b0SelfLinkBOffset, self);   // 0x210cd
    store_u64(base, kConCmdManager210b0RootOffset, 0);           // 0x210d1
    store_zero(base, kConCmdManager210b0ZeroSpanAOffset, kConCmdManager210b0ZeroSpanBytes);  // 0x210d9
    store_byte(base, kConCmdManager210b0ZeroByteOffset, 0);      // 0x210df
    store_zero(base, kConCmdManager210b0ZeroSpanBOffset, kConCmdManager210b0ZeroSpanBytes);  // 0x210e4
    store_u64(base, kConCmdManager210b0ZeroQwordOffset, 0);      // 0x210ea

    // ---- 0x210f2: CritSec init on manager+0x58 (retail discards the code; the host reports it) ----
    out.mutex_error = critsec_init_with(hooks, base + kConCmdManager210b0MutexOffset);
    if (out.mutex_error != 0) {
        out.fail = ConCmdManagerCtorFail::mutex_init;
    }

    // ---- 0x210f7..0x21135: the guarded static list, byte-identical to the register/lookup blocks --
    out.list_initialised = ensure_concmd_pending_list(registry);

    ConCmdListNode& head = registry.pending.head;
    void* const sentinel = static_cast<std::uint8_t*>(static_cast<void*>(&head)) +
                           kConCmdManager210b0SentinelDelta;

    // ---- 0x21136..0x2117d: the FIFO drain, through the register BODY 0x20d80 --------------------
    void* link = head.next;  // 0x21136
    if (link == nullptr) {
        // 0x2114b collapses the element to null, but 0x21152 would then fall into 0x21160 and call
        // Register(nullptr, manager). Fail visibly instead of faulting.
        out.fail = ConCmdManagerCtorFail::null_head_link;
        return out;
    }
    while (element_of(link) != sentinel) {
        void* const element = element_of(link);
        const ConCmdRegisterResult registered =
            register_body_with(hooks, *static_cast<ConCmdBase*>(element), manager,
                               registry);  // 0x21166, rsi = the manager under construction
        ++out.drained;
        if (!registered.ok) {
            ++out.register_failures;
            if (out.fail == ConCmdManagerCtorFail::none) {
                out.fail = ConCmdManagerCtorFail::register_refused;
            }
        }
        link = load_ptr(element, kPendingNextOffset);  // 0x2116b
        if (link == nullptr) {
            // Retail would loop with a null element and call Register(nullptr, manager).
            out.fail = ConCmdManagerCtorFail::null_drained_link;
            return out;
        }
    }

    // ---- 0x2117f..0x211af: re-terminate the head through the first element's predecessor ----------
    void* const first_link = head.next;  // 0x2117f, re-read after the drain
    if (first_link == nullptr) {
        out.fail = ConCmdManagerCtorFail::null_head_link;
        return out;
    }
    void* const first = element_of(first_link);
    // 0x21198. For the sentinel element (empty head) this read is exactly `head.prev`, because the
    // sentinel is `&head - 8`; reading it as such keeps the host inside its own object.
    void* const prev_link = first == sentinel ? static_cast<void*>(head.prev)
                                              : load_ptr(first, kPendingPrevOffset);
    if (prev_link == nullptr) {
        // 0x211a7 would store through null+8.
        out.fail = ConCmdManagerCtorFail::null_tail_link;
        return out;
    }
    void* const tail = element_of(prev_link);
    if (tail == sentinel) {
        head.next = &head;  // 0x211a7 with the sentinel: the store lands on head.next
    } else {
        store_ptr(tail, kPendingNextOffset, &head);  // 0x211a7
    }
    head.prev = static_cast<ConCmdListNode*>(prev_link);  // 0x211ab / 0x211af

    out.list_emptied = head.next == &head && head.prev == &head;
    out.ok = out.fail == ConCmdManagerCtorFail::none;
    return out;
}

std::uint64_t concmd_manager_qword(const void* manager, std::size_t offset) noexcept {
    std::uint64_t value = 0;
    if (manager == nullptr) return value;
    std::memcpy(&value, static_cast<const std::uint8_t*>(manager) + offset, sizeof(value));
    return value;
}

}  // namespace sotc
