// CPU-01-HELPER-CALLEE-MODELS-03: contract of the PLT import thunk 0xf8f630 (ELF sha256 d0723afb...5861d3).
//
//   0xf8f630: ff 25 d2 0a 67 00   jmp qword [rip + 0x670ad2]     ; GOT slot 0x1600108
//   (then `push 0x2a ; jmp 0xf8f380`, the lazy-binding stub, which is the PLT body and not part of the callable)
//
// The GOT slot 0x1600108 has exactly one JMPREL row (R_X86_64_JUMP_SLOT, addend 0, symbol 370) whose NID `6sJWiWSRuqk`
// equals the NID hash of the C library function `strncpy` (tools/lift/check_helper_f8f630.py recomputes it with the
// same NID function that proves the sibling thunks 0xf8f390 = __cxa_atexit, 0xf8f3a0 = __stack_chk_fail, 0xf8f3b0 =
// strlen and 0xf8f480 = memmove). So 0xf8f630 is NOT game code and NOT a Sony SDK function: it is ISO C `strncpy`, and a
// native build implements it with the ISO C semantics below; no libSce module is involved.
//
//   char* strncpy(char* dst, const char* src, size_t n):
//     copies at most n bytes of src, stops copying at the first NUL, then writes NUL bytes up to exactly n bytes in
//     total (the destination is NOT terminated when src has n or more bytes). Overlap is undefined. Returns dst.
//
// Sole startup-slot consumer: slot 503 (0x247f00), call at 0x247f47: `[0x2eea480] = 0 ; strncpy(0x2eea484, "NULL", 15) ;
// [0x2eea493] = 0`. The source literal is the five bytes `NULL\0` at 0xfc15f3 (pinned by the checker). The cell is a
// 0x14 byte record `{u32 id; char name[16]}`; the model of that first block is `store_null_name_cell_247f47`. The meaning of
// the record (INFERENCE only: a default-named entry) is not recovered. Other callers of the thunk (the function at 0x14c3f0, call at
// 0x14c471, a 4 byte copy) are outside this task.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kStrncpyPltRva = 0xf8f630;
inline constexpr std::uint64_t kStrncpyGotRva = 0x1600108;
inline constexpr std::uint64_t kNullNameCellCopyLength = 15;     // edx at 0x247f2d
inline constexpr std::uint64_t kNullNameCellTerminatorOffset = 0x13;   // movb $0, 0x13(%rbx) at 0x247f53

// ISO C strncpy with the exact padding rule. `dst` must have room for `n` bytes, `src` must be NUL terminated or have at
// least `n` readable bytes. Returns `dst`.
char* strncpy_f8f630(char* dst, const char* src, std::size_t n) noexcept;

// The 0x14 byte record slot 503 initialises at 0x2eea480.
struct NullNameCell247f47 {
    std::uint32_t id;       // +0x00  movl $0, (%rbx)
    char name[16];          // +0x04  strncpy(rbx+4, "NULL", 15); name[15] (offset 0x13) = 0
};
static_assert(sizeof(NullNameCell247f47) == 0x14 && offsetof(NullNameCell247f47, name) == 4);

// Slot 503's first block (0x247f41..0x247f53) exactly: id = 0, strncpy(name, "NULL", 15), byte 0x13 = 0.
void store_null_name_cell_247f47(NullNameCell247f47& cell) noexcept;

}  // namespace sotc
