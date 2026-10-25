// Row-14 owner constructor `0xa597d0` (`kOwnerMemberRows[14]`, 0x958 B heap object, pointer slot
// `0x32f8b80`).
//
// Characterization: `local/re/evidence/owner-row14-a597d0/report.md` (offline static decode only).
// Body `0xa597d0..0xa599d2`, size `0x202` (514 B), sha256
// `1c8596ab15884f169300e6712ebbcc41dee261fed36ff1369b6c9469f45142ec`, 98 instructions, 6 `call`
// sites / 5 distinct targets, two back edges (a single-trip allocation loop and a 64-trip clear
// loop). The FDE boundary `0xa599d2` is followed by 14 B of `0x90` padding and the next function
// `0xa599e0`, so the extent is byte-exact. (The E570 report's `0x215` / 114 is a walk overshoot
// into that padding and the next function; see report §0.)
//
// What is native here (25 `this` store sites + 1 block-array site + 3 loop sites, in order):
//   - head: `+0x00 = 1`, `+0x08..+0x17 = 0`, `+0x18 = 0`, `+0x28 = 0x10000`
//     (the dword left `+0x2a == 1`; that byte is an observable side effect, not a separate store);
//   - the 0x50 B chunked-deque header at `+0x830` (map pointer, map size 8, begin/end iterators),
//     its 0x40 B map block (`0x40 / 8 = 8` slots) and its single 0x100 B chunk;
//   - `+0x888 = 0`, `+0x8b0 = 0`;
//   - `+0x938..+0x947 = 0`, `+0x948 = 0`;
//   - the 64-trip clear loop: 64 records of 0x20 B from `+0x30` to `+0x830`, writing only
//     `+0x00` (byte), `+0x08..+0x17` (16 B) and `+0x18` (dword) of each record;
//   - `+0x29 = 0`, `+0x2b = 1` (the byte handed to the startup-mode notifier);
//   - `+0x890 = 0`, `+0x898..+0x8a7 = 0`, `+0x8a8 = 0`.
//
// What stays a hook (report §3): `0x17090` twice (0x40 B map block, then one 0x100 B chunk),
// `0x3b3d0` (event flag `{qword handle, u8 flag}` at `+0x8b8`, called with (0, 0)), `0x3b5c0`
// (7-argument thread start frame at `+0x8c8`) and `0xba40` (startup-mode notifier; its two global
// operands `0x32f8b90` / `*(0x32f8bf0)` are host state, pinned in the checker but not modelled).
//
// Fidelity rules the fixture enforces:
//   1. No `memset`/zero-fill. Only the characterized offsets are written. The object is partially
//      initialized - every hole keeps the caller's fill byte: `+0x20..+0x27`, `+0x2c..+0x2f`,
//      every record's `+0x01..+0x07` and `+0x1c..+0x1f`, `+0x82c..+0x82f`, `+0x880..+0x887`,
//      `+0x891..+0x897`, `+0x8ac..+0x8af`, the event object's `+0x8c1..+0x8c7`, `+0x8f0..+0x937`
//      and `+0x950..+0x957`.
//   2. The body never reads `this` (only the block it just allocated), so a second call writes the
//      same offsets with the same values; only the host-supplied allocation addresses may differ.
//   3. Allocation order is `0x40` then `0x100`; the map pointer `+0x830` is stored *before* the
//      chunk allocation, so a chunk-allocation failure keeps that prefix.
//
// Not modelled: the destructor `0xa59a10..0xa59bb7` (size `0x1a7`, `kDtorThenFree` pairing). Its
// observed contract: release `+0x8b0`, two `0x3b450` event-flag teardowns, the `0x8c8` thread
// object, the clear loop, tag-2 free of `[+0x938]`, then the deque chunks and the map with
// `0x170c0`. A caller that owns the object must free what this constructor allocated; the exact
// teardown order is a contract and is not reproduced here.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kOwnerCtorA597D0Rva = 0xA597D0;
inline constexpr std::size_t kOwnerCtorA597D0Size = 0x202;
inline constexpr std::size_t kOwnerCtorA597D0ObjSize = 0x958;
inline constexpr std::uint64_t kOwnerCtorA597D0SlotRva = 0x32F8B80;  // pointer slot, not the object

// Destructor contract only (report §6): row teardown is `0xa59a10` then `0x170b0`.
inline constexpr std::uint64_t kOwnerDtorA59A10Rva = 0xA59A10;
inline constexpr std::size_t kOwnerDtorA59A10Size = 0x1A7;

