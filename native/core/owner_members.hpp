// Native driver for the `0x2569d0` member-install rows (region A, E570/E579).
//
// The original performs 20 `new` calls in execution order (rows 1..21; row 11 allocates inside its
// factory), stores 19 results into globals / `this` fields, and hands two to self-registering
// constructors. Replace/teardown uses three byte-distinct idioms:
//   (a) virtual deleting dtor at `[old_vptr+8]`, no `0x170b0` (rows 1,2,3,5,6,11,16);
//   (b) direct dtor call then `0x170b0(old)` (rows 9,10,12,13,14,15,17,18,19,20,21);
//   (c) no replace check at all (row 7 only); the new object is stored unconditionally.
//
// This module models the allocation/ctor/replace sequence, not the ctor bodies. Every ctor is a
// host hook; a missing hook is a visible failure and never synthesises a body. Row 2 has no ctor
// function (the body is only a vptr store), so it also needs a host ctor hook. Rows 4 and 8
// receive `&self_cell` and are not stored.
//
// Arg sources (rows 3, 17, 18): `[0x2ee2d00]`, `[0x2edf140]`, `[0x2edf140]`. Rows 4/8 receive
// `&0x2eec4d8` / `&0x2eec4e0`. The state maps hold native pointers; no original RVA is executed.
//
// NOT modelled: the 18 ctor bodies, the post-construct wiring between rows 9 and 12, the mode/gate
// dispatch after region A, the registry block (`registry_slot`, done) and the property rows
// (`apply_owner_properties`, done). G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>
#include <map>

namespace sotc {

inline constexpr std::size_t kOwnerMemberRowCount = 21;
inline constexpr std::uint64_t kOwnerMembersStartRva = 0x256A15;
inline constexpr std::uint64_t kOwnerMembersEndRva = 0x2570A9;
inline constexpr std::uint64_t kOwnerMembersAllocRva = 0x17070;
inline constexpr std::uint64_t kOwnerMembersFreeRva = 0x170B0;

enum class OwnerRowDest : std::uint8_t {
    kGlobal = 0,        // store into a BSS/global cell (RVA)
    kThisField = 1,     // store into this + offset
    kSelfRegistered = 2,  // ctor receives &cell and installs itself; no store
};

enum class OwnerRowDestroy : std::uint8_t {
    kVirtualDtor = 0,  // call [old_vptr+8]; that dtor frees
    kDtorThenFree = 1, // call the direct dtor, then 0x170b0(old)
    kNone = 2,         // row 7: no replace check, nothing destroyed
};

enum class OwnerRowArg : std::uint8_t {
    kNone = 0,
    kGlobal = 1,   // arg = state.globals[arg_rva]
    kSelfCell = 2, // arg = &state.self_cells[arg_rva]
};

struct OwnerMemberRow {
    std::uint32_t size;      // immediate to edi (row 11: inside the factory)
    std::uint64_t new_site;  // evidence only
    std::uint64_t ctor_rva;  // evidence only; 0 when the body is only a vptr store
    OwnerRowDest dest_kind;
    std::uint64_t dest;      // global RVA or this offset
    OwnerRowDestroy destroy;
    OwnerRowArg arg_kind;
    std::uint64_t arg_rva;   // global RVA or self-cell RVA
};

extern const OwnerMemberRow kOwnerMemberRows[kOwnerMemberRowCount];

struct OwnerMemberState {
    std::map<std::uint64_t, void*> globals;      // keyed by RVA
    std::map<std::uint64_t, void*> self_cells;   // keyed by RVA; the cell address is the arg
    std::map<std::size_t, void*> this_fields;    // keyed by this offset
};

struct OwnerMemberHooks {
    // 0x17070(size). Null => visible failure before any row.
    void* (*allocate)(std::size_t size) = nullptr;
    // 0x170b0(ptr). Null => visible failure before any row.
    bool (*deallocate)(void* ptr) = nullptr;
    // One per row; `row` is 1-based. Return false stops the install and frees the new object.
    bool (*ctor)(std::size_t row, void* object, void* arg) = nullptr;
    // Idiom (a): the deleting destructor frees the object. Null when a virtual row needs it is a
    // visible failure.
    void (*virtual_dtor)(void* object) = nullptr;
    // Idiom (b): direct destructor, followed by deallocate. Null when needed is a visible failure.
    void (*dtor)(void* object) = nullptr;
};

struct OwnerMembersResult {
    bool ok = false;
    int installed = 0;           // slot rows stored (globals + this fields)
    int self_registered = 0;     // rows 4 and 8
    int replaced = 0;            // stores where the previous value was non-null
    int virtual_destroyed = 0;   // idiom (a) calls
    int dtor_destroyed = 0;      // idiom (b) direct dtor calls
    int freed = 0;               // 0x170b0 calls after idiom (b)
    int stopped_row = 0;         // 1-based failing row, 0 on success
};

// Runs the 21 rows in order. A missing allocator/deallocator fails before row 1. A missing ctor or
// destroy hook, an allocation failure, or a false return stops at that row, frees the newly
// allocated object with the deallocator, and leaves the previous value in place.
[[nodiscard]] OwnerMembersResult init_owner_members(
    OwnerMemberState& state, const OwnerMemberHooks& hooks) noexcept;

}  // namespace sotc
