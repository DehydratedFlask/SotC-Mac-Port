# User-directed stopping checkpoint

- Time/agent: 2026-10-20 21:32 CDT (America/Chicago), `/root`, sole integrator.
- User instruction: stop goal work and reach a good stopping point. All three workers stopped; no further implementation, provider query, verifier replay or integration is authorized by this checkpoint. Resume only after a new user request.
- Base: `5d4075ed8f6edbe0268c1613ea1d02235181aa08` (`main`, previously pushed).
- Changed public files: HANDOFF.md, docs/TASKS.md, docs/EVIDENCE.md, INSTALL_LOG.md, this checkpoint, regenerated docs/FOLDERS.md, and four checkpointed WIP checksum files listed below. CMake/README remain at their previous state; no checksum test is registered in CTest yet.
- Gate status: G0 PARTIAL; G1-G6 NOT RUN. No native game boot, game audio, or real progress save/reload is established. Prior 222-test fixture result predates this WIP; it does not cover these new files.

## SAV-01-NATIVE-CHECKSUM-01 — PAUSED, worker checks passed, root acceptance pending

Worker `/root/aud02_waitloop_resume_audit` stopped at a coherent boundary. Preserve these standalone, unintegrated source files:

| File | SHA-256 |
|---|---|
| native/core/save_checksum.hpp | ec646dfdb0598019d835b467ad9c2fa4243b5edf9d73a91edfbd3d87135013f4 |
| native/core/save_checksum.cpp | 5a7f1b49f3017ed27f3cedecdcd0399c098a313a2a5b981d7133126ed410446c |
| native/tests/test_save_checksum.cpp | c917366944158ad2ba1a63b7b04dd669a61f5c616e9f66017c0af6ec92da29b0 |
| tools/lift/check_f161f50.py | 7981c7d67ceed127e92894c648053181ddd39b7545786b090f63ddcd58ccbb6c |

Root read all four files for the stopping/publication boundary; root did not replay or accept implementation correctness. The helper is an ordinary explicit-seed byte-span FNV-1a-32 primitive. Original signed-negative-length behavior is outside this native span API; verified Save/Load lengths are nonnegative dwords. It is not a serializer. Source checker pins source identity/body digest and selected decoded helper/caller invariants; its 38-byte helper digest is `0e10cab6ef5646275365017d20e8baaf90f26bc0ac33edeb88cb97c1d105c981`.

Worker-reported commands from repo root (all completed before stop; no root replay at stopping):

```sh
python3 -I tools/lift/check_f161f50.py > local/build/save-checksum-worker/source-check.log 2>&1
clang++ -std=c++20 -arch arm64 -DNDEBUG -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -Wall -Wextra -Wpedantic -I native native/core/save_checksum.cpp native/tests/test_save_checksum.cpp -o local/build/save-checksum-worker/test_save_checksum > local/build/save-checksum-worker/build.log 2>&1
local/build/save-checksum-worker/test_save_checksum > local/build/save-checksum-worker/run.log 2>&1
```

Worker reports all exit0; root read retained run log `save_checksum: all checks passed`. Source-check log SHA `c3824579b5b0f0e1f13733f178e2d4d3b0a1c951a6019d8c39c0d600c41c8054`; build log empty; run log SHA `d6b85ec615f73c346769c1cb1d2b4855f6d274c1562c54f6766cbf331e89f286`. Binary-info log SHA `c5f9d18a49ef292b0a391001a693f8c07868ada971863e8582e6026d9a7d3d54`, reports Mach-O arm64. Worker reports one scratch prime mutant (`0x01000193` to `0x01000192`) compiled then exited1 at the `a` known answer, as expected; rejection log SHA `e2bada58233beb14fdce0597c65fbe947545e1e2e67431516310119bb9543be6`. Worker corrected an earlier misleading commentary: the failure belonged to the deliberate mutant, while production passed. Extra debug/rerun logs are retained in the manifest, not hidden. Failed implementation tries:0; deliberate mutant rejection is not a production failure.

