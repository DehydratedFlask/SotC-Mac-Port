// Function `0x31520` (0x38 B): wait for the pending Fios op, then close the open file handle.
//
// Real target of the withdrawn `vptr+0x18` calls (`0x312f0`, `0x31360`): `0x1595420` holds `0x31520`
// (one type-8 RELA, `tools/lift/check_frontier_target_models.py`). Characterised from the ELF bytes
// (E1100): 0x38 B, 18 instructions, 1 indirect call (`vptr+0xb0`), 1 direct PLT call (`0xf8f660` =
// `sceFiosFHCloseSync`, libSceFios2), standard `push rbp` frame, no jump table.
//
//   void f(Object* this) {
//       this->vtable[0xb0](this);                  // 0x31532: complete the pending op (hook)
//       if (*(u32*)(this + 0x8) == 1) {            // cmpl $1, 8(%rbx)
//           sceFiosFHCloseSync(/*attr*/ 0, *(u32*)(this + 0x10));   // edi = 0, esi = [this+0x10]
//           *(u32*)(this + 0x10) = 0;
//           *(u64*)(this + 0x8)  = 0;              // qword store: clears 0x8..0xf
//       }
//   }
//
// The `vptr+0xb0` call is virtual. When the object's vptr is the `0x15953f8 + 0x10` address point that
// `0x31310` installs, that slot is `0x15954b8` -> `0x319c0` (see `frontier_319c0_fios_wait`); a
// derived object may differ, so the wait is a hook and its absence is fail-visible: with no hook the
// model changes nothing and reports `ok == false`. The close call has no result use. A missing close
// hook when the handle is open is likewise fail-visible, and the object is left untouched.
// `+0x8 == 1` is the "handle open" state (inferred: `0x31900` sets it to 1 after a successful state-1
// completion and clears it in state 2).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kFrontier31520Rva = 0x31520;
inline constexpr std::size_t kFrontier31520Size = 0x38;
inline constexpr std::uint64_t kFrontier31520EndAddress = 0x31558;
inline constexpr std::uint64_t kFrontier31520SlotAddress = 0x1595420;   // 0x15953f8 + 0x10 + 0x18
inline constexpr std::uint32_t kFrontier31520WaitVtableOffset = 0xb0;
inline constexpr std::uint64_t kFrontier31520CloseSyncPlt = 0xf8f660;   // sceFiosFHCloseSync
inline constexpr std::uint32_t kFrontier31520ModeOffset = 0x8;
inline constexpr std::uint32_t kFrontier31520HandleOffset = 0x10;
inline constexpr std::uint32_t kFrontier31520ModeOpen = 1;

struct Frontier31520Cells {
    std::uint8_t* object = nullptr;  // at least 0x18 bytes; null is fail-visible
};

struct Frontier31520Hooks {
    void (*wait)(void* object, void* user) = nullptr;                 // vptr+0xb0
    void (*close_sync)(std::uint64_t attr, std::uint32_t handle, void* user) = nullptr;  // PLT 0xf8f660
    void* user = nullptr;
};

struct Frontier31520Result {
    bool ok = false;
    bool wait_called = false;
    bool closed = false;            // sceFiosFHCloseSync was called
    std::uint32_t closed_handle = 0;
    const char* missing_hook = nullptr;
};

[[nodiscard]] Frontier31520Result frontier_31520_fios_close(
    const Frontier31520Cells& cells = {}, const Frontier31520Hooks& hooks = {}) noexcept;

}  // namespace sotc
