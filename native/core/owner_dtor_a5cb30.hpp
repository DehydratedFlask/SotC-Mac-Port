// Row-9 owner destructor `0xa5cb30` (`kOwnerMemberRows[9]`, the `kDtorThenFree` half).
//
// Characterization source: `local/re/evidence/owner-ctor-a5c850/report.md` section 9 (the dtor
// contract) plus the byte decode recorded in
// `local/re/evidence/owner-dtor-a5cb30-native/report.md`. Offline static analysis only.
//
// Body `0xa5cb30..0xa5cd82`, size `0x252` (594 B), sha256
// `0b5c450169b29ead0eed06b88cad7993ad01d310dc7142bc3a21302a727f0ff8`, 142 instructions,
// 22 call instructions (19 direct, 3 indirect through element/object vtables), one epilogue.
// There is no stack canary.
//
// Teardown order (byte order, not reverse-ctor guesswork):
//   1. `+0xa510`, then `+0xa500` -> `0x3b440` (`sceKernelDeleteEventFlag`);
//   2. `+0xa4f8` -> vtable slot `+8`, then the field is set to 0;
//   3. `+0x8bd0` vector: 0xc8 stride, element vtable slot 0, then free `begin` (`0x170c0`) unless
//      it is null or equals the marker *field* at `header + 0x20`;
//   4. `+0x72a8` vector: same shape;
//   5. `+0x7270` -> `0xb09690` (clear `0xb0a080`, then free the node read from `+0x7278`);
//   6. `+0x7250` -> `0xf590` (shared release; the ctor leaves an empty handle);
//   7. `+0x6fe8` vector (0x48 stride): per element `0xf590(elem + 0x28)` *then* `0xf590(elem)`;
//   8. `+0x6ac0` vector (0x28 stride): per element `0xf590(elem)`;
//   9. `+0x68a0` buffer: free unless null / inline marker (no element loop);
//  10. `+0x5c60` -> `0xa62340` (drain). The ctor's `+0x5c70 == 0` makes the spin and lane walk
//      dead; the tail still writes `+0x5c80 = 0`, `+0x5c98 = 0`, `+0x5c90 = ~0`;
//  11. `+0x28` vector (0x38 stride): per element `0xf590(elem + 0x18)`; if byte `elem + 8`, then
//      `0x14d2d0([elem])` and `elem + 8 = 0`; then always `0x14ccc0([elem])` and `[elem] = 0`;
//  12. `+0x20` gate: if byte `+0x20`, then if byte `+0x18` call `0x14d2d0([+0x10])` and clear
//      `+0x18`; then always `0x14ccc0([+0x10])` and qword `[+0x10] = 0`.
//
// Corrections to report section 9 (from the bytes):
//   - step 12's optional call is gated on byte `+0x18`, not on `+0x20` alone; `+0x20` gates the
//     whole head block and `0x14ccc0`/the pointer clear always follow when it is armed;
//   - the "inline" tests compare `begin` against the *value stored at* `header + 0x20`, which the
//     ctor set to `object + buffer`, not against the field's address;
//   - the empty drain still writes the three tail fields above;
//   - step 7 performs the `+0x28` release before the `+0x00` release.
//
// Partial-initialization contract:
//   - a fully constructed object never has `+0x20 != 0`, so the dtor never reads the `+0x18` byte
//     or the qword at `+0x10` (the ctor writes `+0x10` as a single byte; report R1 stays open);
//   - the never-written `+0x7270` qword, the vector `+0x18` slots and `+0xa524..+0xa530` are not
//     read;
//   - a ctor prefix that faulted before `0xb09650` has a null tree node. The guest `0xb09690`
//     dereferences it, so the port reports `tree_destroy` instead of inventing a tree.
//
// Hooks model the callees that are not lifted here. A missing or false hook is a visible
// `failed_at`; the function never claims a release that did not run. The empty-drain path is native
// (the only path a constructed object reaches); an armed drain is a host contract through
// `drain_armed`, which owns all of `0xa62340` including its three tail stores.
#pragma once

#include <cstddef>
#include <cstdint>

#include "owner_ctor_a5c850.hpp"

