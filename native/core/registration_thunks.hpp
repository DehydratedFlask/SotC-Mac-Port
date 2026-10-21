// Native counterparts of constructor-table slots 12-19 (original RVAs 0xf06340, 0xf058f0, 0xf02ff0,
// 0xf01100, 0xf00090, 0xefe320, 0xefd020, 0xefacd0): eight "registration thunks" of the 34-slot
// descriptor + heap-mutex + atexit family (docs/CTOR_CLASSES.md).
//
// What the ORIGINAL thunk does (every item replayed from the pinned ELF bytes by
// tools/lift/check_ctor_thunks.py on the offline tracer tools/lift/thunk_trace.py):
//   1. builds and tears down a stack-local object of an UNIDENTIFIED class with owner = null and
//      field20 = field28 = 0x10 (so the class constructors skip their default state), and obtains the
//      class's vtable address point V -- directly (slots 12, 15), from an inline construct/destroy
//      callee that returns it (13, 19), or from the object after its constructor (14, 16, 17, 18, which
//      then destroy it through vtable slot 0). The tracer shows that probe touches nothing outside its
//      own stack frame;
//   2. stores V into .bss global A, malloc(0x10)s a block (size word [0x12d2d1c] == 0x10), stores the
//      pointer into A + 0x10, initialises a recursive (2) + priority-inherit (1) pthread mutex in it
//      through 0xb30950, and registers __cxa_atexit(0xe44a10, A + 0x10, 0x1604218).
//
// What the NATIVE thunk models: ONLY the registration. DescriptorRegistration owns the recursive +
// priority-inherit mutex (StartupState RAII, the settled slot-10 decision) and records an opaque
// RegistrationThunkDescriptor in place of V. The classes behind V are NOT identified, so there is no
// construct/destroy hook, no layout and no name here; the original heap block, its size word, the
// atexit ordering and V's numeric value are not reproduced (original numbers are identity only and are
// never used as addresses). The only class-level behaviour kept is the shared base-object probe
// (ClothingParamBase(nullptr, 0x10, 0x10) construct + destroy), which is what slots 12 and 15 do
// verbatim; the derived constructors/destructors of the other six are proven effect-free by the
// tracer and are intentionally not modelled.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sotc {

struct StartupState;

// How the original thunk builds the probe object it takes V from.
enum class RegistrationProbeShape : std::uint8_t {
    base_only,       // inline: base ctor (0xe1eda0), store V, base dtor (0xe1ede0)         -- slots 12, 15
    callee_inline,   // call a construct+destroy callee that returns V                      -- slots 13, 19
    vslot0_dtor,     // call the class ctor, read V from the object, destroy via [V]        -- slots 14, 16-18
};

// Opaque native stand-in for the descriptor the original publishes. All numbers are ORIGINAL
// identities (for tests and the ELF checker only); none is dispatched or dereferenced.
struct RegistrationThunkDescriptor {
    std::uint32_t ctor_index;            // position in the original 573-entry constructor list
    std::uint32_t original_thunk_rva;    // equals the ELF constructor-table entry for ctor_index
    std::uint32_t original_vtable_rva;   // V: address point stored into the first .bss global
    std::uint32_t original_global_rva;   // A: first .bss global; A + 0x10 held the heap mutex pointer
    RegistrationProbeShape probe_shape;
};

inline constexpr std::size_t kFirstRegistrationThunkIndex = 12;
inline constexpr std::size_t kRegistrationThunkCount = 8;

