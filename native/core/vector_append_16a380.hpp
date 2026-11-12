// CPU-01-STARTUP-SLOT-524-HELPER: native model of the 544 B argument-token appender at RVA 0x16a380.
//
// Original ELF sha256 d0723afb...5861d3. FDE [0x16a380, 0x16a5a0), 544 B (the last byte is the
// trailing nop 0x90); body sha256 3bece5e4a2eca939b86aaaf162b244965cbd2384e6ae6942fc6599862471f3be.
// It has exactly two image callers, both relative `callq`:
//   0x16a27e  inside the identity-only scratch fn 0x16a1f0 (stored by slot 524, never invoked by it)
//   0x16a994  inside startup constructor slot 524 (0x16a810)
//
// Shape (rdi = owner whose +0x80 vector of 0x50 B tokens, rsi = name view, rdx = key dword):
//
//   0x16a393       guard <- *[*[0x15fe518]], stored at rsp+0x28
//   0x16a3ac       staging kind dword (rsp+0x24) = 1
//   0x16a3b4       end = owner[+0x88]; if end < cap -> inline push, else 0x20640(owner+0x80, &kind, name)
//   inline push    token[+0x00] = 1; token[+0x08] = name (16 B + aux + ctl, `lock inc` when ctl);
//                  no store to +0x04..+0x07; 0xf180(token+0x28) builds the empty SharedStr
//   0x16a424       token[+0x48] = *key; +0x4c..+0x4f are NOT written
//   0x16a42b       once-guard byte 0x2ee3e00; 0xf8f3d0 / 0xf8f3e0 around a four-entry table at
//                  0x2ee3da0, stride 0x18: {text ptr @0, prefix ptr @8, key dword @0x10}, the last
//                  entry a sentinel {nullptr, "EVirtualResolutionMode", 0}; +0x14..+0x17 unwritten
//   0x16a4f7       scan: rdx = entry.text; null -> fallback; entry.key == *key -> found; else advance
//   found         0xf2f0(rsp, 0, entry.text) builds a measured view (0xf2f0 measures with strlen)
//   fallback      0x10a90(rsp, "%d", 2): the value argument is the literal 2, not the key, so the
//                  formatted text is always "2". The original's formatter returns an owning string;
//                  this model reports the same observable text as a borrowed view (EXPLICIT OMISSION:
//                  the owning allocation, unreachable from slot 524, whose only key is 0).
//   0x16a555       if token+0x28 != view: 0xf590(token+0x28), copy 32 bytes, zero the local, then
//                  0xf590(local). 0xf180 leaves ctl null, so both releases are exact no-ops.
//   0x16a582       guard compare; mismatch -> 0xf8f3a0 __stack_chk_fail
//
// The token record is the same 0x50 B shape the command vector uses (FConCmdArgRecord): dword kind
// at +0x00, SharedStr at +0x08, SharedStr at +0x28, dword key at +0x48. The second dword key slot is
// what marks this helper: slot 524's first token gets key 0 and enum label "Off".
//
// Nothing here executes, loads, relocates or calls an original RVA. Every RVA above is an identity
// tag; the table/guard are modelled as native constants. 0x20540 is NOT called by this body.
#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "fconcmd_object_slot306.hpp"
#include "hashed_string_14c3a0.hpp"
#include "init_sub_3be930.hpp"
#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kAppend16a380Rva = 0x16a380;
inline constexpr std::size_t kAppend16a380Size = 0x220;   // 544 B, trailing nop included
inline constexpr std::uint64_t kAppend16a380CanaryCellRva = 0x15fe518;
inline constexpr std::uint64_t kAppend16a380CanaryBranchRva = 0x16a58b;   // jne 0x16a59a
inline constexpr std::uint64_t kAppend16a380CanaryTargetRva = 0x16a59a;
inline constexpr std::uint64_t kAppend16a380StackChkFailRva = 0xf8f3a0;
inline constexpr std::uint32_t kAppend16a380FrameBytes = 0x40;

