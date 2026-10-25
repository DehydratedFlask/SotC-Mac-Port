// Row-18 owner constructor `0x3c5cb0` (`kOwnerMemberRows[18]`, 0xb8 B heap object, `this + 0x3b70`
// field slot, first argument `[0x2edf140]`).
//
// Characterization: this header plus `local/re/evidence/owner-row18-3c5cb0/` (`stores.txt` = the
// llvm-objdump store/call census, `ghidra-census.txt` = Ghidra `-noanalysis -readOnly` census,
// `stores.py`); offline static decode only. The round-29 lane-C report carries the narrative.
// Body `0x3c5cb0..0x3c5ded` (FDE extent), size `0x13d` (317 B), sha256
// `2d30b3a53e7b438ab83f9f74888f8b0a1376c03b0d6b44a502afab74b7c562fe`, 72 decoded instructions
// (Ghidra: 71 real + the trailing `nop`), nine call sites / six distinct targets, zero indirect
// calls. Three `nop`s of padding follow, then the *next function* at `0x3c5df0`.
//
// Correction to the E570 row table (`owner-rows-2569d0/report.md` §1.2/§1.4), which listed this
// body as "`0x227b`, 1528 insns, 31 calls, ... largest body in the set, blocked": the bytes say
// **0x13d / 72 insns / 9 call sites**. The E570 recursive-descent walk fell through the
// `call 0xf8f3a0` (`__stack_chk_fail`, noreturn) into the padding and then the following
// functions - including `0x3c5df0` (a `sub rsp,0xdc8` function) and `0x3c7380`, which are the two
// *callbacks this constructor registers*. This is the smallest owner ctor body after the three
// trivial ones and the factory `0xa2b960`.
//
// Object layout (`this`, 0xb8 B), byte-exact. The object is partially initialized: every hole keeps
// the caller's fill byte.
//   +0x00 byte 0                       (+0x01..+0x03 hole)
//   +0x04 dword 0                      (covers +0x04..+0x07; no hole before the slot)
//   +0x08 0x20 B empty slot            (`0xf180`, native as `init_slot_f180`)
//   (+0x28..+0x2f hole)
//   +0x30 qword  sentinel A            (0x30 B node: [0]=[8]=itself; the rest is never written)
//   +0x38 qword  0                     (a size counter by inference)
//   (+0x40..+0x47 hole)
//   +0x48 qword  sentinel B            (0x30 B node, same shape)
//   +0x50 qword  0
//   +0x58 dword  0                     (+0x5c..+0x67 hole)
//   +0x68 qword  sentinel C            (0x50 B node: [0]=[8]=itself)
//   +0x70 qword  0
//   +0x78 dword  0xffffffff
//   +0x7c dword  0
//   +0x80 byte 0, +0x81 byte 0         (+0x82..+0x83 hole)
//   +0x84 qword  0                     (unaligned: covers +0x84..+0x8b; +0x8c..+0x8f hole)
//   +0x90 0x20 B empty slot            (`0xf180`)
//   +0xb0 8 B mutex handle             (`0x3b310`, hook-owned; ends exactly at 0xb8)
// The three sentinels are self-linked circular-list heads allocated with the plain `0x17070`
// thunk (0x30, 0x30, 0x50). That they are list heads with a size counter next to them is inference
// from the {self,self} stores plus the adjacent zeroed qword; the observable contract is only the
// stores above.
//
// After the layout the body registers two callbacks on the argument object (`[0x2edf140]`, row
// 9's 0x2203f0 B object; `0x116040` iterates a vector of callables at `+0x2201f0` and appends to
// the container at `+0x220288`): `0x116040(pool, &{this, 0x3c5df0, 0, 0}, 1)`, then
// `0x116540(pool, &{this, 0x3c7380, 0, 0})`. The second callable word is the relocated
// `.data.rel.ro` slot (`0x15ff810` / `0x15ff590`), i.e. the raw RVA of a member-function entry
// that is NOT native and must never be called; the lift keeps the RVA as data.
//
// What is native: every store above, the three allocations in order, both `0xf180` slots, both
// callable frames. What stays a hook: `0x17070` (allocator), `0x3b310` (mutex init), `0x116040`
// and `0x116540` (callback registration - not lifted). A missing hook is a named `failed_at`.
//
// Not modelled: the destructor `0x3c74b0` (kDtorThenFree pairing at `0x256f84`/`0x256f8c`) and the
// two callbacks `0x3c5df0` / `0x3c7380`. The constructor never frees on failure and neither does
// this lift: the owning caller must run the teardown contract for what was allocated.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kOwnerCtor3C5CB0Rva = 0x3C5CB0;
inline constexpr std::size_t kOwnerCtor3C5CB0Size = 0x13D;
inline constexpr std::size_t kOwnerCtor3C5CB0InsnCount = 72;
inline constexpr std::size_t kOwnerCtor3C5CB0ObjSize = 0xB8;
inline constexpr std::uint64_t kOwnerCtor3C5CB0SlotOffset = 0x3B70;  // this + 0x3b70 (field slot)
inline constexpr std::uint64_t kOwnerCtor3C5CB0ArgRva = 0x2EDF140;   // first argument is [this cell]
inline constexpr std::size_t kOwnerMemberRow18 = 18;

// Destructor contract only: `0x3c74b0`; the row teardown is dtor then `0x170b0`.
inline constexpr std::uint64_t kOwnerDtor3C74B0Rva = 0x3C74B0;

// Callees in call order.
inline constexpr std::uint64_t kOwnerRow18SlotInitRva = 0xF180;
inline constexpr std::uint64_t kOwnerRow18AllocRva = 0x17070;
inline constexpr std::uint64_t kOwnerRow18MutexInitRva = 0x3B310;
inline constexpr std::uint64_t kOwnerRow18RegisterARva = 0x116040;
inline constexpr std::uint64_t kOwnerRow18RegisterBRva = 0x116540;

