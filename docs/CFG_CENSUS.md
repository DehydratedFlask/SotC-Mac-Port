# Whole-program direct-call census (CPU-01-AOT-CFG-SCALE-01)

**Evidence label: STATIC_ANALYSIS_VERIFIED.** Nothing here was produced by executing the game, the ELF, or any
guest instruction stream; every number comes from reading three files on disk. Scope: first bounded increment of
whole-program CFG closure — the direct call graph of our own ELF, with the indirect frontier enumerated rather
than guessed. Functions are identified by unwind (FDE) ranges.

## Inputs (private, gitignored) and commands

```
local/re/eboot.elf                                        d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3
…/registry-analysis-agent/all.dis                         9c1962541d929c167dec881a1f7093159bc5b156b71e873cd1bb3325dc107e83
…/registry-analysis-agent/fde_ranges.json                 f507ab6cdc67e12c07a03be2c51cf02e8fd5fb7e27d00c45f1817c15bd226dd9
```

```sh
python3 -I tools/lift/cfg_census.py --seed 0x531d60 --json local/validation/aot-cfg/closure_531d60.json
python3 -I tools/lift/cfg_census.py --seed 0x1bdf30 --json local/validation/aot-cfg/closure_1bdf30.json
python3 -I tools/lift/cfg_census.py --seed 0x531d60 --seed 0x1bdf30 --json local/validation/aot-cfg/closure_union.json
```

Each run streams `all.dis` (251.7 MB, 6,612,510 lines) once in ~1.1 s — no sampling was needed, the performance target is met with a wide margin.

## Function universe and coverage

| metric | value |
| --- | --- |
| FDE ranges / total bytes | 55,790 / 15,839,273 |
| FDE extent | `0x80 .. 0xf8f321` |
| overlaps / duplicate keys / zero length | 0 / 0 / 0 |
| executable `PT_LOAD#0` | `0x0 .. 0x1593978` (22,624,632 B) |
| coverage of that segment | 15,839,273 B = **70.01 %** |

The 30 % gap is not missing code: one 6,309,463 B gap starts at `0xf8f321`,
immediately after the final `retq` at `0xf8f320`, and is the read-only data tail of
the R+X segment; the other 51,421 gaps are 475,896 B of alignment padding.

## Decoded call sites and direct edges

| metric | value |
| --- | --- |
| `callq` sites | 227,401 (direct 179,920, indirect 47,481) |
| `jmp`/`jmpq` | 99,147 (direct 83,638, indirect 15,509) |
| unparsed call lines / call sites outside any FDE | 0 / 17,268 |
| internal edges (target in an FDE range) | 148,201 sites, 19,322 callees |
| internal unique (caller, callee) pairs | 86,774 |
| external edges inside the executable segment | 27,927 sites, 1,045 callees |
| external edges outside the image | 3,792 sites, 3,792 callees |
| unresolved | 0 |

All 148,201 internal edges land exactly on an FDE start (0 mid-function targets).
The external class is one real structure, not noise: all 27,366 external sites
whose *source* is FDE-covered land in `0xf8f380 .. 0xf91740`, the PLT
(`jmpq *GOT(%rip)` stubs of 16 B, then `pushq $index; jmp 0xf8f380`, last index
`0x23a`). The 558 external sites outside that span and all 3,792 out-of-image
targets originate only from non-FDE (data) addresses — **0** out-of-image targets
come from real code. The direct call graph is therefore closed: every direct call
from real code enters another FDE function or the PLT.

## Closure

| seed | functions reached | edges followed | indirect frontier |
| --- | --- | --- | --- |
| `0x531d60` (Init entry) | **608** | 1,496 | **105** (17.3 %) |
| `0x1bdf30` (dispatch) | **416** | 1,175 | **59** (14.2 %) |
| both seeds (union) | **817** | 2,102 | **136** (16.6 %) |

Frontier = closure members containing at least one `callq *` or `jmpq *`. Largest contributors: `0x13380`
(10 indirect calls), `0xc300`, `0xc580` (5 each), `0x34d40` (2 indirect jumps). Two seeds reach 1.5 % of the
55,790 known functions.

## Limits

- **No indirect target is resolved.** 47,481 indirect calls and 15,509 indirect
  jumps are counted and attributed to their containing function only; the
  frontier above is the entire boundary this increment can establish.
- **No exception landing pads.** LSDA / call-site tables are not read, so
  `catch`-only edges are absent.
- **Disassembly-derived, not decoder-derived.** Addresses come from
  `llvm-objdump` text; in data regions objdump prints plausible instructions
  (`outsl`, `insb`). Those sites are counted but separated by non-FDE source.
- **Call edges only.** Intra-function branches are ignored; direct tail calls are
  excluded by default (`--include-direct-jumps` adds 42 functions, +6.9 %, from
  `0x531d60`).
- **FDE coverage is the function definition.** The PLT has no FDE, so imports
  terminate every path; a function without an unwind entry would be invisible.
- **Private artifacts required.** The tool needs `all.dis`, `fde_ranges.json` and
  the pinned ELF, none publishable, so it cannot run from a clean checkout.

Nothing above demonstrates boot, rendering, audio or save/load — it describes the shape of the call graph, not that anything works.
