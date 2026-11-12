// CPU-01-AOT-CFG-SCALE-01-BATCH24 (slot 452): native model of startup constructor slot 452
// (RVA 0x498640, 1,235 B; FDE [0x498640, 0x498643+...)= [0x498640, 0x498b13)). Original ELF sha256
// d0723afb...5861d3; the constructor-table word at 0x2dd70e0 - 8 * 452 = 0x2dd62c0 holds 0x498640.
// Every fact below was read end to end from the pinned bytes; the byte oracle is
// tools/lift/check_ctor_slot_452.py. NOTHING here executes, loads or calls an original RVA:
// destructor RVAs, the handler RVAs and the __dso_handle are IDENTITY TAGS, recorded and never run.
//
// ================================ observed shape ================================
// Frame: push rbp; mov rsp,rbp; push r15/r14/r13/r12/rbx; and rsp,-0x20; sub rsp,0xa0.
// Canary: `[0x15fe518]` -> [rsp+0x80]; epilogue compares it and branches to 0xf8f3a0
// (__stack_chk_fail). 251 instructions, 28 direct call sites over 7 targets, **4 indirect sites**.
//
// The four `callq *%r12` sites are NOT unresolved. Each is preceded by
//   `leaq 0x1594dc0, %rax` ... `movq 0x20(%rax), %r12`
// and the RELATIVE relocation at 0x1594de0 holds 0x20d60, so every one of them calls
// `concmd_register`. That is the same vtable dispatch shape core/fconcmd_object_slot306 already
// models (`[kFConCmdVtableStart + 0x20]`), and slot 452 uses the same 0xa0-byte command object.
//
// Body, in order:
//   1. node 0x2ef8bf0: zero 16 B at +0x08, `operator new(0x48)` via 0x17070, then
//      {self,self,self} at +0x00/+0x08/+0x10, bytes 0x18 = 1 and 0x19 = 1, node stored at +0x08,
//      then __cxa_atexit(0x497610, 0x2ef8bf0, 0x1604218).
//   2. cell 0x2ef8c08: qword 0, then __cxa_atexit(0x493190, 0x2ef8c08, 0x1604218).
//   3. four console-COMMAND objects at 0x2ef8c10 + 0xa0*i, each exactly the slot-306 shape:
//      name view (0xf2f0), description view {cstr, immediate length}, ConCmdBase ctor 0x20af0 with
//      flags = 2, vptr 0x1594dd0 and the {self, 0x1990, 0, handler} delegate at +0x60, zeroed
//      +0x80..+0x97, register through [0x1594dc0+0x20] (0x20d60), two inert 0xf590 releases, then
//      __cxa_atexit(0x4b0, object).
//   4. one debug-menu TOGGLE at 0x2ef8e90: `0x26ea0(item, "Shared", "ResetPhysicsCameraCut",
//      target = command 3, flags = 0x30)`, two inert releases, __cxa_atexit(0x272d0, item).
// Seven __cxa_atexit records in total, in that order.
//
// The command names carry cp10_ debug-command semantics; what executes them is the delegate handler.
// The handlers 0x497320 / 0x497520 / 0x497560 / 0x4975a0 are IDENTITY TAGS here, never called.
// That the toggle resets a physics camera cut is read from its text; the effect itself is INFERENCE.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "ctor_slot_family_items.hpp"
#include "debug_menu_item.hpp"
#include "fconcmd_object_slot306.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot452Rva = 0x498640;
inline constexpr std::size_t kSlot452Size = 1235;
inline constexpr std::uint64_t kSlot452TableWord = 0x2dd62c0;   // 0x2dd70e0 - 8 * 452

inline constexpr std::uint64_t kSlot452NodeObject = 0x2ef8bf0;   // 0x18-byte object, atexit 0x497610
inline constexpr std::uint64_t kSlot452NodeCell = 0x2ef8bf8;     // the node pointer
inline constexpr std::uint64_t kSlot452Cell = 0x2ef8c08;         // 8-byte cell, atexit 0x493190
inline constexpr std::uint64_t kSlot452Object0 = 0x2ef8c10;      // four commands, 0xa0 apart
inline constexpr std::size_t kSlot452CommandCount = 4;
inline constexpr std::size_t kSlot452CommandStride = 0xa0;
inline constexpr std::uint64_t kSlot452Toggle = 0x2ef8e90;       // 0x2ef8c10 + 4 * 0xa0
inline constexpr std::uint64_t kSlot452NodeSize = 0x48;          // the allocation size
inline constexpr std::uint64_t kSlot452NodeDtor = 0x497610;
inline constexpr std::uint64_t kSlot452CellDtor = 0x493190;
inline constexpr std::uint64_t kSlot452CommandDtor = 0x4b0;
inline constexpr std::uint64_t kSlot452ItemDtor = 0x272d0;
inline constexpr std::uint64_t kSlot452DsoHandle = 0x1604218;
inline constexpr std::uint64_t kSlot452AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot452AllocRva = 0x17070;