// Rows are parsed by tools/lift/check_ctor_thunks.py: keep the exact `{index, thunk, V, A, shape}` form.
inline constexpr std::array<RegistrationThunkDescriptor, kRegistrationThunkCount> kRegistrationThunkDescriptors = {{
    {12, 0xf06340, 0x15f9f00, 0x3355f88, RegistrationProbeShape::base_only},
    {13, 0xf058f0, 0x15f9298, 0x3355470, RegistrationProbeShape::callee_inline},
    {14, 0xf02ff0, 0x15f8670, 0x3355028, RegistrationProbeShape::vslot0_dtor},
    {15, 0xf01100, 0x15f7a28, 0x3354e00, RegistrationProbeShape::base_only},
    {16, 0xf00090, 0x15f6dc0, 0x3354b58, RegistrationProbeShape::vslot0_dtor},
    {17, 0xefe320, 0x15f63b8, 0x33549f0, RegistrationProbeShape::vslot0_dtor},
    {18, 0xefd020, 0x15f54f8, 0x3354660, RegistrationProbeShape::vslot0_dtor},
    {19, 0xefacd0, 0x15f4448, 0x3354190, RegistrationProbeShape::callee_inline},
}};

// ---------------------------------------------------------------------------------------------------
// REGISTERED-AHEAD slots 24-47 (CPU-01-NATIVE-SLOT24-47-01). Same contract, same descriptor type.
//
// These 24 further members of the 34-slot family (docs/CTOR_CLASSES.md) have native registration
// metadata, but are deliberately NOT part of the production constructor table (`recovered_constructors()`):
// constructor slot 23 (0xe71990) still needs the original name strings (open principal decision) and is
// the explicit blocker, so the production runner must keep stopping there. The focused test exercises
// the ahead table directly; `tools/lift/verify_ctor_family.py` replays the 34-slot body family, while
// `tools/lift/check_ctor_thunks.py` performs its detailed side-effect trace for slots 12-19. The
// `check_ctor_slots.py` checker validates the production list and ELF row identities; it does not scan
// the whole source tree for references. These rows are never part of the product startup path.
//
// Probe-shape census (all replayed from the bytes): base_only 19 (26, 29-46), callee_inline 3 (24, 25,
// 27), vslot0_dtor 2 (28, 47). Slot 47's virtual slot-0 destructor makes one extra direct call (0xe44cd0)
// that the tracer shows to be effect-free; no slot deviates from the family contract.
// ---------------------------------------------------------------------------------------------------
inline constexpr std::size_t kFirstAheadRegistrationIndex = 24;
inline constexpr std::size_t kAheadRegistrationCount = 24;

// Rows are parsed by tools/lift/check_ctor_thunks.py: keep the exact `{index, thunk, V, A, shape}` form.
inline constexpr std::array<RegistrationThunkDescriptor, kAheadRegistrationCount> kAheadRegistrationDescriptors = {{
    {24, 0xe56b70, 0x15ef440, 0x3353ec0, RegistrationProbeShape::callee_inline},
    {25, 0xe55f10, 0x15eeb90, 0x3353d90, RegistrationProbeShape::callee_inline},
    {26, 0xe55240, 0x15ee368, 0x3353ce8, RegistrationProbeShape::base_only},
    {27, 0xe54a30, 0x15edb20, 0x3353bf0, RegistrationProbeShape::callee_inline},
    {28, 0xe53a10, 0x15ed2b0, 0x3353b20, RegistrationProbeShape::vslot0_dtor},
    {29, 0xe52a20, 0x15ecad0, 0x3353ac8, RegistrationProbeShape::base_only},
    {30, 0xe52150, 0x15ec310, 0x3353a80, RegistrationProbeShape::base_only},
    {31, 0xe518e0, 0x15ebb70, 0x3353a40, RegistrationProbeShape::base_only},
    {32, 0xe510e0, 0x15eb410, 0x3353a10, RegistrationProbeShape::base_only},
    {33, 0xe50980, 0x15eac30, 0x33539b8, RegistrationProbeShape::base_only},
    {34, 0xe500b0, 0x15ea470, 0x3353970, RegistrationProbeShape::base_only},
    {35, 0xe4f840, 0x15e9cd0, 0x3353930, RegistrationProbeShape::base_only},
    {36, 0xe4f040, 0x15e9570, 0x3353900, RegistrationProbeShape::base_only},
    {37, 0xe4e8e0, 0x15e8d90, 0x33538a8, RegistrationProbeShape::base_only},
    {38, 0xe4e010, 0x15e85d0, 0x3353860, RegistrationProbeShape::base_only},
    {39, 0xe4d7a0, 0x15e7e30, 0x3353820, RegistrationProbeShape::base_only},
    {40, 0xe4cfa0, 0x15e76d0, 0x33537f0, RegistrationProbeShape::base_only},
    {41, 0xe4c840, 0x15e6ef0, 0x33537a0, RegistrationProbeShape::base_only},
    {42, 0xe4bf10, 0x15e6790, 0x3353770, RegistrationProbeShape::base_only},
    {43, 0xe4b7b0, 0x15e5ff0, 0x3353730, RegistrationProbeShape::base_only},
    {44, 0xe4afb0, 0x15e5888, 0x3353700, RegistrationProbeShape::base_only},
    {45, 0xe47670, 0x15e4c70, 0x33535c0, RegistrationProbeShape::base_only},
    {46, 0xe467e0, 0x15e40d0, 0x3353478, RegistrationProbeShape::base_only},
    {47, 0xe459e0, 0x15e37a8, 0x33533c8, RegistrationProbeShape::vslot0_dtor},
}};

