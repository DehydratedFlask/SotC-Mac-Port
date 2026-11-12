// CPU-01-STARTUP-SLOT-524: native model of startup constructor slot 524 (RVA 0x16a810).
//
// Original ELF sha256 d0723afb...5861d3. FDE [0x16a810, 0x16aae7), 727 B (last byte the trailing nop
// 0x90); body sha256 efb3dbda41ab7cca2e7d9215c5d5c07571a99eed596010fa61358a3f3e165dd3. Constructor
// table word 0x2dd70e0 - 8 * 524 = 0x2dd6080 holds 0x16a810. This is the `screenshot` command.
//
// Body map (every address from the pinned bytes; all callbacks/vtables/destructors are IDENTITY
// TAGS and are never executed):
//
//   0x16a828..0x16a848  guard <- *[*[0x15fe518]] at rsp+0xe0
//   0x16a82f..0x16a857  qword 0x2ee3ce0 = 0; __cxa_atexit(0x166f20, 0x2ee3ce0, &__dso_handle)
//   0x16a85c..0x16a86d  name view through 0xf2f0 at rsp+0x40: "screenshot" (0xfaba58, 10)
//   0x16a872..0x16a8a2  description view at rsp+0x20: {0xfaba63, 0x37, 0, 0}
//                       ("Take a screenshot. Params: [+4x] [<filename or folder>]")
//   0x16a87d..0x16a8db  delegate staging at rsp+0x00: {self=&staging, 0x1990, 0, 0x169be0};
//                       scratch words at rsp+0xd0 = 0x1598780, rsp+0xd8 = 0x16a1f0
//   0x16a8e8            0x20af0(command, &name, &description, flags 2)
//   0x16a8ed..0x16a91b  command vptr = 0x1594dd0; the 32-byte delegate is copied to +0x60; the
//                       fix-up tests the handler word (nonzero) and rewrites +0x60 from the stack
//                       self to &command+0x60
//   0x16a922..0x16a946  +0x80/+0x88 zeroed (16 B), +0x90 = 0 (+0x98 NOT written), then the
//                       vtable slot +0x10 call 0x20540(vector, 2) reserves two 0x50 B tokens
//   0x16a94b..0x16a994  token 0 through 0x16a380: name "resolution" (0xfabc04, 10), key 0 at
//                       rsp+0x84. The helper writes kind 1, the name view, the 0xf180 empty view,
//                       key 0 at +0x48, then replaces +0x28 with the measured enum label "Off".
//   0x16a999            0xf590(&resolution) (inert)
//   0x16a9a1..0x16aa3c  token 1 inline push: kind 4, name "filename" (0xfabc0f, 8), empty view at
//                       +0x28 through 0xf180, then 0xf730 + a conditional 0xf590/copy replacing that
//                       view by the 0xf730 empty static. Token 1's +0x48 dword is NOT written.
//   0x16aa7e..0x16aa8c  vptr +0x10 = 0x20d60 register-once (manager cell null => pending list)
//   0x16aa8f..0x16aa97  call [0x1598780 + 0x18] = 0x16a5d0 (a one-byte `ret`) on the rsp+0xb0
//                       scratch; the scratch's +0x00..+0x1f are not written
//   0x16aa9a/0x16aaa4  0xf590(description), 0xf590(name), both borrowed therefore inert
//   0x16aaae..0x16aabf  __cxa_atexit(0x4b0, command, &__dso_handle)
//   0x16aac4..0x16aae6  guard compare; mismatch -> 0xf8f3a0 __stack_chk_fail; trailing nop
//
// Object map: cell 8 B at 0x2ee3ce0, command 0xa0 at 0x2ee3ce8. The two 0x50 B tokens live in the
// command's +0x80 vector; the original gets that storage from 0x20540, so the native fixture owns
// the token array. The scratch is 0x30 B with the vtable tag at +0x20 and the tag at +0x28; only
// those two words are stored, and only the vtable's +0x18 slot (a `ret`) is invoked.
//
// The helper 0x16a380 is modelled natively in core/vector_append_16a380 (see its header), together
// with the four-row virtual-resolution label table at 0x2ee3da0 and its guard byte 0x2ee3e00.
// Nothing here loads, relocates or executes an original RVA; 0x166f20, 0x169be0, 0x16a1f0,
// 0x16a5d0 and the vtable words are identity data only.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "fconcmd_object_slot306.hpp"
#include "hashed_string_14c3a0.hpp"
#include "vector_append_16a380.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot524Rva = 0x16a810;
inline constexpr std::size_t kSlot524Size = 0x2d7;   // 727 B
inline constexpr std::uint64_t kSlot524TableWordRva = 0x2dd6080;
inline constexpr std::uint64_t kSlot524CanaryCellRva = 0x15fe518;
inline constexpr std::uint32_t kSlot524CanaryOffset = 0xe0;
inline constexpr std::uint64_t kSlot524CanaryBranchRva = 0x16aad0;   // jne 0x16aae1
inline constexpr std::uint64_t kSlot524CanaryTargetRva = 0x16aae1;
inline constexpr std::uint64_t kSlot524StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint32_t kSlot524FrameBytes = 0x100;

