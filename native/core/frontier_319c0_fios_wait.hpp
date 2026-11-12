// Function `0x319c0` (0x3a B): block until the Fios op settles, then tail-call the success predicate.
//
// This is what `0x31520` reaches through `vptr+0xb0` when the object's vptr is the `0x1595408` address
// point (`0x15954b8` holds `0x319c0`, one type-8 RELA). It is the same loop as the body of `0x31310`:
//
//   while (!this->vtable[0xa8](this)) sceFiosOpWait(*(u32*)(this + 0x50));   // 0x31900 poll, PLT 0xf8f7d0
//   return this->vtable[0xa0](this);                                         // `jmp *0xa0(%rax)` tail call
//
// Characterised from the ELF bytes (E1100): 0x3a B, 20 instructions, `push rbp` frame, 2 indirect
// transfers (`call vptr+0xa8`, tail `jmp vptr+0xa0`), 1 PLT call. The poll and the tail target are
// virtual, so both are hooks (`0x31900` / `0x318f0` for the `0x1595408` table). The retail loop has no
// exit while the op never settles; the model stops after `kFrontier319c0MaxWaits` and says so.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kFrontier319c0Rva = 0x319c0;
inline constexpr std::size_t kFrontier319c0Size = 0x3a;
inline constexpr std::uint64_t kFrontier319c0EndAddress = 0x319fa;
inline constexpr std::uint64_t kFrontier319c0SlotAddress = 0x15954b8;   // 0x15953f8 + 0x10 + 0xb0
inline constexpr std::uint64_t kFrontier319c0WaitPlt = 0xf8f7d0;        // sceFiosOpWait
inline constexpr std::uint32_t kFrontier319c0OpHandleOffset = 0x50;
inline constexpr std::uint32_t kFrontier319c0MaxWaits = 1024;           // not retail

struct Frontier319c0Hooks {
    bool (*poll)(void* object, void* user) = nullptr;              // vptr+0xa8 (0x31900)
    void (*wait)(std::uint32_t op_handle, void* user) = nullptr;   // sceFiosOpWait
    bool (*tail)(void* object, void* user) = nullptr;              // vptr+0xa0 (0x318f0), returned
    void* user = nullptr;
};

struct Frontier319c0Result {
    bool ok = false;
    bool value = false;          // the tail call's result
    std::uint32_t waits = 0;
    bool wait_overflow = false;
    const char* missing_hook = nullptr;
};

[[nodiscard]] Frontier319c0Result frontier_319c0_fios_wait(
    std::uint8_t* object, const Frontier319c0Hooks& hooks = {}) noexcept;  // object >= 0x54 B; null fails

}  // namespace sotc
