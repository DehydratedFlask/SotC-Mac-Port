// CPU-01-STARTUP-BATCH-10: native model of startup constructor slot 342 (RVA 0x7d8e10, 1070 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 342. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_342.py. NOTHING here executes,
// loads or calls an original RVA: vtables, functor tags, handler tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// This slot was flagged "1 other indirect call, 1 unmodelled helper" by the startup survey. Both are resolved statically:
//   * `callq *0x20(%rax)` at 0x7d91e8 (rax = 0x1594dc0) is the console-command register-once virtual slot [0x1594dc0 + 0x20] = 0x20d60 (the
//     `fcmd_vtable` extra pins that relocation), the same shape as slots 306 / 339 / 340 / 343 / 359. The two `callq *0x18(%rax)` (0x7d8fe7 /
//     0x7d90d5, rax = [rsp + 0x80] = the caller functor tag 0x15ce3f0) are the float functor destroy of the 68-table family.
//   * 0x23fd00 (x2) is the 86 byte name-cell initialiser, modelled as core/name_cell_23fd00 (NULL source here: hash 0, text "NULL").
//
// Shape (a cutscene debug page; the reading is INFERENCE from the strings). In order (object RVA):
//   b0   0x325ec38  bool variable `enableCutsceneDebugCommands` (empty description, flags 1, value = default 1, caller tag 0x15ce3d0), atexit 0x20b0
//   f0   0x325ed00  unbounded float `skipCutsceneHoldTime`             (flags 1, value = default 2.0f, caller tag 0x15ce3f0, early release), atexit 0x20f0
//   f1   0x325edd8  unbounded float `skipCutscenePromptDisplayTime`    (same shape, 2.0f), atexit 0x20f0
//   n0   0x325eeb0  HashedName20 initialised by 0x23fd00(cell, NULL)   (no atexit)
//   n1   0x325eec4  HashedName20 initialised by 0x23fd00(cell, NULL)   (no atexit)
//   cmd  0x325ef00  console command `cp10_skipCurCutsceneCut` (description `Skips the current cutscene cut.`, flags 2, handler tag 0x7d8d60 never called,
//                   vptr 0x1594dc0 + 0x10), built by init_fconcmd_object, atexit 0x4b0
// Registration order: b0, f0, f1, cmd. The 0x28 bytes 0x325eed8..0x325ef00 are never written. All strings are borrowed rodata views, so every 0xf590
// release is an exact no-op. The __stack_chk_guard compare is a hook. No consumer of the variables or of the cells is lifted.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_shapes_343_356.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"
#include "name_cell_23fd00.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot342Rva = 0x7d8e10;
inline constexpr std::size_t kSlot342Size = 1070;
inline constexpr std::uint64_t kSlot342BoolFnTag = 0x15ce3d0;
inline constexpr std::uint64_t kSlot342FloatFnTag = 0x15ce3f0;
inline constexpr std::uint64_t kSlot342HandlerTag = 0x7d8d60;
inline constexpr std::uint32_t kSlot342VarFlags = 1;

inline constexpr char kSlot342BoolName[] = "enableCutsceneDebugCommands";
inline constexpr std::size_t kSlot342BoolNameLength = 0x1b;
inline constexpr std::uint8_t kSlot342BoolValue = 1;
inline constexpr std::size_t kSlot342FloatCount = 2;
inline constexpr char kSlot342FloatNames[kSlot342FloatCount][32] = {"skipCutsceneHoldTime", "skipCutscenePromptDisplayTime"};
inline constexpr std::size_t kSlot342FloatNameLengths[kSlot342FloatCount] = {0x14, 0x1d};
inline constexpr std::uint32_t kSlot342FloatValueBits = 0x40000000;   // 2.0f (value = default)
inline constexpr std::size_t kSlot342NameCellCount = 2;
inline constexpr char kSlot342CommandName[] = "cp10_skipCurCutsceneCut";
inline constexpr char kSlot342CommandDescription[] = "Skips the current cutscene cut.";
inline constexpr std::size_t kSlot342CommandDescriptionLength = 0x1f;

inline constexpr std::uint64_t kSlot342BoolRva = 0x325ec38;
inline constexpr std::uint64_t kSlot342Float0Rva = 0x325ed00;
inline constexpr std::uint64_t kSlot342Float1Rva = 0x325edd8;
inline constexpr std::uint64_t kSlot342Cell0Rva = 0x325eeb0;
inline constexpr std::uint64_t kSlot342Cell1Rva = 0x325eec4;
inline constexpr std::uint64_t kSlot342CommandRva = 0x325ef00;

struct Slot342Objects {
    ConCmdBoolVar bvar;                              // 0x325ec38
    ConCmdRangedVar fvar[kSlot342FloatCount];        // 0x325ed00, 0x325edd8
    HashedName20 cells[kSlot342NameCellCount];       // 0x325eeb0, 0x325eec4
    std::uint8_t pad_cmd[0x28];                      // 0x325eed8..0x325ef00 never written
    ConCmdCommand command;                           // 0x325ef00
};

struct Slot342Result {
    bool ok = false;
    std::size_t rows_done = 0;                       // b0, f0, f1, n0, n1, cmd (6 on success)
    HelperRefusal refusal = HelperRefusal::none;
    HelperFloatSiteResult fvar[kSlot342FloatCount]{};
    FConCmdBuildResult command{};
};

[[nodiscard]] Slot342Result init_slot342_7d8e10(Slot342Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
