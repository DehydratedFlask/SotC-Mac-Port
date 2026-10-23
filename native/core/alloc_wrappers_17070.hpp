// Native lift of the three tiny allocator wrappers around `0x12820` / `0x12af0`.
//
// ELF sha256 d0723afb…861d3. Byte-verified (all end in a rel32 `jmp`, so they are tail wrappers):
//   0x17070 (20 B): mov esi,1 ; mov edx,8 ; mov ecx,1 ; jmp 0x12820   ; allocate(size=edi)
//   0x170b0 (10 B): mov esi,1 ; jmp 0x12af0                          ; free(ptr, tag 1)
//   0x170c0 (10 B): mov esi,2 ; jmp 0x12af0                          ; free(ptr, tag 2)
//
// This corrects an earlier label: `0x170b0` is not "operator delete" in the abstract — it is the
// tag-1 entry of the same dispatcher `0x12af0` that `shared_release_f590` reaches with tag 0.
// The allocator family therefore is: allocate via `0x12820` (tag 1, alignment 8, flags 1) and
// deallocate via `0x12af0` with a tag that distinguishes the owner kind.
//
// NOT modelled: the bodies of `0x12820` and `0x12af0` (size-class bookkeeping, TLS state and
// logging); the meaning of each tag beyond "owner kind / alignment class". G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kAlloc17070Rva = 0x17070;
inline constexpr std::size_t kAlloc17070Size = 20;
inline constexpr std::uint64_t kFree170b0Rva = 0x170B0;
inline constexpr std::size_t kFree170b0Size = 10;
inline constexpr std::uint64_t kFree170c0Rva = 0x170C0;
inline constexpr std::size_t kFree170c0Size = 10;
inline constexpr std::uint64_t kAllocDispatchRva = 0x12820;
inline constexpr std::uint64_t kFreeDispatchRva = 0x12AF0;

// Host contract for the un-lifted dispatch pair. Tags are passed through verbatim so a caller can
// distinguish the wrappers.
struct AllocDispatchContract {
    void* (*allocate)(std::size_t size, std::uint32_t esi_tag, std::uint32_t align,
                      std::uint32_t flags) = nullptr;
    bool (*deallocate)(void* ptr, std::uint32_t tag) = nullptr;
};

struct AllocWrapperResult {
    bool ok = false;
    bool dispatched = false;
    void* pointer = nullptr;     // allocation result
    std::uint32_t tag = 0;       // tag used for a free
    std::size_t size = 0;        // size used for an allocation
};

// 0x17070: allocate(edi = size) with the recovered constants (tag 1, align 8, flags 1).
[[nodiscard]] AllocWrapperResult alloc_17070(std::size_t size,
                                             const AllocDispatchContract& contract) noexcept;

// 0x170b0 / 0x170c0: free(ptr) through 0x12af0 with tag 1 / tag 2. Null ptr is a no-op success
// (the dispatcher checks for null itself); a missing contract is a visible failure.
[[nodiscard]] AllocWrapperResult free_170b0(void* ptr,
                                            const AllocDispatchContract& contract) noexcept;
[[nodiscard]] AllocWrapperResult free_170c0(void* ptr,
                                            const AllocDispatchContract& contract) noexcept;

}  // namespace sotc