inline constexpr std::uint64_t kSlot524CellRva = 0x2ee3ce0;
inline constexpr std::uint64_t kSlot524CellDtorRva = 0x166f20;   // identity tag, 46 B
inline constexpr std::size_t kSlot524CellDtorSize = 0x2e;
inline constexpr std::uint64_t kSlot524CommandRva = 0x2ee3ce8;
inline constexpr std::uint64_t kSlot524DsoRva = 0x1604218;

inline constexpr std::uint64_t kSlot524NameViewRva = 0xf2f0;
inline constexpr std::size_t kSlot524NameViewSize = 40;
inline constexpr std::uint64_t kSlot524EmptyStaticRva = 0xf730;
inline constexpr std::size_t kSlot524EmptyStaticSize = 0x72;
inline constexpr std::uint64_t kSlot524EmptyDataRva = 0x2dd9f78;
inline constexpr std::uint64_t kSlot524ReserveRva = 0x20540;
inline constexpr std::size_t kSlot524ReserveSize = 253;
inline constexpr std::uint64_t kSlot524GrowRva = 0x20640;
inline constexpr std::size_t kSlot524GrowSize = 342;
inline constexpr std::uint64_t kSlot524ReleaseRva = 0xf590;
inline constexpr std::size_t kSlot524ReleaseSize = 73;
inline constexpr std::uint64_t kSlot524AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot524CommandVtableRva = 0x1594dc0;
inline constexpr std::uint64_t kSlot524CommandVptrRva = 0x1594dd0;
inline constexpr std::uint64_t kSlot524CommandRegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot524CommandThunkCellRva = 0x15fe5e0;
inline constexpr std::uint64_t kSlot524CommandTail8 = 0x1990;
inline constexpr std::uint64_t kSlot524CommandHandlerRva = 0x169be0;
inline constexpr std::uint64_t kSlot524CommandDtorRva = 0x4b0;
inline constexpr std::size_t kSlot524CommandDtorSize = 113;
inline constexpr std::uint32_t kSlot524CommandFlags = 2;

inline constexpr std::uint64_t kSlot524ScratchVtableRva = 0x1598780;
inline constexpr std::uint64_t kSlot524ScratchFnRva = 0x16a1f0;
inline constexpr std::uint64_t kSlot524ScratchVcallRva = 0x16a5d0;
inline constexpr std::array<std::uint64_t, 8> kSlot524ScratchVtableWords{
    0x16a5a0, 0x16a5b0, 0x16a5c0, 0x16a5d0, 0, 0, 0x16f550, 0x16f720};
inline constexpr std::size_t kSlot524ScratchSize = 0x30;

inline constexpr char kSlot524Name[] = "screenshot";
inline constexpr std::uint64_t kSlot524NameRva = 0xfaba58;
inline constexpr std::size_t kSlot524NameLength = 10;
inline constexpr char kSlot524Description[] = "Take a screenshot. Params: [+4x] [<filename or folder>]";
inline constexpr std::uint64_t kSlot524DescriptionRva = 0xfaba63;
inline constexpr std::size_t kSlot524DescriptionLength = 0x37;
static_assert(sizeof kSlot524Name - 1 == kSlot524NameLength);
static_assert(sizeof kSlot524Description - 1 == kSlot524DescriptionLength);

inline constexpr char kSlot524ResolutionName[] = "resolution";
inline constexpr std::uint64_t kSlot524ResolutionRva = 0xfabc04;
inline constexpr std::size_t kSlot524ResolutionLength = 10;
inline constexpr char kSlot524FilenameName[] = "filename";
inline constexpr std::uint64_t kSlot524FilenameRva = 0xfabc0f;
inline constexpr std::size_t kSlot524FilenameLength = 8;
static_assert(sizeof kSlot524ResolutionName - 1 == kSlot524ResolutionLength);
static_assert(sizeof kSlot524FilenameName - 1 == kSlot524FilenameLength);

