# Checkpoint — Init `0x531d60` continuation batch E (53 units, 350 B)

- Local: 2026-10-23 03:05 CDT (America/Chicago)
- Agent: orchestrator round-9 worker subagent (non-Claude model); integrator replayed the build,
  checker and suite (and cleared a stale-object build artefact by rebuilding).
- Task: `CPU-01-INIT-531D60-CONTINUE-BATCH-05`
- Gate: **G1 PARTIAL — control-flow + descriptor setup; callees are contracts**
- Evidence: E541

## What was lifted

`native/core/init_531d60_post_base_continue_e.{hpp,cpp}` models the `je 0x5329ae` **taken** block
`0x5329ae..0x532b0c` — **350 B, 53 consecutive units**, region sha256
`2aee42e047ad6c993898621aa65c65673befa328d58639140a6e3999e3c10d91` — ending exactly where the
already-recovered registry fragment begins. The slice now runs this block when batch C's merge
branch is modelled and taken, instead of stopping.

## OBS (checker-verified)

- Three `call 0xa54920` at `0x532a07`, `0x532a62`, `0x532ae2` with distinct name-struct pointers
  `0x2efd9a0`, `0x2efd9f8`, `0x2efda50` and stack descriptors whose vtable differs for the third
  (`0x15c8d98` / `0x15c8d98` / `0x15c8e18`).
- Three `call [rax+0x18]` no-ops resolving through `0x15c8d98+0x18 → 0x538200` and
  `0x15c8e18+0x18 → 0x5388c0`, both bare `ret`s — **correction to the task brief**, which said
  `call [rax+0x08]`: the encoding is slot 3 (`+0x18`).
- One `call [rax+0x08]` at `0x532b09` through `*(0x2eff450)`, resolved by E538 to `0x54b250`,
  called as `(this, 0, 2)`.
- Boss-flag stamp `mov [rax],r12b` at `0x532a04` into `0x2dcdf14`.
- Nine RIP-relative lea targets and one `vmovq` from `0x15ff528`.
- Descriptors: 0x50 B, `vmovaps`-zeroed through `+0x20`, self-pointer at `+0x20`, vtable at `+0x40`;
  the third additionally sets self-pointer at `+0x00`, `0x5388e0` at `+0x18`, `0x538870` at
  `+0x48`.

## NOT modelled

Bodies of `0xa54920` and `0x54b250` (visible hooks/contracts), the two `ret` no-ops' bodies,
`0x32f79a8`'s value, `r13`/`r12b` provenance, descriptor field semantics beyond the pinned
addresses, and any save-data side effect. Per E540, `0xa54920` is a statistics/ConVar counter
registrar, **not** a save-data transaction, so this block does not touch the PS4 save layer.

## Verification (integrator replay)

```
cmake -S native -B local/build/orch-r10 -G Ninja   -> exit 0
cmake --build local/build/orch-r10                 -> exit 0
ctest --test-dir local/build/orch-r10              -> 100% tests passed out of 331
python3 -I tools/lift/check_f531d60_post_base_continue_e.py -> OK (350 B, 53 units)
python3 -I tools/lift/check_ctor_slots.py          -> 155 pairs
```

## Next exact action

Lift `0x54b250` (119 B stat query, engine `0xa56560`) natively — the analysis in E540 recommends
implementing it rather than contracting it — and then produce a coverage report over the whole
`0x531d60` body to prove how much of the function is modelled rather than asserting it.
