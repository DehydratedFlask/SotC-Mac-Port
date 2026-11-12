// CPU-01-AOT-CFG-SCALE-01-BATCH19: native model of the "availability" entry constructor 0xa42960 (590 B, FDE 590; ELF sha256
// d0723afb...5861d3, body sha 5b1e5967...fbe81f456). Called 6 times by startup slot 428 (0x52ec10), one 0x48 B entry per call.
// Byte-pinned by tools/lift/check_helper_a42960.py. Nothing here executes or calls an original RVA.
//
//   0xa42960(rdi = entry, rsi = const SharedStr* a, rdx = const SharedStr* b)       ; entry is 0x48 B
//     init_slot_f180(entry + 0x00) ; init_slot_f180(entry + 0x20) ; entry->var = 0
//     if (entry != a)        { 0xf590(entry + 0); entry->a = *a  (lock inc ctl) }
//     if (entry + 0x20 != b) { 0xf590(entry + 0x20); entry->b = *b (lock inc ctl) }
//     var = 0x17070(0xc8)                                         ; a 0xc8 B ConCmdBoolVar
//     name = 0x10a90("entitlement_available_%s", len 0x18, entry->a.data)           ; an owned string
//     desc = { data = "", 0, 0, 0 }
//     bool var constructed EXACTLY as the shared bool-var shape without the atexit record (init_bool_var_vector): 0x20af0(var, name,
//         desc, 1), mid vptr, value_ref = var + 0xc0, zeros, fn tag 0x15d17b0, release(desc), final vptr, value 0, default 0,
//         0x20d60(var) registers it
//     old = entry->var ; if (old != var && old != 0) old->vtable[1]() ; entry->var = var        ; old is always 0 here (zeroed above)
//     0xf590(name)                                                ; the real release of the formatted name
//     push entry onto the process vector V2 {begin 0x32f6f38, end 0x32f6f40, cap 0x32f6f48}   ; the doubling push shared with 0xa59620
//
// What is a hook: the allocator pair (AllocDispatchContract), the formatter 0x10a90 (a native snprintf stand-in for the ONE observed format
// string "entitlement_available_%s"; the real Format is unlifted), the deallocator for the owned name. The indirect delete of a previous var
// is unreachable (the field was zeroed three instructions earlier) and is reported, never faked. Refusals are decided before the first
// observable side effect that cannot be reversed; a refusal leaves the entry written (as in the original) but the var unregistered.
// NOT modelled: what "entitlement available" means (INFERENCE only), the exit-time teardown of V2. FIXTURE_VERIFIED + STATIC_ANALYSIS_VERIFIED.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "helper_a59620_id_entry.hpp"
#include "shared_str_ctor_f1a0.hpp"
#include "shared_str_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kAvailEntryCtorRva = 0xa42960;
inline constexpr std::size_t kAvailEntryCtorSize = 590;
inline constexpr std::uint64_t kAvailEntryVecBeginRva = 0x32f6f38;     // V2 begin; end at +8, cap at +0x10
inline constexpr std::size_t kAvailEntryStride = 0x48;
inline constexpr std::size_t kAvailVarAllocSize = 0xc8;
inline constexpr std::uint64_t kAvailVarFnVtbl = 0x15d17b0;            // functor tag stored at var + 0xb0
inline constexpr std::size_t kAvailNameLength = 0x18;                  // edx of the Format call: strlen("entitlement_available_%s")

static_assert(sizeof(ConCmdBoolVar) == kAvailVarAllocSize);

// The 0x48 B entry at the call sites.
struct AvailEntry {
    SharedStr a;                  // +0x00
    SharedStr b;                  // +0x20
    ConCmdBoolVar* var;           // +0x40
};
static_assert(sizeof(AvailEntry) == kAvailEntryStride && offsetof(AvailEntry, b) == 0x20 && offsetof(AvailEntry, var) == 0x40);

struct AvailEntryHooks {
    AllocDispatchContract alloc{};                        // 0x17070 (var), 0x17090 / 0x170c0 (vector)
    SharedStrCtorHooks name_alloc{};                      // 0xf1a0 for the formatted name
    SharedStrDeallocatorF590* name_dealloc = nullptr;     // the real 0xf590 release of the owned name
};

enum class AvailEntryStage : std::uint8_t {
    none,
    hooks_missing,       // no allocator / no name deallocator: nothing written
    var_alloc_failed,    // 0x17070 returned null (the entry strings are already written)
    name_failed,         // the formatted name could not be built
    construct_failed,    // the bool var construction or registration reported failure
    push_failed,         // the V2 push refused (see push_refusal)
    ok,
};

struct AvailEntryResult {
    bool ok = false;
    AvailEntryStage stage = AvailEntryStage::none;
    PtrVecRefusal push_refusal = PtrVecRefusal::none;
    bool a_aliased = false;
    bool b_aliased = false;
    bool grew = false;
    std::size_t new_capacity = 0;
    BoolVarResult var_result{};
};

// The formatted name "entitlement_available_" + text (the one observed Format string).
[[nodiscard]] bool format_entitlement_name(char* out, std::size_t capacity, const char* text) noexcept;

// 0xa42960. `registry` receives the var's 0x20d60 registration.
[[nodiscard]] AvailEntryResult register_avail_entry_a42960(PtrVec24& vec, AvailEntry& entry, const SharedStr& a, const SharedStr& b,
                                                           ConCmdRegistry& registry, const AvailEntryHooks& hooks) noexcept;

}  // namespace sotc
