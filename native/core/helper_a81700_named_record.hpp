// CPU-01-HELPER-CALLEE-MODELS-03: native model of the named-record constructor/registrar 0xa81700 (190 B, FDE 190) and the
// two bodies it reaches, 0xa81840 (265 B, the two-vector push) and 0xa82050 (245 B, vector<T*>::reserve(n)); ELF sha256
// d0723afb...5861d3. Called 13 times by startup slot 513 (0x1d0770) and 7 times by slot 497 (0x262a80); a whole-image scan of
// [0, 0xf90000) finds 48 direct call sites in all (the other 28 belong to bodies outside this task). Byte-pinned by tools/lift/check_helper_a81700.py. Nothing here executes or calls an
// original RVA: the allocator pair is the project's AllocDispatchContract hook (0x17070 / 0x170b0), the C++ guard and the
// exit-time destructor 0xa81b80 are identity/pending records.
//
//   0xa81700(rdi = record, rsi = const SharedStr* name, edx = kind, cl = flag)
//     record+0x00..0x1f  = *name (16 B view {data,size}, aux, ctl), then `lock inc qword [ctl]` when ctl != 0
//     record+0x20 dword  = kind        record+0x28 qword = 0
//     record+0x30 byte   = 0           record+0x31 byte  = flag        (+0x24 and +0x32..0x37 are never written)
//     then the process registry (the function-local static at 0x32fd398, guard byte 0x33053f0):
//       first use: zero it exactly like 0xa81b50 (+0x08 16 B, +0x18, +0x28 32 B, +0x50 dword), then
//                  __cxa_atexit(0xa81b80, &registry, DSO)            -> reported as `registry_initialised_now`
//       tail-jmp 0xa81840(registry, record):
//         vector A  (begin +0x08, end +0x10, cap +0x18): if end == cap { reserve(1) } ; *end++ = record
//         vector B  (begin +0x28, end +0x30, cap +0x38): the same
//   0xa82050(vec, n) with n = 1 (libc++-style, growth 1.5x): when (cap - end) / 8 >= n nothing; else size = (end-begin)/8,
//     if (max - size < n) -> 0xa82150 (calls the import 0xf8fc10 with a message at 0x12c6bf0, then ud2: never returns; INFERENCE: throws std::length_error) ; want = size + n ;
//     new = capacity / 2 ; if (max - new < capacity) new = 0 else new += capacity ; new = max(new, want) ;
//     if (new > max) -> import 0xf8f5f0 (NID eT2UsmTewbU, name not recovered, then ud2: never returns; INFERENCE: bad_alloc) ; buf = 0x17070(new * 8) ; memmove(buf, begin, end - begin) ;
//     if (begin) 0x170b0(begin) ; begin = buf, end = buf + size, cap = buf + new.
//   Capacities from empty therefore run 1, 2, 3, 4, 6, 9, 13, 19, ...
//   0xa81840 also copes with a push of an element that lives inside the vector (begin <= &local < end); the pushed value is a
//   stack copy of the record pointer, so that branch is unreachable here and is NOT modelled.
//
// The two vectors both hold the record pointers; why there are two (INFERENCE only: an ordered list and a lookup list) and
// what the records mean (kind / flag) is not recovered. The records' exit-time unregistration (0xa81950 / 0xa819f0) and the
// registry destructor 0xa81b80 are NOT modelled. The original performs no allocator or length checks of its own beyond
// the two throws: a null allocation faults there, a missing/failed allocation here returns a refusal and leaves the vector
// untouched. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kNamedRecordCtorRva = 0xa81700;
inline constexpr std::size_t kNamedRecordCtorSize = 190;
inline constexpr std::uint64_t kRecordRegistryPushRva = 0xa81840;
inline constexpr std::size_t kRecordRegistryPushSize = 265;
inline constexpr std::uint64_t kVecReserveRva = 0xa82050;
inline constexpr std::size_t kVecReserveSize = 245;
inline constexpr std::uint64_t kRecordRegistryCtorRva = 0xa81b50;
inline constexpr std::size_t kRecordRegistryCtorSize = 34;
inline constexpr std::uint64_t kRecordRegistryDtorRva = 0xa81b80;     // atexit target on first use: identity only
inline constexpr std::uint64_t kRecordRegistryObjectRva = 0x32fd398;
inline constexpr std::uint64_t kRecordRegistryGuardRva = 0x33053f0;
inline constexpr std::size_t kNamedRecordStride = 0x38;
inline constexpr std::uint64_t kVecMaxElements = 0x1fffffffffffffffULL;

