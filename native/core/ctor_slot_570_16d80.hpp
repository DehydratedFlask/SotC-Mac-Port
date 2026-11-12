// CPU-01-AOT-CFG-SCALE-01-BATCH21 (slot 570): native model of startup constructor slot 570
// (RVA 0x16d80, 740 B; FDE [0x16d80, 0x17064), trailing 90 90 90 90). Original ELF sha256
// d0723afb...5861d3; the constructor-table word at 0x2dd70e0 - 8 * 570 = 0x2dd69c0 holds 0x16d80.
// Every fact below was read end to end from the pinned bytes; the byte oracle is
// tools/lift/check_ctor_slot_570.py. NOTHING here executes, loads or calls an original RVA:
// vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded in the atexit log
// and never run by the startup runner.
//
// ================================ observed shape ================================
// Frame: push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; sub rsp,0x48; guard <- *[0x15fe518].
// The body has exactly one conditional branch: 0x1704d `jne 0x1705e`, the __stack_chk_guard
// compare, whose target is the __stack_chk_fail tail at 0x1705e. Everything else is straight line.
//
// Preamble: three bespoke static tables, each followed by its own __cxa_atexit(dtor, object):
//   flags   0x2dd9f80 / 0x2dd9f84 / 0x2dd9f88 / 0x2dd9f8c  <- dword 1 each
//   cells   0x2dd9f90, 0x2dd9fa8, 0x2dd9fc0, 0x2dd9fd8      <- dword 0 and 16 zero bytes at +8;
//           the bytes at +4..+7 are never written
//   xchg    0x2dd9ff0 <- 0   (`xchgq %rax,0x2dd9ff0(%rip)` with %rax = 0; the loaded previous
//           value goes to %rax and is discarded, so the body observes nothing from it)
//           atexit(0x11070, 0x2dd9ff0)  -- identity dtor
//   block   0x2dda000: +0x00 qword 0; +0x10 16 zero bytes; +0x20 qword 0x2dda038
//           (= block + 0x38, a self pointer); +0x28 qword -1; +0x30 dword 0; +0x34 dword 0x80.
//           The qword at +0x08 is never written.
//   atexit(0x11080, 0x2dfa440) where 0x2dfa440 = 16 zero bytes then qword 0 at +0x10.
//   atexit(0x11090, 0x2dfa460) where 0x2dfa460 = qword 0x2dfa480, qword 0x2dfa480, qword 0x2dfa930
//           (the third entry is the bool variable's own address -- a back pointer; the values at
//           0x2dfa480 and 0x2dfa470 are lea'd and stored, never written by this body).
//
// Then one bool console variable and one debug-menu toggle, in this order:
//   `mem_Verify` (0xf9489c, 0xa) at 0x2dfa930: name view built by 0xf2f0 (measure), description the
//   shared empty string at 0xf94b3f (length 0), ConCmdBase ctor 0x20af0, class vptr 0x1594218,
//   functor tag 0x15949f0 -> slots {0xc2a0,0xc2c0,0xc2e0,0xc2f0} (the bool family), register-once
//   0x20d60, value and default byte 0, atexit(0x20b0).
//   toggle 0x2dfa9f8: 0x26ea0(label `Memory` 0xf948a7, text `Verify Memory Integrity` 0xf948ae,
//   target = the bool variable, flags 0x30), then 0xf590(text), 0xf590(label), atexit(0x272d0).
//
// Interpretations: the strings read as a memory-verification debug toggle. What reads the variable
// at run time is NOT established; that reading is INFERENCE only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"
#include "ctor_slot_family_items.hpp"
#include "debug_menu_item.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot570Rva = 0x16d80;
inline constexpr std::size_t kSlot570Size = 740;
inline constexpr std::uint64_t kSlot570TableWord = 0x2dd5f10;   // 0x2dd70e0 - 8 * 570

// Preamble tables.
inline constexpr std::uint64_t kSlot570FlagCell0 = 0x2dd9f80;
inline constexpr std::size_t kSlot570FlagCount = 4;
inline constexpr std::uint32_t kSlot570FlagValue = 1;
inline constexpr std::uint64_t kSlot570ZeroCell0 = 0x2dd9f90;
inline constexpr std::size_t kSlot570ZeroCellStride = 0x18;
inline constexpr std::size_t kSlot570ZeroCellCount = 4;
inline constexpr std::size_t kSlot570ZeroCellTail = 16;        // zero bytes at +8
inline constexpr std::uint64_t kSlot570XchgCell = 0x2dd9ff0;
inline constexpr std::uint64_t kSlot570SentinelBlock = 0x2dda000;
inline constexpr std::uint64_t kSlot570SentinelSelf = 0x2dda038;   // block + 0x38
inline constexpr std::uint64_t kSlot570SentinelOnes = 0xffffffffffffffff;
inline constexpr std::uint32_t kSlot570SentinelCapacity = 0x80;
inline constexpr std::uint64_t kSlot570ZeroBlock2 = 0x2dfa440;
inline constexpr std::uint64_t kSlot570TripleBlock = 0x2dfa460;
inline constexpr std::uint64_t kSlot570TripleSelf0 = 0x2dfa480;
inline constexpr std::uint64_t kSlot570TripleSelf1 = 0x2dfa480;
inline constexpr std::uint64_t kSlot570TripleBackPtr = 0x2dfa930;   // the bool variable
inline constexpr std::uint64_t kSlot570IdentityDtor0 = 0x11070;
inline constexpr std::uint64_t kSlot570IdentityDtor1 = 0x11080;
inline constexpr std::uint64_t kSlot570IdentityDtor2 = 0x11090;

