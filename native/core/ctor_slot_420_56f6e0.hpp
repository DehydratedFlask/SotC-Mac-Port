// CPU-01-STARTUP-BATCH-DEBUG-PAGE-01 (batch 6): native model of startup constructor slot 420 (RVA 0x56f6e0, 1006 bytes). Original ELF
// sha256 d0723afb...5861d3, constructor-table word at 0x2dd70e0 - 8 * 420. Every fact below is byte-pinned by
// tools/lift/check_ctor_slot_420.py. NOTHING here executes, loads or calls an original RVA: vtable pointers, the functor tags and
// the destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// Shape, in order: (1) bool variable `sndx_debugOcclusionWork` (object 0x2f11408, tag 0x15caf18, value = default = 0, empty
// description, static/literal shape); (2) RANGED int variable `sndx_occlusionWorkIndex` (0xc580 through helper_c580_ranged_int,
// range 0..0x1f, flags 1, value = default = 0, tag 0x15caf38, object 0x2f114d0); (3) a 0x26b60 SEPARATOR (item 0x2f115a8, parent
// `Sound`, flags 0x30); (4) a 0x26ea0 toggle for the bool (item 0x2f11650, parent `Sound`, caption `Show Occlusion Tests`, flags 0x30);
// (5) a 0x27050 slider for the int variable (item 0x2f116f8, parent `Sound`, caption `Occlusion Index`, flags 0x30, range
// {1.0f, 1.0f}); (6) an 8 byte zero cell (0x2f117a0) with __cxa_atexit(0x56ef80, cell): the destructor frees a non-null
// pointer through 0x170b0, so a zero cell is inert (IDENTITY TAG: recorded, not run).
//
// HOOK (documented, fail closed): the separator builds the text "MenuSeparator" through the native 0xf1a0, which needs the engine
// allocator behind 0x12820 / 0x12af0 (NOT lifted). `default_slot420_hooks()` installs the documented malloc/free stand-in (the same
// convention as slots 306 / 329); with the allocator cleared the fragment reports ok == false at the separator and builds
// nothing after it. The separator text storage is returned to that allocator by the objects' destructor.
//
// NOT established: what reads the two variables at run time. "Occlusion work" reading from the strings is INFERENCE only. The
// __stack_chk_guard compare is a hook.
#pragma once

#include "ctor_shapes_343_356.hpp"
#include "fmenu_ctors_slot306.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"
#include "shared_str_ctor_f1a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot420Rva = 0x56f6e0;
inline constexpr std::size_t kSlot420Size = 1006;
inline constexpr std::uint64_t kSlot420BoolFnTag = 0x15caf18;
inline constexpr std::uint64_t kSlot420IntFnTag = 0x15caf38;
inline constexpr std::uint64_t kSlot420HolderDtorRva = 0x56ef80;
inline constexpr char kSlot420Var0Name[] = "sndx_debugOcclusionWork";
inline constexpr std::size_t kSlot420Var0NameLength = 0x17;
inline constexpr char kSlot420Var1Name[] = "sndx_occlusionWorkIndex";
inline constexpr std::size_t kSlot420Var1NameLength = 0x17;
inline constexpr std::int32_t kSlot420Var1Min = 0;
inline constexpr std::int32_t kSlot420Var1Max = 0x1f;
inline constexpr char kSlot420Parent[] = "Sound";
inline constexpr std::size_t kSlot420ParentLength = 5;
inline constexpr char kSlot420Item1Text[] = "Show Occlusion Tests";
inline constexpr std::size_t kSlot420Item1TextLength = 0x14;
inline constexpr char kSlot420Item2Text[] = "Occlusion Index";
inline constexpr std::size_t kSlot420Item2TextLength = 0xf;
inline constexpr std::uint32_t kSlot420Item2MinBits = 0x3f800000;
inline constexpr std::uint32_t kSlot420Item2MaxBits = 0x3f800000;
inline constexpr std::uint32_t kSlot420ItemFlags = 0x30;
inline constexpr std::uint64_t kSlot420Var0Rva = 0x2f11408;
inline constexpr std::uint64_t kSlot420Var1Rva = 0x2f114d0;
inline constexpr std::uint64_t kSlot420SepRva = 0x2f115a8;
inline constexpr std::uint64_t kSlot420Item1Rva = 0x2f11650;
inline constexpr std::uint64_t kSlot420Item2Rva = 0x2f116f8;
inline constexpr std::uint64_t kSlot420HolderRva = 0x2f117a0;

struct Slot420Objects {
    ConCmdBoolVar var0;       // 0x2f11408
    ConCmdRangedVar var1;     // 0x2f114d0
    DebugMenuItem sep;        // 0x2f115a8
    DebugMenuItem item1;      // 0x2f11650
    DebugMenuItem item2;      // 0x2f116f8
    std::uint64_t holder = 0; // 0x2f117a0, atexit 0x56ef80
    // The allocator the separator text came from; its storage is returned to it by the destructor.
    AllocDispatchContract owned_alloc{};

    Slot420Objects() = default;
    Slot420Objects(const Slot420Objects&) = delete;
    Slot420Objects& operator=(const Slot420Objects&) = delete;
    ~Slot420Objects();
};

struct Slot420Hooks {
    SharedStrCtorHooks separator_alloc{};   // allocator behind 0xf1a0 ("MenuSeparator")
};

// Host allocator stand-in (malloc/free) for the "MenuSeparator" text. NOT the engine allocator.
[[nodiscard]] Slot420Hooks default_slot420_hooks() noexcept;

struct Slot420SeparatorResult {
    bool ok = false;
    bool ctor_ok = false;              // 0x26b60 incl. the 0xf1a0 text allocation
    bool registration_ok = false;      // the tail call into 0x268b0
    bool views_inert = false;          // f590(parent view)
    bool atexit_recorded = false;      // 0x272d0 recorded, not run
};

struct Slot420Result {
    bool ok = false;
    HelperRefusal refusal = HelperRefusal::none;   // the ranged-int wrapper's refusal, if any
    BoolVarResult var0{};
    HelperIntSiteResult var1{};
    Slot420SeparatorResult sep{};
    MenuSiteResult item1{};
    MenuSiteResult item2{};
    bool holder_stored = false;
    bool holder_atexit_recorded = false;
};

[[nodiscard]] Slot420Result init_slot420_56f6e0(Slot420Objects& objects, ConCmdRegistry& concmd, DebugMenuRegistry& menu,
                                                const Slot420Hooks& hooks) noexcept;

}  // namespace sotc
