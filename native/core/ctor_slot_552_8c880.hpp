// CPU-01-STARTUP-SLOT-552: native model of startup constructor index 552 / RVA 0x8c880.
//
// Original ELF sha256 d0723afb...5861d3. FDE [0x8c880, 0x8d617), 3,479 B; body sha256 is pinned by
// tools/lift/check_ctor_slot_552.py. Constructor table word 0x2dd70e0 - 8 * 552 = 0x2dd5ec0 holds
// 0x8c880. Offline only: no original RVA is executed.
//
// Body shape (all values pinned from the original bytes):
//   * one straight-line block: the only control flow is the tail `jmp 0xf8f390` at 0x8d612; there is
//     no stack canary, no loop and no conditional branch.
//   * 8 `callq 0x475c0` page-base constructors, 7 `callq 0xf8f390` records and the eighth record as
//     that tail jump. 0x475c0 is the 87 B page base ctor already modelled for slot 314.
//   * the global block 0x2ed1f48..0x2ed33e8 (0x14a0 B) is written in five parts: 7 flag dwords set
//     to 1, 17 zeroed 0x18-byte records, a 0x20-byte gap, six 0x2e0-byte page objects and two
//     smaller page objects (0xf0 and 0x98 bytes).
//
// ================================ object map (verified strides) ================================
//   +0x000  0x1c   7 x dword 1                      0x2ed1f48, 0x1f4c, 0x1f50, 0x1f54, 0x1f58, 0x1f5c,
//                                                   0x1f60
//   +0x01c  0x04   UNWRITTEN                        0x2ed1f64
//   +0x020  0x198  17 records of 0x18: dword 0 at +0, UNWRITTEN +4..+7, 16 zero bytes at +8.
//                                                   record k starts at 0x2ed1f68 + 0x18k
//   +0x1b8  0x20   UNWRITTEN                        0x2ed2100..0x2ed2120
//   +0x1d8  0x2e0  page A 0, "GameToRenderThread"   0x2ed2120
//   +0x4b8  0x2e0  page A 1, "GameToGPU"            0x2ed2400
//   +0x798  0x2e0  page A 2, "CPU_RenderToGPUToGame" 0x2ed26e0
//   +0xa78  0x2e0  page A 3, "RenderSingleFrameGPU" 0x2ed29c0
//   +0xd58  0x2e0  page A 4, "RenderSingleFrameCPU" 0x2ed2ca0
//   +0x1038 0x2e0  page A 5, "XPRTempData"          0x2ed2f80
//   +0x1318 0x0f0  page B,   "MaterialParamHeap"    0x2ed3260
//   +0x1408 0x098  page C,   "VirtualMemoryGPU"     0x2ed3350
//   total   0x14a0                                   block ends 0x2ed33e8
//
// Each page begins with the same 0x80-byte 0x475c0 base block: vptr 0x1595900 (a vtable address
// point, never dispatched here), byte +0x08 = 0, byte +0x09 = 1, the borrowed name pointer at
// +0x10, dword +0x18 = 0, the 32 zero bytes at +0x1c, dword +0x3c = 0, the self-link {+0x40, +0x48}
// = page + 0x38, qword +0x50 = 0, dword +0x58 = 0, qword +0x60 = 0 (the original uses
// `xchg rax,[rdi+0x60]` with rax = 0) and qword +0x70 = 0.
//
// Page A then stores its own vptr 0x1594ca0, qword +0x80 = 0, dword +0x88 = 0, 32 zero bytes at
// +0x90, and twelve self-referential list nodes at +0xb0 + 0x30k whose {next, prev} both point at
// the node itself. The 16 bytes at node+0x20 are zeroed for nodes 0..10 (the original writes each
// as part of a 32-byte zero store that the following node's own store then overwrites); the last
// node is 0x20 bytes and the page's final 16 bytes (+0x2d0..+0x2df) are never written.
//
// Page B stores its vptr 0x1595b70, four qwords at +0x80..+0x9f and the qword at +0xa0, all equal
// to page + 0x80, qword +0xa8 = 0, and 0x38 zero bytes at +0xb8..+0xef (written as two overlapping
// 32-byte stores). Its +0xb0..+0xb7 stay unwritten.
//
// Page C stores its vptr 0x1597560, 16 zero bytes at +0x80 and the qword +0x90 = 0; it is 0x98 B.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact): the whole straight-line store program, the five-part block layout, the
// 0x475c0 page base ctor (`init_slot552_page_475c0`), the page-A/B/C store bodies, and all eight
// __cxa_atexit records in body order.
// HOOK: `Slot552Hooks::page_ctor` (0x475c0). It has NO default in the struct; a null hook stops the
// fragment before the first page's stores, and `default_slot552_hooks()` installs the native model.
//
// Fail-visible contracts (reported through `blocked` / `ok == false`, never a silent pass):
//   * a null or rejecting page ctor (a name that is not the exact borrowed literal);
//   * a failing __cxa_atexit record.
// NOT modelled: every page destructor body (0x1ce70 for pages A, 0x48790 for page B, 0x8c360 for
// page C). They are identity tags recorded by __cxa_atexit and are never executed. Nothing consumes
// these pages yet, so no render-thread, GPU or memory-pool behaviour is claimed.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "concmd_base.hpp"
#include "l2_slot_common.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot552Rva = 0x8c880;
inline constexpr std::size_t kSlot552Size = 0xd97;             // 3479 B, FDE [0x8c880, 0x8d617)
inline constexpr std::uint64_t kSlot552EndRva = 0x8d617;
inline constexpr std::size_t kSlot552InstructionCount = 561;
inline constexpr std::size_t kSlot552DirectCallSites = 16;     // 8 x 0x475c0 + 8 x __cxa_atexit
inline constexpr std::size_t kSlot552PageCtorCalls = 8;
inline constexpr std::size_t kSlot552AtexitCalls = 8;          // 7 calls + the tail jump
inline constexpr std::size_t kSlot552RipStoreSites = 171;
inline constexpr std::size_t kSlot552RipTargets = 211;
inline constexpr std::uint64_t kSlot552TableWordRva = 0x2dd5ec0;   // 0x2dd70e0 - 552*8
inline constexpr std::uint32_t kSlot552TableIndex = 552;
inline constexpr std::uint8_t kSlot552PadByte = 0x90;

