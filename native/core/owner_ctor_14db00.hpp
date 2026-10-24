// Row-3 owner constructor `0x14db00` = the `ResourcePool` constructor (`kOwnerMemberRows[3]`,
// 0x138 B heap object, pointer slot `0x2ee2cf8`, first argument `[0x2ee2d00]` = the row-1 engine
// object stored at `+0x08`).
//
// Characterization: this header plus `local/re/evidence/owner-row3-14db00/` (`stores.txt` = the
// llvm-objdump store/call census, `ghidra-census.txt` = Ghidra `-noanalysis -readOnly` census,
// `stores.py`); offline static decode only. The round-29 lane-C report carries the narrative. Body `0x14db00..0x14dc83` (FDE extent), size `0x183` (387 B), sha256
// `22e020bf87b5fd68100b7957ed762e95e6605ba8a8a0ab10a9705febe836b20a`, 94 decoded instructions
// (Ghidra: 93 + the trailing `nop`), eleven call sites (nine direct to seven distinct targets,
// plus two indirect calls that are both statically resolvable, see below). Thirteen `nop`s of
// padding follow, then the next
// function `0x14dc90` (which is also this object's registered `resource_dump` handler).
//
// Corrections to earlier summaries:
//   - E570 (`owner-rows-2569d0/report.md` §1.2/§1.4) lists this body as "`0x426`, 276 insns,
//     20 calls, 4 vtable dispatches in the ctor - blocked". The bytes say 0x183 / 94 insns /
//     11 call sites / 2 indirect calls (E570's walk fell through the noreturn `__stack_chk_fail`).
//     E570 also called it "blocked"; the callees below are small and mostly liftable.
//   - The `resource-pool` report (§4.1) says 386 B: that is the Ghidra extent without the single
//     trailing `nop` of the FDE; the FDE-based size used by the sibling lifts is 0x183.
//   - The two indirect calls are not data-dependent dispatch: (1) `call [0x1594dc0 + 0x20]` reads a
//     fixed `.data.rel.ro` slot = vtable `0x1594dd0` entry 2 = `0x20d60`; (2) `call r15` with
//     `r15 = [[this] + 0x50]`, where `[this]` is the vptr this very constructor stored at
//     `0x14db3b` (`0x1598628`), so the target is `0x1598628 + 0x50` -> `0x14e910`, always.
//
// What the body does (all native here, in instruction order):
//   1. `[this] = vptr 0x1598628` (host value), `+0x08 = engine`, the std::map header
//      (`+0x18 = +0x20 = this + 0x18`, `+0x28 = 0`, `+0x30..+0x3f = 0`, byte `+0x30 = 0`),
//      `+0x48..+0x57 = 0`, `+0x60..+0x6f = 0`.
//   2. `0x17070(0x60)` -> the second-tree anchor `A`: `[A] = [A+8] = [A+0x10] = A`, bytes
//      `[A+0x18] = [A+0x19] = 1`; `[this+0x60] = A`. (The rest of the 0x60 B node is untouched.)
//   3. `0x3b310(this + 0x70)` mutex init (8 B handle, hook); `[this+0x78] = 0`.
//   4. Two string holders on the stack: `0xf2f0(&A, 0, "resource_dump")` builds
//      `{ptr, strlen, 0, 0}` (native); the second is built inline as `{desc, 0x3e, 0, 0}`.
//   5. `0x20af0(this + 0x80, &A, &B, 2)` - the 0x60 B console-command base constructor (native):
//      `[+0] = vptr 0x1594d90`, byte `+0x18 = 0`, dword `+0x1c = flags`, the 0x20 B name copy at
//      `+0x20`, the 0x20 B description copy at `+0x40`, and `lock inc` of each non-null control.
//   6. `[this+0x80] = vptr 0x1594dd0` (overwrites the base vptr), `[this+0xe8] = 0x14dc90`
//      (the relocated slot `0x15feb58`, raw RVA of the handler - never called), `[this+0xf0] = 0`,
//      `[this+0xe0] = this`, `+0xf8..+0x117 = 0`.
//   7. `0x20d60(this + 0x80)`: `if (byte [cmd+0x18]) return; byte [cmd+0x18] = 1;` then it tail
//      jumps into `0x20d80(cmd, *(0x2dfcbc8))` (console-registry insert). The guard/flag store is
//      native here; the insert is the `console_register` hook (the host supplies the console).
//   8. `0xf590(&B)`, `0xf590(&A)` - native `release_shared_str_f590`; both holders have a null
//      control (proven by the stores above), so neither release can reach a deallocator (the
//      lift's deallocator would `std::terminate`, a contract violation, not a failure path).
//   9. byte `+0x120 = 0`, `+0x128..+0x137 = 0`.
//  10. `0x17070(0x28)` -> helper `H`; `0x1ab270(H)` (native): `[H] = vptr 0x1598ab0`,
//      `[H+0x10..+0x1f] = 0`, `[H+0x20] = 0` (`[H+8]` is a hole).
//  11. `0x14e910(this, H)` (native, shown devirtualized above): `C = 0x17090(0x20)`;
//      `[C+8] = 1`, `[C+0xc] = 1`, `[C] = vptr 0x15986f0`, `[C+0x10] = H`; `[this+0x48] = H`;
//      `old = [this+0x50]; [this+0x50] = C`. `old` is provably zero here (step 1 zeroed
//      `+0x48..+0x57` and nothing writes `+0x50` before this point), so the original's
//      shared-control release of `old` is dead on this path and is not modelled. `{H, C}` is a
//      `shared_ptr`-shaped pair (strong = weak = 1) by inference.
//
// Hooks (honest list): `0x17070`, `0x17090`, `0x3b310`, and `0x20d80` (the console registry
// insert, reached through `console_register`). Five host vptr values (guest vtables are never
// executed). A missing hook or a null host vptr is a named `failed_at`.
//
// Not modelled: the destructor (`0x14e170` deleting wrapper -> `0x14de00`, idiom (a)),
// the std::map header's later use (`0x150360` insert, `0x14e9b0` erase; `resource_pool.{hpp,cpp}`
// models the map semantics with std::map and does not use this guest layout), the second tree
// at `+0x58/+0x60`, `0x14dc90` (the `resource_dump` handler), and `0x20d80`.
#pragma once

