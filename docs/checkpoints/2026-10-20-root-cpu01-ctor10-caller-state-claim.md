# CPU-01-CTOR10-CALLER-STATE-01 — slot-10 caller state for `0xe1ede0`

- Date/time: 2026-10-20 00:50 CDT (America/Chicago).
- Integrator/owner: `/root`; bounded analysis dispatch pending.
- Task ID: `CPU-01-CTOR10-CALLER-STATE-01`; parent CPU-01.
- Prerequisites: E43 / `CPU-01-CTOR10-ANALYSIS-01`, E46 / `CPU-01-CTOR10-INIT-HELPER-01`, and E52 / `CPU-01-CTOR10-E1EDE0-01`.
- Status: CLAIMED; no analysis hypothesis attempted yet.
- Failed hypotheses/tries: 0.

## Bounded scope

Characterize only the caller-local state that the slot-10 function at `0xf09e60` passes to
`FUN_00e1ede0` at callsite `0xf09f04`. E52 verifies `RDI = caller RSP+8` on the observed byte path.
The callee's two call guards inspect qwords `[RDI+0x30]` and `[RDI+0x38]` and bytes `[RDI+0x41]`
and `[RDI+0x42]`; these correspond to caller stack offsets `RSP+0x38`, `RSP+0x40`, `RSP+0x49`,
and `RSP+0x4a` respectively. Trace only writes, reads, and verified helper effects that can affect
those four values before the direct call. If a called helper could modify one of them and its effect
is not byte-verified, leave that value unknown. Do not infer a C++ type or lifecycle role from
offsets, neighboring names, or the helper's apparent cleanup behavior.

Inputs are the original read-only pinned ELF SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, existing E43/E46/E52 private
records, and raw PT_LOAD bytes. Reuse existing REA/Ghidra evidence; do not overwrite any baseline or
prior snapshots. Only request a distinct REA analysis if the existing record lacks the needed caller
body, and save any new analysis under `local/re/`.

## Acceptance and limits

- Provide exact caller bytes/ranges and byte-backed register/stack dataflow for each guard field.
- Independently bind each reported range and value to the pinned ELF; retain full command, exit,
  stdout/stderr, hashes, and a repeatable private verifier/report under `local/re/evidence/`.
- Distinguish unknown helper effects from observed stores; never claim the two computed calls are
  unreachable unless every relevant pre-call value and path is established.
- No ELF/game execution, installation, download, module semantics, native implementation, constructor
  reachability, service/ABI claim, or G1 promotion. Root owns shared docs/publication.

## Claim checkpoint

Claim commit includes `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md`, and this file.
Exact next action: publish the claim, then dispatch the bounded static analysis to a GPT-6 Luna
subagent at Extra High reasoning; root reviews the private report and independently reruns its verifier.
Failed hypotheses/tries: 0.

## Dispatch checkpoint

- Date/time: 2026-10-20 00:51 CDT (America/Chicago); integrator `/root`.
- Task ID: `CPU-01-CTOR10-CALLER-STATE-01`; analyst `/root/ctor10_caller_state` (`gpt-6-luna`, Extra High).
- Claim publication: `python3 scripts/project_index.py` exit 0 (4,133 folders; checkpoint directory count 34); `graft check` exit 0 (wiring graph synchronized; optional deep layer 0%, 1,219 nodes pending); `git diff --check` and staged check passed. Staged audit accepted 4 paths; full audit accepted 589; fetch succeeded and pre-commit divergence was `0 0`. Commit `d9d4d70` (`docs: claim slot-10 caller state analysis`) pushed (`411a45b..d9d4d70`). Post-push tree clean, `HEAD` and `origin/main` both `d9d4d701ff3bb0c9804aba968e5bba6d9b2fb7fe`, divergence `0 0`.
- Dispatch: after the claim was pushed, root sent the bounded request to `/root/ctor10_caller_state`, explicitly permitting only private analysis files under `local/re/`; no shared-doc/source/reference changes allowed.
- Result: dispatch accepted; analysis is in progress. No field value, branch result, or runtime behavior is inferred from dispatch.
- Remaining risks: caller stack values can be affected by earlier calls; any unverified side effect must stay unknown. Constructor reachability and G1 remain unproven.
- Exact next action: inspect the private static report and verifier when returned; independently rerun the checker, then integrate shared evidence and publication.
- Failed analysis hypotheses/tries: 0.
