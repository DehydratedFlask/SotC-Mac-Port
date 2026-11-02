// CPU-01-499F80-BODY-NATIVE-01: native lift of app-main helper 0x499f80 (193 B FDE, E1031).
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
// ============================== HOST FAILURE POLICY (deliberately absent) ==============================
// The retail body has no failure branch, so no retail error semantics exist to recover (E1046). This model
// therefore performs **no rollback**: when a host allocation or CritSec call fails, the call returns with
// `ok == false`, a named `stage`, and the still-live resources reported (`object_live`, `critsec_live`,
// `buffer_a`, `buffer_b`). Deciding what to release is `CPU-01-499F80-HOST-FAILURE-ROLLBACK-01`, which the
// live task board still lists as WAITING_FOR_DEPENDENCY; nothing here invents that policy.
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

struct Init499f80BodyHooks {
    AllocDispatchContract alloc{};
    bool (*critsec_init)(void* object_base) noexcept = nullptr;    // 0x3b310 (default: critsec_init_at == 0)
    bool (*critsec_destroy)(void* object_base) noexcept = nullptr;   // 0x3b380
};

// Host-only classification of where the call stopped. `none` means the body ran to its epilogue.
enum class Init499f80Stage : std::uint8_t {
    none = 0,
    globals_missing,           // host contract: a caller cell is null (no retail counterpart)
    hooks_missing,             // host contract: an alloc/CritSec hook is null
    alloc_object_failed,       // 0x17070(0x50) returned null
    critsec_init_failed,       // 0x3b310 returned non-zero (retail discards this code)
    alloc_buffer_a_failed,     // first 0x17090(0xa00) returned null
    alloc_buffer_b_failed,     // second 0x17090(0xa00) returned null
};

struct Init499f80BodyResult {
    bool ok = false;
    Init499f80Stage stage = Init499f80Stage::none;
    const char* blocked = nullptr;   // same stage as a string, kept for existing callers/tests
    void* arg_used = nullptr;        // the effective argument actually stored in the store cell
    void* new_object = nullptr;      // object returned by 0x17070; caller-owned when ok
    void* previous_object = nullptr; // value read from the old-object cell (may be null)

    // Host observation of state that stays live when the call returns early. Nothing in this module
    // releases it; these fields exist so a host policy can decide (see the header note).
    bool store_cell_written = false;
    bool object_live = false;
    bool critsec_live = false;
    void* buffer_a = nullptr;
    void* buffer_b = nullptr;
};

// Runs the retail body once. Caller owns `new_object` when ok; prior-object release uses the hooks.
// Ordering of observable effects equals the original; the only deviations are the host-side failure
// checks above (retail has none).
[[nodiscard]] Init499f80BodyResult run_init_499f80_body(void* arg, Init499f80Globals& globals,
                                                        const Init499f80BodyHooks& hooks) noexcept;

[[nodiscard]] Init499f80BodyHooks default_init_499f80_body_hooks(
    const AllocDispatchContract& alloc) noexcept;

}  // namespace sotc
