// Row-9 owner constructor `0xa5c850` (`kOwnerMemberRows[9]`, 0xa530 B heap object, pointer slot
// `0x32f8ff0`).
//
// Characterization: `local/re/evidence/owner-ctor-a5c850/report.md` (offline static decode only).
// Body `0xa5c850..0xa5cb21`, size `0x2d1` (721 B), sha256
// `74b32f4fc7ffb86c4a3fc3018fef5ea3e37230e6093e1c5f95b70f837a754bb1`, 119 instructions in three
// basic blocks (B0 entry, B1 a 256-trip loop, B2 tail), one branch, five call sites / four targets.
//
// What is native here:
//   - 56 `this`-relative store sites (55 plain + one sequentially consistent atomic) for the head
//     scalars, the six inline small-vector headers, the lane group and the constant blocks;
//   - 7 store sites inside the `0x1c50..0x5c50` array, executed for all 256 records (stride 0x40);
//   - 2 absolute stores that copy the 24 B "Sound" record into the global at `0x32f97f8`
//     (8 B at `+0x10`, then 16 B at `+0x00`; the global is 8-aligned, hence the split).
//
// What stays a hook (report §4, §6): `0xb09650` (tree header), `0x3b3d0` twice
// (`sceKernelCreateEventFlag`, GOT slot `0x1600298`, NID `BpFoboUJoZU` - NOT a mutex) and
// `0x11bc0` (the `"Sound"` name-registry lookup with `create == false`). `0xf180` is already
// native and is reused from `init_sub_3be930`.
//
// Fidelity rules that the fixture enforces:
//   1. No `memset`/zero-fill. Only the characterized offsets are written; the holes of report §8
//      stay untouched (`+0x000c..+0x000f`, `+0x0011..+0x0017`, `+0x0019..+0x001f`,
//      `+0x0021..+0x0027`, every vector's `+0x18` slot, the six inline buffers, each record's
//      `+0x08..+0x2b/+0x30..+0x33` (+0x38 is a qword store), the `0x60`-stride lanes, `+0x7270` and
//      `+0xa524..+0xa530`). The original object is partially initialized; "helpful" zeroing
//      would diverge from it.
//   2. `+0x0010` is a one-byte store (report R1). The upper seven bytes are never touched, even
//      though the destructor reads that field as a qword.
//   3. The body never reads `this` (every `[rbx+...]` operand is a store destination, and the
//      atomic's loaded value is discarded), so a second call writes the same bytes - but it also
//      allocates again and creates two more kernel event flags, so a row install must call it
//      once (report R5).
//
// Not modelled: the destructor `0xa5cb30` (size `0x252`, `kDtorThenFree` pairing) is a recorded
// contract only. See report §9; its teardown order is not reproduced here.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kOwnerCtorA5C850Rva = 0xA5C850;
inline constexpr std::size_t kOwnerCtorA5C850Size = 0x2D1;
inline constexpr std::size_t kOwnerCtorA5C850ObjSize = 0xA530;
inline constexpr std::uint64_t kOwnerCtorA5C850SlotRva = 0x32F8FF0;   // pointer slot, not the object
inline constexpr std::uint64_t kOwnerCtorA5C850SoundRva = 0x32F97F8;  // 24 B global record

// Destructor contract only (report §9): `0xa5cb30..0xa5cd81`, `0x252` B, row teardown is
// dtor then `0x170b0`. Deliberately not modelled.
inline constexpr std::uint64_t kOwnerDtorA5CB30Rva = 0xA5CB30;
inline constexpr std::size_t kOwnerDtorA5CB30Size = 0x252;

inline constexpr std::size_t kOwnerA5C850VecCount = 6;
inline constexpr std::size_t kOwnerA5C850RecCount = 0x100;
inline constexpr std::size_t kOwnerA5C850RecStride = 0x40;
inline constexpr std::size_t kOwnerA5C850RecBase = 0x1C50;
inline constexpr std::size_t kOwnerA5C850RecLimit = 0x5C50;  // == RecBase + RecCount * RecStride
inline constexpr std::size_t kOwnerA5C850LaneCount = 0x20;
inline constexpr std::size_t kOwnerA5C850LaneStride = 0x60;
inline constexpr std::size_t kOwnerA5C850LaneArrayOffset = 0x5CA0;  // 32 lanes * 0x60 = 0xC00 B

