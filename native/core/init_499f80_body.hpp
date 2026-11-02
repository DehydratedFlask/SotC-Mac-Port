// CPU-01-499F80-BODY-NATIVE-01: native lift of app-main helper 0x499f80 (193 B FDE, E1031).
// CPU-01-499F80-HOST-FAILURE-ROLLBACK-01: host-only rollback and cleanup-result contract (E1055).
// Offline analysis only; no original RVA is executed and no game state is touched.
//
// ============================== OBSERVED (pinned body, 193 B) ==============================
// Straight-line body, four internal conditional branches, **no error/status branch after any call**
// (E1046). In address order:
//
//   0x499f8a  testq %rdi,%rdi ; 0x499f94 cmoveq [0x15ff9a8],%rdi   ; arg==0 -> global default
//   0x499f9c  movq %rdi,(%rax)                                      ; [0x1604988] = effective arg
//   0x499fa4  call 0x17070(0x50)          ; one object, tag-1/align-8/flags-1 wrapper
//   0x499faf  call 0x3b310(obj)           ; CritSec init at obj+0 (E1032: 8-byte handle shim)
//   0x499fb9  movl $0x0,0x48(%rbx)        ; obj+0x48 = 0, BEFORE the first buffer allocation
//   0x499fc0  call 0x17090(0xa00)         ; buffer A (flags 2)
//   0x499fc5  obj+0x08 = begin ; 0x499fc9 obj+0x10 = begin ; 0x499fd8 obj+0x18 = begin+0xa00
//   0x499fdc  call 0x17090(0xa00)         ; buffer B (flags 2)
//   0x499fe8  obj+0x28 = begin ; 0x499fec obj+0x30 = begin ; 0x499ff6 obj+0x38 = begin+0xa00
//   0x499ffa  r14 = [0x2ef9fc8]           ; the previous object in that global cell
//   0x49a000  previous == new  -> return without storing (the cell already holds this pointer)
//   0x49a005  previous == null -> store the new pointer
//   0x49a010  call 0x170c0(previous+0x28) ; tag-2 release of buffer B, guarded by a null test
//   0x49a01e  call 0x170c0(previous+0x08) ; tag-2 release of buffer A, guarded by a null test
//   0x49a026  call 0x3b380(previous)      ; CritSec destroy on the old object
//   0x49a02e  call 0x170b0(previous)      ; tag-1 free of the old object
//   0x49a033  [0x2ef9fc8] = new
//   0x49a040  retq
//
// Eight relative calls to six targets: 0x17070, 0x3b310, 0x17090 x2, 0x170c0 x2, 0x3b380, 0x170b0.
// The tail `addq $0x8,%rsp; pop x3; pop rbp; retq` is a plain epilogue: the body never returns a status.
//
// ============================== HOST FAILURE POLICY (E1055 owner decision) ==============================
// The retail body has no failure branch, so it has **no** error semantics to recover (E1046). Everything
// in this section is a **host safety policy** selected by the owner ("host rollback with cleanup-result
// reporting", docs/checkpoints/2026-10-31-499f80-host-rollback-handoff-e1055.md). It is not recovered
// retail behaviour and must never be cited as such. The success path keeps the pinned byte ordering.
//
// 1. Candidate. The new object is built as a candidate. The store at 0x499f9c is made *speculative*: the
//    prior `store_cell` value is captured (`store_cell_prior`) and restored whenever the call does not
//    commit. A commit is the final `[old_object_cell] = new` store (0x49a033); `old_object_cell` is never
//    written before it.
// 2. Rollback order. Candidate resources are released in reverse acquisition order: buffer B (tag 2),
//    buffer A (tag 2), CritSec destroy (0x3b380), object free (tag 1). Every attempt is captured in
//    `cleanup[]`; a rejected release stays owned and reported.
// 3. Handle safety. The object is never freed while its CritSec destroy is outstanding or was rejected
//    (including EBUSY): the live handle token lives in the object's first 8 bytes. Successfully released
//    buffer pointers are cleared in the object (+0x08/+0x10/+0x18 and +0x28/+0x30/+0x38) and in the
//    result, and every released resource is recorded, so `rollback_init_499f80_candidate()` can be
//    retried without a double free or a second destroy.
// 4. Previous object. Replacement releases are attempted in the pinned retail order (buffer B, buffer A,
//    CritSec, object). If any is rejected, the previous object stays reachable (`old_object_cell` is not
//    written), the pointer triples of resources actually released are cleared so a later call cannot
//    free them twice, and the candidate is rolled back under the same contract. A previous handle that is
//    already empty is skipped, so a retry never destroys the old CritSec twice.
// 5. Hooks. `critsec_init` must be failure-atomic: register the handle first and leave nothing
//    registered or initialized when it returns non-zero. The model cannot verify an arbitrary host hook,
//    so it refuses to run unless `Init499f80BodyHooks::critsec_init_failure_atomic` asserts it. The
//    default hooks satisfy this (`critsec_init_at` is failure-atomic, E1055); replacing the hook
//    requires asserting it explicitly.
//
// ============================== NOT MODELLED ==============================
// The allocator dispatchers 0x12820 / 0x12af0 beyond the AllocDispatchContract hooks; the consumer of
// [0x1604988]; and the next call at 0x531df4 (0x56dcb0), which this task must not continue.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit499f80BodyRva = 0x499f80;
inline constexpr std::size_t kInit499f80BodySize = 193;
inline constexpr std::size_t kInit499f80BodyFdeLength = 193;
inline constexpr std::uint64_t kInit499f80GlobalDefaultArgRva = 0x15ff9a8;
inline constexpr std::uint64_t kInit499f80GlobalStoreRva = 0x1604988;
inline constexpr std::uint64_t kInit499f80GlobalOldObjectRva = 0x2ef9fc8;
inline constexpr std::size_t kInit499f80ObjectAllocSize = 0x50;
inline constexpr std::size_t kInit499f80BufferChunkSize = 0xa00;
inline constexpr std::size_t kInit499f80ObjectFlagsOff = 0x48;      // u32 store
inline constexpr std::size_t kInit499f80BufferAOff = 0x08;          // begin / cur / end at +0x08/+0x10/+0x18
inline constexpr std::size_t kInit499f80BufferBOff = 0x28;          // begin / cur / end at +0x28/+0x30/+0x38

