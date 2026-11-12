// CPU-01-AOT-CFG-SCALE-01-BATCH22 (slot 532): native model of startup constructor slot 532
// (RVA 0x143f50, 832 B; FDE [0x143f50, 0x144290)). Original ELF sha256 d0723afb...5861d3; the
// constructor-table word at 0x2dd70e0 - 8 * 532 = 0x2dd6040 holds 0x143f50.
// Every fact below was read end to end from the pinned bytes; the byte oracle is
// tools/lift/check_ctor_slot_532.py. NOTHING here executes, loads or calls an original RVA:
// destructor RVAs and the __dso_handle are IDENTITY TAGS, recorded in the atexit log and never run.
//
// ================================ observed shape ================================
// Frame: push rbp; mov rsp,rbp; push r15/r14/rbx; push rax. No local frame, no stack guard and no
// conditional branch anywhere in the body: it is one straight-line run of 21 identical groups.
//
// Group i (i = 0..20), object 0x2ee2a58 + 0x20 * i:
//   lea  rbx, object          ; lea rdx, literal ; xor esi,esi ; mov rdi,rbx
//   call 0xf2f0               ; SharedStr from a NUL-terminated C string: {ptr, strlen, 0, 0}
//   mov  rdi,0xf7b0 (dtor)    ; mov rsi,rbx (object) ; mov rdx,0x1604218 (__dso_handle)
//   call __cxa_atexit         ; the 21st group ends in `jmp __cxa_atexit` after the epilogue
//
// `esi` is zero at every site and the literal is passed in `rdx`, so 0xf2f0 receives
// (destination, 0, cstring). The views are BORROWED: 0xf2f0 stores only {ptr, strlen, 0, 0} and the
// 0x20-byte object has no control block, so nothing is allocated and no release call appears.
//
// The literals are fragments of debug strings in .rodata (source paths and pretty-function tails);
// several windows contain non-UTF-8 bytes, which is expected for an interior pointer. The body only
// measures them with strlen, so the model records the measured length and never reinterprets text.
//
// Interpretations: the 21 views read as a static table of debug/backtrace name fragments registered
// for teardown. What reads the table at run time is NOT established; that reading is INFERENCE only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot532Rva = 0x143f50;
inline constexpr std::size_t kSlot532Size = 832;
inline constexpr std::uint64_t kSlot532TableWord = 0x2dd6040;   // 0x2dd70e0 - 8 * 532

inline constexpr std::uint64_t kSlot532Object0 = 0x2ee2a58;
inline constexpr std::size_t kSlot532Stride = 0x20;             // sizeof(SharedStr)
inline constexpr std::size_t kSlot532Count = 21;
inline constexpr std::uint64_t kSlot532LastObject = 0x2ee2cd8;   // Object0 + 20 * Stride

inline constexpr std::uint64_t kSlot532BorrowedStrRva = 0xf2f0;  // {ptr, strlen, 0, 0}
inline constexpr std::uint64_t kSlot532StrlenRva = 0xf8f3b0;     // the strlen 0xf2f0 calls
inline constexpr std::uint64_t kSlot532Dtor = 0xf7b0;            // the registered destructor
inline constexpr std::uint64_t kSlot532DsoHandle = 0x1604218;
inline constexpr std::uint64_t kSlot532AtexitRva = 0xf8f390;

// The literals, in body order, and the byte length the body's strlen measures (the run to the next
// NUL). Both arrays are pinned independently against the ELF by the checker.
inline constexpr std::uint64_t kSlot532TextRvas[kSlot532Count] = {
    0xfa8d4c, 0xfa8d86, 0xfa8dc2, 0xfa8e00, 0xfa8e31, 0xfa8e63, 0xfa8e95,
    0xfa8ec6, 0xfa8ef5, 0xfa8f27, 0xfa8f5d, 0xfa8f93, 0xfa8fc7, 0xfa9002,
    0xfa9037, 0xfa9070, 0xfa90aa, 0xfa90e4, 0xfa9122, 0xfa9169, 0xfa91a1,
};
inline constexpr std::size_t kSlot532TextLengths[kSlot532Count] = {
    0x15, 0x52, 0x16, 0x3f, 0x0e, 0x59, 0x27, 0x63, 0x34, 0x02, 0x00,
    0x00, 0x55, 0x1a, 0x03, 0x24, 0x58, 0x1e, 0x00, 0x00, 0x00,
};
inline constexpr std::size_t kSlot532EmptyCount = 5;   // indices 10, 11, 18, 19 and 20 are empty


