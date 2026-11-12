// CPU-01-STARTUP-SLOT-567: native model of startup constructor index 567 / RVA 0x235d0.
//
// Original ELF sha256 d0723afb...5861d3. FDE [0x235d0, 0x23c84), 1,716 B; body sha256 is pinned by
// tools/lift/check_ctor_slot_567.py. Constructor table word 0x2dd70e0 - 8 * 567 = 0x2dd5f28 holds
// 0x235d0. Offline only: no original RVA is executed.
//
// Body shape (all values pinned from the original bytes):
//   * 347 instructions, 20 direct calls, 6 indirect calls, 17 conditional branches, one stack canary
//     (0x15fe518) and the usual `call __stack_chk_fail` tail at 0x23c7e.
//   * direct calls: 8 x 0xf590 (inert view release), 5 x 0xf8f390 (__cxa_atexit), 4 x 0xf2f0
//     (borrowed-string view from a C literal), 2 x 0x20d80 (register body, manager second argument)
//     and 1 x 0xf8f3a0 (stack check fail).
//   * indirect calls: 0x23380 (help pre-register callback), the command vtable slot +0x10
//     (0x1594dd0 + 0x10 = 0x20d60) for `help`/`core_unittests`, and the scratch vtable slot +0x18
//     (0x1594e98 + 0x18 = 0x235c0 for `help`, 0x1594e78 + 0x18 = 0x23370 for `core_unittests`).
//   * the body builds four 0xa0-byte console commands and registers all four.
//
// ================================ object map (verified strides) ================================
//   0x2dfcbc8  +0x000  0x008  the ConCmdManager cell `[0x2dfcbc8]`, written 0 by 0x23612
//   0x2dfcbd0  +0x008  0x0a0  command 0, "executeCmd"
//   0x2dfcc70  +0x0a8  0x0a0  command 1, "getValue"
//   0x2dfcd10  +0x148  0x0a0  command 2, "help"
//   0x2dfcdb0  +0x1e8  0x0a0  command 3, "core_unittests"
//   0x2dfce50  +0x288          the global pending list head, immediately after the block
//
// The commands are contiguous (no per-command head); the earlier scoping note that each command sat
// behind its own pending-list head was wrong. The manager cell is the only extra qword.
//
// Each command is a 0x60-byte ConCmdBase followed by the 0x40-byte derived tail:
//   +0x00  vptr          written twice: 0x1594d90 (0x1594d80 + 0x10) then 0x1594dd0 (0x1594dc0 + 0x10)
//   +0x08  pending link  NOT written by the body; the register callee 0x20d80 links it
//   +0x18  registered    0 for every command; executeCmd/getValue then set it to 1 before the direct
//                        0x20d80 call, help/core_unittests leave it 0 for 0x20d60 to set
//   +0x19  3 bytes       NOT written
//   +0x1c  flags         2, 2, 2, 0
//   +0x20  name view     borrowed {literal, length, 0, null} from 0xf95f01/0xf95f4b/0xf95f7d/0xf96002
//   +0x40  description   borrowed view from 0xf95f0c/0xf95f54/0xf95f82/0xf96011 (62/40/19/22 chars)
//   +0x60  tail.self     &command + 0x60 (the self-rewrite when the handler is non-zero)
//   +0x68  tail.word68   0x1990
//   +0x70  tail.word70   0x2512a0
//   +0x78  tail.handler  0x22580, 0x225f0, 0x22750, 0x22a40
//   +0x80  16 zero bytes
//   +0x90  qword 0
//   +0x98  8 bytes       NOT written
//
// Written bytes: 0x21c of 0x288. Documented holes: 0x6c = 4 x (0x10 pending link + 3 pad19..1c +
// 8 tail gap98..a0). The register callee 0x20d80 additionally writes the pending link of each command
// it queues; that write belongs to the callee, not to this body, so `slot567_byte_written` describes
// the body's own stores only.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact): the manager-cell zero store, all four command objects (every field the
// body writes, in body order), the delegate self-rewrite, the eight atexit records' order and sites,
// the register-once flag handling, and the direct/guarded registration choice per command.
// HOOKS:
//   * `register_direct` (0x20d80) and `register_guarded` (0x20d60): the default implementations are
//     the native `register_concmd_body_20d80` / `register_concmd_20d60` from concmd_base. They queue
//     into the pending list while `[0x2dfcbc8]` is null, which is the whole startup path (E1070).
//     A null hook or a refused insert is a visible block, never a silent pass.
//   * `pre_register` (0x23380, `help` only): identity tag, default succeeds without effect.
//   * `scratch` (0x235c0 / 0x23370): identity tags, default succeeds without effect.
//   * `atexit`: records into ConCmdRegistry::atexit_log; the destructor bodies 0x21060 and 0x4b0 are
//     pinned and never executed.
//   * `release_view`: the 0xf590 releases of the two stack views; borrowed literals carry no control
//     counter, so the native release is exactly inert and returns true.
// NOT modelled: 0x20d80's manager (non-null) insert beyond the `ConCmdManagerContract` hook, the
// destructor bodies, and anything the four handlers 0x22580/0x225f0/0x22750/0x22a40 do.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot567Rva = 0x235d0;
inline constexpr std::size_t kSlot567Size = 0x6b4;             // 1716 B, FDE [0x235d0, 0x23c84)
inline constexpr std::uint64_t kSlot567EndRva = 0x23c84;
inline constexpr std::size_t kSlot567InstructionCount = 347;
inline constexpr std::size_t kSlot567DirectCallSites = 20;
inline constexpr std::size_t kSlot567IndirectCallSites = 6;
inline constexpr std::size_t kSlot567BranchSites = 17;
inline constexpr std::uint64_t kSlot567TableWordRva = 0x2dd5f28;   // 0x2dd70e0 - 567*8
inline constexpr std::uint32_t kSlot567TableIndex = 567;
inline constexpr std::uint8_t kSlot567PadByte = 0x90;

