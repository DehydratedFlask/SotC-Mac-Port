#include "owner_ctor_14db00.hpp"

#include <atomic>
#include <cstring>
#include <exception>
#include <type_traits>

#include "shared_str_release_f590.hpp"

namespace sotc {
namespace {

constexpr std::uint8_t kZero16[16] = {};
constexpr std::uint8_t kZero32[32] = {};

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

// Both holders have a null control (see the header), so this deallocator is unreachable; it exists
// because `release_shared_str_f590` requires one and "no default allocator" is a contract there.
// Reaching it would mean a holder gained a control block, which the stores above exclude.
class TripwireDeallocator final : public SharedStrDeallocatorF590 {
public:
    void deallocate(const void*, SharedStrDeallocationKindF590) noexcept override {
        std::terminate();
    }
};

// `lock inc qword [ctl]` for a non-null control.
void retain(const SharedStr& str) noexcept {
    if (str.ctl != nullptr) {
        std::atomic_ref<std::int64_t>(*str.ctl).fetch_add(1, std::memory_order_seq_cst);
    }
}

// Copies the 0x20 B holder as two movups-style 16 B halves plus the two trailing qwords.
void copy_holder(std::uint8_t* object, std::size_t offset, const SharedStr& str) noexcept {
    put_bytes(object, offset, &str, 16);
    put<std::int64_t>(object, offset + 0x10, str.aux);
    put_pointer(object, offset + 0x18, str.ctl);
}

OwnerRow3Binding* g_row3_binding = nullptr;

}  // namespace

static_assert(sizeof(SharedStr) == 0x20);
static_assert(kOwnerCommandNameOffset + 0x20 == kOwnerCommandDescriptionOffset);
static_assert(kOwnerCommandDescriptionOffset + 0x20 == kOwnerCommandBaseSize);
static_assert(kOwnerRow3CommandOffset + kOwnerCommandBaseSize == kOwnerRow3BackPointerOffset);
static_assert(kOwnerRow3BackPointerOffset + 0x08 == kOwnerRow3HandlerOffset);
static_assert(kOwnerRow3HandlerOffset + 0x10 == kOwnerRow3Zero0F8Offset);
static_assert(kOwnerRow3Zero0F8Offset + 0x20 <= kOwnerRow3Byte120Offset);
static_assert(kOwnerRow3Zero128Offset + 0x10 == kOwnerCtor14DB00ObjSize);
static_assert(kOwnerRow3MutexOffset + 0x08 == kOwnerRow3Qword78Offset);
static_assert(kOwnerRow3Qword78Offset + 0x08 == kOwnerRow3CommandOffset);
static_assert(kOwnerRow3AnchorOffset + 0x10 == kOwnerRow3MutexOffset);
static_assert(kOwnerRow3HelperPtrOffset + 0x10 <= kOwnerRow3AnchorOffset);
static_assert(kOwnerRow3HelperCtrlOffset == kOwnerRow3HelperPtrOffset + 0x08);
static_assert(kOwnerRow3Zero30Offset + 0x10 <= kOwnerRow3HelperPtrOffset);

void owner_row3_string_view_f2f0(SharedStr& out, const char* cstr) noexcept {
    // 0xf2f0 stores the pointer, measures it with strlen (PLT 0xf8f3b0), stores the length, then
    // zeroes the trailing 16 B.
    out.data = reinterpret_cast<const std::uint8_t*>(cstr);
    out.size = static_cast<std::int64_t>(std::strlen(cstr));
    out.aux = 0;
    out.ctl = nullptr;
}

void owner_row3_command_ctor_20af0(void* at, const void* base_vptr, const SharedStr& name,
                                   const SharedStr& description, std::uint32_t flags) noexcept {
    auto* const o = static_cast<std::uint8_t*>(at);
    put_pointer(o, kOwnerCommandVptrOffset, base_vptr);
    put<std::uint8_t>(o, kOwnerCommandRegisteredOffset, 0u);
    put<std::uint32_t>(o, kOwnerCommandFlagsOffset, flags);
    copy_holder(o, kOwnerCommandNameOffset, name);
    retain(name);
    copy_holder(o, kOwnerCommandDescriptionOffset, description);
    retain(description);
}

void owner_row3_helper_ctor_1ab270(void* at, const void* vptr) noexcept {
    auto* const o = static_cast<std::uint8_t*>(at);
    put_pointer(o, 0x00, vptr);
    put_bytes(o, 0x10, kZero16, 16);
    put<std::uint64_t>(o, 0x20, 0u);
}

OwnerRow3Result owner_ctor_14db00(void* object, void* engine,
                                  const OwnerRow3Hooks& hooks) noexcept {
    if (object == nullptr) {
        return {false, "null-object"};
    }
    if (reinterpret_cast<std::uintptr_t>(object) % alignof(std::uint64_t) != 0) {
        return {false, "object-align"};
    }
    if (hooks.pool_vptr == nullptr || hooks.command_base_vptr == nullptr ||
        hooks.command_vptr == nullptr || hooks.control_vptr == nullptr ||
        hooks.helper_vptr == nullptr) {
        return {false, "vptr"};
    }

    auto* const o = static_cast<std::uint8_t*>(object);

    // ---- 0x14db3b..0x14db60: vptr, engine, map header, zero blocks. ---------------------------
    put_pointer(o, kOwnerRow3VptrOffset, hooks.pool_vptr);
    put_pointer(o, kOwnerRow3EngineOffset, engine);
    put_pointer(o, kOwnerRow3MapHeaderOffset, o + kOwnerRow3MapHeaderOffset);
    put_pointer(o, kOwnerRow3MapLeftOffset, o + kOwnerRow3MapHeaderOffset);
    put<std::uint64_t>(o, kOwnerRow3MapRootOffset, 0u);
    put_bytes(o, kOwnerRow3Zero30Offset, kZero16, 16);
    put<std::uint8_t>(o, kOwnerRow3Zero30Offset, 0u);  // 0x14db57, redundant with the line above
    put_bytes(o, kOwnerRow3HelperPtrOffset, kZero16, 16);
    put_bytes(o, kOwnerRow3AnchorOffset, kZero16, 16);

    // ---- 0x17070(0x60) and the second-tree anchor (0x14db65..0x14db81). ----------------------
    if (hooks.allocate == nullptr) {
        return {false, "alloc_anchor"};
    }
    void* const anchor = hooks.allocate(kOwnerRow3AnchorSize);
    if (anchor == nullptr) {
        return {false, "alloc_anchor"};
    }
    auto* const a = static_cast<std::uint8_t*>(anchor);
    put_pointer(a, 0x00, anchor);
    put_pointer(a, 0x08, anchor);
    put_pointer(a, 0x10, anchor);
    put<std::uint8_t>(a, 0x18, 1u);
    put<std::uint8_t>(a, 0x19, 1u);
    put_pointer(o, kOwnerRow3AnchorOffset, anchor);

    // ---- 0x3b310(this + 0x70), then +0x78 = 0 (0x14db85, 0x14db9a). ---------------------------
    if (hooks.mutex_init == nullptr || !hooks.mutex_init(o + kOwnerRow3MutexOffset)) {
        return {false, "mutex_init"};
    }
    put<std::uint64_t>(o, kOwnerRow3Qword78Offset, 0u);

    // ---- the two stack holders (0x14db8a..0x14dbd7). ------------------------------------------
    SharedStr name{};
    owner_row3_string_view_f2f0(name, kOwnerRow3CommandName);
    SharedStr description{};
    description.data = reinterpret_cast<const std::uint8_t*>(kOwnerRow3CommandDescription);
    description.size = static_cast<std::int64_t>(kOwnerRow3CommandDescriptionLength);
    description.aux = 0;
    description.ctl = nullptr;

    // ---- 0x20af0(this + 0x80, &name, &description, 2) (0x14dbdc). -----------------------------
    owner_row3_command_ctor_20af0(o + kOwnerRow3CommandOffset, hooks.command_base_vptr, name,
                                  description, kOwnerRow3CommandFlags);

    // ---- the derived tail (0x14dbf7..0x14dc11). -----------------------------------------------
    put_pointer(o, kOwnerRow3CommandOffset + kOwnerCommandVptrOffset, hooks.command_vptr);
    put<std::uint64_t>(o, kOwnerRow3HandlerOffset, kOwnerRow3HandlerRva);
    put<std::uint64_t>(o, kOwnerRow3HandlerOffset + 0x08, 0u);
    put_pointer(o, kOwnerRow3BackPointerOffset, o);
    put_bytes(o, kOwnerRow3Zero0F8Offset, kZero32, 32);

    // ---- 0x20d60(this + 0x80): guard byte, then the console insert (0x14dc19). ----------------
    std::uint8_t* const command = o + kOwnerRow3CommandOffset;
    if (command[kOwnerCommandRegisteredOffset] == 0) {
        command[kOwnerCommandRegisteredOffset] = 1;
        if (hooks.console_register == nullptr || !hooks.console_register(command)) {
            return {false, "console_register"};
        }
    }

    // ---- 0xf590(&description), 0xf590(&name) (0x14dc1f, 0x14dc27). ----------------------------
    TripwireDeallocator deallocator;
    release_shared_str_f590(description, deallocator);
    release_shared_str_f590(name, deallocator);

    // ---- +0x120, +0x128 (0x14dc30, 0x14dc3c). -------------------------------------------------
    put<std::uint8_t>(o, kOwnerRow3Byte120Offset, 0u);
    put_bytes(o, kOwnerRow3Zero128Offset, kZero16, 16);

    // ---- 0x17070(0x28) + 0x1ab270 (0x14dc4b, 0x14dc56). ---------------------------------------
    void* const helper = hooks.allocate(kOwnerRow3HelperSize);
    if (helper == nullptr) {
        return {false, "alloc_helper"};
    }
    owner_row3_helper_ctor_1ab270(helper, hooks.helper_vptr);

    // ---- [[this] + 0x50] == 0x14e910(this, helper), devirtualized (0x14dc44..0x14dc61). -------
    if (hooks.allocate_17090 == nullptr) {
        return {false, "alloc_control"};
    }
    void* const control = hooks.allocate_17090(kOwnerRow3ControlSize);
    if (control == nullptr) {
        return {false, "alloc_control"};
    }
    auto* const c = static_cast<std::uint8_t*>(control);
    put<std::uint32_t>(c, 0x08, 1u);
    put<std::uint32_t>(c, 0x0C, 1u);
    put_pointer(c, 0x00, hooks.control_vptr);
    put_pointer(c, 0x10, helper);
    put_pointer(o, kOwnerRow3HelperPtrOffset, helper);
    // The previous +0x50 is zero (written by the 16 B block at +0x48 above), so the release of the
    // old control block in the original is dead here and is not modelled.
    put_pointer(o, kOwnerRow3HelperCtrlOffset, control);

    return {true, nullptr};
}

void owner_row3_bind(OwnerRow3Binding* binding) noexcept {
    g_row3_binding = binding;
}

bool owner_row3_ctor_hook(std::size_t row, void* object, void* arg) noexcept {
    if (row != kOwnerMemberRow3 || g_row3_binding == nullptr) {
        return false;
    }
    g_row3_binding->last = owner_ctor_14db00(object, arg, g_row3_binding->hooks);
    g_row3_binding->last_engine = arg;
    ++g_row3_binding->calls;
    return g_row3_binding->last.ok;
}

}  // namespace sotc