Next exact action on resumption: review the source/checker and independent known answers, wire CMake/README, rebuild/requery Graft, run root ARM64 sanitizer/checker/appropriate CTest verification, then decide fixture acceptance. Do not imply compatibility with actual saves.

## CPU-01-CTOR10-OBJECT-INIT-01 — PAUSED, one success and one interrupted query

The one access-scoped retry of `0xb30950` completed: REA CLI6.7.0/Ghidra12.1.4, exit0, 370727ms, empty stderr. Evidence `ev_5244df9c0cdd09628007481ae7de7c3d4adf109c3e1e1956257103fe018e264e`; profile digest `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`. All artifacts are in `local/re/evidence/cpu01-ctor10-object-init-01/`:

- Exact command `rea-b30950-elevated.command.txt`, SHA `e519653a70158eb6732d1fd66c7e2df73fdf941a50c88ebcbf72a12f00eafa54`.
- JSON `rea-b30950-elevated.json`, 344911 bytes, SHA `7f83e09276c8fceae8e1ce31aed3f6247d9578093c9ec9896f30443e5e448aaf`.
- Exit record SHA `9a271f2a916b0b6ee6cecb2426f0b3206ef074578be55d9bc94f6f3fe3ab86aa`; working snapshot SHA `99f012787f5ec3cfe0543ef37a62d8c354d632b9e3349bc9f0b21c8faf4d2e5a`.
- Pristine snapshot remains `32f2c278247c85bdccaeb51f4e9d4bd8d7234eca00a78abc225b957f413af6ed`.

Worker reports 112-byte body `0xb30950..0xb309bf`, body digest `ec15142da417b882084a63c5b979c0c1130685c2621a126ea0491b835afe6e20`, with attr-init / settype2 / setprotocol1 / mutex-init / attr-destroy and a canary check. Mapping to recursive/inherit POSIX contracts uses the clean pinned shadPS4 source; Ghidra parameter types remain low confidence. Root has not independently accepted these semantic claims. Private report/verifier are not created. Native Darwin behavior/ownership remain open; slot10 is still unregistered.

The second distinct query `0xe44a10` had started at21:27:23 CDT. Worker sent Ctrl-C to its session83347 at21:29:19; terminal exit1, about1m56s, user interruption rather than provider failure. Preserve partial JSON (86794B), SHA `259f614d0f318c2e36210c5a4730f7bc215440aebd2afb9c6290162cdb50e4f5`; exact command `rea-e44a10-elevated.command.txt`, SHA `0843b6b42648977521a98e0cf49c31d727a819d4c106075d746d562201f77a70`; exit record SHA `560edb9ccc09356e89df77f79c0d65d012d9c9de16b0a13279fecf4de3432b9f`; stderr empty and snapshot remains pristine. Partial JSON is unaccepted and was not semantically interpreted. No automatic restart. Failed count remains1 infrastructure failure (original Swift-cache denial),0 rejected analysis hypotheses, plus1 user-cancelled attempt tracked separately; ancestor history remains intact.

Worker process check found no remaining task-owned REA/Ghidra process. Root independently ran `pgrep -alf 'cpu01-ctor10-object-init-01|sav01-loader-01|save-checksum-worker|ghidra.*[Hh]eadless|[a]nalyzeHeadless'` with read-only scoped elevation: exit1, no output/matches. No unrelated process was killed. Root process-check record: `local/checkpoints/root-20261020-pause-process-check.txt`.

Next exact action only on resumption: inspect partial callback capture for completeness, decide whether a clean callback query needs explicit authorization under the distinct-query limit, then construct/replay private evidence before choosing native ownership. Do not retry escalated `0xd71f60`.

## SAV-01-LOADER-01 — PAUSED, worker try3 passes; private report/root review pending

Two checker failures are now explicitly counted. Try1 read relationship coverage at the wrong object level; try2 corrected the reader but frozen claims still used that wrong level. Logs preserve commands/tracebacks and disclose reconstructed historical checker hashes because no attempt-time source snapshots existed. Logs SHA: try1 `f172fbb3667c06a2a93ff93cc08356a7c6eef8626ab2a2076bbd333989a7458d`; try2 `bd98ac31393f4d57e1fc6e505eacdd36f4be1a997b670aebb463ab7560a222ff`.

