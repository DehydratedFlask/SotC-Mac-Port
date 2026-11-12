// CPU-01-STARTUP-SLOT-510: native model of startup constructor index 510 / RVA 0x211340.
//
// Original ELF sha256 d0723afb...5861d3. Body [0x211340, 0x211bf2), 0x8b2 B (2,226); last byte is the
// trailing nop 0x211bf1, then the 0x90 padding run that precedes the next constructor 0x211c00. Body
// sha256 is pinned by tools/lift/check_ctor_slot_510.py. Constructor table word
// 0x2dd70e0 - 8 * 510 = 0x2dd60f0 holds 0x211340. Offline only: NO original RVA is executed.
//
// Body facts (read end to end from the pinned bytes; every one is oracle-pinned):
//   * 377 instructions, 38 direct calls, 2 indirect calls, 14 branches (8 conditional, 6 jmp), one
//     stack canary (0x15fe518) and the `call __stack_chk_fail` tail at 0x211bec.
//   * direct calls: 14 x 0xf590 (inert view release), 6 x 0xf180 (empty view), 6 x 0x20640
//     (token grow, only reachable past the reserved capacity), 4 x 0xf8f390 (__cxa_atexit),
//     2 x 0x20af0 (ConCmd base ctor), 1 x 0x20d60 (FollowGameCamera register), 1 x 0x26ea0
//     (toggle item), 1 x 0xf2f0 (name view from a C literal), 1 x 0x20540 (token reserve),
//     1 x 0x210e00 (connection-token append), 1 x 0xf8f3a0 (__stack_chk_fail).
//   * indirect calls: the command vtable slot +0x10 (0x1594dd0 + 0x10 -> 0x20d60) for
//     SyncCameraAcrossTools, and the scratch table slot 0x159abc0 + 0x18 (0x2111d0, a bare `ret`).
//
// ================================ object map (verified strides) ================================
//   0x2ee8438  +0x000  0x030  string property cell `inputRequiresAppFocus` (vptr 0x15976b0)
//   0x2ee8468  +0x030  0x0c8  bool var `FollowGameCamera` (ConCmdBoolVar shape, +0xb0 tag 0x159aba0)
//   0x2ee8530  +0x0f8  0x0a4  debug-menu toggle `Debug Game` / `Follow Game Camera`, flags 0x21
//   0x2ee85d4  +0x19c  0x004  alignment padding, never written
//   0x2ee85d8  +0x1a0  0x030  camera pose defaults (two float4 + four id words)
//   0x2ee8608  +0x1d0  0x0a8  command `SyncCameraAcrossTools`, delegate and 7-token vector
// Total 0x278 B, ending exactly at startup slot 509's first object 0x2ee86b0.
//
// The body's own stores cover 0x211 of the 0x278 bytes; the 0x67 holes are: each ConCmdBase pending
// link (+0x08..+0x17) and the +0x19..+0x1b pad, the bool var's +0x6d..+0x6f / +0x90..+0x9f /
// +0xa8..+0xaf / +0xc1..+0xc7, the toggle item's +0x21..+0x27 / +0x4c..+0x4f, the 4-byte alignment
// pad, and the command's +0x98..+0x9f / +0xa0..+0xa7 (the +0xa0 byte is handler-owned at runtime and
// is never written by this constructor). The register callees 0x20d60/0x20d80 and 0x268b0 link list
// nodes; those writes are registry state, not this body's store program, exactly as at slot 567.
//
// ================================ modelled vs hook (G2) =================================
// MODELLED (native, exact): the property cell, the bool variable (pre/mid/final vptr sequence, value
// and default 0, functor tag 0x159aba0), the toggle item through 0x26ea0 + 0x268b0 with target gate
// {slot3 = 1, slot6 = 0} so the default range {1.0, 10.0} is kept, the 0x30-byte pose constant copy,
// the command base/delegate/self-rewrite, the 0x20540 reserve of seven 0x50 B tokens, token 0 through
// 0x210e00 (SendersConnectionType = 1 -> `Tool` from the pinned EConnectionType table) and the six
// inline kind-3 axis tokens (transX/Y/Z, eulerX/Y/Z), registration, scratch call, all fourteen view
// releases and the four atexit records in body order.
// HOOKS (mandatory; a null hook blocks before anything is written):
//   * register_guarded: default = native 0x20d60 (queues into the pending list while the manager cell
//     0x2dfcbc8 is null, the whole startup path).
//   * menu_toggle: default = native 0x26ea0 + 0x268b0 (root list, since the parent cell is null).
//   * connection_token / axis_token: default = the native token append helpers below. The connection
//     helper accepts only value 1 (`Tool`) and fails closed for any other enum row.
//   * atexit: records (0x8ef00, 0x20b0, 0x272d0, 0x4b0); bodies are pinned and never executed.
//   * release_view: 0xf590 on borrowed stack views; exactly inert.
//   * scratch_noop: the 0x2111d0 bare-ret call.
// NOT modelled: anything the handler 0x20e900 does, the bool variable's consumers, the EConnectionType
// rows other than 1, the register callees' list/manager inserts beyond their native defaults.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "concmd_base.hpp"
#include "concmd_vars.hpp"
#include "debug_menu_item.hpp"
#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kSlot510Rva = 0x211340;
inline constexpr std::size_t kSlot510Size = 0x8b2;             // 2,226 B
inline constexpr std::uint64_t kSlot510EndRva = 0x211bf2;
inline constexpr std::size_t kSlot510InstructionCount = 377;
inline constexpr std::size_t kSlot510DirectCallSites = 38;
inline constexpr std::size_t kSlot510IndirectCallSites = 2;
inline constexpr std::size_t kSlot510BranchSites = 14;
inline constexpr std::uint64_t kSlot510TableWordRva = 0x2dd60f0;   // 0x2dd70e0 - 510*8
inline constexpr std::uint32_t kSlot510TableIndex = 510;
inline constexpr std::uint8_t kSlot510PadByte = 0x90;

