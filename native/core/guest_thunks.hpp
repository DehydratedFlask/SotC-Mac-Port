// Retail-ABI callable entries for guest RVAs whose bodies are small enough to lift exactly
// (CPU-01-AOT-CFG-CALLABLE-01 successor).
//
// A thunk is an ordinary SysV function: six integer arguments in rdi, rsi, rdx, rcx, r8, r9 and the
// result in rax. `guest_thunk_invoke` calls it through a real function pointer, so the call itself
// exercises the retail ABI rather than a table lookup.
//
// Registered bodies (pinned ELF sha256 d0723afb...5861d3):
//   0x20d60  26 B  sha256 f4463b3b78853849ab4eec851d22d17ecde2d207efd57918e0186412bd84b403
//           cmpb $0,0x18(%rdi); je 0x20d67; ret; 0x20d67 sets the byte, reloads the manager pointer
//           from the BSS global 0x2dfcbc8 and jumps to 0x20d80 (map insert + callbacks). The map
//           insert needs the live manager/global state, so the native body flips the flag once and
//           reports the registration through a host hook. Exact contract: object+0x18 is the
//           "already registered" byte; the first call invokes the hook, later calls are no-ops.
//   0x13d20   4 B  sha256 71ce06d0aa5b8a12c09d86a62c9d2fd4f4d293dcd4e2ef33f188dad1835fbf3c
//           movl %esi,0x1c(%rdi); ret. A pure leaf setter: store the low 32 bits of rsi at
//           object+0x1c. No host state; the fixture observes the stored value after the call.
//
// The 14 one-byte `ret` targets in the resolved indirect set (0xc4f0, 0xc770, 0x19f0, 0x25aa0,
// 0x2bdd0, 0x538520, ...) are deliberately not registered: they do nothing and leave rax as the
// caller had it, which a C thunk cannot reproduce without inline assembly, and a `return 0` would
// be an invented result rather than the retail behaviour.

#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

using GuestRetailFn = std::uint64_t (*)(std::uint64_t rdi, std::uint64_t rsi,
                                        std::uint64_t rdx, std::uint64_t rcx,
                                        std::uint64_t r8, std::uint64_t r9);

struct GuestThunk {
    std::uint64_t rva;
    const char* name;
    GuestRetailFn fn;
    std::uint8_t arg_count;
};

using GuestConCmdRegisterHook = void (*)(void* object, void* manager);

void guest_set_concmd_register_hook(GuestConCmdRegisterHook hook) noexcept;
const GuestThunk* guest_thunk_find(std::uint64_t rva) noexcept;
std::size_t guest_thunk_count() noexcept;

// Args are read up to the thunk's arg_count; missing arguments are zero. Returns false for an
// unknown RVA. The call itself is a normal SysV call through the registry's function pointer.
bool guest_thunk_invoke(std::uint64_t rva, const std::uint64_t* args, std::uint64_t* result) noexcept;

}  // namespace sotc