// Globals are offset-only host cells. They are caller-owned stand-ins for the pinned .bss/.data cells and
// carry no recovered meaning: the body only reads the default argument, writes the store cell and
// swaps the previous-object cell.
struct Init499f80Globals {
    void* default_arg = nullptr;     // models 0x15ff9a8
    void** store_cell = nullptr;       // models 0x1604988 (qword store)
    void** old_object_cell = nullptr;  // models 0x2ef9fc8
};

// Host-only cleanup bookkeeping (no retail counterpart; see the policy note above). One record exists per
// resource this call could own, so a host can see exactly what was released and what was retained.
enum class Init499f80Resource : std::uint8_t {
    candidate_object = 0,   // 0x17070(0x50)        / tag-1 free 0x170b0
    candidate_critsec,      // 0x3b310(obj)         / 0x3b380(obj)
    candidate_buffer_a,     // first 0x17090(0xa00) / tag-2 free 0x170c0
    candidate_buffer_b,     // second 0x17090(0xa00)/ tag-2 free 0x170c0
    previous_buffer_b,      // 0x170c0(previous+0x28) at 0x49a010
    previous_buffer_a,      // 0x170c0(previous+0x08) at 0x49a01e
    previous_critsec,       // 0x3b380(previous)      at 0x49a026
    previous_object,        // 0x170b0(previous)      at 0x49a02e
    count,
};
inline constexpr std::size_t kInit499f80CleanupCount =
    static_cast<std::size_t>(Init499f80Resource::count);

// `not_owned`: this call never had to release it (it was never acquired, or the call committed and the
// caller owns it). `released`: released successfully. `failed`: a release was attempted and rejected, so
// the resource is still owned. `held`: not attempted because a release it depends on failed; still owned.
enum class Init499f80Cleanup : std::uint8_t {
    not_owned = 0,
    released,
    failed,
    held,
};

struct Init499f80CleanupRecord {
    Init499f80Cleanup status = Init499f80Cleanup::not_owned;
    std::uint32_t tag = 0;    // allocator tag of an attempted free (1 = object, 2 = buffer); 0 = no free
    int error = 0;            // errno-style code from a rejected CritSec hook; 0 when the hook has none
    void* pointer = nullptr;  // address the record refers to (the object itself for CritSec records)
};