// Shared callees.
inline constexpr std::uint64_t kSlot552PageCtorRva = 0x475c0;
inline constexpr std::size_t kSlot552PageCtorSize = 87;
inline constexpr std::uint64_t kSlot552AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot552DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot552AtexitTailJumpRva = 0x8d612;

// Identity vtable address points (never dispatched through).
inline constexpr std::uint64_t kSlot552PageCtorVptr = 0x1595900;   // 0x15958f0 + 0x10
inline constexpr std::uint64_t kSlot552PageAVptr = 0x1594ca0;      // 0x1594c90 + 0x10
inline constexpr std::uint64_t kSlot552PageBVptr = 0x1595b70;      // 0x1595b60 + 0x10
inline constexpr std::uint64_t kSlot552PageCVptr = 0x1597560;      // 0x1597550 + 0x10

// Destructor identity tags (recorded, never run).
inline constexpr std::uint64_t kSlot552PageADtorRva = 0x1ce70;
inline constexpr std::uint64_t kSlot552PageBDtorRva = 0x48790;
inline constexpr std::uint64_t kSlot552PageCDtorRva = 0x8c360;

// Global block.
inline constexpr std::uint64_t kSlot552BlockRva = 0x2ed1f48;
inline constexpr std::size_t kSlot552BlockSize = 0x14a0;
inline constexpr std::uint64_t kSlot552FlagsRva = 0x2ed1f48;
inline constexpr std::size_t kSlot552FlagCount = 7;
inline constexpr std::uint64_t kSlot552RecordRva = 0x2ed1f68;
inline constexpr std::size_t kSlot552RecordCount = 17;
inline constexpr std::size_t kSlot552RecordStride = 0x18;
inline constexpr std::uint64_t kSlot552GapAfterFlagsRva = 0x2ed1f64;
inline constexpr std::uint64_t kSlot552GapBeforePagesRva = 0x2ed2100;
inline constexpr std::uint64_t kSlot552PageARva = 0x2ed2120;
inline constexpr std::size_t kSlot552PageACount = 6;
inline constexpr std::size_t kSlot552PageASize = 0x2e0;
inline constexpr std::uint64_t kSlot552PageBRva = 0x2ed3260;
inline constexpr std::size_t kSlot552PageBSize = 0xf0;
inline constexpr std::uint64_t kSlot552PageCRva = 0x2ed3350;
inline constexpr std::size_t kSlot552PageCSize = 0x98;
inline constexpr std::size_t kSlot552NodeCount = 12;
inline constexpr std::size_t kSlot552NodeStride = 0x30;
inline constexpr std::size_t kSlot552NodesOffset = 0xb0;
inline constexpr std::size_t kSlot552BaseSize = 0x80;

