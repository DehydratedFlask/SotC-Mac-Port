// Native lift of original RVA `0x54b250` — the stat-query wrapper reached from Init's tail
// (`0x532b09 call [rax+0x8]` through `*(0x2eff450)`, vtable slot 1).
//
// ELF sha256 d0723afb…861d3. Body `0x54b250..0x54b2c7` = 119 B, sha256 40814086…d39f.
// Byte-verified shape:
//   push rbp; mov rbp,rsp; push r14; push rbx; and rsp,-0x20; sub rsp,0x60
//   rbx = [0x15fe518] (__stack_chk_guard); canary saved at [rsp+0x58]
//   zero a 0x60 B stack descriptor: [rsp..rsp+0x20) = 0, [rsp+0x30] = 0, [rsp+0x40] = 0x15c9920
//     (traits/table), [rsp+0x48] = 0
//   rcx = &descriptor, r8 = r9 = 0, r14 = &descriptor + 0x20
//   call 0xa56560(this, incoming esi, incoming edx, &descriptor, 0, 0)   <- engine (NOT lifted)
//   rax = descriptor[+0x40]; rdi = &descriptor + 0x20; call *0x18(%rax)
//     -> vtable 0x15c9920 slot 3 = 0x538200, a bare `ret` (no observable effect)
//   canary compare; mismatch -> __stack_chk_fail (0xf8f3a0); else epilogue/ret
// Return type is void: the query result stays in the discarded stack descriptor.
//
// NOT modelled: the body of `0xa56560`, the descriptor's internal layout beyond the fields this
// function writes, and whether the discarded result matters to any caller. The no-op vcall is
// elided but its resolution is pinned by the checker. G1 PARTIAL — the engine is a contract.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kStatQuery54b250Rva = 0x54B250;
inline constexpr std::size_t kStatQuery54b250Size = 119;
inline constexpr std::uint64_t kStatQuery54b250EndRva = 0x54B2C7;
inline constexpr std::uint64_t kStatQuery54b250EngineRva = 0xA56560;
inline constexpr std::uint64_t kStatQuery54b250TraitsRva = 0x15C9920;
inline constexpr std::uint64_t kStatQuery54b250NoopSlotRva = 0x538200;
inline constexpr std::uint64_t kStatQuery54b250GuardCellRva = 0x15FE518;
inline constexpr std::size_t kStatQuery54b250DescriptorSize = 0x60;

// Host contract for the un-lifted engine `0xa56560`. It receives the caller's `this`, the two
// incoming 32-bit arguments, the descriptor and two zeros. Returning false is a visible failure.
struct StatsEngine54b250 {
    bool (*query)(void* this_obj, std::uint32_t arg1, std::uint32_t arg2, void* descriptor,
                  std::uint64_t r8, std::uint64_t r9) = nullptr;
};

struct StatQuery54b250Result {
    bool ok = false;
    int engine_calls = 0;        // expect 1
    int noop_dispatches = 0;     // expect 1 (the pinned 0x538200 bare-ret slot)
    bool engine_known = false;   // false when no engine contract was supplied
    bool engine_ok = false;
    // Descriptor fields written before the engine call (host-visible copy).
    std::uintptr_t traits_40 = 0;
    std::uintptr_t zero_30 = 0;
    std::uintptr_t zero_48 = 0;
};

// Runs the recovered sequence. `arg1`/`arg2` are the caller's esi/edx (Init passes 0 and 2).
[[nodiscard]] StatQuery54b250Result stat_query_54b250(void* this_obj, std::uint32_t arg1,
                                                      std::uint32_t arg2,
                                                      const StatsEngine54b250& engine) noexcept;

}  // namespace sotc
