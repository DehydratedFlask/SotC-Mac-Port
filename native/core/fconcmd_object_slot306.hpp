// CPU-01-SLOT306-CONCMD-OBJECT-01: native model of the console-COMMAND object that startup slot 306
// (0xa633b0, the `sndx_` block) constructs three times (`sndx_reloadData`, `sndx_categoryMuteAll`,
// `sndx_categoryFilterAll`), plus its registration, its invoker thunk, and the whole destructor
// chain (atexit 0x4b0 -> base destructor 0x20b50 -> manager erase 0x213b0).
//
// Original ELF sha256 d0723afb...5861d3. Every body named here was read end to end from the pinned
// bytes with `xcrun llvm-objdump -d` and is byte-pinned by tools/lift/check_fconcmd_object_slot306.py.
// NOTHING here executes, loads or calls an original RVA. Vtable pointers, vtable slots and the three
// handler addresses are IDENTITY TAGS (original addresses kept as numbers); no native code dispatches
// through them, and construction never reaches them. Since CPU-01-SLOT306-HANDLERS-NATIVE-01 the
// three handlers' observed BEHAVIOUR is modelled natively in core/sndx_command_handlers (see the
// handler-tag wiring below the site table); the tags remain the only link, and the original bodies
// are still never executed.
//
// ---- Object layout (0xa0 bytes stride; 0x98 bytes written) ---------------------------------------
//   +0x00..+0x5f  ConCmdBase (0x20af0, concmd_base.hpp), flags = 2 for commands (1 for variables)
//   +0x00         vptr = 0x1594dd0  (vtable 0x1594dc0, address point +0x10; overwrites 0x1594d90)
//   +0x60..+0x7f  bound delegate (ymm store of the stack staging block, then one fix-up):
//                   +0x60 object   = &this->delegate (self address; the original stores the address of
//                                    its own +0x60, only when the handler word is non-zero)
//                   +0x68 fn       = 0x1990  (the value stored at 0x15fe5e0, a RELATIVE relocation:
//                                    the invoker thunk, an even code address => a plain, non-virtual
//                                    member-function-pointer `ptr`)
//                   +0x70 adj      = 0       (member-function-pointer `adj`)
//                   +0x78 handler  = the handler RVA (a raw function pointer in the original)
//   +0x80 / +0x88 / +0x90  begin / end / capacity of a vector of 0x50-byte records, all zero at
//                   construction: {+0x00 qword, +0x08 SharedStr, +0x28 SharedStr, +0x48 qword}. The
//                   destructor releases +0x28 then +0x08 of each record and frees the storage. No code
//                   in the three slot-306 sequences ever fills it; its meaning (argument descriptors?)
//                   is inference and the fill path is not modelled (fail-closed contract below).
//   +0x98..+0x9f  never written (stride padding).
//
// ---- Virtual table 0x1594dc0 (address point 0x1594dd0; the two header words are zero) --------------
//   slot 0 0x4b0   complete destructor (atexit target of all three objects)
//   slot 1 0x22a60 deleting destructor (same body as 0x4b0 + base dtor + `jmp 0x170b0`)
//   slot 2 0x20d60 register-once guard (the INDIRECT call `[0x1594dc0 + 0x20]` of the slot body; it is
//                  the same function as the base class slot 2 at 0x1594da0, so registration is the
//                  already-native register_concmd_20d60)
//   slot 3 0x22ae0 `xor eax,eax; ret`
//   slot 4 0x21da0 execute: the head of its body is a bound call on the delegate (modelled below as
//                  invoke_delegate_head_21dc6); the diagnostics that follow are NOT modelled
//   slot 5 0x21f80 not read (not reachable from slot 306)
//
// ---- Invoker thunk 0x1990 (12 B) ------------------------------------------------------------------
//   mov rax,[rdi+0x18]; mov rdi,rsi; mov rsi,rdx; jmp rax
//   i.e. thunk(delegate_object, cmd, args) => handler(cmd, args), where `delegate_object + 0x18` is the
//   handler word at cmd+0x78. Reachable only through invoke_delegate_head_21dc6.
//
// ---- Construction sequence per command (identical for the three sites, byte-pinned) ----------------
//   1. 0xf2f0(&name, 0, cstr)                       name view {cstr, strlen, 0, 0}
//   2. stack staging: description view {cstr, imm length, 0, 0} (borrowed, no control counter) and
//      the delegate block {r15 (stack address), 0x1990, 0, handler}
//   3. 0x20af0(obj, &name, &description, flags = 2) base constructor
//   4. [obj] = 0x1594dd0; obj+0x60..0x7f = staging block; if handler != 0 then [obj+0x60] = obj+0x60
//   5. zero obj+0x80..0x97
//   6. `call [0x1594dc0 + 0x20]` => 0x20d60(obj)    register-once; manager cell null => pending list
//   7. 0xf590(&description); 0xf590(&name)           inert (no control counters)
//   8. __cxa_atexit(0x4b0, obj, 0x1604218)
//
// Modelled: everything above, the register path selection (pending list / manager contract), the
// destructor chain with its vector walk, manager erase hook, pending-list unlink and view releases.
// Hooks (explicit, fail closed with ok == false when missing and needed): the three handler bodies,
// the manager `erase` (0x213b0 plus the 0x3b390/0x3b3c0 lock), the shared-release destroy/free
// callees (0x12af0 / 0x170b0) and the vector-storage free (0x170c0). Not modelled: 0x21da0 after the
// bound call, vtable slot 5, the vector fill path, and the real tree layout of the manager map.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <mutex>