// Global block, exactly 0x278 B.
inline constexpr std::uint64_t kSlot510BlockRva = 0x2ee8438;
inline constexpr std::size_t kSlot510BlockSize = 0x278;
inline constexpr std::size_t kSlot510BlockWritten = 0x211;
inline constexpr std::size_t kSlot510BlockHoles = 0x67;
inline constexpr std::size_t kSlot510CellOffset = 0x000;
inline constexpr std::size_t kSlot510VarOffset = 0x030;
inline constexpr std::size_t kSlot510ItemOffset = 0x0f8;
inline constexpr std::size_t kSlot510PadOffset = 0x19c;
inline constexpr std::size_t kSlot510PoseOffset = 0x1a0;
inline constexpr std::size_t kSlot510CommandOffset = 0x1d0;
inline constexpr std::size_t kSlot510CellSize = 0x30;
inline constexpr std::size_t kSlot510VarSize = 0xc8;
inline constexpr std::size_t kSlot510ItemSize = 0xa4;
inline constexpr std::size_t kSlot510PadSize = 0x04;
inline constexpr std::size_t kSlot510PoseSize = 0x30;
inline constexpr std::size_t kSlot510CommandSize = 0xa8;

// Shared callees and the canary.
inline constexpr std::uint64_t kSlot510AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kSlot510DsoRva = 0x1604218;
inline constexpr std::uint64_t kSlot510CanaryRva = 0x15fe518;
inline constexpr std::uint64_t kSlot510StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint64_t kSlot510BaseCtorRva = 0x20af0;
inline constexpr std::uint64_t kSlot510RegisterRva = 0x20d60;
inline constexpr std::uint64_t kSlot510ViewCtorRva = 0xf2f0;
inline constexpr std::uint64_t kSlot510ReleaseRva = 0xf590;
inline constexpr std::uint64_t kSlot510EmptyViewRva = 0xf180;
inline constexpr std::uint64_t kSlot510ReserveRva = 0x20540;
inline constexpr std::uint64_t kSlot510GrowRva = 0x20640;
inline constexpr std::uint64_t kSlot510ConnectionTokenRva = 0x210e00;
inline constexpr std::uint64_t kSlot510SqlStubRva = 0x2111d0;      // bare `ret`
inline constexpr std::uint64_t kSlot510ItemCtorRva = 0x26ea0;
inline constexpr std::uint64_t kSlot510ItemRegisterRva = 0x268b0;

