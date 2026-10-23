// Native lift of the shared-ownership release at RVA `0xf590` (`__shared_ptr_destroy`-shaped).
//
// ELF sha256 d0723afb…861d3. Body `0xf590..0xf5d9` = 73 B.
// Byte-verified sequence:
//   rbx = rdi                       ; rdi points at a 0x20 B holder
//   rax = [rbx+0x18]                ; control block / refcount pointer
//   test rax,rax ; je ret           ; no control block => nothing to release
//   rcx = -1 ; lock xadd rcx,[rax]  ; atomically decrement, rcx = previous value
//   cmp rcx,1 ; jne ret             ; still referenced => return
//   rdi = [rbx] ; esi = 0 ; call 0x12af0   ; tagged deallocate of the pointee (tag 0)
//   rdi = [rbx+0x18] ; test ; je ret
//   jmp 0x170b0                     ; operator delete on the control block (tail call)
//
// This is the counter used by the many `call 0xf590` sites across the recovered Init slices: a
// release that only destroys when the last reference goes away.
//
// The add side is **not** a single function. E557 proposed `0x14ccb0` and then withdrew it (it
// increments a 32-bit field at `[object+0x30]`, the wrong width and offset). E558 found the real
// shape: acquires are inline, indirect `lock incq (%reg)` on the control block — 33 sites in the
// executable segment, e.g. `0x1bdb9d`/`0x1bdbce` inside the 0x58-node inserter `0x1bdb50`. There is
// no add-side function to lift; each site increments in place.
//
// NOTE (E553): `0x12af0` is not a C++ destructor but the allocator's **tagged deallocate
// dispatcher** — `0x170b0`/`0x170c0` reach the same function with tags 1 and 2. The tag-0 call here
// therefore frees the pointee's storage; any destructor work happens inside the dispatcher and is
// not observable from this body.
//
// NOT modelled: bodies of `0x12af0` and `0x170b0`; the layout
// of the object at `[holder]`; any weak-reference semantics (none observed).
// G1 PARTIAL — the refcount protocol is exact, the destroy/free callees are contracts.
#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kSharedReleaseF590Rva = 0xF590;
inline constexpr std::size_t kSharedReleaseF590Size = 73;
inline constexpr std::uint64_t kSharedReleaseF590EndRva = 0xF5D9;
inline constexpr std::uint64_t kSharedReleaseF590DestroyRva = 0x12AF0;
inline constexpr std::uint64_t kSharedReleaseF590FreeRva = 0x170B0;
inline constexpr std::size_t kSharedReleaseF590ControlOffset = 0x18;

// The 0x20 B holder the release operates on. Only `object` and `control` are read here; the
// two middle words belong to the owning type and are untouched.
struct SharedHolderF590 {
    void* object;                                  // +0x00  destroyed when the count hits zero
    void* reserved_08;                             // +0x08  untouched
    void* reserved_10;                             // +0x10  untouched
    std::atomic<std::uint64_t>* control;           // +0x18  refcount / control block pointer
};

struct SharedReleaseF590Hooks {
    // 0x12af0(object, 0) — destroy the pointee. Null => reported as not destroyed.
    void (*destroy)(void* object, int flags) = nullptr;
    // 0x170b0(control) — free the control block. Null => reported as not freed.
    void (*free_control)(void* control) = nullptr;
};

struct SharedReleaseF590Result {
    bool ok = false;          // true when the release completed or was a no-op by design
    bool had_control = false;
    bool decremented = false;
    bool was_last = false;    // previous count was 1
    bool destroyed = false;
    bool freed = false;
    std::uint64_t previous = 0;
};

// Runs the recovered release. The decrement is a real atomic, so concurrency behaves like the
// original. A last-reference release with a missing hook returns ok == false instead of claiming
// the object was destroyed or freed.
[[nodiscard]] SharedReleaseF590Result shared_release_f590(const SharedHolderF590& holder,
                                                          const SharedReleaseF590Hooks& hooks) noexcept;

}  // namespace sotc
