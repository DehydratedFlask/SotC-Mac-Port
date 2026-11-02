#include "init_499f80_body.hpp"

#include "critsec_3b310.hpp"

#include <cstring>

namespace sotc {
namespace {

int default_critsec_init(void* obj) noexcept { return critsec_init_at(obj); }

int default_critsec_destroy(void* obj) noexcept { return critsec_destroy_at(obj); }

Init499f80CleanupRecord& record(Init499f80BodyResult& out,
                                Init499f80Resource resource) noexcept {
    return out.cleanup[static_cast<std::size_t>(resource)];
}

void stop(Init499f80BodyResult& out, Init499f80Stage stage, const char* name) noexcept {
    out.ok = false;
    out.stage = stage;
    out.blocked = name;
}

// Host policy 1: the 0x499f9c store is speculative, so a call that does not commit restores the value the
// caller cell held before the store. There is no failure mode: the cell is a plain caller-owned location.
void restore_speculative_store(Init499f80BodyResult& out,
                               Init499f80Globals& globals) noexcept {
    if (!out.store_cell_written || globals.store_cell == nullptr) return;
    *globals.store_cell = out.store_cell_prior;
    out.store_cell_restored = true;
}

// Host policy 2/3 aggregation: what is owned, what failed to release, and whether anything is retained.
void summarize(Init499f80BodyResult& out) noexcept {
    out.cleanup_failures = 0;
    out.cleanup_complete = true;
    for (const Init499f80CleanupRecord& entry : out.cleanup) {
        if (entry.status == Init499f80Cleanup::failed) {
            ++out.cleanup_failures;
            out.cleanup_complete = false;
        } else if (entry.status == Init499f80Cleanup::held) {
            out.cleanup_complete = false;
        }
    }
    out.rolled_back = !out.object_live && !out.critsec_live &&
                      out.buffer_a == nullptr && out.buffer_b == nullptr;
}

// Host safety check for a previous object whose CritSec destroy already succeeded in an earlier call: the
// native destroy zeroes the 8-byte handle, and an empty handle must not be destroyed twice.
bool handle_is_empty(const void* handle_bytes) noexcept {
    std::uint64_t token = 0;
    std::memcpy(&token, handle_bytes, sizeof(token));
    return token == 0;
}

// begin / cur / end are contiguous; clearing them keeps a retained object from handing a freed buffer back
// to a retry.
void clear_pointer_triple(std::uint8_t* object, std::size_t begin_off) noexcept {
    *reinterpret_cast<void**>(object + begin_off + 0x00) = nullptr;
    *reinterpret_cast<void**>(object + begin_off + 0x08) = nullptr;
    *reinterpret_cast<void**>(object + begin_off + 0x10) = nullptr;
}

// Host-only release of one candidate buffer through the pinned tag-2 wrapper (0x170c0). A rejected free
// leaves the buffer owned and reported; a successful one clears both the result field and the object's
// begin/cur/end triple.
void release_candidate_buffer(Init499f80BodyResult& out, const Init499f80BodyHooks& hooks,
                              Init499f80Resource resource, std::size_t begin_off,
                              void*& live, std::uint8_t* object) noexcept {
    Init499f80CleanupRecord& entry = record(out, resource);
    entry.pointer = live;
    entry.tag = 2;
    const AllocWrapperResult freed = free_170c0(live, hooks.alloc);
    if (!freed.ok) {
        entry.status = Init499f80Cleanup::failed;
        return;
    }
    entry.status = Init499f80Cleanup::released;
    live = nullptr;
    clear_pointer_triple(object, begin_off);
}

// Host policy 2/3: reverse acquisition order, object last, and never free the handle storage while its
// CritSec destroy is outstanding or rejected (the live token sits in the object's first 8 bytes).
void rollback_candidate(Init499f80BodyResult& out, const Init499f80BodyHooks& hooks) noexcept {
    if (!out.object_live || out.new_object == nullptr) return;
    auto* object = static_cast<std::uint8_t*>(out.new_object);

    if (out.buffer_b != nullptr) {
        release_candidate_buffer(out, hooks, Init499f80Resource::candidate_buffer_b,
                                 kInit499f80BufferBOff, out.buffer_b, object);
    }
    if (out.buffer_a != nullptr) {
        release_candidate_buffer(out, hooks, Init499f80Resource::candidate_buffer_a,
                                 kInit499f80BufferAOff, out.buffer_a, object);
    }

    if (out.critsec_live) {
        Init499f80CleanupRecord& entry = record(out, Init499f80Resource::candidate_critsec);
        entry.pointer = object;
        const int rc = hooks.critsec_destroy(object);
        if (rc == 0) {
            entry.status = Init499f80Cleanup::released;
            entry.error = 0;
            out.critsec_live = false;
        } else {
            entry.status = Init499f80Cleanup::failed;
            entry.error = rc;
        }
    }

    Init499f80CleanupRecord& object_entry = record(out, Init499f80Resource::candidate_object);
    object_entry.pointer = out.new_object;
    if (out.buffer_a != nullptr || out.buffer_b != nullptr || out.critsec_live) {
        object_entry.status = Init499f80Cleanup::held;
        return;
    }
    object_entry.tag = 1;
    const AllocWrapperResult freed = free_170b0(out.new_object, hooks.alloc);
    if (!freed.ok) {
        object_entry.status = Init499f80Cleanup::failed;
        return;
    }
    object_entry.status = Init499f80Cleanup::released;
    out.object_live = false;
}

// Host policy 4: one previous-object buffer in the pinned retail order (0x170c0, tag 2).
bool release_previous_buffer(Init499f80BodyResult& out, const Init499f80BodyHooks& hooks,
                             Init499f80Resource resource, std::uint8_t* previous,
                             std::size_t begin_off) noexcept {
    Init499f80CleanupRecord& entry = record(out, resource);
    void* pointer = *reinterpret_cast<void**>(previous + begin_off);
    entry.pointer = pointer;
    if (pointer == nullptr) return true;   // 0x49a00e / 0x49a01c: the pinned null guard
    entry.tag = 2;
    const AllocWrapperResult freed = free_170c0(pointer, hooks.alloc);
    if (!freed.ok) {
        entry.status = Init499f80Cleanup::failed;
        return false;
    }
    entry.status = Init499f80Cleanup::released;
    return true;
}

// Host policy 4: releases the previous object in the pinned order and returns true only when the
// replacement may commit. Any rejection keeps the old object reachable and clears the triples of buffers
// that were actually released, so a later call cannot free them twice.
bool cleanup_previous(Init499f80BodyResult& out, const Init499f80BodyHooks& hooks,
                      void* previous) noexcept {
    auto* old = static_cast<std::uint8_t*>(previous);

    const bool buffer_b_ok = release_previous_buffer(
        out, hooks, Init499f80Resource::previous_buffer_b, old, kInit499f80BufferBOff);
    const bool buffer_a_ok = release_previous_buffer(
        out, hooks, Init499f80Resource::previous_buffer_a, old, kInit499f80BufferAOff);

    bool critsec_ok = true;
    Init499f80CleanupRecord& critsec_entry = record(out, Init499f80Resource::previous_critsec);
    critsec_entry.pointer = previous;
    if (!handle_is_empty(old)) {
        const int rc = hooks.critsec_destroy(old);
        if (rc == 0) {
            critsec_entry.status = Init499f80Cleanup::released;
            critsec_entry.error = 0;
        } else {
            critsec_entry.status = Init499f80Cleanup::failed;
            critsec_entry.error = rc;
            critsec_ok = false;
        }
    }

    Init499f80CleanupRecord& object_entry = record(out, Init499f80Resource::previous_object);
    object_entry.pointer = previous;

    bool object_released = false;
    if (!buffer_b_ok || !buffer_a_ok || !critsec_ok) {
        object_entry.status = Init499f80Cleanup::held;
    } else {
        object_entry.tag = 1;
        const AllocWrapperResult freed = free_170b0(previous, hooks.alloc);
        object_released = freed.ok;
        object_entry.status = freed.ok ? Init499f80Cleanup::released : Init499f80Cleanup::failed;
    }

    if (!object_released) {
        if (record(out, Init499f80Resource::previous_buffer_b).status ==
            Init499f80Cleanup::released) {
            clear_pointer_triple(old, kInit499f80BufferBOff);
        }
        if (record(out, Init499f80Resource::previous_buffer_a).status ==
            Init499f80Cleanup::released) {
            clear_pointer_triple(old, kInit499f80BufferAOff);
        }
        return false;
    }
    return true;
}

// Every failure path of the candidate construction and of the replacement falls through here.
void fail_and_roll_back(Init499f80BodyResult& out, Init499f80Globals& globals,
                        const Init499f80BodyHooks& hooks) noexcept {
    rollback_candidate(out, hooks);
    restore_speculative_store(out, globals);
    summarize(out);
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
    if (!hooks.critsec_init_failure_atomic) {
        // Host policy 5: the rollback frees the object without a matching destroy after a rejected init,
        // which is only valid for a hook that asserts failure atomicity.
        stop(out, Init499f80Stage::hooks_not_failure_atomic, "hooks_atomic");
        return out;
    }

    // testq %rdi,%rdi ; cmoveq [0x15ff9a8],%rdi ; movq %rdi,(%rax)  -- before any allocation.
    void* effective = arg != nullptr ? arg : globals.default_arg;
    out.arg_used = effective;
    out.store_cell_prior = *globals.store_cell;
    *globals.store_cell = effective;
    out.store_cell_written = true;

    // 0x17070(0x50)
    const AllocWrapperResult object_alloc =
        alloc_17070(kInit499f80ObjectAllocSize, hooks.alloc);
    if (!object_alloc.ok || object_alloc.pointer == nullptr) {
        // Host policy 1: nothing was created, so the speculative store is rolled back.
        stop(out, Init499f80Stage::alloc_object_failed, "alloc_object");
        fail_and_roll_back(out, globals, hooks);
        return out;
    }
    out.new_object = object_alloc.pointer;
    out.object_live = true;

    // Host stand-in for the libkernel write: the retail shim performs no store to the caller's 8 bytes,
    // scePthreadMutexInit does. Zeroing keeps a fresh token table independent of allocator contents.
    std::memset(out.new_object, 0, kCritSecHandleBytes);
    // 0x3b310(obj)
    const int init_rc = hooks.critsec_init(out.new_object);
    if (init_rc != 0) {
        // The hook contract guarantees nothing was registered, so the candidate object is freed without a
        // destroy and no CritSec record is created.
        stop(out, Init499f80Stage::critsec_init_failed, "critsec_init");
        out.critsec_init_error = init_rc;
        fail_and_roll_back(out, globals, hooks);
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
        fail_and_roll_back(out, globals, hooks);
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
        fail_and_roll_back(out, globals, hooks);
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
        out.committed = true;   // the cell already holds this pointer (0x49a000)
        summarize(out);
        return out;
    }
    if (previous != nullptr && !cleanup_previous(out, hooks, previous)) {
        // Host policy 4: the previous object stays reachable, so the replacement does not commit. The
        // candidate cannot be installed and is rolled back under the same contract.
        stop(out, Init499f80Stage::previous_cleanup_failed, "previous_cleanup");
        fail_and_roll_back(out, globals, hooks);
        return out;
    }
    *globals.old_object_cell = out.new_object;

    out.ok = true;
    out.stage = Init499f80Stage::none;
    out.blocked = nullptr;
    out.committed = true;
    summarize(out);
    return out;
}

bool rollback_init_499f80_candidate(Init499f80BodyResult& result,
                                    const Init499f80BodyHooks& hooks) noexcept {
    if (result.committed) return result.rolled_back;   // a committed candidate belongs to the caller
    if (hooks.alloc.deallocate == nullptr || hooks.critsec_destroy == nullptr) {
        // Nothing may be released with an incomplete hook set; keep the report as it stands.
        return result.rolled_back;
    }
    rollback_candidate(result, hooks);
    summarize(result);
    return result.rolled_back;
}

Init499f80BodyHooks default_init_499f80_body_hooks(const AllocDispatchContract& alloc) noexcept {
    Init499f80BodyHooks hooks{};
    hooks.alloc = alloc;
    hooks.critsec_init = default_critsec_init;
    hooks.critsec_destroy = default_critsec_destroy;
    hooks.critsec_init_failure_atomic = true;   // critsec_init_at registers only after success (E1055)
    return hooks;
}

}  // namespace sotc