template <std::size_t N>
constexpr bool registration_table_is_consistent(const std::array<RegistrationThunkDescriptor, N>& table,
                                                std::size_t first_index) noexcept {
    for (std::size_t i = 0; i < table.size(); ++i) {
        const RegistrationThunkDescriptor& d = table[i];
        if (d.ctor_index != first_index + i) return false;
        for (std::size_t j = 0; j < i; ++j) {
            const RegistrationThunkDescriptor& e = table[j];
            if (d.original_thunk_rva == e.original_thunk_rva || d.original_vtable_rva == e.original_vtable_rva ||
                d.original_global_rva == e.original_global_rva)
                return false;
        }
    }
    return true;
}

// No thunk, vtable or global is shared between the two tables either.
constexpr bool registration_tables_are_disjoint() noexcept {
    for (const RegistrationThunkDescriptor& a : kRegistrationThunkDescriptors)
        for (const RegistrationThunkDescriptor& b : kAheadRegistrationDescriptors)
            if (a.original_thunk_rva == b.original_thunk_rva || a.original_vtable_rva == b.original_vtable_rva ||
                a.original_global_rva == b.original_global_rva)
                return false;
    return true;
}

constexpr bool registration_thunk_table_is_consistent() noexcept {
    return registration_table_is_consistent(kRegistrationThunkDescriptors, kFirstRegistrationThunkIndex);
}
constexpr bool ahead_registration_table_is_consistent() noexcept {
    return registration_table_is_consistent(kAheadRegistrationDescriptors, kFirstAheadRegistrationIndex);
}
static_assert(registration_thunk_table_is_consistent());
static_assert(ahead_registration_table_is_consistent());
static_assert(registration_tables_are_disjoint());

// Runs the native counterpart of constructor-table slot `ctor_index` (12..19): the shared base-object
// probe, then DescriptorRegistration::publish(&kRegistrationThunkDescriptors[ctor_index - 12]) on
// StartupState::thunk_registrations. Idempotent for the same descriptor (returns true). Returns false
// and sets state.init_failed when `ctor_index` is outside 12..19, when a different descriptor already
// occupies the registration, or when the owned mutex cannot be initialised.
[[nodiscard]] bool init_registration_thunk(StartupState& state, std::uint32_t ctor_index) noexcept;

// Registered-ahead counterpart for slots 24..47 (see the block above): same shared base-object probe and
// DescriptorRegistration::publish(&kAheadRegistrationDescriptors[ctor_index - 24]) on
// StartupState::ahead_registrations. Same idempotence / failure contract as init_registration_thunk.
// NOT reachable from the production runner (slot 23 blocks it first).
[[nodiscard]] bool init_registration_thunk_ahead(StartupState& state, std::uint32_t ctor_index) noexcept;

}  // namespace sotc