// Identity vtable / tag words (never dispatched by this model).
inline constexpr std::uint64_t kSlot510CellVptr = 0x15976b0;       // 0x15976a0 + 0x10
inline constexpr std::uint64_t kSlot510VarMidVptr = 0x15940b0;     // 0x15940a0 + 0x10
inline constexpr std::uint64_t kSlot510VarVptr = 0x1594218;        // 0x1594208 + 0x10
inline constexpr std::uint64_t kSlot510VarFnVtable = 0x159aba0;
inline constexpr std::uint64_t kSlot510CommandVptr = 0x1594dd0;    // 0x1594dc0 + 0x10
inline constexpr std::uint64_t kSlot510ScratchVtable = 0x159abc0;

// Destructors recorded through __cxa_atexit (pinned, never executed).
inline constexpr std::uint64_t kSlot510CellDtorRva = 0x8ef00;
inline constexpr std::uint64_t kSlot510VarDtorRva = 0x20b0;
inline constexpr std::uint64_t kSlot510ItemDtorRva = 0x272d0;
inline constexpr std::uint64_t kSlot510CommandDtorRva = 0x4b0;

// Literals (borrowed views; identity addresses of the original rodata).
inline constexpr std::uint64_t kSlot510PropertyRva = 0xfbedbc;
inline constexpr std::uint64_t kSlot510VarNameRva = 0xfbedd2;
inline constexpr std::uint64_t kSlot510VarDescRva = 0xfbf1ca;      // empty string
inline constexpr std::uint64_t kSlot510GroupRva = 0xfbede3;
inline constexpr std::uint64_t kSlot510ToggleRva = 0xfbedee;
inline constexpr std::uint64_t kSlot510CommandNameRva = 0xfbee01;
inline constexpr std::uint64_t kSlot510CommandDescRva = 0xfbee17;
inline constexpr std::uint64_t kSlot510TokenNameRva = 0xfbf1cb;

inline constexpr char kSlot510Property[] = "inputRequiresAppFocus";
inline constexpr char kSlot510VarName[] = "FollowGameCamera";
inline constexpr char kSlot510Group[] = "Debug Game";
inline constexpr char kSlot510Toggle[] = "Follow Game Camera";
inline constexpr char kSlot510CommandName[] = "SyncCameraAcrossTools";
inline constexpr char kSlot510CommandDesc[] = "Used to sync the camera across games and tools";
inline constexpr char kSlot510TokenName[] = "SendersConnectionType";
inline constexpr char kSlot510ConnectionLabel[] = "Tool";
inline constexpr std::array<const char*, 6> kSlot510AxisNames = {
    "transX", "transY", "transZ", "eulerX", "eulerY", "eulerZ"};
inline constexpr std::array<std::uint64_t, 6> kSlot510AxisRvas = {
    0xfbf1e1, 0xfbf1e8, 0xfbf1ef, 0xfbf1f6, 0xfbf1fd, 0xfbf204};

inline constexpr std::size_t kSlot510PropertyLength = sizeof kSlot510Property - 1;   // 21
inline constexpr std::size_t kSlot510VarNameLength = sizeof kSlot510VarName - 1;      // 16
inline constexpr std::size_t kSlot510GroupLength = sizeof kSlot510Group - 1;          // 10
inline constexpr std::size_t kSlot510ToggleLength = sizeof kSlot510Toggle - 1;        // 18
inline constexpr std::size_t kSlot510CommandNameLength = sizeof kSlot510CommandName - 1;   // 21
inline constexpr std::size_t kSlot510CommandDescLength = sizeof kSlot510CommandDesc - 1;   // 46
inline constexpr std::size_t kSlot510TokenNameLength = sizeof kSlot510TokenName - 1;       // 21
inline constexpr std::size_t kSlot510AxisNameLength = 6;

inline constexpr std::uint32_t kSlot510VarFlags = 1;
inline constexpr std::uint32_t kSlot510CommandFlags = 2;
inline constexpr std::uint32_t kSlot510ItemFlags = 0x21;
inline constexpr std::size_t kSlot510TokenCount = 7;
inline constexpr std::size_t kSlot510TokenSize = 0x50;
inline constexpr std::uint32_t kSlot510ConnectionTypeValue = 1;   // EConnectionType::Tool
inline constexpr std::uint64_t kSlot510DelegateWord68 = 0x1990;
inline constexpr std::uint64_t kSlot510DelegateWord70 = 0x2512a0;
inline constexpr std::uint64_t kSlot510HandlerRva = 0x20e900;
inline constexpr std::uint64_t kSlot510PoseIds[4] = {0xff1b6468u, 0xff1b6416u, 0xff1b640cu,
                                                     0xff1b6402u};

