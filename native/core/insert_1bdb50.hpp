// Native lift of the 0x58-byte-node list inserter at RVA `0x1bdb50` (E558 found it, E559 lifted its
// destructor partner).
//
// ELF sha256 d0723afb…861d3. Body `0x1bdb50..0x1bdc80` = 304 B, sha256 add72de8…1313, `ret` at
// 0x1bdc77 with the stack-canary failure tail at 0x1bdc78.
//
// Signature recovered from the register use:
//   rdi = sentinel   (the list head pair itself; the same pointer `0x1bdeb0` receives)
//   rsi = callable A (0x20 B holder)
//   rdx = callable B (0x20 B holder)
//   ecx = flag       (stored as a dword at node+0x50; non-zero => skipped by the destructor)
//
// Byte-verified sequence:
//   temp_a = *A ; if (temp_a.control) lock incq (*temp_a.control)      ; 0x1bdb7c..0x1bdb9e
//   temp_b = *B ; if (temp_b.control) lock incq (*temp_b.control)      ; 0x1bdba1..0x1bdbcf
//   node = 0x17090(0x58)                                               ; 0x1bdbfc..0x1bdc0b
//   node[+0x10] = temp_a ; node[+0x30] = temp_b                        ; 0x1bdc1e / 0x1bdc27
//   node[+0x50] = flag                                                 ; 0x1bdc2c
//   node->prev = sentinel ; node->next = sentinel->next                ; 0x1bdc30 / 0x1bdc42
//   node->next->prev = node ; sentinel->next = node                    ; 0x1bdc46 / 0x1bdc49
//   incq [sentinel+0x10]                                               ; 0x1bdc4d
//   temp_b = {} ; temp_a = {} ; 0xf590(&temp_b) ; 0xf590(&temp_a)      ; 0x1bdc33..0x1bdc5b
//
// The two trailing releases are **no-ops by construction**: the temporaries are zeroed first, and
// `0xf590` returns immediately when the holder's control pointer is null (E552). Their purpose is
// only to hand the acquired reference to the node, so exactly one increment per callable survives,
// owned by the node. The native model keeps that shape rather than "optimising" the releases away.
//
// Node layout (shared with `list_dtor_1bdeb0`): prev +0x00, next +0x08, callable A +0x10,
// callable B +0x30, dword flag +0x50.
//
// What the list actually holds (E561, from the five call sites' string constants): an **application
// info registry** — named key/value entries such as
//   "Game"                      -> "Shadow of the Colossus"   (0xb72)
//   "Build Version"             -> a global functor            (0x25a538)
//   "Platform Type"             -> "ps4 - NEO" / "ps4 - STA"   (0x25a604)
//   "Is Debug Memory Available" -> "True" / "False"            (0x25a68a)
//   (dynamic name)              -> value from 0x10a90          (0x25a88f)
// Every observed insert passes flag = 1; no writer of 0 was found, so the destructor's `flag == 0`
// purge targets a state produced by a path that has not been located. The container is the pointer
// at `0x2ee68c8`, and its teardown is `0x1bdeb0` (E558/E559). This registry is **not** the ConVar
// path of `0xa54920`: that function never references `0x2ee68c8`.
//
// NOT modelled: the 0x17090/0x12820 allocator body (contract), the 0xf590 body (contract), the
// 0x15fe518 stack canary and 0xf8f3a0 failure path (host stack protection), and the meaning of the
// flag. The original performs no null check on the allocation; the native model reports a visible
// failure and restores both refcounts instead of dereferencing null. G1 PARTIAL.
#pragma once

#include <cstddef>
#include <cstdint>

#include "alloc_wrappers_17070.hpp"
#include "list_dtor_1bdeb0.hpp"
#include "shared_release_f590.hpp"

namespace sotc {

inline constexpr std::uint64_t kInsert1bdb50Rva = 0x1BDB50;
inline constexpr std::size_t kInsert1bdb50Size = 0x130;
inline constexpr std::uint64_t kInsert1bdb50EndRva = 0x1BDC80;
inline constexpr std::uint64_t kInsert1bdb50AllocRva = 0x17090;
inline constexpr std::uint64_t kInsert1bdb50ReleaseRva = 0xF590;
inline constexpr std::uint64_t kInsert1bdb50CanaryRva = 0x15FE518;
inline constexpr std::uint64_t kInsert1bdb50StackFailRva = 0xF8F3A0;
inline constexpr std::size_t kInsert1bdb50NodeSize = 0x58;
inline constexpr std::size_t kInsert1bdb50FlagOffset = 0x50;
inline constexpr std::size_t kInsert1bdb50CountOffset = 0x10;

// The inserter and the destructor build and consume the same 0x58-byte node, so the layout is
// defined once, in `list_dtor_1bdeb0.hpp`, and aliased here with the local name.
using Callable1bdb50 = Callable1bdeb0;
using Node1bdb50 = Node1bdeb0;
using List1bdb50 = List1bdeb0;

struct Insert1bdb50Hooks {
    // 0x17090(0x58) -> 0x12820 with tag 1, align 8, flags 1. Null => visible failure.
    AllocDispatchContract alloc{};
    // 0xf590 on the two zeroed temporaries. Only reached with a null control pointer in the
    // original, so the hooks are never used on the success path; kept for fidelity.
    SharedReleaseF590Hooks release{};
};

struct Insert1bdb50Result {
    bool ok = false;
    bool allocated = false;
    int acquires = 0;          // one per callable with a live control block
    Node1bdb50* node = nullptr;
    std::uint64_t count_after = 0;
    bool temp_release_ok = true;
    bool refs_restored = false;  // failure path: the acquires were released again
};

// Inserts a node carrying `a`, `b` and `flag` at the front of `sentinel`'s list, taking one shared
// reference to each callable. `sentinel` is the head pair itself, matching the original's `rdi`.
[[nodiscard]] Insert1bdb50Result insert_1bdb50(List1bdb50& sentinel,
                                               const Callable1bdb50& a,
                                               const Callable1bdb50& b,
                                               std::uint32_t flag,
                                               const Insert1bdb50Hooks& hooks) noexcept;

}  // namespace sotc
