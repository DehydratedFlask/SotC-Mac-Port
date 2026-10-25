# Checkpoint — `0xa473b0` lifted with explicit capture-service contracts

- Local: 2026-10-23 02:45 CDT (America/Chicago)
- Agent: orchestrator round-8 worker subagent (non-Claude model); integrator replayed build,
  checker and the 331-test suite.
- Task: `CPU-01-CAPTURE-A473B0-NATIVE-01`
- Gate: **G1 PARTIAL — services are contracts, nothing ran against the game**
- Evidence: E539

## What was built

`native/core/capture_a473b0.{hpp,cpp}` + `native/tests/test_capture_a473b0.cpp` (331st test) +
`tools/lift/check_fa473b0.py`. Init batch D now executes this body when a `CaptureServices`
implementation is supplied; the hook remains the fallback.

## OBS (integrator replay)

Body `0xa473b0..0xa47436` = **134 B**, sha256 `e755e3f05375…`; guard load
`0xa473bb 4c 8b 35 56 71 bb 00` → cell `0x15fe518` (`__stack_chk_guard`).

| Call site | Thunk | NID | Contract method |
| --- | --- | --- | --- |
| `0xa473cf` | `0xf91380` | `dWM80AX39o4` | live-streaming enable(bool) |
| `0xa473d8` | `0xf913a0` | `2xxUtuC-RzE` | screen-shot enable (enable path) |
| `0xa473e6` | `0xf91390` | `tIYf0W5VTi8` | screen-shot disable (disable path) |
| `0xa47400` | `0xf913b0` | `Fc8qxlKINYQ` | video-recording set-info(`0x0d`, &state, 4) |
| `0xa4741a` | `0xf913b0` | `Fc8qxlKINYQ` | video-recording set-info(`0xa007`, &state=0, 4) |
| `0xa47431` | `0xf8f3a0` | `Ou3iL1abvng` | `__stack_chk_fail` (canary mismatch only) |

Init reaches it as `mov rdi,[0x32f79a8]; xor esi,esi; call 0xa473b0`, i.e. **enable = false**,
so the observed order on this path is live-streaming(false) → screen-shot disable →
set-info(`0x0d`, `&1`, 4) → set-info(`0xa007`, `&0`, 4).

## Service contract

`CaptureServices` exposes one method per import, each returning
`{ ok | disabled | unsupported }`; the lift records every dispatch, and `ok` requires all four
sites in order with no `unsupported`. The default `UnsupportedCaptureServices` makes a missing
platform service visible (`any_unsupported`, `ok=false`) instead of silently succeeding. No Sony
module name is a runtime dependency; NIDs are documentation only. `__stack_chk_fail` becomes a
`stack_chk_mismatch` flag, never an abort.

## NOT modelled

The Sony service bodies; the unused `this` argument (never read by the original); the host
guard-cell lookup when absent; the meaning of set-info ids `0x0d`/`0xa007`; the abort path.

## Verification

```
cmake -S native -B local/build/orch-r9 -G Ninja      -> exit 0
cmake --build local/build/orch-r9                    -> exit 0
ctest --test-dir local/build/orch-r9                 -> 100% tests passed out of 331
python3 -I tools/lift/check_fa473b0.py               -> OK (134 B, six rel32, guard, state)
python3 -I tools/lift/check_f531d60_post_base_continue_d.py -> OK
python3 -I tools/lift/check_ctor_slots.py            -> 155 pairs
```

## Correction for the next task

The worker's "unresolved `call [rax+0x8]` at `0x532b09`" is now resolved by E538: the vtable is
`0x15c9830` and slot 1 is `0x54b250`, so the 350 B block's indirect call is known.

## Next exact action

Lift the `0x5329ae..0x532b0c` block (350 B) with `0xa54920` behind a contract/hook and the three
provably-`ret` vcalls elided; the `0x532b09` target is `0x54b250`.
