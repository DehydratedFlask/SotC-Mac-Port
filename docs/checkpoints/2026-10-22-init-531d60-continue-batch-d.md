# Checkpoint — Init `0x531d60` continuation batch D (17 B fall-through)

- Local: 2026-10-23 02:05 CDT (America/Chicago)
- Agent: orchestrator (integrator)
- Task: `CPU-01-INIT-531D60-CONTINUE-BATCH-04`
- Gate: **G1 PARTIAL — control-flow pin only**
- Evidence: E535

## What was lifted

`native/core/init_531d60_post_base_continue_d.{hpp,cpp}` models the fall-through of batch C's
`je 0x5329ae`: `0x53299d..0x5329ae`, **17 B / 4 units**, region sha256
`6f4e5fa8921e53532c8ad4319ba268bd181d3874e83e4f140a4873bfccf3cb2d`.

| Unit | RVA | Size | Mnemonic |
| --- | --- | --- | --- |
| 1 | `0x53299d` | 7 | `lea rax,[0x32f79a8]` |
| 2 | `0x5329a4` | 2 | `xor esi,esi` |
| 3 | `0x5329a6` | 3 | `mov rdi,[rax]` |
| 4 | `0x5329a9` | 5 | `call 0xa473b0` |

Arithmetic note: the earlier batch-C report called this a 13 B block; `0x5329ae - 0x53299d` is
`0x11` = 17 B, and four instructions exactly fill it.

Wiring: slice result + hooks parameter, run after batch C; test asserts 4 units, one cell load,
one call receiving the loaded pointer, and `end_rva == 0x5329ae`; checker
`tools/lift/check_f531d60_post_base_continue_d.py` pins the region sha, the lea/call targets, the
`xor`/`mov` encodings, and that batch C's end matches this entry.

## NOT modelled

Body of `0xa473b0`; the value held by `0x32f79a8`; the `0x5329ae` branch-target block; the merge
`je` outcome; incoming registers.

## Verification

```
cmake -S native -B local/build/orch-r8 -G Ninja   -> exit 0
cmake --build local/build/orch-r8                 -> exit 0
ctest --test-dir local/build/orch-r8              -> 100% tests passed out of 330
python3 -I tools/lift/check_f531d60_post_base_continue_d.py -> OK
```

## Next exact action

Lift the `0x5329ae` branch-target block (analysis in flight) or model the merge `je` outcome from
the same `[0x2efe340]+0x60` flag that E530 identified.
