// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 339 (RVA 0x7e2fc0, 1004 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 339. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_339.py. NOTHING here
// executes, loads or calls an original RVA: vtables, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// This was the startup-table cliff after batch 8 ("one non-functor indirect call plus the unmodelled-caller helper 0x52a990"). Both flags were
// naming misses: 0x52a990 is the native fnv_cstr_52a990 (`mov esi,0x811c9dc5; jmp 0x161fb0`, the NameHashCell hash of core/hashed_name_ctors),
// and the one non-functor indirect call `callq *0x20(%rax)` at 0x7e3356 is the console-command register-once virtual slot [0x1594dc0 + 0x20] =
// 0x20d60 (relocation-pinned by the `fcmd_vtable` extra), the same shape as slots 306 / 343 / 386 / 557. The other indirect call, `*0x18(%rax)`
// at 0x7e3258, is the float functor destroy of the 68-table family (tag 0x15ce440).
//
// Shape (no menu item, no group). In order (object RVA):
//   c0..c6  0x325ffb0 + 0x10 * n   seven {name ptr, fnv1a(name)} cells (the NameHashCell shape, 16 B, +0x0c never written):
//                                  CurrentPlayTime{EASY,NORMAL,HARD}, FastestPlayTime{EASY,NORMAL,HARD}, CurrentDeathCount
//   h0      0x3260020  0x30 B NameHolder30 (vptr 0x15976a0 + 0x10, borrowed view `shadowDemoMode`, tail 0), atexit 0x8ef00
//   h1      0x3260050  0x30 B NameHolder30 (same class, view `shadowDemoMode_gameplayIntro`), atexit 0x8ef00
//   f0      0x3260080  unbounded float variable `shadowDemoMode_gameplayTimer` (0x6aa00, flags 1, caller tag 0x15ce440, value = default
//                      0x42f00000), atexit 0x20f0
//   cmd     0x3260160  console command `killBoss` (description `Kills the current boss if present. Params: None`, flags 2, handler tag
//                      0x7def00 never called, vptr 0x1594dc0 + 0x10), built by init_fconcmd_object, atexit 0x4b0
// (the 8 bytes 0x3260158..0x3260160 are never written). Registration order: the float, then the command. Atexit order: h0, h1, float, cmd.
// The float goes through helper_init_unbounded_float_var_site and requires HelperRefusal::none (tag 0x15ce440 is a member of the 68-table float
// family). All strings are borrowed rodata views, so every 0xf590 release is an exact no-op. Meaning of the names (demo-mode page, play-time and
// death-count cells, a boss kill command) is INFERENCE from the strings only; no consumer or handler is lifted. The __stack_chk_guard compare is a hook.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot339Rva = 0x7e2fc0;
inline constexpr std::size_t kSlot339Size = 1004;
inline constexpr std::uint64_t kSlot339FloatFnTag = 0x15ce440;
inline constexpr std::uint64_t kSlot339HandlerTag = 0x7def00;
inline constexpr std::uint64_t kSlot339HolderDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot339HolderVptr = 0x15976b0;   // 0x15976a0 + 0x10 (identity tag)

inline constexpr std::size_t kSlot339CellCount = 7;
inline constexpr char kSlot339CellNames[kSlot339CellCount][24] = {"CurrentPlayTimeEASY", "CurrentPlayTimeNORMAL", "CurrentPlayTimeHARD",
                                                                    "FastestPlayTimeEASY", "FastestPlayTimeNORMAL", "FastestPlayTimeHARD",
                                                                    "CurrentDeathCount"};
inline constexpr char kSlot339Holder0Name[] = "shadowDemoMode";
inline constexpr std::size_t kSlot339Holder0NameLength = 0xe;
inline constexpr char kSlot339Holder1Name[] = "shadowDemoMode_gameplayIntro";
inline constexpr std::size_t kSlot339Holder1NameLength = 0x1c;
inline constexpr char kSlot339FloatName[] = "shadowDemoMode_gameplayTimer";
inline constexpr std::size_t kSlot339FloatNameLength = 0x1c;
inline constexpr std::uint32_t kSlot339FloatValueBits = 0x42f00000;
inline constexpr char kSlot339CommandName[] = "killBoss";
inline constexpr char kSlot339CommandDescription[] = "Kills the current boss if present. Params: None";
inline constexpr std::size_t kSlot339CommandDescriptionLength = 0x2f;

inline constexpr std::uint64_t kSlot339Cell0Rva = 0x325ffb0;
inline constexpr std::uint64_t kSlot339Holder0Rva = 0x3260020;
inline constexpr std::uint64_t kSlot339Holder1Rva = 0x3260050;
inline constexpr std::uint64_t kSlot339FloatRva = 0x3260080;
inline constexpr std::uint64_t kSlot339CommandRva = 0x3260160;

struct Slot339Objects {
    NameHashCell cells[kSlot339CellCount];   // 0x325ffb0
    NameHolder30 holder0;                    // 0x3260020
    NameHolder30 holder1;                    // 0x3260050
    ConCmdRangedVar fvar;                    // 0x3260080
    std::uint8_t pad_cmd[8];                 // 0x3260158 never written
    ConCmdCommand command;                   // 0x3260160
};

struct Slot339Result {
    bool ok = false;
    bool cells_ok = false;                   // all seven hashes written (name pointer + fnv1a)
    bool holders_ok = false;                 // both holders built, f590 inert, (0x8ef00, holder) recorded
    HelperRefusal refusal = HelperRefusal::none;
    HelperFloatSiteResult fvar{};
    FConCmdBuildResult command{};
};

[[nodiscard]] Slot339Result init_slot339_7e2fc0(Slot339Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