// Global block: manager cell + four contiguous commands, ending where the pending list starts.
inline constexpr std::uint64_t kSlot567BlockRva = 0x2dfcbc8;
inline constexpr std::size_t kSlot567BlockSize = 0x288;         // .. 0x2dfce50
inline constexpr std::size_t kSlot567CommandCount = 4;
inline constexpr std::size_t kSlot567CommandSize = 0xa0;
inline constexpr std::size_t kSlot567CommandWritten = 0x85;
inline constexpr std::size_t kSlot567BlockWritten = 0x21c;
inline constexpr std::size_t kSlot567BlockHoles = 0x6c;

// Shared callees.
inline constexpr std::uint64_t kSlot567AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot567DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot567CanaryRva = 0x15fe518;
inline constexpr std::uint64_t kSlot567StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot567ViewCtorRva = 0xf2f0;
inline constexpr std::uint64_t kSlot567ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot567RegisterBodyRva = 0x20d80;
inline constexpr std::uint64_t kSlot567RegisterGuardRva = 0x20d60;
inline constexpr std::uint64_t kSlot567HelpCallbackRva = 0x23380;

// Identity vtable address points (never dispatched by this model).
inline constexpr std::uint64_t kSlot567BaseVptr = 0x1594d90;    // 0x1594d80 + 0x10
inline constexpr std::uint64_t kSlot567CmdVptr = 0x1594dd0;     // 0x1594dc0 + 0x10
inline constexpr std::uint64_t kSlot567HelpScratchVptr = 0x1594e98;
inline constexpr std::uint64_t kSlot567HelpScratchRva = 0x235c0;
inline constexpr std::uint64_t kSlot567UnitScratchVptr = 0x1594e78;
inline constexpr std::uint64_t kSlot567UnitScratchRva = 0x23370;

