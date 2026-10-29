# SAV-01 payload reader caller bound at `0x543462` — E957 claim

- Time: 2026-10-29 06:43 CDT (America/Chicago).
- Agent / integrator: `/root`; task `SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01`.
- Status: `ACTIVE`; analysis attempts 0; failed semantic hypotheses 0.

## Claim and rationale

The current task register has no rows marked `READY`. After E956's principal reassessment, a read-only readiness scout identified one narrow continuation from the newly published E955 result: test whether the call at `0x543462` in `0x542880` establishes that a full eight-byte value is available before calling `0x8eba0`.

E955 byte-verified the `0x8eba0` reader and all four direct call instructions plus captured caller contexts. It did not trace the predecessor guards or prove caller-side payload bounds. The existing `SAV-01-LOADER-01` result is partial and leaves unresolved edges; this task addresses one concrete caller path only.

## Scope

Use the pinned ELF `local/re/eboot.elf` (SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`) and the saved Ghidra 12.1.4 project at `local/ghidra/persistent-20261020/SotC.gpr` in `-noanalysis -readOnly` mode. Resolve the call argument setup and only the predecessor control-flow/guard conditions needed to determine whether this concrete path establishes eight available bytes. Independently byte-check every cited target/block/callsite against the pinned ELF.

Do not generalize to every reader caller, malformed-input safety, ownership, runtime import resolution, a native lift, or real save/load behavior. The callsite's existence and primitive-level behavior from E955 are prerequisites, not proof of the caller-side length condition.

## Preconditions and next action

- Clean `main` at `a057a1a2ab228a38cff46ae341597d7513d0ede0`, fetched `origin/main` and `FETCH_HEAD` matched, and no existing row for this task ID.
- Prerequisites: `SAV-01-PAYLOAD-READ-PRIMITIVE-8EBA0-01`, `SAV-01-LOADER-01`, and `docs/REVERSE_ENGINEERING.md` Stage 2.
- Claim publication gates: configure/build passed; CTest 487/487; lift checkers 572 passed, 0 skipped, 0 failed; Trail check in sync with 10,779 deep-layer nodes pending; project index 43,073/current. Path and attribution audits are pending publication. Logs: `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01/claim-publication/attempt-01/`.
- Next exact action: run a bounded saved-project Ghidra query for `0x543462`, `0x542880`, its controlling predecessor blocks and relevant argument definitions; then implement an independent byte verifier before drawing a conclusion.

No analysis finding or runtime claim is made in this claim checkpoint.
