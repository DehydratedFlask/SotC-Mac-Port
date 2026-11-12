// CPU-01-STARTUP-BATCH-10: native recovery of the original helper 0x23fd00 (86 bytes, ELF sha256 d0723afb...5861d3), a 20 byte "hash + short name"
// cell initialiser. It is called by startup constructor slot 342 (twice, with a NULL source) and by 84 other static initialisers; each call site is
// byte-pinned by the slot checkers, and the helper body itself by tools/lift/check_name_cell_23fd00.py.
//
//   rdi = cell, rsi = source C string (may be NULL)
//   if (rsi != NULL) { cell->hash = 0x162020(rsi, 0x811c9dc5); strncpy(cell + 4, rsi, 15); }
//   else             { cell->hash = 0;                          strncpy(cell + 4, "NULL", 15); }   // 4 bytes + zero padding
//   cell[0x13] = 0;                                              // the 16th text byte is always a terminator
//
// 0x162020 is the native fnv_path_cstr_162020 (path-normalising FNV-1a; seed = the FNV offset basis, so the cell hash equals fnv_cstr_a2ddc0). The
// strncpy import is identified through its JMPREL NID 6sJWiWSRuqk (the project's service_map.json names that NID strncpy; the pointed-to libc routine is
// not lifted: native uses a bounded loop with the same observable result). The default text is the 5 bytes `NULL\0` at 0xfc15f3. Nothing else is
// written, read or called. What the 20 byte cell is used for is NOT established.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kNameCell23fd00Rva = 0x23fd00;
inline constexpr std::size_t kNameCell23fd00Size = 86;
inline constexpr std::uint64_t kNameCell23fd00NullTextRva = 0xfc15f3;   // `NULL\0` read by the NULL branch
inline constexpr char kNameCell23fd00NullText[] = "NULL";
inline constexpr std::size_t kNameCell23fd00CopyLength = 15;            // strncpy length argument (edx = 0xf)

struct HashedName20 {
    std::uint32_t hash;       // +0x00
    char text[16];            // +0x04 .. +0x13; text[15] (offset 0x13) is always 0
};
static_assert(sizeof(HashedName20) == 20 && offsetof(HashedName20, hash) == 0 && offsetof(HashedName20, text) == 4);

// Writes all 20 bytes of the cell. NULL source -> hash 0 and text "NULL" (zero padded).
void init_name_cell_23fd00(HashedName20& cell, const char* source) noexcept;

}  // namespace sotc