Try3 `python3 -B -I local/re/evidence/sav01-loader-01/verify_sav01_loader.py` was already running at stop and completed exit0. Worker reports ALL ASSERTIONS PASSED:4 body ranges,45 selected byte-mapped instruction rows,q1/q2 exact direct calls6/117,14 generic-reader callsites,5 bounded mutations rejected. These are unreviewed worker claims. Q2 retains unresolved targets `0x5435fc`/`0x543611` and3 LLVM-only rows. No runtime save/load result. Cumulative failed checker tries2,successful try3; no escalation threshold reached and no fourth run occurred.

Private identities: `claims.json` SHA `091e04348b829b9439efe176a2da5b70b09a3eabec5149e9a1b56387f19ea666`; `verify_sav01_loader.py` SHA `0d327aa76dd02d5a923729097d0c3f1e3b89ea51953e4f6cbce4bec3ebb8c8ae`; `verification.json` SHA `64a6dac7ca8bd78958c6d03c912b2d145eeece9536f13973e88b17093ce30548`. q1/q2 provider captures already ended exit0 and are preserved; do not rerun. `REPORT.md`/`SHA256SUMS` are not yet created. Raw game-data blob is15576B; complete chunk is15596B; keep distinct.

Next exact action on resumption: write the two private summary/manifest documents from retained artifacts, inspect actual verifier/claims for assertion strength, then root replay to a separate retained result before any acceptance. Do not treat a schema-consistent expected/result pair alone as independent evidence.

## Stopping maintenance and limits

Root hashed existing files only, without rerunning analysis/tests. Identity manifest: `local/checkpoints/root-20261020-pause-manifest.json`, SHA `a30b8792204b5b7ae7f514cfb55176bd7e9dbbc5e10f749a386f35cec4a851f8`. Original inputs/references remain untouched and private evidence stays excluded. No new dependency, installation or download.

`graft check` reports STALE,14 added nodes from the four WIP files; saved log `local/checkpoints/root-20261020-pause-graft.log`. This is an explicitly documented stale index, not a passing gate. No rebuild is started during the user-directed stop. Existing reference scope is missing and deep layer unbuilt; prior sync did not prove reference coverage. On resumption rebuild using persisted scopes without dropping scripts/tools/reference/native/launcher/tests and prove these files queryable before integration acceptance. Pre-existing untracked `tools/graft/` is excluded from staging.

Final maintenance runs `python3 scripts/project_index.py`, `git diff --check`, staged/full path audits, and an explicit allowlisted commit/push. Record results below before ending. The goal remains incomplete and paused by user request.

## Final checkpoint checks

- index: exit0; `local/checkpoints/root-20261020-pause-index.log`, SHA-256 `b302ddeb006321fb039934284af8d987cdf7a8d0d3bda86ef71fe884c72a45d6`; 5604 folders indexed; written.
- diff-check: exit0; `local/checkpoints/root-20261020-pause-diff-check.log`, SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; empty output.
- audit-staged: exit0; `local/checkpoints/root-20261020-pause-audit-staged.log`, SHA-256 `f24cb3fd5755a79810ceadcf3d2f3783f5199d31e060c983a58c05a2666937fb`; PUBLISH PATH AUDIT OK (10 path(s)).
- audit-all: exit0; `local/checkpoints/root-20261020-pause-audit-all.log`, SHA-256 `1930950d73a32096744558afe8184458a9c1cafcc3e4a127dff6ca865f6ee5f7`; PUBLISH PATH AUDIT OK (1126 path(s)).
- Graft: STALE output retained; not a passing source-graph check. No rebuild at user stop.
- Recording the checks initially copied the full audit path list and introduced one trailing space. The final staged diff check caught it; the checkpoint was shortened to log identities and the whitespace corrected in a follow-up documentation commit. No implementation changed.