inline constexpr std::uint64_t kAppend16a380EmptyViewRva = 0xf180;
inline constexpr std::size_t kAppend16a380EmptyViewSize = 28;
inline constexpr std::uint64_t kAppend16a380NameViewRva = 0xf2f0;
inline constexpr std::size_t kAppend16a380NameViewSize = 40;
inline constexpr std::uint64_t kAppend16a380PushRva = 0x20640;       // grow branch
inline constexpr std::size_t kAppend16a380PushSize = 342;
inline constexpr std::uint64_t kAppend16a380ReleaseRva = 0xf590;
inline constexpr std::size_t kAppend16a380ReleaseSize = 73;
inline constexpr std::uint64_t kAppend16a380AtexitRva = 0xf8f390;
inline constexpr std::uint64_t kAppend16a380FormatRva = 0x10a90;
inline constexpr std::uint64_t kAppend16a380GuardAcquireRva = 0xf8f3d0;   // 6 B PLT jump
inline constexpr std::uint64_t kAppend16a380GuardReleaseRva = 0xf8f3e0;   // 6 B PLT jump
inline constexpr std::size_t kAppend16a380GuardStubSize = 6;

inline constexpr std::uint64_t kAppend16a380TableRva = 0x2ee3da0;
inline constexpr std::uint64_t kAppend16a380GuardByteRva = 0x2ee3e00;
inline constexpr std::size_t kAppend16a380TableStride = 0x18;
inline constexpr std::size_t kAppend16a380TableCount = 4;

// The four 0x18 B table rows. Only `text` (+0x00) and `key` (+0x10) are read by the helper; the
// prefix words are built by the once block and never consumed here. The sentinel row has a null
// text and terminates the scan before its key is compared.
struct VirtualResolutionLabel {
    const char* text;          // +0x00
    const char* prefix;        // +0x08
    std::uint32_t key;         // +0x10
    std::uint8_t gap14[4];     // +0x14..+0x17 never written
};
static_assert(sizeof(VirtualResolutionLabel) == kAppend16a380TableStride &&
              offsetof(VirtualResolutionLabel, prefix) == 8 &&
              offsetof(VirtualResolutionLabel, key) == 0x10);

inline constexpr char kVirtualResolutionPrefix[] = "EVirtualResolutionMode::";
inline constexpr char kVirtualResolutionTypeName[] = "EVirtualResolutionMode";
inline constexpr char kVirtualResolutionOff[] = "Off";
inline constexpr char kVirtualResolution4x[] = "4x";
inline constexpr char kVirtualResolution9x[] = "9x";
inline constexpr char kVirtualResolutionFallback[] = "2";   // 0x10a90("%d", 2)

inline constexpr std::uint64_t kVirtualResolutionOffRva = 0xfabc18;
inline constexpr std::uint64_t kVirtualResolutionPrefixOffRva = 0xfabc1c;
inline constexpr std::uint64_t kVirtualResolution4xRva = 0xfabc38;
inline constexpr std::uint64_t kVirtualResolutionPrefix4xRva = 0xfabc3b;
inline constexpr std::uint64_t kVirtualResolution9xRva = 0xfabc61;
inline constexpr std::uint64_t kVirtualResolutionPrefix9xRva = 0xfabc64;
inline constexpr std::uint64_t kVirtualResolutionTypeNameRva = 0xfabc8a;
inline constexpr std::uint64_t kVirtualResolutionFallbackFormatRva = 0xfabca1;

inline constexpr std::array<VirtualResolutionLabel, kAppend16a380TableCount>
    kVirtualResolutionLabels{{
        {kVirtualResolutionOff, kVirtualResolutionPrefix, 0, {0, 0, 0, 0}},
        {kVirtualResolution4x, kVirtualResolutionPrefix, 1, {0, 0, 0, 0}},
        {kVirtualResolution9x, kVirtualResolutionPrefix, 2, {0, 0, 0, 0}},
        {nullptr, kVirtualResolutionTypeName, 0, {0, 0, 0, 0}},
    }};

static_assert(sizeof kVirtualResolutionPrefix - 1 == 24);
static_assert(sizeof kVirtualResolutionTypeName - 1 == 22);
static_assert(sizeof kVirtualResolutionFallback - 1 == 1);