struct NamedRecord;

struct RecordPtrVec {
    NamedRecord** begin;      // +0
    NamedRecord** end;        // +8
    NamedRecord** cap;        // +0x10
};
static_assert(sizeof(RecordPtrVec) == 0x18);

// The named record (0x38 B stride at the call sites).
struct NamedRecord {
    SharedStr name;               // +0x00 .. +0x1f   view copy, ctl incremented
    std::uint32_t kind;           // +0x20            edx
    std::uint8_t gap24[4];        // never written
    std::uint64_t q28;            // +0x28            = 0
    std::uint8_t b30;             // +0x30            = 0
    std::uint8_t flag;            // +0x31            cl
    std::uint8_t gap32[6];        // never written
};
static_assert(sizeof(NamedRecord) == 0x38 && offsetof(NamedRecord, kind) == 0x20 && offsetof(NamedRecord, q28) == 0x28 &&
              offsetof(NamedRecord, b30) == 0x30 && offsetof(NamedRecord, flag) == 0x31);

// The process registry object at 0x32fd398 (0x54 live bytes).
struct RecordRegistry32fd398 {
    std::uint64_t q00;            // +0x00 never written
    RecordPtrVec first;           // +0x08 .. +0x1f
    std::uint64_t q20;            // +0x20 never written
    RecordPtrVec second;          // +0x28 .. +0x3f
    std::uint64_t q40;            // +0x40 zeroed by the constructor (part of the 32 B store at +0x28)
    std::uint8_t gap48[8];        // never written
    std::uint32_t d50;            // +0x50 zeroed
    std::uint8_t gap54[4];
};
static_assert(sizeof(RecordRegistry32fd398) == 0x58 && offsetof(RecordRegistry32fd398, first) == 0x08 &&
              offsetof(RecordRegistry32fd398, second) == 0x28 && offsetof(RecordRegistry32fd398, q40) == 0x40 &&
              offsetof(RecordRegistry32fd398, d50) == 0x50);

// The registry plus its C++ guard byte (0x33053f0). A zero-initialised cell is the original's BSS state.
struct RecordRegistryCell {
    RecordRegistry32fd398 registry;
    std::uint8_t guard;
};

enum class RecordRegistryRefusal : std::uint8_t {
    none,
    no_allocator,          // growth needed and the allocate hook is missing (or no deallocate hook for the old block)
    allocation_failed,     // the allocate hook returned null
    length_error,          // max - size < n : the original calls 0xa82150, which never returns (INFERENCE: throws std::length_error)
    bad_alloc,             // new capacity > max : the original calls the import 0xf8f5f0, which never returns (INFERENCE: bad_alloc)
    free_failed,           // the old block's deallocate hook reported failure (state updated, block leaked)
};

// 0xa81b50: the zeroing constructor (also the inline first-use initialisation inside 0xa81700). Zeroes only the bytes the
// original zeroes.
void construct_record_registry_a81b50(RecordRegistry32fd398& registry) noexcept;

// 0xa82050(vec, n): reserve room for `n` more elements (the only caller passes 1). `grew` reports a reallocation.
struct VecReserveResult {
    bool ok = false;
    RecordRegistryRefusal refusal = RecordRegistryRefusal::none;
    bool grew = false;
    std::size_t new_capacity = 0;     // elements, valid when grew
};
[[nodiscard]] VecReserveResult vec_reserve_a82050(RecordPtrVec& vec, std::size_t n, const AllocDispatchContract& alloc) noexcept;

struct NamedRecordRegistration {
    bool ok = false;
    RecordRegistryRefusal refusal = RecordRegistryRefusal::none;
    bool registry_initialised_now = false;   // the guard was clear: the registry was zeroed and atexit(0xa81b80) is pending
    bool pushed_first = false;
    bool pushed_second = false;
    std::uint32_t grows = 0;                 // reallocations performed (0, 1 or 2)
};

// 0xa81700. The record is always written (the original writes it before anything can fail). On a refusal the vector that
// failed is unchanged; if the first vector was already pushed it stays pushed (the original has no rollback either).
[[nodiscard]] NamedRecordRegistration register_named_record_a81700(RecordRegistryCell& cell, NamedRecord& record,
                                                                    const SharedStr& name, std::uint32_t kind,
                                                                    std::uint8_t flag, const AllocDispatchContract& alloc) noexcept;

// The process-wide cell (the original's BSS object). Tests use their own cells.
[[nodiscard]] RecordRegistryCell& record_registry_cell_32fd398() noexcept;

}  // namespace sotc