// Strings (borrowed literal views; identity addresses of the original rodata).
inline constexpr std::uint64_t kSlot552NameTableRva = 0xf9ecf0;
inline constexpr char kSlot552Name0[] = "GameToRenderThread";
inline constexpr char kSlot552Name1[] = "GameToGPU";
inline constexpr char kSlot552Name2[] = "CPU_RenderToGPUToGame";
inline constexpr char kSlot552Name3[] = "RenderSingleFrameGPU";
inline constexpr char kSlot552Name4[] = "RenderSingleFrameCPU";
inline constexpr char kSlot552Name5[] = "XPRTempData";
inline constexpr char kSlot552Name6[] = "MaterialParamHeap";
inline constexpr char kSlot552Name7[] = "VirtualMemoryGPU";

// ---- layout -----------------------------------------------------------------------------------
// The 0x80-byte block 0x475c0 writes at the start of every page. The eight bytes at +0x0a, the four
// at +0x5c, the eight at +0x68 and the eight at +0x78 are never written.
struct Slot552PageBase {
    std::uint64_t vptr;        // +0x00  0x1595900 from the ctor, then the page's own tag
    std::uint8_t byte08;       // +0x08  0
    std::uint8_t byte09;       // +0x09  1
    std::uint8_t gap0a[6];     // +0x0a  never written
    std::uint64_t name;        // +0x10  the borrowed name pointer
    std::uint32_t word18;      // +0x18  the ctor's third argument (0 at every site)
    std::uint8_t zero1c[0x20]; // +0x1c  32 zero bytes
    std::uint32_t word3c;      // +0x3c  0
    std::uint64_t link40;      // +0x40  &page + 0x38
    std::uint64_t link48;      // +0x48  &page + 0x38
    std::uint64_t word50;      // +0x50  0
    std::uint32_t word58;      // +0x58  0
    std::uint8_t gap5c[4];     // +0x5c  never written
    std::uint64_t word60;      // +0x60  0 (xchg with rax = 0 in the original)
    std::uint8_t gap68[8];     // +0x68  never written
    std::uint64_t word70;      // +0x70  0
    std::uint8_t gap78[8];     // +0x78  never written
};
static_assert(sizeof(Slot552PageBase) == kSlot552BaseSize);

struct Slot552Node {
    void* next;                 // +0x00  written as &node
    void* prev;                 // +0x08  written as &node
    std::uint8_t gap10[0x10];   // +0x10  never written
    std::uint8_t zero20[0x10];  // +0x20  zeroed for the first eleven nodes
};
static_assert(sizeof(Slot552Node) == kSlot552NodeStride);

struct Slot552NodeTail {
    void* next;                 // +0x00  written as &node
    void* prev;                 // +0x08  written as &node
    std::uint8_t gap10[0x10];   // +0x10  never written
};
static_assert(sizeof(Slot552NodeTail) == 0x20);