// The bool variable and its menu row.
inline constexpr std::uint64_t kSlot570BoolRva = 0x2dfa930;
inline constexpr std::uint64_t kSlot570BoolFnTag = 0x15949f0;
inline constexpr std::uint64_t kSlot570BoolClassVptr = 0x1594218;
inline constexpr std::uint64_t kSlot570BoolDtor = 0x20b0;
inline constexpr char kSlot570BoolName[] = "mem_Verify";
inline constexpr std::size_t kSlot570BoolNameLength = 0xa;
inline constexpr char kSlot570BoolEmptyDescription[] = "";
inline constexpr std::size_t kSlot570BoolEmptyDescriptionLength = 0;
inline constexpr std::uint8_t kSlot570BoolValue = 0;
inline constexpr std::uint8_t kSlot570BoolDefault = 0;
inline constexpr std::size_t kSlot570AtexitCount = 5;   // three identity tables, the bool, the item

inline constexpr std::uint64_t kSlot570ToggleRva = 0x2dfa9f8;
inline constexpr char kSlot570ToggleLabel[] = "Memory";
inline constexpr std::size_t kSlot570ToggleLabelLength = 6;   // "Memory"
inline constexpr char kSlot570ToggleText[] = "Verify Memory Integrity";
inline constexpr std::size_t kSlot570ToggleTextLength = 0x17;
inline constexpr std::uint32_t kSlot570ToggleFlags = 0x30;
inline constexpr std::uint64_t kSlot570MenuDtor = 0x272d0;

// The three preamble tables, with the bytes the body leaves untouched represented explicitly.
struct Slot570ZeroCell {
    std::uint32_t zero;         // +0x00
    std::uint32_t not_written;  // +0x04: the body never stores here
    std::uint8_t tail[kSlot570ZeroCellTail];   // +0x08
};
static_assert(sizeof(Slot570ZeroCell) == 0x18);

struct Slot570Sentinel {
    std::uint64_t qword0;       // +0x00 = 0
    std::uint64_t gap08;        // +0x08: never written
    std::uint8_t zeros10[16];   // +0x10
    std::uint64_t self;         // +0x20 = block + 0x38
    std::uint64_t ones;         // +0x28 = -1
    std::uint32_t dword30;      // +0x30 = 0
    std::uint32_t capacity;     // +0x34 = 0x80
};
static_assert(sizeof(Slot570Sentinel) == 0x38);

struct Slot570ZeroBlock2 {
    std::uint8_t zeros[16];     // +0x00
    std::uint64_t qword10;      // +0x10 = 0
};
static_assert(sizeof(Slot570ZeroBlock2) == 0x18);

struct Slot570Triple {
    std::uint64_t a;            // +0x00 = 0x2dfa480
    std::uint64_t b;            // +0x08 = 0x2dfa480
    std::uint64_t back;         // +0x10 = the bool variable's address
};
static_assert(sizeof(Slot570Triple) == 0x18);

struct Slot570Objects {
    std::uint32_t flags[kSlot570FlagCount]{};
    Slot570ZeroCell cells[kSlot570ZeroCellCount]{};
    std::uint64_t xchg_cell = 0;
    Slot570Sentinel sentinel{};
    Slot570ZeroBlock2 zero_block2{};
    Slot570Triple triple{};
    ConCmdBoolVar var{};          // 0x2dfa930
    DebugMenuItem toggle{};       // 0x2dfa9f8
};

struct Slot570Result {
    bool ok = false;
    bool preamble_ok = false;     // all three tables written and their three identity records logged
    bool bool_var_ok = false;
    bool toggle_ok = false;
    std::uint64_t xchg_previous = 0;   // the value the xchgq loads into %rax and discards
    std::size_t atexit_records = 0;
};

// `fn_tag` is the site's caller argument-functor tag. The model claims exactly one: any other tag is
// refused before the first store, because no other tag's slot bodies were checked for this site.
[[nodiscard]] Slot570Result init_slot570_16d80(Slot570Objects& objects, ConCmdRegistry& registry,
                                               DebugMenuRegistry& menu,
                                               std::uint64_t fn_tag = kSlot570BoolFnTag) noexcept;

}  // namespace sotc
