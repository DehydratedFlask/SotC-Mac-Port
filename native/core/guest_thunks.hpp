// Retail-ABI callable entries for guest RVAs whose complete FDE body is a proved leaf shape
// (CPU-01-AOT-CFG-CALLABLE-01 continuation).
//
// A thunk is an ordinary SysV function: six integer arguments in rdi, rsi, rdx, rcx, r8, r9 and the
// result in rax. `guest_thunk_invoke` calls it through a real function pointer, so the call itself
// exercises the retail ABI rather than a table lookup. The generated table
// `core/guest_thunk_table.inc` is re-derived by `tools/lift/entry_closure.py` from the pinned ELF:
//   - `void_noop`      one-byte `ret`; registered only when backward rax liveness proves every
//                      resolved call site ignores the return.
//   - `const_u32`      `movl $imm,%eax; retq`; exact zero-extended return.
//   - `zero_u32`       `xorl %eax,%eax; retq`; exact zero return.
//   - `set_u32`        `movl %esi,disp(%rdi); retq`; exact store, return ignored at every known site.
//   - `vtable_pair_ctor` 20 B `mov rsi+0x10 -> rdi+0x10`, pinned address -> `rdi+0x20`, returns
//                      `rsi+0x10`; exact for a host-supplied object.
// Two custom bodies. `0x20d60` reproduces the `+0x18` flag test/store and reports its manager-map
// insert through a host hook: the insert (`0x20d80`) needs the live manager pointer from BSS global
// `0x2dfcbc8`, which the host does not have. `0x9844b0` tail-jumps to imported printf with the
// pinned `%s` format and forwards argument 3; a host hook observes that string. Entries whose
// complete body is not a proved leaf keep an exact RVA and reason in `docs/entry_closure.json`
// under `native_thunks.blocked`.

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

// One CFG-resolved indirect site, its host vtable image and the native entry it dispatches to.
struct GuestDispatchSite {
    std::uint64_t site_rva;
    std::uint64_t function_rva;
    std::uint64_t target_rva;
    std::uint64_t vtable_rva;
    std::uint64_t slot_offset;
};

using GuestConCmdRegisterHook = void (*)(void* object, void* manager);
using GuestPrintfHook = void (*)(const char* text);

void guest_set_concmd_register_hook(GuestConCmdRegisterHook hook) noexcept;
void guest_set_printf_hook(GuestPrintfHook hook) noexcept;
const GuestThunk* guest_thunk_find(std::uint64_t rva) noexcept;
std::size_t guest_thunk_count() noexcept;

// Args are read up to the thunk's arg_count; missing arguments are zero. Returns false for an
// unknown RVA. The call itself is a normal SysV call through the registry's function pointer.
bool guest_thunk_invoke(std::uint64_t rva, const std::uint64_t* args, std::uint64_t* result) noexcept;

const GuestDispatchSite& guest_dispatch_site() noexcept;
const std::uint64_t* guest_resolved_targets(std::size_t* count) noexcept;
const std::uint8_t* guest_dispatch_vtable(std::size_t* size) noexcept;

// Read the target RVA from the host representation of the guest vtable (slot bytes), then invoke
// that entry through `guest_thunk_invoke`. Fails for a short/unknown slot or an unregistered target.
bool guest_dispatch_slot(const std::uint8_t* vtable, std::size_t vtable_size, std::size_t slot_offset,
                         const std::uint64_t* args, std::uint64_t* result,
                         std::uint64_t* target_rva) noexcept;

}  // namespace sotc
