# Checkpoint — Init `0x531d60` continuation batch B (14 units, 50 B)

- Local: 2026-10-23 ~00:20 CDT (America/Chicago)
- Agent: orchestrator round-3 worker subagent (non-Claude model) + integrator replay
- Task: `CPU-01-INIT-531D60-CONTINUE-BATCH-02` — attempt 1, success
- Gate: **G1 PARTIAL — control-flow pin only, not PASSED**
- Evidence: E520

## What was lifted

`native/core/init_531d60_post_base_continue_b.{hpp,cpp}` models the **fall-through block** after
batch A: `0x53277f..0x5327b1` (50 B, **14 consecutive units**), ending at the conditional
boundary `je 0x5328e0` (outcome not modelled).

Wiring: `native/core/appmain_init_ctor_98a90_slice.{hpp,cpp}` (result + hooks, run after
`post_base_continue_a`), assertions/telemetry in
`native/tests/test_appmain_init_ctor_98a90_slice.cpp`, one line in `native/CMakeLists.txt`,
checker `tools/lift/check_f531d60_post_base_continue_b.py`.

## OBS (verified by the integrator from `local/re/eboot.elf`)

Region sha256 `409d74e274de0f6c73c799f325a084a2c6ac742896802657c734b6972c1cabd4`; 14 units
contiguous `0x53277f + Σsize = 0x5327b1` (no gaps/overlaps).

| Call sites | Target |
| --- | --- |
| `0x53278e` | `0x1e4c0` |
| `0x5327a0` | `0x4c4950` |

Branch: `0x5327ab..0x5327b1 je 0x5328e0` (rel32 near-jump target pinned, `branch_outcome_modelled
= false`). Cell read `0x5327a5 mov rdi,[r13+0]` where `r13 = 0x32f84f8` was pinned by batch A.
Next instruction at `0x5327b1` is `e9 4e 01 00 00` (`jmp 0x532904`).

## NOT modelled

Bodies of `0x1e4c0` and `0x4c4950`; the `je` outcome and both successors (`0x5327b1`
fall-through, `0x5328e0` branch target); incoming `rax` (return of `0x1fc80` from batch A);
`r13` provenance beyond the batch-A pin.

## Verification (integrator replay)

```
cmake --build local/build/orch-final      -> exit 0 (331 targets)
ctest --test-dir local/build/orch-final   -> 100% tests passed out of 326
SOTC_NATIVE_BUILD=local/build/orch-final python3 -I tools/lift/check_f531d60_post_base_continue_b.py -> OK
```

Independent byte replay: region sha matches, 14 units contiguous to `0x5327b1`, both call
targets, the `je` target, and the `jmp` at the boundary all match. `check_ctor_slots.py` still
**155** pairs.

## Next exact action

Continue at the batch-B boundary. Two ownership questions must be settled before choosing:
`0x5327b1` is an unconditional `jmp 0x532904` on the fall-through path, while `0x5328e0` is the
`je` target. Prefer modelling the `0x532904` merge block next, since it is reached from the
fall-through regardless of the branch outcome; keep the branch outcome unmodelled.
