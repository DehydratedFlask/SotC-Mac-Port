// Native recovery of original constructor-table index 303 / RVA 0xa82170 (ELF sha256
// d0723afb...5861d3). 455-byte static initialiser of the `ProfileCPU` console variable plus its
// debug-menu toggle. Two objects:
//
//   A  0x32fd228  bool variable (shared bool-var shape, core/concmd_vars)
//        name view {"ProfileCPU" (0x12c6b81), len 0xa, 0, 0}
//        description view {0x12c6bef -> a NUL byte, len 0, 0, 0}
//        [A] = 0x1594218; call 0x20af0(A, name, description, 1)      ; ConCmd base constructor
//        [A] = 0x15940b0; [A+0x60] = A+0xc0; dword [A+0x69] = 0; zero [A+0x70..0x8f];
//        qword [A+0xa0] = 0; qword [A+0xb0] = 0x15d2478; qword [A+0xb8] = 0
//        call 0xf590(description)                                     ; inert
//        [A] = 0x1594218; byte [A+0xc0] = 0; byte [A+0x68] = 0       ; value 0 / default 0
//        call 0x20d60(A)                                              ; register (pending list)
//        call 0xf590(name)                                            ; inert
//        __cxa_atexit(0x20b0, A, 0x1604218)
//   B  0x32fd2f0  debug-menu toggle item (0x26ea0), directly after A's value byte
//        label view {"Profile" (0x12c6b8c), len 7}, text view {"Profile CPU" (0x12c6b94), len 0xb},
//        target A, flags 0x30; the lifted bool-class virtual slots return {1, 0}, so the default
//        range {1.0, 10.0} is kept; registered with a null parent (root list 0x2dfd4c0, whose
//        atexit 0x27280 was already recorded by slot 299, so it is not recorded again here)
//        call 0xf590(text); call 0xf590(label); __cxa_atexit(0x272d0, B, 0x1604218)
//
// This is the third observed site of the bool-variable shape: only the functor tag (0x15d2478) and
// the stored value/default (0 here, 1 at slots 301 and 299-B) differ, which is why init_bool_var_
// static takes them as parameters. The functor vtable 0x15d2478 is an identity tag only; its slot
// bodies are not lifted and not needed, because the destroy slot is reached only through the
// registered destructor 0x20b0, which is pinned and never executed.
//
// Why the toggle is never live at this point: the debug-menu parent cell [0x2dfd4b8] is null at
// startup (see debug_menu_item.hpp), so the item is only appended to the root list. The value has
// no native consumer yet; no profiling, timing or menu behaviour is claimed.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot303Rva = 0xa82170;
inline constexpr std::size_t kSlot303BodySize = 455;
inline constexpr std::uint64_t kSlot303VarRva = 0x32fd228;
inline constexpr std::uint64_t kSlot303ItemRva = 0x32fd2f0;
inline constexpr std::uint64_t kSlot303FnVtbl = 0x15d2478;     // functor vtable tag (identity only)
inline constexpr std::uint8_t kSlot303Value = 0;               // byte +0xc0
inline constexpr std::uint8_t kSlot303Default = 0;             // byte +0x68
inline constexpr std::uint32_t kSlot303ItemFlags = 0x30;
inline constexpr char kSlot303Name[] = "ProfileCPU";           // 0x12c6b81, length 10
inline constexpr char kSlot303Label[] = "Profile";             // 0x12c6b8c, length 7
inline constexpr char kSlot303Text[] = "Profile CPU";          // 0x12c6b94, length 11

struct Slot303Objects {
    ConCmdBoolVar var{};       // A, 0x32fd228
    DebugMenuItem item{};      // B, 0x32fd2f0
};

struct Slot303Result {
    bool ok = false;
    BoolVarResult var{};
    bool item_ctor_ok = false;
    DebugMenuRegisterResult item_registration{};
    bool item_views_inert = false;
    bool item_atexit_recorded = false;
};

// Models 0xa82170 against the shared ConCmd registry and the debug-menu registry. `ok == false` is
// a visible failure (registration contract missing, a view carrying a control counter, an
// unmodelled 0x268b0 branch, or an atexit record that could not be allocated).
[[nodiscard]] Slot303Result init_slot303_a82170(Slot303Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
