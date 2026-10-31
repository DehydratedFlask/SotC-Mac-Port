// CPU-01-STARTUP-SLOTS-M5 (lane M5): native model of startup constructor slot 535 (RVA 0x138e20, 96 bytes).
// Original ELF sha256 d0723afb...5861d3; the constructor-table word for slot 535 is at 0x2dd70e0 - 535 * 8. Every
// body fact below was read end to end from the pinned bytes and is byte-pinned by tools/lift/check_ctor_slot_535.py.
// NOTHING here executes, loads or calls an original RVA: destructor RVAs are IDENTITY TAGS, recorded and never run.
//
// Shape: (1) 0x3b310(0x2ee14f0) -- the 106 B critical-section constructor. OBSERVATION: it is a straight-line shim
// over the imports scePthreadMutexattrInit, scePthreadMutexattrSettype(attr, 2), scePthreadMutexInit(obj, attr,
// "CritSec") and scePthreadMutexattrDestroy (the four import PLT targets and the string are byte-pinned);
// __cxa_atexit(0x3b380, mutex) where 0x3b380 is a `jmp scePthreadMutexDestroy`. (2) the object 0x2ee14f8 (0x20 B):
// bytes +0x00..+0x07 are NEVER written; +0x08..+0x17 get one 16 B zero store and +0x18..+0x1f one qword zero;
// __cxa_atexit(0x138370, obj) -- 0x138370 (50 B) frees [obj+8] through 0x170b0 and re-zeroes 0x18 B (a tag here).
//
// NATIVE STAND-IN (INFERENCE, stated openly): the PS4 mutex handle is replaced by `CritSecHandle`
// (core/critsec_3b310): 8-byte opaque token + sidecar plain recursive mutex (no priority inheritance).
// Type 2 is read as PTHREAD_MUTEX_RECURSIVE (FreeBSD/PS4 inference, not observed in the image).
// The "CritSec" debug name has no host counterpart. Consumers of the lock and of the object remain hooks.
#pragma once

#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "critsec_3b310.hpp"

namespace sotc {

inline constexpr std::uint64_t kLaneM5Slot535Rva = 0x138e20;
inline constexpr std::size_t kLaneM5Slot535Size = 96;
inline constexpr std::uint64_t kLaneM5Slot535MutexObjectRva = 0x2ee14f0;
inline constexpr std::uint64_t kLaneM5Slot535ObjectRva = 0x2ee14f8;
inline constexpr std::uint64_t kLaneM5Slot535MutexCtorRva = 0x3b310;
inline constexpr std::uint64_t kLaneM5Slot535MutexDtorRva = 0x3b380;
inline constexpr std::uint64_t kLaneM5Slot535ObjectDtorRva = 0x138370;
inline constexpr std::size_t kLaneM5Slot535ObjectSize = 0x20;

enum class Slot535Stage : std::uint8_t { none, mutex_init, mutex_atexit, object_atexit };

struct Slot535Objects {
    CritSecHandle mutex;                         // 0x2ee14f0 (8 B opaque handle)
    std::uint8_t object[kLaneM5Slot535ObjectSize];      // 0x2ee14f8 (+0x00 never written)
};

struct Slot535Result {
    bool ok = false;
    Slot535Stage blocked = Slot535Stage::none;
    int mutex_status = 0;                         // RecursiveMutex::initialize() return (0 = success)
};

[[nodiscard]] Slot535Result init_slot535_138e20(Slot535Objects& objects, StartupAtexitLog& log) noexcept;

}  // namespace sotc
