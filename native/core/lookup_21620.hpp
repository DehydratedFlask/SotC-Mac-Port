// Registry/map lookup `0x21620` — contract pins only (CPU-01-INIT-21620-SPEC-01 / Luna E1042).
//
// NOT IMPLEMENTED: stop rules in docs/checkpoints/2026-10-30-init-21620-luna-spec-e1042.md
// block native body until key-descriptor layout (both callsites), tree/fallback views, and
// 0x3b390/0x3b3c0 lock lifetime are pinned. Do not reuse lookup_1fc80.
//
// Callers (hook-only today): vslot40 miss @ 0x53327d (r_fps key); lazy_bind @ 0x91227 (this+8).
#pragma once

#include <cstddef>
#include <cstdint>

namespace sotc {

inline constexpr std::uint64_t kLookup21620Rva = 0x21620;
// Hot span through ret @ 0x217e0 (checker also pins full 0x1d0 B including cold tail).
inline constexpr std::size_t kLookup21620HotSize = 449;
inline constexpr std::size_t kLookup21620BodySize = 0x1d0;
inline constexpr char kLookup21620HotSha256[] =
    "787bf4ada236589505be9d753be8317445d253e8f43a2db55f102b0cf77b5185";
inline constexpr char kLookup21620BodySha256[] =
    "a7ba043b102574823a8381f36c654b1bf30f6ef812b64e209e9b438d670413bd";

// Observable node/key field offsets (static disassembly; types NOT proven).
inline constexpr std::size_t kLookup21620NodeNameOff = 0x20;
inline constexpr std::size_t kLookup21620NodeKeyOff = 0x28;
inline constexpr std::size_t kLookup21620NodeValueOff = 0x40;

inline constexpr std::uint64_t kLookup21620LockBeginRva = 0x3b390;
inline constexpr std::uint64_t kLookup21620LockEndRva = 0x3b3c0;

}  // namespace sotc
