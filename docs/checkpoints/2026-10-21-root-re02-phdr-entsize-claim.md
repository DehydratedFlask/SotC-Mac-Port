# Claim: RE-02-DYNLINK-PHDR-ENTRY-SIZE-01

- Local time: 2026-10-21 10:09 CDT (America/Chicago).
- Agent / integrator: `/root` (Codex desktop task `01a15eab-88d5-7a83-a23b-cc55dc002eb6`); worker after claim publication: `slot23_raw_xrefs`.
- Task status / attempts: ACTIVE; 0 failed implementation tries.
- Bounded scope: validate that ELF64 `e_phentsize` is exactly 56 before decoding its program-header table, and add a synthetic test that mutates the field and expects a clear `ValueError`. Keep edits to `scripts/ps4_dynlink.py` and `scripts/test_ps4_dynlink.py`. No live ELF, loader, executable, or runtime behavior claim.
- Prerequisite: `RE-02-DYNLINK-PHDR-MACHINE-01` is FIXTURE_VERIFIED.

## Claim-time evidence

- Graft command `trail ask "How does ps4_dynlink parse ELF64 program headers, and does it validate e_phentsize?" --source --in scripts/` refreshed one indexed file. It located `program_headers` at `scripts/ps4_dynlink.py:46-52`: the parser unpacks an ELF64 header, checks class/data/version and machine, then decodes each record with a fixed 56-byte stride. It does not validate the header's `e_phentsize` field (tuple index 9).
- Graft command `trail callers program_headers --depth all --in scripts/` found the call chain `program_headers` ← `parse` ← `build` ← `main`; no other indexed caller.
- Live test source has a synthetic ELF64 header helper at `scripts/test_ps4_dynlink.py:30-34` that encodes `e_phentsize=56`, an accepted-header test at lines 279-285, and wrong-class/data/machine/truncation tests immediately following. No entry-size case exists.
- Checkout at claim: `main`, HEAD `1260c96ca2153d01b890a25d68e28f68f0834123`, matching `origin/main`. `git status --short` shows only pre-existing escalated `native/api/CMakeLists.txt` and untracked `native/api/host/`; these are excluded from this task and any commit.

## Acceptance and boundaries

- Test-first evidence: add a synthetic mutation of the `e_phentsize` field and run the focused test to show the new assertion fails on current code; then add only the fixed-format entry-size guard and rerun the focused suite.
- Acceptance command: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`.
- Observation: parser indexing and stride both represent ELF64 program headers as 56-byte records. Inference: rejecting a different advertised record size is necessary before the parser applies that fixed stride.
- Exact commands/results so far: no acceptance test has been run for this task; baseline is NOT RUN. No logs or code changes exist yet. `python3 scripts/project_index.py` exited 0 and reported 14,203 folders indexed; `trail build --check` exited 0 and reported the wiring graph in sync, with the deep layer not built and 3,517 meaning-tier nodes pending.
- Remaining risks: synthetic test proves a local parser contract only; it does not validate a real ELF, dynamic loader, or game behavior. Keep any failed attempt and private logs in `local/validation/re02-dynlink-phdr-entsize/`.
- Files changed to claim: `HANDOFF.md`, `docs/TASKS.md`, generated `docs/FOLDERS.md` (checkpoint count incremented 228→229), and this claim checkpoint. No parser/test source changed.
- Publication gates: `python3 -I scripts/audit_publish_paths.py` exited 0 for four staged allowlisted paths; `python3 -I scripts/audit_publish_paths.py --all` exited 0 for 1,495 allowed tracked paths; `git diff --cached --check` exited 0. Only the four claim paths are staged; pre-existing escalated host-protocol paths remain unstaged and excluded.

## Claim publication and assignment checkpoint

- Local date/time: 2026-10-21 10:10 CDT (America/Chicago). Integrator: `/root`; worker: `slot23_raw_xrefs`.
- Task: `RE-02-DYNLINK-PHDR-ENTRY-SIZE-01`; files changed since claim: only `HANDOFF.md` and this checkpoint. Worker code edits have not been integrated or reviewed yet.
- Exact command: `git push origin main`; exit 0, advanced `1260c96..bad72a6`. The claim is now public.
- Observation: the worker owns only `scripts/ps4_dynlink.py` and `scripts/test_ps4_dynlink.py`; their work is test-first and logged under `local/validation/re02-dynlink-phdr-entsize/`. No test has yet been run for this task.
- Remaining risks: no parser guard/test result exists yet; all live ELF and loader behavior remains unverified.
- Next exact action: wait for the worker's red/green focused-suite logs, review the two-file diff and hashes, and independently replay the acceptance command. Task remains ACTIVE; 0 failed implementation tries.
