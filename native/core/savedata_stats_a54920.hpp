// Native model of original RVA `0xa54920` (1,840 B, `0xa54920..0xa55050`, sha256
// 27c76ceb08e4aac8d9e67b1dfcdeb87c8833fa8e73ae4f56de2481c475167810).
//
// This is the save-data/statistics registry constructor called three times from Init's
// `0x5329ae..0x532b0c` block (`0x532a07`, `0x532a62`, `0x532ae2`). The body is unrolled, not a
// loop: three 0xd8 B counters, three 0x26ea0 menu toggles, three deferred 0x272d0 atexit records,
// then one 0xa55050 sibling object attached to the parent's vector at `parent+0x5e8`.
//
// Measured from the pinned ELF in `logs/a54920_body.dis` (llvm-objdump, 2026-11-09):
//   363 instructions, 42 direct calls, 11 indirect, 13 branches, 73 memory stores.
//   direct calls: 0xf590 x12, 0x10a90 x6, 0x17070 x5, 0x17090 x1, 0x170b0 x2, 0x20d60 x3,
//                 0x26ea0 x3, 0x272d0 x3, 0x3a980 x3, 0xa55050 x1, 0xa57c10 x1, 0xa586a0 x1,
//                 0xf8f3a0 x1 (cold canary tail).
//   indirect: `[rax+0x18]` x4 (bare-ret descriptor/functor slots) and `[rax+0x8]` x4 / `[rax]` x3
//             (old parent-slot and descriptor vcalls).
//   branches: je x10, jmp x1, jl x1, jne x1. No backward branch; the three register blocks are
//             unrolled copies.
//
// The three counters are built with the already-lifted 0x3a980 worker
// (`init_unbounded_int_var_worker_3a980`). The only new admission is the caller's argument functor
// table 0x15d1d80, whose four slot words are verified by the oracle to be the lifted
// 0xc720/0xc740/0xc760/0xc770 bodies. Each counter then takes:
//   vptr = 0x1594c08, +0x68 = +0xd0 = max_count (first) or 0 (second/third),
//   0x20d60 registration, and replacement of the caller's +0x40/+0x48/+0x50 slot.
//
// The three menu toggles are the 0x26ea0 shape with the "Save System" label and the formatted
// "{prefix} Max Count" / "{prefix} Saves Used" / "{prefix} Save Mem Used (MB)" text. Their
// 0x272d0 atexit records are deferred until all three toggles exist, exactly as the original orders
// them. The 0x26ea0 range stays {1.0, 10.0} because the int class answers virtual slots 1 / 2.
//
// The sibling slot allocator `0xa55050` is a full 347 B model in `native/core/savedata_family.*`
// and is called directly here. `args.parent` points at the eight 0xb8 slots; `args.sibling_cursor`
// supplies the 0x32f88a0 dword. The returned slot is 0xb8 B; the 0x100 B allocation at a54fd9 is
// the deque block, not a descriptor. The list child block is 0x30 B.
//
// The previous list head is cleared by the real `0xa57c10` model with `self = previous+8`, then
// `previous->child` and `previous` are freed through `0x170b0`. The parent vector append calls the
// real `0xa586a0` deque-map grow and allocates its 0x100 B block through `0x17090`.
//
// Two hooks remain precise and caller-owned: the old parent-slot destructor at 0xa54a64/0xa54b7b/
// 0xa54c92 (virtual `[old_vptr+8]`) and the five descriptor vcalls at 0xa54f07 (`[in.vtable40+8]`),
// 0xa54f20 (`[tmp.vptr]`), 0xa54f36 (`[item.vptr]`), 0xa54f46 (`[tmp.vptr]`), and 0xa54f71
// (`[tmp.vptr+0x18]`). Their targets come from caller-supplied virtual objects, not a fixed body.
//
// G1 PARTIAL: no game boot, audio, save or Metal gate is claimed. RVAs are identity tags.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "ctor_slot_family_items.hpp"
#include "savedata_family.hpp"

