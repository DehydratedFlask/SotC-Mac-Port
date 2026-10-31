// CPU-01-499F80-BODY-NATIVE-01: native lift of app-main helper 0x499f80 (193 B, E1031).
// Offline analysis only; no original RVA is executed.
//
// Models: arg selection (rdi or global default), store through global cell, allocate 0x50 object,
// CritSec init at object+0, buffer fields via alloc_17090 (0xa00), conditional release of prior
// object at global old-object cell. Globals are offset-only host cells (no semantic names).
//
// NOT modelled: allocator dispatchers 0x12820/0x12af0 beyond AllocDispatchContract hooks.
// Does not advance engine_ready or chain to 0x56dcb0.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"

namespace sotc {

inline constexpr std::uint64_t kInit499f80BodyRva = 0x499f80;
inline constexpr std::size_t kInit499f80BodySize = 193;
inline constexpr std::uint64_t kInit499f80GlobalDefaultArgRva = 0x15ff9a8;
inline constexpr std::uint64_t kInit499f80GlobalStoreRva = 0x1604988;
inline constexpr std::uint64_t kInit499f80GlobalOldObjectRva = 0x2ef9fc8;
inline constexpr std::size_t kInit499f80ObjectAllocSize = 0x50;
inline constexpr std::size_t kInit499f80BufferChunkSize = 0xa00;

struct Init499f80Globals {
    void* default_arg = nullptr;     // models 0x15ff9a8
    void** store_cell = nullptr;       // models 0x1604988 (qword store)
    void** old_object_cell = nullptr;  // models 0x2ef9fc8
};

struct Init499f80BodyHooks {
    AllocDispatchContract alloc{};
    bool (*critsec_init)(void* object_base) noexcept = nullptr;    // 0x3b310
    bool (*critsec_destroy)(void* object_base) noexcept = nullptr;   // 0x3b380
};

struct Init499f80BodyResult {
    bool ok = false;
    const char* blocked = nullptr;
    void* arg_used = nullptr;
    void* new_object = nullptr;
    void* previous_object = nullptr;
};

// Caller owns returned `new_object` when ok; prior object release uses hooks.
[[nodiscard]] Init499f80BodyResult run_init_499f80_body(void* arg, Init499f80Globals& globals,
                                                        const Init499f80BodyHooks& hooks) noexcept;

[[nodiscard]] Init499f80BodyHooks default_init_499f80_body_hooks(
    const AllocDispatchContract& alloc) noexcept;

}  // namespace sotc
