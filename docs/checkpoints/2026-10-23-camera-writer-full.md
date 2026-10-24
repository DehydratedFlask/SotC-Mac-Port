# Checkpoint — full camera delegate-writer census

- Local: 2026-10-23 (America/Chicago)
- Agent: static-analysis worker
- Task: `CPU-01-CAMERA-DELEGATE-WRITER-FULL-01`
- Gate: **STATIC_ANALYSIS_VERIFIED** (negative result)

## Verdict

No writer of the 24-byte block at `D+0x2e8` exists in the shipped ELF, and for the `_Root` class
previously called `D` the field is **not a delegate**: `0x1d72d0` copies two 0x20-byte records from
`+0x2e8`/`+0x308` and rebases the copied head to `&dst`
(`lea rax,[rdi+0x2e8]; mov [rax],rax`) — intrusive-list node semantics.

## Coverage

- Whole RX `0x0..0x1593978`: 22,624,632 B / 6,524,277 instructions / 300,290 invalid bytes (1.33%).
- Window A `0x1c0000..0x1da000`: 106,496 B, 29,041 instructions, 0 invalid bytes.
- Window B `0x20c000..0x216800`: 43,008 B, 12,464 instructions, 0 invalid bytes.
- 464 stores overlap `0x2e8..0x307`; inside the camera window only the ctor zero (`0x1d6386`) and
  the three list-rebase stores in `0x1d72d0`.
- All 45 qword stores into `+0x2f0`/`+0x310` were inspected: 0 with a non-zero immediate, 0 from a
  `lea` of a function. No `__pfn` is ever written.
- 1,374 instructions materialise an offset in `[0x2a8,0x308)`; none is passed as a call argument.
- `0x216480` is never address-taken (3 direct call sites only).

## Consequence

The `_Root` ctor sets flags `0x40|0x200`, and the `0x2165d0` gate requires exactly those bits, so a
`_Root` object passes the gate and would `call 0`. Either the `0x1c113a` pose path is debug/snapshot
only, or `D` is not the 0x3c8 class. That class-identity question is unresolved.

## Limits and next action

- Residual blind spots: 1.33% undecodable bytes (none in either window); global-sweep desync
  (330/20,396 call targets), but the camera-region store sets agree across two decodes.
- rizin 0.9.1 desyncs at `0x1c5940`; its counts were not trusted. The harness uses its own decoder.
- **No native `BoundMemberFn` contract should be modelled** until `D`'s class is resolved: decode
  `0xa68030` (the handle-to-element resolver used at `0x1c0ba6`) and find the single store to
  `[element+0x18]`, then re-run the `+0x2e8` writer census for that class.
