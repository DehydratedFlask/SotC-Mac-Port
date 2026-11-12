// CPU-01-HELPER-CALLEE-MODELS-04: native model of the 48 six byte getters that startup slot 498 (0x252ae0) calls, and of the
// 24 hash-record stores the slot builds from them.
//
// Every getter is `b8 imm32 ; c3` (`mov eax, imm32 ; ret`, FDE 6 B): it reads nothing and calls nothing. They come in
// 24 adjacent pairs (rva, rva + 0x10). For each pair the slot runs (all instructions pinned by tools/lift/check_helper_498.py)
//
//   call lo_getter ; mov ebx, eax ; call hi_getter ; mov [cell+0], ebx ; mov [cell+4], eax ; mov dword [cell+8], size
//
// into 24 consecutive 12 byte records at 0x2eec3b0..0x2eec4d0 (0x120 B). The records are therefore {lo, hi, size} dword triples;
// the slot's last pair has the stack-canary load interleaved after the second call, which does not change the stores.
//
// What is proven: each getter's returned dword, each record's cell, the stores' order and the size immediate. What is NOT
// recovered: what the hashes are (INFERENCE only: `lo` repeats across neighbouring rows, e.g. 0xaadb73f3 in eight rows, 0x3d2f4f61
// in three, which looks like a type id with a per-member hash in `hi`). The model does not interpret them. The rest of slot 498
// (the menu / console-variable constructors) is slot-side and is not covered here. Nothing here executes an original RVA.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::size_t kMemberHashRecordCount498 = 24;
inline constexpr std::size_t kMemberHashRecordStride498 = 12;
inline constexpr std::uint64_t kMemberHashCellsRva498 = 0x2eec3b0;
inline constexpr std::size_t kMemberHashCellsBytes498 = 0x120;
inline constexpr std::uint64_t kMemberHashSlot498Rva = 0x252ae0;
inline constexpr std::size_t kMemberHashGetterSize498 = 6;
inline constexpr std::uint32_t kMemberHashGetterSpacing498 = 0x10;   // the hi getter sits 0x10 B after the lo getter

struct MemberHashRecord498 {
    std::uint32_t lo_getter_rva;   // `mov eax, lo ; ret`
    std::uint32_t hi_getter_rva;   // lo_getter_rva + 0x10
    std::uint32_t call_site;       // the slot's `call lo_getter`
    std::uint32_t lo;
    std::uint32_t hi;
    std::uint32_t size;            // the `mov dword [cell+8], imm`
};

extern const MemberHashRecord498 kMemberHashRecords498[kMemberHashRecordCount498];

// The value one of the 48 getters returns; false (and `value` untouched) when `rva` is not one of them.
bool member_hash_getter_498(std::uint32_t rva, std::uint32_t& value) noexcept;

// The slot's 24 record stores into the 0x120 byte block at 0x2eec3b0 (every dword of the block is written; stride 12).
void fill_member_hash_cells_498(std::uint8_t (&cells)[kMemberHashCellsBytes498]) noexcept;

}  // namespace sotc