// Allocation sizes, in order.
inline constexpr std::size_t kOwnerRow18NodeASize = 0x30;
inline constexpr std::size_t kOwnerRow18NodeBSize = 0x30;
inline constexpr std::size_t kOwnerRow18NodeCSize = 0x50;

// Object offsets.
inline constexpr std::size_t kOwnerRow18Byte00Offset = 0x00;
inline constexpr std::size_t kOwnerRow18Dword04Offset = 0x04;
inline constexpr std::size_t kOwnerRow18Slot0Offset = 0x08;   // 0xf180
inline constexpr std::size_t kOwnerRow18HeadAOffset = 0x30;
inline constexpr std::size_t kOwnerRow18CountAOffset = 0x38;
inline constexpr std::size_t kOwnerRow18HeadBOffset = 0x48;
inline constexpr std::size_t kOwnerRow18CountBOffset = 0x50;
inline constexpr std::size_t kOwnerRow18Dword58Offset = 0x58;
inline constexpr std::size_t kOwnerRow18HeadCOffset = 0x68;
inline constexpr std::size_t kOwnerRow18CountCOffset = 0x70;
inline constexpr std::size_t kOwnerRow18Dword78Offset = 0x78;  // 0xffffffff
inline constexpr std::size_t kOwnerRow18Dword7COffset = 0x7C;
inline constexpr std::size_t kOwnerRow18Byte80Offset = 0x80;
inline constexpr std::size_t kOwnerRow18Byte81Offset = 0x81;
inline constexpr std::size_t kOwnerRow18Qword84Offset = 0x84;  // unaligned
inline constexpr std::size_t kOwnerRow18Slot1Offset = 0x90;    // 0xf180
inline constexpr std::size_t kOwnerRow18MutexOffset = 0xB0;    // 0x3b310, 8 B
inline constexpr std::uint32_t kOwnerRow18Dword78Value = 0xFFFFFFFFu;

// The two registered callables {this, relocated slot value, 0, 0}.
inline constexpr std::uint64_t kOwnerRow18CallbackASlotRva = 0x15FF810;
inline constexpr std::uint64_t kOwnerRow18CallbackARva = 0x3C5DF0;
inline constexpr std::uint64_t kOwnerRow18CallbackBSlotRva = 0x15FF590;
inline constexpr std::uint64_t kOwnerRow18CallbackBRva = 0x3C7380;
inline constexpr std::uint32_t kOwnerRow18RegisterAFlag = 1;  // edx of the 0x116040 call

// One 0x20 B callable frame, exactly as the original builds it on its stack.
struct OwnerRow18Callable {
    void* self;                // +0x00 this
    std::uint64_t function;    // +0x08 raw RVA from the relocated slot - never called
    std::uint64_t word2;       // +0x10 0
    std::uint64_t word3;       // +0x18 0
};

static_assert(sizeof(OwnerRow18Callable) == 0x20);
static_assert(offsetof(OwnerRow18Callable, function) == 0x08);

struct OwnerRow18Hooks {
    // 0x17070(size): called with 0x30, 0x30, 0x50 in that order. Null or a null result fails at
    // "alloc_list_a" / "alloc_list_b" / "alloc_list_c" and keeps the prefix.
    void* (*allocate)(std::size_t size) = nullptr;
    // 0x3b310(this + 0xb0): mutex init (8 B handle). Null or false fails at "mutex_init"; the body
    // never writes +0xb0..+0xb8 itself.
    bool (*mutex_init)(void* at) = nullptr;
    // 0x116040(pool, &callable, 1). Null or false fails at "register_116040".
    bool (*register_116040)(void* pool, const OwnerRow18Callable& callable,
                            std::uint32_t flag) = nullptr;
    // 0x116540(pool, &callable). Null or false fails at "register_116540".
    bool (*register_116540)(void* pool, const OwnerRow18Callable& callable) = nullptr;
};

struct OwnerRow18Result {
    bool ok;
    const char* failed_at;  // "null-object", "object-align", "alloc_list_a", "alloc_list_b",
                            // "alloc_list_c", "mutex_init", "register_116040", "register_116540"
};

// Builds the 0xb8 B object at `object` and registers the two callbacks on `pool` (the row's first
// argument, `[0x2edf140]`; passed through verbatim - the original does not test it, so a null
// `pool` is the hook's problem). `object` must be writable for 0xb8 B and 8-byte aligned.
// `ok == false` stops at the first failure and keeps the already-written prefix, exactly like the
// original would before faulting; nothing is freed or rolled back.
[[nodiscard]] OwnerRow18Result owner_ctor_3c5cb0(void* object, void* pool,
                                                 const OwnerRow18Hooks& hooks) noexcept;

// Adapter for `owner_members`' context-free ctor hook (`bool (*)(row, object, arg)`, E579).
// `arg` is the row's first argument (`state.globals[0x2EDF140]`). One binding at a time, bound by
// the host before `init_owner_members` and unbound after; any row other than 18 is refused
// without touching `object`. Single-threaded startup use only.
struct OwnerRow18Binding {
    OwnerRow18Hooks hooks{};
    OwnerRow18Result last{false, nullptr};
    std::size_t calls = 0;
    void* last_pool = nullptr;
};

void owner_row18_bind(OwnerRow18Binding* binding) noexcept;  // nullptr unbinds
[[nodiscard]] bool owner_row18_ctor_hook(std::size_t row, void* object, void* arg) noexcept;

}  // namespace sotc
