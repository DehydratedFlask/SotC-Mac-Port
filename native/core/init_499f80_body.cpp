#include "init_499f80_body.hpp"

#include "critsec_3b310.hpp"

#include <cstring>

namespace sotc {
namespace {

bool default_critsec_init(void* obj) noexcept { return critsec_init_at(obj) == 0; }

bool default_critsec_destroy(void* obj) noexcept { return critsec_destroy_at(obj) == 0; }

}  // namespace

Init499f80BodyResult run_init_499f80_body(void* arg, Init499f80Globals& globals,
                                          const Init499f80BodyHooks& hooks) noexcept {
    Init499f80BodyResult out{};
    if (globals.store_cell == nullptr || globals.old_object_cell == nullptr) {
        out.blocked = "globals";
        return out;
    }
    if (hooks.alloc.allocate == nullptr || hooks.alloc.deallocate == nullptr ||
        hooks.critsec_init == nullptr || hooks.critsec_destroy == nullptr) {
        out.blocked = "hooks";
        return out;
    }

    void* effective = arg != nullptr ? arg : globals.default_arg;
    out.arg_used = effective;
    *globals.store_cell = effective;

    const AllocWrapperResult object_alloc =
        alloc_17070(kInit499f80ObjectAllocSize, hooks.alloc);
    if (!object_alloc.ok || object_alloc.pointer == nullptr) {
        out.blocked = "alloc_object";
        return out;
    }
    out.new_object = object_alloc.pointer;

    std::memset(out.new_object, 0, kCritSecHandleBytes);
    if (!hooks.critsec_init(out.new_object)) {
        out.blocked = "critsec_init";
        return out;
    }

    auto* obj = static_cast<std::uint8_t*>(out.new_object);
    *reinterpret_cast<std::uint32_t*>(obj + 0x48) = 0;

    const AllocWrapperResult buf_a = alloc_17090(kInit499f80BufferChunkSize, hooks.alloc);
    if (!buf_a.ok || buf_a.pointer == nullptr) {
        out.blocked = "alloc_buf_a";
        return out;
    }
    *reinterpret_cast<void**>(obj + 0x08) = buf_a.pointer;
    *reinterpret_cast<void**>(obj + 0x10) = buf_a.pointer;

    void* end_a = static_cast<std::uint8_t*>(buf_a.pointer) + kInit499f80BufferChunkSize;
    *reinterpret_cast<void**>(obj + 0x18) = end_a;

    const AllocWrapperResult buf_b = alloc_17090(kInit499f80BufferChunkSize, hooks.alloc);
    if (!buf_b.ok || buf_b.pointer == nullptr) {
        out.blocked = "alloc_buf_b";
        return out;
    }
    *reinterpret_cast<void**>(obj + 0x28) = buf_b.pointer;
    *reinterpret_cast<void**>(obj + 0x30) = buf_b.pointer;
    void* end_b = static_cast<std::uint8_t*>(buf_b.pointer) + kInit499f80BufferChunkSize;
    *reinterpret_cast<void**>(obj + 0x38) = end_b;

    void* previous = *globals.old_object_cell;
    out.previous_object = previous;
    if (previous != nullptr && previous != out.new_object) {
        auto* old = static_cast<std::uint8_t*>(previous);
        if (void* p = *reinterpret_cast<void**>(old + 0x28)) {
            (void)free_170c0(p, hooks.alloc);
        }
        if (void* p = *reinterpret_cast<void**>(old + 0x08)) {
            (void)free_170c0(p, hooks.alloc);
        }
        (void)hooks.critsec_destroy(old);
        (void)free_170b0(old, hooks.alloc);
    }
    *globals.old_object_cell = out.new_object;

    out.ok = true;
    return out;
}

Init499f80BodyHooks default_init_499f80_body_hooks(const AllocDispatchContract& alloc) noexcept {
    Init499f80BodyHooks hooks{};
    hooks.alloc = alloc;
    hooks.critsec_init = default_critsec_init;
    hooks.critsec_destroy = default_critsec_destroy;
    return hooks;
}

}  // namespace sotc
