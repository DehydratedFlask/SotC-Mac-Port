// CPU-01-STARTUP-BATCH-9: native model of startup constructor slot 469 (RVA 0x394a20, 3658 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 469. Generated from the ELF by a static register tracker and byte-pinned by tools/lift/check_ctor_slot_469.py.
// NOTHING here executes, loads or calls an original RVA: vtable pointers, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Sound auto ambient grass page (inference from the strings): 5 bool and 3 bounded float sndx_autoGrass* variables, the Sound group, 6 explicit-text toggles and 4 sliders, and a trailing self-referential sentinel container whose destructor 0x3941b0 is recorded.
// Rows, in constructor order (19 objects at 0x2ef5590..): 5 bool variables (caller functor tag 0x15adbd8, dtor 0x20b0), 3 BOUNDED float variables
// (0xc300, flags 1, value = default, caller functor tag 0x15adbf8, dtor 0x20f0), then the menu rows (item flags 0x30, dtor 0x272d0): 0x26db0 toggles (text = the target's own name view;
// over a float target the range narrows to {0.1, 0.5}), 0x26ea0 toggles (explicit text), 0x27120 sliders and 0x26c10 groups.
// Menu: Sound group; Auto Ambient Grass toggles and sliders.
//
// NOT established: what reads the variables at run time. Any reading of the names is INFERENCE only.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "string_var_257a0.hpp"
#include "ctor_slot_family_items.hpp"
#include "fnv_cstr_161fb0.hpp"
#include "hashed_name_ctors.hpp"
#include "hashed_string_14c3a0.hpp"
#include "helper_620e10_local_int_var.hpp"
#include "helper_a81700_named_record.hpp"
#include "helper_int_var_sites.hpp"
#include "l2_slot_common.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot469Rva = 0x394a20;
inline constexpr std::size_t kSlot469Size = 3658;
inline constexpr std::uint64_t kSlot469BoolFnTag = 0x15adbd8;
inline constexpr std::uint64_t kSlot469FloatFnTag = 0x15adbf8;
inline constexpr std::uint32_t kSlot469ItemFlags = 0x30;
inline constexpr std::uint32_t kSlot469VarFlags = 1;
inline constexpr std::uint64_t kSlot469UFloatFnTag = 0x0;
inline constexpr std::size_t kSlot469UFloatCount = 0;
inline constexpr std::size_t kSlot469BoolCount = 5;
inline constexpr std::size_t kSlot469FloatCount = 3;
inline constexpr std::size_t kSlot469RowCount = 19;
inline constexpr std::size_t kSlot469StringCount = 11;
inline constexpr std::uint64_t kSlot469IntFnTag = 0x0;
inline constexpr std::size_t kSlot469IntCount = 0;
inline constexpr std::size_t kSlot469LocalIntCount = 0;
inline constexpr std::size_t kSlot469SliderCount = 4;
inline constexpr std::uint64_t kSlot469UIntFnTag = 0x0;
inline constexpr std::size_t kSlot469UIntCount = 0;
inline constexpr std::size_t kSlot469CmdCount = 0;
inline constexpr std::size_t kSlot469StrVarCount = 0;
inline constexpr std::uint64_t kSlot469Object0Rva = 0x2ef5590;
inline constexpr std::size_t kSlot469CellCount = 0;
inline constexpr std::size_t kSlot469HolderCount = 0;
inline constexpr std::size_t kSlot469SentinelCount = 1;
inline constexpr std::uint64_t kSlot469SentinelDtorRva = 0x3941b0;
inline constexpr char kSlot469BoolNames[kSlot469BoolCount][35] = {"sndx_showAutoGrassInfo", "sndx_showAutoGrassRadius", "sndx_showAutoGrassRustle", "sndx_enableAutoGrassListenerOffset", "sndx_flatenListenerDirForOffset"};
inline constexpr std::size_t kSlot469BoolNameLengths[kSlot469BoolCount] = {0x16, 0x18, 0x18, 0x22, 0x1f};
inline constexpr std::uint8_t kSlot469BoolValues[kSlot469BoolCount] = {0, 0, 0, 1, 1};
inline constexpr char kSlot469FloatNames[kSlot469FloatCount][35] = {"sndx_autoGrassRadius", "sndx_autoGrassFalloff", "sndx_autoGrassListenerOffset"};
inline constexpr std::size_t kSlot469FloatNameLengths[kSlot469FloatCount] = {0x14, 0x15, 0x1c};
inline constexpr std::uint32_t kSlot469FloatLoBits[kSlot469FloatCount] = {0x00000000, 0x00000000, 0xc1200000};
inline constexpr std::uint32_t kSlot469FloatHiBits[kSlot469FloatCount] = {0x42480000, 0x42480000, 0x42480000};
inline constexpr std::uint32_t kSlot469FloatValueBits[kSlot469FloatCount] = {0x40eccccd, 0x415e6666, 0x40f00000};
inline constexpr char kSlot469MenuStrings[kSlot469StringCount][25] = {"Sound", "Auto Ambient Grass", "Show Info", "Draw Radius/Falloff", "Draw Rustle", "Emitter Spacing", "Full Volume Radius", "Falloff Distance", "Enable Listener Offset", "Flaten Listener Forward", "Listener Offset Distance"};
inline constexpr std::size_t kSlot469MenuStringLengths[kSlot469StringCount] = {0x5, 0x12, 0x9, 0x13, 0xb, 0xf, 0x12, 0x10, 0x16, 0x17, 0x18};
inline constexpr std::uint32_t kSlot469SliderLoBits[kSlot469SliderCount] = {0x3dcccccd, 0x3dcccccd, 0x3dcccccd, 0x3dcccccd};
inline constexpr std::uint32_t kSlot469SliderHiBits[kSlot469SliderCount] = {0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000};