inline constexpr std::uint64_t kSlot452CanaryCell = 0x15fe518;
inline constexpr std::uint64_t kSlot452StackChkFailRva = 0xf8f3a0;
inline constexpr std::size_t kSlot452CanaryFrameOffset = 0x80;
inline constexpr std::size_t kSlot452FrameBytes = 0xa0;

// 0x26ea0 arguments for the one toggle.
inline constexpr std::uint64_t kSlot452ToggleParentRva = 0xffcaad;
inline constexpr std::size_t kSlot452ToggleParentLength = 6;      // "Shared"
inline constexpr std::uint64_t kSlot452ToggleTextRva = 0xffcab4;
inline constexpr std::size_t kSlot452ToggleTextLength = 21;       // "ResetPhysicsCameraCut"
inline constexpr std::uint32_t kSlot452ToggleFlags = 0x30;
inline constexpr std::size_t kSlot452ToggleTarget = 3;            // command 3, the last object
inline constexpr std::uint64_t kSlot452ToggleGate3 = 0x22ae0;     // vtable slot 3: `xor eax,eax; ret`

// The four command sites, in body order. Lengths are the immediates the body stores into the
// description view; they are pinned independently against the ELF by the checker.
struct Slot452CommandSite {
    const char* name;
    std::size_t name_length;
    const char* description;
    std::size_t description_length;
    std::uint64_t name_rva;
    std::uint64_t description_rva;
    std::uint64_t object_rva;
    std::uint64_t handler_rva;
};

inline constexpr std::array<Slot452CommandSite, kSlot452CommandCount> kSlot452CommandSites{{
    {"cp10_cutsceneEvent", 18,
     "Trigger a cutscene event. Params: <state: 0=start, 1=Update, 2=stop, 3=skip, 4=suspend, 5=resume> <eventName> <cutIndex> <time>", 127,
     0xffc929, 0xffc93c, 0x2ef8c10, 0x497320},
    {"cp10_setAreaLoaderState", 23,
     "Manually set the desired area loader state. Params: <zoneId>", 60,
     0xffc9bc, 0xffc9d4, 0x2ef8cb0, 0x497520},
    {"cp10_setUIAreaLoaderState", 25,
     "Manually set the desired area loader state for the UI. Params: <zoneId>", 71,
     0xffca11, 0xffca2b, 0x2ef8d50, 0x497560},
    {"cp10_ResetPhysicsCameraCut", 26,
     "Debug test of reseting physics", 30,
     0xffca73, 0xffca8e, 0x2ef8df0, 0x4975a0},
}};

inline constexpr const char* kSlot452ToggleParent = "Shared";
inline constexpr const char* kSlot452ToggleText = "ResetPhysicsCameraCut";

// The 0x48-byte node `operator new` produces: three self links, then the two flag bytes.
struct Slot452Node {
    std::uint64_t self0;
    std::uint64_t self8;
    std::uint64_t self10;
    std::uint8_t flag18;
    std::uint8_t flag19;
    std::uint8_t pad[0x2e];
};
static_assert(sizeof(Slot452Node) == kSlot452NodeSize);

struct Slot452Objects {
    std::uint64_t node_head[3]{};                  // 0x2ef8bf0: {?, node, ?}
    std::uint64_t cell = 0;                        // 0x2ef8c08
    std::array<ConCmdCommand, kSlot452CommandCount> commands{};
    DebugMenuItem item{};                          // 0x2ef8e90
    Slot452Node node{};                            // stands in for the 0x17070 allocation
};

// `allocate` is the engine allocator 0x17070 (a hook: its body 0x12820 is not lifted). When it is
// null the model uses the slot's own node storage, so the fixture exercises the real store sequence.
struct Slot452Hooks {
    void* (*allocate)(std::size_t size) noexcept = nullptr;
};

struct Slot452Result {
    bool ok = false;
    bool canary_matched = false;      // a native function has no cookie; recorded, always true
    bool node_linked = false;
    std::size_t commands_built = 0;
    std::size_t registrations = 0;
    std::size_t menu_items = 0;
    std::size_t atexit_records = 0;
};

// The toggle's target: command 3 with its vtable slot 3 answered (0x22ae0 returns 0, so the slot-6
// value is unobserved and left 0).
[[nodiscard]] DebugMenuTarget slot452_toggle_target(const ConCmdCommand& command) noexcept;

[[nodiscard]] Slot452Result init_slot452_498640(Slot452Objects& objects, ConCmdRegistry& registry,
                                                DebugMenuRegistry& menu,
                                                const Slot452Hooks& hooks = {}) noexcept;

}  // namespace sotc
