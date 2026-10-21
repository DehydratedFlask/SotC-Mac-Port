# Constructor-table classification (CPU-01)

Baseline: 2026-10-21. Source: private pinned ELF sha256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, constructor table
`0x2dd70e0` (573 entries, sha256 `5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28`).
Method: `tools/lift/classify_ctors.py` — a reachability walk per entry
(`tools/lift/analyze_slot_struct.py`), fully offline. Nothing is executed. Per-slot spans, digests,
call counts and class labels are in the private `local/re/work/ctor-classes.json`.

## Result

| Class | Slots | Meaning |
|---|---|---|
| `leaf_vec_copy` | 227 | one unguarded 16-byte constant copy then `ret` (17 bytes) |
| `guarded_const` | 12 | guarded static-constant initializer blocks |
| `descriptor_mutex_atexit` | 34 | publishes pointer(s), `malloc`s a 16-byte block, initializes a recursive+priority-inherit pthread mutex, registers cleanup via `__cxa_atexit` |
| `other` | 300 | real code; not constant initialisation |
| `undecoded` | 0 | every entry decoded after the parser fix below |
| **total** | **573** | |

### `guarded_const` indices

`0, 1, 2, 7, 8, 67` are lifted. `64, 65, 66, 115, 116, 117` were unlifted at the time of this
classification:

| Index | RVA | Span | Note |
|---|---|---|---|
| 0 | `0xf6cad0` | 286 B | `math_constants` (lifted, registered) |
| 1 | `0xf66640` | 286 B | clone (lifted, registered) |
| 2 | `0xf53de0` | 286 B | clone (lifted, registered) |
| 7 | `0xf4b510` | 39 B | `vec_neg036`-style guarded splat (lifted) |
| 8 | `0xf3d9d0` | 286 B | clone (lifted, registered) |
| 64 | `0xdd91b0` | 330 B | 9 guarded cells + unguarded 16-byte tail |
| 65 | `0xdd7e30` | 319 B | same guarded cells as 64 |
| 66 | `0xdd4210` | 334 B | same guarded cells as 64 |
| 67 | `0xdd41e0` | 39 B | guarded splat (lifted, registered) |
| 115 | `0xd39b00` | 467 B | 9 guarded cells plus a second block |
| 116 | `0xd366d0` | 302 B | shared guarded cells |
| 117 | `0xd33b50` | 302 B | shared guarded cells |

The 330–467 B members are **not** 39-byte one-block constructors: they repeat the guarded idiom
8–11 times and write the same `.bss` constant range (`0x3351ef8…0x3351f90`) as slots 0–2, plus a
few private cells. Any lift must model every repetition and the guard-shared cells, not just the
first block.

### `descriptor_mutex_atexit` — the slot 10–19 / 24–47 family

Contiguous runs **10–19** and **24–47**. Shared, byte-verified contract:

- `0xb30a10` = `lea rax,[rip]; ret` returning `0x12d2d1c`; `[0x12d2d1c] == 0x10`, passed to `malloc`.
- `0xb30950` initializes the mutex: `pthread_mutexattr_init` → `settype` → `setprotocol` →
  `pthread_mutex_init` → `pthread_mutexattr_destroy`.
- `0xe44a10` is the shared destructor: `pthread_mutex_destroy` (`0xb309d0`) then tail `free`.
- `__cxa_atexit(dtor = 0xe44a10, arg = &<second global>, dso = 0x1604218)` (GOT `0x15fffb8`).
- Two adjacent `.bss` globals are published per slot; every family slot's pair is recorded in the
  private manifest.

Sub-shapes: 79 B (13, 19, 24, 25, 27), 205–284 B (12, 14–18, 26, 45), 233 B (29–44). Slots
14/16/17/18 and 12/15 also carry a `__stack_chk_fail` guard; 13 and 19 do not.

Slot 10 has a native callback; **slot 11 is the explicit blocker** (`NEEDS_PRINCIPAL` history in
TASKS.md), so the family is analysed but not registered.

## Why reachability rather than a window

Two wrong results are worth remembering:

1. A fixed 0x600-byte `objdump` window made a byte scan report 49 family slots — including
   `0xf53de0`, whose reachable body has **no calls at all**; the mutex-init `call` bytes belonged to
   the following function. Classifying by reachable body removes it.
2. A linear sweep from `0xf06340` ran into the next function because a `__stack_chk_fail` block
   falls through into padding. Treating noreturn imports as terminators and walking the CFG fixed
   the span (1229 → 205 bytes).

Publish only reachability results, and never quote a body digest that came from a rejected span.

3. An `objdump` line whose encoded bytes exactly fill the 28-character byte column has **no trailing
   space** before the tab, so a `(?:[0-9a-f]{2} )+` parser silently drops that instruction — and with
   it every subsequent block reachable only through that instruction. This truncated two bodies
   (`0xdd7e30`: 319 instead of 396 bytes; `0xdd4210`: 334 instead of 486) and produced three bogus
   `undecoded` slots. The fixed parser splits on the tab; all 573 entries now decode.
