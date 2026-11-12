// CPU-01-STARTUP-BATCH-8: native model of startup constructor slot 474 (RVA 0x344850, 1300 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 474. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_474.py. NOTHING here executes,
// loads or calls an original RVA: vtables, functor tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape (no menu item, no group), in order (object RVA, kind):
//   h0  0x2ef0a18  0x30 B holder, vptr 0x15976a0 + 0x10, view `arealoader_areaStateTint`            atexit 0x8ef00
//   h1  0x2ef0a48  0x30 B holder, vptr 0x15976a0 + 0x10, view `arealoader_areaMapTint`              atexit 0x8ef00
//   h2  0x2ef0a78  0x30 B holder, vptr 0x159e190 + 0x10, view `areaLoader_FadeTime`                 atexit 0x2fced0
//   h3  0x2ef0aa8  0x30 B holder, vptr 0x159e190 + 0x10, view `areaLoader_HoldInLoadStateBeforeUnloadingTime`   atexit 0x2fced0
//   b0  0x2ef0ad8  bool   tag 0x15a9850  0   `areaLoader_LightingMode`
//   b1  0x2ef0ba0  bool   tag 0x15a9850  0   `areaLoader_EditorControl`
//   s0  0x2ef0c68  string variable (0x257a0, atexit 0x23c90) name `EntityCreationContext`, initial view `null`
//   b2  0x2ef0db0  bool   tag 0x15a9850  1   `areaLoader_CreateOneAtATime`
// The holders are the 0x30 byte name objects of slots 539 / 550 / 337 (view at +8, zero tail at +0x28); only the class pointer and the destructor differ
// between the two pairs. The bool rows are the static/literal bool shape; the string variable is the already-native 0x257a0 / 0x258f0 pair.
// All strings are borrowed rodata views, so every 0xf590 release is an exact no-op. Meaning of the names (an area loader debug page) is INFERENCE only.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_slot_539_121940.hpp"   // NameHolder30
#include "lane_m6_shapes.hpp"
#include "string_var_257a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot474Rva = 0x344850;
inline constexpr std::size_t kSlot474Size = 1300;
inline constexpr std::uint64_t kSlot474BoolFnTag = 0x15a9850;
inline constexpr std::uint64_t kSlot474HolderVptrA = 0x15976b0;   // 0x15976a0 + 0x10 (identity tag)
inline constexpr std::uint64_t kSlot474HolderVptrB = 0x159e1a0;   // 0x159e190 + 0x10 (identity tag)
inline constexpr std::uint64_t kSlot474HolderDtorA = 0x8ef00;
inline constexpr std::uint64_t kSlot474HolderDtorB = 0x2fced0;

inline constexpr std::size_t kSlot474HolderCount = 4;
inline constexpr char kSlot474HolderNames[kSlot474HolderCount][48] = {"arealoader_areaStateTint", "arealoader_areaMapTint", "areaLoader_FadeTime",
                                                                       "areaLoader_HoldInLoadStateBeforeUnloadingTime"};
inline constexpr std::size_t kSlot474HolderNameLengths[kSlot474HolderCount] = {0x18, 0x16, 0x13, 0x2d};

inline constexpr std::size_t kSlot474BoolCount = 3;
inline constexpr char kSlot474BoolNames[kSlot474BoolCount][32] = {"areaLoader_LightingMode", "areaLoader_EditorControl", "areaLoader_CreateOneAtATime"};
inline constexpr std::size_t kSlot474BoolNameLengths[kSlot474BoolCount] = {0x17, 0x18, 0x1b};
inline constexpr std::uint8_t kSlot474BoolValues[kSlot474BoolCount] = {0, 0, 1};

inline constexpr char kSlot474StringName[] = "EntityCreationContext";
inline constexpr std::size_t kSlot474StringNameLength = 0x15;
inline constexpr char kSlot474StringInit[] = "null";
inline constexpr std::size_t kSlot474StringInitLength = 4;

inline constexpr std::uint64_t kSlot474Obj0Rva = 0x2ef0a18;

struct Slot474Objects {
    NameHolder30 holder0;        // 0x2ef0a18
    NameHolder30 holder1;        // 0x2ef0a48
    NameHolder30 holder2;        // 0x2ef0a78
    NameHolder30 holder3;        // 0x2ef0aa8
    ConCmdBoolVar bvar0;         // 0x2ef0ad8
    ConCmdBoolVar bvar1;         // 0x2ef0ba0
    ConCmdStringVar svar;        // 0x2ef0c68
    ConCmdBoolVar bvar2;         // 0x2ef0db0
};

struct Slot474Result {
    bool ok = false;
    bool holder_ok[kSlot474HolderCount]{};   // holder built, f590 inert, (dtor, holder) recorded
    BoolVarResult bvar[kSlot474BoolCount]{};
    StringVarResult svar{};
    bool svar_views_inert = false;
    bool svar_atexit_recorded = false;
};

[[nodiscard]] Slot474Result init_slot474_344850(Slot474Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
