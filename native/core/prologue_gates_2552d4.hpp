// Prologue epilogue / debug gates OBS `0x2552d4..0x255318` (after store self+0x3b40).
//
// Always-on on measured Init path (prologue 0x255180). Prefer over vslot / 0xf08 /
// ensure migrate / non-null list as next deepen after E416.
//
// OBS:
//   rax = *(*(0x2dfce70)+0x60); if (*rax==0) al = !vcall(*(0x2dfcf40).vt+0x48)
//        else al = 0
//   call 0xba40(0x2eec840, *(0x2eec840+0x60), &al)
//   vcall (*cell_0x2dfd4b8).vt+0x10
//   stack-chk — NOT modelled (host)
//
// Claimed: epilogue control flow + empty-path 0xba40 (+0x6a==0, listeners null).
// NOT modelled: non-empty ba40 listener invoke; +0x1c bit branch; stack-chk PLT;
// bodies of vslot+0x48 / a30 vslot+0x10. PR #5 cite-only. No leaf-vec; not idx 95.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kPrologueGates2552d4Rva = 0x2552d4;
inline constexpr std::size_t kPrologueGates2552d4Size = 68;  // through a30 vcall (excl. canary)
inline constexpr std::uint64_t kPrologueGateFlagObjRva = 0x2dfce70;
inline constexpr std::uint64_t kPrologueGateVtblObjRva = 0x2dfcf40;
inline constexpr std::uint64_t kPrologueDebugObjRva = 0x2eec840;
inline constexpr std::uint64_t kBa40Rva = 0xba40;
inline constexpr std::size_t kBa40Size = 173;  // through first ret @ 0xbaec
inline constexpr std::size_t kPrologueDebugObjBytes = 0x90;

struct PrologueGates2552d4Hooks {
    // Models vcall [*(0x2dfcf40)]+0x48 when gate flag byte is 0. Null → false.
    bool (*vslot_48_2dfcf40)() = nullptr;
    // If non-null, replaces lifted empty-path ba40.
    void (*ba40)(void* debug_obj, std::uint8_t* state, std::uint8_t* gate_byte) = nullptr;
    // Models vcall on a30 at +0x10. Null → no-op.
    void (*a30_vslot_10)(void* a30) = nullptr;
};

struct PrologueGates2552d4Result {
    bool ok = false;
    std::uint8_t gate_byte = 0;
    bool took_vslot_48 = false;
    bool ba40_ran = false;
    bool state_updated = false;
    bool a30_vslot_called = false;
};

// Process-wide stand-ins for BSS objects (gate flag obj, vtable obj, debug obj).
[[nodiscard]] std::uint8_t& prologue_gate_flag_byte() noexcept;   // *(*(0x2dfce70)+0x60)
[[nodiscard]] std::uint8_t& prologue_debug_state_byte() noexcept; // *(0x2eec840+0x60)
[[nodiscard]] void* prologue_debug_obj_2eec840() noexcept;

// Empty-path model of 0xba40 when +0x6a==0 and +0x70/+0x78 null.
void init_ba40_empty(void* debug_obj, std::uint8_t* state, std::uint8_t* gate_byte) noexcept;

// Models 0x2552d4..0x255318. a30 may be null (skips vslot+0x10).
[[nodiscard]] PrologueGates2552d4Result run_prologue_gates_2552d4(
    void* a30, const PrologueGates2552d4Hooks& hooks = {}) noexcept;

}  // namespace sotc