struct Init499f80BodyHooks {
    AllocDispatchContract alloc{};
    // 0x3b310 / 0x3b380 equivalents. Both return 0 on success and an errno-style code on failure; the
    // retail shims discard every return code, so these codes are host detection only.
    int (*critsec_init)(void* object_base) noexcept = nullptr;     // 0x3b310
    int (*critsec_destroy)(void* object_base) noexcept = nullptr;  // 0x3b380
    // Host assertion, not a retail fact: `critsec_init` leaves no registered handle, sidecar or
    // initialized mutex behind when it returns non-zero, so the rollback may free the object without a
    // matching `critsec_destroy`. The default hooks satisfy this (critsec_init_at is failure-atomic,
    // E1055); a replaced hook must set it explicitly. `run_init_499f80_body` refuses to run otherwise.
    bool critsec_init_failure_atomic = false;
};

// Host-only classification of where the call stopped. `none` means the body ran to its epilogue.
enum class Init499f80Stage : std::uint8_t {
    none = 0,
    globals_missing,              // host contract: a caller cell is null (no retail counterpart)
    hooks_missing,                // host contract: an alloc/CritSec hook is null
    hooks_not_failure_atomic,     // host contract: critsec_init is not asserted failure-atomic
    alloc_object_failed,          // 0x17070(0x50) returned null
    critsec_init_failed,          // 0x3b310 returned non-zero (retail discards this code)
    alloc_buffer_a_failed,        // first 0x17090(0xa00) returned null
    alloc_buffer_b_failed,        // second 0x17090(0xa00) returned null
    previous_cleanup_failed,      // host policy: replacement could not release the previous object
};

struct Init499f80BodyResult {
    bool ok = false;
    Init499f80Stage stage = Init499f80Stage::none;
    const char* blocked = nullptr;   // same stage as a string, kept for existing callers/tests
    void* arg_used = nullptr;        // the effective argument of the store at 0x499f9c
    void* new_object = nullptr;      // candidate identity; owned only while object_live
    void* previous_object = nullptr; // value read from the old-object cell (may be null)
    int critsec_init_error = 0;      // errno-style code from a rejected critsec_init hook

    // Retail-visible cell state at return. The store write is speculative under the host policy.
    bool store_cell_written = false;    // the 0x499f9c store executed
    bool store_cell_restored = false;   // host rollback restored the prior value
    void* store_cell_prior = nullptr;   // value read before the speculative store

    // Candidate resources still owned when the call returns. A successful release clears the field, so a
    // caller retry cannot free or destroy the same resource twice.
    bool object_live = false;
    bool critsec_live = false;
    void* buffer_a = nullptr;
    void* buffer_b = nullptr;

    // Host-only commitment and cleanup report.
    bool committed = false;         // [old_object_cell] now holds the candidate
    bool rolled_back = false;       // no candidate resource remains owned
    bool cleanup_complete = true;   // no record is `failed` or `held`
    std::size_t cleanup_failures = 0;
    Init499f80CleanupRecord cleanup[kInit499f80CleanupCount];

    [[nodiscard]] const Init499f80CleanupRecord& record(Init499f80Resource resource) const noexcept {
        return cleanup[static_cast<std::size_t>(resource)];
    }
};

// Runs the retail body once. Caller owns `new_object` when `committed`; a non-committed call has already
// rolled back everything it could and reports anything it could not. Ordering of observable effects on
// the success path equals the original; every rollback is host policy (retail has no such branch).
[[nodiscard]] Init499f80BodyResult run_init_499f80_body(void* arg, Init499f80Globals& globals,
                                                        const Init499f80BodyHooks& hooks) noexcept;

// Host-only retry for a candidate rollback that reported `failed`/`held` (for example an EBUSY CritSec
// destroy or a rejected allocator free). Releases only what is still owned, in the same order, and
// updates `result`; calling it again after a complete rollback performs no allocator or CritSec call.
// A committed result is never rolled back: it belongs to the caller. Returns `result.rolled_back`.
[[nodiscard]] bool rollback_init_499f80_candidate(Init499f80BodyResult& result,
                                                  const Init499f80BodyHooks& hooks) noexcept;

[[nodiscard]] Init499f80BodyHooks default_init_499f80_body_hooks(
    const AllocDispatchContract& alloc) noexcept;

}  // namespace sotc