// Page A: 0x2e0 B, six instances.
struct Slot552PageA {
    Slot552PageBase base{};                 // +0x000
    std::uint64_t word80;                   // +0x080  0
    std::uint32_t word88;                   // +0x088  0
    std::uint8_t gap8c[4];                  // +0x08c  never written
    std::uint8_t zero90[0x20];              // +0x090  32 zero bytes
    std::array<Slot552Node, 11> nodes;      // +0x0b0  .. +0x2bf
    Slot552NodeTail last_node;              // +0x2c0  .. +0x2df, +0x2d0..+0x2df unwritten
};
static_assert(sizeof(Slot552PageA) == kSlot552PageASize);
static_assert(offsetof(Slot552PageA, word80) == 0x80 && offsetof(Slot552PageA, word88) == 0x88 &&
              offsetof(Slot552PageA, gap8c) == 0x8c && offsetof(Slot552PageA, zero90) == 0x90 &&
              offsetof(Slot552PageA, nodes) == 0xb0 &&
              offsetof(Slot552PageA, last_node) == 0x2c0);

// Page B: 0xf0 B. The five qwords at +0x80..+0xa7 all hold page + 0x80.
struct Slot552PageB {
    Slot552PageBase base{};                 // +0x000
    std::array<std::uint64_t, 5> head;      // +0x080  {p, p, p, p, p}, p = &page + 0x80
    std::uint64_t word_a8;                  // +0x0a8  0
    std::uint8_t gapb0[8];                  // +0x0b0  never written
    std::uint8_t zero_b8[0x38];             // +0x0b8  0x38 zero bytes
};
static_assert(sizeof(Slot552PageB) == kSlot552PageBSize);
static_assert(offsetof(Slot552PageB, head) == 0x80 && offsetof(Slot552PageB, word_a8) == 0xa8 &&
              offsetof(Slot552PageB, gapb0) == 0xb0 && offsetof(Slot552PageB, zero_b8) == 0xb8);

// Page C: 0x98 B.
struct Slot552PageC {
    Slot552PageBase base{};                 // +0x000
    std::uint8_t zero80[0x10];              // +0x080
    std::uint64_t word90;                   // +0x090
};
static_assert(sizeof(Slot552PageC) == kSlot552PageCSize);
static_assert(offsetof(Slot552PageC, zero80) == 0x80 && offsetof(Slot552PageC, word90) == 0x90);

// One zeroed 0x18-byte record: dword 0, four unwritten bytes, sixteen zero bytes.
struct Slot552Record {
    std::uint32_t word;        // +0x00  0
    std::uint8_t gap4[4];      // +0x04  never written
    std::uint8_t zero8[0x10];  // +0x08  16 zero bytes
};
static_assert(sizeof(Slot552Record) == kSlot552RecordStride);

struct Slot552Flags {
    std::array<std::uint32_t, kSlot552FlagCount> value;   // seven dwords, all 1
    std::uint8_t gap1c[4];                                // never written
};
static_assert(sizeof(Slot552Flags) == 0x20);

// The whole 0x14a0-byte block. Offsets are pinned by static_assert against the original addresses.
struct Slot552Objects {
    Slot552Flags flags{};
    std::array<Slot552Record, kSlot552RecordCount> records{};
    std::uint8_t gap1b8[0x20];                 // 0x2ed2100..0x2ed2120, never written
    std::array<Slot552PageA, kSlot552PageACount> pages_a{};
    Slot552PageB page_b{};
    Slot552PageC page_c{};
};
static_assert(sizeof(Slot552Objects) == kSlot552BlockSize);
static_assert(offsetof(Slot552Objects, flags) == 0x000);
static_assert(offsetof(Slot552Objects, records) == 0x020);
static_assert(offsetof(Slot552Objects, gap1b8) == 0x1b8);
static_assert(offsetof(Slot552Objects, pages_a) == 0x1d8);
static_assert(offsetof(Slot552Objects, page_b) == 0x1318);
static_assert(offsetof(Slot552Objects, page_c) == 0x1408);
static_assert(kSlot552BlockRva + 0x020 == kSlot552RecordRva);
static_assert(kSlot552BlockRva + 0x1b8 == kSlot552GapBeforePagesRva);
static_assert(kSlot552BlockRva + 0x1d8 == kSlot552PageARva);
static_assert(kSlot552PageARva + kSlot552PageACount * kSlot552PageASize == kSlot552PageBRva);
static_assert(kSlot552PageBRva + kSlot552PageBSize == kSlot552PageCRva);
static_assert(kSlot552PageCRva + kSlot552PageCSize == 0x2ed33e8);