// Sentinel container (0x3941b0 is its atexit destructor, an IDENTITY TAG). The original writes {[0x00] = [0x08] = [0x20] = this + 0x28, [0x10] = this + 0xa8} and nothing else:
// a self-referential header whose sentinel node starts at +0x28 and whose 0x80 B area ends at +0xa8 (the size and the element type are INFERENCE; +0x18 and the area are never written).
struct Slot469Sentinel {
    std::uintptr_t q00;   // = this + 0x28
    std::uintptr_t q08;   // = this + 0x28
    std::uintptr_t q10;   // = this + 0xa8
    std::uintptr_t q18;   // never written
    std::uintptr_t q20;   // = this + 0x28
    std::uint8_t area[0x80];   // never written
};
static_assert(sizeof(Slot469Sentinel) == 0xa8);

struct Slot469Objects {
    ConCmdBoolVar r00;   // 0x2ef5590 bool
    ConCmdBoolVar r01;   // 0x2ef5658 bool
    ConCmdBoolVar r02;   // 0x2ef5720 bool
    ConCmdRangedVar r03;   // 0x2ef57e8 float
    ConCmdRangedVar r04;   // 0x2ef58c0 float
    ConCmdBoolVar r05;   // 0x2ef5998 bool
    ConCmdBoolVar r06;   // 0x2ef5a60 bool
    ConCmdRangedVar r07;   // 0x2ef5b28 float
    DebugMenuItem r08;   // 0x2ef5c00 group
    DebugMenuItem r09;   // 0x2ef5ca8 toge
    DebugMenuItem r10;   // 0x2ef5d50 toge
    DebugMenuItem r11;   // 0x2ef5df8 toge
    DebugMenuItem r12;   // 0x2ef5ea0 slider50
    DebugMenuItem r13;   // 0x2ef5f48 slider50
    DebugMenuItem r14;   // 0x2ef5ff0 slider50
    DebugMenuItem r15;   // 0x2ef6098 toge
    DebugMenuItem r16;   // 0x2ef6140 toge
    DebugMenuItem r17;   // 0x2ef61e8 slider50
    Slot469Sentinel r18;   // 0x2ef6290 sent
    const void* external_target_0 = nullptr;   // identity of the object at 0x2ee65e8 (built by another slot; the host supplies it, the model only stores the pointer)
};

struct Slot469Result {
    bool ok = false;
    std::size_t rows_done = 0;   // rows that completed (variable registered / menu item built, registered, views released and its destructor recorded)
};

[[nodiscard]] Slot469Result init_slot469_394a20(Slot469Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu) noexcept;

}  // namespace sotc
