# Checkpoint — `0x25a4d0` app-info installer decoded; E561 amended

- Local: 2026-10-23 08:0x (America/Chicago)
- Agent: static-RE worker; integrator byte replay
- Task: `CPU-01-APPINFO-INSTALLER-01`
- Gate: **G1 PARTIAL — static only**
- Evidence: E563 (amends E561)

## Extent and ABI

- Body `0x25a4d0..0x25a8bf` inclusive (ret), 1007 B, plus cold stack-check block
  `0x25a8c0..0x25a8c5`; function end `0x25a8c6`, **1014 B**, sha256 `12a2d0d7…1347fa`.
- Prologue `push rbp; mov rbp,rsp; push r15,r14,r13,r12,rbx; sub rsp,0x178` (frame 0x1A0).
- `rsi` = registry container (saved in `r14`, used as the list-A sentinel), `rdi` = app object
  (saved in `r15`, only `[rdi+0x2668]` is read), `rdx`/`rcx` unused, returns `void`.

## Entries (all flag `ecx = 1`)

| order | site | name | value |
|---|---|---|---|
| 1 | `0x25a538` | `"Build Version"` (13) | `*(0x2dfcf40+0x60)`, the shared build-version string |
| 2 | `0x25a604` | `"Platform Type"` (13) | `0x1360("%s%s","ps4", IsNeo() ? " - NEO" : " - STANDARD")` |
| 3 | `0x25a68a` | `"Is Debug Memory Available"` (25) | borrowed `"True"` / `"False"`; predicate `0x120c0` is `xor eax,eax; ret` ⇒ always `"False"` in this build |
| 4 | `0x25a88f` | `"Camera Location"` (15) | `0x10a90("setCameraPos %.2f %.2f %.2f %.2f %.2f %.2f", 6 doubles)`, only when `[owner+0x2668] != 0` |

All four values are **refcounted strings** `{data,size,capacity,control}`; the inserter copies the
0x20 B descriptor and takes one reference, and each site releases its local exactly once. Names are
borrowed `.rodata` views. `"Game"` is **not** installed here.

## E561 corrections

1. `"ps4 - STANDARD"` is 14 characters, not `"ps4 - STA"`.
2. `[0x2dfcf40+0x60]` points at the shared build-version string, **not** a global functor.
3. The camera entry is conditional on `[owner+0x2668]`, not unconditional.
4. `0xb72` is not a global constructor: it is the insert call inside free function `0xb10`
   (`0xb10..0xb9a`, 139 B, sha256 `4220f09b…5d8494`), which inserts `"Game"` →
   `"Shadow of the Colossus"`. `0x1290` registers `0xb10` as a list-C delegate at `0x12f5`.

## Dispatch order correction

`0x1bdf30` walks list-3 from `sentinel.prev` and then follows `node.prev` (`0x1bdf66 mov rbx,[rbx+0x40]`,
`0x1bdf79 mov rbx,[rbx]`). Because `0x1beeb0` pushes at the front, `sentinel.prev` is the **oldest**
node and the walk is **FIFO in registration order**, not LIFO as the worker report inferred.
`0x2569d0` registers `0x25a4d0` before `0x1290` registers `0xb10`, so product info is inserted first
and "Game" last. Final list-A front-to-back is therefore
`Game, Camera Location, Is Debug Memory Available, Platform Type, Build Version`, and list-C purge
`0x1bdeb0` skips all five because every flag is 1. The container destructor `0x1bede0` frees them.

## Verification

- Private evidence: `local/re/evidence/appinfo-installer-25a4d0/` (`report.md`, `summary.json`,
  annotated listings under `work/`).
- Native tie: `tools/lift/check_fapp_info_install.py` pins both extents/hashes, the five insert
  calls, the four value producers and all twelve strings; exit 0 on the pinned ELF.

## Unresolved

Who reads list A; dispatch timing/frequency; whether a second dispatch can duplicate entries; the
`0x2dfcf40` object identity; the camera handle chain; the `flag = 0` producer. **G1 PARTIAL.**