inline constexpr std::uint32_t kSlot524FirstKey = 0;
inline constexpr std::uint32_t kSlot524FirstKind = 1;    // 0x16a3ac staging
inline constexpr std::uint32_t kSlot524SecondKind = 4;   // 0x16a9cf staging
inline constexpr std::size_t kSlot524TokenCount = 2;
inline constexpr std::size_t kSlot524AtexitCount = 2;    // cell + command
inline constexpr std::size_t kSlot524PendingCount = 1;   // one command registration
inline constexpr std::size_t kSlot524MenuCount = 0;

struct Slot524Cell {
    std::uint64_t word;   // +0x00
};
static_assert(sizeof(Slot524Cell) == 8);

// 0x30 B stack scratch at rsp+0xb0. Only +0x20 and +0x28 are stored by the body.
struct Slot524Scratch {
    std::uint64_t gap00[4];   // +0x00..+0x1f not written
    std::uint64_t vtable;     // +0x20 = 0x1598780
    std::uint64_t tag;        // +0x28 = 0x16a1f0
};
static_assert(sizeof(Slot524Scratch) == kSlot524ScratchSize &&
              offsetof(Slot524Scratch, vtable) == 0x20 && offsetof(Slot524Scratch, tag) == 0x28);

struct Slot524Objects {
    Slot524Cell cell{};                            // 0x2ee3ce0
    ConCmdCommand command{};                       // 0x2ee3ce8 (0xa0)
    std::array<ConCmdArgToken, kSlot524TokenCount> tokens{};   // host storage for 0x20540
    Slot524Scratch scratch{};                      // stack scratch at rsp+0xb0
};
static_assert(offsetof(Slot524Objects, cell) == 0 && offsetof(Slot524Objects, command) == 8 &&
              offsetof(Slot524Objects, tokens) == 8 + sizeof(ConCmdCommand) &&
              offsetof(Slot524Objects, scratch) ==
                  8 + sizeof(ConCmdCommand) + kSlot524TokenCount * sizeof(ConCmdArgToken));
static_assert(sizeof(Slot524Objects) ==
              8 + sizeof(ConCmdCommand) + kSlot524TokenCount * sizeof(ConCmdArgToken) +
                  sizeof(Slot524Scratch));
static_assert(kSlot524CellRva + sizeof(Slot524Cell) == kSlot524CommandRva);

struct Slot524Hooks {
    // 0x15fe518. Null => the cookie is not claimed; a changed non-null cookie fails at `canary`.
    const std::uint64_t* stack_guard = nullptr;
    // The 0x1598780 + 0x18 call lands on the one-byte `ret` 0x16a5d0. A host may observe it; null
    // records the no-op without executing guest code.
    bool (*noop_vcall)(void* context, Slot524Scratch& scratch) noexcept = nullptr;
    void* noop_context = nullptr;
    // 0x20640, used only if a host shrinks the 0x20540 reserve (never at this site). Passed through
    // to the helper so a forced grow fails closed or completes under an explicit contract.
    bool (*grow)(void* context, ConCmdCommand& owner, const SharedStr& name,
                 std::uint32_t kind) noexcept = nullptr;
    void* grow_context = nullptr;
};

enum class Slot524Stage : std::uint8_t {
    none,
    cell_atexit,
    name_view,
    command_build,
    delegate_store,
    reserve,
    token0_append,
    token0_release,
    token1_push,
    token1_normalize,
    registration,
    scratch_call,
    description_release,
    name_release,
    command_atexit,
    canary,
};
[[nodiscard]] const char* slot524_stage_name(Slot524Stage stage) noexcept;

struct Slot524Result {
    bool ok = false;
    Slot524Stage blocked = Slot524Stage::none;
    std::uint32_t blocked_index = 0;
    std::size_t atexit_records = 0;
    bool reserved = false;
    std::size_t reserve_count = 0;
    std::size_t tokens = 0;
    VectorAppendResult16a380 first{};
    bool first_key_written = false;
    bool first_label_off = false;
    bool second_inline_push = false;
    bool second_flag_written = false;   // token 1 +0x48 is NOT written by the body
    bool empty_normalized = false;      // token 1 +0x28 replaced by the 0xf730 static
    ConCmdRegisterResult registration{};
    bool registered = false;
    bool noop_vcall = false;
    bool canary_checked = false;
    bool canary_matched = false;
    bool views_inert = true;
};

// Runs original constructor 524 natively. The original is unconditional, so the first failing step
// stops the slot and names its stage. `objects.tokens` is the host storage the reserve stands for.
[[nodiscard]] Slot524Result init_slot524_16a810(Slot524Objects& objects, ConCmdRegistry& registry,
                                                const Slot524Hooks& hooks = {}) noexcept;

}  // namespace sotc
