// CPU-01-STARTUP-BATCH-10: native model of startup constructor slot 388 (RVA 0x6a8c60, 733 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 388. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_388.py. NOTHING here executes,
// loads or calls an original RVA: vtables, functor tags, handler tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// The survey flagged "1 other indirect call". It is resolved statically: `callq *0x20(%rax)` at 0x6a8eea has rax = `leaq 0x1594dc0(%rip)` (0x6a8e8b,
// unchanged until the call), the console-command register-once virtual slot [0x1594dc0 + 0x20] = 0x20d60 (the `fcmd_vtable` extra pins that
// relocation), the same shape as slots 306 / 339 / 340 / 342 / 343 / 359.
//
// Shape (a scripted-events debug page; the reading is INFERENCE from the strings). In order (object RVA):
//   b0   0x3017610  bool variable `cp10EnableBPAnimStateOverrides` (empty description, flags 1, value = default 1, caller tag 0x15cc6f8), atexit 0x20b0
//   t0   0x30176d8  0x26ea0 toggle, parent `Scripted Events`, text `Enable BP AnimState Overrides`, target b0, flags 0x30, atexit 0x272d0
//   cmd  0x3017780  console command `gCP10ExtractCP3EventFrameTables` (description `Extract CP3 Event FrameTables`, flags 2, handler tag 0x6a6570
//                   never called, vptr 0x1594dc0 + 0x10), built by init_fconcmd_object, atexit 0x4b0
// Registration order: b0, cmd (the toggle is a menu item). All strings are borrowed rodata views, so every 0xf590 release is an exact no-op.
// The __stack_chk_guard compare is a hook. No consumer of the variable or of the command is lifted.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_family_items.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot388Rva = 0x6a8c60;
inline constexpr std::size_t kSlot388Size = 733;
inline constexpr std::uint64_t kSlot388BoolFnTag = 0x15cc6f8;
inline constexpr std::uint64_t kSlot388HandlerTag = 0x6a6570;
inline constexpr std::uint32_t kSlot388ItemFlags = 0x30;

inline constexpr char kSlot388BoolName[] = "cp10EnableBPAnimStateOverrides";
inline constexpr std::size_t kSlot388BoolNameLength = 0x1e;
inline constexpr std::uint8_t kSlot388BoolValue = 1;
inline constexpr char kSlot388ToggleParent[] = "Scripted Events";
inline constexpr std::size_t kSlot388ToggleParentLength = 0xf;
inline constexpr char kSlot388ToggleText[] = "Enable BP AnimState Overrides";
inline constexpr std::size_t kSlot388ToggleTextLength = 0x1d;
inline constexpr char kSlot388CommandName[] = "gCP10ExtractCP3EventFrameTables";
inline constexpr char kSlot388CommandDescription[] = "Extract CP3 Event FrameTables";
inline constexpr std::size_t kSlot388CommandDescriptionLength = 0x1d;

inline constexpr std::uint64_t kSlot388BoolRva = 0x3017610;
inline constexpr std::uint64_t kSlot388ItemRva = 0x30176d8;
inline constexpr std::uint64_t kSlot388CommandRva = 0x3017780;

struct Slot388Objects {
    ConCmdBoolVar bvar;        // 0x3017610
    DebugMenuItem item;        // 0x30176d8
    ConCmdCommand command;     // 0x3017780
};

struct Slot388Result {
    bool ok = false;
    std::size_t rows_done = 0;   // b0, t0, cmd (3 on success)
    FConCmdBuildResult command{};
};

[[nodiscard]] Slot388Result init_slot388_6a8c60(Slot388Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
