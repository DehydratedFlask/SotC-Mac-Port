// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 369 (RVA 0x734190, 898 bytes). Original ELF
// sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 369. Every fact below is byte-pinned by
// tools/lift/check_ctor_slot_369.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, the functor
// tags and the destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) bool variable `cp10_enableWanderBonePhysics` (object 0x32509b0, tag 0x15cd160, value 1, default 1, empty
// description, static/literal shape: f590(desc), 0x20d60, f590(name)); (2) a 0x26ea0 toggle (item 0x3250a78) for it under the parent
// path `CP10-Anim`, caption `Enable Wander Bone Physics`, flags 0x30; (3) UNBOUNDED float variable
// `cp10_WanderBonePhysicsCollisionRadius` (0x6aa00 through helper_6aa00_unbounded_float, tag 0x15cd180, flags 1, value = default
// = 0x3e19999a, empty description released early, object 0x3250b20); (4) a 0x27050 slider (item 0x3250bf8) for the float
// variable under the same parent, caption `Wander Bone Physics Collision Radius`, flags 0x30, explicit range
// {0x3c23d70a, 0x3dcccccd}. 0x27050 is native (core/debug_menu_item init_debug_menu_item_27050, ELF-tied by
// check_ctor_slot_361 / 467 and re-pinned here: body hash and instruction operands). The __stack_chk_guard compare is a hook.
//
// NOT established: what reads the two variables at run time. "Wander bone physics" reading from the strings is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot369Rva = 0x734190;
inline constexpr std::size_t kSlot369Size = 898;
inline constexpr std::uint64_t kSlot369BoolFnTag = 0x15cd160;
inline constexpr std::uint64_t kSlot369FloatFnTag = 0x15cd180;
inline constexpr char kSlot369Var0Name[] = "cp10_enableWanderBonePhysics";
inline constexpr std::size_t kSlot369Var0NameLength = 0x1c;
inline constexpr char kSlot369Parent[] = "CP10-Anim";
inline constexpr std::size_t kSlot369ParentLength = 9;
inline constexpr char kSlot369Item0Text[] = "Enable Wander Bone Physics";
inline constexpr std::size_t kSlot369Item0TextLength = 0x1a;
inline constexpr char kSlot369Var1Name[] = "cp10_WanderBonePhysicsCollisionRadius";
inline constexpr std::size_t kSlot369Var1NameLength = 0x25;
inline constexpr std::uint32_t kSlot369Var1ValueBits = 0x3e19999a;
inline constexpr char kSlot369Item1Text[] = "Wander Bone Physics Collision Radius";
inline constexpr std::size_t kSlot369Item1TextLength = 0x24;
inline constexpr std::uint32_t kSlot369Item1MinBits = 0x3c23d70a;
inline constexpr std::uint32_t kSlot369Item1MaxBits = 0x3dcccccd;
inline constexpr std::uint32_t kSlot369ItemFlags = 0x30;
inline constexpr std::uint64_t kSlot369Var0Rva = 0x32509b0;
inline constexpr std::uint64_t kSlot369Item0Rva = 0x3250a78;
inline constexpr std::uint64_t kSlot369Var1Rva = 0x3250b20;
inline constexpr std::uint64_t kSlot369Item1Rva = 0x3250bf8;

struct Slot369Objects {
    ConCmdBoolVar var0;       // 0x32509b0
    DebugMenuItem item0;      // 0x3250a78
    ConCmdRangedVar var1;     // 0x3250b20
    DebugMenuItem item1;      // 0x3250bf8
};

struct Slot369Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // the float wrapper's refusal, if any
    BoolVarResult var0{};
    MenuSiteResult item0{};
    HelperFloatSiteResult var1{};
    MenuSiteResult item1{};
};

[[nodiscard]] Slot369Result init_slot369_734190(Slot369Objects& objects, ConCmdRegistry& concmd,
                                    DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
