// Native 0xa5cd90 body: SoundSystemSndx::Init(const SndxInitParams&).
//
// Original: 0xa5cd90..0xa5d207, 1,143 B (0x477), sha256
// 5d554780bc47358511278e0adad41d4d9bf2aca46902afc5976167dc41becf23.
//
// The body is one linear initializer with two conditional regions:
//   * a resource lookup driven by the caller's 16-byte descriptor
//     ({const char* name; uint16_t mode; uint16_t pad}) and the static registry at
//     [0x2ee2cf8], including the once/mutex registration pair and the field replacement at
//     this+0x10;
//   * a two-arena allocation plus a 0x70-byte task object registration.
// Measured from the pinned ELF on 2026-11-12: 44 direct call instructions and 2 indirect call
// sites (`0xa5ceb5` = the [[0x2ee2cf8]+0x18] resolver, `0xa5d1d7` = the old field's virtual
// release), plus the cold stack-check tail. An earlier note said 45 calls / 43 direct; the
// checker now pins 44 (tools/lift/check_a5cd90_body.py). Every callee is an unmodelled engine or
// libc contract and is exposed as a named host seam below; the native body owns the branch
// structure, the argument setup and every store listed in the header of the cpp.
//
// Observed stores:
//   this+0x69c8 <- u32 from 0x992210
//   this+0x08   <- u32 from 0x9892d0
//   this+0x10   <- resource pointer, this+0x18/0x20 flags
//   this+0x00   <- 1
//   this+0xa4f8 <- the 0x70-byte task object
// and two local descriptors: five pinned callback/model pointers passed to 0x991c70, and a
// {this, 0xa5d610, 0x23468c8, 0} block passed to 0x3b5c0 with stack argument 0x100000.

#pragma once

#include <cstddef>
#include <cstdint>

#include "init_post_call_trace.hpp"

namespace sotc {

inline constexpr std::uint64_t kInitA5cd90BodyRva = 0xa5cd90;
inline constexpr std::size_t kInitA5cd90BodySize = 0x477;  // 1,143 B
inline constexpr std::uint64_t kInitA5cd90BodyEndRva = 0xa5d207;
inline constexpr std::size_t kInitA5cd90ObjectBytes = 0xa500;
inline constexpr std::size_t kInitA5cd90KeyBytes = 0x20;
inline constexpr std::size_t kInitA5cd90Arena0Bytes = 0x200000;
inline constexpr std::size_t kInitA5cd90Object70Bytes = 0x70;
inline constexpr std::size_t kInitA5cd90TaskDescriptorBytes = 0x20;
inline constexpr std::size_t kInitA5cd90LogDescriptorBytes = 0x18;

// Pinned callback/model block passed to 0x991c70: three absolute callbacks plus two qwords
// loaded from 0x15feb80/0x15fed98.
inline constexpr std::uint64_t kInitA5cd90Callback0 = 0xa5d2e0;
inline constexpr std::uint64_t kInitA5cd90Callback1 = 0xa5d3b0;
inline constexpr std::uint64_t kInitA5cd90Callback2 = 0xa5d580;
inline constexpr std::uint64_t kInitA5cd90Model0Rva = 0x15feb80;
inline constexpr std::uint64_t kInitA5cd90Model0 = 0xa5d5a0;
inline constexpr std::uint64_t kInitA5cd90Model1Rva = 0x15fed98;
inline constexpr std::uint64_t kInitA5cd90Model1 = 0xa5d5f0;

// Pinned strings and task constants.
inline constexpr std::uint64_t kInitA5cd90DescriptorNameRva = 0x101e7ac;  // "$/sound/shadow/sotc_buss.sxd"
// The two config strings selected by the [0x32f9730]+0x60 byte. Names state the flag value
// that selects each one (the earlier Config0/Config1 names were inverted).
inline constexpr std::uint64_t kInitA5cd90ConfigFlagZeroRva = 0x12c4f51;
inline constexpr std::uint64_t kInitA5cd90ConfigFlagSetRva = 0x12c4efa;
inline constexpr std::uint64_t kInitA5cd90FunctionNameRva = 0x12c4ec7;
inline constexpr std::uint64_t kInitA5cd90SourcePathRva = 0x12c4e79;
inline constexpr std::uint64_t kInitA5cd90ErrorMessageRva = 0x12c4fa8;
inline constexpr std::uint64_t kInitA5cd90TaskNameRva = 0x12c4fd6;  // "SoundSystemSndx"
inline constexpr std::uint64_t kInitA5cd90TaskSourceRva = 0x12c4fd6;
inline constexpr std::uint64_t kInitA5cd90TaskConstant0 = 0xa5d610;
inline constexpr std::uint64_t kInitA5cd90TaskConstant1 = 0x23468c8;
inline constexpr std::uint64_t kInitA5cd90ErrorLine = 0x25e;
inline constexpr std::uint32_t kInitA5cd90TaskLine = 0x2bc;
inline constexpr std::uint32_t kInitA5cd90TaskArg4 = 0x60;
inline constexpr std::uint64_t kInitA5cd90TaskStackArg = 0x100000;

// Static addresses the body reads or publishes.
inline constexpr std::uint64_t kInitA5cd90RegistryRva = 0x2ee2cf8;
inline constexpr std::uint64_t kInitA5cd90ConfigFlagRva = 0x32f9730;
inline constexpr std::uint64_t kInitA5cd90ArenaTagRva = 0x2dd9f80;
inline constexpr std::uint64_t kInitA5cd90OnceKeyRva = 0x2ed3668;
inline constexpr std::uint64_t kInitA5cd90OnceDataRva = 0x2ed3640;
inline constexpr std::uint64_t kInitA5cd90KeyCreateFnRva = 0x95610;
inline constexpr std::uint64_t kInitA5cd90KeyCreateDataRva = 0x1604218;
inline constexpr std::uint32_t kInitA5cd90ResourceNameId = 0xd;

struct InitA5cd90Hooks {
    // Host-owned SoundSystemSndx object (0xa500 B), the caller's 16-byte parameter block, and
    // the registry pointer read from [0x2ee2cf8].
    std::uint8_t* object = nullptr;
    const std::uint8_t* descriptor = nullptr;  // {const char* name; uint16_t mode; uint16_t pad}
    void* registry = nullptr;
    const std::uint8_t* config_flag = nullptr;  // byte at [0x32f9730]+0x60
    const std::uint32_t* arena_tag = nullptr;   // dword at 0x2dd9f80 (BSS)

