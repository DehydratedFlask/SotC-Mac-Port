// Resolved slot `vtable 0x15953f8 + 0x18 -> 0x31310` (0x69 B): the Fios read object's teardown.
//
// Body:
//   *(u64*)this = 0x15953f8 + 0x10 = 0x1595408      ; install the base vtable
//   pending = u8(0x31900(this))                     ; sub-init, returns a "not finished" byte
//   while (pending == 0) {
//       sceFiosOpWait(*(u32*)(this+0x50));          ; PLT 0xf8f7d0, libSceFios2
//       pending = u8(this->vtable[0xa8](this));     ; the positioned read (0x318b0)
//   }
//   this->vtable[0xa0](this);                       ; the plain read (0x31870)
//   if (*(u32*)(this+0x8) == 1) this->vtable[0x18](this);
//   0x4a4a0(this);                                  ; 1-byte `retq` — a no-op call, kept as a hook
//   jmp 0x170b0(this);                              ; tag-1 free of `this`
//
// The wait import is named by the PLT map (`sceFiosOpWait`), so the loop is an async-completion wait
// rather than a retry with unknown semantics. What `0x31900`, `0x4a4a0` and the flag at `+0x8` mean
// is not modelled; they are hooks, and a bound wait that never completes is refused instead of spun.
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
inline constexpr std::uint64_t kVtable15953f8Slot18SubInitRva = 0x31900;
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
    bool (*sub_init)(void* self) = nullptr;          // 0x31900; true means "finished"
    void (*wait)(std::uint32_t op_handle) = nullptr; // sceFiosOpWait
    bool (*positioned_read)(void* self) = nullptr;   // slot 0xa8 (0x318b0)
    void (*plain_read)(void* self) = nullptr;        // slot 0xa0 (0x31870)
    void (*mode_one)(void* self) = nullptr;          // slot 0x18
    void (*finish)(void* self) = nullptr;            // 0x4a4a0
};

struct Vtable15953f8Slot18_31310Result {
    bool ok = false;
    void* vtable_written = nullptr;
    std::uint32_t waits = 0;
    bool wait_overflow = false;
    bool plain_read_called = false;
    bool mode_one_called = false;
    bool finish_called = false;
    bool freed = false;
    std::uint32_t free_tag = 0;
};

[[nodiscard]] Vtable15953f8Slot18_31310Result vtable_15953f8_slot18_31310(
    const Vtable15953f8Slot18_31310Cells& cells = {},
    const Vtable15953f8Slot18_31310Hooks& hooks = {}) noexcept;

}  // namespace sotc