// ---- result -----------------------------------------------------------------------------------
enum class Slot552Stage : std::uint8_t {
    none,
    flags,
    records,
    page_ctor,
    page_stores,
    page_atexit,
};
[[nodiscard]] const char* slot552_stage_name(Slot552Stage stage) noexcept;

struct Slot552PageResult {
    bool ok = false;
};

struct Slot552Hooks {
    // 0x475c0. NO default in this struct: a missing hook stops the fragment at `page_ctor`.
    // `default_slot552_hooks()` installs the native model.
    Slot552PageResult (*page_ctor)(void* page, const SharedStr& name) noexcept = nullptr;
};

inline constexpr std::size_t kSlot552AtexitRowCount = 8;

struct Slot552AtexitRow {
    std::uint32_t page;        // 0..7, body order
    std::uint32_t group;       // 0 = A, 1 = B, 2 = C
    std::uint64_t site_rva;    // the call (or tail jump) site
    std::uint64_t dtor_rva;    // rdi: identity tag, never run
};

inline constexpr std::array<Slot552AtexitRow, kSlot552AtexitRowCount> kSlot552AtexitRows = {{
    {0, 0, 0x8cc06, kSlot552PageADtorRva},
    {1, 0, 0x8cddc, kSlot552PageADtorRva},
    {2, 0, 0x8cfb2, kSlot552PageADtorRva},
    {3, 0, 0x8d188, kSlot552PageADtorRva},
    {4, 0, 0x8d35e, kSlot552PageADtorRva},
    {5, 0, 0x8d530, kSlot552PageADtorRva},
    {6, 1, 0x8d5b4, kSlot552PageBDtorRva},
    {7, 2, 0x8d612, kSlot552PageCDtorRva},
}};

struct Slot552Result {
    bool ok = false;
    Slot552Stage blocked = Slot552Stage::flags;
    std::uint32_t blocked_page = 0;
    std::uint32_t flags_written = 0;
    std::uint32_t records_written = 0;
    std::uint32_t pages_built = 0;
    std::uint32_t atexit_records = 0;
    bool names_borrowed = true;
};

// 0x475c0: the page base ctor. Writes +0x00..+0x77 only. Refuses (writes nothing) when the name is
// not a borrowed view over the expected literal.
[[nodiscard]] Slot552PageResult init_slot552_page_475c0(void* page, const SharedStr& name) noexcept;

// The page store bodies (the instructions between each 0x475c0 call and its __cxa_atexit record).
void init_slot552_page_a_stores(Slot552PageA& page) noexcept;
void init_slot552_page_b_stores(Slot552PageB& page) noexcept;
void init_slot552_page_c_stores(Slot552PageC& page) noexcept;

// The 7 flag dwords and the 17 zeroed records (the block's first two parts).
void init_slot552_prefix(Slot552Objects& objects) noexcept;

// The production hooks: the native 0x475c0 model.
[[nodiscard]] Slot552Hooks default_slot552_hooks() noexcept;

// Which byte of the 0x14a0-byte block the original body writes. Used by the fixture to prove the
// documented unwritten bytes; it is a static property of the body, not a runtime hook.
[[nodiscard]] bool slot552_byte_written(std::size_t offset) noexcept;

// The names, in body order.
[[nodiscard]] const char* slot552_page_name(std::uint32_t page) noexcept;
[[nodiscard]] std::size_t slot552_page_name_len(std::uint32_t page) noexcept;

// Runs original constructor 552 natively. The original is unconditional, so the first failing step
// stops the slot; every failure names its stage and page.
[[nodiscard]] Slot552Result init_slot552_8c880(Slot552Objects& objects, ConCmdRegistry& concmd,
                                               const Slot552Hooks& hooks = {}) noexcept;

}  // namespace sotc
