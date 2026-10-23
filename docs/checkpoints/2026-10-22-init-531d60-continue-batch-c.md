# Checkpoint — Init `0x531d60` continuation batch C (38 units, 153 B)

- Local: 2026-10-23 00:15 CDT (America/Chicago)
- Agent: orchestrator round-4 worker subagent (non-Claude model); integrator completed the
  verification and wiring replay after the worker stalled before its own build.
- Task: `CPU-01-INIT-531D60-CONTINUE-BATCH-03`
- Gate: **G1 PARTIAL — control-flow pin only, not PASSED**
- Evidence: E526

## What was lifted

`native/core/init_531d60_post_base_continue_c.{hpp,cpp}` models the **merge block**
`0x532904..0x53299d` (153 B, **38 consecutive units**) reached unconditionally by the batch-B
fall-through (`0x5327b1 e9 4e 01 00 00` = `jmp 0x532904`) and also by the batch-B `je` target
`0x5328e0` (a 36 B straight-line run ending in `sete r14b` that falls into `0x532904`).

Structure: three subsystem dispatches sharing vtable source `0x2efe318`, with `0xa52b70`,
`0xa599e0`, `0xa431e0` as the setters and `0x2efe340+0x60` as the flag source; the block ends at
`je 0x5329ae` (outcome not modelled).

Wiring: slice `.hpp/.cpp`, test hooks/assertions/telemetry, one `native/CMakeLists.txt` line.

## OBS (integrator-replayed from `local/re/eboot.elf`)

Region sha256 `2390d483b2e1bc47fd6657dae2bda4290b4f0d5f3bf986c7c27f795164560aa1`; 153 B,
`0x532904 + 153 = 0x53299d`.

| Call sites | Target |
| --- | --- |
| `0x53290b` | `0xa52b70` |
| `0x532946` | `0xa599e0` |
| `0x53297a` | `0xa431e0` |

| RIP-relative lea | Target |
| --- | --- |
| `0x532910` | `0x32f8b80` |
| `0x532917` | `0x2efe318` |
| `0x53294b` | `0x32f6d18` |

Branch `0x53299b je 0x5329ae` (rel8 target pinned, outcome unmodelled). The `0x5328e0` run's last
instruction is `41 0f 94 c6` (`sete r14b`) at `0x5328fa`, falling into `0x532904`
(`41 0f b6 f6`), which confirms the shared merge entry rather than a separate block.

## NOT modelled

Bodies of `0xa52b70`/`0xa599e0`/`0xa431e0`; the three indirect `call [rax]` vtable-slot targets;
`je` outcome and both successors (`0x53299d`, `0x5329ae`); incoming `r14b` and `r15` provenance;
the real contents of the `0x2efe318`/`0x2efe340`/`0x32f8b80`/`0x32f6d18` cells (host stand-ins
only).

## Verification (integrator replay)

```
cmake -S native -B local/build/orch-r4 -G Ninja   -> exit 0
cmake --build local/build/orch-r4                 -> exit 0
ctest --test-dir local/build/orch-r4              -> 100% tests passed out of 327
SOTC_NATIVE_BUILD=local/build/orch-r4 python3 -I tools/lift/check_f531d60_post_base_continue_c.py -> OK
python3 -I tools/lift/check_ctor_slots.py         -> 155 pairs
```

## Next exact action

Continue at `0x53299d` (fall-through) or `0x5329ae` (`je` target); keep the branch outcome
unmodelled until the `0x2efe340` flag source has a contract.