#include "concmd_base.hpp"
#include "sndx_command_handlers.hpp"  // handler-tag wiring static_asserts only
#include "shared_release_f590.hpp"

namespace sotc {

// ---- identities ------------------------------------------------------------------------------------
inline constexpr std::uint64_t kFConCmdVtableStart = 0x1594dc0;
inline constexpr std::uint64_t kFConCmdVptr = 0x1594dd0;               // address point
inline constexpr std::uint64_t kFConCmdVtableRegisterOffset = 0x20;    // `call [0x1594dc0 + 0x20]`
inline constexpr std::array<std::uint64_t, 6> kFConCmdVtableSlots{0x4b0,   0x22a60, 0x20d60,
                                                                  0x22ae0, 0x21da0, 0x21f80};
static_assert(kFConCmdVtableSlots[(kFConCmdVtableRegisterOffset - 0x10) / 8] == kConCmdRegisterRva,
              "slot 2 of the command vtable is the already-native register-once guard");

inline constexpr std::uint64_t kFConCmdDtorRva = 0x4b0;
inline constexpr std::size_t kFConCmdDtorSize = 0x71;
inline constexpr std::uint64_t kFConCmdDeletingDtorRva = 0x22a60;
inline constexpr std::size_t kFConCmdDeletingDtorSize = 0x79;
inline constexpr std::uint64_t kFConCmdInvokeThunkRva = 0x1990;
inline constexpr std::size_t kFConCmdInvokeThunkSize = 0xc;
inline constexpr std::uint64_t kFConCmdThunkCellRva = 0x15fe5e0;       // qword holding 0x1990 (RELATIVE reloc)
inline constexpr std::uint64_t kFConCmdExecuteRva = 0x21da0;           // vtable slot 4
inline constexpr std::uint64_t kFConCmdExecuteBoundCallRva = 0x21dc6;  // head modelled: 0x21dc6..0x21dfa
inline constexpr std::uint64_t kConCmdManagerEraseRva = 0x213b0;       // lock 0x3b390 / unlock 0x3b3c0 around it
inline constexpr std::size_t kConCmdManagerEraseSize = 0xcd;           // 0x213b0..0x2147d (incl. cold tail)
inline constexpr std::uint64_t kConCmdManagerLockOffset = 0x58;        // manager + 0x58: the mutex
inline constexpr std::uint64_t kConCmdManagerMapSizeOffset = 0x28;     // manager + 0x28: entry count
inline constexpr std::uint64_t kFConCmdFreeStorageRva = 0x170c0;       // tagged deallocate (tag 2)
inline constexpr std::uint32_t kFConCmdFlags = 2;
inline constexpr std::size_t kFConCmdObjectSize = 0xa0;
inline constexpr std::size_t kFConCmdArgRecordSize = 0x50;

// ---- layout ------------------------------------------------------------------------------------------
// +0x60 bound delegate: the shape read by 0x21da0 (object, ptr, adj) with the handler word in the object.
struct FConCmdDelegate {
    std::uint64_t object;    // +0x60 self address (or 0 when no handler)
    std::uint64_t fn;        // +0x68 invoker thunk RVA tag (0x1990)
    std::uint64_t adj;       // +0x70 this-adjustment, 0
    std::uint64_t handler;   // +0x78 handler identity tag (original RVA, never called natively)
};
static_assert(sizeof(FConCmdDelegate) == 0x20);

// One 0x50-byte vector record (destructor view: +0x28 and +0x08 are released, in that order).
struct FConCmdArgRecord {
    std::uint64_t head;      // +0x00 never touched by the destructor
    SharedStr first;         // +0x08
    SharedStr second;        // +0x28
    std::uint64_t tail;      // +0x48 never touched by the destructor
};
static_assert(sizeof(FConCmdArgRecord) == kFConCmdArgRecordSize && offsetof(FConCmdArgRecord, first) == 0x08 &&
              offsetof(FConCmdArgRecord, second) == 0x28);

struct ConCmdCommand {
    ConCmdBase base;                 // +0x00
    FConCmdDelegate delegate;        // +0x60
    FConCmdArgRecord* args_begin;    // +0x80
    FConCmdArgRecord* args_end;      // +0x88
    FConCmdArgRecord* args_cap;      // +0x90
    std::uint8_t gap98[8];           // +0x98 never written
};
static_assert(sizeof(ConCmdCommand) == kFConCmdObjectSize && offsetof(ConCmdCommand, delegate) == 0x60 &&
              offsetof(ConCmdCommand, args_begin) == 0x80 && offsetof(ConCmdCommand, args_end) == 0x88 &&
              offsetof(ConCmdCommand, args_cap) == 0x90 && offsetof(ConCmdCommand, gap98) == 0x98);

// ---- the three slot-306 sites ------------------------------------------------------------------------
struct Slot306CommandSite {
    const char* name;
    std::size_t name_length;               // strlen, produced by 0xf2f0
    const char* description;
    std::size_t description_length;        // immediate stored into the stack view
    std::uint64_t name_rva;                // rodata
    std::uint64_t description_rva;
    std::uint64_t object_rva;              // .bss global
    std::uint64_t handler_rva;             // identity tag only
    std::uint64_t name_call_rva;           // call 0xf2f0
    std::uint64_t ctor_call_rva;           // call 0x20af0
    std::uint64_t register_call_rva;       // call r14 (= [0x1594dc0 + 0x20])
    std::uint64_t atexit_call_rva;         // call 0xf8f390
};

inline constexpr std::array<Slot306CommandSite, 3> kSlot306CommandSites{{
    {"sndx_reloadData", 15,
     "Reloads all currently loaded banks and the sound table. Params: None", 68,
     0x12c4c31, 0x12c4c41, 0x32f9f68, 0xa5bf20, 0xa64660, 0xa646bc, 0xa64725, 0xa6474d},
    {"sndx_categoryMuteAll", 20,
     "Mutes/Unmutes all categories. Params: <bool mute>", 49,
     0x12c4c86, 0x12c4c9b, 0x32fa008, 0xa5bf50, 0xa64763, 0xa647b7, 0xa64811, 0xa64839},
    {"sndx_categoryFilterAll", 22,
     "Filter/Unfilter all categories. Params: <bool filter>", 53,
     0x12c4ccd, 0x12c4ce4, 0x32fa0a8, 0xa5c020, 0xa6484f, 0xa648a3, 0xa648fd, 0xa64928},
}};

// ---- handler-tag wiring (CPU-01-SLOT306-HANDLERS-NATIVE-01) ------------------------------------------
// The three handler RVAs above stay IDENTITY TAGS — construction never calls them. Their observed
// behaviour is now modelled natively in core/sndx_command_handlers: a host executing a command
// resolves the object's delegate.handler through sndx_command_id_for_tag() and runs the model via
// run_sndx_handler(). These static_asserts bind the site table to that module's tag constants, so
// the two tables cannot drift apart.
static_assert(kSlot306CommandSites[0].handler_rva == kSndxReloadDataHandlerRva,
              "sndx_reloadData site must carry the native module's reload tag");
static_assert(kSlot306CommandSites[1].handler_rva == kSndxCategoryMuteAllHandlerRva,
              "sndx_categoryMuteAll site must carry the native module's mute tag");
static_assert(kSlot306CommandSites[2].handler_rva == kSndxCategoryFilterAllHandlerRva,
              "sndx_categoryFilterAll site must carry the native module's filter tag");

// ---- construction -------------------------------------------------------------------------------------
enum class FConCmdEvent : std::uint8_t {
    name_view_f2f0,
    base_ctor_20af0,
    vptr_and_delegate_store,
    vector_zero,
    register_via_vtable_slot2,
    release_description_f590,
    release_name_f590,
    atexit_4b0,
};

// Optional ordered record of the steps; fixed storage so construction stays allocation-free.
struct FConCmdTrace {
    std::array<FConCmdEvent, 16> events{};
    std::size_t count = 0;
    void push(FConCmdEvent e) noexcept {
        if (count < events.size()) events[count] = e;
        ++count;
    }
};

struct FConCmdCommandSpec {
    const char* name;                    // NUL-terminated; 0xf2f0 measures it
    const char* description;             // borrowed view
    std::size_t description_length;
    std::uint64_t handler_tag;           // non-zero identity tag of the handler (never called here)
};

struct FConCmdBuildResult {
    bool ok = false;
    bool vtable_register_slot_ok = false;   // [0x1594dc0 + 0x20] == 0x20d60
    ConCmdRegisterResult registration{};
    bool views_inert = false;               // both 0xf590 calls were exact no-ops
    bool atexit_recorded = false;           // dtor 0x4b0 recorded, not run
    bool handler_invoked = false;           // always false: construction never reaches a handler
};

// Models steps 1..8 above for an arbitrary command spec. Fails closed BEFORE writing anything when
// the spec is unusable (null strings or a zero handler: the original would leave a stack address in
// the delegate). Otherwise `ok` is the conjunction of registration, inert releases and atexit record.
[[nodiscard]] FConCmdBuildResult init_fconcmd_object(ConCmdCommand& obj, const FConCmdCommandSpec& spec,
                                                     ConCmdRegistry& registry,
                                                     FConCmdTrace* trace = nullptr) noexcept;

// The three observed sites. index >= 3 => ok == false, nothing written.
[[nodiscard]] FConCmdBuildResult init_slot306_command(std::size_t index, ConCmdCommand& obj,
                                                      ConCmdRegistry& registry,
                                                      FConCmdTrace* trace = nullptr) noexcept;

// ---- invoker thunk 0x1990 and the bound call at the head of vtable slot 4 (0x21da0) --------------------
struct FConCmdInvokeHooks {
    // Native stand-in for a handler body. Receives the handler identity tag, the command and the opaque
    // argument pointer; returns the handler's int status. Null => the call fails closed (nothing called).
    int (*handler)(std::uint64_t handler_tag, ConCmdCommand& cmd, void* args) noexcept = nullptr;
};

struct FConCmdInvokeResult {
    bool ok = false;
    bool no_delegate = false;   // object == 0 && fn == 0: the original skips the call and yields status 1
    int status = 0;             // eax of the bound call (post-call diagnostics at 0x21dfd.. NOT modelled)
};

// 0x1990 as native code: handler = delegate_object->handler word, then handler(cmd, args). The
// thunk only reads `delegate_object + 0x18`, so `delegate_object` must be this command's own
// delegate (checked); anything else fails closed.
[[nodiscard]] FConCmdInvokeResult invoke_thunk_1990(ConCmdCommand& cmd, const FConCmdDelegate* delegate_object,
                                                    void* args, const FConCmdInvokeHooks& hooks) noexcept;

// 0x21dc6..0x21df8: `rdi = [this+0x60]; rax = [this+0x68]; (both zero => status 1); rdi += [this+0x70];
// if (rax & 1) virtual lookup; call rax(rdi, this, args)`. Only fn == 0x1990 is modelled; a virtual
// (odd) fn or any other fn fails closed.
[[nodiscard]] FConCmdInvokeResult invoke_delegate_head_21dc6(ConCmdCommand& cmd, void* args,
                                                             const FConCmdInvokeHooks& hooks) noexcept;

// ---- destructor chain ------------------------------------------------------------------------------------
struct FConCmdDtorHooks {
    // Non-null manager path of 0x20b50: lock(manager + 0x58), 0x213b0(manager, &name), unlock. The
    // return value of 0x213b0 (1 = erased, 0 = absent) is passed through; the caller ignores it.
    bool (*manager_erase)(void* manager, const SharedStr& name) noexcept = nullptr;
    // 0xf590 last-reference callees (0x12af0 / 0x170b0).
    SharedReleaseF590Hooks shared{};
    // 0x170c0(storage): frees the vector storage at cmd+0x80.
    void (*free_storage)(void* storage) noexcept = nullptr;
};

struct ConCmdBaseDtorResult {
    bool ok = false;
    bool manager_path = false;        // manager cell non-null: erase hook consulted
    bool manager_erased = false;      // the hook returned true
    bool list_initialised = false;    // the guarded `head = {&head, &head}` ran in this call
    bool unlinked = false;            // the node was found in the pending list and removed
    bool description_released = false;
    bool name_released = false;
    std::uint64_t final_vptr = 0;
};

// 0x20b50 (vptr = 0x1594d90; optional manager erase; pending-list unlink; release description (+0x40)
// then name (+0x20)). The caller of the original passes the object in rdi.
[[nodiscard]] ConCmdBaseDtorResult destroy_concmd_base_20b50(ConCmdBase& cmd, ConCmdRegistry& registry,
                                                             const FConCmdDtorHooks& hooks) noexcept;

struct FConCmdDtorResult {
    bool ok = false;
    std::size_t records_released = 0;   // vector records whose two views were released
    bool storage_freed = false;         // 0x170c0 called (begin != 0)
    ConCmdBaseDtorResult base{};
};

// 0x4b0 (the vtable-0 destructor registered with atexit): vptr = 0x1594dd0, release +0x28 then +0x08
// of every record in [begin, end), free the storage when begin != 0, tail-jump into 0x20b50. The
// vector pointers are left unchanged afterwards (as in the original). A malformed vector (end before
// begin, or a length that is not a multiple of 0x50) fails closed before anything is released.
[[nodiscard]] FConCmdDtorResult destroy_fconcmd_4b0(ConCmdCommand& cmd, ConCmdRegistry& registry,
                                                    const FConCmdDtorHooks& hooks) noexcept;

// ---- behavioural model of the non-null manager (0x20d80 manager path, 0x213b0) ------------------------
// The original is a map from a SharedStr key to ConCmd* guarded by the mutex at manager+0x58:
//   insert (0x20d80): lower-bound by the `strcasecmp`-labeled import (NID-name inference) on
//     node.key.data and name.data; if the key is absent the key
//     (a SharedStr copy, control counter acquired) and `cmd` are inserted; if present NOTHING is
//     overwritten (first registration wins).
//   erase  (0x213b0): lower-bound by the same imported comparator; if found, size -= 1, unlink the node, release the key
//     (0xf590), free the node, return 1; otherwise return 0. The lookup is by KEY ONLY, so destroying
//     a command whose name was already taken erases the OTHER command's entry.
// This class reproduces that behaviour only. The red-black node layout, 0x211d0 and 0x42f50 are not
// reproduced, and it is not a claim about the engine's real manager (nothing installs one natively).
class FConCmdManagerModel {
public:
    explicit FConCmdManagerModel(SharedReleaseF590Hooks key_release_hooks = {}) noexcept
        : key_release_hooks_(key_release_hooks) {}

    // Signature-compatible with ConCmdManagerContract::insert / FConCmdDtorHooks::manager_erase.
    [[nodiscard]] static bool insert_hook(void* manager, ConCmdBase& cmd) noexcept;
    [[nodiscard]] static bool erase_hook(void* manager, const SharedStr& name) noexcept;

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] ConCmdBase* find(const char* name) const noexcept;
    [[nodiscard]] bool failed() const noexcept { return failed_; }   // sticky: an allocation/release failed

private:
    struct CaseInsensitiveLess {
        bool operator()(const char* a, const char* b) const noexcept;
    };
    struct Entry {
        SharedStr key;        // copy of cmd.name, control counter acquired
        ConCmdBase* cmd;
    };
    SharedReleaseF590Hooks key_release_hooks_;
    mutable std::mutex lock_;
    std::map<const char*, Entry, CaseInsensitiveLess> map_;
    bool failed_ = false;
};

}  // namespace sotc
