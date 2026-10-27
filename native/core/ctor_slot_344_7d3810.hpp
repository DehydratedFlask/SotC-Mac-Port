// CPU-01-STARTUP-SLOTS-343-356 (lane L4): native model of startup constructor slot 344 (RVA 0x7d3810, 706 bytes).
// Original ELF sha256 d0723afb...5861d3, constructor-table word at 0x2dda750 + (306 - 344) * 8. Every body fact
// below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_344.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are
// IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) two 0x20-byte cells at 0x325e8d0 / 0x325e8f0, each `qword = 0x3f80000000000001` and a
// locked word store of 0 at +0x10 (no atexit); (2) bool variable `ShowCinematicTime` (object 0x325e910,
// tag 0x15ce3b0, value 0, default 0); (3) a menu toggle through the NEW constructor 0x26db0 (item 0x325e9d8)
// under the parent path `Cutscene`, its caption taken from the variable's name; flags 0x30; f590(parent);
// atexit(0x272d0); (4) the command `cutscene_nextCut` (object 0x325ea80, handler tag 0x7d2c20, flags 2).
// The meaning of the two cells is NOT recovered (observation: constants above only).
#pragma once

#include "ctor_shapes_343_356.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot344Rva = 0x7d3810;
inline constexpr std::size_t kSlot344Size = 706;
inline constexpr char kSlot344VarName[] = "ShowCinematicTime";
inline constexpr char kSlot344Parent[] = "Cutscene";
inline constexpr std::uint64_t kSlot344FnTag = 0x15ce3b0;
inline constexpr char kSlot344CommandName[] = "cutscene_nextCut";
inline constexpr char kSlot344CommandDescription[] = "Skip to the next cut in the current cutscene";
inline constexpr std::size_t kSlot344CommandDescriptionLength = 0x2c;
inline constexpr std::uint64_t kSlot344HandlerTag = 0x7d2c20;

struct Slot344Objects {
    FlagScaleCell cell_a;    // 0x325e8d0
    FlagScaleCell cell_b;    // 0x325e8f0
    ConCmdBoolVar var;       // 0x325e910
    DebugMenuItem item;      // 0x325e9d8
    ConCmdCommand command;   // 0x325ea80
};

struct Slot344Result {
    bool ok = false;
    BoolVarResult var{};
    MenuSiteResult item{};
    FConCmdBuildResult command{};
};

[[nodiscard]] Slot344Result init_slot344_7d3810(Slot344Objects& objects, ConCmdRegistry& concmd,
                                                DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