// The 21 literals, byte for byte from the pinned rodata. Interior pointers into debug strings:
// several windows contain non-UTF-8 bytes. Escapes are split with "" so a hex escape can never
// swallow the following character. The runner passes this table; nothing here is interpreted.
inline constexpr char kSlot532Text0[] = "EvaluatorAllocator &)";
inline constexpr char kSlot532Text1[] = "Main\\Code\\BPE\\Source\\Renderer\\GameObjects\\Properties/./CPOLdrFogDirectionality.cpp";
inline constexpr char kSlot532Text2[] = "rFogDirectionality.cpp";
inline constexpr char kSlot532Text3[] = "erties(CGameObjectApplyPropertiesData &, CEvaluatorAllocator &)";
inline constexpr char kSlot532Text4[] = "orAllocator &)";
inline constexpr char kSlot532Text5[] = "\\Main\\Code\\BPE\\Source\\Renderer\\GameObjects\\Properties/./CPOLdrWindDirectionProperties.cpp";
inline constexpr char kSlot532Text6[] = "ies/./CPOLdrWindDirectionProperties.cpp";
inline constexpr char kSlot532Text7[] = "drWindDirectionProperties::ApplyProperties(CGameObjectApplyPropertiesData &, CEvaluatorAllocator &)";
inline constexpr char kSlot532Text8[] = "eObjectApplyPropertiesData &, CEvaluatorAllocator &)";
inline constexpr char kSlot532Text9[] = "&)";
inline constexpr char kSlot532Text10[] = "";
inline constexpr char kSlot532Text11[] = "";
inline constexpr char kSlot532Text12[] = "\xff""\xb9"".\x15""\xff""<.\x15""\xff""\xf7"".\x15""\xff""\x09"";\x15""\xff""@<\x15""\xff""@<\x15""\xff""@<\x15""\xff""@<\x15""\xff""@<\x15""\xff""s;\x15""\xff""@<\x15""\xff""\xdd"";\x15""\xff""@<\x15""\xff""@<\x15""\xff""@<\x15""\xff""@<\x15""\xff""@<\x15""\xff""\x04""<\x15""\xff""+<\x15""\xff""Material";
inline constexpr char kSlot532Text13[] = "\x15""\xff""@<\x15""\xff""@<\x15""\xff""\x04""<\x15""\xff""+<\x15""\xff""Material";
inline constexpr char kSlot532Text14[] = "ies";
inline constexpr char kSlot532Text15[] = "enderer\\Particles\\ParticleHandle.cpp";
inline constexpr char kSlot532Text16[] = "ndle bpe::game::ParticleManager::CreateHandle(const TAsset<bpe::ParticleSystem> &, bool)";
inline constexpr char kSlot532Text17[] = "<bpe::ParticleSystem> &, bool)";
inline constexpr char kSlot532Text18[] = "";
inline constexpr char kSlot532Text19[] = "";
inline constexpr char kSlot532Text20[] = "";
inline constexpr const char* kSlot532Texts[kSlot532Count] = {
    kSlot532Text0, kSlot532Text1, kSlot532Text2, kSlot532Text3, kSlot532Text4, kSlot532Text5, kSlot532Text6, kSlot532Text7, kSlot532Text8, kSlot532Text9, kSlot532Text10, kSlot532Text11, kSlot532Text12, kSlot532Text13, kSlot532Text14, kSlot532Text15, kSlot532Text16, kSlot532Text17, kSlot532Text18, kSlot532Text19, kSlot532Text20,
};

struct Slot532Objects {
    SharedStr views[kSlot532Count]{};   // 0x2ee2a58 + 0x20 * i, in body order
};

struct Slot532Result {
    bool ok = false;
    bool refused = false;          // the caller's text table was missing or short: nothing was written
    std::size_t views_built = 0;
    std::size_t atexit_records = 0;
};

// `texts` must expose kSlot532Count NUL-terminated strings, in body order. The model measures each
// one (the body's 0xf2f0 does the same) and registers the view's destructor identity; it never
// stores the text itself.
[[nodiscard]] Slot532Result init_slot532_143f50(Slot532Objects& objects, StartupAtexitLog& log,
                                                const char* const* texts) noexcept;

}  // namespace sotc