    // 0x9854f0(callback 0xa5d210), 0x994dd0(0xa5c630, 0xa5c440), 0x994e70(config string).
    // The 0x994e70 argument is the config string's original-image RVA carried as an IDENTITY
    // TAG; the body never dereferences original memory, so the hook must not read it either.
    InitPostCallOutcome (*call_9854f0)(std::uintptr_t callback) = nullptr;
    InitPostCallOutcome (*call_994dd0)(std::uintptr_t first, std::uintptr_t second) = nullptr;
    InitPostCallOutcome (*call_994e70)(std::uintptr_t config_rva) = nullptr;
    InitPostCallOutcome (*call_991c70)(const void* block) = nullptr;

    // 0x12820(size, tag, align, flags) -> pointer; 0x992210(buffer, size, align, flags) -> int.
    InitPostCallOutcome (*alloc_12820)(std::size_t size, std::uint32_t tag, std::uint32_t align,
                                       std::uint32_t flags, void** out) = nullptr;
    InitPostCallOutcome (*call_992210)(void* buffer, std::size_t size, std::uint32_t align,
                                       std::uint32_t flags, std::int32_t* result) = nullptr;

    // Resource key and lookup: 0x14c2f0(&key), then the [[0x2ee2cf8]+0x18] virtual resolver.
    InitPostCallOutcome (*call_14c2f0)(void* key_out) = nullptr;
    InitPostCallOutcome (*resolve_resource)(void* out, void* registry, const void* key) = nullptr;

    // Reference/destruction and notification contracts.
    InitPostCallOutcome (*call_14ccb0)(void* pointer) = nullptr;  // addref
    InitPostCallOutcome (*call_14ccc0)(void* pointer) = nullptr;  // release
    InitPostCallOutcome (*call_14d2d0)(void* pointer) = nullptr;  // destroy
    InitPostCallOutcome (*call_95580)(void* pointer) = nullptr;   // notify
    InitPostCallOutcome (*release_f590)(void* pointer) = nullptr;  // key release

