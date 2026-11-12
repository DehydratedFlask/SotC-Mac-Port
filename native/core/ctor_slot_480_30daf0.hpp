// CPU-01-STARTUP-BATCH-10: native model of startup constructor slot 480 (RVA 0x30daf0, 1907 bytes). Original ELF sha256 d0723afb...5861d3,
// constructor-table word at 0x2dd70e0 - 8 * 480. Every body fact below is byte-pinned by tools/lift/check_ctor_slot_480.py. NOTHING here executes,
// loads or calls an original RVA: vtables, functor tags, handler tags and destructor RVAs are IDENTITY TAGS, recorded (the atexit log) and never run.
//
// The survey flagged "3 functor slot-3 sites and 2 register indirect calls". Both are resolved statically: the three `callq *0x18(%rax)` are the float
// caller-functor destroy of tag 0x15a66b8 (the four float bodies are pinned by the checker's float_tags); the two `callq *%r15` (0x30e132 / 0x30e20d)
// call r15 = `movq 0x20(%rax), %r15` with rax = `leaq 0x1594dc0(%rip)`, the console-command register-once virtual slot [0x1594dc0 + 0x20] = 0x20d60
// (the `fcmd_vtable` extra pins that relocation), the same shape as slots 306 / 339 / 340 / 342 / 343 / 359 / 388.
//
// Shape (an on-screen message debug page; the reading is INFERENCE from the strings). In order (object RVA):
//   f0   0x2eefe80  bounded float `renderMessageScale`  (range [0.1, 10], value = default 0.8, caller tag 0x15a66b8, flags 1), atexit 0x20f0
//   f1   0x2eeff58  bounded float `renderStatsScale`    (same range, 0.8), atexit 0x20f0
//   f2   0x2ef0030  bounded float `renderErrorScale`    (same range, 0.7), atexit 0x20f0
//   b0   0x2ef0108  bool variable `renderMessages`  (empty description, flags 1, value = default 1, caller tag 0x15a66d8), atexit 0x20b0
//   b1   0x2ef01d0  bool variable `renderErrorMsgs` (same shape), atexit 0x20b0
//   c0   0x2ef0298  console command `clearPersistentMessages` (description `Clears persistent messages from screen`, flags 2, handler tag 0x30d940), atexit 0x4b0
//   c1   0x2ef0338  console command `clearErrorMessages` (description `Clears error messages from screen`, flags 2, handler tag 0x30d960), atexit 0x4b0
// Registration order is the construction order. All strings are borrowed rodata views, so every 0xf590 release is an exact no-op. Handler tags are never
// called. The __stack_chk_guard compare is a hook. No consumer of the variables or of the commands is lifted.
#pragma once

#include <cstddef>
#include <cstdint>

#include "ctor_shapes_343_356.hpp"
#include "helper_int_var_sites.hpp"
#include "lane_m6_shapes.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot480Rva = 0x30daf0;
inline constexpr std::size_t kSlot480Size = 1907;
inline constexpr std::uint64_t kSlot480BoolFnTag = 0x15a66d8;
inline constexpr std::uint64_t kSlot480FloatFnTag = 0x15a66b8;
inline constexpr std::uint32_t kSlot480VarFlags = 1;
inline constexpr std::uint32_t kSlot480FloatLoBits = 0x3dcccccd;   // 0.1f
inline constexpr std::uint32_t kSlot480FloatHiBits = 0x41200000;   // 10.0f

inline constexpr std::size_t kSlot480FloatCount = 3;
inline constexpr char kSlot480FloatNames[kSlot480FloatCount][24] = {"renderMessageScale", "renderStatsScale", "renderErrorScale"};
inline constexpr std::size_t kSlot480FloatNameLengths[kSlot480FloatCount] = {0x12, 0x10, 0x10};
inline constexpr std::uint32_t kSlot480FloatValueBits[kSlot480FloatCount] = {0x3f4ccccd, 0x3f4ccccd, 0x3f333333};   // 0.8f, 0.8f, 0.7f (value = default)
inline constexpr std::size_t kSlot480BoolCount = 2;
inline constexpr char kSlot480BoolNames[kSlot480BoolCount][24] = {"renderMessages", "renderErrorMsgs"};
inline constexpr std::size_t kSlot480BoolNameLengths[kSlot480BoolCount] = {0xe, 0xf};
inline constexpr std::uint8_t kSlot480BoolValue = 1;
inline constexpr std::size_t kSlot480CommandCount = 2;
inline constexpr char kSlot480CommandNames[kSlot480CommandCount][32] = {"clearPersistentMessages", "clearErrorMessages"};
inline constexpr char kSlot480CommandDescriptions[kSlot480CommandCount][40] = {"Clears persistent messages from screen", "Clears error messages from screen"};
inline constexpr std::size_t kSlot480CommandDescriptionLengths[kSlot480CommandCount] = {0x26, 0x21};
inline constexpr std::uint64_t kSlot480HandlerTags[kSlot480CommandCount] = {0x30d940, 0x30d960};

inline constexpr std::uint64_t kSlot480Float0Rva = 0x2eefe80;
inline constexpr std::uint64_t kSlot480Bool0Rva = 0x2ef0108;
inline constexpr std::uint64_t kSlot480Command0Rva = 0x2ef0298;

struct Slot480Objects {
    ConCmdRangedVar fvar[kSlot480FloatCount];   // 0x2eefe80, 0x2eeff58, 0x2ef0030
    ConCmdBoolVar bvar[kSlot480BoolCount];      // 0x2ef0108, 0x2ef01d0
    ConCmdCommand command[kSlot480CommandCount];   // 0x2ef0298, 0x2ef0338
};

struct Slot480Result {
    bool ok = false;
    std::size_t rows_done = 0;   // f0, f1, f2, b0, b1, c0, c1 (7 on success)
    FConCmdBuildResult command[kSlot480CommandCount]{};
};

[[nodiscard]] Slot480Result init_slot480_30daf0(Slot480Objects& objects, ConCmdRegistry& concmd) noexcept;

}  // namespace sotc
