# SAV-01 payload reader caller bound at `0x543462` — E958

- Time: 2026-10-29 06:43 CDT (America/Chicago).
- Agent / integrator: `/root`; task `SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01`.
- Status: `STATIC_ANALYSIS_VERIFIED` within the one caller-path boundary; one verifier syntax correction; zero failed semantic hypotheses.

## Claim and rationale

The current task register has no rows marked `READY`. After E956's principal reassessment, a read-only readiness scout identified one narrow continuation from the newly published E955 result: test whether the call at `0x543462` in `0x542880` establishes that a full eight-byte value is available before calling `0x8eba0`.

E955 byte-verified the `0x8eba0` reader and all four direct call instructions plus captured caller contexts. It did not trace the predecessor guards or prove caller-side payload bounds. The existing `SAV-01-LOADER-01` result is partial and leaves unresolved edges; this task addresses one concrete caller path only.

## Scope

Use the pinned ELF `local/re/eboot.elf` (SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`) and the saved Ghidra 12.1.4 project at `local/ghidra/persistent-20261020/SotC.gpr` in `-noanalysis -readOnly` mode. Resolve the call argument setup and only the predecessor control-flow/guard conditions needed to determine whether this concrete path establishes eight available bytes. Independently byte-check every cited target/block/callsite against the pinned ELF.

Do not generalize to every reader caller, malformed-input safety, ownership, runtime import resolution, a native lift, or real save/load behavior. The callsite's existence and primitive-level behavior from E955 are prerequisites, not proof of the caller-side length condition.

## Preconditions and claim

- Clean `main` at `a057a1a2ab228a38cff46ae341597d7513d0ede0`, fetched `origin/main` and `FETCH_HEAD` matched, and no existing row for this task ID.
- Prerequisites: `SAV-01-PAYLOAD-READ-PRIMITIVE-8EBA0-01`, `SAV-01-LOADER-01`, and `docs/REVERSE_ENGINEERING.md` Stage 2.
- Claim publication gates: configure/build passed; CTest 487/487; lift checkers 572 passed, 0 skipped, 0 failed; Trail check in sync with 10,779 deep-layer nodes pending; project index 43,073/current. Path and attribution audits are pending publication. Logs: `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01/claim-publication/attempt-01/`.
- Claim and publication gates: the task was claimed in E957. Configure/build passed; CTest 487/487; lift checkers 572 passed, 0 skipped, 0 failed; Trail check in sync with 10,779 deep-layer nodes pending; project index 43,073/current. Claim-publication path and attribution audits passed before push; exact logs are under `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01/claim-publication/attempt-01/`.

## Findings

The read-only query shows the call at `0x543462` is in `FUN_00542880`, body `0x542880..0x543b9a` (4,886 bytes, 949 instructions). The sequence is:

1. `0x543440` calls `0xa81180`; `0x543445` loads the returned word tag and `0x54344d` compares it with `0xd`.
2. `0x543450` branches to cleanup at `0x543484` when the tag differs. On fallthrough, the code prepares RDI=`[RSP+0x308]` (reader) and RSI=`[RSP+0x2b0]` (qword destination), then calls `0x8eba0` at `0x543462`.
3. Only after `0x8eba0` returns, `0x543467` compares the qword output with RBX; `0x54346f` branches to cleanup on mismatch.

There is no caller-side payload-length comparison before this qword read. The direct caller gate is the tag check. The later qword comparison cannot prove eight bytes were available beforehand. E952/E955 establish that `0xa81180` initializes a payload view and that `0x8eba0`'s short-read path wraps without a status result. This remains a static caller-path observation, not proof of physical over-read, runtime behavior, or save/load.

## Verification

- Ghidra 12.1.4 reopened the saved project with `-noanalysis -readOnly`; exact command/script and final log are under `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01/attempt-01/`.
- `python3 -I local/validation/orch-20261029/SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01/attempt-01/verify_caller_bound.py` exited 0. It matched all 949 instruction byte sequences / 4,886 body bytes and all decoded direct call/jump edges against the pinned ELF, including the tag branch, reader call, and post-read comparison.
- A first verifier run had an apostrophe quoting syntax error in its final output string; after correction, replay passed. One tooling correction, zero failed semantic hypotheses.
- Private checksum replay `shasum -a 256 -c local/validation/orch-20261029/SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01/attempt-01/SHA256SUMS` passed all six files.

## Limits

This does not establish the runtime import binding, physical backing allocation, whether malformed files are exercised, behavior of other reader callers, native lift/wiring, authentic save/reload, or any G1–G6 gate.

## Publication

The E958 result was published in owner-authored commit `aa41cf9bc0bf68b224c82328ffc1bd138b7b8f52` (`docs: characterize save reader caller bounds`), changing five allowlisted docs. Configure/build passed; CTest 487/487; lift checkers 572 passed, 0 skipped, 0 failed; Trail graph was in sync with 10,779 deep-layer nodes pending; project index wrote and checked 43,075 folders current. Staged and explicit-path audits passed 5/5; all-path audits passed 3,366/3,366; identity, attribution, and diff checks passed. Push/fetch verified `HEAD == origin/main == FETCH_HEAD`; the worktree was clean. Exact logs are under `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01/result-publication/attempt-01/`.

For the E959 publication closeout record, configure/build passed; CTest 487/487; lift checkers 572/0/0; Trail graph remained in sync with 10,779 deep-layer nodes pending; project index wrote and checked 43,076 folders current. Exact repeat-run logs are under `local/validation/orch-20261029/SAV-01-PAYLOAD-READ-CALLER-BOUND-543462-01/result-publication/attempt-02/`.
