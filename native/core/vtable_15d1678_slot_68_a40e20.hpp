// Resolved vtable target `0xa40e20` (reached from vtable `0x15d1678` slot `0x68`).
//
// Evidence: `0xa3dfda` is `call *0x68(%rax)` with `%rax` from the table `0x15d1678`; the qword at
// slot `0x68` is `0xa40e20`, the relocation at `0x15d16e0` is type 8 with addend `0xa40e20`, and
// `0xa40e20` is an FDE function start of length `0x1`.
//
// Body (1 B, `c3`): `retq`. An empty virtual hook: it reads nothing, writes nothing, returns
// nothing. Modelling it as an explicit no-op keeps the call site honest instead of pretending the
// call has an effect.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable15d1678Slot68_a40e20Rva = 0xa40e20;
inline constexpr std::size_t kVtable15d1678Slot68_a40e20Size = 0x1;
inline constexpr std::uint64_t kVtable15d1678Slot68_a40e20EndAddress = 0xa40e21;
inline constexpr std::uint64_t kVtable15d1678Slot68_a40e20VtableAddress = 0x15d1678;
inline constexpr std::uint32_t kVtable15d1678Slot68_a40e20SlotOffset = 0x68;
inline constexpr std::uint64_t kVtable15d1678Slot68_a40e20RelocAddress = 0x15d16e0;

struct Vtable15d1678Slot68_a40e20Result {
    bool ok = false;
    std::uint32_t calls = 0;
};

// A pure no-op: returns ok with the call counted.
[[nodiscard]] Vtable15d1678Slot68_a40e20Result vtable_15d1678_slot_68_a40e20() noexcept;

}  // namespace sotc
