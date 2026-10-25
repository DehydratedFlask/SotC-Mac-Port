// Native lift of `0xf3c0` (`SharedStr::resize`) and `0xf6b0` (resize + NUL).
//
// ELF sha256 d0723afb…861d3. Body `0xf3c0..0xf58b` = 459 B,
// sha256 `980c6911da8fad573162fd1d97f77b1dc20677bb7eedbee3e7e5fc8fe25227fb`.
// Body `0xf6b0..0xf6d0` = 32 B,
// sha256 `bfaf9b09a6f5f0345f8341cbc75da04f7ebe4dcf9ae0d697d86bbac82da25be1`.
//
// The 0x20 B holder is the existing `SharedStr`; its `aux` field is **capacity** (proven by the
// `cmp qword [rbx+0x10],r14` / `jae` pair at `0xf3ea`/`0xf3ee`). The allocator hooks are the
// existing `SharedStrCtorHooks` from `shared_str_ctor_f1a0.hpp`; data allocations and frees use
// owner tag 0, the 8-byte control block uses owner tag 1.
//
// Four paths, keyed on the control block:
//   1. `control == 0`            : alloc align4(newLen+4), copy min(size,newLen), alloc control,
//                                  *control = 1, size = newLen, capacity = alloc-1.
//   2. `*control != 1` (shared)  : alloc new data, copy min(size,newLen), `lock xadd -1` on the
//                                  old control; if previous == 1 free old data (tag 0) and old
//                                  control (tag 1); then new control with *control = 1.
//   3. `*control == 1` and
//      `capacity >= newLen`      : only `size = newLen`.
//   4. `*control == 1` and
//      `capacity < newLen`       : alloc align16(newLen+0x10) for newLen <= 0x80,
//                                  align256(newLen+0x100) for <= 0x400, else
//                                  align1024(newLen+0x400); copy the old `size` bytes; free old
//                                  data (tag 0); keep the control block.
//
// RETURN VALUE — byte-verified correction to `shared-string-20/report.md` §3.2:
// the report table says paths 1 and 2 return true. They do not. `0xf430` zeroes `r15d`
// (`45 31 ff`) and `0xf46b` jumps to the common epilogue `0xf579`; `0xf50e` zeroes `r15d` again
// and `0xf511` jumps to `0xf579`. Only path 4 executes `mov r15b,1` at `0xf576`. The original
// therefore returns `al == 1` **only** on the capacity-short reallocation path. This lift follows
// the bytes: `resize_shared_str_f3c0` returns true only for path 4. Paths 1 and 2 still allocate;
// observe that through the allocator hooks (the original gives callers no flag for it).
//
// `0xf6b0` calls `0xf3c0` (return ignored) and then writes `data[size] = 0`. It is modelled as a
// bool so a rejected/failed resize is visible instead of a write through a stale buffer.
//
// Native additions (the original has no failure checks): a negative length, a malformed holder, a
// missing allocator/deallocator, or a failed allocation returns false with the old holder intact.
// Path 2 allocates the replacement control block before releasing the old reference, so a control
// allocation failure cannot leave the holder pointing at a freed counter. This is the only
// ordering difference from the original on a path the original would have crashed on.
//
// NOT modelled: the guest allocator bodies (`0x12820`, `0x12af0`, `0x17070`, `0x170b0`) and any
// concurrency on the holder fields themselves. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "shared_str_ctor_f1a0.hpp"  // SharedStr, SharedStrCtorHooks, allocator RVAs

namespace sotc {

// RVA and size already pinned in `shared_str_ctor_f1a0.hpp`:
//   kSharedStrResizeF3c0Rva  == 0xf3c0
//   kSharedStrResizeF3c0Size == 459
inline constexpr std::uint64_t kSharedStrResizeF3c0EndRva = 0xF58B;
inline constexpr char kSharedStrResizeF3c0Sha256[] =
    "980c6911da8fad573162fd1d97f77b1dc20677bb7eedbee3e7e5fc8fe25227fb";

inline constexpr std::uint64_t kSharedStrTerminateF6b0Rva = 0xF6B0;
inline constexpr std::size_t kSharedStrTerminateF6b0Size = 32;
inline constexpr std::uint64_t kSharedStrTerminateF6b0EndRva = 0xF6D0;
inline constexpr char kSharedStrTerminateF6b0Sha256[] =
    "bfaf9b09a6f5f0345f8341cbc75da04f7ebe4dcf9ae0d697d86bbac82da25be1";

// Owner tags: the same values the release (`0xf590`) uses for its tagged frees.
inline constexpr std::uint32_t kSharedStrResizeDataTag = 0;
inline constexpr std::uint32_t kSharedStrResizeControlTag = 1;

// Path 1/2: align4(newLen + 4), capacity = alloc - 1.
inline constexpr std::uint64_t kSharedStrResizeAlign4 = 4;

// Path 4 size classes and their alignment constants.
inline constexpr std::int64_t kSharedStrResizeSmallLimit = 0x80;
inline constexpr std::int64_t kSharedStrResizeMediumLimit = 0x400;
inline constexpr std::uint64_t kSharedStrResizeSmallSlack = 0x10;
inline constexpr std::uint64_t kSharedStrResizeMediumSlack = 0x100;
inline constexpr std::uint64_t kSharedStrResizeLargeSlack = 0x400;
inline constexpr std::uint64_t kSharedStrResizeSmallAlign = 0x10;
inline constexpr std::uint64_t kSharedStrResizeMediumAlign = 0x100;
inline constexpr std::uint64_t kSharedStrResizeLargeAlign = 0x400;

// 0xf3c0. Returns the original `al`: true only on path 4 (capacity-short reallocation). Paths 1
// and 2 allocate but return false in the original. On false the caller cannot distinguish
// "no reallocation" from "rejected"; the holder is only modified when the operation completed.
[[nodiscard]] bool resize_shared_str_f3c0(SharedStr& str, std::int64_t new_len,
                                          const SharedStrCtorHooks& hooks) noexcept;

// 0xf6b0. Runs the resize (ignoring its return, as the original does), then writes
// `data[size] = 0`. Returns false when the resize was rejected/failed or the holder has no data;
// in that case nothing is written.
[[nodiscard]] bool terminate_shared_str_f6b0(SharedStr& str, std::int64_t new_len,
                                             const SharedStrCtorHooks& hooks) noexcept;

}  // namespace sotc
