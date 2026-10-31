#pragma once

// CPU-01-INIT-91210-LAZY-BIND-01: native model of vtable slot 0 at 0x91210 (0x58 B).
// `this` is the object whose first qword lives at global 0x2efe318; the bound
// sub-object is this+0x28 (global 0x2efe340).
//
// Byte polarity (corrects the E530 "vtable[6] must be nonzero" note):
//   * [this+0x28] already set -> return 1, no lookup
//   * lookup 0x21620(this+8) null, or vtable+0x18 returns not 1 -> leave unbound, return 0
//   * store the sub-object, then vtable+0x30: eax==0 keeps the bind; eax!=0 clears it
//   * return 1 iff [this+0x28] != 0
// 0x21620 and both virtual calls stay hooks. Missing hooks on the slow path fail closed.

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kLazyBind91210Rva = 0x91210;
inline constexpr std::size_t kLazyBind91210Size = 0x58;
inline constexpr char kLazyBind91210Sha256[] =
    "46be9cad276c1d24bd7a7b6b23ab45d4616346774a4d9aee77ba63b53166b802";

inline constexpr std::size_t kLazyBind91210BoundOffset = 0x28;
inline constexpr std::size_t kLazyBind91210NameSlotOffset = 0x08;

struct LazyBind91210Hooks {
    void* (*lookup_21620)(void* host, void* name_slot) = nullptr;
    int (*vcall_18)(void* host, void* sub) = nullptr;
    int (*vcall_30)(void* host, void* sub) = nullptr;
    void* host = nullptr;
    [[nodiscard]] bool complete() const noexcept {
        return lookup_21620 != nullptr && vcall_18 != nullptr && vcall_30 != nullptr;
    }
};

struct LazyBind91210Result {
    bool ok = false;
    int returned = 0;
    bool fast_path = false;
    bool stored = false;
    bool rolled_back = false;
};

[[nodiscard]] LazyBind91210Result run_lazy_bind_91210(void* object,
                                                      const LazyBind91210Hooks& hooks) noexcept;

}  // namespace sotc