    // Once/mutex registration: the byte at 0x2ed3668 plus 0xf8f3d0 is folded into one
    // stateful hook; the other four calls and the debug-name call stay individual seams.
    InitPostCallOutcome (*once_2ed3668)(bool* initialized) = nullptr;
    InitPostCallOutcome (*call_f730)(std::uint64_t* thread) = nullptr;
    InitPostCallOutcome (*call_14c4a0)(std::uintptr_t lock, std::uint64_t value,
                                       std::uint64_t owner) = nullptr;
    InitPostCallOutcome (*call_f8f390)(std::uintptr_t function, std::uintptr_t lock,
                                       std::uintptr_t data) = nullptr;
    InitPostCallOutcome (*call_f8f3e0)(std::uintptr_t key) = nullptr;
    InitPostCallOutcome (*call_14cf20)(void* resource, std::uint32_t name_id,
                                       std::uintptr_t lock) = nullptr;

    // Field replacement query and object query.
    InitPostCallOutcome (*call_14d880)(std::uint64_t* current) = nullptr;
    InitPostCallOutcome (*call_14d7b0)(void* object, std::uint64_t* param_out) = nullptr;

    // 0x9892d0(param,0,0,0) -> status; 0x946590(status,&size); 0x946650(status,region,size).
    InitPostCallOutcome (*call_9892d0)(std::uintptr_t param, std::int32_t* status) = nullptr;
    InitPostCallOutcome (*call_946590)(std::int32_t status, std::uint64_t* size) = nullptr;
    InitPostCallOutcome (*call_946650)(std::int32_t status, void* region,
                                       std::uint64_t size) = nullptr;

    // Descriptor word callbacks and the UserID check/log path.
    InitPostCallOutcome (*call_994070)(std::uint8_t low, std::uint8_t high) = nullptr;
    InitPostCallOutcome (*call_f901e0)(std::uint64_t* value, std::int32_t* result) = nullptr;
    InitPostCallOutcome (*call_994640)(std::int32_t code, std::uint64_t value) = nullptr;
    InitPostCallOutcome (*log_condition)(bool* should_log) = nullptr;
    InitPostCallOutcome (*call_3e7e0)(std::int32_t a, std::int32_t b, const void* block,
                                      std::int32_t c, std::int32_t d,
                                      std::uintptr_t message) = nullptr;

    // 0x17070(0x70), 0x3b5c0(task, descriptor, name, line, 0x60, 1, 0x100000), virtual release.
    InitPostCallOutcome (*alloc_17070)(std::size_t size, void** out) = nullptr;
    InitPostCallOutcome (*call_3b5c0)(void* task, const void* descriptor,
                                      std::uintptr_t name, std::uint32_t line,
                                      std::uint32_t arg4, std::uint32_t arg5,
                                      std::uint64_t stack_arg) = nullptr;
    InitPostCallOutcome (*virtual_release)(void* pointer) = nullptr;

    // Canary pair and the pinned stack-check trap.
    std::uint64_t (*canary)() = nullptr;
    InitPostCallOutcome (*trap_f8f3a0)() = nullptr;
};

struct InitA5cd90Result {
    bool ok = false;
    const char* blocked = nullptr;
    int calls_total = 0;
    int alloc_calls = 0;
    std::uint32_t field_69c8 = 0;
    std::uint32_t field_08 = 0;
    void* resource = nullptr;
    bool resource_ready = false;
    std::uint8_t flag_18 = 0;
    std::uint8_t flag_20 = 0;
    std::uint64_t object_a4f8 = 0;
    std::uint8_t byte0 = 0;
    int once_calls = 0;
    int user_id_check = 0;
    int log_calls = 0;
    int trap_calls = 0;
    std::size_t arena0_bytes = 0;
    std::size_t arena1_bytes = 0;
    bool stack_check_failed = false;
    InitPostCallLog call_log{};
};

[[nodiscard]] InitA5cd90Result run_init_a5cd90_body(
    const InitA5cd90Hooks& hooks) noexcept;

}  // namespace sotc
