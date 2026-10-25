# Checkpoint — what the 0x58 list actually holds: an application info registry

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-REGISTRY-SEMANTICS-01`
- Gate: **G1 PARTIAL — semantics identified; the container's construction is still not native**
- Evidence: E561

## Method

E560 left the natural next question: who calls `0x1bdb50`? A bulk `E8 rel32` scan finds five call
sites — `0xb72`, `0x25a538`, `0x25a604`, `0x25a68a`, `0x25a88f` — and every one of them builds the
callable arguments as **string views** (`{char*, size_t}` at `+0x00`/`+0x08`) before calling, with
`ecx = 1`. Reading the string constants they pass names the registry.

## OBS — the entries

| Site | Name | Value | flag |
|---|---|---|---|
| `0xb72` | `"Game"` (4) | `"Shadow of the Colossus"` (22) | 1 |
| `0x25a538` | `"Build Version"` (13) | global functor `[0x2dfcf40+0x60]` | 1 |
| `0x25a604` | `"Platform Type"` (13) | `"ps4 - NEO"` / `"ps4 - STA"` | 1 |
| `0x25a68a` | `"Is Debug Memory Available"` (25) | `"True"` / `"False"` | 1 |
| `0x25a88f` | built at `-0x160` | produced by `0x10a90` | 1 |

The value slot is a 0x20 B holder like the name slot, so an entry's value may be a **string or a
callable** — `"Build Version"` stores a functor pointer, the others store text.

## OBS — the container and its family

- The registry pointer is `*(0x2ee68c8)`; the only reference to that address in the teardown region
  is `0x25dc6c` (`lea rax,[0x2ee68c8]; rdi = *(rax); call 0x1bdeb0`).
- `0x25a4d0(rdi, rsi = registry)` is the function that installs the four product-info entries. It
  has **no direct caller**; it is reached through a function-pointer slot at `0x15ff080`
  (`R_X86_64_RELATIVE` RELA), the fifth entry of the table
  `{0x259b30, 0x2662b0, 0x362cb0, 0x3d6930, 0x25a4d0, 0x260e90, 0x2eee8b0, 0x24c15a0}`.
- `0x1bdb50` and `0x1bdeb0` themselves have **no** relocation entries, which is why the earlier
  addend scan found nothing: they are only ever called directly.

## Negative result worth keeping

`0xa54920` — the save-ConVar registrar from E540 — does **not** reference `0x2ee68c8` anywhere in
its body. So this registry is *not* the ConVar path, despite the superficial resemblance (both are
name/value tables). Recording the negative stops a plausible but unsupported link from being
inherited by later work.

## Consequence for the flag

Every observed insert passes `flag = 1`, and no writer of `0` was located. The destructor's
`flag == 0` purge therefore targets a state produced by a path that has not been found yet — it is
not the normal teardown of live entries. Both lifted headers now say exactly that instead of
"meaning unknown".

## Verification

```
ctest --test-dir local/build/orch-r22 -> 100% tests passed out of 339
tools/lift/run_all_checkers.sh        -> 419 passed, 20 skipped, 0 failed
```

## Next exact action

`0x25a4d0`'s caller is the table at `0x15ff080`. Identify what owns that table (scan the RELA set
for references to `0x15ff060..0x15ff0a0` and for the object that passes `rsi = registry`), because
that object is what ultimately constructs `0x2ee68c8` — the missing piece before any of this can run
natively.