// Allocator (0x17090) sizes and order.
inline constexpr std::size_t kOwnerA597D0MapBytes = 0x40;    // 8 slots x 8 B
inline constexpr std::size_t kOwnerA597D0ChunkBytes = 0x100; // one deque chunk
inline constexpr std::size_t kOwnerA597D0MapSlots = 8;
inline constexpr std::size_t kOwnerA597D0ChunkTrips = 1;  // (0x20 - 0x18) / 8 == 1

// Head offsets.
inline constexpr std::size_t kOwnerA597D0RefCountOffset = 0x00;  // dword 1
inline constexpr std::size_t kOwnerA597D0VectorOffset = 0x08;    // 0x18 B {begin, end, cap_end}
inline constexpr std::size_t kOwnerA597D0FlagsOffset = 0x28;     // dword 0x10000
inline constexpr std::uint32_t kOwnerA597D0FlagsValue = 0x10000;
inline constexpr std::size_t kOwnerA597D0Byte29Offset = 0x29;    // byte 0
inline constexpr std::size_t kOwnerA597D0ModeByteOffset = 0x2B;  // byte 1

// The 64 x 0x20 inline record block.
inline constexpr std::size_t kOwnerA597D0RecordBase = 0x30;
inline constexpr std::size_t kOwnerA597D0RecordCount = 64;
inline constexpr std::size_t kOwnerA597D0RecordStride = 0x20;
inline constexpr std::size_t kOwnerA597D0RecordLimit = 0x830;  // == base + count * stride
inline constexpr std::size_t kOwnerA597D0RecordBytesOffset = 0x08;    // 16 B zero
inline constexpr std::size_t kOwnerA597D0RecordDwordOffset = 0x18;    // dword 0

// Chunked-deque header at +0x830 (libc++ shape: map, map_size, begin{cur,first,last,node},
// end{cur,first,last,node}). The 0x100 B chunk and 8 map slots are exact; the element size is not
// recoverable from this body (a 0x10 B element would follow from libc++ block sizing - unproven).
inline constexpr std::size_t kOwnerA597D0DequeOffset = 0x830;
inline constexpr std::size_t kOwnerA597D0DequeMapOffset = 0x00;     // qword map block
inline constexpr std::size_t kOwnerA597D0DequeMapSizeOffset = 0x08; // qword 8
inline constexpr std::size_t kOwnerA597D0DequeBeginOffset = 0x10;   // 4 qwords
inline constexpr std::size_t kOwnerA597D0DequeEndOffset = 0x30;     // 4 qwords
inline constexpr std::size_t kOwnerA597D0DequeBytes = 0x50;

// Tail fields.
inline constexpr std::size_t kOwnerA597D0Qword888Offset = 0x888;  // qword 0
inline constexpr std::size_t kOwnerA597D0Byte890Offset = 0x890;   // byte 0
inline constexpr std::size_t kOwnerA597D0Block898Offset = 0x898;  // 16 B zero
inline constexpr std::size_t kOwnerA597D0Dword8A8Offset = 0x8A8;  // dword 0
inline constexpr std::size_t kOwnerA597D0Qword8B0Offset = 0x8B0;  // qword 0
inline constexpr std::size_t kOwnerA597D0EventOffset = 0x8B8;     // 0x10 B {qword, u8}
inline constexpr std::size_t kOwnerA597D0ThreadOffset = 0x8C8;    // 0x28 B thread object
inline constexpr std::size_t kOwnerA597D0OwnedPtr938Offset = 0x938;  // 16 B zero; dtor frees it
inline constexpr std::size_t kOwnerA597D0Qword948Offset = 0x948;     // qword 0

// 0x3b3d0 contract: the callee stores the returned handle at +0 and the `esi` byte at +8, and
// creates the flag with attribute 0x21 and the `edx` initial pattern. Both operands are 0 here.
inline constexpr std::uint32_t kOwnerA597D0EventFlagByte = 0;
inline constexpr std::uint32_t kOwnerA597D0EventPattern = 0;

// The 0x3b5c0 callable frame is {this, 0xb09590, 0, 0}. The second word is the static addend of
// the relocated `.data.rel.ro` slot `0x15fff08`; the lift keeps the raw RVA because the worker
// `0xb09590` is not native and must never be called. A host that really starts the thread must
// resolve it to a native entry and fail the hook otherwise.
inline constexpr std::uint64_t kOwnerA597D0ThreadEntryRva = 0xB09590;
inline constexpr std::uint64_t kOwnerA597D0ThreadEntrySlotRva = 0x15FFF08;
inline constexpr std::size_t kOwnerA597D0CallableWords = 4;

