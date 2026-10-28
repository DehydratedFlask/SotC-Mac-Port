# E880 — E879 publication closeout

**Date:** 2026-10-28, CDT
**Task:** `PUB-01-E879-PUBLISH-CLOSEOUT-01`
**Status:** `ARTIFACT_VERIFIED`

## Published commit

E879 commit `20b6323f46a0f3add7a2fe32e64c4b964d47f752` (`docs: record E879 slot-23 evidence
limits`) was authored and committed by Johnny Wang. The exact-range attribution audit passed
before push; the commit contains no Claude/Anthropic credit line. `git push origin main`
completed successfully. Fetch, `git ls-remote`, local `HEAD`, `origin/main`, and `FETCH_HEAD`
all agree on `20b6323f46a0f3add7a2fe32e64c4b964d47f752`.

GitHub's contributors endpoint currently lists only `DehydratedFlask`. The latest full
advertised-ref census remains the E872 measurement at 2,485 commits; every subsequent commit
through E879 was separately owner-authored and exact-range checked. Private `refs/stash` remains
at `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`, unchanged per the owner's direction.

## Work and limits closed out

E879 recorded a two-disassembler, instruction-level register-flow observation for the slot-23
consumer. Caller reachability, vtable target identities/semantics, and string necessity remain
unknown; the native slot remains unregistered and G1 stays `PARTIAL`. The CXML parser invocation
was recorded as an out-of-scope scout action: it produced no structure counts, and the task's
UNKNOWN state remains unchanged. No code, product behavior, or game gate advanced.

E879 validation passed: CMake configure/build exit 0 (Ninja had no work), CTest 431/431, lift
checkers 516/0/0, full path audit 3,095/3,095, project index 32,025/current, Trail graph check
current at 9,877 nodes (deep layer not built), staged path audit 6/6, and exact-range attribution
exit 0. Detailed logs are private under `local/validation/orch-20261028/e879/`.

## Current stopping point

The read-only CPU/audio/save scouts found no existing `READY` product task. The save scout's
one-field GameData-writer idea is not yet a task card; the usable saved Ghidra project pointer
currently fails REA validation because it is zero bytes. `APP-01-VOICEOVER-READINESS-02` remains
`NEEDS_PRINCIPAL`, G4 remains `PARTIAL`, and boot/audio/real-save acceptance remain unverified.

Next exact action: select a bounded product analysis only when its pinned artifact and analysis
prerequisites are available; otherwise keep the established CPU partial result and do not
register slot 23 or reopen the stopped CXML parser probe.

## E880 validation and publication

E880 closeout validation from the workspace root:

- `cmake -S native -B local/build/orch-r22` — exit 0.
- `cmake --build local/build/orch-r22 -j 12` — exit 0, `ninja: no work to do`.
- `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` — exit 0, 431/431.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` — exit 0, 516 passed, 0 skipped, 0 failed.
- `python3 -I scripts/audit_publish_paths.py --all` — exit 0, 3,096/3,096.
- `python3 -I scripts/check_commit_attribution.py` before the E880 commit — exit 0.
- `trail build --check` — exit 0, wiring graph current at 9,877 nodes; deep meaning layer not built.
- `python3 scripts/project_index.py` — exit 0, 32,026 folders indexed; `python3 scripts/project_index.py --check` — exit 0, current.

Logs: `local/validation/orch-20261028/e880/`. The staged path audit, final cached diff check,
E880 exact-range attribution, commit, push, and live-ref verification are completed by the
following publication checkpoint.