namespace sotc {

inline constexpr std::size_t kOwnerDtorA5CB30InstrCount = 142;
inline constexpr std::size_t kOwnerDtorA5CB30CallCount = 22;
inline constexpr std::size_t kOwnerDtorA5CB30DirectCallCount = 19;

// Destructor-only layout (the constructor header carries the rest).
inline constexpr std::size_t kOwnerA5C850HeadPtrOffset = 0x10;   // qword read only when +0x20
inline constexpr std::size_t kOwnerA5C850HeadFlagOffset = 0x18;  // byte read only when +0x20
inline constexpr std::size_t kOwnerA5C850HeadGateOffset = 0x20;  // byte, ctor writes 0
inline constexpr std::size_t kOwnerA5C850VectorBeginSlot = 0x00;
inline constexpr std::size_t kOwnerA5C850VectorEndSlot = 0x08;
inline constexpr std::size_t kOwnerA5C850VectorMarkerSlot = 0x20;
inline constexpr std::size_t kOwnerA5C850StrideC8 = 0xC8;
inline constexpr std::size_t kOwnerA5C850Stride48 = 0x48;
inline constexpr std::size_t kOwnerA5C850Stride28 = 0x28;
inline constexpr std::size_t kOwnerA5C850Stride38 = 0x38;
// 0xa62340 is called with base = object + 0x5c60.
inline constexpr std::size_t kOwnerA5C850DrainWordOffset = 0x10;      // object + 0x5c70
inline constexpr std::size_t kOwnerA5C850DrainTailMaskOffset = 0x20;  // object + 0x5c80
inline constexpr std::size_t kOwnerA5C850DrainTailSeqOffset = 0x30;   // object + 0x5c90
inline constexpr std::size_t kOwnerA5C850DrainTailCountOffset = 0x38; // object + 0x5c98

struct OwnerDtorA5CB30Hooks {
    // 0x3b440(at): `rdi = [at]; jmp 0xf8f960` (sceKernelDeleteEventFlag PLT, GOT 0x16002a0).
    // Called with `object + 0xa510` first, then `object + 0xa500`. Read the handle yourself; a
    // zero handle means no kernel object was created (ctor-prefix case). Null/false => visible.
    bool (*event_flag_destroy)(void* flag) = nullptr;
    // `[vptr + 8](object)` for the non-null qword at `object + 0xa4f8`. The dtor reads the vptr and
    // passes it; it must be non-null. Null/false => visible.
    bool (*poly_release)(void* object, void* const* vptr) = nullptr;
    // `[vptr](element)` for every element of the two 0xc8-stride vectors (+0x8bd0, +0x72a8).
    bool (*element_release)(void* element, void* const* vptr) = nullptr;
    // 0xb09690(at): clear the tree (`0xb0a080`) and free `[at + 8]` via `0x170b0`. The node read
    // from `+0x7278` is the allocation that must be released; a null node is a ctor prefix and
    // must be reported false.
    bool (*tree_destroy)(void* at) = nullptr;
    // 0xf590(holder): shared release of a 0x20 B holder. Called for `+0x7250`, twice per element
    // of `+0x6fe8` (+0x28 before +0x00), once per element of `+0x6ac0`, and for `elem + 0x18` of
    // `+0x28`. The existing `shared_release_f590` can back this hook.
    bool (*shared_release)(void* holder) = nullptr;
    // 0x170c0(begin): tagged free (tag 2) of a vector's heap buffer. Never called for a null or
    // inline buffer.
    bool (*buffer_free)(void* buffer) = nullptr;
    // 0xa62340(base), base = `object + 0x5c60`, only when `[base + 0x10] != 0` (the ctor leaves it
    // null). The hook owns the spin, the lane vtable calls, the three tail stores and the release
    // of the flag word; the null path is native.
    bool (*drain_armed)(void* base) = nullptr;
    // 0x14d2d0(object): conditional refcount release used by the `+0x28` element flag and by the
    // `+0x18` head flag.
    bool (*ref_release_conditional)(void* object) = nullptr;
    // 0x14ccc0(object): unconditional refcount release, followed by clearing the owning field.
    bool (*ref_release)(void* object) = nullptr;
};

struct OwnerDtorA5CB30Result {
    bool ok = false;
    const char* failed_at = nullptr;
    std::size_t event_flags = 0;
    std::size_t polymorphic_releases = 0;
    std::size_t element_releases = 0;
    std::size_t tree_destroys = 0;
    std::size_t shared_releases = 0;
    std::size_t buffers_freed = 0;
    std::size_t ref_conditionals = 0;
    std::size_t ref_releases = 0;
    bool drain_ran = false;
};

// Runs the recovered teardown. `object` must be writable for 0xa530 B and 8-byte aligned.
// `failed_at` values: "null-object", "object-align", "event_flag_1", "event_flag_0",
// "poly_release", "vector_8bd0_span", "vector_8bd0_element", "vector_8bd0_buffer",
// "vector_72a8_span", "vector_72a8_element", "vector_72a8_buffer", "tree_destroy",
// "shared_7250", "vector_6fe8_span", "vector_6fe8_element", "vector_6fe8_buffer",
// "vector_6ac0_span", "vector_6ac0_element", "vector_6ac0_buffer", "vector_68a0_buffer",
// "drain", "vector_28_span", "vector_28_element", "vector_28_conditional",
// "vector_28_release", "vector_28_buffer", "head_conditional", "head_release".
// `ok == false` stops at the first failure; sub-objects released before it stay released, exactly
// like the guest body's unconditional prefix.
[[nodiscard]] OwnerDtorA5CB30Result owner_dtor_a5cb30(
    void* object, const OwnerDtorA5CB30Hooks& hooks) noexcept;

}  // namespace sotc