#include <cstddef>
#include <cstdint>

#include "hashed_string_14c3a0.hpp"

namespace sotc {

inline constexpr std::uint64_t kOwnerCtor14DB00Rva = 0x14DB00;
inline constexpr std::size_t kOwnerCtor14DB00Size = 0x183;
inline constexpr std::size_t kOwnerCtor14DB00InsnCount = 94;
inline constexpr std::size_t kOwnerCtor14DB00ObjSize = 0x138;
inline constexpr std::uint64_t kOwnerCtor14DB00SlotRva = 0x2EE2CF8;  // pointer slot
inline constexpr std::uint64_t kOwnerCtor14DB00ArgRva = 0x2EE2D00;   // first argument is [this cell]
inline constexpr std::size_t kOwnerMemberRow3 = 3;

// Destructor contract only (idiom (a), no separate `0x170b0`): the row calls `[vptr + 8]` =
// `0x14e170`, a 28 B deleting wrapper = `0x14de00(this)` (the complete dtor, vtable entry 0) then
// `jmp 0x170b0`.
inline constexpr std::uint64_t kOwnerDtor14E170Rva = 0x14E170;
inline constexpr std::uint64_t kOwnerDtor14DE00Rva = 0x14DE00;

// Guest vtable addresses the body stores (+0x10 already added). Host values replace them.
inline constexpr std::uint64_t kOwnerRow3PoolVptrRva = 0x1598628;
inline constexpr std::uint64_t kOwnerRow3CommandBaseVptrRva = 0x1594D90;
inline constexpr std::uint64_t kOwnerRow3CommandVptrRva = 0x1594DD0;
inline constexpr std::uint64_t kOwnerRow3ControlVptrRva = 0x15986F0;
inline constexpr std::uint64_t kOwnerRow3HelperVptrRva = 0x1598AB0;
inline constexpr std::uint64_t kOwnerRow3CommandRegisterRva = 0x20D60;  // vtable 0x1594dd0, entry 2
inline constexpr std::uint64_t kOwnerRow3SetHelperRva = 0x14E910;      // vtable 0x1598628, entry 10

// Callees in call order (0xf590 and the stack check excluded).
inline constexpr std::uint64_t kOwnerRow3AllocRva = 0x17070;
inline constexpr std::uint64_t kOwnerRow3MutexInitRva = 0x3B310;
inline constexpr std::uint64_t kOwnerRow3StringViewRva = 0xF2F0;
inline constexpr std::uint64_t kOwnerRow3CommandCtorRva = 0x20AF0;
inline constexpr std::uint64_t kOwnerRow3HelperCtorRva = 0x1AB270;
inline constexpr std::uint64_t kOwnerRow3Alloc17090Rva = 0x17090;

// Allocation sizes.
inline constexpr std::size_t kOwnerRow3AnchorSize = 0x60;
inline constexpr std::size_t kOwnerRow3HelperSize = 0x28;
inline constexpr std::size_t kOwnerRow3ControlSize = 0x20;

// String literals (.rodata 0xfaa240 / 0xfaa24e) and the description length the body stores.
inline constexpr const char* kOwnerRow3CommandName = "resource_dump";
inline constexpr const char* kOwnerRow3CommandDescription =
    "Dump all resources with the specified FourCC. Params: <fourcc>";
inline constexpr std::uint64_t kOwnerRow3CommandNameRva = 0xFAA240;
inline constexpr std::uint64_t kOwnerRow3CommandDescriptionRva = 0xFAA24E;
inline constexpr std::size_t kOwnerRow3CommandNameLength = 13;
inline constexpr std::size_t kOwnerRow3CommandDescriptionLength = 0x3E;
inline constexpr std::uint32_t kOwnerRow3CommandFlags = 2;

// Object offsets.
inline constexpr std::size_t kOwnerRow3VptrOffset = 0x00;
inline constexpr std::size_t kOwnerRow3EngineOffset = 0x08;
inline constexpr std::size_t kOwnerRow3MapHeaderOffset = 0x18;  // header; leftmost/rightmost
inline constexpr std::size_t kOwnerRow3MapLeftOffset = 0x20;
inline constexpr std::size_t kOwnerRow3MapRootOffset = 0x28;
inline constexpr std::size_t kOwnerRow3MapCountOffset = 0x38;   // inside the +0x30 zero block
inline constexpr std::size_t kOwnerRow3Zero30Offset = 0x30;     // 16 B
inline constexpr std::size_t kOwnerRow3HelperPtrOffset = 0x48;  // 16 B zero, then raw helper
inline constexpr std::size_t kOwnerRow3HelperCtrlOffset = 0x50;
inline constexpr std::size_t kOwnerRow3AnchorOffset = 0x60;     // 16 B zero, then anchor
inline constexpr std::size_t kOwnerRow3MutexOffset = 0x70;      // 8 B, 0x3b310
inline constexpr std::size_t kOwnerRow3Qword78Offset = 0x78;
inline constexpr std::size_t kOwnerRow3CommandOffset = 0x80;    // 0x60 B base + derived tail
inline constexpr std::size_t kOwnerRow3BackPointerOffset = 0xE0;
inline constexpr std::size_t kOwnerRow3HandlerOffset = 0xE8;    // {0x14dc90, 0}
inline constexpr std::size_t kOwnerRow3Zero0F8Offset = 0xF8;    // 32 B
inline constexpr std::size_t kOwnerRow3Byte120Offset = 0x120;
inline constexpr std::size_t kOwnerRow3Zero128Offset = 0x128;   // 16 B, ends at 0x138
inline constexpr std::uint64_t kOwnerRow3HandlerSlotRva = 0x15FEB58;
inline constexpr std::uint64_t kOwnerRow3HandlerRva = 0x14DC90;

// Console-command base object (0x20af0), relative to the command at `this + 0x80`.
inline constexpr std::size_t kOwnerCommandVptrOffset = 0x00;
inline constexpr std::size_t kOwnerCommandRegisteredOffset = 0x18;  // byte
inline constexpr std::size_t kOwnerCommandFlagsOffset = 0x1C;       // dword
inline constexpr std::size_t kOwnerCommandNameOffset = 0x20;        // 0x20 B copy
inline constexpr std::size_t kOwnerCommandDescriptionOffset = 0x40; // 0x20 B copy
inline constexpr std::size_t kOwnerCommandBaseSize = 0x60;

struct OwnerRow3Hooks {
    // 0x17070(size): called with 0x60 (anchor) then 0x28 (helper). Null or a null result fails at
    // "alloc_anchor" / "alloc_helper".
    void* (*allocate)(std::size_t size) = nullptr;
    // 0x17090(size): called once, with 0x20 (the helper's control block, inside 0x14e910). Null or
    // a null result fails at "alloc_control".
    void* (*allocate_17090)(std::size_t size) = nullptr;
    // 0x3b310(this + 0x70): mutex init. Null or false fails at "mutex_init".
    bool (*mutex_init)(void* at) = nullptr;
    // 0x20d80(command, *(0x2dfcbc8)): the console-registry insert reached by 0x20d60 after it set
    // the registered byte. The host supplies the console. Null or false fails at
    // "console_register" (the registered byte stays 1, like the original's prefix).
    bool (*console_register)(void* command) = nullptr;
    // Host stand-ins for the guest vptrs (never callable). All five must be non-null; checked
    // before any write ("vptr").
    const void* pool_vptr = nullptr;          // 0x1598628
    const void* command_base_vptr = nullptr;  // 0x1594d90 (overwritten by command_vptr)
    const void* command_vptr = nullptr;       // 0x1594dd0
    const void* control_vptr = nullptr;       // 0x15986f0
    const void* helper_vptr = nullptr;        // 0x1598ab0
};

struct OwnerRow3Result {
    bool ok;
    const char* failed_at;  // "null-object", "object-align", "vptr", "alloc_anchor", "mutex_init",
                            // "console_register", "alloc_helper", "alloc_control"
};

// Callees lifted for this constructor, exposed so the fixture can pin them in isolation.
//
// 0xf2f0(out, _, cstr): `{ptr = cstr, size = strlen(cstr), aux = 0, ctl = 0}`.
void owner_row3_string_view_f2f0(SharedStr& out, const char* cstr) noexcept;

// 0x20af0(at, name, description, flags): the 0x60 B console-command base constructor. Writes the
// vptr, byte +0x18 = 0, dword +0x1c, copies both holders (0x20 B each) and `lock inc`s each non-null
// control. Bytes +0x08..+0x17 and +0x19..+0x1b stay untouched. A non-null control must be an
// 8-byte-aligned counter.
void owner_row3_command_ctor_20af0(void* at, const void* base_vptr, const SharedStr& name,
                                   const SharedStr& description, std::uint32_t flags) noexcept;

// 0x1ab270(at): the 0x28 B helper constructor. `[0] = vptr`, `[0x10..0x1f] = 0`, `[0x20] = 0`;
// `[8..0xf]` is a hole.
void owner_row3_helper_ctor_1ab270(void* at, const void* vptr) noexcept;

// Builds the 0x138 B pool at `object` with `engine` stored at `+0x08` (the row's first argument
// `[0x2ee2d00]`, stored verbatim - the original does not test it). `object` must be writable for
// 0x138 B and 8-byte aligned. `ok == false` stops at the first failure and keeps the
// already-written prefix; nothing is freed or rolled back (the original has no failure path).
[[nodiscard]] OwnerRow3Result owner_ctor_14db00(void* object, void* engine,
                                                const OwnerRow3Hooks& hooks) noexcept;

// Adapter for `owner_members`' context-free ctor hook. `arg` is `state.globals[0x2EE2D00]`.
// Same contract as the row-14/18 adapters: one binding at a time, any row other than 3 refused.
struct OwnerRow3Binding {
    OwnerRow3Hooks hooks{};
    OwnerRow3Result last{false, nullptr};
    std::size_t calls = 0;
    void* last_engine = nullptr;
};

void owner_row3_bind(OwnerRow3Binding* binding) noexcept;  // nullptr unbinds
[[nodiscard]] bool owner_row3_ctor_hook(std::size_t row, void* object, void* arg) noexcept;

}  // namespace sotc