// Delegate constants and destructor identity tags (recorded, never executed).
inline constexpr std::uint64_t kSlot567DelegateWord68 = 0x1990;
inline constexpr std::uint64_t kSlot567DelegateWord70 = 0x2512a0;
inline constexpr std::uint64_t kSlot567CellDtorRva = 0x21060;
inline constexpr std::uint64_t kSlot567CmdDtorRva = 0x4b0;

// Command literals (borrowed views; identity addresses of the original rodata).
inline constexpr std::uint64_t kSlot567NameRva0 = 0xf95f01;
inline constexpr std::uint64_t kSlot567NameRva1 = 0xf95f4b;
inline constexpr std::uint64_t kSlot567NameRva2 = 0xf95f7d;
inline constexpr std::uint64_t kSlot567NameRva3 = 0xf96002;
inline constexpr std::uint64_t kSlot567DescRva0 = 0xf95f0c;
inline constexpr std::uint64_t kSlot567DescRva1 = 0xf95f54;
inline constexpr std::uint64_t kSlot567DescRva2 = 0xf95f82;
inline constexpr std::uint64_t kSlot567DescRva3 = 0xf96011;

inline constexpr char kSlot567Name0[] = "executeCmd";
inline constexpr char kSlot567Name1[] = "getValue";
inline constexpr char kSlot567Name2[] = "help";
inline constexpr char kSlot567Name3[] = "core_unittests";
inline constexpr char kSlot567Desc0[] =
    "Executes the specified command. Params: <Cmd Name>, <Cmd Args>";
inline constexpr char kSlot567Desc1[] = "Prints the value of the specified ConVar";
inline constexpr char kSlot567Desc2[] = "Prints help message";
inline constexpr char kSlot567Desc3[] = "Execute core unit test";

struct Slot567CommandSpec {
    std::uint32_t index;
    const char* name;
    std::size_t name_len;
    const char* description;
    std::size_t description_len;
    std::uint64_t name_rva;
    std::uint64_t description_rva;
    std::uint32_t flags;
    std::uint64_t handler_rva;
    bool guarded_register;           // true: 0x20d60 through vtable +0x10; false: direct 0x20d80
    std::uint64_t pre_callback_rva;  // 0x23380 for `help`, 0 otherwise
    std::uint64_t scratch_vptr;      // 0 when the command has no scratch+0x18 call
    std::uint64_t scratch_rva;
    std::uint64_t atexit_site_rva;
};

inline constexpr std::array<Slot567CommandSpec, kSlot567CommandCount> kSlot567Commands = {{
    {0, kSlot567Name0, sizeof kSlot567Name0 - 1, kSlot567Desc0, sizeof kSlot567Desc0 - 1,
     kSlot567NameRva0, kSlot567DescRva0, 2, 0x22580, false, 0, 0, 0, 0x237a1},
    {1, kSlot567Name1, sizeof kSlot567Name1 - 1, kSlot567Desc1, sizeof kSlot567Desc1 - 1,
     kSlot567NameRva1, kSlot567DescRva1, 2, 0x225f0, false, 0, 0, 0, 0x2390a},
    {2, kSlot567Name2, sizeof kSlot567Name2 - 1, kSlot567Desc2, sizeof kSlot567Desc2 - 1,
     kSlot567NameRva2, kSlot567DescRva2, 2, 0x22750, true, kSlot567HelpCallbackRva,
     kSlot567HelpScratchVptr, kSlot567HelpScratchRva, 0x23aac},
    {3, kSlot567Name3, sizeof kSlot567Name3 - 1, kSlot567Desc3, sizeof kSlot567Desc3 - 1,
     kSlot567NameRva3, kSlot567DescRva3, 0, 0x22a40, true, 0, kSlot567UnitScratchVptr,
     kSlot567UnitScratchRva, 0x23c56},
}};