// Structural offsets used by the body and the fixture (report §5).
inline constexpr std::size_t kOwnerA5C850LaneGroupOffset = 0x5C50;  // u32 scalar
inline constexpr std::size_t kOwnerA5C850LaneAtomicOffset = 0x5C60;  // xchg qword, old value dropped
inline constexpr std::size_t kOwnerA5C850ConstBlock0Offset = 0x69CC;  // 48 B, {1.0f,0,0,0} x3
inline constexpr std::size_t kOwnerA5C850ConstBlock1Offset = 0x69FC;  // 48 B, same
inline constexpr std::size_t kOwnerA5C850ConstZeroOffset = 0x6A2C;   // 16 B zero
inline constexpr std::size_t kOwnerA5C850OnesOffset = 0x6A3C;         // 32 x 1.0f
inline constexpr std::size_t kOwnerA5C850OnesCount = 0x20;
inline constexpr std::size_t kOwnerA5C850SharedOffset = 0x7250;   // 0x20 B empty shared handle
inline constexpr std::size_t kOwnerA5C850TreeOffset = 0x7270;     // 0xb09650 target
inline constexpr std::size_t kOwnerA5C850TreeBodyOffset = 0x7288;  // 32 B zero
inline constexpr std::size_t kOwnerA5C850PolyOffset = 0xA4F8;     // qword 0; dtor calls [vptr+8]
inline constexpr std::size_t kOwnerA5C850Event0Offset = 0xA500;   // 0x10 B event flag
inline constexpr std::size_t kOwnerA5C850Event1Offset = 0xA510;   // 0x10 B event flag
inline constexpr std::size_t kOwnerA5C850TailOffset = 0xA520;     // u32 0; +0xa524.. is a hole

// 0x3b3d0 contract: sceKernelCreateEventFlag(&h, "PS4SyncEvent", 0x21, pattern, NULL); the object
// is {qword handle, u8 flag} and the byte at +8 is 0 for both calls. Attr 0x21 reads as
// 0x01 FIFO | 0x20 MULTI under the SDK attribute convention (report §0, inference).
inline constexpr const char* kOwnerA5C850EventFlagName = "PS4SyncEvent";
inline constexpr std::uint32_t kOwnerA5C850EventFlagAttr = 0x21;
inline constexpr std::uint32_t kOwnerA5C850EventPattern0 = 0;
inline constexpr std::uint32_t kOwnerA5C850EventPattern1 = 1;

// 0x11bc0 is called with name1 == name2 == "Sound" and create == 0.
inline constexpr const char* kOwnerA5C850SoundName = "Sound";
inline constexpr std::size_t kOwnerA5C850SoundRecordBytes = 24;
inline constexpr std::uint64_t kOwnerA5C850SoundLiteralRva = 0x12C4D1B;

// One inline small-vector header (report §5.2). `header + 0x18` is the fourth slot and is never
// written by `0xa5c850`; `header + 0x28 == buffer` for all six.
struct OwnerA5C850Vector {
    std::size_t header;
    std::size_t buffer;
    std::size_t cap_end;
    std::size_t elem;  // 0 => element size unproven (the +0x68a0 buffer has no dtor loop)
};

extern const OwnerA5C850Vector kOwnerA5C850Vectors[kOwnerA5C850VecCount];

struct OwnerA5C850Hooks {
    // 0xb09650(at): new(0x88) + three self-links + byte +0x18 = +0x19 = 1, stored at at+8; the
    // callee also zeroes at+8..at+0x18 first. Null or false => visible failure.
    bool (*tree_init)(void* at) = nullptr;
    // 0x3b3d0(at, initial_pattern): sceKernelCreateEventFlag(&h, "PS4SyncEvent", 0x21, pattern,
    // NULL); [at] = h; byte [at+8] = 0. Null or false => visible failure.
    bool (*event_flag_create)(void* at, std::uint32_t initial_pattern) = nullptr;
    // 0x11bc0(name1, name2, create) -> pointer to a stable 24 B {u32 id, ptr, ptr} record.
    // Called with ("Sound", "Sound", false). A missing entry with create == false must return
    // nullptr; the lift then fails at "name_record" instead of inventing a record (report R4).
    const void* (*name_record)(const char* name1, const char* name2, bool create) = nullptr;
    // Host stand-in for the zeroed static `0x2dd9f78` that 0xf180 stores. Null selects the
    // existing `slot_vtable_2dd9f78()` stand-in, so the 0xf180 reuse stays dependency-free.
    // A hook that returns null fails at "null_payload".
    void* (*null_payload)() = nullptr;
};

struct OwnerA5C850Result {
    bool ok;
    const char* failed_at;
};

// Writes `object[0..0xa530)` exactly as `0xa5c850` does, then copies the 24 B name record into
// `sound_record[0..24)`. `object` must be writable for 0xa530 B and 8-byte aligned (the atomic at
// +0x5c60); `sound_record` must be writable for 24 B. `ok == false` stops at the first failure and
// keeps the already-written prefix (the original would have written it before faulting).
//
// `failed_at` values: "null-object", "sound-record", "object-align" (native preconditions),
// "null_payload" (native-only guard for the 0xf180 stand-in), "tree_init", "event_flag_0",
// "event_flag_1", "name_record".
[[nodiscard]] OwnerA5C850Result owner_ctor_a5c850(void* object, std::uint8_t* sound_record,
                                                  const OwnerA5C850Hooks& hooks) noexcept;

}  // namespace sotc