// The six EConnectionType table rows the constructor's token helper walks; row 1 is the only value
// the body requests (`Tool`).
struct Slot510ConnectionRow {
    const char* label;
    std::uint64_t label_rva;
    std::uint32_t value;
};
inline constexpr std::array<Slot510ConnectionRow, 6> kSlot510ConnectionRows = {{
    {"None", 0xfbeeb1, 0},
    {"Tool", 0xfbeee7, 1},
    {"GameConnection", 0xfbef1d, 2},
    {"MaterialEditor", 0xfbef67, 4},
    {"Maya", 0xfbeff7, 8},
    {"AllTools", 0xfbf02d, 0x10},
}};

// 0x30-byte string property cell.
struct Slot510Cell {
    std::uint64_t vptr;   // +0x00
    SharedStr text;       // +0x08
    std::uint64_t tail;   // +0x28
};
static_assert(sizeof(Slot510Cell) == kSlot510CellSize);

// 0x30-byte pose default block: two float4 words then four id words.
struct Slot510Pose {
    float vec0[4];              // +0x00  {1,0,0,0}
    float vec1[4];              // +0x10  {1,0,0,0}
    std::uint32_t ids[4];       // +0x20
};
static_assert(sizeof(Slot510Pose) == kSlot510PoseSize);

// Token element (0x50 B) as written by 0x210e00 / the six inline pushes. `hint` is written only by
// the 0x210e00 path (value 1 here); the six inline elements leave +0x48..+0x4f untouched.
struct Slot510Token {
    std::uint32_t type;      // +0x00  1 (0x210e00) or 3 (inline)
    std::uint8_t gap04[4];   // +0x04  never written
    SharedStr name;          // +0x08
    SharedStr value;         // +0x28  empty for the six axes, `Tool` for token 0
    std::uint32_t hint;      // +0x48
    std::uint8_t gap4c[4];   // +0x4c
};
static_assert(sizeof(Slot510Token) == kSlot510TokenSize &&
              offsetof(Slot510Token, name) == 0x08 && offsetof(Slot510Token, value) == 0x28 &&
              offsetof(Slot510Token, hint) == 0x48);

// Host storage for the command's +0x80 vector (the original gets it from 0x20540/0x20640).
struct Slot510TokenStore {
    std::array<Slot510Token, kSlot510TokenCount> items{};
    std::size_t count = 0;
    std::size_t capacity = 0;
};

struct Slot510Delegate {
    std::uint64_t self;      // +0x00  &command.delegate
    std::uint64_t word68;    // +0x08  0x1990
    std::uint64_t word70;    // +0x10  0x2512a0
    std::uint64_t handler;   // +0x18  0x20e900
};
static_assert(sizeof(Slot510Delegate) == 0x20);

struct Slot510Command {
    ConCmdBase base;          // +0x00
    Slot510Delegate delegate; // +0x60
    std::uint64_t vector_begin;   // +0x80
    std::uint64_t vector_end;     // +0x88
    std::uint64_t vector_cap;     // +0x90
    std::uint8_t gap98[8];        // +0x98 never written by the body
    std::uint8_t session_flag;    // +0xa0 handler-owned; NOT written by the body
    std::uint8_t gapa1[7];        // +0xa1 never written by the body
};
static_assert(sizeof(Slot510Command) == kSlot510CommandSize &&
              offsetof(Slot510Command, delegate) == 0x60 &&
              offsetof(Slot510Command, vector_begin) == 0x80 &&
              offsetof(Slot510Command, vector_cap) == 0x90 &&
              offsetof(Slot510Command, gap98) == 0x98 &&
              offsetof(Slot510Command, session_flag) == 0xa0);

