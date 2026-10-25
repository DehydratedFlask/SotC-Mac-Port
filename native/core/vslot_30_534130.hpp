// App vtable slot +0x30 → OBS `0x534130` (294 B; bool gate on Base Initialize).
//
// Always-on on measured Init success path (base 0x255130 @ vcall +0x30). Prefer over
// 0x533100 (3956 B), 0xf08, ensure migrate, non-null list.
//
// OBS:
//   if (!0x255f60(self)) return false;
//   table = self+8;
//   lookup "boss" / "shadow_jumpToBoss" → optional 0x1e5e0 → cells 0x325e8b4/0x325e8b8/0x2dcdf14
//   lookup "boss_outro" → optional 0x1e5e0 → same cells (mode=1, value+1)
//   lookup "stage" → optional 0x1e5e0 → cell 0x2dd9578; return true
//   (lookups miss → still return true after precheck)
//
// Claimed: 0x534130 control flow + host stand-in cells + string name pins.
// NOT modelled: bodies of 0x255f60 / 0x1fc80 / 0x1e5e0; vslot 0x533100; mid 0x255340;
// 0xf08. PR #5 cite-only. No leaf-vec; not idx 95.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVslot30_534130Rva = 0x534130;
inline constexpr std::size_t kVslot30_534130Size = 294;
inline constexpr std::uint64_t kVslot30Precheck255f60Rva = 0x255f60;
inline constexpr std::uint64_t kVslot30Lookup1fc80Rva = 0x1fc80;
inline constexpr std::uint64_t kVslot30Parse1e5e0Rva = 0x1e5e0;
inline constexpr std::uint64_t kVslot30CellBossValRva = 0x325e8b4;
inline constexpr std::uint64_t kVslot30CellBossModeRva = 0x325e8b8;
inline constexpr std::uint64_t kVslot30CellBossFlagRva = 0x2dcdf14;
inline constexpr std::uint64_t kVslot30CellStageRva = 0x2dd9578;

inline constexpr const char kVslot30NameBoss[] = "boss";
inline constexpr const char kVslot30NameJump[] = "shadow_jumpToBoss";
inline constexpr const char kVslot30NameOutro[] = "boss_outro";
inline constexpr const char kVslot30NameStage[] = "stage";

struct Vslot30_534130Hooks {
    // Models 0x255f60(self). Null → true (measured empty / skip heavy precheck).
    bool (*precheck_255f60)(void* self) = nullptr;
    // Models 0x1fc80(table=self+8, name). Null → nullptr (miss).
    void* (*lookup_1fc80)(void* table, const char* name) = nullptr;
    // Models 0x1e5e0(node, 0, 0, 0). Null → 0.
    int (*parse_1e5e0)(void* node) = nullptr;
};

struct Vslot30_534130Result {
    bool ok = false;  // return value of 0x534130
    bool precheck_ran = false;
    bool precheck_ok = false;
    int lookups = 0;
    int parses = 0;
};

// Process-wide stand-ins for OBS BSS cells written by 0x534130.
[[nodiscard]] std::int32_t& vslot30_cell_boss_val_325e8b4() noexcept;
[[nodiscard]] std::int32_t& vslot30_cell_boss_mode_325e8b8() noexcept;
[[nodiscard]] std::uint8_t& vslot30_cell_boss_flag_2dcdf14() noexcept;
[[nodiscard]] std::int32_t& vslot30_cell_stage_2dd9578() noexcept;

// Models 0x534130(rdi=self). Requires self with readable +8 table region when lookups used.
[[nodiscard]] Vslot30_534130Result vslot_30_534130(void* self,
                                                   const Vslot30_534130Hooks& hooks = {}) noexcept;

}  // namespace sotc
