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

constexpr bool registration_thunk_table_is_consistent() noexcept {
    for (std::size_t i = 0; i < kRegistrationThunkDescriptors.size(); ++i) {
        const RegistrationThunkDescriptor& d = kRegistrationThunkDescriptors[i];
        if (d.ctor_index != kFirstRegistrationThunkIndex + i) return false;
        for (std::size_t j = 0; j < i; ++j) {
            const RegistrationThunkDescriptor& e = kRegistrationThunkDescriptors[j];
            if (d.original_thunk_rva == e.original_thunk_rva || d.original_vtable_rva == e.original_vtable_rva ||
                d.original_global_rva == e.original_global_rva)
                return false;
        }
    }
    return true;
}
static_assert(registration_thunk_table_is_consistent());

// Runs the native counterpart of constructor-table slot `ctor_index` (12..19): the shared base-object
// probe, then DescriptorRegistration::publish(&kRegistrationThunkDescriptors[ctor_index - 12]) on
// StartupState::thunk_registrations. Idempotent for the same descriptor (returns true). Returns false
// and sets state.init_failed when `ctor_index` is outside 12..19, when a different descriptor already
// occupies the registration, or when the owned mutex cannot be initialised.
[[nodiscard]] bool init_registration_thunk(StartupState& state, std::uint32_t ctor_index) noexcept;

}  // namespace sotc