// The whole block, byte-layout exact. `var` uses the shared ConCmdBoolVar shape, whose 8-byte
// alignment already pads the 0xc1-byte payload to the original 0xc8 stride; `item` is the shared
// 0xa4 B debug-menu item.
struct Slot510Objects {
    Slot510Cell cell{};             // +0x000  0x2ee8438
    ConCmdBoolVar var{};            // +0x030  0x2ee8468 (pad to 0xc8; +0xc1..c7 never written)
    DebugMenuItem item{};           // +0x0f8  0x2ee8530 (0xa4 payload + 4 B tail pad)
    Slot510Pose pose{};             // +0x1a0  0x2ee85d8
    Slot510Command command{};       // +0x1d0  0x2ee8608
};
static_assert(sizeof(Slot510Objects) == kSlot510BlockSize);
static_assert(offsetof(Slot510Objects, cell) == kSlot510CellOffset &&
              offsetof(Slot510Objects, var) == kSlot510VarOffset &&
              offsetof(Slot510Objects, item) == kSlot510ItemOffset &&
              offsetof(Slot510Objects, pose) == kSlot510PoseOffset &&
              offsetof(Slot510Objects, command) == kSlot510CommandOffset);
static_assert(kSlot510BlockRva + kSlot510BlockSize == 0x2ee86b0);

enum class Slot510Stage : std::uint8_t {
    none,
    hooks,
    property_cell,
    property_atexit,
    var_build,
    var_register,
    var_atexit,
    toggle,
    toggle_atexit,
    pose,
    command_build,
    command_reserve,
    token_connection,
    token_axes,
    register_command,
    scratch_call,
    release_views,
    command_atexit,
    canary,
};

struct Slot510Result {
    bool ok = false;
    Slot510Stage blocked = Slot510Stage::none;
    BoolVarResult var{};
    bool property_atexit = false;
    bool toggle_ok = false;
    bool toggle_atexit = false;
    bool pose_copied = false;
    bool command_built = false;
    bool delegate_rewritten = false;
    bool reserved = false;
    std::size_t tokens = 0;
    ConCmdRegisterResult command_registration{};
    bool registered = false;
    bool scratch_called = false;
    bool views_inert = false;
    std::size_t atexit_records = 0;
    bool canary_checked = false;
};

// Mandatory hooks; a null pointer blocks at Slot510Stage::hooks before any store.
struct Slot510Hooks {
    bool (*register_guarded)(ConCmdBase& cmd, ConCmdRegistry& registry) noexcept = nullptr;
    bool (*menu_toggle)(DebugMenuItem& item, const SharedStr& label, const SharedStr& text,
                        const DebugMenuTarget& target, std::uint32_t flags, DebugMenuRegistry& menu,
                        ConCmdRegistry& registry) noexcept = nullptr;
    bool (*connection_token)(Slot510TokenStore& store, const SharedStr& name,
                             std::uint32_t value) noexcept = nullptr;
    bool (*axis_token)(Slot510TokenStore& store, const SharedStr& name) noexcept = nullptr;
    bool (*atexit)(std::uint64_t dtor_rva, const void* object, ConCmdRegistry& registry) noexcept = nullptr;
    bool (*release_view)(const SharedStr& view) noexcept = nullptr;
    bool (*scratch_noop)() noexcept = nullptr;
};

[[nodiscard]] Slot510Hooks default_slot510_hooks() noexcept;

// The two token appenders, exposed so tests can use them under stub hooks.
[[nodiscard]] bool slot510_append_connection_token(Slot510TokenStore& store, const SharedStr& name,
                                                   std::uint32_t value) noexcept;
[[nodiscard]] bool slot510_append_axis_token(Slot510TokenStore& store, const SharedStr& name) noexcept;

// True when the body's own store program covers this byte of the 0x278-byte block.
[[nodiscard]] bool slot510_byte_written(std::size_t offset) noexcept;

[[nodiscard]] const char* slot510_stage_name(Slot510Stage stage) noexcept;

// Builds the whole block. Registration may additionally link list nodes (registry state).
[[nodiscard]] Slot510Result init_slot510_211340(Slot510Objects& objects, Slot510TokenStore& tokens,
                                                ConCmdRegistry& registry, DebugMenuRegistry& menu,
                                                const Slot510Hooks& hooks) noexcept;

}  // namespace sotc