// One 0x50 B element of the command's +0x80 vector, exactly the shape this helper writes:
// kind dword at +0x00, name at +0x08, text at +0x28, key dword at +0x48.
struct ConCmdArgToken {
    std::uint32_t kind;         // +0x00 = 1 (helper) / 4 (slot 524's second token)
    std::uint8_t gap04[4];      // +0x04..+0x07 never written
    SharedStr name;             // +0x08
    SharedStr text;             // +0x28
    std::uint32_t key;          // +0x48 (slot 524's second token never writes it)
    std::uint8_t gap4c[4];      // +0x4c..+0x4f never written
};
static_assert(sizeof(ConCmdArgToken) == 0x50 && offsetof(ConCmdArgToken, name) == 0x08 &&
              offsetof(ConCmdArgToken, text) == 0x28 && offsetof(ConCmdArgToken, key) == 0x48);
// The command vector's element type is FConCmdArgRecord (0x50 B: head, first @8, second @0x28,
// tail @0x48). This helper's record has the same extent and the same two SharedStr offsets; the two
// dword fields (kind at +0, key at +0x48) live where FConCmdArgRecord keeps opaque qwords. The
// boundary casts below are therefore exact, not type punning of different layouts.
static_assert(sizeof(ConCmdArgToken) == sizeof(FConCmdArgRecord));
static_assert(offsetof(ConCmdArgToken, name) == offsetof(FConCmdArgRecord, first));
static_assert(offsetof(ConCmdArgToken, text) == offsetof(FConCmdArgRecord, second));
static_assert(offsetof(ConCmdArgToken, key) == offsetof(FConCmdArgRecord, tail));

// Walks the modelled table exactly like 0x16a4f7..0x16a52f: a null text terminates the scan and
// yields nullptr (the fallback path). Returns the matching row otherwise.
[[nodiscard]] const VirtualResolutionLabel* find_virtual_resolution_label(std::uint32_t key) noexcept;

enum class VectorAppendStage : std::uint8_t {
    none,
    validate,
    inline_push,
    grow,
    key_store,
    label_lookup,
    label_replace,
    canary,
};
[[nodiscard]] const char* vector_append_stage_name(VectorAppendStage stage) noexcept;

struct VectorAppendHooks16a380 {
    // 0x15fe518. Null => the guard is not claimed; a changed non-null guard fails at `canary`.
    const std::uint64_t* stack_guard = nullptr;
    // 0x20640(owner+0x80, &kind, name): append one element, reallocating if needed. The original
    // reserves its own storage; the native vector is host-owned, so a host must supply this when the
    // inline branch is full. Null with end == cap fails closed at `grow`.
    bool (*grow)(void* context, ConCmdCommand& owner, const SharedStr& name,
                 std::uint32_t kind) noexcept = nullptr;
    void* grow_context = nullptr;
};

struct VectorAppendResult16a380 {
    bool ok = false;
    VectorAppendStage blocked = VectorAppendStage::none;
    bool inline_push = false;
    bool grow_path = false;          // 0x20640 was taken (never from slot 524)
    bool label_found = false;        // table row matched; false = "%d" fallback
    bool fallback_taken = false;
    bool text_normalized = false;    // 0xf590 + 32-byte copy into token+0x28
    bool canary_checked = false;
    bool canary_matched = false;
    std::size_t token_index = 0;     // index in the vector before the append
    std::uint32_t key = 0;
    const char* label = nullptr;
    std::size_t label_length = 0;
};

// Append one token exactly as 0x16a380 does. The owner must be a command whose +0x80 vector is
// either empty or points at 0x50 B-aligned host storage; malformed vectors fail closed before any
// store. Nothing is written when `validate` fails.
[[nodiscard]] VectorAppendResult16a380 append_arg_token_16a380(
    ConCmdCommand& owner, const SharedStr& name, std::uint32_t key,
    const VectorAppendHooks16a380& hooks = {}) noexcept;

}  // namespace sotc