namespace sotc {

inline constexpr std::uint64_t kSavedataStatsA54920Rva = 0xA54920;
inline constexpr std::size_t kSavedataStatsA54920Size = 1840;   // decimal: report_init_coverage.py parses decimal Size
inline constexpr std::uint64_t kSavedataStatsA54920EndRva = 0xA55050;
inline constexpr char kSavedataStatsA54920Sha256[] =
    "27c76ceb08e4aac8d9e67b1dfcdeb87c8833fa8e73ae4f56de2481c475167810";

inline constexpr std::uint64_t kSavedataStatsSiblingRva = 0xA55050;
inline constexpr std::uint64_t kSavedataStatsListDtorRva = 0xA57C10;
inline constexpr std::uint64_t kSavedataStatsVectorGrowRva = 0xA586A0;

inline constexpr std::uint64_t kSavedataStatsCounterVptr = 0x1594C08;   // 0x1594bf8 + 0x10
inline constexpr std::uint64_t kSavedataStatsArgFnTable = 0x15D1D80;
inline constexpr std::uint32_t kSavedataStatsCtorFlags = 1;
inline constexpr std::uint32_t kSavedataStatsItemFlags = 0x30;
inline constexpr std::size_t kSavedataStatsCounterCount = 3;
inline constexpr std::size_t kSavedataStatsConVarCount = 3;
inline constexpr std::size_t kSavedataStatsCounterSize = 0xD8;
inline constexpr std::size_t kSavedataStatsNameBufferSize = 0x20;
inline constexpr std::size_t kSavedataStatsDescriptorSize = 0xB8;
inline constexpr std::size_t kSavedataStatsInDescriptorSize = 0x50;
inline constexpr std::size_t kSavedataStatsParentMapOffset = 0x5E8;
inline constexpr std::size_t kSavedataStatsListHeadOffset = 0x38;
inline constexpr std::uint64_t kSavedataStatsListSentinel = 0x100000001;
inline constexpr std::uint32_t kSavedataStatsListType = 2;
inline constexpr std::uint32_t kSavedataStatsListIndex = 0xFFFFFFFF;
inline constexpr std::size_t kSavedataStatsListNodeSize = 0x28;
inline constexpr std::size_t kSavedataStatsListChildSize = 0x30;

inline constexpr std::uint64_t kSavedataStatsLabelRva = 0x12C412D;
inline constexpr char kSavedataStatsLabel[] = "Save System";
inline constexpr std::size_t kSavedataStatsLabelLen = 0xB;
inline constexpr std::uint64_t kSavedataStatsEmptyDescriptionRva = 0x12C41FE;

inline constexpr std::uint64_t kSavedataStatsFormatMaxCountRva = 0x12C4360;
inline constexpr std::uint64_t kSavedataStatsFormatSavesUsedRva = 0x12C4372;
inline constexpr std::uint64_t kSavedataStatsFormatMemUsedRva = 0x12C4385;
inline constexpr char kSavedataStatsFormatMaxCount[] = "save_%s_max_count";
inline constexpr char kSavedataStatsFormatSavesUsed[] = "save_%s_saves_used";
inline constexpr char kSavedataStatsFormatMemUsed[] = "save_%s_mem_used";
inline constexpr std::size_t kSavedataStatsFormatMaxCountLen = 0x11;
inline constexpr std::size_t kSavedataStatsFormatSavesUsedLen = 0x12;
inline constexpr std::size_t kSavedataStatsFormatMemUsedLen = 0x10;

inline constexpr std::uint64_t kSavedataStatsConVarMaxCountRva = 0x12C4396;
inline constexpr std::uint64_t kSavedataStatsConVarSavesUsedRva = 0x12C43A3;
inline constexpr std::uint64_t kSavedataStatsConVarMemUsedRva = 0x12C43B1;
inline constexpr char kSavedataStatsConVarMaxCount[] = "%s Max Count";
inline constexpr char kSavedataStatsConVarSavesUsed[] = "%s Saves Used";
inline constexpr char kSavedataStatsConVarMemUsed[] = "%s Save Mem Used (MB)";
inline constexpr std::size_t kSavedataStatsConVarMaxCountLen = 0xC;
inline constexpr std::size_t kSavedataStatsConVarSavesUsedLen = 0xD;
inline constexpr std::size_t kSavedataStatsConVarMemUsedLen = 0x15;

// Caller-owned name record (`[r12]` in the original). Only the fields this body reads are named.
struct SavedataStatsName {
    std::uint8_t gap00[8];        // +0x00 never read here
    const char* prefix;           // +0x08 snprintf `%s` argument
    std::uint8_t gap10[0x28];     // +0x10 never read here
    std::uint32_t max_count;      // +0x38 first counter's +0x68/+0xd0
    std::uint8_t gap3c[4];
    void* parent_slot40;          // +0x40 first counter attachment
    void* parent_slot48;          // +0x48 second counter attachment
    void* parent_slot50;          // +0x50 third counter attachment
};
static_assert(sizeof(SavedataStatsName) == 0x58);
static_assert(offsetof(SavedataStatsName, prefix) == 0x08);
static_assert(offsetof(SavedataStatsName, max_count) == 0x38);
static_assert(offsetof(SavedataStatsName, parent_slot40) == 0x40);
static_assert(offsetof(SavedataStatsName, parent_slot48) == 0x48);
static_assert(offsetof(SavedataStatsName, parent_slot50) == 0x50);

// The caller's stack descriptor (`rdx`), copied by the tail. Only the copied fields are named.
struct SavedataStatsInDescriptor {
    std::uint64_t field00;        // -> sibling +0x48
    std::uint64_t field08;        // 16 B -> sibling +0x50
    std::uint64_t field10;
    std::uint64_t field18;        // -> sibling +0x60; non-zero makes +0x48 self-point
    std::uint8_t gap20[0x20];     // passed to the first descriptor vcall (rsi = this + 0x20)
    void* vtable40;               // vcall [this+0x40]+0x8
    std::uint64_t field48;        // -> sibling +0x90
};
static_assert(sizeof(SavedataStatsInDescriptor) == 0x50);
static_assert(offsetof(SavedataStatsInDescriptor, vtable40) == 0x40);
static_assert(offsetof(SavedataStatsInDescriptor, field48) == 0x48);

// The 0x28 B list node allocated at 0xa54e36, owner = the third call's name record.
struct SavedataStatsListNode {
    const void* owner;            // +0x00 = name record
    std::uint8_t gap08[8];
    void* child;                  // +0x10 = the 0x30 B self-linked child
    std::uint64_t zero18;         // +0x18 = 0
    std::uint32_t index20;        // +0x20 = -1
    std::uint8_t pad24[4];
};
static_assert(sizeof(SavedataStatsListNode) == 0x28);
static_assert(offsetof(SavedataStatsListNode, child) == 0x10);
static_assert(offsetof(SavedataStatsListNode, zero18) == 0x18);
static_assert(offsetof(SavedataStatsListNode, index20) == 0x20);

struct SavedataStatsListChild {
    SavedataStatsListChild* self0;   // +0x00 = this
    SavedataStatsListChild* self8;   // +0x08 = this
    SavedataStatsListChild* self10;  // +0x10 = this
    std::uint8_t one18;              // +0x18 = 1
    std::uint8_t one19;              // +0x19 = 1
    std::uint8_t pad1a[0x16];        // the original allocates 0x30 B at a54e46
};
static_assert(sizeof(SavedataStatsListChild) == 0x30);

// The sibling object this body receives from 0xa55050. Fields are at the offsets the body writes.
struct SavedataStatsDescriptor {
    std::uint8_t gap00[0x10];
    std::uint64_t sentinel10;         // +0x10 = 0x0000000100000001
    std::uint8_t gap18[4];
    std::uint32_t type1c;             // +0x1c = 2
    std::uint8_t gap20[0x18];
    SavedataStatsListNode* head38;    // +0x38 replaces the previous node
    std::uint8_t gap40[8];
    std::uint64_t field48;            // +0x48 = in.field00 (or &field48 when in.field18 != 0)
    std::uint8_t field50[0x10];       // +0x50 = in.field08..+0x17
    std::uint64_t field60;            // +0x60 = in.field18
    std::uint8_t subobject68[0x28];   // +0x68 descriptor vcall carrier
    std::uint64_t fn90;               // +0x90 = in.field48
    std::uint8_t tail98[0x20];        // slot tail to 0xb8; not written by this body
};
static_assert(sizeof(SavedataStatsDescriptor) == 0xB8);
static_assert(offsetof(SavedataStatsDescriptor, sentinel10) == 0x10);
static_assert(offsetof(SavedataStatsDescriptor, type1c) == 0x1C);
static_assert(offsetof(SavedataStatsDescriptor, head38) == 0x38);
static_assert(offsetof(SavedataStatsDescriptor, field48) == 0x48);
static_assert(offsetof(SavedataStatsDescriptor, field50) == 0x50);
static_assert(offsetof(SavedataStatsDescriptor, field60) == 0x60);
static_assert(offsetof(SavedataStatsDescriptor, subobject68) == 0x68);
static_assert(offsetof(SavedataStatsDescriptor, fn90) == 0x90);

enum class SavedataStatsRefusal : std::uint8_t {
    none,
    null_name,
    null_descriptor,
    null_parent,
    null_cursor,
    worker_rejected,       // 0x3a980 refused the argument functor table
    registration_failed,   // 0x20d60 did not queue/insert
    item_failed,           // 0x26ea0 / 0x268b0 refused
    sibling_allocation_failed,   // 0xa55050 refused its parent/cursor
    previous_clear_failed,       // 0xa57c10 could not release the old head
    previous_free_failed,        // 0x170b0 refused old child/node
    invalid_parent_map,          // deque finish fields are absent
    vector_grow_failed,          // 0xa586a0 refused or had no allocator
    vector_block_failed,         // 0x17090 refused the 0x100 B block
};

struct SavedataStatsA54920Hooks {
    // Five descriptor vcalls: 0 = [in.vtable40+8], 1 = [tmp.vptr], 2 = [item.vptr],
    // 3 = [desc+0x90], 4 = [tmp.vptr+0x18]. Never guest code.
    void (*descriptor_vcall)(int index, void* a, void* b) noexcept = nullptr;
    // Old parent-slot destructor `[old_vptr+8]`. Null -> counted, not called.
    void (*parent_slot_dtor)(void* old_slot) noexcept = nullptr;
};

struct SavedataStatsA54920Args {
    void* parent = nullptr;                                       // 0xa55050 argument + deque owner
    std::uint32_t* sibling_cursor = nullptr;                      // 0x32f88a0 dword cell
    AllocDispatchContract alloc{};                                // 0x17070/0x17090/0x170b0 dispatch
    std::array<SavedataStatsName*, kSavedataStatsCounterCount> names{};
    const SavedataStatsInDescriptor* descriptor = nullptr;
};

struct SavedataStatsA54920Result {
    bool ok = false;
    SavedataStatsRefusal refusal = SavedataStatsRefusal::none;