// 0x3b5c0 argument values recovered from the call frame (report §2). `arg3` is the same 700
// constant the capture lane sees as a user-service priority; the remaining roles are unproven, so
// they keep positional names.
inline constexpr const char* kOwnerA597D0ThreadName = "Trophy Task Thread";
inline constexpr std::uint64_t kOwnerA597D0ThreadNameRva = 0x12C4892;
inline constexpr std::uint64_t kOwnerA597D0ThreadArg3 = 0x2BC;
inline constexpr std::uint64_t kOwnerA597D0ThreadArg4 = 0x60;
inline constexpr std::uint64_t kOwnerA597D0ThreadArg5 = 1;
inline constexpr std::uint64_t kOwnerA597D0ThreadArg6 = 0x100000;

// 0xba40 operands. The native hook receives only the byte pointer; the two globals are host state.
inline constexpr std::uint64_t kOwnerA597D0ModeConfigRva = 0x32F8B90;
inline constexpr std::uint64_t kOwnerA597D0ModeCellRva = 0x32F8BF0;

// The seven arguments of the 0x3b5c0 call, in register order followed by the stack argument.
struct OwnerA597D0ThreadRequest {
    void* thread_object;              // rdi: this + 0x8c8
    std::uint64_t callable[4];        // rsi: {this, 0xb09590, 0, 0}
    const char* name;                 // rdx: "Trophy Task Thread"
    std::uint64_t arg3;               // rcx: 0x2bc
    std::uint64_t arg4;               // r8:  0x60
    std::uint64_t arg5;               // r9:  1
    std::uint64_t stack_arg;          // [rsp]: 0x100000
};

struct OwnerA597D0Hooks {
    // 0x17090(size): allocate from the tag-2 family (see alloc_wrappers_17070.hpp). Called exactly
    // twice: 0x40, then 0x100. Null or a null result stops at that allocation.
    void* (*allocate)(std::size_t size) = nullptr;
    // 0x3b3d0(at, esi, edx): create the event flag object at `at` (0x10 B). The lift passes (0, 0).
    bool (*event_flag_create)(void* at, std::uint32_t flag_byte,
                              std::uint32_t initial_pattern) = nullptr;
    // 0x3b5c0(request): start the "Trophy Task Thread" worker. The request carries all 7 arguments
    // exactly as the original frame does; the worker itself is not native.
    bool (*thread_create)(const OwnerA597D0ThreadRequest& request) = nullptr;
    // 0xba40(0x32f8b90, *(0x32f8bf0), &this->0x2b): publish the mode byte. The host owns both
    // globals; `value` points at the object byte that was just set to 1.
    bool (*startup_mode_notify)(std::uint8_t* value) = nullptr;
};

struct OwnerA597D0Result {
    bool ok;
    const char* failed_at;
};

// Writes `object[0..0x958)` exactly as `0xa597d0` does. `object` must be writable for 0x958 B and
// 8-byte aligned. `ok == false` stops at the first failure and keeps the already-written prefix
// (the original writes that prefix before it would fault). The memory returned by `allocate` is
// not rolled back on failure - the original does not roll back either; the owning caller must run
// the documented `0xa59a10` teardown contract.
//
// `failed_at` values: "null-object", "object-align" (native preconditions), "alloc_block",
// "alloc_chunk", "event_flag", "thread_create", "startup_mode".
[[nodiscard]] OwnerA597D0Result owner_ctor_a597d0(void* object,
                                                  const OwnerA597D0Hooks& hooks) noexcept;

// Adapter for `owner_members`' context-free ctor hook
// (`bool (*)(std::size_t row, void* object, void* arg)`). `owner_members` passes no user pointer,
// so one binding is held at a time (single-threaded host contract). Bind before the install,
// unbind after. Only row 14 is accepted; any other row returns false without touching `object`.
struct OwnerRow14Binding {
    OwnerA597D0Hooks hooks{};
    OwnerA597D0Result last{false, nullptr};
    std::size_t calls = 0;
};

// `binding == nullptr` unbinds. The binding must outlive the install.
void owner_row14_bind(OwnerRow14Binding* binding) noexcept;

// `arg` is unused: `kOwnerMemberRows[14]` has no ctor argument.
[[nodiscard]] bool owner_row14_ctor_hook(std::size_t row, void* object, void* arg) noexcept;

}  // namespace sotc
