// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 488 (RVA 0x2ba5d0, 1028 bytes). Every fact
// below is byte-pinned by tools/lift/check_ctor_slot_488.py. NOTHING here executes, loads or calls an original RVA: vtable
// pointers, the functor tag and the destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) RANGED int variable `SM_showActiveState` (0xc580 through helper_c580_ranged_int, range 0..2, flags 1,
// value = default = 0, empty description, caller functor tag 0x15a0a18, object 0x2eef788); (2) a 0x30 B static holder (object
// 0x2eef860) built inline: class pointer 0x15a0a38 + 0x10, a borrowed text view `SM_debugIdentifier` (0x12 bytes) at +8, 0x18 zero
// bytes, then f590 of the temporary view and __cxa_atexit(0x2b5050, holder) (recorded, never run); (3) UNBOUNDED int variable
// `SM_showActiveStateMinTime` (0x3a980 through helper_3a980_unbounded_int, flags 1, value = default = 0, object 0x2eef890);
// (4) UNBOUNDED int variable `SM_showActiveStateMaxTime` (object 0x2eef968), same tag. No debug-menu item. The
// __stack_chk_guard compare is a hook (not modelled).
//
// NOT established: what the 0x15a0a38 class does or what reads the three variables at run time. "State-machine debug" reading
// from the strings is INFERENCE only; no consumer is lifted.
#pragma once

#include <cstddef>
#include <cstdint>

#include "helper_int_var_sites.hpp"
#include "lane_m4_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot488Rva = 0x2ba5d0;
inline constexpr std::size_t kSlot488Size = 1028;
inline constexpr std::uint64_t kSlot488IntFnTag = 0x15a0a18;
inline constexpr std::uint64_t kSlot488HolderVtbl = 0x15a0a48;       // class table 0x15a0a38 + 0x10 (identity tag)
inline constexpr std::uint64_t kSlot488HolderDtorRva = 0x2b5050;
inline constexpr char kSlot488Var0Name[] = "SM_showActiveState";
inline constexpr std::size_t kSlot488Var0NameLength = 0x12;
inline constexpr char kSlot488HolderText[] = "SM_debugIdentifier";
inline constexpr std::size_t kSlot488HolderTextLength = 0x12;
inline constexpr char kSlot488Var1Name[] = "SM_showActiveStateMinTime";
inline constexpr std::size_t kSlot488Var1NameLength = 0x19;
inline constexpr char kSlot488Var2Name[] = "SM_showActiveStateMaxTime";
inline constexpr std::size_t kSlot488Var2NameLength = 0x19;
inline constexpr std::int32_t kSlot488Var0Min = 0;
inline constexpr std::int32_t kSlot488Var0Max = 2;
inline constexpr std::uint64_t kSlot488Var0Rva = 0x2eef788;
inline constexpr std::uint64_t kSlot488HolderRva = 0x2eef860;
inline constexpr std::uint64_t kSlot488Var1Rva = 0x2eef890;
inline constexpr std::uint64_t kSlot488Var2Rva = 0x2eef968;

struct Slot488Holder {
    std::uint64_t vptr;                 // +0x00  kSlot488HolderVtbl (identity tag)
    const char* text;                   // +0x08  borrowed view data
    std::int64_t text_length;           // +0x10
    std::uint8_t zero[0x18];            // +0x18..+0x2f
};
static_assert(sizeof(Slot488Holder) == 0x30);

struct Slot488Objects {
    ConCmdRangedVar var0;               // 0x2eef788
    Slot488Holder holder;               // 0x2eef860
    ConCmdRangedVar var1;               // 0x2eef890
    ConCmdRangedVar var2;               // 0x2eef968
};

struct Slot488Result {
    bool ok = false;
    HelperIntSiteResult var0{};
    bool holder_stored = false;
    bool holder_views_inert = false;    // f590 of the temporary borrowed view was an exact no-op
    bool holder_atexit_recorded = false;   // 0x2b5050 recorded, not run
    HelperIntSiteResult var1{};
    HelperIntSiteResult var2{};
};

[[nodiscard]] Slot488Result init_slot488_2ba5d0(Slot488Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