    std::array<ConCmdRangedVar, kSavedataStatsCounterCount> counters{};
    std::array<DebugMenuItem, kSavedataStatsConVarCount> items{};
    std::array<std::array<char, kSavedataStatsNameBufferSize>, kSavedataStatsCounterCount> names{};
    // Owned toggle text buffers: 0x26ea0 stores a borrowed SharedStr view, so the buffer must
    // outlive the item and cannot be a loop local.
    std::array<std::array<char, kSavedataStatsNameBufferSize>, kSavedataStatsConVarCount> item_texts{};
    std::array<std::uint32_t, kSavedataStatsCounterCount> max_counts{};

    // The registries this run filled; inspectable by the fixture.
    ConCmdRegistry concmd{};
    DebugMenuRegistry menu{};

    SavedataStatsDescriptor* descriptor = nullptr;   // points into args.parent's 0xb8 slot
    SavedataStatsListNode node{};
    SavedataStatsListChild child{};

    std::array<bool, kSavedataStatsCounterCount> worker_ok{};
    std::array<bool, kSavedataStatsCounterCount> name_view_inert{};
    std::array<bool, kSavedataStatsCounterCount> description_view_inert{};
    std::array<bool, kSavedataStatsCounterCount> registered{};
    std::array<bool, kSavedataStatsCounterCount> parent_slot_replaced{};
    std::array<bool, kSavedataStatsCounterCount> old_parent_slot_destroyed{};
    std::array<bool, kSavedataStatsCounterCount> item_ctor_ok{};
    std::array<bool, kSavedataStatsCounterCount> item_registered{};

    int atexit_count = 0;                 // expect 3 (0x272d0, one per item)
    int descriptor_vcall_count = 0;       // expect 5
    bool sibling_slot_found = false;
    std::uint32_t sibling_slot_index = 0;
    bool descriptor_head_installed = false;
    bool previous_head_destroyed = false;
    bool parent_vector_appended = false;
    bool vector_grow_called = false;
    bool vector_block_allocated = false;
};

// Runs the recovered sequence. The caller owns every name record and the input descriptor; the
// result owns the three counters, three items and sibling object by value. No guest byte is run.
[[nodiscard]] SavedataStatsA54920Result savedata_stats_a54920(
    const SavedataStatsA54920Args& args,
    const SavedataStatsA54920Hooks& hooks = {}) noexcept;

}  // namespace sotc
