// CPU-01-STARTUP-BATCH-7: native model of startup constructor slot 506 (RVA 0x2340b0, 860 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 506. Every fact below is byte-pinned by tools/lift/check_ctor_slot_506.py (and the local unbounded-int
// twin by tools/lift/check_helper_233ec0.py). NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and
// destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order (the object block starts at 0x2ee94a0):
//   (1) UNBOUNDED int variable `drawTouch` (the twin constructor 0x233ec0 through helper_init_unbounded_int_twin_site, flags 1, value = default = 0,
//       empty description, caller functor tag 0x159b680 (the twin's private class: the object keeps it as +0xc0), object 0x2ee94a0, final class 0x159b618, atexit 0x230d60);
//   (2) UNBOUNDED int variable `checkTouch` (0x3a980 through helper_init_unbounded_int_var_site, flags 1, value = default = 0, empty description,
//       caller functor tag 0x159b6a0, object 0x2ee9578, atexit 0x18a60);
//   (3) a 0x26c10 group (item 0x2ee9650: empty parent, caption `Touch Manager`, flags 0x30) and one 0x26ea0 toggle `Draw` under `Touch Manager`
//       (item 0x2ee96f8, flags 0x30) whose target is variable (1).
// Variable (1)'s final class 0x159b618 answers the 0x26ea0 gate with slot 3 = slot 6 = 1, so the toggle keeps the {1.0, 10.0} default range.
// NOT established: what reads these variables at run time (the touch-manager reading is INFERENCE from the strings only). The __stack_chk_guard
// compare is a hook (not modelled).
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_slot_family_items.hpp"
#include "helper_233ec0_unbounded_int_twin.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot506Rva = 0x2340b0;
inline constexpr std::size_t kSlot506Size = 860;
inline constexpr std::uint64_t kSlot506Var0FnTag = 0x159b680;   // the twin's private class is also the caller's argument table
inline constexpr std::uint64_t kSlot506Var1FnTag = 0x159b6a0;
inline constexpr char kSlot506Var0Name[] = "drawTouch";
inline constexpr std::size_t kSlot506Var0NameLength = 9;
inline constexpr char kSlot506Var1Name[] = "checkTouch";
inline constexpr std::size_t kSlot506Var1NameLength = 10;
inline constexpr char kSlot506GroupCaption[] = "Touch Manager";
inline constexpr std::size_t kSlot506GroupCaptionLength = 13;
inline constexpr char kSlot506ItemText[] = "Draw";
inline constexpr std::size_t kSlot506ItemTextLength = 4;
inline constexpr std::uint32_t kSlot506ItemFlags = 0x30;
inline constexpr std::uint64_t kSlot506Var0Rva = 0x2ee94a0;
inline constexpr std::uint64_t kSlot506Var1Rva = 0x2ee9578;
inline constexpr std::uint64_t kSlot506GroupRva = 0x2ee9650;
inline constexpr std::uint64_t kSlot506ItemRva = 0x2ee96f8;

struct Slot506Objects {
    ConCmdRangedVar var0;       // 0x2ee94a0
    ConCmdRangedVar var1;       // 0x2ee9578
    DebugMenuItem group;        // 0x2ee9650
    DebugMenuItem item;         // 0x2ee96f8
};

struct Slot506Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // the first wrapper refusal, if any
    HelperIntSiteResult var0{};
    HelperIntSiteResult var1{};
    FamilyItemResult group{};
    MenuSiteResult item{};
};

[[nodiscard]] Slot506Result init_slot506_2340b0(Slot506Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
