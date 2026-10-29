// CPU-01-STARTUP-SLOTS-M5 (lane M5): native model of startup constructor slot 540 (RVA 0x10e390, 488 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 540 is at 0x2dd70e0 - 540 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_540.py.
// NOTHING here executes, loads or calls an original RVA: the vptr, the entry writer 0xc1c70, the store helper
// 0x158340, and the destructor RVAs are IDENTITY TAGS (0xc1c70 and 0x158340 are 7 B leaves modelled in place).
//
// Shape, in order:
//   1. Ten {u32 id, (+4 never written), char* name} records at 0x2edef70 + 0x10 * k, each written by 0xc1c70
//      (`mov [rdi],esi; mov [rdi+8],rdx; ret`) with id 0x24 and the name:
//        ParticleGroup_Normal, _UltraLowRes, _BelowWater, _Distortion, _Fog, _WaterDisplace, _WaterDisplace_Micro,
//        _GroundDisplace, _RainDisplace, _ScreenOverlay          (names borrowed from the original rodata text)
//   2. rand() (libc import thunk 0xf90130) -> 0x158340(0x2edf010, value): u32 at +0 = the value, byte +4 = 0.
//   3. Name object 0x2edf018 (0x30 B, the release-family `name_object` shape): name {"showParticle_Errors", 19, 0, 0}
//      at +8, tail +0x28 = 0, vptr 0x15976b0 (0x15976a0 + 0x10) stored LAST; 0xf590 on the stack view (inert);
//      __cxa_atexit(0x8ef00, obj).
//   4. List sentinel 0x2edf048 = {self, self, 0}; __cxa_atexit(0x101ea0, sentinel) (0x101ea0 is a 58 B list-free walker).
// OBSERVATION: all of the above. INFERENCE (not modelled): the ids/names look like particle render groups and the
// random value like a per-run seed or token; their consumers are not lifted. rand() is a HOST CHOICE: the original
// calls the Sony libc rand (a different generator than the host's), so the value is supplied by a caller-chosen
// source, defaulting to std::rand; tests use a deterministic source.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "ctor_release_family_slots.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot540Rva = 0x10e390;
inline constexpr std::size_t kSlot540Size = 488;
inline constexpr std::uint64_t kSlot540TableRva = 0x2edef70;
inline constexpr std::size_t kSlot540EntryCount = 10;
inline constexpr std::uint32_t kSlot540EntryId = 0x24;
inline constexpr std::uint64_t kSlot540RandCellRva = 0x2edf010;
inline constexpr std::uint64_t kSlot540NameObjectRva = 0x2edf018;
inline constexpr std::uint64_t kSlot540SentinelRva = 0x2edf048;
inline constexpr std::uint64_t kSlot540EntryWriterRva = 0xc1c70;
inline constexpr std::uint64_t kSlot540StoreHelperRva = 0x158340;
inline constexpr std::uint64_t kSlot540NameVptr = 0x15976b0;
inline constexpr std::uint64_t kSlot540NameDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot540SentinelDtorRva = 0x101ea0;
inline constexpr char kSlot540ErrorsName[] = "showParticle_Errors";

inline constexpr char kSlot540Name0[] = "ParticleGroup_Normal";
inline constexpr char kSlot540Name1[] = "ParticleGroup_UltraLowRes";
inline constexpr char kSlot540Name2[] = "ParticleGroup_BelowWater";
inline constexpr char kSlot540Name3[] = "ParticleGroup_Distortion";
inline constexpr char kSlot540Name4[] = "ParticleGroup_Fog";
inline constexpr char kSlot540Name5[] = "ParticleGroup_WaterDisplace";
inline constexpr char kSlot540Name6[] = "ParticleGroup_WaterDisplace_Micro";
inline constexpr char kSlot540Name7[] = "ParticleGroup_GroundDisplace";
inline constexpr char kSlot540Name8[] = "ParticleGroup_RainDisplace";
inline constexpr char kSlot540Name9[] = "ParticleGroup_ScreenOverlay";

struct Slot540Entry {
    std::uint32_t id;          // +0x00
    std::uint8_t gap04[4];     // +0x04 never written
    const char* name;          // +0x08 borrowed literal
};
static_assert(sizeof(Slot540Entry) == 0x10 && offsetof(Slot540Entry, name) == 8);

struct Slot540Sentinel {
    void* next;                // +0x00 = &sentinel
    void* prev;                // +0x08 = &sentinel
    std::uint64_t count;       // +0x10 = 0
};
static_assert(sizeof(Slot540Sentinel) == 0x18);

struct Slot540Objects {
    Slot540Entry entries[kSlot540EntryCount];   // 0x2edef70
    std::uint8_t rand_cell[8];                  // 0x2edf010: u32 value, byte +4 cleared, +5..+7 never written
    ReleaseFamilyNameObject name_object;        // 0x2edf018
    Slot540Sentinel sentinel;                   // 0x2edf048
};
static_assert(offsetof(Slot540Objects, rand_cell) == 0xa0 && offsetof(Slot540Objects, name_object) == 0xa8 &&
              offsetof(Slot540Objects, sentinel) == 0xd8);

using Slot540RandSource = int (*)() noexcept;

enum class Slot540Stage : std::uint8_t { none, view_release, name_atexit, sentinel_atexit };

struct Slot540Result {
    bool ok = false;
    Slot540Stage blocked = Slot540Stage::none;
    std::uint32_t rand_value = 0;
};

// `rand_source == nullptr` selects std::rand.
[[nodiscard]] Slot540Result init_slot540_10e390(Slot540Objects& objects, StartupAtexitLog& log,
                                                Slot540RandSource rand_source = nullptr) noexcept;

}  // namespace sotc
