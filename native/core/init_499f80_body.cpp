#include "init_499f80_body.hpp"

#include "critsec_3b310.hpp"

#include <cstring>

namespace sotc {
namespace {

bool default_critsec_init(void* obj) noexcept { return critsec_init_at(obj) == 0; }

bool default_critsec_destroy(void* obj) noexcept { return critsec_destroy_at(obj) == 0; }

// Records the stage name and marks the failure. No cleanup: the retail body has no error branch, so any
// release here would be invented host policy (see the header note).
void stop(Init499f80BodyResult& out, Init499f80Stage stage, const char* name) noexcept {
    out.ok = false;
    out.stage = stage;
    out.blocked = name;
}

}  // namespace

Init499f80BodyResult run_init_499f80_body(void* arg, Init499f80Globals& globals,
                                          const Init499f80BodyHooks& hooks) noexcept {
    Init499f80BodyResult out{};
    if (globals.store_cell == nullptr || globals.old_object_cell == nullptr) {
        stop(out, Init499f80Stage::globals_missing, "globals");
        return out;
    }
    if (hooks.alloc.allocate == nullptr || hooks.alloc.deallocate == nullptr ||
        hooks.critsec_init == nullptr || hooks.critsec_destroy == nullptr) {
        stop(out, Init499f80Stage::hooks_missing, "hooks");
        return out;
    }

    // testq %rdi,%rdi ; cmoveq [0x15ff9a8],%rdi ; movq %rdi,(%rax)  -- before any allocation.
    void* effective = arg != nullptr ? arg : globals.default_arg;
    out.arg_used = effective;
    *globals.store_cell = effective;
    out.store_cell_written = true;

    // 0x17070(0x50)
    const AllocWrapperResult object_alloc =
        alloc_17070(kInit499f80ObjectAllocSize, hooks.alloc);
    if (!object_alloc.ok || object_alloc.pointer == nullptr) {
        stop(out, Init499f80Stage::alloc_object_failed, "alloc_object");
        return out;
    }
    out.new_object = object_alloc.pointer;
    out.object_live = true;

    // Host stand-in for the libkernel write: the retail shim performs no store to the caller's 8 bytes,
    // scePthreadMutexInit does. Zeroing keeps a fresh token table independent of allocator contents.
    std::memset(out.new_object, 0, kCritSecHandleBytes);
    // 0x3b310(obj)
    if (!hooks.critsec_init(out.new_object)) {
        stop(out, Init499f80Stage::critsec_init_failed, "critsec_init");
        return out;
    }
    out.critsec_live = true;

    auto* obj = static_cast<std::uint8_t*>(out.new_object);
    // movl $0x0,0x48(%rbx) -- before the first buffer allocation.
    *reinterpret_cast<std::uint32_t*>(obj + kInit499f80ObjectFlagsOff) = 0;

    // 0x17090(0xa00) -- buffer A.
    const AllocWrapperResult buf_a = alloc_17090(kInit499f80BufferChunkSize, hooks.alloc);
    if (!buf_a.ok || buf_a.pointer == nullptr) {
        stop(out, Init499f80Stage::alloc_buffer_a_failed, "alloc_buf_a");
        return out;
    }
    out.buffer_a = buf_a.pointer;
    *reinterpret_cast<void**>(obj + kInit499f80BufferAOff + 0x00) = buf_a.pointer;
    *reinterpret_cast<void**>(obj + kInit499f80BufferAOff + 0x08) = buf_a.pointer;
    *reinterpret_cast<void**>(obj + kInit499f80BufferAOff + 0x10) =
        static_cast<std::uint8_t*>(buf_a.pointer) + kInit499f80BufferChunkSize;

    // 0x17090(0xa00) -- buffer B.
    const AllocWrapperResult buf_b = alloc_17090(kInit499f80BufferChunkSize, hooks.alloc);
    if (!buf_b.ok || buf_b.pointer == nullptr) {
        stop(out, Init499f80Stage::alloc_buffer_b_failed, "alloc_buf_b");
        return out;
    }
    out.buffer_b = buf_b.pointer;
    *reinterpret_cast<void**>(obj + kInit499f80BufferBOff + 0x00) = buf_b.pointer;
    *reinterpret_cast<void**>(obj + kInit499f80BufferBOff + 0x08) = buf_b.pointer;
    *reinterpret_cast<void**>(obj + kInit499f80BufferBOff + 0x10) =
        static_cast<std::uint8_t*>(buf_b.pointer) + kInit499f80BufferChunkSize;

    // Replacement of the previous object: buffer B (tag 2), buffer A (tag 2), CritSec destroy, object free
    // (tag 1), then the cell write. `previous == new` returns without a store, exactly like 0x49a000.
    void* previous = *globals.old_object_cell;
    out.previous_object = previous;
    if (previous == out.new_object) {
        out.ok = true;
        return out;
    }
    if (previous != nullptr) {
        auto* old = static_cast<std::uint8_t*>(previous);
        if (void* p = *reinterpret_cast<void**>(old + kInit499f80BufferBOff)) {
            (void)free_170c0(p, hooks.alloc);
        }
        if (void* p = *reinterpret_cast<void**>(old + kInit499f80BufferAOff)) {
            (void)free_170c0(p, hooks.alloc);
        }
        (void)hooks.critsec_destroy(old);
        (void)free_170b0(old, hooks.alloc);
    }
    *globals.old_object_cell = out.new_object;

    out.ok = true;
    out.stage = Init499f80Stage::none;
    out.blocked = nullptr;
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
