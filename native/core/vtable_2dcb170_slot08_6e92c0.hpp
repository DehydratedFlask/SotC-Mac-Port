// Table slot resolved by the provenance-audited vtable-closure sweep:
// `0x2dcb170 + 0x08 -> 0x6e92c0`.
//
// Evidence:
//   * `0x6a2dc4` is `call *0x8(%r14)` with `%r14` loaded by `leaq 0x2dcb170(%rip), %r14` at
//     `0x6a2da7` and never rewritten (the callee `0xf8f490` must preserve `%r14`);
//   * `0x6a4ada` is `call *0x8(%rax)` with `%rax` loaded by `leaq 0x2dcb170(%rip), %rax`
//     two instructions earlier;
//   * the qword at `0x2dcb170 + 0x08` in the pinned image is `0x6e92c0`;
//   * the dynamic relocation table has one type-8 row at `0x2dcb178` with addend `0x6e92c0`;
//   * `0x6e92c0` is an FDE function start with length `0xd8`.
//
// The container is a mixed pointer table, not a C++ vtable: slot `0x00` holds the data pointer
// `0x1272a60`, slot `0x08` holds this handler, and `+0x10` is a packed non-pointer constant.
//
// Body (216 B) applies one preset block to the wave subsystem:
//   canary = *0x15fe518                                   ; stack protector
//   format(0x5f5a10)(out, arg, id, 0x1606140, (0.01, 0, -0.05, 1))
//       id = *(i32*)(*(0x24c01c8) + 0x10) = -256
//   owner = *(*(0x1bb2b78) + 0x18)
//   if (owner != null) {
//       targets = 0x660fc0(owner)
//       *(float4*)(*(void**)(targets + 0x160)) = *(0x1606140) = (0, 0, 0, 1)
//       *(float4*)(*(void**)(targets + 0x150)) = *(0x2f4cfd0) = mutable block, zero at load
//       0x6610d0(owner)
//   }
//   canary check; return
//
// Modelled: the unconditional format call with its exact id and 16-byte block, the singleton walk,
// both 16-byte copies and the commit call as hooks. Not modelled: the bodies of `0x5f5a10`,
// `0x660fc0`, `0x6610d0`, the singleton, and the two destination blocks. A missing hook or a null
// destination is a visible failure; the retail body would dereference it.
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0Rva = 0x6e92c0;
inline constexpr std::size_t kVtable2dcb170Slot08_6e92c0Size = 0xd8;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0EndAddress = 0x6e9398;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0TableAddress = 0x2dcb170;
inline constexpr std::uint32_t kVtable2dcb170Slot08_6e92c0SlotOffset = 0x08;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0RelocAddress = 0x2dcb178;
// Hook targets: guest addresses, deliberately not declared with an `Rva` suffix because no native
// entry exists for them.
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0FormatCallAddress = 0x5f5a10;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0AccessCallAddress = 0x660fc0;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0CommitCallAddress = 0x6610d0;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0IdGlobalAddress = 0x24c01c8;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0IdValueOffset = 0x10;
inline constexpr std::int32_t kVtable2dcb170Slot08_6e92c0IdValue = -256;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0SingletonAddress = 0x1bb2b78;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0SingletonMemberOffset = 0x18;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0TargetsBlockAOffset = 0x160;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0TargetsBlockBOffset = 0x150;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0CopySourceAAddress = 0x1606140;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0CopySourceBAddress = 0x2f4cfd0;
inline constexpr std::uint64_t kVtable2dcb170Slot08_6e92c0VectorAddress = 0x1276ef0;

struct Vtable2dcb170Slot08_6e92c0Cells {
    // `rdi` at entry, forwarded unchanged as the format call's second argument.
    const void* argument = nullptr;
    // `rbp-0x58`: written by the format call, unconditionally.
    void* format_out = nullptr;
    // `*(u64*)(*(0x1bb2b78) + 0x18)`. Null skips the whole tail block, exactly like the body.
    void* owner = nullptr;
    // Optional overrides for the two 16-byte source blocks. Null means "use the pinned image
    // content": `(0, 0, 0, 1)` from `0x1606140`, and the zero-initialised block at `0x2f4cfd0`.
    // Both addresses live in the writable PT_LOAD, so a host that runs the block more than once
    // must supply the current content instead of relying on the defaults.
    const std::uint8_t* copy_source_a = nullptr;
    const std::uint8_t* copy_source_b = nullptr;
};

// The two pointer fields the accessor's returned struct holds. The retail body reads them through
// `*(void**)(struct + 0x160)` and `*(void**)(struct + 0x150)` and writes 16 bytes through each.
struct Vtable2dcb170Slot08_6e92c0Targets {
    void* block_a = nullptr;  // receives (0, 0, 0, 1)
    void* block_b = nullptr;  // receives the mutable block
};

struct Vtable2dcb170Slot08_6e92c0Hooks {
    // Stands in for `call 0x5f5a10(out, argument, id, 0x1606140, vector)`. The body calls it
    // unconditionally and ignores its return value.
    void (*format_call)(void* out, const void* argument, std::int32_t id, const void* block,
                        const float (&vector)[4]) = nullptr;
    // Stands in for `call 0x660fc0(owner)` plus the two `movq 0xNNN(%rax)` field reads.
    const Vtable2dcb170Slot08_6e92c0Targets* (*access_targets)(void* owner) = nullptr;
    // Stands in for `call 0x6610d0(owner)`.
    void (*commit)(void* owner) = nullptr;
};

struct Vtable2dcb170Slot08_6e92c0Result {
    bool ok = false;
    bool formatted = false;       // the format call ran
    std::int32_t id = 0;          // the -256 the body reads from `*(0x24c01c8) + 0x10`
    bool tail_ran = false;        // the singleton's member was non-null
    bool copied_a = false;
    bool copied_b = false;
    bool committed = false;
    float vector[4] = {};         // the 16-byte vector handed to the format call
    float copied_a_values[4] = {};
    float copied_b_values[4] = {};
};

// Applies the preset block; `ok` is false unless every retail dereference has a host target.
[[nodiscard]] Vtable2dcb170Slot08_6e92c0Result vtable_2dcb170_slot08_6e92c0(
    const Vtable2dcb170Slot08_6e92c0Cells& cells = {},
    const Vtable2dcb170Slot08_6e92c0Hooks& hooks = {}) noexcept;

}  // namespace sotc
