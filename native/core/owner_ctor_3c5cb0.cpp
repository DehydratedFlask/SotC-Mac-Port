#include "owner_ctor_3c5cb0.hpp"

#include <cstring>
#include <type_traits>

#include "init_sub_3be930.hpp"

namespace sotc {
namespace {

constexpr std::uint8_t kZero16[16] = {};

template <typename T>
void put(std::uint8_t* object, std::size_t offset, T value) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);
    std::memcpy(object + offset, &value, sizeof value);
}

void put_bytes(std::uint8_t* object, std::size_t offset, const void* source,
               std::size_t size) noexcept {
    std::memcpy(object + offset, source, size);
}

void put_pointer(std::uint8_t* object, std::size_t offset, const void* value) noexcept {
    static_assert(sizeof(void*) == sizeof(std::uint64_t));
    std::memcpy(object + offset, &value, sizeof value);
}

// `mov [rax], rax; mov [rax+8], rax`: a self-linked sentinel. Only these two qwords are written;
// the rest of the freshly allocated node keeps whatever the allocator returned.
void self_link(void* node) noexcept {
    auto* const bytes = static_cast<std::uint8_t*>(node);
    put_pointer(bytes, 0x00, node);
    put_pointer(bytes, 0x08, node);
}

OwnerRow18Binding* g_row18_binding = nullptr;

}  // namespace

static_assert(kOwnerRow18Slot0Offset + 0x20 <= kOwnerRow18HeadAOffset);
static_assert(kOwnerRow18Slot1Offset + 0x20 == kOwnerRow18MutexOffset);
static_assert(kOwnerRow18MutexOffset + 0x08 == kOwnerCtor3C5CB0ObjSize);
static_assert(kOwnerRow18Qword84Offset + 0x08 <= kOwnerRow18Slot1Offset);
static_assert(kOwnerRow18CountAOffset == kOwnerRow18HeadAOffset + 0x08);
static_assert(kOwnerRow18CountBOffset == kOwnerRow18HeadBOffset + 0x08);
static_assert(kOwnerRow18CountCOffset == kOwnerRow18HeadCOffset + 0x08);
static_assert(kOwnerDtor3C74B0Rva > kOwnerCtor3C5CB0Rva);

OwnerRow18Result owner_ctor_3c5cb0(void* object, void* pool,
                                   const OwnerRow18Hooks& hooks) noexcept {
    if (object == nullptr) {
        return {false, "null-object"};
    }
    if (reinterpret_cast<std::uintptr_t>(object) % alignof(std::uint64_t) != 0) {
        return {false, "object-align"};
    }

    auto* const o = static_cast<std::uint8_t*>(object);

    // ---- 0x3c5cd5, 0x3c5cd8: byte +0x00 and dword +0x04 (the 3 + 3 bytes between are holes). --
    put<std::uint8_t>(o, kOwnerRow18Byte00Offset, 0u);
    put<std::uint32_t>(o, kOwnerRow18Dword04Offset, 0u);

    // ---- 0xf180(this + 8): the 0x20 B empty slot (0x3c5cdf). ---------------------------------
    init_slot_f180(o + kOwnerRow18Slot0Offset);

    // ---- 0x3c5ced: xmm zero at +0x30, then 0x17070(0x30) and the self-link (0x3c5cf2..07). ---
    put_bytes(o, kOwnerRow18HeadAOffset, kZero16, 16);
    if (hooks.allocate == nullptr) {
        return {false, "alloc_list_a"};
    }
    void* const node_a = hooks.allocate(kOwnerRow18NodeASize);
    if (node_a == nullptr) {
        return {false, "alloc_list_a"};
    }
    self_link(node_a);
    put_pointer(o, kOwnerRow18HeadAOffset, node_a);

    // ---- 0x3c5d0b: xmm zero at +0x48, then 0x17070(0x30) (0x3c5d10..25). ----------------------
    put_bytes(o, kOwnerRow18HeadBOffset, kZero16, 16);
    void* const node_b = hooks.allocate(kOwnerRow18NodeBSize);
    if (node_b == nullptr) {
        return {false, "alloc_list_b"};
    }
    self_link(node_b);
    put_pointer(o, kOwnerRow18HeadBOffset, node_b);
    put<std::uint32_t>(o, kOwnerRow18Dword58Offset, 0u);
    put_bytes(o, kOwnerRow18HeadCOffset, kZero16, 16);

    // ---- 0x3c5d35: 0x17070(0x50) and the +0x78.. scalars (0x3c5d41..68). ----------------------
    void* const node_c = hooks.allocate(kOwnerRow18NodeCSize);
    if (node_c == nullptr) {
        return {false, "alloc_list_c"};
    }
    self_link(node_c);
    put_pointer(o, kOwnerRow18HeadCOffset, node_c);
    put<std::uint32_t>(o, kOwnerRow18Dword78Offset, kOwnerRow18Dword78Value);
    put<std::uint32_t>(o, kOwnerRow18Dword7COffset, 0u);
    put<std::uint8_t>(o, kOwnerRow18Byte80Offset, 0u);
    put<std::uint8_t>(o, kOwnerRow18Byte81Offset, 0u);
    put<std::uint64_t>(o, kOwnerRow18Qword84Offset, 0u);

    // ---- 0xf180(this + 0x90) (0x3c5d73), then 0x3b310(this + 0xb0) (0x3c5d7f). -----------------
    init_slot_f180(o + kOwnerRow18Slot1Offset);
    if (hooks.mutex_init == nullptr || !hooks.mutex_init(o + kOwnerRow18MutexOffset)) {
        return {false, "mutex_init"};
    }

    // ---- 0x116040(pool, &{this, [0x15ff810], 0, 0}, 1) (0x3c5d84..a9). ------------------------
    OwnerRow18Callable first{};
    first.self = object;
    first.function = kOwnerRow18CallbackARva;
    first.word2 = 0;
    first.word3 = 0;
    if (hooks.register_116040 == nullptr ||
        !hooks.register_116040(pool, first, kOwnerRow18RegisterAFlag)) {
        return {false, "register_116040"};
    }

    // ---- 0x116540(pool, &{this, [0x15ff590], 0, 0}) (0x3c5dae..ce). ---------------------------
    OwnerRow18Callable second{};
    second.self = object;
    second.function = kOwnerRow18CallbackBRva;
    second.word2 = 0;
    second.word3 = 0;
    if (hooks.register_116540 == nullptr || !hooks.register_116540(pool, second)) {
        return {false, "register_116540"};
    }

    return {true, nullptr};
}

void owner_row18_bind(OwnerRow18Binding* binding) noexcept {
    g_row18_binding = binding;
}

bool owner_row18_ctor_hook(std::size_t row, void* object, void* arg) noexcept {
    if (row != kOwnerMemberRow18 || g_row18_binding == nullptr) {
        return false;
    }
    g_row18_binding->last = owner_ctor_3c5cb0(object, arg, g_row18_binding->hooks);
    g_row18_binding->last_pool = arg;
    ++g_row18_binding->calls;
    return g_row18_binding->last.ok;
}

}  // namespace sotc
