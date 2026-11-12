// Resolved slot `vtable 0x15953f8 + 0x18 -> 0x31310` (0x69 B): the Fios read object's teardown.
//
// CORRECTION (E1095): the table row for the callers `0x312f0` and `0x31360` was withdrawn. Those
// calls reload an object vptr stored as `lea 0x15953f8(%rip); add $0x10`, so `call *0x18(%rax)`
// reads `0x15953f8 + 0x10 + 0x18 = 0x1595420` (-> `0x31520`), not this slot.
//
// CORRECTION (E1100, hook names): after this body installs `0x1595408` as the vptr, the three
// indirect calls and the direct call at `0x31327` reach bodies that are now modelled from ELF bytes:
//   direct `call 0x31900` and `vptr+0xa8` (`0x15954b0`) -> `0x31900`, the Fios-op poll
//        (`frontier_31900_fios_poll`); returns `[this+0x54] == 0`, true when no op is outstanding;
//   `vptr+0xa0` (`0x15954a8`) -> `0x318f0`, `[this+0x5c] == 0` (`frontier_318f0_fios_succeeded`);
//   `vptr+0x18` (`0x1595420`) -> `0x31520`, wait-for-completion then `sceFiosFHCloseSync`
//        (`frontier_31520_fios_close`).
// The older names `sub_init` / `positioned_read` / `plain_read` / `mode_one` came from the withdrawn
// `0x318b0` / `0x31870` tie and described Fios reads that are never called here; they are replaced.
//
// Body:
//   *(u64*)this = 0x15953f8 + 0x10 = 0x1595408      ; install the base vtable
//   settled = 0x31900(this)                         ; direct call: poll the outstanding Fios op
//   while (!settled) {
//       sceFiosOpWait(*(u32*)(this+0x50));          ; PLT 0xf8f7d0, libSceFios2
//       settled = u8(this->vtable[0xa8](this));     ; the same poll again, through the vptr (0x31900)
//   }
//   this->vtable[0xa0](this);                       ; 0x318f0, "op succeeded" (result discarded)
//   if (*(u32*)(this+0x8) == 1) this->vtable[0x18](this);  ; 0x31520, wait + close the file handle
//   0x4a4a0(this);                                  ; 1-byte `retq` - a no-op call, kept as a hook
//   jmp 0x170b0(this);                              ; tag-1 free of `this`
//
// The wait import is named by the PLT map (`sceFiosOpWait`), so the loop is an async-completion wait
// rather than a retry with unknown semantics. `0x4a4a0` is a 1-byte `retq`. The poll, success and
// close steps stay hooks here so a caller can bind either the matching `frontier_*` model or a host
// service; a bound wait that never completes is refused instead of spun.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15953f8Slot18_31310Rva = 0x31310;
inline constexpr std::size_t kVtable15953f8Slot18_31310Size = 0x69;
inline constexpr std::uint64_t kVtable15953f8Slot18_31310EndAddress = 0x31379;
inline constexpr std::uint32_t kVtable15953f8Slot18 = 0x18;
inline constexpr std::uint64_t kVtable15953f8Slot18Reloc = 0x1595410;  // 0x15953f8 + 0x18
inline constexpr std::uint64_t kVtable15953f8Slot18InstalledTable = 0x1595408;
inline constexpr std::uint64_t kVtable15953f8Slot18WaitPlt = 0xf8f7d0;   // sceFiosOpWait
inline constexpr std::uint64_t kVtable15953f8Slot18PollRva = 0x31900;         // direct call and vptr+0xa8 target
inline constexpr std::uint64_t kVtable15953f8Slot18FinishRva = 0x4a4a0;  // 1 B `retq`
inline constexpr std::uint64_t kVtable15953f8Slot18OpHandleField = 0x50;
inline constexpr std::uint64_t kVtable15953f8Slot18ModeField = 0x08;
inline constexpr std::uint32_t kVtable15953f8Slot18ModeOne = 1;
// A wait loop that never completes is a hang in the retail code; the model stops and says so.
inline constexpr std::uint32_t kVtable15953f8Slot18MaxWaits = 1024;

struct Vtable15953f8Slot18_31310Cells {
    void* self = nullptr;
    void** vtable_field = nullptr;        // *this, receives 0x1595408
    std::uint32_t* op_handle = nullptr;   // this+0x50, read for the wait
    std::uint32_t* mode = nullptr;        // this+0x8, compared with 1
};

struct Vtable15953f8Slot18_31310Hooks {
    bool (*poll_direct)(void* self) = nullptr;       // direct call 0x31900; true = no op outstanding
    void (*wait)(std::uint32_t op_handle) = nullptr; // sceFiosOpWait
    bool (*poll_virtual)(void* self) = nullptr;      // vptr+0xa8 -> 0x31900 (same body as poll_direct)
    void (*succeeded)(void* self) = nullptr;         // vptr+0xa0 -> 0x318f0; the result is discarded
    void (*close)(void* self) = nullptr;             // vptr+0x18 -> 0x31520 (wait + sceFiosFHCloseSync)
    void (*finish)(void* self) = nullptr;            // 0x4a4a0
};

struct Vtable15953f8Slot18_31310Result {
    bool ok = false;
    void* vtable_written = nullptr;
    std::uint32_t waits = 0;
    bool wait_overflow = false;
    bool succeeded_called = false;
    bool close_called = false;
    bool finish_called = false;
    bool freed = false;
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable15953f8Slot18_31310Result vtable_15953f8_slot18_31310(
    const Vtable15953f8Slot18_31310Cells& cells = {},
    const Vtable15953f8Slot18_31310Hooks& hooks = {}) noexcept;

}  // namespace sotc
