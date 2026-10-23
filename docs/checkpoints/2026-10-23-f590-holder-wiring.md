# Checkpoint — 0xf590 holder census and the 17 Init-slice wirings

- Local: 2026-10-23 08:1x (America/Chicago)
- Agents: static-RE worker (census), code worker (wiring), integrator replay
- Tasks: `CPU-01-F590-HOLDER-CENSUS-01`, `CPU-01-F590-WIRE-17-01`
- Gate: **G1 PARTIAL — static + fixture only**
- Evidence: E565

## Census

Two independent scans (raw `E8`/disp32 byte scan and a full `llvm-objdump` sweep) find the same
**10,168** direct `call 0xf590` sites, all on instruction boundaries, 0 disagreements. The callee
body byte-matches `native/core/shared_release_f590.hpp` (73 B, `0xf590..0xf5d9`).

| class | n | meaning |
|---|---:|---|
| `A-frame-slot` | 8,173 | `rdi = &frame[rsp/rbp ± k]`, exact offset |
| `A-object-offset` | 1,189 | `rdi = base + k`, base not identified |
| `A-slot-pointer` | 321 | `rdi = *(frame slot)` |
| `A-loop-cursor` | 288 | array element cursor |
| `B-global` | 38 | 37 static addresses |
| `C-live-in` | 44 | incoming register |
| `C-call-result` | 9 | `rax` of a preceding call |
| `C-unresolved` | 106 | walk stopped at a join/limit |

## The 17 Init-slice sites

Every native `call_f590` hook is inside `0x531d60` and is a 32 B `{ptr,len,0,0}` stack temporary
with `+0x10..0x1f` zeroed before the call. `0x26800` reads only `[src+0x18]`, and `0xa2ed00` reads
`[rdx]`/`[rcx]`; neither writes the caller's `+0x18`. The lifted release therefore returns
`had_control == false` and never runs `0x12af0`/`0x170b0` at these sites — an exact, inert result,
not an invented one.

## Change

All 17 modules (`init_531d60_post_base_26800_f590`, `_sbj`, `_sboj`,
`init_531d60_post_base_f590_a2ed00` + `_u2.._u13`, `init_531d60_post_base_continue_a`) now expose:

```cpp
SharedReleaseF590Result (*call_f590)(SharedHolderF590&) = nullptr;  // optional host override
SharedReleaseF590Hooks release_hooks{};                              // 0x12af0 / 0x170b0
```

The body builds a zeroed `SharedHolderF590`, prefers the override, otherwise calls
`shared_release_f590`, stores `out.release`, and keeps the original `call_f590_count`. Tests assert
the measured no-op for the default path and the override path for each module. A mutation removing
the lift from `_u9.cpp` made the slice test fail (exit 1 at `rel.ok`), then was restored.

## Verification

| Command | Result |
|---|---|
| `cmake --build local/build/orch-r22 -j 12` | exit 0, 346/346 at that point |
| targeted 6 tests incl. `appmain_init_ctor_98a90_slice` | 6/6 pass |
| full `ctest -j 12` | **344/344 pass** |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | **442 passed, 0 skipped, 0 failed** |

Private evidence: `local/re/evidence/f590-callsite-holders/` (10,168-row CSV/JSON, `ctrl_slot.json`,
`init_holder_state.json`, `verify.py` 18/18 fresh re-decodes) and
`local/re/evidence/f590-wiring/report.md`.

## Limits

Only the 17 fully determined P1 sites are wired. The 8,194 P2 sites with an exact address but a
`+0x18` slot written by an unmodelled call need the same callee-body check first; P3/P4 remain
unresolved. `release_hooks` is unreachable with the zeroed holders but is kept so a host-supplied
holder needs no signature change. No boot/audio/save claim. **G1 PARTIAL.**
