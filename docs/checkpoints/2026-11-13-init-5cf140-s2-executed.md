# 2026-11-13 — Init `0x5cf140` S2 executed: "Shadow Credits" runs natively

**State: green.** Continuation of commit `44cef29a` on branch
`cursor/init-hash-551140-6aa93e8`. Resumed at the recorded `0x5bc3d4` (S4), measured that S4's
two callees have no native models, and instead used the improved slice tracer to execute S2, the
`0x5cf140` "Shadow Credits" page builder.

## What executes now

`0x5cf140..0x5d06d6` (5,526 B, 932 instructions, 132 direct calls, single `ret` at `0x5d06d6`, no
backward branch), in body order, through the same proven models as S3:

| # | item | ctor | detail |
|---|---|---|---|
| 1 | group "Shadow Credits" under "Shadow" | `0x26c10` | site `0x5cf1c6` |
| 2 | command "Jump to credits level" | `0x26f90` | "BP_JumpToCredits" -> `0x2dfcbd0`, site `0x5cf23a` |
| 3 | separator | `0x26b60` | text through `0xf1a0`, host allocator, site `0x5cf261` |
| 4 | named variable (external) | `0x26db0` | `cp10_creditsUseNewList`, cell `0x2ff59c0`, literal `0x1268590` (built by unlifted `0x5d41c0`) |
| 5 | bool var "Apply adjustments" | `0x20af0` + `0x20d60` + atexit `0xacf0` | cell `0x2ff5a90`, guard `0x2ff5b50` |
| 6 | bool var "Credit Pause" | `0x20af0` + `0x20d60` + atexit `0xacf0` | cell `0x2ff5b58`, guard `0x2ff5c18` |
| 7-8 | separators | `0x26b60` | sites `0x5cf57b`, `0x5cf592` |
| 9-21 | ten bounded floats + `0x27120` sliders | `0xc300` | functor `0x15cb238`, atexit `0xad30` |

Bounded variables (range, domain): Font Scale (0.01/0.1, 0..4), Scroller Speed (0.1/1, -20..20),
Scroller Width (0.01/0.1, -2..2), Scroller Offset (0.01/0.1, -2..2), Scroller Left Line Delay
(0.5/10, 0..1000), Scroller Right Line Delay (0.5/10, 0..10000), Fade top offset (0.01/0.1, -1..1),
Fade top height (0.01/0.1, 0..1), Fade bottom offset (0.01/0.1, -1..1), Fade bottom height
(0.01/0.1, 0..1). Separators at `0x5cf75c`, `0x5cffbf`, `0x5d030b`.

Totals: **21 items, 21 menu registrations, 12 ConCmd registrations, 28 releases, 21 destroys,
17 atexit records (root + 2 + 10 + 4 S3)**.

## Coverage

- `0x5b9f90` body extent executed: **9,279 of 46,881 B (19.8 %)** = S0 177 + S2 call site 5 +
  S3 9,097.
- Native code executed by the same run including the callee body: **14,805 B (31.6 %)**.
- Eight executed slice records; one contract left (S1 `0x2170`).
- Resume offset unchanged: **`0x5bc3d4`**. S4's `0x5dbf40` (7,533 B, first `ret` `0x5ddcad`) and
  `0x7f8930` (90 B, calls `0x7f8610`, touches `0x15fe570`) remain unmodelled.

## Verification

- `tools/lift/check_init_5b9f90_slices.py`: **OK**, re-deriving the S2 extent (932 insns, one `ret`,
  no backward branch, 132 calls) and all 21 tuples; `executed=9279B items=53 executed_total=14805B`.
- `python3 -I tools/lift/slice_5b9f90.py` regenerates `docs/5B9F90_SLICES.md` +
  `docs/5b9f90_slices.json` with S2 marked `executed` and its 21-row table.
- Full build **0**; full CTest **704/704**; init runtime exit **0**, `ok=true`,
  `post_call_count=25`, `post_calls_executed=24`, `post_calls_blocked=1`,
  `post_calls_complete=false`, boss `0xA7`.
- Checker sweep **914 passed / 0 skipped / 0 failed**, exit 0.

## Not claimed

Cold boot, audio, save/reload and Metal validation stay **NOT RUN**. `post_calls_complete` stays
false: S1 `0x2170` is still a contract (extent ≥ 28,288 B) and S4..S9 are still blocked/contract.
G1 **PARTIAL**.