// The derived 0x40-byte tail of every command.
struct Slot567Tail {
    std::uint64_t self;                     // +0x60  &command + 0x60
    std::uint64_t word68;                   // +0x68  0x1990
    std::uint64_t word70;                   // +0x70  0x2512a0
    std::uint64_t handler;                  // +0x78  the command's handler RVA
    std::array<std::uint64_t, 2> zero80;    // +0x80  16 zero bytes
    std::uint64_t word90;                   // +0x90  0
    std::uint8_t gap98[8];                  // +0x98  never written
};
static_assert(sizeof(Slot567Tail) == 0x40);
static_assert(offsetof(Slot567Tail, self) == 0x00 && offsetof(Slot567Tail, word68) == 0x08 &&
              offsetof(Slot567Tail, word70) == 0x10 && offsetof(Slot567Tail, handler) == 0x18 &&
              offsetof(Slot567Tail, zero80) == 0x20 && offsetof(Slot567Tail, word90) == 0x30 &&
              offsetof(Slot567Tail, gap98) == 0x38);

struct Slot567Command {
    ConCmdBase base{};      // +0x00  the modelled console-command base
    Slot567Tail tail{};     // +0x60
};
static_assert(sizeof(Slot567Command) == kSlot567CommandSize);
static_assert(offsetof(Slot567Command, tail) == 0x60);

struct Slot567Objects {
    std::uint64_t manager{};                       // +0x000  0x2dfcbc8
    std::array<Slot567Command, kSlot567CommandCount> commands{};
};
static_assert(sizeof(Slot567Objects) == kSlot567BlockSize);
static_assert(offsetof(Slot567Objects, commands) == 0x008);
static_assert(kSlot567BlockRva + 0x008 == 0x2dfcbd0);
static_assert(kSlot567BlockRva + kSlot567BlockSize == 0x2dfce50);

enum class Slot567Stage : std::uint8_t {
    none,
    hooks,
    cell,
    cell_atexit,
    build,
    pre_register,
    register_command,
    scratch,
    release,
    command_atexit,
    canary,
};

struct Slot567Result {
    bool ok = false;
    Slot567Stage blocked = Slot567Stage::none;
    std::uint32_t blocked_command = 0;
    std::uint32_t commands_built = 0;
    std::uint32_t registered = 0;
    std::uint32_t atexit_records = 0;
    bool canary_checked = false;
};

// Every hook is mandatory; a null pointer blocks at Slot567Stage::hooks. The defaults are the native
// registry paths and inert identity tags (see the header's modelled-vs-hook section).
struct Slot567Hooks {
    bool (*register_direct)(ConCmdBase& cmd, void* manager, ConCmdRegistry& registry) noexcept = nullptr;
    bool (*register_guarded)(ConCmdBase& cmd, ConCmdRegistry& registry) noexcept = nullptr;
    bool (*pre_register)(ConCmdBase& cmd) noexcept = nullptr;
    bool (*scratch)(std::uint32_t index, const SharedStr& view) noexcept = nullptr;
    bool (*atexit)(std::uint64_t dtor_rva, const void* object, ConCmdRegistry& registry) noexcept = nullptr;
    bool (*release_view)(const SharedStr& view) noexcept = nullptr;
};

[[nodiscard]] Slot567Hooks default_slot567_hooks() noexcept;

// Builds and registers all four commands. Writes exactly the bytes the body writes (the register
// callee may additionally link `base.pending`; that is registry state, not this body's store).
[[nodiscard]] Slot567Result init_slot567_235d0(Slot567Objects& objects, ConCmdRegistry& registry,
                                               const Slot567Hooks& hooks) noexcept;

// True when the body's own store program covers this offset of the 0x288-byte block.
[[nodiscard]] bool slot567_byte_written(std::size_t offset) noexcept;

[[nodiscard]] const char* slot567_stage_name(Slot567Stage stage) noexcept;
[[nodiscard]] const Slot567CommandSpec& slot567_command(std::uint32_t index) noexcept;

// Derived-tail write used by init_slot567_235d0; exposed for the fixture's field checks.
void init_slot567_command(Slot567Command& command, const Slot567CommandSpec& spec) noexcept;

}  // namespace sotc
