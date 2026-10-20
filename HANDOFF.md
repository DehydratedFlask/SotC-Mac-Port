# SoC Mac Native — agent handoff

Updated: 2026-10-20 01:10 CDT (America/Chicago). Principal: root agent of this planning chat.
No native game executable exists. G1 NOT PASSED.

## CURRENT CHECKPOINT — 2026-10-20 01:10 CDT, /root — two private analyses underway

### RE-02-APPMAIN-25A8D0-01 — ACTIVE, private analysis underway

Parent CPU-01 and RE-02 remain ACTIVE. Prerequisites are the pinned RE-01 ELF identity and the
existing app-main dossier `local/re/work/app-main-re02.{md,json}`. The ELF SHA-256 is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; its existing notes record
the byte-level vtable slot `0x15c8cf0 -> 0x25a8d0` and the `0x536730` call passing `RSI=-1`.
The incomplete disassembly currently ends at an undecoded instruction near `0x25aacc`. Those are
static-source observations; no runtime call is claimed. Scope is one bounded characterization of
the full function body, exact byte range/hash, direct call/data references, incoming vtable/caller
linkage, and unresolved edges/coverage. String evidence must not be promoted into a proven C++
class or `BPE_HANDLE` contract. Worker outputs are private under
`local/re/evidence/re02-appmain-25a8d0/`; root owns shared documentation and must independently
rerun the byte/reference verifier before recording evidence. Acceptance: verifier exits 0 against
the exact ELF and all reported ranges/calls/data references match PT_LOAD-backed bytes; report,
verifier, exact REA/Ghidra recipe/Evidence ID and coverage limits are retained. No reachability,
runtime, native implementation, or G1 claim. Failed analysis hypotheses: 0.

Pre-claim identity check command:
`date '+%Y-%m-%d %H:%M:%S %Z' && git status --short --branch && sha256sum local/re/eboot.elf local/re/work/app-main-re02.md local/re/work/app-main-re02.json local/re/work/a25a8d0.dis && sed -n '1,200p' local/re/work/app-main-re02.md && cat local/re/work/app-main-re02.json && tail -30 local/re/work/a25a8d0.dis`
exited `0` at 2026-10-20 01:03 CDT; tree was clean at `ba7b4319f1ae0226a587ac04151b1d992f2941dd`.
Observed hashes: ELF `d0723afb...61d3`, app-main note `cb70fff5...286b6`, structured dossier
`cdfea58a...1ca48`, partial disassembly `09fa7535...7509fb`. Claim details:
[RE-02-APPMAIN-25A8D0-01](docs/checkpoints/2026-10-20-root-re02-appmain-25a8d0-claim.md).
Publication checks: `python3 scripts/project_index.py` exited `0` (`4,135 folders indexed; written`);
`graft check` exited `0` (wiring graph synchronized; optional deep meaning layer remains unbuilt,
1,219 nodes pending); `git diff --check` and `git diff --cached --check` passed; staged path audit
accepted 4 allowlisted paths; full audit accepted 593; `git fetch origin main` succeeded and
pre-commit divergence was `0 0`. Commit `a7904b9` (`docs: claim app-main function analysis`) pushed
successfully (`ba7b431..a7904b9`). This publication record was then pushed in `addc714` (`docs:
record app-main claim publication`, `a7904b9..addc714`). The subsequent checkpoint-record commit
`5008163` (`docs: record claim publication state`) was pushed (`addc714..5008163`). Post-push tree
is clean, HEAD equals origin/main at `5008163a4b29129125785b5f1f6763eea7874c8e`, divergence `0 0`.

`/root/appmain_25a8d0` (`gpt-6-luna`, Extra High) has read its task docs and is using process-local
REA/Ghidra CLI. Its preflight Graft query for `reference/ShadowOfTheColossus-PC/` exited `1` because
that prefix has no indexed files; this is a graph-scope limitation for a local ELF task, not an
analysis failure. Worker reports fresh read-only Ghidra 12.1.4 analysis underway through REA 6.7.0;
at about 2 minutes there was no function Evidence yet. The copied log reports generic function-body
overlap-repair failures and decompiler p-code warnings; applicability to target `0x25a8d0` remains
unresolved, and the worker is preserving the log and continuing byte-level checks. A later
address-specific log query found no `0x25a8d0` match; this is not enough to prove a complete or
healthy Ghidra body. No files changed; failed analysis hypotheses remain 0. Exact next action:
review its exported Evidence/report and independently verify body/range and xrefs. No source or
reference changes, game execution, install, or download. G1 NOT PASSED.

### AUD-01-STS-BYTE-PROBE-01 — ACTIVE, private asset-byte analysis underway

Parent AUD-01, prerequisites DATA-01 and `AUD-01-STS-IDENTITY-01`. The probe is limited to the
paired asset `cp3shadow/sound/test_adpcm/b_info_01.sts_cp3`, read in memory from both verified
cachedFiles archives through `Psarc.read()`. The identity artifact is
`local/validation/aud01-sts-identity-01.json`, SHA-256
`566275c3c104dfc473f8cb3c227641a16b10e2b0da7ffb2811e22b00f877e654`; base archive SHA-256
`373828b9a6e5c689a6a05a01aa14b295026976438aa7796e9baa789ae03dda0`, member ID `10472`, size
`344`, member SHA-256 `d6d74c4ca3cfae66db345551dc22670604d155475a99aa40d6f3a12463056ee7`; patch archive
SHA-256 `656d6870a159013921d036d0b7cf18c679bfc62fe5862b31332c459ff7da05d0`, same member ID `10472`,
size `561`, member SHA-256 `42eb5a005793f67c3dcd2761ec75ad33f3e83a2d863a30daaf79513ebbe0eb86`. Goal is a
bounded private byte-layout characterization and independent verification of any observed
offsets/boundaries. Do not infer codec from directory/extension names, patch-wins runtime behavior,
NGS2 behavior, game use, or playback; if bytes do not establish a schema, leave format
`UNCLASSIFIED`. Do not extract to disk or publish payload bytes. Outputs remain under
`local/re/evidence/aud01-sts-byte-probe-01/`. Failed analysis hypotheses: 0. Claim:
[AUD-01-STS-BYTE-PROBE-01](docs/checkpoints/2026-10-20-root-aud01-sts-byte-probe-claim.md).

Pre-claim selection used the verified identity JSON, choosing the lexicographically first
hash-different pair where both decoded sizes are at least 256 bytes; the identity artifact SHA was
rechecked at 01:06 CDT. An exploratory listing script used a nonexistent `layers.*.members` key
and exited `1`; inspecting `comparison.paired_members` corrected the lookup. No format hypothesis
or byte extraction was attempted. Publication checks: `python3 scripts/project_index.py` exited
`0` (`4,136 folders indexed; written`); `graft check` exited `0` (wiring graph synchronized; deep
meaning layer unbuilt, 1,219 nodes pending); `git diff --check` and `git diff --cached --check`
passed; staged path audit accepted 5 paths; full audit accepted 594; `git fetch origin main`
succeeded and pre-commit divergence was `0 0`. Commit `e51b60e` (`docs: claim paired audio format
probe`) pushed successfully (`5008163..e51b60e`). Post-push tree is clean, HEAD equals `origin/main`
at `e51b60edc638ed056bdfcfe00073a6b2a9ea5bbd`, divergence `0 0`. Follow-up checkpoint commit
`452b55a` (`docs: record audio probe claim publication`) was then pushed (`e51b60e..452b55a`);
post-push tree is clean and HEAD equals `origin/main` at
`452b55ab9fb9365b6c6b25c1dc01323ff2c5b94f`, divergence `0 0`. Worker `/root/aud01_sts_byte_probe`
(`gpt-6-luna`, Extra High) is dispatched for an in-memory pair probe; it will use Graft on the
`Psarc.read()` source before verifying the live reader. Exact next action: receive its private
report/verifier and independently rerun checks. Root keeps shared-doc ownership. No game execution
or audio playback. G1 NOT PASSED.

## PREVIOUS CHECKPOINT — 2026-10-20 01:01 CDT, /root — two bounded analyses published

### CPU-01-CTOR10-CALLER-STATE-01 — STATIC_ANALYSIS_VERIFIED

Worker `/root/ctor10_caller_state`'s private report is hash-bound to ELF SHA
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Root reran
`python3 -I local/re/evidence/cpu01-ctor10-caller-state-01/verify.py > local/re/evidence/cpu01-ctor10-caller-state-01/root-verification.stdout.log 2> local/re/evidence/cpu01-ctor10-caller-state-01/root-verification.stderr.log; result_code=$?; printf '%s\n' "$result_code" > local/re/evidence/cpu01-ctor10-caller-state-01/root-verification.exit.txt; exit "$result_code"`; exit `0`, stderr empty, and root JSON SHA `2695197686a62e0fbbc298aa661db7753f678eb7580c77dadbcd4f3d274bead2` matches the worker verification JSON. The four callee gate fields are byte-zero on the examined path; the caller's separate guard skips unresolved call `0xf09efb`, and helper `0xe1ede0`'s qword gates skip its computed calls for these inputs. No constructor reachability, other-state effects, or runtime behavior is inferred. Report and root logs remain private; see E53 and [checkpoint](docs/checkpoints/2026-10-20-root-cpu01-ctor10-caller-state-complete.md).

### RE-02-PHDR10-SOURCE-AUDIT-01 — ARTIFACT_VERIFIED, meaning UNCLASSIFIED

Worker `/root/phdr10_source_audit`'s private report SHA-256 is `c1572f6181ece3d0e31b4210150240504ded1b100284a9496cadf16575a14e5b`; root independently reopened its cited official SIE and generic ELF sources and saved a review under `local/re/evidence/re02-phdr10-source-audit/root-review.md`. No attributable first-party mapping was found in the bounded public search. The generic ELF ABI only puts this numeric value within the OS-specific range. Partner-only documentation was not accessed; no absence-of-meaning or runtime conclusion is drawn. See E54 and [checkpoint](docs/checkpoints/2026-10-20-root-re02-phdr10-source-audit-complete.md).

Exact next action at that checkpoint: choose and claim the next small CPU-01/RE-02 slice only after checking its prerequisites; do not infer a native startup chain from these static results. `CPU-01-CTOR10-CALLER-STATE-01` has 0 failed analysis hypotheses; `RE-02-PHDR10-SOURCE-AUDIT-01` has 0; parent CPU-01 and RE-02 remain ACTIVE. No code or reference files changed; no install/download or game execution. G1 NOT PASSED.

Shared files changed in this result checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
generated `docs/FOLDERS.md`, and the two linked completion checkpoints. Root verifier output and the
source review remain private under `local/re/`.

Prepublication checks: `python3 scripts/project_index.py` exited `0` (`4,135 folders indexed; written`);
generated `docs/FOLDERS.md` records 37 checkpoint files. The last source-graph check at 00:54 exited
`0` with wiring synchronized; no source files changed since then. Root source pages and exact search
queries are captured in the linked private root review. At the prepublication checkpoint, the exact
next action was to stage only the six allowlisted documentation files, run staged/full publication
audits and staged whitespace check, fetch/check divergence, then commit and push. Failed hypotheses:
0 for both slices.

Publication succeeded in commit `f6d0944` (`docs: verify caller guards and PHDR10 source audit`).
Staged audit accepted 6 paths; full audit accepted 592; `git diff --cached --check` passed; fetch
succeeded and pre-commit divergence was `0 0`. Push advanced `f0889da..f6d0944`. Post-push status
is clean, HEAD equals origin/main at `f6d09449eaa07ac38cce1cbaa7e31f550b82a5f4`, divergence `0 0`.
Exact next action: identify and claim the next small, independent CPU-01/RE-02 slice after checking
its prerequisites and linked specification. Parent CPU-01 and RE-02 remain ACTIVE; G1 NOT PASSED.

## PREVIOUS CHECKPOINT — 2026-10-20 00:55 CDT, /root — PHDR10 public-source search dispatched

`/root/phdr10_source_audit` (`gpt-6-luna`, Extra High) completed the read-only public first-party
source search. The dispatch scope and no-inference boundary are in the published claim checkpoint.
Claim commit `54d12b7`, publication confirmation `15d208c`, and dispatch record `f0889da` are pushed.
Dispatch-record checks: `git diff --check` and `git diff --cached --check` passed; staged audit
accepted 2 paths, full audit accepted 590; fetch succeeded and divergence was `0 0`; commit
`f0889da` pushed (`15d208c..f0889da`). Post-push status was clean at `f0889da41529e1a9e52cd4cff6917043226e7ac2`,
aligned with `origin/main` and divergence `0 0`. Worker report is at
`local/re/evidence/re02-phdr10-source-audit/report.md`; it was then independently reviewed as
recorded in the current checkpoint. Failed hypotheses: 0.

## PREVIOUS CHECKPOINT — 2026-10-20 00:54 CDT, /root — PHDR10 claim published; source-search dispatch pending

The caller-state dispatch record was published in commit `75f936d` (`docs: record slot-10 caller-state
dispatch`) after `d9d4d70`; post-push tree was clean and `HEAD`/`origin/main` aligned at
`75f936db4e3fe8a6d8e5e78735910a9f16268d2d`, divergence `0 0`. `/root/ctor10_caller_state`
(`gpt-6-luna`, Extra High) remains active on the bounded slot-10 pre-call field-state analysis.

New task `RE-02-PHDR10-SOURCE-AUDIT-01` is claimed to seek an authoritative public first-party source
for ELF type `0x6fffff01`, with E51 as the local byte/parser anchor. The exact-literal Graft search
already returned no hits in 528 indexed files; it must not be repeated. Only public, attributable
Sony/PlayStation material or generic ELF specifications may support claims. If no primary mapping is
available, preserve `UNCLASSIFIED`; do not turn community names or absence of search results into a
semantic conclusion. Claim:
[RE-02-PHDR10-SOURCE-AUDIT-01](docs/checkpoints/2026-10-20-root-re02-phdr10-source-audit-claim.md).
The claim was published in `54d12b7` (`docs: claim PHDR10 source audit`) after `python3 scripts/project_index.py`
exited `0` (4,134 folders; 35 checkpoints), `graft check` exited `0` (wiring graph synchronized;
optional deep meaning layer 0%, 1,219 nodes pending), and `git diff --check` passed. Staged path
audit accepted 4 paths; full audit accepted 590; `git diff --cached --check` passed; `git fetch origin
main` succeeded; pre-commit divergence was `0 0`. Commit and push succeeded (`75f936d..54d12b7`);
post-push status is clean, HEAD equals `origin/main` at
`54d12b71eaed5b78745effde0eef52e79ed6e13f`, divergence `0 0`. Exact command/exit/output record:
`local/re/evidence/re02-phdr10/claim-publication.log`. The source-search agent is not yet dispatched.

Exact command/exit/output record: `local/re/evidence/re02-phdr10/claim-publication.log`.
Exact next action at that checkpoint was to dispatch the read-only first-party source search.
Failed hypotheses/tries on the task: 0.

## PREVIOUS CHECKPOINT — 2026-10-20 00:51 CDT, /root — slot-10 caller-state analysis dispatched

Claim publication: `python3 scripts/project_index.py` exited 0 (4,133 folders; `docs/checkpoints`
count 34); `graft check` exited 0 (wiring graph synchronized; optional meaning layer 0%, 1,219
nodes pending); `git diff --check` passed. Staged audit accepted 4 paths, full audit 589; fetch
succeeded and pre-commit divergence was `0 0`. Commit `d9d4d70` (`docs: claim slot-10 caller state
analysis`) pushed (`411a45b..d9d4d70`). Post-push status clean, `HEAD` and `origin/main` both
`d9d4d701ff3bb0c9804aba968e5bba6d9b2fb7fe`, divergence `0 0`.

After publication, `/root/ctor10_caller_state` (`gpt-6-luna`, Extra High) was dispatched for
`CPU-01-CTOR10-CALLER-STATE-01`. Scope is limited to the four caller stack fields read by the
`0xe1ede0` gates. The analyst must preserve unknown helper effects as unknown and may only write
private artifacts under `local/re/`. No analysis result is inferred from dispatch. Claim and exact
scope are in [the checkpoint](docs/checkpoints/2026-10-20-root-cpu01-ctor10-caller-state-claim.md).
Failed analysis hypotheses/tries: 0.

Exact next action: review the private report/verifier when returned, independently rerun its byte
checks, then update EVIDENCE/TASKS/HANDOFF and publish root integration.

## PREVIOUS CHECKPOINT — 2026-10-20 00:50 CDT, /root — slot-10 caller-state analysis claimed

Follow-up publication commit `411a45b` (`docs: record PHDR10 and helper publication`) pushed after
`0e16b9a`; verified post-push tree clean, `HEAD` equals `origin/main` at
`411a45b0d25a20cf83961277f01703176cbd80e9`, divergence `0 0`.

New bounded task `CPU-01-CTOR10-CALLER-STATE-01` is claimed. It traces the four stack fields the
verified helper at `0xe1ede0` reads at its callsite `0xf09f04`, which maps to caller offsets
`RSP+0x38`, `RSP+0x40`, `RSP+0x49`, and `RSP+0x4a`. The claim limits analysis to byte-backed writes,
reads and previously verified helper effects before that call; unknown call effects stay unknown.
No type/ABI/lifecycle inference or claim that the helper's computed calls are unreachable is allowed
without every guard value and path being established. Root owns `HANDOFF.md`; the analyst may edit
only private artifacts under `local/re/`. Claim:
[CPU-01-CTOR10-CALLER-STATE-01](docs/checkpoints/2026-10-20-root-cpu01-ctor10-caller-state-claim.md).
Task row and claim are in the working tree. `python3 scripts/project_index.py` exited `0` with
`4133 folders indexed; written` and checkpoint directory count 34. `graft check` exited `0` with
the wiring graph in sync; the optional deep meaning layer remains 0% (1,219 nodes pending).
`git diff --check` passed. Failed analysis hypotheses: 0.

Exact next action: stage only `HANDOFF.md`, `docs/TASKS.md`, `docs/FOLDERS.md`, and this claim file;
run staged/full publication audits and staged whitespace check, fetch/check divergence, then commit
and push. Dispatch the analyst only after the claim commit is published.

## PREVIOUS CHECKPOINT — 2026-10-20 00:46 CDT, /root — PHDR10 parser slice and CPU helper dossier root-verified

The three supplied LLM Wiki digests were reviewed as operational checkpoints; the project index at
`topics/projects/_index.md` confirms that live repository files remain authoritative. No digest was
promoted as canonical topic knowledge. User-selected launcher decisions from the live task context
remain: clean-exit/Stop logic uses a testable `LauncherCore` helper, and folder-import failures use a
system alert with silent cancellation.

### RE-02-MISSING-PHDR10-01 — STATIC_ANALYSIS_VERIFIED for parser/coverage slice

Root reviewed and reran `/root/phdr10`'s private hash-bound verifier. Exact root command:
`PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/re02-phdr10/verify.py > local/re/evidence/re02-phdr10/root-verification.json 2> local/re/evidence/re02-phdr10/root-verification.stderr.log`; exit `0`, JSON `PASS`, stderr empty. Root and worker verification JSON SHA-256 match at `7f3a873b450f5fda12508bde8125564f50863b0924d887802affc69b439bf259`; verifier hash is `c6146a03d6f7695d8744c61269fbcb67c2e7d4c779d4553472e97fb3399581f5`; report hash is `8041add6304969dc84e5494ed368e5b73c1a89a3430c089b7109acc9d0b41491`. It validates the SELF/ELF hashes, pinned clean reference at `70bb43859cd7ff181a6d409540b757866184d60d`, all 11 program headers, 10 descriptors, every copied interval, and `parse_self` returning `missing=[10]`. PHDR10 is `0x6fffff01`, `p_offset=65,211,380`, `p_filesz=2,928`, `p_memsz=0`; it has no active descriptor, and the pinned reader tolerates it because it is absent from the parser's four required types. This is parser/byte behavior only: platform type meaning, other loaders, and runtime relevance remain unclassified. Completion evidence is E51 and [checkpoint](docs/checkpoints/2026-10-20-root-re02-missing-phdr10-complete.md). Failed analysis hypotheses: 0; RE-02/G0/G1 remain open/not passed.

### CPU-01-CTOR10-E1EDE0-01 — STATIC_ANALYSIS_VERIFIED

`/root/srv01_page_map_19660` completed the private REA 6.7.0/Ghidra 12.1.4 query (Evidence ID `ev_a78fb082a6584d31cebf0c320fc036205741f7093569f68e8355e90ce8237e0c`; exact command, exit `0`, and empty stderr are recorded under `local/re/evidence/rea-e1ede0/analysis.command.txt`). Root reviewed the private report and reran `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/rea-e1ede0/verify.py > local/re/evidence/rea-e1ede0/root-verification.stdout.log 2> local/re/evidence/rea-e1ede0/root-verification.stderr.log`; root exit `0`, stderr empty. Root stdout SHA matches worker JSON `08e9f42004ad18be5599f5c9dfce903b7f71e992e70cedf23b37430d52a8e412`. Function `0xe1ede0..0xe1ee45` is 102 bytes (SHA `fe902c0318c3b14913f6c88d21e54d96a55bc56f1f4c78c3a9648922450add1d`). The verifier checks 92 incoming xrefs (58 E8 calls, 34 E9 tail transfers), body bytes, and `0xf09f04` argument provenance. Two indirect calls and PT_LOAD-gap data remain unresolved; two call refs lack Ghidra caller-function records and relationship coverage is partial. The exact report SHA is `fdbedbaaf50acbfcb16ef8e563ecd21fce126d06002d0de70e6007f0d39b37d2`, verifier SHA `3550ae5c97202fd0c8e4ca86b86c60a00b2c7c6f16b4293683da710b22637b21`. No service/ABI or runtime role is inferred. See E52 and [checkpoint](docs/checkpoints/2026-10-20-root-cpu01-ctor10-e1ede0-complete.md). Failed analysis hypotheses: 0; two verifier assertions were corrected as tool assumptions. Parent CPU-01 remains ACTIVE and G1 NOT PASSED.

Root updated `docs/TASKS.md`, `docs/EVIDENCE.md`, added both completion checkpoints, and regenerated `docs/FOLDERS.md` (checkpoint directory count 33). Exact `python3 scripts/project_index.py` exited `0` (`4133 folders indexed; written`); `graft check` exited `0` (wiring graph synchronized; deep meaning tier remains 0%, 1,219 nodes pending); `git diff --check` passed. These changes are allowlisted documentation only; local reports, verifiers and game-derived bytes remain private under ignored `local/`.

Publication: staged audit accepted exactly 6 paths; full audit accepted 588; `git diff --cached --check` passed; `git fetch origin main` succeeded; pre-commit divergence was `0 0`. Commit `0e16b9a` (`docs: verify PHDR10 and slot-10 helper slices`) pushed successfully (`bfad170..0e16b9a`). Post-push status was clean, `HEAD` and `origin/main` both resolve to `0e16b9aa96b63afb0fb5a85cde7866dd611d5941`, divergence is `0 0`.

Exact next action: select and claim the next bounded task only after reading its linked specification and relevant live artifacts. Both bounded tasks above are `STATIC_ANALYSIS_VERIFIED`; failed analysis hypotheses/tries are 0. RE-02 and CPU-01 remain active; G1 remains NOT PASSED.

## PREVIOUS CHECKPOINT — 2026-10-20 00:41 CDT, /root — PHDR10 verifier dispatched; CPU helper dossier under root review

Task `RE-02-MISSING-PHDR10-01` is claimed as a read-only, hash-bound examination of the one
unclassified SELF/ELF program header. The pinned reader is
`reference/ShadowOfTheColossus-PC/scripts/prepare.py` at commit
`70bb43859cd7ff181a6d409540b757866184d60d`; its working tree was clean. The local identity record
reports program-header index 10 as type `0x6fffff01`, `p_offset=65,211,380`, `p_filesz=2,928`,
`p_memsz=0`, alignment 16, and `missing_segments=[10]`. The pinned reader's code only rejects
unavailable headers for four listed types; index 10 is not in that list, which establishes parser
behavior only, not the ELF type's platform meaning. Claim:
[RE-02-MISSING-PHDR10-01](docs/checkpoints/2026-10-20-root-re02-missing-phdr10-claim.md).
`python3 scripts/project_index.py` exited 0 with 4,132 folders indexed; `git diff --check` passed.
Claim commit `de3fa65` pushed after staged audit accepted 4 paths/full audit 586 paths; pre-commit
divergence `0 0`; post-push tree clean and `HEAD`/`origin/main` aligned. `/root/phdr10` was dispatched
to build a private hash-bound verifier/report; no shared-doc edits permitted. In parallel,
`/root/srv01_page_map_19660` completed its REA/Ghidra query for `0xe1ede0` (exit 0, empty stderr)
and is extracting function/reference details and independently verifying bytes. Exact next action:
review each private result, independently verify the byte-backed findings, then update shared
evidence/checkpoints. Original inputs/reference remain read-only.

## PREVIOUS CHECKPOINT — 2026-10-20 00:36 CDT, /root — G0 environment baseline artifact verified

Task `G0-ENV-BASELINE-01` ran the six read-only commands in `docs/VALIDATION.md` separately through
`python3 -I local/validation/g0-env-baseline-20261020/capture.py`; wrapper exit 0. All six inner
commands exited 0 with empty stderr. Structured private capture is
`local/validation/g0-env-baseline-20261020/capture.json`, SHA-256
`cea524d435eeef15e02d506744969dab872990e4812d6621d6b256f042e475b0`. It records each argv, local
start/end, elapsed seconds, exit code, and per-command stdout/stderr files. Observed machine: arm64,
Apple M4 Pro, 12 CPU cores, 51,539,607,552 bytes RAM (48 GiB); macOS 27.0.1 build 26A434; Xcode
27.1 build 27A9275; Metal compiler at
`/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/metal`;
macOS SDK 27.0. These values match the prior dated machine baseline and extend it with direct
architecture/tool-path/SDK observations. No Metal compilation or game/runtime operation was tested.
The evidence row is E50; the task checkpoint is
[here](docs/checkpoints/2026-10-20-root-g0-env-baseline-complete.md). G0 remains PARTIAL. Exact
`python3 scripts/project_index.py` exited 0 and reported 4,132 folders indexed. `graft check` exited
0: wiring graph in sync; optional deep meaning layer remains 0% (1,219 nodes pending).
`git diff --check` passed. Publication checkpoint: staged path audit accepted 6 paths; full audit
accepted 585 paths; `git diff --cached --check` passed; `git fetch origin main` succeeded; pre-commit
divergence was `0 0`. Commit `a4c40f5` (`docs: verify G0 environment baseline`) pushed successfully
(`8d5af9d..a4c40f5`). Post-push `git status --short` was empty, `HEAD` equals `origin/main` at
`a4c40f566b8f31b168a409f6d3bec42d53d0dd1e`, and divergence was `0 0`. Exact next action: continue
root integration of the active `CPU-01-CTOR10-E1EDE0-01` dossier when its analyst returns; do not
promote G0 from PARTIAL based on this inventory slice.

Concurrent task `CPU-01-CTOR10-E1EDE0-01` remains assigned to `/root/srv01_page_map_19660`; its
REA/Ghidra function query is running and the initial independent raw disassembly is complete.

## PREVIOUS CHECKPOINT — 2026-10-20 00:31 CDT, /root — slot-10 evidence and next helper claim published; analysis dispatched

Task APP-01-IMPORTER-ERROR-01: `launcher/Sources/SoCLauncher/ContentView.swift` now presents the
localized error in a system alert for import failures and leaves the prior folder/readiness intact;
the user selected a system alert and silent cancellation. Exact build command
`swift build --package-path launcher --scratch-path local/build/launcher` exited 0; log
`local/build/app01-import-error-build.log` SHA-256
`fa020037fa18385f827a2d9e5dc2adfce29d4db1df3258784166f65959b3741d`. `git diff --check` passed.
`python3 scripts/project_index.py` exited 0 (4,129 folders indexed). Source SHA-256
`f472a514645881d189e4397a4d8704a60dd2c78505d4e61205ba93d1855695f1`. Graft refresh/build/check
passed and `folderImportError` is queryable. These establish compilation and static wiring only;
native alert interaction, cancellation, real failure text, and sandbox access were not exercised.
The source and task evidence were published in `5c2e093`; a follow-up publication checkpoint is
`f48e512`. For `5c2e093`, the staged audit accepted 5 paths and full audit accepted 581; fetch and
pre-commit divergence check passed at `0 0`, push advanced `4387463..5c2e093`, and post-push status
was clean. The follow-up doc commit also passed audits (2 staged / 581 full) and pushed
`5c2e093..f48e512`; status was clean at that checkpoint. APP task status is
STATIC_ANALYSIS_VERIFIED, failed tries 0; native alert interaction NOT RUN; APP-01 active, G4 and
G1 NOT PASSED.

### SRV-01-PAGE-MAP-19660-01 — pinned function `0x19660` — STATIC ANALYSIS VERIFIED

Worker `/root/srv01_page_map_19660` completed read-only REA 6.7.0 / Ghidra 12.1.4 analysis; root
independently verified the private dossier with
`PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/rea-19660/root-verify.py` (exit 0; result
`local/re/evidence/rea-19660/root-verification.json`, SHA-256
`edd138e0caf9786ad77c104923bbd908b37c21eede685fc08f59081249726c4d`; verifier SHA-256
`08215b3dd940154d38f699069e1329c723bbe9506a11d5ea1e9916e74d3c33f3`). It validates the pinned
ELF and unchanged baseline hash, maps the range through PT_LOAD to file offset `0x1d660`, confirms
the 357 raw function bytes/hash and Ghidra JSON range/call sites, and confirms independent disassembly
ends at `retq` `0x197c4`. The private REA summary and full exact command are in
[the task checkpoint](docs/checkpoints/2026-10-20-root-srv01-page-map-claim.md); report SHA-256
`943cdd6b3067192140b4570c37d97962ec1690fce732981173d50264fc1855bc`; Evidence ID
`ev_be5ca8d1f70434e1b56f9300aba0db39aa895878e4e311c026ed8d9d99a53359`.

Static observation: `FUN_00019660` spans `0x19660..0x197c4`; six resolved incoming call sites
across four callers and one direct call `0x196f8 -> 0x19e00`. Byte patterns include pointer/list-like
writes, a counter branch and record initialization with `0x70` stride. No source-level allocator or
page-bitmap role is assigned; no bitmap-bit update appears in this function. REA's `complete` coverage
is limited to resolved Ghidra references; indirect/computed flow remains unknown. No runtime,
reachability, service contract, native implementation or G1 proof. Tool discovery/retained-log
issues (REA/readelf missing from PATH, absent LLVM read tools, and an ephemeral Ghidra log deleted
after exit) are documented separately; failed analysis hypotheses/tries: 0. `python3 scripts/project_index.py`
exited 0 (4,129 folders indexed); `graft check` exited 0 (wiring graph synchronized; deep meaning
tier remains unbuilt). Status:
STATIC_ANALYSIS_VERIFIED. Files root changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and
the existing task checkpoint; `local/re/evidence/rea-19660/` is private. No install or download;
INSTALL_LOG unchanged. Page-map and Fios semantics remain open and SRV-01 stays ACTIVE.

Publication: staged audit `python3 -I scripts/audit_publish_paths.py` accepted 4 paths; full audit
`python3 -I scripts/audit_publish_paths.py --all` accepted 581. `git fetch origin main` and staged
whitespace check passed; pre-commit divergence was `0 0`. Commit `5d3d262` pushed successfully
(`f48e512..5d3d262 main -> main`). No private REA or root-verifier outputs were staged.

### CPU-01-CTOR10-INDIRECT-01 — slot-10 call dataflow at `0xf09efb` — STATIC ANALYSIS VERIFIED; TARGET UNRESOLVED

The analyst reused E43/E46; no new REA pass was needed. Its private report is
`local/re/evidence/cpu01-ctor10-indirect-01/report.md`, SHA-256
`8e89a42839c9185043b1823bdeec10bdb8670f88e84a6c34f0682919fe107155`. Root reran the byte and PS4
relocation verifier at
`python3 -I local/re/evidence/cpu01-ctor10-indirect-01/verify.py > local/re/evidence/cpu01-ctor10-indirect-01/root-verification.json 2> local/re/evidence/cpu01-ctor10-indirect-01/root-verification.stderr.log`;
it exited 0, with JSON SHA-256 `2f01ccb78d20316a42709b99af563c0c5d32edc40656c3444ae45ce114c8f0d2`
matching the analyst output. Root also ran the independent raw-byte check in
`local/re/evidence/cpu01-ctor10-indirect-01/root-raw-recheck.py` (script SHA-256
`0f7dea2a010bbd0653536e76ecc71d4ae86887a92455040eefe43e599353a6d4`); it exited 0 and confirmed
the 260-byte `0xf09e60..0xf09f63` body hash `22204129cd9b4997ae893d5d078a9bf8b0409114bb122fab2324956b5039a3c4`,
the `vxorps` zeroing at `0xf09e80`, the subsequent zeroed guard store, and the VMA-0 qword.

The function's byte-backed setup zeroes YMM0, stores it over both guard fields, then tests them; this
setup skips the computed call. The helper also stores zero in the caller-local pointer field read at
`0xf09ef3`. If a hypothetical call-reaching state retains that zero, the next load reads file-backed
VMA 0 (`/libexec` bytes), making `[RAX+0xc0]` point to `0x6365786562696cef`, outside either PT_LOAD.
The PS4 relocation parser finds no relocation covering VMA 0 or that operand address. The constructor
slot's `R_X86_64_RELATIVE` relocation identifies only the entry `0xf09e60`. Keep `0xf09efb` unresolved:
static evidence does not establish external process mappings, constructor execution, or a service
contract. Failed analysis hypotheses: 0; G1 remains NOT PASSED. Tool issues and exact commands are in
the [completion checkpoint](docs/checkpoints/2026-10-20-root-cpu01-ctor10-indirect-claim.md).

### CPU-01-CTOR10-E1EDE0-01 — direct helper at `0xe1ede0` — DISPATCHED

The next bounded CPU-01 slice is the direct constructor call `0xf09f04 -> 0xe1ede0` already recorded
in E43. `/root/srv01_page_map_19660` is working on the private static characterization: exact callee range,
bytes, callers/callees, caller argument values, and only byte-supported effects. Graft
`graft grep "e1ede0" --in native/` exited 0 with no hits in 369 indexed files. If existing E43
artifacts do not contain a callee body, use a separate `local/re/rea-e1ede0-snapshot.json` and
private output under `local/re/evidence/rea-e1ede0/`. No service/ABI, runtime, constructor
reachability, or G1 claim. Full claim:
[CPU-01-CTOR10-E1EDE0-01](docs/checkpoints/2026-10-20-root-cpu01-ctor10-e1ede0-claim.md).

Graft coverage limitation: `graft grep "f09efb" --in native/` found no native indexed occurrences;
`graft ask` for `reference/ShadowOfTheColossus-PC/` returned exit 1 (`nothing indexed under` that
subtree). The documented nested rebuild exited 0 but replayed only 528 cached root files and did not
index the pinned nested reference source. `graft check` still exited 0 for the current wiring graph,
so it does not prove that subtree is in scope. `python3 scripts/project_index.py` also exited 0 with
4,129 folders and updated the `docs/checkpoints` count in generated `docs/FOLDERS.md` from 26 to 27.
The exact missing-scope commands and outputs
are recorded in the claim checkpoint; prove queryability before relying on Graft coverage there.

Several bounded slices have passed their local checks. DATA-02 and AUD-01 remain partial inventory
tasks; CPU-01d remains a leaf fixture only. The principal agent integrated all docs; private
archive, manifest, test logs and build outputs stay under ignored `local/`.

Latest bounded result: CPU-01-CTOR-PREFIX-01 now stops after the ten recovered constructor callbacks
and reports the unresolved original slot 10 at RVA `0xf09e60`. A private hash-bound checker verifies
103 registered RVA/index pairs against the pinned ELF and constructor table. Root independently
reran the build, focused CTest (1/1), checker, and whitespace check successfully; worker logs are
under `local/validation/20261019T2321-cpu-01-ctor-prefix-01/`. This proves runner bookkeeping only.
The prefix runner bookkeeping, two leaf-fixture index annotations, bridge member identities, the
static slot-10 dossier, and its caller-local initializer dataflow slice are verified and published.
A read-only AUD-01 survey verified exact identities and bytes for paired test_adpcm STS members; the
APP-01 static UI/accessibility review is recorded, and folder-import failure feedback now builds.
Runtime constructor
behavior, helper contracts, audio format/use, game startup, and G1 remain unproven. Recent commits:
`7c2626b` (page-map publication record), `5d3d262` (page-map dossier), `f48e512` (launcher
publication checkpoint), `5c2e093` (launcher importer feedback), and `19cd2a2` (STS completion);
prior evidence commits include `bda7289`, `545f08f`, `b26768a`, `fc1b278`, and `e40a2a4`.

### AUD-01-STS-IDENTITY-01 — paired cachedFiles test_adpcm identity survey — FIXTURE VERIFIED; parent active

`/root/aud01_sts_cp3_identity` created a private repeatable report under `local/validation/`; root
reviewed the script and independently ran
`PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/aud01-sts-identity-01.py` with stdout and
stderr redirected to the private root log (exit 0). Root output matches worker run1 byte-for-byte;
report SHA-256 is
`566275c3c104dfc473f8cb3c227641a16b10e2b0da7ffb2811e22b00f877e654`. All 290 target entries in
each archive were read through `Psarc.read()` and match the path/ID/offset/block/size manifest
locators. The exact path sets match; all 290 decoded hashes differ and 285 member sizes differ.
Both archive summary identities matched before and after the read. Observation only: this identifies
per-layer bytes/sizes. No codec, file schema, gameplay use, NGS2 behavior, or playback is established.
Failed tries: 0. Exact evidence is in E44 and
[ASSETS](docs/ASSETS.md). The full claim/completion record is:
[AUD-01-STS-IDENTITY-01](docs/checkpoints/2026-10-19-root-aud01-sts-identity-claim.md).
Next exact action: recover the game's own asset/audio reader through the open RE-02/Fios/ULT work;
do not guess a codec from `.sts_cp3` names or bytes.

### APP-01-UI-A11Y-REVIEW-01 — static macOS launcher UI/accessibility review — STATIC REVIEW COMPLETE

`/root/app01_ui_review` completed a read-only review of `SoCLauncherApp`, `ContentView`,
`ReadinessRow`, and the file-import/process handoff. It found conditional security-scoped URL and
child-process-lifetime risks, silent importer-error handling, and a VoiceOver announcement check
to perform. These remain bounded follow-ups, not confirmed runtime defects: sandbox/distribution
policy is undecided, and UI behavior has not been exercised in a built app. No source changes or
tests/builds were made. VoiceOver, Accessibility Inspector, Finder launch, native window behavior,
Full Keyboard Access, and process cleanup at app quit remain NOT RUN; G4 remains NOT PASSED.
Evidence and primary Apple references are in [E45](docs/EVIDENCE.md) and the
[review checkpoint](docs/checkpoints/2026-10-19-root-app01-ui-review-claim.md).

### APP-01-IMPORTER-ERROR-01 — report folder-import failures — BUILD VERIFIED; UI RUN NOT DONE

`ContentView` now keeps the previous folder/readiness on failure and presents the localized error in a
system alert. The user's selected behavior is honored. For the current single-folder `fileImporter`
overload, Apple documents that cancellation does not call `onCompletion`; cancellation remains
silent. Build command `swift build --package-path launcher --scratch-path local/build/launcher`
passed; log is `local/build/app01-import-error-build.log`. No test suite was added or run; no native
alert interaction was performed. Sandbox/bookmark and child-process ownership findings from E45
remain open. Full record: [APP-01-IMPORTER-ERROR-01](docs/checkpoints/2026-10-20-root-app01-import-error-claim.md).

### SRV-01-PAGE-MAP-19660-01 — characterize pinned function at `0x19660` — CLAIMED

`/root/srv01_page_map_19660` owns a read-only REA/Ghidra characterization of the pinned local ELF
at `0x19660`, the next open SRV-01 page-map target. Graft confirms no native lift for this address
exists; this claim does not assume its purpose from nearby comments. Keep the baseline REA snapshot
unchanged, use a distinct snapshot and private evidence under `local/re/evidence/`, and report exact
range/hash/calls/data references, coverage, and raw-byte limits. No game execution, installation,
native replacement, or publication of game-derived binaries. Next exact action: export and verify
the static dossier, then stop for root integration. Claim:
[SRV-01-PAGE-MAP-19660-01](docs/checkpoints/2026-10-20-root-srv01-page-map-claim.md).

### DATA-02 — pakN archive identity

- OBS: `local/extracted/CUSA08809/pakN.psarc` is 12,838,532,222 bytes. Updated
  `scripts/psarc.py` hashes in 1 MiB bounded reads and detects observed path/descriptor replacement,
  truncation, growth, or metadata changes. `scripts/test_psarc.py` covers bounded multi-chunk reads
  and path replacement. `python3 -I scripts/test_psarc.py` passed 12/12; root log:
  `local/build/psarc-tests-20261019-root.log`.
- The private summary records SHA-256
  `559f7a3fa66bc652d36c5f1a885b351e4aeb17dadbba06a0006520bfd325be73`; independent
  `shasum -a 256 local/extracted/CUSA08809/pakN.psarc` and a size/summary assertion passed.
  Root log: `local/build/pakN-sha256-20261019-root.log`. Summary:
  `local/manifests/assets/CUSA08809-pakN.summary.json`. The generation pass used a cleaned
  temporary output directory under `local/build/`; its exact path and command are in the completion
  checkpoint.
- Observation: archive identity is verified. Inference: this does not establish member schemas,
  TOC digest meaning, `pakN` member encoding, or mount order. Task remains ACTIVE/partial; failed
  implementation tries: 0. Next exact action: keep DATA-02 open for member-format/schema work.

### AUD-01 — 86 audio member identities

- `scripts/classify_audio_headers.py` now hashes each full decoded member and records its PSARC
  entry ID, offset and first block; header family/entropy remain prefix observations. The private
  `local/manifests/assets/audio-headers.json` has 86 rows across 5 archives (3,726,760 decoded
  bytes). `python3 -I scripts/test_audio_headers.py` passed 12/12 at
  `local/build/audio-headers-tests-20261019-root.log`; independent per-row hash/locator validation
  passed at `local/build/audio-identities-verify-20261019-root.log`.
- An initial `python3 -I -m unittest scripts/test_audio_headers.py` invocation exited 1 because
  isolated mode omitted the cwd from import resolution (`ModuleNotFoundError: scripts`). Running
  the test file directly passed; this was a command invocation issue, not a failing assertion or
  implementation hypothesis. Codec remains UNKNOWN; no NGS2 or playback claim. Task ACTIVE/partial;
  failed implementation tries: 0. Next exact action: inspect actual bank readers and format evidence.

### CPU-01d — unguarded vector-copy leaf `0xcdc760`

- Hash-pinned ELF `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`: body
  `c5 f8 10 05 d8 ac 61 00 c5 f8 11 05 00 55 67 02 c3`; source `0x12f7440`, destination
  `0x3351c70`, lanes `{~0,~0,~0,0}`; constructor survey position 157 and copy-manifest position 99.
  Native implementation/test/checker passed. CMake target build log:
  `local/build/native/cdc760-build.log`; focused CTest 1/1:
  `local/build/native/cdc760-ctest.log`; byte/hash/manifest/native-dump checker:
  `local/build/native/cdc760-checker.log`.
- First target build returned 2 because the existing Make tree did not yet have the target rule;
  `cmake -S native -B local/build/native` regenerated it, then the same target passed. The initial
  transient output was not retained separately; configure/build logs capture the successful
  recovery. No code hypothesis failed; failed tries: 0. `graft grep "init_vec_cdc760"` finds 4
  hits across implementation/header/test; `graft check` reports the wiring graph in sync, with
  deep meaning tier unbuilt; `python3 scripts/project_index.py` indexed 4,116 folders.
- This is a plain leaf fixture, not startup registration or engine reachability. The separate
  `0xd71f60` escalation remains untouched. Task slice FIXTURE_VERIFIED; G1 NOT PASSED, G2-G6 NOT
  RUN. Next exact action: select one more bounded, dependency-valid task and claim before editing.

Publication: claim docs for `0xcdc760` are on `main` in `1494700`, with the follow-up publication
record in `54de3cf`. The implementation/docs batch is commit `3370386` (`feat: add archive hashes
and cdc760 fixture`), pushed to `origin main`. Before commit, `git diff --cached --check` passed;
`python3 -I scripts/audit_publish_paths.py` passed 16 staged paths and
`python3 -I scripts/audit_publish_paths.py --all` passed 563 paths. No private `local/` outputs
were staged. Next exact action: select and claim another bounded, dependency-valid task before
editing; the APP-01 claim below now defines the next exact action. Cold boot/audio/save/Metal and
G1 remain unproven.

### APP-01 — clean-exit and requested-Stop readiness — SLICE FIXTURE-VERIFIED, PARENT ACTIVE

Root delivered a bounded launcher lifecycle fix. In `launcher/Sources/SoCLauncher/LauncherModel.swift`,
`processEnded` returns to `.idle` after a requested Stop or a zero-status normal exit. `canStart`
only accepts `.ready` or a failed state, so when the retained readiness report remains startable the
Play button becomes disabled and status says “Not ready to play.” Implement a testable lifecycle
transition in `LauncherCore`: requested Stop and successful exit return to `.ready` only when the
current report permits start, otherwise `.idle`; unexpected exit preserves the failed status.
Agent `/root/app01_probe` previously identified this behavior; root owns implementation/docs.

- Graft evidence this claim: `graft callers canStart --depth all` found no indexed symbol; switched
  to `graft grep "canStart"` (10 hits across four files; ~2,579 tokens saved). `graft callers
  LauncherModel.validate --depth all` reported no indexed callers (~632 tokens saved). Live spans
  were checked in `LauncherModel.swift`, `LauncherState.swift`, `ContentView.swift`, the LauncherCore
  tests, and G4 in `docs/VALIDATION.md`.
- Planned files: `launcher/Sources/LauncherCore/LauncherLifecycle.swift`,
  `launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift`, and the focused
  `LauncherModel.swift` call-site update. No process ownership, executable selection, or core API
  changes are part of the claim.
- Exact verification: `swift test --package-path launcher --scratch-path local/build/launcher`.
  This validates only model mapping, not a real GUI child lifecycle or G4 acceptance.
- User confirmed the testable LauncherCore helper at 2026-10-19 23:05 CDT. An initial
  helper/model version passed the package tests (11 tests), but its output was overwritten by the
  next run. Adding `canStart` and direct predicate tests then failed to compile because its switch
  cases lacked explicit returns; this remains failed implementation try 1. The correction adds
  explicit returns. `swift test --package-path launcher --scratch-path local/build/launcher`
  now passes all 12 tests (5 lifecycle cases plus 7 existing readiness tests); log:
  `local/validation/20261019T2307-root-app01-lifecycle-try1/swift-test.log`.
- `python3 scripts/project_index.py` completed and updated the LauncherCore and test folder counts
  in `docs/FOLDERS.md`. `graft check` reports the wiring graph in sync; its deep meaning layer is
  still 0% built. The refreshed graph can query `LauncherLifecycle`. `git diff --check` passed.
  G4 remains NOT RUN: this pure helper suite does not exercise Finder launch, visible UI, or a
  native game core. G1 remains NOT PASSED; G2-G6 remain NOT RUN.
- The lifecycle helper, model call-site and tests were published in commit `8b10c09` and pushed to
  `origin main` at 2026-10-19 23:08 CDT. The staged path audit passed 8 paths; the full tracked
  path audit passed 566 paths. No private `local/` outputs were staged.
- Prior checkpoint with completed archive/audio identity and `0xcdc760` fixture slices is
  [recorded here](docs/checkpoints/2026-10-19-root-data02-aud01-cpu-cdc760-complete.md), published
  through `f59500e`. Failed implementation tries: 1; the compile failure was corrected within
  the same bounded hypothesis. APP-01 remains ACTIVE; G4 is NOT RUN.

### AUD-01-XFF-01 — characterize one paired XFF sound bank — FIXTURE_VERIFIED

Integrator `/root` claims a bounded AUD-01 slice for `/root/audio_asset_survey`, which owns only
`scripts/audio_bank_header.py` and `scripts/test_audio_bank_header.py`. Inspect the `.hd`/`.bd`
pair `cp3shadow/sound/se/com_sys.hd` and `cp3shadow/sound/se/com_sys.bd` in the private
`local/extracted/CUSA08809/pak2.psarc`. Add a small bounds-checked structural reader and synthetic
tests for only fields justified by repeated byte layouts and byte spans. If no field beyond the
magic/length boundary is established, retain that uncertainty instead of assigning guessed
semantics. Do not claim a codec, NGS2 contract, gameplay use, or playback.

- Agent-reported live identity: archive size 16,557,548 bytes, SHA-256
  `9c655de637baba5f2c35250a2a7024f405f2f95c19fb4739ff084fa95cbdf0b5`. `.bd` is entry 1310,
  offset 6,989,994, first block 664, decoded size 3,672, SHA-256
  `137e64c118ada84b4dd9bd73bf9e4b7e9e1ef061fa2574dc304d5f7cdd1ebd81`; `.hd` is entry 1311,
  offset 7,884,070, first block 825, decoded size 1,908, SHA-256
  `39ebb27c1d469853e12ffc439b17e8641d6258f9cc0f1029303c92bb9683ffe3`. Both begin with the
  observed bytes `78 66 66 00 00 00 00 00 00 00 00 00 01 00 00 00` (`xff\0` plus unknown fields).
- Root Graft query: `graft ask "What existing PSARC APIs can safely read a named sound-bank member and what bounds checks are already established?" --source --in scripts/` returned `Psarc.read` and its block/decompressed-length checks. Approximate saving: 8,994 tokens. The deep Graft meaning layer remains unbuilt.
- Exact acceptance commands: `python3 -I scripts/test_audio_bank_header.py`; then
  `python3 -I scripts/audio_bank_header.py --archive local/extracted/CUSA08809/pak2.psarc --member cp3shadow/sound/se/com_sys.hd --member cp3shadow/sound/se/com_sys.bd`.
  The latter must confirm live locators/hashes and report only supported structural facts.
- Cross-check checkpoint: a reproducible private scan of 25 `.hd`/`.bd` pair groups (50 live
  decoded members) matched every manifest path/locator/hash. Of 22 `xff\0` pairs, the little-endian
  u32 at offset `0x14` equals decoded member size in all 44 members (44/44, zero mismatches).
  Prefixes have two observed variants; `boss01.bd` shows the byte-12 word is not universal, so
  bytes 4..19 remain opaque. Exact command:
  `PYTHONDONTWRITEBYTECODE=1 python3 -I local/validation/20261019-231219-audio-bank-header-crosscheck/pair_crosscheck.py`;
  exit 0; output `local/validation/20261019-231219-audio-bank-header-crosscheck/pair_crosscheck.log`.
- `/root/audio_asset_survey` implemented only the claimed two scripts. Six synthetic tests cover
  the observed prefix/length rule, minimum bound, truncation, prefix rejection, inconsistent
  length, and byte-order mismatch. Root reran both acceptance commands successfully; private logs
  and command list are in `local/validation/20261019-2315-root-aud01-xff-verification/`.
  `python3 -I scripts/test_audio_bank_header.py` passed 6/6. The live pair command exited 0 and
  matched archive summary identity plus both manifest locators/hashes; each member reports
  `xff\0` and u32le@0x14 equal to decoded size. `graft grep "parse_xff_member"` resolves the parser
  and tests; `graft check` reports the wiring graph in sync, with deep meaning tier unbuilt.
- Initial `python3 scripts/project_index.py --check` exited 1 after the verification directory was
  added; `python3 scripts/project_index.py` refreshed 4,126 folders and the subsequent check passed.
  Logs: `project_index_refresh.log`, `project_index_check.log`; `git diff --check` and
  `git diff --cached --check` passed. This index freshness correction was not an implementation
  hypothesis failure.
- Observation: the parser reports only XFF magic, opaque bytes 4..19, and the length word.
  Remaining risk: other field meanings, codec, NGS2 behavior, gameplay use, and playback stay
  UNKNOWN. Task status FIXTURE_VERIFIED (bounded slice), failed implementation tries 0. The
  allowlisted source/evidence batch was pushed in `7b2747b`. The two independent task claims below
  were published in `00844f9` and dispatched to agents with distinct owned files.
- A separate read-only CPU survey proposed RVA `0xcc5a10`; it decodes to the same 16-byte constant
  copy behavior covered by 95 existing fixtures, so root deferred it for low marginal semantic
  coverage. This was task selection only, not a failed implementation try.

### RE-02-DYNLINK-01 — synthetic dynamic-link parse fixture — FIXTURE_VERIFIED

Integrator `/root` claims a test-only RE-02 slice for `/root/re02_triage`; implementation owns only
`scripts/test_ps4_dynlink.py`. Existing `scripts/ps4_dynlink.py` remains a read-only dependency.
Claim record: [RE-02-DYNLINK-01 checkpoint](docs/checkpoints/2026-10-19-root-re02-dynlink-fixture-claim.md).

- Graft navigation: `graft skeleton scripts/ps4_dynlink.py` located `parse(elf)` and the `Image`
  API; its source parse result fields were reviewed before defining this test task. The read-only
  survey found six tests for NID hashing, ID widths, thunk lookup, constructor walking, and bounds,
  but no end-to-end `parse()` fixture.
- Build one small synthetic ELF with a module/library declaration, one imported symbol, and one
  `JMPREL` relocation. Assert parsed module/library encodings, symbol NID, relocation table/type,
  symbol index, target, and signed addend. Do not change the parser unless the new fixture exposes
  a concrete defect; stop and report before widening owned paths.
- Exact acceptance command: `python3 -I scripts/test_ps4_dynlink.py`. `/root/re02_triage` and root
  independently passed all seven tests. Worker log: `local/validation/re02-dynlink-01-test_ps4_dynlink.log`;
  root log and command record: `local/validation/20261019-2320-root-re02-dynlink-verification/`.
  `git diff --check -- scripts/test_ps4_dynlink.py` passed. `graft grep "parse(" --in scripts/`
  returned an invalid-regex error; corrected query `graft grep "JMPREL" --in scripts/` refreshed
  one file and found the parser and new fixture. Task status FIXTURE_VERIFIED; failed tries 0.
  This parser fixture does not establish loader execution, ABI behavior, or runtime addend
  semantics; it covers one JMPREL row and not RELA. The fixture and evidence were pushed in
  `b2a5b6f`. Next exact action: collect and verify the independent CPU constructor gate while
  keeping RE-02's runtime loader limits explicit.

### CPU-01-CTOR-PREFIX-01 — fail-fast original constructor prefix — FIXTURE_VERIFIED

Integrator `/root` claims a bounded CPU-01 runner correctness slice for `/root/cpu_leaf_survey`.
Owned files: `native/core/startup_ctors.hpp`, `native/core/startup_ctors.cpp`,
`native/tests/test_startup_ctors.cpp`, and `tools/lift/check_ctor_slots.py`.
Claim record: [CPU-01-CTOR-PREFIX-01 checkpoint](docs/checkpoints/2026-10-19-root-cpu01-ctor-prefix-claim.md).

- Graft: `graft callers run_constructors --depth all` reported ambiguous duplicate definitions and
  no reliable cross-file callers. Follow-up `graft grep "run_constructors"` found the two
  definitions and three invocations in `native/tests/test_startup_ctors.cpp`; no other indexed
  uses were returned.
- Offline source identity: ELF SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; original constructor
  table has 573 entries at `0x2dd70e0`, sentinel at `0x2dd5ef8`, and the extracted table-word SHA-256
  is `5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28`. First unresolved original
  entry is index 10, slot RVA `0x2dd7090` targeting `0xf09e60`. Its 255-byte body hash is
  `fde1ac223a4c084da0a1a9d04804bee8ba8e12be0ac46e1a8e98a925ac4ffcc7`.
- Make original slot gaps explicit and stop the runner at index 10 after exactly ten recovered
  callbacks, reporting `blocked_on_rva == 0xf09e60`; correct the registered indices for
  `0xd32720` and `0xd2ffe0` to 119 and 120. A hash-bound private-input checker must verify every
  registered RVA/index against the original table. Do not implement the slot-10 service graph or
  claim progress beyond it. Disassembly suggests allocator/atexit/callback dependencies, but their
  complete semantics remain unproven.
- Exact acceptance commands:
  `cmake --build local/build/native --target test_startup_ctors`;
  `ctest --test-dir local/build/native -R '^startup_ctors$' --output-on-failure`;
  `python3 -I tools/lift/check_ctor_slots.py`. This gate verifies native runner bookkeeping only;
  it does not establish startup reachability or G1. Failed tries: 0. At 23:19 CDT,
  `/root/cpu_leaf_survey` was assigned the four claimed implementation paths.

- Completion checkpoint — 2026-10-19 23:26 CDT. Agent `/root/cpu_leaf_survey` completed the four
  claimed files; integrator `/root` independently reran all three acceptance commands and
  `git diff --check`. All exited 0; focused CTest passed 1/1. The checker matched ELF SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, table start `0x2dd70e0`,
  573 entries, and table SHA-256 `5917cb85db190e9ef93ce5e06d5b6596d40b6c3623166e37ed7c6c3c3cd3ab28`; it
  verified 103 registered RVA/index pairs and identifies index 10 / `0xf09e60` as the first
  unresolved slot. Worker logs: `local/validation/20261019T2321-cpu-01-ctor-prefix-01/01-cmake-build.log`,
  `02-ctest.log`, `03-check-ctor-slots.log`, and `commands.txt`. Project index refresh and final
  check reported 4,128 folders/current; `graft check` reported the wiring graph in sync; the new
  checker is queryable. The earlier stale-index check outputs are retained and explained in
  `commands.txt`; they were caused by logs added after a refresh, not an implementation failure.
- Observation: the prefix runner stops after ten registered callbacks; `0xd32720` and `0xd2ffe0`
  now carry original indices 119 and 120. Inference: fail-fast bookkeeping prevents the fixture
  runner from silently skipping the first unresolved constructor. The slot's service semantics and
  execution in the game are still unknown. Failed implementation tries: 0. Task status:
  FIXTURE_VERIFIED. G1 remains NOT PASSED; G2-G6 remain NOT RUN. The source/test/checker batch was
  pushed as `b26768a` (`test: enforce constructor prefix blocker`) at 23:26 CDT. Before commit, the
  staged path audit passed 10 paths, `--all` passed 573 tracked paths, staged whitespace check
  passed, project index reported 4,128 folders current, Graft wiring check passed, and fetched
  remote divergence was `0 0`. Next exact action: investigate RVA `0xf09e60` offline; do not code a
  replacement until its behavior and required native contract are established.

### CPU-01-CTOR10-ANALYSIS-01 — characterize first unresolved constructor — STATIC_ANALYSIS_VERIFIED

Integrator `/root` claims a read-only, hash-bound REA/Ghidra analysis for `/root/cpu_slot10_analysis`.
Prerequisite: CPU-01-CTOR-PREFIX-01 is fixture verified and the exact ELF/table identities are in
its checkpoint. Use only the existing local ELF; do not run the game, install/download modules, or
write native replacement code.

- Graft `grep "f09e60"` found the explicit null registration, fixture assertion, and table checker;
  it did not find a body analysis. The private REA/Ghidra dossier is now independently checked.
- Deliver a private evidence record for RVA `0xf09e60`: exact ELF digest/image base, REA and Ghidra
  versions, Evidence ID, function range/body hash and analysis coverage; direct calls and resolved
  import identities; data references; unresolved computed calls; and observation/inference split.
  If a body or references cannot be established, state that limitation without relabeling the slot.
  Preserve a reproducible analysis recipe and export evidence before closing any temporary project.
- Tool/input identity: REA 6.7.0, Ghidra 12.1.4, OpenJDK 25.0.4.1, x86:LE:64:default:gcc; ELF SHA-256
  `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; image-base observation 0;
  Evidence ID `ev_054b2f190852ca75b5f92898208d4f8e554fbcfe6ab6416b0c018242902d7921`. New snapshot
  SHA-256 `cb35935424cae5121c34741b73453d545a49bd61c65b68727ffcf6b9c8333352`; baseline snapshot
  remained `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354`.
- Observation: `FUN_00f09e60` is a Ghidra-assigned inclusive range `0xf09e60..0xf09f63` (260 bytes),
  body SHA-256 `22204129cd9b4997ae893d5d078a9bf8b0409114bb122fab2324956b5039a3c4`. The earlier 255-byte
  hash `fde1ac223a4c084da0a1a9d04804bee8ba8e12be0ac46e1a8e98a925ac4ffcc7` covers through RET and
  excludes the five-byte terminal call at `0xf09f5f..0xf09f63`. Seven direct calls are byte-verified:
  `f09ec7→e1eda0`, `f09f04→e1ede0`, `f09f13→b30a10`, `f09f1a→f90210`, `f09f2c→b30950`,
  `f09f42→f8f390`, `f09f5f→f8f3a0`. Thunks resolve through same-ELF import slots to `malloc` and
  `__cxa_atexit` (`name-by-nid-hash-only`) and `__stack_chk_fail` (`contract-declared`); those labels
  do not prove native ABI contracts. The computed call at `0xf09efb` (`[RAX+0xc0]`) remains unresolved.
  Non-stack data targets: `0x15fb7b0`, `0x15fb7c0`, `0x15fe518`, `0x1604218`, `0x3356678`,
  `0x3356688`, `0xe44a10`. REA relationship coverage is partial and limited to callees.
- Independent root verification: `python3 -I local/re/evidence/rea-f09e60-verify.py > local/re/evidence/rea-f09e60-root-verification.json` exited 0; `cmp` against worker verification output passed. Root hashes of ELF, baseline snapshot, new snapshot and analysis output match the values above; baseline snapshot was not altered.
- Inference/risks: code flow suggests global allocation and cleanup registration, but helper functions and callback contracts remain unknown. Ghidra reported unrelated overlapping-body repairs at `0xb30a00` and `0xb30c20` near target `0xb30a10`, so that local boundary remains cautious; Ghidra's global warnings elsewhere are not target evidence. No runtime behavior/reachability or native ABI was proven; G1 remains NOT PASSED. One verifier attempt used the wrong snapshot key (`id` rather than `evidence_id`) and was corrected; failed task tries: 1 checker correction, 0 failed REA invocations.
- Private artifacts: `local/re/evidence/rea-f09e60-summary.md`, analysis JSON/stderr, distinct snapshot,
  command recipe, progress log, independent objdump/thunk checks, verifier script/output and root
  verifier output. No game-code bytes are copied into public docs.
- Task exit: static blocker characterization complete; no replacement code. Exact next action: resolve
  the indirect-call target and local helper contracts only if a separate bounded task is claimed;
  keep CPU-01 and G1 open until runtime startup and required gates are demonstrated.

### CPU-01-CTOR10-INIT-HELPER-01 — characterize caller-local initializer effects — STATIC_ANALYSIS_VERIFIED

`/root/cpu_ctor10_init_helper` completed a read-only REA/Ghidra analysis of `0xe1eda0`; root reviewed
the private verifier and reran it. ELF SHA-256 is
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Evidence ID
`ev_9b111492162042c8bdb212bc83c30eda2370d34b6d4bb2cec357d83bff0d437d`. Helper range
`0xe1eda0..0xe1eddd` is 62 bytes, SHA-256 `d51c6f9bf6d0197e16ed629e6f6c0f2e450511aa3c16f1a2c475b60352f1f559`.
The caller sets `RDI=RSP+8`, `RSI=0`, `RDX=0x10`, `RCX=0x10`. For those arguments, helper writes
four qwords at RSP-relative spans `[0x8,0x10)`, `[0x10,0x18)`, `[0x28,0x30)`, and `[0x30,0x38)`,
which do not overlap either checked guard field. Before the call, caller zeros `[RSP+0x98,RSP+0xb8)`;
the byte at `RSP+0xa8` is still zero when tested, so `je 0xf09f01` skips the indirect call at
`0xf09efb` on this static path. This does not establish that constructor `0xf09e60` is reachable or
runs, nor resolve the computed target. Full result and checker evidence are in E46 and
[the completion checkpoint](docs/checkpoints/2026-10-19-root-cpu01-ctor10-init-helper-claim.md).

Private REA, snapshot, disassembly and verifier artifacts remain under `local/re/evidence/` and
`local/re/`; the baseline snapshot hash is unchanged. Root's verifier output matched the worker's
byte-for-byte. One first checker shell wrapper failed because zsh reserves `status`; the corrected
wrapper passed. Failed REA invocations: 0; failed analysis hypotheses: 0; failed command wrappers: 1.
Next exact action: if continuing CPU-01, claim a separate bounded task to resolve the computed call
or remaining helper contract; do not infer constructor reachability or runtime behavior. G1 remains
NOT PASSED.

### CPU-01-CTOR-METADATA-01 — correct original indices in leaf fixtures — FIXTURE_VERIFIED

Integrator `/root` claims the narrow metadata follow-up in `/root/cpu_ctor_metadata`. The slot
checker proves that `0xd32720` and `0xd2ffe0` occupy original constructor indices 119 and 120, while
their standalone headers, JSON dumps and private checkers still assert 115 and 116. Graft queries
identified the four stale metadata locations.

- Owned files: `native/core/vec_d32720.hpp`, `native/core/vec_d2ffe0.hpp`,
  `native/tests/test_vec_d32720.cpp`, `native/tests/test_vec_d2ffe0.cpp`,
  `tools/lift/check_fd32720.py`, and `tools/lift/check_fd2ffe0.py`.
- Update only original-index metadata to 119/120. Preserve function semantics, test behavior,
  vector-copy positions and survey/manifests positions. Acceptance: build both test targets, run
  their focused CTests, run both private ELF checkers, and rerun `check_ctor_slots.py`. Failed tries: 0.
- Result: worker and root independently built both test targets, passed focused CTest (2/2), ran
  both private ELF checkers (indices 119 and 120), reran `check_ctor_slots.py` (103 pairs; slot 10
  remains the blocker), and passed `git diff --check`. Worker logs are under
  `local/validation/20261019T2321-cpu-01-ctor-prefix-01/ctor-metadata-{build,ctest,check-fd32720,check-fd2ffe0,check-ctor-slots}.log`.
  Root's reruns were direct and not separately logged. No behavior, addresses, vector positions, or
  function semantics changed. This corrects provenance only and does not pass G1. Failed tries: 0.
  Project index check (4,128 folders), `graft check`, staged/all publication audits (10/576 paths),
  and staged whitespace check passed; fetched `origin/main` had 0/0 divergence. Commit `fc1b278`
  pushed the six owned files and four documentation files to `origin/main`. Exact next action:
  finish CPU slot-10 and bridge-member private analyses, then integrate each result separately.

### DATA-02-BRIDGE-SXD-01 — compare cachedFiles bridge members — ARTIFACT_VERIFIED

Integrator `/root` claims a read-only DATA-02 format characterization for `/root/data02_bridge_sxd`.
Prerequisite: DATA-01 identities and E25 classify exactly two `.sxd` members with observed prefix
`fbad9897`, both named `sound/shadow/bridge.sxd` in base and patch cachedFiles archives.

- Use exact archive/member identities, entry IDs, spans, decoded lengths and full hashes from the
  private manifest and `Psarc.read()`. Compare both variants by byte offset; report only field/record
  boundaries corroborated by raw bytes and a repeatable structural rule. Do not infer engine use or
  semantics from the path or magic. Do not edit shared source or docs; root integrates the result.
  If no schema is established, report that evidence boundary without a permissive parser stub.
- Result (observation): private script `local/validation/data02-bridge-sxd-01.py` re-reads both exact
  members via `Psarc.read()`, verifies summary and member manifests, and emits the report
  `local/validation/data02-bridge-sxd-01.json`. Base archive SHA-256
  `373828b9a6e5c689a66b05a01aa14b295026976438aa7796e9baa789ae03dda0`, entry 40079, stored span
  `[28218302,28218921)`, decoded length 619, member SHA-256
  `81a635460b7da1979052478b2e7324e1cdef0e3402c096d22565d03b9790d2b8`. Patch archive SHA-256
  `656d6870a159013921d036d0b7cf18c679bfc62fe5862b31332c459ff7da05d0`, entry 40079, stored span
  `[29830538,29831176)`, decoded length 638, member SHA-256
  `0dd7a8f06d54b471e06dd9cd4f25fe3db904c3497d47daeb5deaa7c1c122bd78`. The members share prefix
  `fbad9897`; at 619 aligned offsets there are 152 equal and 467 different bytes in 29 runs each;
  patch has a 19-byte suffix `[619,638)`.
- Verification: the worker ran the script twice and `cmp` matched; root reran it to
  `local/validation/data02-bridge-sxd-01-root.log`, which matched `run1.log`. The JSON and all three
  stdout logs hash to `0340b13e1c0bc1f4baf1ae2cb1194a23bce23b57edbfc85447adee024eaf90b3`. Exact
  commands are recorded in the task checkpoint. No member bytes or reports entered Git.
- Inference/limit: two changed versions do not prove record boundaries, field meanings, codec,
  reader behavior, patch semantics or runtime use. One report-construction defect was found and
  corrected before the repeatable final report; count 1 failed reporting try, 0 failed schema
  hypotheses. DATA-02 remains ACTIVE; selected-scene asset resolution is not established.
- Exact next action: continue the CPU slot-10 static analysis. For a future schema slice, identify
  one independently named cachedFiles `.sxd` with the same observed prefix and validate its exact
  manifest locator/hash before making a three-way offset comparison.

### Parallel dispatch checkpoint — 2026-10-19 23:19 CDT

See [the parallel claims dispatch checkpoint](docs/checkpoints/2026-10-19-root-parallel-claims-dispatched.md).
Both earlier claims were pushed before assignment; workers own distinct paths and do not edit shared
docs or commit/push. RE-02-DYNLINK-01 is fixture verified and published in `b2a5b6f`;
CPU-01-CTOR-PREFIX-01 is fixture verified and pushed in `b26768a`. The metadata claim above is
fixture verified and pushed in `fc1b278`. The bridge-member comparison is artifact verified;
CPU slot-10 static analysis remains assigned and active, with root retaining integration and
publication ownership.

### Current publication checkpoint

At 2026-10-19 23:08 CDT, APP-01 commit `8b10c09` was pushed to `origin main`. Before commit,
`python3 scripts/project_index.py --check` reported 4,123 folders current, `git diff --check`
passed, `python3 -I scripts/audit_publish_paths.py` passed the 8 staged paths,
`python3 -I scripts/audit_publish_paths.py --all` passed all 566 tracked paths, and
`git diff --cached --check` passed. `git fetch origin main` found no divergence before commit.
AUD-01-XFF-01 was then recorded before implementation and published in `b17ffe2` at 23:10 CDT.
Before that commit, `python3 scripts/project_index.py --check` reported 4,123 folders current,
`git diff --check`, staged `python3 -I scripts/audit_publish_paths.py` (5 paths),
`python3 -I scripts/audit_publish_paths.py --all` (567 paths), and `git diff --cached --check`
passed. `git fetch origin main` found no divergence. The publication record was pushed in
`a323bf3`. `/root/audio_asset_survey` owns the two claimed scripts files and does not modify shared
docs or commit/push. Its cross-check of all XFF length words passed before parser implementation.
At 23:15 CDT, root independently reran the XFF unit and live-pair commands (both exit 0), refreshed
the project index after the private verification run directory made it stale (4,126 folders; check
current), and confirmed `graft check` and diff whitespace checks. The corresponding private logs
are under `local/validation/20261019-2315-root-aud01-xff-verification/`. The source/evidence batch
was pushed as `7b2747b` (`feat: add bounded XFF bank header reader`); before commit, the staged
path audit passed 8 paths, the full tracked-path audit passed 569 paths, staged whitespace check
passed, and `git fetch origin main` plus `git rev-list --left-right --count origin/main...HEAD`
showed no divergence (`0 0`).
The RE-02-DYNLINK-01 fixture was pushed in `b2a5b6f` (`test: cover synthetic PS4 dynlink import
relocations`); its staged path audit passed 5 files, the full tracked-path audit passed 572 paths,
and `git fetch origin main` plus the divergence count showed `0 0`. The follow-up publication
record was pushed in `205630b`; its 2 staged docs paths passed the staged audit, `--all` passed
572 tracked paths, and the fetched remote was even with HEAD.

## PRIOR CHECKPOINT — 2026-10-19 22:41 CDT, /root — CPU-01d `0xce7300` metadata correction — FIXTURE_VERIFIED

Root completed the bounded provenance/manifest correction in `native/core/vec_ce7300.hpp`,
`native/tests/test_vec_ce7300.cpp`, and `tools/lift/check_fce7300.py`. Claim evidence is preserved
in [the ce7300 claim checkpoint](docs/checkpoints/2026-10-19-root-ce7300-label-claim.md); build,
test, checker and Graft results are in [the completion checkpoint](docs/checkpoints/2026-10-19-root-ce7300-label-complete.md).

- Live Graft wiring shows `init_vec_ce7300` registered in `startup_ctors.cpp` at survey position
  149. The private constructor survey also identifies RVA `0xce7300` at list position 149, while
  `ctor-vec-copies.json` identifies it at vector-copy manifest position 91.
- The header attribution now matches original lift worker `bc-effcf5dc`; the checker verifies both
  list positions and no longer calls position 91 a constructor-survey index. Function behavior and
  startup registration were unchanged. Independent review found no blockers. Failed tries: 0.
- Exact validation: `cmake --build local/build/native --target test_vec_ce7300`,
  `ctest --test-dir local/build/native -R '^vec_ce7300$' --output-on-failure`, and
  `python3 -I tools/lift/check_fce7300.py` all passed. Graft was rebuilt after source edits and its
  freshness check passed; the deep/meaning layer remains unbuilt. G1 remains NOT PASSED.
- Next exact action: select one bounded, unblocked native recovery task from CPU-01/SRV-01, verify
  its address/hash and dependencies, then claim it before editing.

## PRIOR CHECKPOINT — 2026-10-19 22:39 CDT, /root — CPU-01d `0xce7300` metadata correction — ACTIVE CLAIM

The initial claim and read-only evidence are recorded in [the ce7300 claim checkpoint](docs/checkpoints/2026-10-19-root-ce7300-label-claim.md).

## PRIOR CHECKPOINT — 2026-10-19 22:37 CDT, /root — SRV-01 + CPU-01d fixture batch — FIXTURE_VERIFIED

Root reviewed and integrated three isolated, fixture-only slices. The complete command/evidence
record is in [the native fixture batch checkpoint](docs/checkpoints/2026-10-19-root-native-fixture-batch.md).

- SRV-01 `FUN_00019e00`, block `0x1a123–0x1a15a`: raw-offset helper, exact and short span checks,
  sanitizer CMake target, and E28 byte/instruction checker pass. One earlier failed function-target
  hypothesis remains in the count. No allocator runtime wiring; meanings of `+0x80/+0x88/+0x90`
  remain unknown.
- CPU-01d RVA `0xce57b0` (survey position 153) and `0xce0890` (position 155): both native leaves,
  separate test targets, and ELF hash-bound checkers pass. Both source/destination cell pairs are
  distinct. Reviewers found no blockers; a stale survey comment for `0xce57b0` is explicitly not
  used as evidence because the checker decodes its signed displacements from the hash-pinned ELF.
- `python3 scripts/project_index.py` indexed 4,114 folders. Graft was rebuilt with
  `scripts`, `tools`, `reference`, `native`, `launcher`, and `tests`; the three new symbols are
  queryable and `graft check` passes. The meaning/deep layer is still unbuilt.

These are fixture/code checkpoints only: no startup-ctor registration, engine boot, actual game
audio, real save/load, or G1 evidence. G2–G6 remain NOT RUN. Pre-existing unstaged
`0xce7300` edits are preserved and excluded from this batch. Root owns any follow-up cleanup.

## ACTIVE CLAIM — 2026-10-19 22:20 CDT, worker bc-73721c8d (CPU-01d) — 0xce67b0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xce67b0** (survey index 151).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_ce67b0.hpp/cpp, native/tests/test_vec_ce67b0.cpp,
tools/lift/check_fce67b0.py, native/CMakeLists.txt (+vec_ce67b0 only; origin/main base).
Private dump (not published): local/re/work/cce67b0.bin.
Did NOT edit: startup_ctors.*, Drive, SRV. Coordinator HANDOFF STOP after this mid-push finish.
Skipped: 95; busy 0xce8440/0xce74a0; peer CLAIMED 0xce7300. Prior: 0xceb070 = 38760a3.
Checkpoint (2026-10-19 22:20 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x12f8790 dst=0x3351cd0 bits=~0,~0,~0,0; index 151; distinct cells.
- Commands: ctest -R '^vec_ce67b0$' Passed; python3 -I tools/lift/check_fce67b0.py OK.
- Logs: /tmp/ctest_ce67b0.log, /tmp/check_ce67b0.log.
- Store: Project internal/worker-claim-ce67b0.md.
- Push: (this commit). STOP — no new RVA claims.


## ACTIVE CLAIM — 2026-10-19 22:19 CDT, worker bc-9aa0fe88 (CPU-01d) — 0xce8440 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xce8440** (survey index 147).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_ce8440.{hpp,cpp}`, `native/tests/test_vec_ce8440.cpp`,
`tools/lift/check_fce8440.py`, `native/CMakeLists.txt` (+vec_ce8440 only; origin/main base).
Private dump (not published): `local/re/work/cce8440.bin`.
Prior: 0xceb2c0=`dbcff4f`. Skipped: 95; busy peers. HANDOFF STOP after this push.
Did NOT edit Drive / SRV / `startup_ctors.*` / peer untracked; no `git reset --hard`.
Checkpoint (2026-10-19 22:19 CDT, bc-9aa0fe88):
- OBS: unguarded vmovups copy; src=0x12f9100 dst=0x3351d10 bits=~0,~0,~0,0; index 147; distinct cells.
- Commands: ctest -R '^vec_ce8440$' Passed; `python3 -I tools/lift/check_fce8440.py` OK.
- Logs: `/tmp/ctest_ce8440.log`, `/tmp/check_fce8440.log`.
- Store: Project `internal/worker-claim-ce8440.md`.
- Push: (this commit). STOP.

## ACTIVE CLAIM — 2026-10-19 22:18 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xce7300 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 149 | 0xce7300 | 5e5a6d0 |
Gap 147 0xce8440 still open. Base eea8da6 (101) → **102** slots.
Owned: startup_ctors.*, test_startup_ctors.cpp, HANDOFF.md. CMake tip as-is.
Store: Project internal/worker-claim-drive.md + internal/drive-wip/.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:18 CDT, worker bc-81763e53 (CPU-01d) — 0xce6fb0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xce6fb0** (survey index 150).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_ce6fb0.{hpp,cpp}`, `native/tests/test_vec_ce6fb0.cpp`,
`tools/lift/check_fce6fb0.py`, `native/CMakeLists.txt` (+vec_ce6fb0 only; origin/main base).
Private dump (not published): `local/re/work/cce6fb0.bin`.
Skipped: 95; busy/peer `0xce7300`=`5e5a6d0` / `0xce8440`; prior D `0xce74a0`=`8067002`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:18 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12f89a0 dst=0x3351ce0 bits=~0,~0,~0,0; index 150; distinct cells.
- Commands: ctest -R '^vec_ce6fb0$' Passed; `python3 -I tools/lift/check_fce6fb0.py` OK.
- Logs: `/tmp/ctest_ce6fb0.log`, `/tmp/check_fce6fb0.log`.
- Store: Project `internal/worker-claim-ce6fb0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:18 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xceb070/+0xce99c0/+0xce74a0 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 145 | 0xceb070 | 38760a3 |
| 146 | 0xce99c0 | 088c1bc |
| 148 | 0xce74a0 | 8067002 |
Gap 147 0xce8440. Waiting 0xce7300.
Base 2956f01 (98) → **101** slots.
Owned: startup_ctors.*, test_startup_ctors.cpp, HANDOFF.md. CMake tip as-is.
Store: Project internal/worker-claim-drive.md + internal/drive-wip/.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:16 CDT, worker bc-effcf5dc (CPU-01d) — 0xce7300 — COMPLETE

Task: CPU-01d — lift **0xce7300** (ctor-survey.json index **91**). FIXTURE_VERIFIED.
Owned: vec_ce7300 + check + CMake (+only) + HANDOFF. No startup_ctors. Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:17 CDT, worker bc-81763e53 (CPU-01d) — 0xce74a0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xce74a0** (survey index 148).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_ce74a0.{hpp,cpp}`, `native/tests/test_vec_ce74a0.cpp`,
`tools/lift/check_fce74a0.py`, `native/CMakeLists.txt` (+vec_ce74a0 only; origin/main base).
Private dump (not published): `local/re/work/cce74a0.bin`.
Skipped: 95; busy `0xce8440`; prior D `0xce99c0`=`088c1bc`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:17 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12f8d90 dst=0x3351d00 bits=~0,~0,~0,0; index 148; distinct cells.
- Commands: ctest -R '^vec_ce74a0$' Passed; `python3 -I tools/lift/check_fce74a0.py` OK.
- Logs: `/tmp/ctest_ce74a0.log`, `/tmp/check_fce74a0.log`.
- Store: Project `internal/worker-claim-ce74a0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:17 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xceb2c0/+0xceb1c0 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 143 | 0xceb2c0 | dbcff4f (E already on tip) |
| 144 | 0xceb1c0 | 05f0326 |
Base 2d779fe (96) → **98** slots.
Owned: startup_ctors.*, test_startup_ctors.cpp, HANDOFF.md. CMake tip as-is.
Store: Project internal/worker-claim-drive.md + internal/drive-wip/.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:16 CDT, worker bc-81763e53 (CPU-01d) — 0xce99c0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xce99c0** (survey index 146).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_ce99c0.{hpp,cpp}`, `native/tests/test_vec_ce99c0.cpp`,
`tools/lift/check_fce99c0.py`, `native/CMakeLists.txt` (+vec_ce99c0 only; origin/main base).
Private dump (not published): `local/re/work/cce99c0.bin`.
Skipped: 95; busy peers `0xceb2c0`/`0xceb070`; prior D `0xceb1c0`=`05f0326`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:16 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12f9370 dst=0x3351d20 bits=~0,~0,~0,0; index 146; distinct cells.
- Commands: ctest -R '^vec_ce99c0$' Passed; `python3 -I tools/lift/check_fce99c0.py` OK.
- Logs: `/tmp/ctest_ce99c0.log`, `/tmp/check_fce99c0.log`.
- Store: Project `internal/worker-claim-ce99c0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:16 CDT, worker bc-73721c8d (CPU-01d) — 0xceb070 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xceb070** (survey index 145).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_ceb070.hpp/cpp, native/tests/test_vec_ceb070.cpp,
tools/lift/check_fceb070.py, native/CMakeLists.txt (+vec_ceb070 only; origin/main base).
Private dump (not published): local/re/work/cceb070.bin.
Did NOT edit: startup_ctors.*, Drive, SRV, peer WIP.
Skipped: 95; raced 0xceb890 (94794b4). Prior: 0xd04ce0 = 79b10b7 (Drive cb211c7).
Checkpoint (2026-10-19 22:16 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x12f95c0 dst=0x3351d30 bits=~0,~0,~0,0; index 145; distinct cells.
- Commands: ctest -R '^vec_ceb070$' Passed; python3 -I tools/lift/check_fceb070.py OK.
- Logs: /tmp/ctest_ceb070.log, /tmp/check_ceb070.log.
- Store: Project internal/worker-claim-ceb070.md.
- Push: (this commit).


## ACTIVE CLAIM — 2026-10-19 22:16 CDT, worker bc-9aa0fe88 (CPU-01d) — 0xceb2c0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xceb2c0** (survey index 143).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_ceb2c0.{hpp,cpp}`, `native/tests/test_vec_ceb2c0.cpp`,
`tools/lift/check_fceb2c0.py`, `native/CMakeLists.txt` (+vec_ceb2c0 only; origin/main base).
Private dump (not published): `local/re/work/cceb2c0.bin`.
Prior: 0xcfcc60=`036eaec`. Skipped: 95; busy 0xcee8a0/0xceb890.
Did NOT edit Drive / SRV / `startup_ctors.*` / peer untracked; no `git reset --hard`.
Checkpoint (2026-10-19 22:16 CDT, bc-9aa0fe88):
- OBS: unguarded vmovups copy; src=0x12f9990 dst=0x3351d50 bits=~0,~0,~0,0; index 143; distinct cells.
- Commands: ctest -R '^vec_ceb2c0$' Passed; `python3 -I tools/lift/check_fceb2c0.py` OK.
- Logs: `/tmp/ctest_ceb2c0.log`, `/tmp/check_fceb2c0.log`.
- Store: Project `internal/worker-claim-ceb2c0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:16 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xcee8a0/+0xceb890 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 141 | 0xcee8a0 | ef90adf (survey 141; B contested 137 ignored) |
| 142 | 0xceb890 | 94794b4 |
Base 4c866df (94) → **96** slots.
Owned: startup_ctors.*, test_startup_ctors.cpp, HANDOFF.md. CMake tip as-is.
Store: Project internal/worker-claim-drive.md + internal/drive-wip/.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:15 CDT, worker bc-81763e53 (CPU-01d) — 0xceb1c0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xceb1c0** (survey index 144).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_ceb1c0.{hpp,cpp}`, `native/tests/test_vec_ceb1c0.cpp`,
`tools/lift/check_fceb1c0.py`, `native/CMakeLists.txt` (+vec_ceb1c0 only; origin/main base).
Private dump (not published): `local/re/work/cceb1c0.bin`.
Skipped: 95; busy `0xceb2c0`; prior D `0xceb890`=`94794b4`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:15 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12f97d0 dst=0x3351d40 bits=~0,~0,~0,0; index 144; distinct cells.
- Commands: ctest -R '^vec_ceb1c0$' Passed; `python3 -I tools/lift/check_fceb1c0.py` OK.
- Logs: `/tmp/ctest_ceb1c0.log`, `/tmp/check_fceb1c0.log`.
- Store: Project `internal/worker-claim-ceb1c0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:15 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xcfcc60/+0xcfcbc0/+0xcf44a0 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 138 | 0xcfcc60 | 036eaec |
| 139 | 0xcfcbc0 | e1784fd / HANDOFF 564f63f |
| 140 | 0xcf44a0 | 52f309f |
Base cb211c7/52f309f (91) → **94** slots.
Owned: startup_ctors.*, test_startup_ctors.cpp, HANDOFF.md. CMake tip as-is.
Store: Project internal/worker-claim-drive.md + internal/drive-wip/.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:15 CDT, worker bc-81763e53 (CPU-01d) — 0xceb890 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xceb890** (survey index 142).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_ceb890.{hpp,cpp}`, `native/tests/test_vec_ceb890.cpp`,
`tools/lift/check_fceb890.py`, `native/CMakeLists.txt` (+vec_ceb890 only; origin/main base).
Private dump (not published): `local/re/work/cceb890.bin`.
Skipped: 95; busy `0xcee8a0`; prior D `0xcf44a0`=`52f309f`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:15 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12f9b60 dst=0x3351d60 bits=~0,~0,~0,0; index 142; distinct cells.
- Commands: ctest -R '^vec_ceb890$' Passed; `python3 -I tools/lift/check_fceb890.py` OK.
- Logs: `/tmp/ctest_ceb890.log`, `/tmp/check_fceb890.log`.
- Store: Project `internal/worker-claim-ceb890.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:14 CDT, worker bc-02397584 (CPU-01d) — 0xcee8a0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xcee8a0** (survey index 137).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_cee8a0.{hpp,cpp}`, `native/tests/test_vec_cee8a0.cpp`,
`tools/lift/check_fcee8a0.py`, `native/CMakeLists.txt` (+vec_cee8a0 only; origin/main base).
Private dump (not published): `local/re/work/ccee8a0.bin`.
Skipped: 95; busy `0xd04ce0`; prior `0xd06dd0`=`a621f1c`/`754fbe4`. Never touched `startup_ctors.*`.
Checkpoint (2026-10-19 22:14 CDT, bc-02397584):
- OBS: unguarded vmovups; src=0x12f9ec0 dst=0x3351d70 bits=~0,~0,~0,0; index 137.
- Commands: ctest -R '^vec_cee8a0$' Passed; check_fcee8a0.py OK.
- Logs: `/tmp/ctest_cee8a0.log`, `/tmp/check_fcee8a0.log`.
- Store: Project `internal/worker-claim-cee8a0.md`.
- Push: (this commit).


## ACTIVE CLAIM — 2026-10-19 22:14 CDT, worker bc-81763e53 (CPU-01d) — 0xcf44a0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xcf44a0** (survey index 140).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_cf44a0.{hpp,cpp}`, `native/tests/test_vec_cf44a0.cpp`,
`tools/lift/check_fcf44a0.py`, `native/CMakeLists.txt` (+vec_cf44a0 only; origin/main base).
Private dump (not published): `local/re/work/ccf44a0.bin`.
Skipped: 95; busy peers; prior D `0xcfcbc0`=`e1784fd`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:14 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12fa4e0 dst=0x3351d80 bits=~0,~0,~0,0; index 140; distinct cells.
- Commands: ctest -R '^vec_cf44a0$' Passed; `python3 -I tools/lift/check_fcf44a0.py` OK.
- Logs: `/tmp/ctest_cf44a0.log`, `/tmp/check_fcf44a0.log`.
- Store: Project `internal/worker-claim-cf44a0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:13 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xd04ce0 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 136 | 0xd04ce0 | 79b10b7 |
| 137 | 0xd00600 | f05f359 (already on tip f980148) |
Base f980148 (90) → **91** slots; 134-137 continuous.
Owned: startup_ctors.*, test_startup_ctors.cpp, HANDOFF.md. CMake tip as-is.
Store: Project internal/worker-claim-drive.md + internal/drive-wip/.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:13 CDT, worker bc-81763e53 (CPU-01d) — 0xcfcbc0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xcfcbc0** (survey index 139).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_cfcbc0.{hpp,cpp}`, `native/tests/test_vec_cfcbc0.cpp`,
`tools/lift/check_fcfcbc0.py`, `native/CMakeLists.txt` (+vec_cfcbc0 only; origin/main base).
Private dump (not published): `local/re/work/ccfcbc0.bin`.
Skipped: 95; busy peers; prior D `0xd00600`=`f05f359`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint: ctest `vec_cfcbc0` Passed; `check_fcfcbc0.py` OK. Push: `e1784fd`.

## ACTIVE CLAIM — 2026-10-19 22:13 CDT, worker bc-73721c8d (CPU-01d) — 0xd04ce0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd04ce0** (survey index 136).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d04ce0.hpp/cpp, native/tests/test_vec_d04ce0.cpp,
tools/lift/check_fd04ce0.py, native/CMakeLists.txt (+vec_d04ce0 only; origin/main base).
Private dump (not published): local/re/work/cd04ce0.bin.
Did NOT edit: startup_ctors.*, Drive, SRV, peer WIP.
Skipped: 95; busy peers through 0xd00600/0xd06dd0/0xd08240. Prior: 0xd19930 = 0b1eef8 / bb02b62.
Checkpoint (2026-10-19 22:13 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x12fc350 dst=0x3351dc0 bits=~0,~0,~0,0; index 136; distinct cells.
- Commands: ctest -R '^vec_d04ce0$' Passed; python3 -I tools/lift/check_fd04ce0.py OK.
- Logs: /tmp/ctest_d04ce0.log, /tmp/check_d04ce0.log.
- Store: Project internal/worker-claim-d04ce0.md.
- Push: (this commit).


## ACTIVE CLAIM — 2026-10-19 22:13 CDT, worker bc-81763e53 (CPU-01d) — 0xcfcbc0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xcfcbc0** (survey index 139).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_cfcbc0.{hpp,cpp}`, `native/tests/test_vec_cfcbc0.cpp`,
`tools/lift/check_fcfcbc0.py`, `native/CMakeLists.txt` (+vec_cfcbc0 only; origin/main base).
Private dump (not published): `local/re/work/ccfcbc0.bin`.
Skipped: 95; busy peers `0xd04ce0`/`0xcfcc60`; prior D `0xd00600`=`f05f359`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:13 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12fa850 dst=0x3351d90 bits=~0,~0,~0,0; index 139; distinct cells.
- Commands: ctest -R '^vec_cfcbc0$' Passed; `python3 -I tools/lift/check_fcfcbc0.py` OK.
- Logs: `/tmp/ctest_cfcbc0.log`, `/tmp/check_fcfcbc0.log`.
- Store: Project `internal/worker-claim-cfcbc0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:12 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xd08240/+0xd06dd0 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 134 | 0xd08240 | 07ab9da |
| 135 | 0xd06dd0 | a621f1c |
Waiting: 136 0xd04ce0 (sources present; not CMake-linked on tip).
Base 4fd2af6 (87) → **89** slots.
Owned: startup_ctors.*, test_startup_ctors.cpp, HANDOFF.md. CMake tip as-is.
Store: Project internal/worker-claim-drive.md + internal/drive-wip/.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:12 CDT, worker bc-81763e53 (CPU-01d) — 0xd00600 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd00600** (survey index 137).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d00600.{hpp,cpp}`, `native/tests/test_vec_d00600.cpp`,
`tools/lift/check_fd00600.py`, `native/CMakeLists.txt` (+vec_d00600 only; origin/main base).
Private dump (not published): `local/re/work/cd00600.bin`.
Skipped: 95; busy peers; prior D `0xd06dd0`=`a621f1c`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:12 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12fb8a0 dst=0x3351db0 bits=~0,~0,~0,0; index 137; distinct cells.
- Commands: ctest -R '^vec_d00600$' Passed; `python3 -I tools/lift/check_fd00600.py` OK.
- Logs: `/tmp/ctest_d00600.log`, `/tmp/check_fd00600.log`.
- Store: Project `internal/worker-claim-d00600.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:11 CDT, worker bc-201701f2 (Drive / CPU-01e) — backlog 126-133 continuous — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 126 | 0xd19930 | 0b1eef8 |
| 127 | 0xd17c10 | 9ecf79f |
| 128 | 0xd13300 | dc80e0c / 3b33a0f |
| 129 | 0xd0bde0 | df0b694 |
| 130 | 0xd0bcd0 | 4d474c6 / fb9850e |
| 131 | 0xd0a250 | 8d1eb32 |
| 132 | 0xd09160 | d727b89 |
| 133 | 0xd082e0 | ecbbb14 |
Base 4fdb764 (85) → **87** slots (126-133 continuous).
Owned: startup_ctors.*, test_startup_ctors.cpp, HANDOFF.md. CMake tip as-is.
Store: Project internal/worker-claim-drive.md + internal/drive-wip/.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:11 CDT, worker bc-201701f2 (Drive / CPU-01e) — backlog +0xd17c10/+0xd082e0 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 126 | `0xd19930` | `0b1eef8` (tip) |
| 127 | `0xd17c10` | `9ecf79f` |
| 128 | `0xd13300` | `dc80e0c` (tip) |
| 129 | `0xd0bde0` | `df0b694` (tip) |
| 131 | `0xd0a250` | `8d1eb32` (tip) |
| 133 | `0xd082e0` | `ecbbb14` |
Gaps 130 `0xd0bcd0`, 132 `0xd09160`. Base `f693a60` (83) → **85** slots.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake tip as-is.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:11 CDT, worker bc-81763e53 (CPU-01d) — 0xd06dd0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd06dd0** (survey index 135).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d06dd0.{hpp,cpp}`, `native/tests/test_vec_d06dd0.cpp`,
`tools/lift/check_fd06dd0.py`, `native/CMakeLists.txt` (+vec_d06dd0 only; origin/main base).
Private dump (not published): `local/re/work/cd06dd0.bin`.
Skipped: 95; busy peers; prior D `0xd082e0`=`ecbbb14`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:11 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12fc9b0 dst=0x3351dd0 bits=~0,~0,~0,0; index 135; distinct cells.
- Commands: ctest -R '^vec_d06dd0$' Passed; `python3 -I tools/lift/check_fd06dd0.py` OK.
- Logs: `/tmp/ctest_d06dd0.log`, `/tmp/check_fd06dd0.log`.
- Store: Project `internal/worker-claim-d06dd0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:10 CDT, worker bc-effcf5dc (CPU-01d) — 0xd08240 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd08240** (ctor-survey.json index **76**).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d08240.{hpp,cpp}`, `native/tests/test_vec_d08240.cpp`,
`tools/lift/check_fd08240.py`, `native/CMakeLists.txt` (+vec_d08240 only; origin/main base).
Private dump (not published): `local/re/work/cd08240.bin`.
Prior: 74/`0xd09160` `d727b89`, 71/`0xd0bde0` `df0b694`. Skipped 95; busy 75 `0xd082e0`.
Did NOT edit `startup_ctors.*`. No `git reset --hard`.
Checkpoint: ctest/check OK. Store: `internal/worker-claim-d08240.md`. Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:10 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xd0bde0/+0xd0a250 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 126 | `0xd19930` | `0b1eef8` (already on tip `fde6b97`) |
| 128 | `0xd13300` | `dc80e0c` (already on tip `fde6b97`) |
| 129 | `0xd0bde0` | `df0b694` |
| 131 | `0xd0a250` | `8d1eb32` |
Gaps 127 `0xd17c10`, 130 `0xd0bcd0`. Base `fde6b97` (81) → **83** slots.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake tip as-is.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:09 CDT, worker bc-02397584 (CPU-01d) — 0xd0bcd0 — COMPLETE

Task: CPU-01d — re-link CMake for unguarded vec-copy ctor RVA **0xd0bcd0** (survey index 126).
Failed tries: 0. Status: FIXTURE_VERIFIED; sources `4d474c6`; CMake lines restored.
Owned this push: `native/CMakeLists.txt` (+vec_d0bcd0 only), `HANDOFF.md`.
Never touched `startup_ctors.*`. Peer untracked left alone.
Checkpoint (2026-10-19 22:09 CDT, bc-02397584):
- OBS: src=0x12fd870 dst=0x3351e20 bits=~0,~0,~0,0; index 126.
- ctest/check OK. Store: `internal/worker-claim-d0bcd0.md`.
- Push: (this commit).


## ACTIVE CLAIM — 2026-10-19 22:10 CDT, worker bc-81763e53 (CPU-01d) — 0xd082e0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd082e0** (survey index 133).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d082e0.{hpp,cpp}`, `native/tests/test_vec_d082e0.cpp`,
`tools/lift/check_fd082e0.py`, `native/CMakeLists.txt` (+vec_d082e0 only; origin/main base).
Private dump (not published): `local/re/work/cd082e0.bin`.
Skipped: 95; busy peers; prior D `0xd0a250`=`8d1eb32`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:10 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12fd050 dst=0x3351df0 bits=~0,~0,~0,0; index 133; distinct cells.
- Commands: ctest -R '^vec_d082e0$' Passed; `python3 -I tools/lift/check_fd082e0.py` OK.
- Logs: `/tmp/ctest_d082e0.log`, `/tmp/check_fd082e0.log`.
- Store: Project `internal/worker-claim-d082e0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:09 CDT, worker bc-effcf5dc (CPU-01d) — 0xd09160 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd09160** (ctor-survey.json index **74**).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d09160.{hpp,cpp}`, `native/tests/test_vec_d09160.cpp`,
`tools/lift/check_fd09160.py`, `native/CMakeLists.txt` (+vec_d09160 only; origin/main base).
Private dump (not published): `local/re/work/cd09160.bin`.
Prior: 71/`0xd0bde0` `df0b694`, 67/`0xd19af0` `40c8c64`, 116 `4beae8e`.
Skipped 95 + busy peers. Did NOT edit `startup_ctors.*`. No `git reset --hard`.
Checkpoint: ctest/check OK. Store: `internal/worker-claim-d09160.md`. Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:09 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xd19930/+0xd13300 — COMPLETE

Task: CPU-01e — register (ctor-survey.json; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 126 | `0xd19930` | `0b1eef8` / HANDOFF `bb02b62` |
| 128 | `0xd13300` | `dc80e0c` / cmake `3b33a0f` |
Gap 127 `0xd17c10`. Base `124ebae` (79) → **81** slots.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake tip as-is.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:09 CDT, worker bc-81763e53 (CPU-01d) — 0xd0a250 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd0a250** (survey index 131).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d0a250.{hpp,cpp}`, `native/tests/test_vec_d0a250.cpp`,
`tools/lift/check_fd0a250.py`, `native/CMakeLists.txt` (+vec_d0a250 only; origin/main base).
Private dump (not published): `local/re/work/cd0a250.bin`.
Skipped: 95; busy peers; prior D `0xd13300`=`dc80e0c`/`3b33a0f`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 22:09 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x12fd3b0 dst=0x3351e10 bits=~0,~0,~0,0; index 131; distinct cells.
- Commands: ctest -R '^vec_d0a250$' Passed; `python3 -I tools/lift/check_fd0a250.py` OK.
- Logs: `/tmp/ctest_d0a250.log`, `/tmp/check_fd0a250.log`.
- Store: Project `internal/worker-claim-d0a250.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:09 CDT, worker bc-9aa0fe88 (CPU-01d) — 0xd17c10 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd17c10** (survey index 127).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d17c10.{hpp,cpp}`, `native/tests/test_vec_d17c10.cpp`,
`tools/lift/check_fd17c10.py`, `native/CMakeLists.txt` (+vec_d17c10 only; origin/main base).
Private dump (not published): `local/re/work/cd17c10.bin`.
Prior: 0xd2c7e0 = `fc277e0`. Skipped: 95; busy peers `0xd13300`/`0xd0bde0`/`0xd0bcd0`.
Did NOT edit Drive / SRV / `startup_ctors.*` / peer untracked; no `git reset --hard`.
Checkpoint (2026-10-19 22:09 CDT, bc-9aa0fe88):
- OBS: unguarded vmovups copy; src=0x12fe4b0 dst=0x3351e50 bits=~0,~0,~0,0; index 127; distinct cells.
- Commands: ctest -R '^vec_d17c10$' Passed; `python3 -I tools/lift/check_fd17c10.py` OK.
- Logs: `/tmp/ctest_d17c10.log`, `/tmp/check_fd17c10.log`.
- Store: Project `internal/worker-claim-d17c10.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:08 CDT, worker bc-73721c8d (CPU-01d) — 0xd19930 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd19930** (survey index 126).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d19930.hpp/cpp, native/tests/test_vec_d19930.cpp,
tools/lift/check_fd19930.py; CMake already lists vec_d19930 on tip after sources.
Private dump (not published): local/re/work/cd19930.bin.
Did NOT edit: startup_ctors.*, Drive, SRV, peer d17c10 / d13300 / arena_*.
Skipped: 95; busy 0xd2c7e0 / 0xd20590. Prior: 0xd2cb30 = 75e6e00 / 31e50cb / 1ba6e4f.
Checkpoint (2026-10-19 22:08 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x12fe8e0 dst=0x3351e60 bits=~0,~0,~0,0; index 126; distinct cells.
- Commands: ctest -R '^vec_d19930$' Passed; python3 -I tools/lift/check_fd19930.py OK.
- Logs: /tmp/ctest_d19930.log, /tmp/check_d19930.log.
- Store: Project internal/worker-claim-d19930.md.
- Push: 0b1eef8 on origin/main (sources); CMake present on tip.


## ACTIVE CLAIM — 2026-10-19 22:08 CDT, worker bc-effcf5dc (CPU-01d) — 0xd0bde0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd0bde0** (ctor-survey.json index **71**).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d0bde0.{hpp,cpp}`, `native/tests/test_vec_d0bde0.cpp`,
`tools/lift/check_fd0bde0.py`, `native/CMakeLists.txt` (+vec_d0bde0 only; origin/main base).
Private dump (not published): `local/re/work/cd0bde0.bin`.
Prior: 99 `e568b51`, 101 `cac53b7`, 116 `4beae8e`, 67/`0xd19af0` `40c8c64`.
Skipped 95 + busy peers. Did NOT edit `startup_ctors.*` / Drive / SRV / peer WIP / `local/`.
No `git reset --hard`.
Checkpoint (2026-10-19 22:08 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x12fda30 dst=0x3351e30 bits=~0,~0,~0,0; ctor-survey index 71.
- Commands: ctest -R '^vec_d0bde0$' Passed; `python3 -I tools/lift/check_fd0bde0.py` OK.
- Logs: `/tmp/ctest_d0bde0.log`, `/tmp/check_fd0bde0.log`.
- Store: Project `internal/worker-claim-d0bde0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:08 CDT, worker bc-81763e53 (CPU-01d) — 0xd13300 CMake-link — COMPLETE

Task: CPU-01d — CMake-link `vec_d13300` after sources landed in `dc80e0c` (survey index 128).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/CMakeLists.txt` (+vec_d13300 only; origin/main base), `HANDOFF.md`.
Prior sources push: `dc80e0c`. Did NOT edit Drive / SRV / `startup_ctors.*`.
Checkpoint: ctest `vec_d13300` Passed; `check_fd13300.py` OK. Store: `internal/worker-claim-d13300.md`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:08 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry 122–125 — COMPLETE

Task: CPU-01e — one-push register peer leaves (ctor-survey.json indices; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 122 | `0xd2c7e0` | `fc277e0` |
| 123 | `0xd20590` | `288001d` |
| 124 | `0xd19bb0` | `1177ca3` / cmake `2f557c0` |
| 125 | `0xd19af0` | `40c8c64` |
Base `574d844` (75) → **79** slots. Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`.
CMake tip as-is. No mid-flight tip restore after edit.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:07 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry 122–125 — COMPLETE

Task: CPU-01e — one-push register peer leaves (ctor-survey.json indices; skip 95):
| idx | RVA | provenance |
|---|---|---|
| 122 | `0xd2c7e0` | `fc277e0` |
| 123 | `0xd20590` | `288001d` (cmake on tip) |
| 124 | `0xd19bb0` | `1177ca3` / cmake `2f557c0` |
| 125 | `0xd19af0` | `40c8c64` |
Base `574d844` (75) → **79** slots. Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`.
CMake tip as-is. No mid-flight tip restore. Peer pause window.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:06 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xd20590 — COMPLETE

Task: CPU-01e — register `0xd20590` (ctor-survey.json idx **123**, peer D `288001d`) into `startup_ctors`.
Base registry `574d844` (75). CMake +`vec_d20590`. Skip 95. Gap 122 `0xd2c7e0`.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `native/CMakeLists.txt` (+vec_d20590), `HANDOFF.md`.
Registry size **76**. Evidence: ctest `startup_ctors` Passed; `check_fd20590.py` OK.
No mid-flight tip restore of registry after edit. Left peer untracked WIP unstaged.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:06 CDT, worker bc-02397584 (CPU-01d) — 0xd19bb0 — COMPLETE

Task: CPU-01d — unguarded vec-copy ctor RVA **0xd19bb0** (survey index 120).
Failed tries: 0. Status: FIXTURE_VERIFIED (`1177ca3`); re-restore peer `vec_d20590` CMake after later leaf rebuild.
Owned this push: `native/CMakeLists.txt` (+`vec_d20590`), `HANDOFF.md`.
Never touched `startup_ctors.*`.
Checkpoint (2026-10-19 22:06 CDT, bc-02397584):
- OBS: src=0x12fedc0 dst=0x3351e80; index 120; ctest/check OK.
- Store: `internal/worker-claim-d19bb0.md`. Push: (this commit).


## ACTIVE CLAIM — 2026-10-19 22:05 CDT, worker bc-9aa0fe88 (CPU-01d) — 0xd2c7e0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd2c7e0** (survey index 122).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d2c7e0.{hpp,cpp}`, `native/tests/test_vec_d2c7e0.cpp`,
`tools/lift/check_fd2c7e0.py`, `native/CMakeLists.txt` (+vec_d2c7e0 only; origin/main base).
Private dump (not published): `local/re/work/cd2c7e0.bin`.
Skipped: 95; busy/COMPLETE through `0xd19af0`/`0xd19930`; next claim `0xd17c10`.
Did NOT edit Drive / SRV / `startup_ctors.*` / peer untracked; no `git reset --hard`.
Checkpoint (2026-10-19 22:05 CDT, bc-9aa0fe88):
- OBS: unguarded vmovups copy; src=0x12ff9e0 dst=0x3351ea0 bits=~0,~0,~0,0; index 122; distinct cells.
- Commands: ctest -R '^vec_d2c7e0$' Passed; `python3 -I tools/lift/check_fd2c7e0.py` OK.
- Logs: `/tmp/ctest_d2c7e0.log`, `/tmp/check_fd2c7e0.log`.
- Store: Project `internal/worker-claim-d2c7e0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:05 CDT, worker bc-effcf5dc (CPU-01d) — 0xd19af0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd19af0** (ctor-survey.json index **67**).
Note: Drive registry idx 121 is already `0xd2cb30` — this leaf uses ctor-survey.json index 67, not 121.
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d19af0.{hpp,cpp}`, `native/tests/test_vec_d19af0.cpp`,
`tools/lift/check_fd19af0.py`, `native/CMakeLists.txt` (+vec_d19af0 only; origin/main base).
Private dump (not published): `local/re/work/cd19af0.bin`.
Prior: 99 `e568b51`, 101 `cac53b7`, 116 `4beae8e`. Skipped 95; busy `0xd2c7e0`/`0xd20590`/`0xd19bb0`.
Did NOT edit Drive / SRV / `startup_ctors.*` / peer WIP / `local/`. No `git reset --hard`.
Checkpoint (2026-10-19 22:05 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x12fec00 dst=0x3351e70 bits=~0,~0,~0,0; ctor-survey index 67.
- Commands: ctest -R '^vec_d19af0$' Passed; `python3 -I tools/lift/check_fd19af0.py` OK.
- Logs: `/tmp/ctest_d19af0.log`, `/tmp/check_fd19af0.log`.
- Store: Project `internal/worker-claim-d19af0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:03 CDT, worker bc-02397584 (CPU-01d) — 0xd19bb0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd19bb0** (survey index 120).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d19bb0.{hpp,cpp}`, `native/tests/test_vec_d19bb0.cpp`,
`tools/lift/check_fd19bb0.py`, `native/CMakeLists.txt` (+vec_d19bb0 only; origin/main base).
Private dump (not published): `local/re/work/cd19bb0.bin`.
Skipped: 95; busy `0xd2c7e0`/`0xd20590`; prior 115 `3ffc82f`. Never touched `startup_ctors.*`.
Did NOT edit Drive / SRV / peer untracked; no `git reset --hard` / broad stash.
Checkpoint (2026-10-19 22:03 CDT, bc-02397584):
- OBS: unguarded vmovups; src=0x12fedc0 dst=0x3351e80 bits=~0,~0,~0,0; index 120; distinct cells.
- Commands: ctest -R '^vec_d19bb0$' Passed; `python3 -I tools/lift/check_fd19bb0.py` OK.
- Logs: `/tmp/ctest_d19bb0.log`, `/tmp/check_fd19bb0.log`.
- Store: Project `internal/worker-claim-d19bb0.md`.
- Push: (this commit).


## ACTIVE CLAIM — 2026-10-19 22:04 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry 114/115/116/118/121 — COMPLETE

Task: CPU-01e — one-push register peer leaves into `startup_ctors` (skip 95):
| idx | RVA | provenance |
|---|---|---|
| 114 | `0xd3a6d0` | `4463268` |
| 115 | `0xd32720` | `3ffc82f` (re-index from prior 119) |
| 116 | `0xd2ffe0` | `4beae8e` |
| 118 | `0xd32b70` | keep (`f7d1ec0` / `2a8a42a`) |
| 121 | `0xd2cb30` | `75e6e00` / cmake `1ba6e4f` |
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake tip as-is.
Registry size **75**. Gaps 117, 119–120 visible. No mid-flight tip restore.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:03 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xd3a6d0/+0xd32720 — COMPLETE

Task: CPU-01e — register `0xd3a6d0` (survey **114**, `4463268`) and `0xd32720` (survey **119**, `3ffc82f`) into `startup_ctors`.
Base `f7d1ec0` (71) → **73** slots. Skip 95. Gaps 115–117 remain.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake tip as-is.
Evidence: ctest `startup_ctors`; `check_fd3a6d0.py`; `check_fd32720.py`.
Did NOT stage peer untracked WIP. Restored uncommitted edits from Drive WIP backup only (not tip).
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:02 CDT, worker bc-73721c8d (CPU-01d) — 0xd2cb30 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd2cb30** (survey index 121).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d2cb30.hpp/cpp, native/tests/test_vec_d2cb30.cpp,
tools/lift/check_fd2cb30.py, native/CMakeLists.txt (+vec_d2cb30 only; origin/main base).
Private dump (not published): local/re/work/cd2cb30.bin.
Did NOT edit: startup_ctors.*, Drive, SRV, peer d32720 / d2ffe0 / arena_*.
Skipped: 95; busy 0xd3a6d0 / 0xd32720 / 0xd2ffe0. Prior: 0xd32b70 = 2a8a42a / 3a0f362 / 3ae7e3a.
Checkpoint (2026-10-19 22:02 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x13007c0 dst=0x3351eb0 bits=~0,~0,~0,0; index 121; distinct cells.
- Commands: ctest -R vec_d2cb30 Passed; python3 -I tools/lift/check_fd2cb30.py OK.
- Logs: /tmp/ctest_d2cb30.log, /tmp/check_d2cb30.log.
- Store: Project internal/worker-claim-d2cb30.md.
- Push: 75e6e00 on origin/main.

## ACTIVE CLAIM — 2026-10-19 22:01 CDT, worker bc-02397584 (CPU-01d) — 0xd32720 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd32720** (survey index 115).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d32720.{hpp,cpp}`, `native/tests/test_vec_d32720.cpp`,
`tools/lift/check_fd32720.py`, `native/CMakeLists.txt` (+vec_d32720 only; origin/main base).
Private dump (not published): `local/re/work/cd32720.bin`.
Skipped: 95; busy `0xd3a6d0`/`0xd32b70`; prior 103 `0604100`. Never touched `startup_ctors.*`.
Did NOT edit Drive / SRV / peer untracked; no `git reset --hard` / broad stash.
Checkpoint (2026-10-19 22:01 CDT, bc-02397584):
- OBS: unguarded vmovups; src=0x1300ba0 dst=0x3351ed0 bits=~0,~0,~0,0; index 115; distinct cells.
- Commands: ctest -R '^vec_d32720$' Passed; `python3 -I tools/lift/check_fd32720.py` OK.
- Logs: `/tmp/ctest_d32720.log`, `/tmp/check_fd32720.log`.
- Store: Project `internal/worker-claim-d32720.md`.
- Push: (this commit).


## ACTIVE CLAIM — 2026-10-19 22:01 CDT, worker bc-effcf5dc (CPU-01d) — 0xd2ffe0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd2ffe0** (survey index 116).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d2ffe0.{hpp,cpp}`, `native/tests/test_vec_d2ffe0.cpp`,
`tools/lift/check_fd2ffe0.py`, `native/CMakeLists.txt` (+vec_d2ffe0 only; origin/main base).
Private dump (not published): `local/re/work/cd2ffe0.bin`.
Prior: 99 `e568b51`, 101 `cac53b7`. Skipped 95; 113–115 busy (D/peer/B).
Did NOT edit Drive / SRV / `startup_ctors.*` / peer WIP / `local/`. No `git reset --hard`.
Checkpoint (2026-10-19 22:01 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x13009a0 dst=0x3351ec0 bits=~0,~0,~0,0; index 116.
- Commands: ctest -R '^vec_d2ffe0$' Passed; `python3 -I tools/lift/check_fd2ffe0.py` OK.
- Logs: `/tmp/ctest_d2ffe0.log`, `/tmp/check_fd2ffe0.log`.
- Store: Project `internal/worker-claim-d2ffe0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 22:00 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry +0xd32b70 — COMPLETE

Task: CPU-01e — register `0xd32b70` (survey idx **118**, `2a8a42a`/`3ae7e3a`) into `startup_ctors`.
103 `0xd623c0` already in batch `8c0c7cf`. Skip 95. Failed tries: 0.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake tip as-is (+vec_d32b70).
Registry size **71**. Evidence: ctest `startup_ctors` Passed; `check_fd32b70.py` OK.
Did NOT restore `startup_ctors.*` from tip mid-flight; left peer untracked WIP unstaged.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:59 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry batch 97–113 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED vec-copy ctors into `startup_ctors` by RVA
(survey index from `local/re/work/ctor-survey.json`; skip contested leaf header indices).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted Drive push **`8c0c7cf`**.
Wired (skip 95 `0xd71f60`, gap 105 `0xd5a280`):
97 `0xd6dbf0`, 98 `0xd6d890`, 99 `0xd6d280`=`e568b51`, 100 `0xd6a520`,
101 `0xd62b00`=`cac53b7`, 102 `0xd62690`, 103 `0xd623c0`, 104 `0xd5fbe0`,
106 `0xd47880`, 107 `0xd47420`, 108 `0xd47270`, 109 `0xd42b60`,
110 `0xd40ea0`, 111 `0xd405a0`=`21491b6`, 112 `0xd40010`=`627d251`/`61393e8`,
113 `0xd3b790`=`c9d9731`.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake tip as-is.
Registry size **70**. Evidence: ctest `startup_ctors` Passed; check_fd6d280/fd62b00/fd40010/fd3b790/fd405a0/fd6d890 OK.
Store: Project `internal/worker-claim-drive.md` + `internal/drive-wip/`.
Push: **`8c0c7cf`** on origin/main.

## ACTIVE CLAIM — 2026-10-19 21:59 CDT, worker bc-81763e53 (CPU-01d) — 0xd3a6d0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd3a6d0** (survey index 113).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d3a6d0.{hpp,cpp}`, `native/tests/test_vec_d3a6d0.cpp`,
`tools/lift/check_fd3a6d0.py`, `native/CMakeLists.txt` (+vec_d3a6d0 only; origin/main base).
Private dump (not published): `local/re/work/cd3a6d0.bin`.
Skipped: 95; D `0xd3b790`=`c9d9731`; peer `0xd32b70`=`2a8a42a`; busy peers.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 21:59 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x13016d0 dst=0x3352090 bits=~0,~0,~0,0; index 113; distinct cells.
- Commands: ctest -R '^vec_d3a6d0$' Passed; `python3 -I tools/lift/check_fd3a6d0.py` OK.
- Logs: `/tmp/ctest_d3a6d0.log`, `/tmp/check_fd3a6d0.log`.
- Store: Project `internal/worker-claim-d3a6d0.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:59 CDT, worker bc-73721c8d (CPU-01d) — 0xd32b70 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd32b70** (survey index 118).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d32b70.hpp/cpp, native/tests/test_vec_d32b70.cpp,
tools/lift/check_fd32b70.py, native/CMakeLists.txt (+vec_d32b70 only; origin/main base).
Private dump (not published): local/re/work/cd32b70.bin.
Did NOT edit: startup_ctors, Drive, SRV, peer d3a6d0 / arena_*.
Skipped: 95; D 0xd3b790 (c9d9731); busy 114 0xd3a6d0. Prior: 0xd40010 = 627d251 / 61393e8.
Checkpoint (2026-10-19 21:59 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x1300d30 dst=0x3351ee0 bits=~0,~0,~0,0; index 118; distinct cells.
- Commands: ctest -R vec_d32b70 Passed; python3 -I tools/lift/check_fd32b70.py OK.
- Logs: /tmp/ctest_d32b70.log, /tmp/check_d32b70.log.
- Store: Project internal/worker-claim-d32b70.md.
- Push: 2a8a42a on origin/main.

## ACTIVE CLAIM — 2026-10-19 21:57 CDT, worker bc-02397584 (CPU-01d) — 0xd623c0 — COMPLETE

Task: CPU-01d — unguarded vec-copy ctor RVA **0xd623c0** (survey index 103).
Failed tries: 0. Status: FIXTURE_VERIFIED on origin; restore peer `vec_d62b00` lib line dropped in `ac233cb`.
Owned this push: `native/CMakeLists.txt` (+`vec_d62b00` restore + keep `vec_d623c0`), `HANDOFF.md`.
Sources: `0604100` (`vec_d623c0.*`, `check_fd623c0.py`).
Private dump (not published): `local/re/work/cd623c0.bin`.
Did NOT touch Drive / SRV / peer untracked WIP.
Checkpoint (2026-10-19 21:57 CDT, bc-02397584):
- OBS: src=0x1303e90 dst=0x3352940 bits=~0,~0,~0,0; index 103.
- Commands: ctest -R '^vec_d623c0$' Passed; check_fd623c0.py OK.
- Store: Project `internal/worker-claim-d623c0.md`.
- Push: sources **`9dfacf2`**; CMake follow-up this commit (wire `arena_page_debit`; restore peer `vec_d40010` link accidentally dropped). Repair: re-add `core/vec_d62b00.cpp` to sotc_core.

## ACTIVE CLAIM — 2026-10-19 21:56 CDT, worker bc-effcf5dc (CPU-01d) — 0xd62b00 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd62b00** (survey index 101).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d62b00.{hpp,cpp}`, `native/tests/test_vec_d62b00.cpp`,
`tools/lift/check_fd62b00.py`, `native/CMakeLists.txt` (+vec_d62b00 source; test already on origin).
Private dump (not published): `local/re/work/cd62b00.bin`.
Prior: idx 99 `0xd6d280` = `e568b51` on origin.
Skipped: 95; D 100/102; B 103; peer busy leaves. No `git reset --hard`.
Did NOT edit Drive / SRV / peer untracked WIP / `local/`.
Checkpoint (2026-10-19 21:56 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x1304610 dst=0x3352960 bits=~0,~0,~0,0; index 101.
- Commands: ctest -R '^vec_d62b00$' Passed; `python3 -I tools/lift/check_fd62b00.py` OK.
- Logs: `/tmp/ctest_d62b00.log`, `/tmp/check_fd62b00.log`.
- Store: Project `internal/worker-claim-d62b00.md`.
- Push: (this commit).


## ACTIVE CLAIM — 2026-10-19 21:56 CDT, worker bc-81763e53 (CPU-01d) — 0xd3b790 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd3b790** (survey index 112).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d3b790.{hpp,cpp}`, `native/tests/test_vec_d3b790.cpp`,
`tools/lift/check_fd3b790.py`, `native/CMakeLists.txt` (+vec_d3b790 only; origin/main base).
Private dump (not published): `local/re/work/cd3b790.bin`.
Skipped: 95; A `0xd40010` (busy); D `0xd405a0`=`21491b6`; busy C/B peers.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 21:56 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1301a50 dst=0x33520a0 bits=~0,~0,~0,0; index 112; distinct cells.
- Commands: ctest -R '^vec_d3b790$' Passed; `python3 -I tools/lift/check_fd3b790.py` OK.
- Logs: `/tmp/ctest_d3b790.log`, `/tmp/check_fd3b790.log`.
- Store: Project `internal/worker-claim-d3b790.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:55 CDT, worker bc-9aa0fe88 (SRV-01 mem page-debit) — COMPLETE

Task: SRV-01 mem — free-path **`[arena+0x70]` page debit** (`0x1ae67`) + idle-pool insert helpers (`0x1aecd`; reuses `list_insert_at_head`).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/arena_page_debit.{hpp,cpp}`, `native/tests/test_arena_page_debit.cpp`,
`tools/lift/check_f1ae67_page_debit.py`, `native/CMakeLists.txt` (+arena_page_debit only),
docs POINTERS/README/TASKS, `HANDOFF.md`.
**Kept:** host_map/carve/split/list/free-count/high-water/exact-fit.
**Did NOT:** `0x19660`, page-map tables `0x1ab08`, root `arena_accounting`, Drive/`startup_ctors`, peer vec_*.
Store: Project `internal/worker-claim-srv01-page-debit.md`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:57 CDT, worker bc-73721c8d (CPU-01d) — 0xd40010 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd40010** (survey index 112).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d40010.hpp/cpp, native/tests/test_vec_d40010.cpp,
tools/lift/check_fd40010.py, native/CMakeLists.txt (+vec_d40010 only; origin/main base).
Private dump (not published): local/re/work/cd40010.bin.
Did NOT edit: startup_ctors, Drive, SRV, peer arena_* / vec_d405a0.
Skipped: 95; owned 111 0xd405a0. Prior: 0xd40ea0 = 567f70c / c73820f.
Checkpoint (2026-10-19 21:57 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x1301e30 dst=0x33520b0 bits=~0,~0,~0,0; index 112; distinct cells.
- Commands: ctest -R vec_d40010 Passed; python3 -I tools/lift/check_fd40010.py OK.
- Logs: /tmp/ctest_d40010.log, /tmp/check_d40010.log.
- Store: Project internal/worker-claim-d40010.md.
- Push: 627d251 on origin/main.

## ACTIVE CLAIM — 2026-10-19 21:55 CDT, worker bc-02397584 (CPU-01d) — 0xd623c0 — COMPLETE

Task: CPU-01d — re-link CMake for unguarded vec-copy ctor RVA **0xd623c0** (survey index 103).
Failed tries: 0. Status: FIXTURE_VERIFIED; sources already on main (`0604100`); CMake lines restored.
Owned: `native/CMakeLists.txt` (+vec_d623c0 only; origin/main base), `HANDOFF.md`.
Sources already published: `native/core/vec_d623c0.{hpp,cpp}`, `native/tests/test_vec_d623c0.cpp`,
`tools/lift/check_fd623c0.py`.
Private dump (not published): `local/re/work/cd623c0.bin`.
Skipped: 95 escalated; peer WIP left untracked.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/` / peer untracked files.
Checkpoint (2026-10-19 21:55 CDT, bc-02397584):
- OBS: unguarded vmovups; src=0x1303e90 dst=0x3352940 bits=~0,~0,~0,0; index 103.
- Commands: ctest -R '^vec_d623c0$' Passed; `python3 -I tools/lift/check_fd623c0.py` OK.
- Logs: `/tmp/ctest_d623c0.log`, `/tmp/check_fd623c0.log`.
- Store: Project `internal/worker-claim-d623c0.md`.
- Push: (this commit) — CMake/HANDOFF only.


## ACTIVE CLAIM — 2026-10-19 21:55 CDT, worker bc-73721c8d (CPU-01d) — 0xd40ea0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd40ea0** (survey index 110).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d40ea0.hpp/cpp, native/tests/test_vec_d40ea0.cpp,
tools/lift/check_fd40ea0.py, native/CMakeLists.txt (+vec_d40ea0 only; origin/main base).
Private dump (not published): local/re/work/cd40ea0.bin.
Did NOT edit: startup_ctors, Drive, SRV, peer vec_d42b60 / arena_*.
Skipped: 95; busy 109 0xd42b60. Prior: 0xd47270 = eec9fbd / 4600f87.
Checkpoint (2026-10-19 21:55 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x13023d0 dst=0x33520d0 bits=~0,~0,~0,0; index 110; distinct cells.
- Commands: ctest -R vec_d40ea0 Passed; python3 -I tools/lift/check_fd40ea0.py OK.
- Logs: /tmp/ctest_d40ea0.log, /tmp/check_d40ea0.log.
- Store: Project internal/worker-claim-d40ea0.md.
- Push: 567f70c on origin/main.

## ACTIVE CLAIM — 2026-10-19 21:55 CDT, worker bc-effcf5dc (CPU-01d) — 0xd6d280 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd6d280** (survey index 99).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d6d280.{hpp,cpp}`, `native/tests/test_vec_d6d280.cpp`,
`tools/lift/check_fd6d280.py`, `native/CMakeLists.txt` (+vec_d6d280 only; origin/main base).
Private dump (not published): `local/re/work/cd6d280.bin`.
Skipped: idx 95; D 100/102; B 103; peer busy leaves. No `git reset --hard`.
Did NOT edit Drive / SRV / `startup_ctors.*` / peer untracked WIP / `local/`.
B abandoned idx 99 for C; finished from retained local WIP.
Checkpoint (2026-10-19 21:55 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x1304cf0 dst=0x3352980 bits=~0,~0,~0,0; index 99.
- Commands: ctest -R '^vec_d6d280$' Passed; `python3 -I tools/lift/check_fd6d280.py` OK.
- Logs: `/tmp/ctest_d6d280.log`, `/tmp/check_fd6d280.log`.
- Store: Project `internal/worker-claim-d6d280.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:54 CDT, worker bc-81763e53 (CPU-01d) — 0xd42b60 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd42b60** (survey index 108).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d42b60.{hpp,cpp}`, `native/tests/test_vec_d42b60.cpp`,
`tools/lift/check_fd42b60.py`, `native/CMakeLists.txt` (+vec_d42b60 only; origin/main base).
Private dump (not published): `local/re/work/cd42b60.bin`.
Skipped: 95; A `0xd47270`=`eec9fbd`; D `0xd47420`=`64b1662`; busy C/B peers.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 21:54 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1302650 dst=0x33520e0 bits=~0,~0,~0,0; index 108; distinct cells.
- Commands: ctest -R '^vec_d42b60$' Passed; `python3 -I tools/lift/check_fd42b60.py` OK.
- Logs: `/tmp/ctest_d42b60.log`, `/tmp/check_fd42b60.log`.
- Store: Project `internal/worker-claim-d42b60.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:52 CDT, worker bc-9aa0fe88 (SRV-01 mem exact-fit) — COMPLETE

Task: SRV-01 mem — exact-fit consume finalize OBS **0x1aada..0x1aaf3** (size/flags/sentinel; skips `0x19660`).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/exact_fit_finalize.{hpp,cpp}`, `native/tests/test_exact_fit_finalize.cpp`,
`tools/lift/check_f1aada_exact_fit.py`, `native/CMakeLists.txt` (+exact_fit_finalize only),
`docs/POINTERS.md`, `native/README.md`, `docs/TASKS.md`, `HANDOFF.md`.
**Kept:** host_map / carve / split / list / free-count / high-water.
**Did NOT:** `0x19660`, page-map `0x1ab08`, root `arena_accounting`, peer vec_*, Drive.
Checkpoint: ctest 6/6; `check_f1aada_exact_fit.py` OK. Store: `internal/worker-claim-srv01-exact-fit.md`.
Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:53 CDT, worker bc-73721c8d (CPU-01d) — 0xd47270 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd47270** (survey index 108).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d47270.hpp/cpp, native/tests/test_vec_d47270.cpp,
tools/lift/check_fd47270.py, native/CMakeLists.txt (+vec_d47270 only; origin/main base).
Private dump (not published): local/re/work/cd47270.bin.
Did NOT edit: startup_ctors, Drive, SRV, peer vec_d47420 / arena_* / exact_fit_*.
Skipped: 95; busy 107 0xd47420. Prior: 0xd47880 = ee12add / 28c9083.
Checkpoint (2026-10-19 21:53 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x1302a20 dst=0x33520f0 bits=~0,~0,~0,0; index 108; distinct cells.
- Commands: ctest -R vec_d47270 Passed; python3 -I tools/lift/check_fd47270.py OK.
- Logs: /tmp/ctest_d47270.log, /tmp/check_d47270.log.
- Store: Project internal/worker-claim-d47270.md.
- Push: eec9fbd on origin/main.

## ACTIVE CLAIM — 2026-10-19 21:53 CDT, worker bc-81763e53 (CPU-01d) — 0xd47420 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd47420** (survey index 106).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d47420.{hpp,cpp}`, `native/tests/test_vec_d47420.cpp`,
`tools/lift/check_fd47420.py`, `native/CMakeLists.txt` (+vec_d47420 only; origin/main base).
Private dump (not published): `local/re/work/cd47420.bin`.
Skipped: 95; peer 102 `6dc3218`; A 105 `0xd47880`=`ee12add`/`28c9083`; done 104.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 21:53 CDT, bc-81763e53):
- OBS: unguarded vmovups; src=0x1302bc0 dst=0x3352100 bits=~0,~0,~0,0; index 106.
- Commands: ctest -R '^vec_d47420$' Passed; `python3 -I tools/lift/check_fd47420.py` OK.
- Store: Project `internal/worker-claim-d47420.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:51 CDT, worker bc-73721c8d (CPU-01d) — 0xd47880 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd47880** (survey index 106).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d47880.hpp/cpp, native/tests/test_vec_d47880.cpp,
tools/lift/check_fd47880.py, native/CMakeLists.txt (+vec_d47880 only; origin/main base).
Private dump (not published): local/re/work/cd47880.bin.
Did NOT edit: startup_ctors, Drive, SRV, peer busy leaves / arena_*.
Skipped: 95; 99 0xd6d280; 101 0xd62b00; 102 0xd62690; 103 0xd623c0; 104 0xd5fbe0.
Checkpoint (2026-10-19 21:51 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x1302d80 dst=0x3352110 bits=~0,~0,~0,0; index 106; distinct cells.
- Commands: ctest -R vec_d47880 Passed; python3 -I tools/lift/check_fd47880.py OK.
- Logs: /tmp/ctest_d47880.log, /tmp/check_d47880.log.
- Store: Project internal/worker-claim-d47880.md.
- Push: ee12add on origin/main.

## ACTIVE CLAIM — 2026-10-19 21:51 CDT, worker bc-81763e53 (CPU-01d) — 0xd5fbe0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd5fbe0** (survey index 104).
Failed tries: 0. Status: FIXTURE_VERIFIED; sources on origin; CMake/HANDOFF this checkpoint.
Owned: `native/core/vec_d5fbe0.{hpp,cpp}`, `native/tests/test_vec_d5fbe0.cpp`,
`tools/lift/check_fd5fbe0.py`, `native/CMakeLists.txt` (+vec_d5fbe0).
Private dump (not published): `local/re/work/cd5fbe0.bin`.
Skipped: 95; B 99; C 101; 102 `0xd62690` (`6dc3218`); B 103; done 100 `38aea70`.
Checkpoint (2026-10-19 21:51 CDT, bc-81763e53):
- OBS: unguarded vmovups; src=0x1303920 dst=0x3352930 bits=~0,~0,~0,0; index 104.
- Commands: ctest -R '^vec_d5fbe0$' Passed; `python3 -I tools/lift/check_fd5fbe0.py` OK.
- Store: Project `internal/worker-claim-d5fbe0.md`.
- Push: sources **`8d12f1e`**; CMake/HANDOFF (this commit).

## ACTIVE CLAIM — 2026-10-19 21:50 CDT, worker bc-73721c8d (CPU-01d) — 0xd62690 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd62690** (survey index 102).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: native/core/vec_d62690.hpp/cpp, native/tests/test_vec_d62690.cpp,
tools/lift/check_fd62690.py, native/CMakeLists.txt (+vec_d62690 only; origin/main base).
Private dump (not published): local/re/work/cd62690.bin.
Did NOT edit: startup_ctors, Drive, SRV, peer vec_d6d280 / vec_d62b00 / arena_*.
Skipped: 95 0xd71f60 NEEDS_PRINCIPAL; B 99 0xd6d280; D 100 0xd6a520; C 101 0xd62b00.
Checkpoint (2026-10-19 21:50 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x1304380 dst=0x3352950 bits=~0,~0,~0,0; index 102; distinct cells.
- Commands: ctest -R vec_d62690 Passed; python3 -I tools/lift/check_fd62690.py OK.
- Logs: /tmp/ctest_d62690.log, /tmp/check_d62690.log.
- Store: Project internal/worker-claim-d62690.md.
- Push: 6dc3218 on origin/main.

## ACTIVE CLAIM — 2026-10-19 21:49 CDT, worker bc-81763e53 (CPU-01d) — 0xd62690 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd62690** (survey index 102).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d62690.{hpp,cpp}`, `native/tests/test_vec_d62690.cpp`,
`tools/lift/check_fd62690.py`, `native/CMakeLists.txt` (+vec_d62690 only; origin/main base).
Private dump (not published): `local/re/work/cd62690.bin`.
Skipped: 95 escalated; A 97; C 99/101; done 98/100 (`adaa4dd`/`38aea70`).
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 21:49 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1304380 dst=0x3352950 bits=~0,~0,~0,0; index 102; distinct cells.
- Commands: ctest -R '^vec_d62690$' Passed; `python3 -I tools/lift/check_fd62690.py` OK.
- Logs: `/tmp/ctest_d62690.log`, `/tmp/check_fd62690.log`.
- Store: Project `internal/worker-claim-d62690.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:49 CDT, worker bc-81763e53 (CPU-01d) — 0xd6a520 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd6a520** (survey index 100).
Failed tries: 0. Status: FIXTURE_VERIFIED; sources+CMake on origin.
Owned: `native/core/vec_d6a520.{hpp,cpp}`, `native/tests/test_vec_d6a520.cpp`,
`tools/lift/check_fd6a520.py`, `native/CMakeLists.txt` (+vec_d6a520).
Private dump (not published): `local/re/work/cd6a520.bin`.
Skipped: 95 `0xd71f60`; A 97 `0xd6dbf0`; C 99 `0xd6d280`; done 98 `adaa4dd`.
Checkpoint (2026-10-19 21:49 CDT, bc-81763e53):
- OBS: unguarded vmovups; src=0x1304990 dst=0x3352970 bits=~0,~0,~0,0; index 100.
- Commands: ctest -R '^vec_d6a520$' Passed; `python3 -I tools/lift/check_fd6a520.py` OK.
- Store: Project `internal/worker-claim-d6a520.md`.
- Push: **`38aea70`** on origin/main.

## ACTIVE CLAIM — 2026-10-19 21:45 CDT, worker bc-9aa0fe88 (SRV-01 mem high-water) — COMPLETE

Task: SRV-01 mem — arena used-byte high-water OBS **0x1a9bb..0x1a9fc** (`+0x80/+0x90` + peaks `+0x88/+0x98`).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/arena_high_water.{hpp,cpp}`, `native/tests/test_arena_high_water.cpp`,
`tools/lift/check_f1a9bb_high_water.py`, `native/CMakeLists.txt` (+arena_high_water only; origin/main base),
`docs/POINTERS.md`, `native/README.md`, `docs/TASKS.md`, `HANDOFF.md`.
**Kept:** host_map / free_list_carve / region_split / region_list / free-count.
**Did NOT edit:** `arena_accounting.*` (root `0x1a123` slice), `allocator_region.*`, Drive, peer vec_*, `0x19660`.
Page bitmap: OPEN. Distinct from root arena_accounting mode-2 block.
Checkpoint (2026-10-19 21:45 CDT, bc-9aa0fe88):
- OBS: used90+=rbx / peak98; used80+=rbx / peak88; unsigned jbe; precedes call `0x19660` @`0x1aa0f`.
- Commands: ctest 5/5 Passed; `python3 -I tools/lift/check_f1a9bb_high_water.py` OK.
- Logs: `/tmp/ctest_hw.log`, `/tmp/check_hw.log`.
- Store: Project `internal/worker-claim-srv01-high-water.md`.
- Push: **`1206f62`** on origin/main (sources+checker); docs follow-up this commit.

## ACTIVE CLAIM — 2026-10-19 21:48 CDT, worker bc-81763e53 (CPU-01d) — 0xd6a520 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd6a520** (survey index 100).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d6a520.{hpp,cpp}`, `native/tests/test_vec_d6a520.cpp`,
`tools/lift/check_fd6a520.py`, `native/CMakeLists.txt` (+vec_d6a520 only; origin/main base).
Private dump (not published): `local/re/work/cd6a520.bin`.
Skipped: 95 `0xd71f60` (escalated); A 97 `0xd6dbf0` (`ed2a3dc`); C 99 `0xd6d280`; done 98 `adaa4dd`.
Did NOT edit Drive / SRV / `startup_ctors.*` / peer `arena_*` / `local/`.
Checkpoint (2026-10-19 21:48 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1304990 dst=0x3352970 bits=~0,~0,~0,0; index 100; distinct cells.
- Commands: ctest -R '^vec_d6a520$' Passed; `python3 -I tools/lift/check_fd6a520.py` OK.
- Logs: `/tmp/ctest_d6a520.log`, `/tmp/check_fd6a520.log`.
- Store: Project `internal/worker-claim-d6a520.md`.
- Push: code **`1206f62`**; HANDOFF **`ee062e1`**; docs this commit.

## ACTIVE CLAIM — 2026-10-19 21:47 CDT, worker bc-81763e53 (CPU-01d) — 0xd6d890 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd6d890** (survey index 98).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d6d890.{hpp,cpp}`, `native/tests/test_vec_d6d890.cpp`,
`tools/lift/check_fd6d890.py`, `native/CMakeLists.txt` (+vec_d6d890 only; origin/main base).
Private dump (not published): `local/re/work/cd6d890.bin`.
Skipped: idx 95 `0xd71f60` (NEEDS_PRINCIPAL); 96 `0xd6e0f0` (done); 97 `0xd6dbf0` (Survey A).
Did NOT edit Drive / SRV / `startup_ctors.*` / peer `arena_*` / `local/`.
Checkpoint (2026-10-19 21:47 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x13050b0 dst=0x3352990 bits=~0,~0,~0,0; index 98; distinct cells.
- Commands: ctest -R '^vec_d6d890$' Passed; `python3 -I tools/lift/check_fd6d890.py` OK.
- Logs: `/tmp/ctest_d6d890.log`, `/tmp/check_fd6d890.log`.
- Store: Project `internal/worker-claim-d6d890.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 21:46 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 67/94/96 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED `0xdd41e0` (67), `0xd7b930` (94), `0xd6e0f0` (96)
into `startup_ctors`. 94=`56e4a68`+CMake; 96 on tip; 67 cmake-linked (stale gap cleared).
Skipped gap 10–19, idx 50, idx 95 `0xd71f60` NEEDS_PRINCIPAL, SRV.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest OK; check_fdd41e0/fd7b930/fd6e0f0 OK; registry **54**.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake: origin as-is.
Did NOT edit: peer leaf sources / SRV WIP (`arena_accounting*`).
Store: Project `internal/worker-claim-drive.md`.

## ACTIVE CLAIM — 2026-10-19 21:45 CDT, worker bc-81763e53 (CPU-01d) — 0xd6e0f0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd6e0f0** (survey index 96).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d6e0f0.{hpp,cpp}`, `native/tests/test_vec_d6e0f0.cpp`,
`tools/lift/check_fd6e0f0.py`, `native/CMakeLists.txt` (+vec_d6e0f0 only; origin/main base).
Private dump (not published): `local/re/work/cd6e0f0.bin`.
Skipped: idx 95 `0xd71f60` (NEEDS_PRINCIPAL / escalated); peers `0xd7b930`/`0xd7bc60`/`0xd7c380`.
Did NOT edit Drive / SRV / `startup_ctors.*` / `local/`.
Checkpoint (2026-10-19 21:45 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1305460 dst=0x33529b0 bits=~0,~0,~0,0; index 96; distinct cells.
- Commands: ctest -R '^vec_d6e0f0$' Passed; `python3 -I tools/lift/check_fd6e0f0.py` OK.
- Logs: `/tmp/ctest_d6e0f0.log`, `/tmp/check_fd6e0f0.log`.
- Store: Project `internal/worker-claim-d6e0f0.md`.
- Push: (this commit).

## PRIOR CHECKPOINT — 2026-10-19 22:28 CDT, /root — SRV-01 arena accounting — ACTIVE

The 22:28 claim and repo-state record are archived in [the SRV-01 checkpoint](docs/checkpoints/2026-10-19-root-srv01-accounting.md).

## PRIOR CHECKPOINT — 2026-10-19 21:23 CDT, root — SRV-01 arena accounting — ACTIVE

Claim and bounded plan: [SRV-01 arena-accounting checkpoint](docs/checkpoints/2026-10-19-root-srv01-accounting.md).
Ghidra confirmed that `FUN_00019e00` contains `0x1a123–0x1a15a`: qword add/store at `+0x80`,
unsigned greater-than update at `+0x88`, and mode-2 subtract at `+0x90`. The earlier `0x19ae0`
wrapper query remains failed try 1; target confirmation is try 2. No native code exists yet. Next
exact action: dispatch only the new raw-offset helper, focused test, and exact-byte checker to the
allocator survey worker; root will integrate CMake and docs after review.

## STOPPED CLAIM — 2026-10-19 21:12 CDT, root — CPU-01d leaf 95 — NEEDS_PRINCIPAL

Three-try escalation: [CPU-01d leaf 95](docs/escalations/CPU-01d-d71f60.md); [checkpoint](docs/checkpoints/2026-10-19-root-cpu01d-d71f60.md).
Ghidra 12.1.4 analysis/import succeeded for the exact ELF, but the target script returned
`function_found=false` at `0xd71f60`. No function body or native fixture was recovered. No fourth
try without a written principal reassessment.

## ACTIVE CLAIM — 2026-10-19 21:15 CDT, root (SRV-01 arena accounting) — `0x19e00` — ACTIVE

Task: recover only the arena accounting block at `0x1a123–0x1a15a` in inner allocator `0x19e00`
and add a fixture-level native helper for raw fields at `+0x80`, `+0x88`, and conditional `+0x90`,
without naming unknown fields or integrating them into allocator behavior. Claim uses the existing
Ghidra project for ELF `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Failed
tries: 1; try 2 confirmed the target block. The earlier `0x19ae0` query found only the wrapper.
Root owns `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/POINTERS.md`, `native/README.md`,
`native/CMakeLists.txt`, and the task checkpoint. Planned worker-owned files after Ghidra confirms
the block: `native/core/arena_accounting.{hpp,cpp}`, `native/tests/test_arena_accounting.cpp`, and
`tools/lift/check_f19e00_accounting.py`. Private exporter/output remain under `local/re/`.
Do not edit `allocator_region.*`, free-list/list/bitmap code, or `0x19660`; do not claim G1.

## CLAIM — 2026-10-19 21:03 CDT, root (CPU-01d leaf 95) — `0xd71f60` — NEEDS_PRINCIPAL

Task: recover the 17-byte unguarded vector-copy leaf at survey index 95, then add a native
fixture/test/checker if Ghidra confirms it. Exact ELF SHA256:
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
Failed tries: 3; no fourth try without a principal reassessment. Private analysis files:
`local/re/work/ghidra-scripts/ExportD71F60.java`, `local/re/evidence/ghidra-d71f60.txt`, and the
headless import/script logs. The target function was not defined at the requested address in the
imported Ghidra program. No native source was edited. `startup_ctors.*` remains out of scope.

## CURRENT CHECKPOINT — 2026-10-19 20:59 CDT, root — CPU-02 dispatcher behavior coverage — COMPLETE

Claim and complete record: [checkpoint](docs/checkpoints/2026-10-19-root-cpu02-dispatch-claim.md).
Claim checkpoint pushed as `2d237c2` and claim-publication note `5723b07` to `origin/main`.
Completion checkpoint `d5f6ff9` is pushed to `origin/main`.
Task slice: all 13 observed `0x66dec0` dispatch types now have byte-matched behavior coverage;
dispatcher worker owned only `native/tests/test_addr.cpp`; root integrated docs and handoff.
Failed tries: 0. Focused build/CTest and the 13-arm ELF checker passed. Current next exact action:
inspect the installed Ghidra headless options for a longer-deadline offline analysis of `0xd71f60`.

## ACTIVE CLAIM — 2026-10-19 20:55 CDT, root (CPU-02 supplemental) — `0x66dec0` all-arm behavior — COMPLETE (FIXTURE_VERIFIED)

Task: CPU-02 — exercise every observed dispatcher arm and ensure unsupported/no-op cases leave
object bytes unchanged. Prerequisite: E23, `check_f66dec0.py`, `check_f66dec0_dispatch.py`, and
`native/core/addr.*` already record the 13-arm map. Failed tries: 0 for this bounded test slice.
Owned: dispatcher worker `native/tests/test_addr.cpp`; root `docs/TASKS.md`, `docs/EVIDENCE.md`,
`HANDOFF.md`. No dispatcher implementation or type-name/layout claim is included.
Checkpoint: focused build passed; `addr` CTest 1/1 passed; dispatch checker confirmed all 13 arms
byte-exact; `git diff --check` passed. Log: `/tmp/cpu02_addr_dispatch_validation.log`.
The full task checkpoint is in `docs/checkpoints/2026-10-19-root-cpu02-dispatch-claim.md`.

## ACTIVE CLAIM — 2026-10-19 20:37 CDT, worker bc-02397584 (CPU-01d) — 0xd7b930 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd7b930** (survey index 94).
Failed tries: 0. Status: FIXTURE_VERIFIED; on origin/main.
Owned: `native/core/vec_d7b930.{hpp,cpp}`, `native/tests/test_vec_d7b930.cpp`,
`tools/lift/check_fd7b930.py`, `native/CMakeLists.txt` (+vec_d7b930 only; origin/main base).
Private dump (not published): `local/re/work/cd7b930.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xd7be60`/`0xd7bc60`/`0xd7c380`.
Checkpoint (2026-10-19 20:37 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x1306100 dst=0x33529d0 bits=~0,~0,~0,0; index 94; distinct cells.
- Commands: ctest -R '^vec_d7b930$' Passed; `python3 -I tools/lift/check_fd7b930.py` OK.
- Logs: `/tmp/ctest_d7b930.log`, `/tmp/check_fd7b930.log`.
- Store: Project `internal/worker-claim-d7b930.md`.
- Push: `56e4a68` on origin/main.


## ACTIVE CLAIM — 2026-10-19 20:36 CDT, worker bc-9aa0fe88 (SRV-01 mem free-count) — COMPLETE

Task: SRV-01 mem — arena free-count dword at **+0x48** (OBS with list surgery).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/region_list.{hpp,cpp}` (extend only), `native/tests/test_region_list.cpp`,
`tools/lift/check_f1a902_list.py`, `docs/POINTERS.md`, `native/README.md`, `docs/TASKS.md`, `HANDOFF.md`.
**Kept:** host_map / free_list_carve / region_split / region_list modules. **No CMake change.**
**Did NOT edit:** `allocator_region.*`, `startup_ctors.*`, Drive, peer vec_*, `0x19660`.
Page bitmap: OPEN (no `bt*` in `0x18000..0x22000`). High-water `@+0x80/+0x90` deferred to next agent.
Checkpoint (2026-10-19 20:36 CDT, bc-9aa0fe88):
- OBS: `inc [r15+0x48]` @`0x1a933`/`0x1a95d` (insert_before); `dec [r15+0x48]` @`0x1a993` (unlink).
- Commands: ctest -R '^(region_list|region_split|free_list_carve|host_map)$' 4/4 Passed;
  `python3 -I tools/lift/check_f1a902_list.py` OK.
- Logs: `/tmp/ctest_free_count.log`, `/tmp/check_f1a902_list.log`.
- Store: Project `internal/worker-claim-srv01-free-count.md`.
- Push: docs `8c2fb72`; code (this commit) — `region_list` free-count API landed.

## ACTIVE CLAIM — 2026-10-19 20:38 CDT, worker bc-73721c8d (CPU-01d) — 0xd71f60 — ABANDONED / STOPPED

Task: CPU-01d — **retract** false COMPLETE for RVA **0xd71f60** (survey index 95).
Failed tries: 1. Status: ABANDONED — coordinator STOP; sources never published.
OBS: prior HANDOFF `ad46d5a` incorrectly cited `ee04639` (Drive registry commit) as the lift;
`vec_d71f60.*` / `check_fd71f60.py` are **absent** from origin/main. Local fixture work discarded.
Owned leaf COMPLETE remains: **0xd7c380** = `be64c6d` / HANDOFF `e710a6a`.
Did NOT claim ≥95 further. Next agent resumes; leaf 95 is free again.
Store: Project `internal/worker-claim-d71f60.md` (STOPPED).

## ACTIVE CLAIM — 2026-10-19 20:37 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 90–93 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED `0xd7c4d0` (90), `0xd7c380` (91),
`0xd7be60` (92), `0xd7bc60` (93) into `startup_ctors`.
Also CMake-link missing `vec_d7c380` into `sotc_core` (tests were present; library line absent).
Provenance: 90=`79b9159`/`5adcc89`; 91=`be64c6d`; 92=`4a0705c`/`530987d`; 93=`4a026d6`.
Skipped 94 `0xd7b930` (not on origin with CMake). Gap 10–19, idx 50, SRV.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest OK; check_fd7c4d0/fd7c380/fd7be60/fd7bc60 OK; registry **51**.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `native/CMakeLists.txt` (+vec_d7c380 library only), `HANDOFF.md`.
Did NOT edit: peer leaf sources (left local `vec_d7b930` WIP unstaged), SRV/`region_list.*`.
Store: Project `internal/worker-claim-drive.md`.

## ACTIVE CLAIM — 2026-10-19 20:35 CDT, worker bc-81763e53 (CPU-01d) — 0xd7be60 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd7be60** (survey index 92).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d7be60.{hpp,cpp}`, `native/tests/test_vec_d7be60.cpp`,
`tools/lift/check_fd7be60.py`, `native/CMakeLists.txt` (+vec_d7be60 only; origin/main base).
Private dump (not published): `local/re/work/cd7be60.bin`.
Skipped: `0xd7c380` (A `be64c6d`/`e710a6a`), `0xd7bc60` (B/C `4a026d6`), `0xd7c4d0` (B).
Did NOT stage peer WIP (`vec_d7b930*`, `startup_ctors.*`).
Checkpoint (2026-10-19 20:35 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1306a00 dst=0x33529f0 bits=~0,~0,~0,0; index 92; distinct cells.
- Commands: ctest -R '^vec_d7be60$' Passed; `python3 -I tools/lift/check_fd7be60.py` OK.
- Logs: `/tmp/ctest_d7be60.log`, `/tmp/check_fd7be60.log`.
- Store: Project `internal/worker-claim-d7be60.md`.
- Push: (this commit).

## ACTIVE CLAIM — 2026-10-19 20:34 CDT, worker bc-effcf5dc (CPU-01d) — 0xd7bc60 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd7bc60** (survey index 93).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d7bc60.{hpp,cpp}`, `native/tests/test_vec_d7bc60.cpp`,
`tools/lift/check_fd7bc60.py`, `native/CMakeLists.txt` (+vec_d7bc60 only; origin/main base).
Private dump (not published): `local/re/work/cd7bc60.bin`.
Did NOT edit: `startup_ctors.*`, SRV, peer A/B/D leaves; did NOT touch `worker-claim-d7be60.md`.
Checkpoint (2026-10-19 20:34 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x1306860 dst=0x33529e0 bits=~0,~0,~0,0; index 93.
- Commands: ctest -R '^vec_d7bc60$' Passed; `python3 -I tools/lift/check_fd7bc60.py` OK.
- Store: `internal/worker-claim-d7bc60.md`.
- Push: (pending this commit).

## ACTIVE CLAIM — 2026-10-19 20:34 CDT, worker bc-73721c8d (CPU-01d) — 0xd7c380 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd7c380** (survey index 91).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d7c380.{hpp,cpp}`, `native/tests/test_vec_d7c380.cpp`,
`tools/lift/check_fd7c380.py`, `native/CMakeLists.txt` (+vec_d7c380 only; origin/main base).
Private dump (not published): `local/re/work/cd7c380.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xd7c4d0`/`0xd7be60`.
Skipped: `0xd7fe40` (C), `0xd7e640`/`0xd7fe90` (D), `0xd7c9d0`/`0xd7c4d0` (B).
Checkpoint (2026-10-19 20:34 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x1306b90 dst=0x3352a00 bits=~0,~0,~0,0; index 91; distinct cells.
- Commands: ctest -R '^vec_d7c380$' Passed; `python3 -I tools/lift/check_fd7c380.py` OK.
- Logs: `/tmp/ctest_d7c380.log`, `/tmp/check_d7c380.log`.
- Store: Project `internal/worker-claim-d7c380.md`.
- Push: `be64c6d` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:33 CDT, worker bc-02397584 (CPU-01d) — 0xd7c4d0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd7c4d0** (survey index 90).
Failed tries: 0. Status: FIXTURE_VERIFIED; sources on origin; CMake/HANDOFF this checkpoint.
Owned: `native/core/vec_d7c4d0.{hpp,cpp}`, `native/tests/test_vec_d7c4d0.cpp`,
`tools/lift/check_fd7c4d0.py`, `native/CMakeLists.txt` (+vec_d7c4d0 only; origin/main base).
Private dump (not published): `local/re/work/cd7c4d0.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xd7c9d0`/`0xd7fe40`/`0xd7c380`/`0xd7be60`.
Abandoned local race on `0xd7c9d0` (Survey D `8ce468b`).
Checkpoint (2026-10-19 20:33 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x1306d20 dst=0x3352a10 bits=~0,~0,~0,0; index 90; distinct cells.
- Commands: ctest -R '^vec_d7c4d0$' Passed; `python3 -I tools/lift/check_fd7c4d0.py` OK.
- Logs: `/tmp/ctest_d7c4d0.log`, `/tmp/check_fd7c4d0.log`.
- Store: Project `internal/worker-claim-d7c4d0.md`.
- Push: sources `79b9159`; CMake/HANDOFF (this commit).


## ACTIVE CLAIM — 2026-10-19 20:28 CDT, worker bc-9aa0fe88 (SRV-01 mem list) — COMPLETE

Task: SRV-01 mem — intrusive list surgery OBS after split at **0x1a902..0x1a961** / **0x1a974..0x1a9b7**.
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/region_list.{hpp,cpp}`, `native/tests/test_region_list.cpp`,
`tools/lift/check_f1a902_list.py`, `native/CMakeLists.txt` (+region_list only; keep host_map/carve/split),
`docs/POINTERS.md`, `native/README.md`, `docs/TASKS.md`.
**Did NOT edit:** `allocator_region.*`, `startup_ctors.*`, Drive, peer vec WIP.
Deferred: full `0x19660` replenish via `0x19e00`.
Checkpoint (2026-10-19 20:28 CDT, bc-9aa0fe88):
- OBS: links @+0x20 (identity/prev/next); owner @+0x38; unlink self-loop; head via `[arena+0x10]`; inc `[arena+0x20]`.
- Commands: ctest -R '^(region_list|region_split|free_list_carve|host_map)$' 4/4 Passed;
  `python3 -I tools/lift/check_f1a902_list.py` OK; split/carve ties OK.
- Logs: `/tmp/ctest_region_list.log`, `/tmp/check_f1a902_list.log`.
- Store: Project `internal/worker-claim-srv01-list.md`.
- Push: **`94b8332`** on origin/main (code+CMake+POINTERS/README); docs follow-up this commit.

## ACTIVE CLAIM — 2026-10-19 20:32 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 84/86/87/88/89 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED `0xd84ee0` (84), `0xd7fe90` (86),
`0xd7fe40` (87), `0xd7e640` (88), `0xd7c9d0` (89) into `startup_ctors`
(77 `0xdac410` + 85 `0xd80e60` already at `55240a3`).
Skipped gap 10–19, idx 50, SRV. Cleaned accidental conflict markers in this file.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest -R 'startup_ctors|vec_d84ee0|vec_d7fe90|vec_d7fe40|vec_d7e640|vec_d7c9d0|vec_dac410|vec_d80e60' OK;
binary checks OK; registry **47**.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake: origin as-is.
Did NOT edit: peer leaf sources (left local `vec_d85490` WIP unstaged), SRV.
Store: Project `internal/worker-claim-drive.md`.

## ACTIVE CLAIM — 2026-10-19 20:31 CDT, worker bc-81763e53 (CPU-01d) — 0xd7c9d0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd7c9d0** (survey index 89).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d7c9d0.{hpp,cpp}`, `native/tests/test_vec_d7c9d0.cpp`,
`tools/lift/check_fd7c9d0.py`, `native/CMakeLists.txt` (+vec_d7c9d0 only; origin/main base).
Private dump (not published): `local/re/work/cd7c9d0.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xd84ee0`/`0xd7fe40`/`0xd85490`.
Prior: `0xd7e640`=`159b654`/`9a731ef`.
Checkpoint (2026-10-19 20:31 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1306eb0 dst=0x3352a20 bits=~0,~0,~0,0; index 89; distinct cells.
- Commands: ctest -R '^vec_d7c9d0$' Passed; `python3 -I tools/lift/check_fd7c9d0.py` OK.
- Logs: `/tmp/ctest_d7c9d0.log`, `/tmp/check_d7c9d0.log`.
- Store: Project `internal/worker-claim-d7c9d0.md`.
- Push: `8ce468b` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:29 CDT, worker bc-81763e53 (CPU-01d) — 0xd7e640 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd7e640** (survey index 88).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d7e640.{hpp,cpp}`, `native/tests/test_vec_d7e640.cpp`,
`tools/lift/check_fd7e640.py`, `native/CMakeLists.txt` (+vec_d7e640 only; origin/main base).
Private dump (not published): `local/re/work/cd7e640.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xd84ee0`/`0xd7fe40`/`0xdac410`.
Prior: `0xd7fe90` sources `d7174df`, CMake `b1ab361`.
Checkpoint (2026-10-19 20:29 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1307200 dst=0x3352a30 bits=~0,~0,~0,0; index 88; distinct cells.
- Commands: ctest -R '^vec_d7e640$' Passed; `python3 -I tools/lift/check_fd7e640.py` OK.
- Logs: `/tmp/ctest_d7e640.log`, `/tmp/check_d7e640.log`.
- Store: Project `internal/worker-claim-d7e640.md`.
- Push: `159b654` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:28 CDT, worker bc-81763e53 (CPU-01d) — 0xd7fe90 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd7fe90** (survey index 86).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d7fe90.{hpp,cpp}`, `native/tests/test_vec_d7fe90.cpp`,
`tools/lift/check_fd7fe90.py`, `native/CMakeLists.txt` (+vec_d7fe90 only; origin/main base).
Private dump (not published): `local/re/work/cd7fe90.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xd84ee0`/`0xdac410`/`0xd85490`.
Checkpoint (2026-10-19 20:28 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1307dc0 dst=0x3352a50 bits=~0,~0,~0,0; index 86; distinct cells.
- Commands: ctest -R '^vec_d7fe90$' Passed; `python3 -I tools/lift/check_fd7fe90.py` OK.
- Logs: `/tmp/ctest_d7fe90.log`, `/tmp/check_d7fe90.log`.
- Store: Project `internal/worker-claim-d7fe90.md`.
- Push: `d7174df` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:28 CDT, worker bc-02397584 (CPU-01d) — 0xdac410 CMake restore — COMPLETE

Task: CPU-01d — restore peer `vec_d80e60` CMake link accidentally dropped while wiring `vec_dac410`.
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted CMake push this checkpoint.
Owned: `native/CMakeLists.txt` (keep `vec_d80e60` + `vec_dac410`), `HANDOFF.md`.
Sources: `0xdac410`=`7cb0e83`; prior CMake race `96712b5` dropped `d80e60` — restored here.
Did NOT edit: peer sources, Drive, SRV.
Checkpoint: ctest -R 'vec_dac410|vec_d80e60' Passed.
Store: Project `internal/worker-claim-dac410.md`.


## ACTIVE CLAIM — 2026-10-19 21:02 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 77 + CMake dac410 — COMPLETE

Task: CPU-01e — CMake-link peer `0xdac410` (sources `7cb0e83`) and register idx **77** into `startup_ctors`.
Skipped peer IN PROGRESS `0xd84ee0` (84), absent `0xd85490` (82), gap 10–19, idx 50, SRV.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest OK; check_fdac410 + check_fd80e60 OK; registry **42** (+77 `0xdac410`, +85 `0xd80e60` / `d446092`).
Push: `55240a3` on origin/main. Superseded by registry 84/86/88 slice above.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, CMake Drive lines for `vec_dac410`, `HANDOFF.md`.

## ACTIVE CLAIM — 2026-10-19 20:27 CDT, worker bc-81763e53 (CPU-01d) — 0xd80e60 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd80e60** (survey index 85).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d80e60.{hpp,cpp}`, `native/tests/test_vec_d80e60.cpp`,
`tools/lift/check_fd80e60.py`, `native/CMakeLists.txt` (+vec_d80e60 only; origin/main base).
Private dump (not published): `local/re/work/cd80e60.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xdac410`/`0xd85490`/`0xd84ee0`.
Checkpoint (2026-10-19 20:27 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x1307f70 dst=0x3352a60 bits=~0,~0,~0,0; index 85; distinct cells.
- Commands: ctest -R '^vec_d80e60$' Passed; `python3 -I tools/lift/check_fd80e60.py` OK.
- Logs: `/tmp/ctest_d80e60.log`, `/tmp/check_d80e60.log`.
- Store: Project `internal/worker-claim-d80e60.md`.
- Push:  on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:40 CDT, worker bc-73721c8d (CPU-01d) — 0xd84ee0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd84ee0** (survey index 84).
Failed tries: 0. Status: FIXTURE_VERIFIED; on origin/main.
Owned: `native/core/vec_d84ee0.{hpp,cpp}`, `native/tests/test_vec_d84ee0.cpp`,
`tools/lift/check_fd84ee0.py`, `native/CMakeLists.txt` (+vec_d84ee0 only; origin/main base).
Private dump (not published): `local/re/work/cd84ee0.bin`.
Did NOT edit: `startup_ctors.*`, SRV, peer `0xdac410`/`0xd85490`/`0xd853a0`.
Store: Project `internal/worker-claim-d84ee0.md`.
- Push: `4e802b7` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:58 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 78/79/83 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED `0xd94cb0` (78), `0xd90310` (79), `0xd853a0` (83).
Skipped `0xdac410` (not on origin), gap 10–19, idx 50, SRV. Mid-lift `0xd85490` deferred.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest OK; check_fd94cb0/fd90310/fd853a0 OK; registry **40** (+78/+79/+83). `0xdac410` still absent.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake: origin as-is.

## ACTIVE CLAIM — 2026-10-19 20:26 CDT, worker bc-02397584 (CPU-01d) — 0xdac410 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdac410** (survey index 77).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_dac410.{hpp,cpp}`, `native/tests/test_vec_dac410.cpp`,
`tools/lift/check_fdac410.py`, `native/CMakeLists.txt` (+vec_dac410 only; origin/main base).
Private dump (not published): `local/re/work/cdac410.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xd85490`/`0xd853a0`/`region_list`.
Abandoned leaf: Survey C store claim `0xdb7e60` (already on origin); did not take that leaf.
Checkpoint (2026-10-19 20:26 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x130ca00 dst=0x3352c30 bits=~0,~0,~0,0; index 77; distinct cells.
- Commands: ctest -R '^vec_dac410$' Passed; `python3 -I tools/lift/check_fdac410.py` OK.
- Logs: `/tmp/ctest_dac410.log`, `/tmp/check_fdac410.log`.
- Store: Project `internal/worker-claim-dac410.md`.
- Push: sources `7cb0e83`; CMake `96712b5`/`(restore this commit)`.

## ACTIVE CLAIM — 2026-10-19 20:25 CDT, worker bc-81763e53 (CPU-01d) — 0xd853a0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd853a0** (survey index 83).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d853a0.{hpp,cpp}`, `native/tests/test_vec_d853a0.cpp`,
`tools/lift/check_fd853a0.py`, `native/CMakeLists.txt` (+vec_d853a0 only; origin/main base).
Private dump (not published): `local/re/work/cd853a0.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xdac410`/`0xd85490`.
Checkpoint (2026-10-19 20:25 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x130a720 dst=0x3352a80 bits=~0,~0,~0,0; index 83; distinct cells.
- Commands: ctest -R '^vec_d853a0$' Passed; `python3 -I tools/lift/check_fd853a0.py` OK.
- Logs: `/tmp/ctest_d853a0.log`, `/tmp/check_d853a0.log`.
- Store: Project `internal/worker-claim-d853a0.md`.
- Push: (pending this commit).

## ACTIVE CLAIM — 2026-10-19 20:55 CDT, worker bc-201701f2 (Drive / CPU-01e) — 0xd85490 + registry 78/79/82 — COMPLETE

Task: CPU-01d/e — lift **0xd85490** (idx 82); register peer **78** `0xd94cb0` / **79** `0xd90310` into `startup_ctors`.
Skipped **77** `0xdac410` (not on origin), gap 10–19, idx 50, SRV region_split.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest 48/48; check_fd85490 + fd94cb0 + fd90310 OK; registry size **40**.

## ACTIVE CLAIM — 2026-10-19 20:36 CDT, worker bc-73721c8d (CPU-01d) — 0xd94cb0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd94cb0** (survey index 78).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_d94cb0.{hpp,cpp}`, `native/tests/test_vec_d94cb0.cpp`,
`tools/lift/check_fd94cb0.py`, `native/CMakeLists.txt` (+vec_d94cb0 only; origin/main base).
Private dump (not published): `local/re/work/cd94cb0.bin`.
Did NOT edit: `startup_ctors.*`, SRV, peer `0xdac410`/`0xdb3960`/`0xdb7e60`.
Note: commit `277ef68` also carried unrelated peer-ready `vec_d90310` sources (not claimed here).
Checkpoint (2026-10-19 20:36 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x130b660 dst=0x3352c20 bits=~0,~0,~0,0; index 78; distinct cells.
- Commands: ctest -R '^vec_d94cb0$' Passed; `python3 -I tools/lift/check_fd94cb0.py` OK.
- Store: Project `internal/worker-claim-d94cb0.md`.
- Push: sources `277ef68`; CMake wire-up `74dc5d5` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:24 CDT, worker bc-81763e53 (CPU-01d) — 0xd90310 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xd90310** (survey index 79).
Failed tries: 0. Status: FIXTURE_VERIFIED; CMake link push this checkpoint.
Owned: `native/CMakeLists.txt` (+vec_d90310 only). Sources already on origin via peer bundle `277ef68` (with `0xd94cb0`); this commit adds CMake.
Private dump (not published): `local/re/work/cd90310.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xdac410`/`0xd94cb0` sources.
Checkpoint (2026-10-19 20:24 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x130b1a0 dst=0x3352c10 bits=~0,~0,~0,0; index 79; distinct cells.
- Commands: ctest -R '^vec_d90310$' Passed; `python3 -I tools/lift/check_fd90310.py` OK.
- Logs: `/tmp/ctest_d90310.log`, `/tmp/check_d90310.log`.
- Sources on origin: `277ef68` (bundled with peer d94cb0). CMake: `b980851` on origin/main.
- Store: Project `internal/worker-claim-d90310.md`.

## ACTIVE CLAIM — 2026-10-19 20:33 CDT, worker bc-9aa0fe88 (SRV-01 mem split) — COMPLETE

Task: SRV-01 mem — free-list region split surgery OBS at **0x1a8b6..0x1a8fa** (after `0x19660` pop).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/region_split.{hpp,cpp}`, `native/tests/test_region_split.cpp`,
`tools/lift/check_f1a8b6_split.py`, `native/CMakeLists.txt` (+region_split only; keep host_map/carve),
`docs/POINTERS.md`, `native/README.md`.
**Did NOT edit:** `allocator_region.*`, `startup_ctors.*`, Drive, peer vec WIP.
Deferred: full `0x19660` replenish via `0x19e00`.
Checkpoint: ctest region_split Passed; check_f1a8b6_split.py OK.
Store: Project `internal/worker-claim-srv01-split.md`.

## ACTIVE CLAIM — 2026-10-19 20:50 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 76 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED `0xdb3960` (idx 76) into `startup_ctors`.
Skipped peer-owned `0xdac410` (IN PROGRESS), gap 10–19, idx 50, SRV.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest 44/44; check_fdb3960 OK; registry size **37** (+76 `0xdb3960` / `6c869f3`).
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake: origin as-is.

## ACTIVE CLAIM — 2026-10-19 20:22 CDT, worker bc-81763e53 (CPU-01d) — 0xdb3960 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdb3960** (survey index 76).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_db3960.{hpp,cpp}`, `native/tests/test_vec_db3960.cpp`,
`tools/lift/check_fdb3960.py`, `native/CMakeLists.txt` (+vec_db3960 only; origin/main base).
Private dump (not published): `local/re/work/cdb3960.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV, peer `0xdb7e60`/`0xdb5dd0`/`0xdac410`.
Checkpoint (2026-10-19 20:22 CDT, bc-81763e53):
- OBS: unguarded vmovups copy; src=0x130d050 dst=0x3352c40 bits=~0,~0,~0,0; index 76; distinct cells.
- Commands: ctest -R '^vec_db3960$' Passed; `python3 -I tools/lift/check_fdb3960.py` OK.
- Logs: `/tmp/ctest_db3960.log`, `/tmp/check_db3960.log`.
- Store: Project `internal/worker-claim-db3960.md`.
- Push: sources `f443aaf`, CMake/HANDOFF `6c869f3` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:22 CDT, worker bc-81763e53 (CPU-01d) — 0xdbb280 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdbb280** (survey index 71).
Failed tries: 0. Status: FIXTURE_VERIFIED + COMPLETE on origin/main.
Sources: `634dd65`. Drive tip registry **34** + CMake-link: `f502553`. Prior CMake append: `978fd41`.
Cells: src=0x130dbb0 dst=0x3352c90 bits=~0,~0,~0,0.
Store: Project `internal/worker-claim-dbb280.md`.

## ACTIVE CLAIM — 2026-10-19 20:42 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 73/74 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED `0xdb7e60` (73) and `0xdb5dd0` (74) into `startup_ctors`.
Skipped gap 10–19, idx 50, SRV host_map/free_list_carve/region_split.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest 43/43; check_fdb7e60 + check_fdb5dd0 OK; registry size **36** (+73 `0xdb7e60`, +74 `0xdb5dd0`).
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, `HANDOFF.md`. CMake: origin as-is.

## ACTIVE CLAIM — 2026-10-19 20:35 CDT, worker bc-02397584 (CPU-01d) — 0xdac410 — COMPLETE (see tip)

Superseded by tip COMPLETE checkpoint for the same leaf (2026-10-19 20:26 CDT).

## ACTIVE CLAIM — 2026-10-19 20:33 CDT, worker bc-73721c8d (CPU-01d) — 0xdb5dd0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdb5dd0** (survey index 74).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_db5dd0.{hpp,cpp}`, `native/tests/test_vec_db5dd0.cpp`,
`tools/lift/check_fdb5dd0.py`, `native/CMakeLists.txt` (+vec_db5dd0 only; origin/main base).
Private dump (not published): `local/re/work/cdb5dd0.bin`.
Did NOT edit: `startup_ctors.*`, SRV split/carve/`host_map`, peer `0xdb7e60`/`0xdb93a0`/`0xdbb280`.
Abandoned: `0xdb93a0` (Survey C / bc-effcf5dc store claim).
Checkpoint (2026-10-19 20:34 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x130d380 dst=0x3352c60 bits=~0,~0,~0,0; index 74; distinct cells.
- Commands: ctest -R '^vec_db5dd0$' Passed; `python3 -I tools/lift/check_fdb5dd0.py` OK.
- Store: Project `internal/worker-claim-db5dd0.md`.

## ACTIVE CLAIM — 2026-10-19 20:36 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 72/75 + CMake dbb280 — COMPLETE

Task: CPU-01e — register origin peer leaves `0xdb93a0` (72) and `0xdb3a70` (75); ensure CMake links
already-registered `0xdbb280` (71; Survey D leaf claim respected — Drive owns registry only).
Skipped gap 10–19, idx 50.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest 41/41; check_fdb3a70/fdb93a0/fdbb280 OK; registry **34** (+72/+75; CMake links dbb280). Survey D leaf ownership noted.
Owned: `startup_ctors.*`, `test_startup_ctors.cpp`, CMake Drive lines for `vec_dbb280`, `HANDOFF.md`.

## ACTIVE CLAIM — 2026-10-19 20:27 CDT, worker bc-73721c8d (CPU-01d) — 0xdb93a0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdb93a0** (survey index 72).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_db93a0.{hpp,cpp}`, `native/tests/test_vec_db93a0.cpp`,
`tools/lift/check_fdb93a0.py`, `native/CMakeLists.txt` (+vec_db93a0; may already be linked).
Private dump (not published): `local/re/work/cdb93a0.bin`.
Did NOT edit: `startup_ctors.*`, SRV carve/`host_map`, peer `0xdbb280`/`0xdbfa60`/`0xdb3a70`.
Checkpoint (2026-10-19 20:29 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x130d940 dst=0x3352c80 bits=~0,~0,~0,0; index 72; distinct cells.
- Commands: ctest -R '^vec_db93a0$' Passed; `python3 -I tools/lift/check_fdb93a0.py` OK.
- Store: Project `internal/worker-claim-db93a0.md`.
- Push: `cd5f125` on origin/main.


## ACTIVE CLAIM — 2026-10-19 20:30 CDT, worker bc-02397584 (CPU-01d) — 0xdb3a70 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdb3a70** (survey index 75).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_db3a70.{hpp,cpp}`, `native/tests/test_vec_db3a70.cpp`,
`tools/lift/check_fdb3a70.py`, `native/CMakeLists.txt` (+vec_db3a70 only; origin/main base).
Private dump (not published): `local/re/work/cdb3a70.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV carve/`host_map`, peer sequential WIP.
Note: skipped raced `0xdc2d50`/`0xdbfa60` (stale claim withdrawn).

Checkpoint (2026-10-19 20:30 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x130d1e0 dst=0x3352c50 bits=~0,~0,~0,0; index 75; distinct cells.
- Commands: ctest -R '^vec_db3a70$' Passed; `python3 -I tools/lift/check_fdb3a70.py` OK.
- Logs: `/tmp/ctest_db3a70.log`, `/tmp/check_db3a70.log`.
- CMake: origin/main base + vec_db3a70 only.
- Store: Project `internal/worker-claim-db3a70.md`.
- Push: `4177079` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:23 CDT, worker bc-73721c8d (CPU-01d) — 0xdc4180 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdc4180** (survey index 68).
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Owned: `native/core/vec_dc4180.{hpp,cpp}`, `native/tests/test_vec_dc4180.cpp`,
`tools/lift/check_fdc4180.py`, `native/CMakeLists.txt` (+vec_dc4180 only; origin/main base).
Private dump (not published): `local/re/work/cdc4180.bin`.
Did NOT edit: `startup_ctors.*`, SRV `host_map`/`free_list_carve`, peer vec WIP.
Checkpoint (2026-10-19 20:24 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x130e220 dst=0x3352cc0 bits=~0,~0,~0,0; index 68; distinct cells.
- Commands: ctest -R '^vec_dc4180$' Passed; `python3 -I tools/lift/check_fdc4180.py` OK.
- Store: Project `internal/worker-claim-dc4180.md`.
- Push: `9fa5222` on origin/main (CMake restored later by peer `8d3554e` if raced).

## ACTIVE CLAIM — 2026-10-19 20:16 CDT, worker bc-effcf5dc (CPU-01d) — 0xdc2d50 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdc2d50** (survey index 69).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_dc2d50.{hpp,cpp}`, `native/tests/test_vec_dc2d50.cpp`,
`tools/lift/check_fdc2d50.py`, `native/CMakeLists.txt` (+vec_dc2d50 only; origin/main base).
Private dump (not published): `local/re/work/cdc2d50.bin`.
Did NOT edit: `startup_ctors.*`, SRV `host_map`/`free_list_carve`, peer `0xdc4180`/`0xdddc90`.
Checkpoint (2026-10-19 20:16 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x130e010 dst=0x3352cb0 bits=~0,~0,~0,0; index 69; distinct cells.
- Commands: ctest -R '^vec_dc2d50$' Passed; `python3 -I tools/lift/check_fdc2d50.py` OK.
- Logs: `/tmp/ctest_dc2d50.log`, `/tmp/check_dc2d50.log`.
- Prior: `0xde7fb0`=`d0afd8c`, `0xddab00`=`4ce4fec`. Store: `internal/worker-claim-dc2d50.md`.
- Push: (pending this commit).

## ACTIVE CLAIM — 2026-10-19 20:23 CDT, worker bc-02397584 (CPU-01d) — 0xdc2d50 — IN PROGRESS

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdc2d50** (survey index 69).
Failed tries: 0. Status: CLAIMED / lifting.
Owned: `native/core/vec_dc2d50.{hpp,cpp}`, `native/tests/test_vec_dc2d50.cpp`,
`tools/lift/check_fdc2d50.py`, `native/CMakeLists.txt` (+vec_dc2d50 only; origin/main base).
Private dump (not published): `local/re/work/cdc2d50.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV carve/`host_map`, peer `0xdc4180`/`0xdda990`.
Store: Project `internal/worker-claim-dc2d50.md`.

## ACTIVE CLAIM — 2026-10-19 20:20 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 63 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED `0xdda990` (idx 63) into `startup_ctors`.
Skipped gap 10–19, idx 50, SRV `host_map`.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest 35/35; check_fdda990 OK; registry size **27** (+63 `0xdda990` / `f83ba81`).
Owned: `native/core/startup_ctors.*`, `native/tests/test_startup_ctors.cpp`, `HANDOFF.md`.

## ACTIVE CLAIM — 2026-10-19 20:18 CDT, worker bc-9aa0fe88 (SRV-01 mem carve) — COMPLETE

Task: SRV-01 mem — lift pure free-list carve-fit math from RVA **0x1a800** (fit/align only).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/free_list_carve.{hpp,cpp}`, `native/tests/test_free_list_carve.cpp`,
`tools/lift/check_f1a800_carve.py`, `native/CMakeLists.txt` (+free_list_carve only; keep host_map),
`docs/POINTERS.md`, `native/README.md`, `native/core/host_map.hpp` (MAP_FIXED blocker comment).
**Did NOT edit:** `allocator_region.*`, `startup_ctors.*`, Drive gaps, peer vec WIP.
Blocked alternative (documented): MAP_FIXED — `0x479c2` rdi is object context, not preferred VA.
Checkpoint: ctest free_list_carve Passed; check_f1a800_carve.py OK; check_host_map_calls.py OK.
Store: Project `internal/worker-claim-srv01-carve.md`.
Push (code): pending docs follow-up after `9d773cc`.


## ACTIVE CLAIM — 2026-10-19 20:18 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 61/62 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED vec-copy ctors into `startup_ctors`:
**61** `0xdddc90`, **62** `0xddab00` (both on origin/main with CMake).
Skipped gap 10–19, idx 50 (`vec_e094e0` absent), SRV `host_map`, incomplete `0xdd41e0`.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest 34/34; check_fdddc90 + check_fddab00 OK; registry size **26** (+61 `0xdddc90`, +62 `0xddab00`).
Owned: `native/core/startup_ctors.*`, `native/tests/test_startup_ctors.cpp`, `HANDOFF.md`.
CMake: origin/main as-is. Store: Project `internal/worker-claim-drive.md`.

## ACTIVE CLAIM — 2026-10-19 20:20 CDT, worker bc-02397584 (CPU-01d) — 0xdda990 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdda990** (survey index 63).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_dda990.{hpp,cpp}`, `native/tests/test_vec_dda990.cpp`,
`tools/lift/check_fdda990.py`, `native/CMakeLists.txt` (+vec_dda990 only; origin/main base).
Private dump (not published): `local/re/work/cdda990.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV `host_map`, peer `0xddab00`/`0xdddc90`.
Note: skipped `0xddab00` after peer race (`4ce4fec`).

Checkpoint (2026-10-19 20:21 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x130eee0 dst=0x3352ea0 bits=~0,~0,~0,0; index 63; distinct cells.
- Commands: ctest -R '^vec_dda990$' Passed; `python3 -I tools/lift/check_fdda990.py` OK.
- Logs: `/tmp/ctest_dda990.log`, `/tmp/check_dda990.log`.
- CMake: origin/main base + vec_dda990 only.
- Store: Project `internal/worker-claim-dda990.md`.
- Push: `f83ba81` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:13 CDT, worker bc-effcf5dc (CPU-01d) — 0xddab00 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xddab00** (survey index 62).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_ddab00.{hpp,cpp}`, `native/tests/test_vec_ddab00.cpp`,
`tools/lift/check_fddab00.py`, `native/CMakeLists.txt` (+vec_ddab00 only; origin/main base).
Private dump (not published): `local/re/work/cddab00.bin`.
Did NOT edit: `startup_ctors.*`, SRV `host_map`, peer `0xdddc90` WIP.
Checkpoint (2026-10-19 20:13 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x130f090 dst=0x3352eb0 bits=~0,~0,~0,0; index 62; distinct cells.
- Commands: ctest -R '^vec_ddab00$' Passed; `python3 -I tools/lift/check_fddab00.py` OK.
- Logs: `/tmp/ctest_ddab00.log`, `/tmp/check_ddab00.log`.
- Prior: `0xde7fb0` pushed `d0afd8c`. Store: `internal/worker-claim-ddab00.md`.
- Push: `b428a2c` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:17 CDT, worker bc-02397584 (CPU-01d) — 0xddab00 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xddab00** (survey index 62).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_ddab00.{hpp,cpp}`, `native/tests/test_vec_ddab00.cpp`,
`tools/lift/check_fddab00.py`, `native/CMakeLists.txt` (+vec_ddab00 only; origin/main base).
Private dump (not published): `local/re/work/cddab00.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV `host_map`, peer `0xdddc90`/`0xddef00`/`0xde7fb0`.

Checkpoint (2026-10-19 20:18 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x130f090 dst=0x3352eb0 bits=~0,~0,~0,0; index 62; distinct cells.
- Commands: ctest -R '^vec_ddab00$' Passed; `python3 -I tools/lift/check_fddab00.py` OK.
- Logs: `/tmp/ctest_ddab00.log`, `/tmp/check_ddab00.log`.
- CMake: origin/main base + vec_ddab00 only.
- Store: Project `internal/worker-claim-ddab00.md`.

## ACTIVE CLAIM — 2026-10-19 20:16 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 56/60 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED vec-copy ctors into `startup_ctors`:
**56** `0xde7fb0`, **60** `0xddef00` (both now on origin/main with CMake links).
Skipped gap 10–19, idx 50 (`vec_e094e0` absent), SRV `host_map`.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest 32/32; check_fde7fb0 + check_fddef00 OK; registry size **24** (+56 `0xde7fb0`, +60 `0xddef00` / `99ce863`).
Owned: `native/core/startup_ctors.*`, `native/tests/test_startup_ctors.cpp`, `HANDOFF.md`.
CMake: origin/main as-is (already links both). Store: Project `internal/worker-claim-drive.md`.

## ACTIVE CLAIM — 2026-10-19 20:12 CDT, worker bc-effcf5dc (CPU-01d) — 0xde7fb0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xde7fb0** (survey index 56).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_de7fb0.{hpp,cpp}`, `native/tests/test_vec_de7fb0.cpp`,
`tools/lift/check_fde7fb0.py`, `native/CMakeLists.txt` (+vec_de7fb0 only; origin/main base).
Private dump (not published): `local/re/work/cde7fb0.bin`.
Did NOT edit: `startup_ctors.*`, peer lifts, Drive, SRV host_map.
Skipped push of `0xe0b180` — already on origin/main as `aa53115`.
Checkpoint (2026-10-19 20:12 CDT, bc-effcf5dc):
- OBS: unguarded vmovups copy; src=0x1310c90 dst=0x3353320 bits=~0,~0,~0,0; index 56; distinct cells.
- Commands: ctest -R '^vec_de7fb0$' Passed; `python3 -I tools/lift/check_fde7fb0.py` OK.
- Logs: `/tmp/ctest_de7fb0.log`, `/tmp/check_de7fb0.log`.
- Store: Project `internal/worker-claim-de7fb0.md`.
- Push: `d0afd8c` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:15 CDT, worker bc-73721c8d (CPU-01d) — 0xdddc90 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdddc90** (survey index 61).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_dddc90.{hpp,cpp}`, `native/tests/test_vec_dddc90.cpp`,
`tools/lift/check_fdddc90.py`, `native/CMakeLists.txt` (+vec_dddc90 only; origin/main base).
Private dump (not published): `local/re/work/cdddc90.bin`.
Did NOT edit: `startup_ctors.*`, SRV `host_map`, peer `0xddef00`/`0xde7fb0`/`0xde1ad0`.
Checkpoint (2026-10-19 20:16 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x130f350 dst=0x3352ec0 bits=~0,~0,~0,0; index 61; distinct cells.
- Commands: ctest -R '^vec_dddc90$' Passed; `python3 -I tools/lift/check_fdddc90.py` OK.
- Logs: `/tmp/ctest_dddc.log`, `/tmp/check_dddc.log`.
- Store: Project `internal/worker-claim-dddc90.md`.
- Push: `43395d3` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:12 CDT, worker bc-9aa0fe88 (SRV-01 mem) — COMPLETE

Task: SRV-01 mem — Darwin host mapping behind verified allocator region API `0x19ae0`.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Owned: `native/core/host_map.{hpp,cpp}`, `native/tests/test_host_map.cpp`,
`tools/lift/check_host_map_calls.py`, `native/CMakeLists.txt` (+host_map only),
`docs/POINTERS.md`, `docs/TASKS.md`, `native/README.md`.
**Did NOT edit:** `allocator_region.*`, `startup_ctors.*`, Drive gaps, peer vec WIP.
Checkpoint: ctest host_map Passed; check_host_map_calls.py OK; check_f19ae0.py OK.
Push (code): `f68caf3` on origin/main. Docs follow-up this commit.
Store: Project `internal/worker-claim-srv01.md`.


## ACTIVE CLAIM — 2026-10-19 20:14 CDT, worker bc-02397584 (CPU-01d) — 0xddef00 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xddef00** (survey index 60).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_ddef00.{hpp,cpp}`, `native/tests/test_vec_ddef00.cpp`,
`tools/lift/check_fddef00.py`, `native/CMakeLists.txt` (+vec_ddef00 only; origin/main base).
Private dump (not published): `local/re/work/cddef00.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV `host_map`, peer `0xdfd5f0`/`0xde7fb0`.

Checkpoint (2026-10-19 20:15 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x130f540 dst=0x3352ed0 bits=~0,~0,~0,0; index 60; distinct cells.
- Commands: ctest -R '^vec_ddef00$' Passed; `python3 -I tools/lift/check_fddef00.py` OK.
- Logs: `/tmp/ctest_ddef00.log`, `/tmp/check_ddef00.log`.
- CMake: origin/main base + vec_ddef00 only.
- Store: Project `internal/worker-claim-ddef00.md`.
- Push: `99ce863` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:12 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 54/57/58 — COMPLETE

Task: CPU-01e — register peer FIXTURE_VERIFIED vec-copy ctors into `startup_ctors`:
**54** `0xdfd5f0`, **57** `0xde6d20`, **58** `0xde1ad0`. Skip `0xde7fb0` (not on origin yet),
idx 50 / gap 10–19. CMake already links these on origin/main — Drive owns registry only.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Checkpoint: ctest 29/29; check_fdfd5f0/fde6d20/fde1ad0 OK; registry size 22 (+54/57/58; de7fb0 skipped if absent on origin).
Includes **0xdfd5f0** idx 54 (sources 8921252, CMake 56aca61).
Owned: `native/core/startup_ctors.*`, `native/tests/test_startup_ctors.cpp`, `HANDOFF.md`.
Did NOT edit: peer vec sources, SRV `host_map`, CMake (no new lines needed).
Store: Project `internal/worker-claim-drive.md`.

## ACTIVE CLAIM — 2026-10-19 20:11 CDT, worker bc-02397584 (CPU-01d) — 0xde1ad0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xde1ad0** (survey index 58).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_de1ad0.{hpp,cpp}`, `native/tests/test_vec_de1ad0.cpp`,
`tools/lift/check_fde1ad0.py`, `native/CMakeLists.txt` (+vec_de1ad0 only; origin/main base).
Private dump (not published): `local/re/work/cde1ad0.bin`.
Did NOT edit: `startup_ctors.*`, Drive, SRV-01 `host_map`, peer `0xdfd5f0`/`0xde7fb0`/`0xde6d20`.

Checkpoint (2026-10-19 20:12 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x130fd00 dst=0x3353300 bits=~0,~0,~0,0; index 58; distinct cells.
- Commands: ctest -R '^vec_de1ad0$' Passed; `python3 -I tools/lift/check_fde1ad0.py` OK.
- Logs: `/tmp/ctest_de1ad0.log`, `/tmp/check_de1ad0.log`.
- CMake: origin/main base + vec_de1ad0 only (diff verified clean).
- Store: Project `internal/worker-claim-de1ad0.md`.
- Push: `53b9534` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:09 CDT, worker bc-201701f2 (Drive / CPU-01e) — registry idx 49/51/52/53 — COMPLETE

Task: CPU-01e — register already FIXTURE_VERIFIED peer vec-copy ctors into `startup_ctors`:
indices **49** (`0xe0b180`), **51** (`0xe085e0`), **52** (`0xe05320`), **53** (`0xe026b0`).
Skipped SRV-01/`host_map`, gap 10–19, idx 50 (sources not on origin), peer WIP `de7fb0`/`dfd5f0`/`de6d20`.
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Owned: `native/core/startup_ctors.*`, `native/tests/test_startup_ctors.cpp`,
`native/CMakeLists.txt` (+vec_e0b180/+vec_e05320 link lines only; sources already on origin).
Store: Project `internal/worker-claim-drive.md`.


## ACTIVE CLAIM — 2026-10-19 20:08 CDT, worker bc-02397584 (CPU-01d) — 0xde6d20 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xde6d20** (survey index 57).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_de6d20.{hpp,cpp}`, `native/tests/test_vec_de6d20.cpp`,
`tools/lift/check_fde6d20.py`, `native/CMakeLists.txt` (+vec_de6d20 only; origin/main base).
Private dump (not published): `local/re/work/cde6d20.bin`.
Did NOT edit: `startup_ctors.*`, Drive `0xea52f0`, SRV-01 `host_map`, peer `0xdfd5f0`/`0xde7fb0`.

Checkpoint (2026-10-19 20:09 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x1310350 dst=0x3353310 bits=~0,~0,~0,0; index 57; distinct cells.
- Commands: ctest -R '^vec_de6d20$' Passed; `python3 -I tools/lift/check_fde6d20.py` OK.
- Logs: `/tmp/ctest_de6d20.log`, `/tmp/check_de6d20.log`.
- CMake: origin/main base + vec_de6d20 only (diff verified clean of peer/host_map).
- Store: Project `internal/worker-claim-de6d20.md`.
- Push: `89984fa` (lift) + `74103e4` (CMake wire) on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:08 CDT, worker bc-73721c8d (CPU-01d) — 0xdfd5f0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xdfd5f0** (survey index 54).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_dfd5f0.{hpp,cpp}`, `native/tests/test_vec_dfd5f0.cpp`,
`tools/lift/check_fdfd5f0.py`, `native/CMakeLists.txt` (+vec_dfd5f0 only on origin/main base).
Private dump (not published): `local/re/work/cdfd5f0.bin`.
Did NOT edit: `startup_ctors.*`, peer `vec_e026b0.*`, Drive `init_ea52f0.*` / early registry.
Checkpoint (2026-10-19 20:09 CDT, bc-73721c8d):
- OBS: unguarded vmovups copy; src=0x13124a0 dst=0x3353340 bits=~0,~0,~0,0; index 54; distinct cells.
- Commands: ctest -R vec_dfd5f0 Passed; `python3 -I tools/lift/check_fdfd5f0.py` OK.
- Logs: `/tmp/ctest_dfd5.log`, `/tmp/check_dfd5.log`.
- Store: Project `internal/worker-claim-dfd5f0.md`.
- Push: sources `8921252`; CMake wire-up `56aca61` (`56aca617800e`) on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:07 CDT, worker bc-201701f2 (Drive / CPU-01e) — 0xea52f0 + registry idx 20 — COMPLETE

Task: CPU-01e — lift pure data ctor RVA **0xea52f0** (index 22); register already-lifted idx **20**
(`0xef85e0` / `vec_ef85e0`) into `startup_ctors`. Skipped SRV-01 (bc-9aa0fe88). Skipped idx 10–19
(heap/atexit wrappers need native malloc).
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Owned: `native/core/init_ea52f0.*`, `native/tests/test_init_ea52f0.cpp`,
`tools/lift/check_fea52f0.py`, `native/core/startup_ctors.*`, `native/tests/test_startup_ctors.cpp`,
`native/CMakeLists.txt` (+init_ea52f0 only).
Did NOT edit: peer `vec_e026b0.*`, `addr.*`, SRV platform.
Checkpoint: ctest 24/24; `check_fea52f0` / `check_fef85e0` OK. Registry size 15 (0–9,20,22,48,520,538).
Store: Project `internal/worker-claim-drive.md`.

## ACTIVE CLAIM — 2026-10-19 20:05 CDT, worker bc-02397584 (CPU-01d) — 0xe026b0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xe026b0** (survey index 53).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_e026b0.{hpp,cpp}`, `native/tests/test_vec_e026b0.cpp`,
`tools/lift/check_fe026b0.py`, `native/CMakeLists.txt` (+vec_e026b0 only; origin/main base).
Private dump (not published): `local/re/work/ce026b0.bin`.
Did NOT edit: `startup_ctors.*`, peer WIP, Drive `0xf4eba0`/`0xf4b540`/`0xf4fd40`.

Checkpoint (2026-10-19 20:06 CDT, bc-02397584):
- OBS: unguarded vmovups copy; src=0x1312f10 dst=0x3353350 bits=~0,~0,~0,0; index 53; distinct cells.
- Commands: ctest -R '^vec_e026b0$' Passed; `python3 -I tools/lift/check_fe026b0.py` OK.
- Logs: `/tmp/ctest_e026b0.log`, `/tmp/check_e026b0.log`.
- Next: allowlisted push; store `internal/worker-claim-e026b0.md`.
- Push: `e4cec61` on origin/main.

## ACTIVE CLAIM — 2026-10-19 20:00 CDT, worker bc-dc141fdf (CPU-01d) — 0xe05320 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xe05320** (index 52).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_e05320.{hpp,cpp}`, `native/tests/test_vec_e05320.cpp`,
`tools/lift/check_fe05320.py`, `native/CMakeLists.txt` (+vec_e05320 only).
Private dump (not published): `local/re/work/ce05320.bin`.
Did NOT edit: `startup_ctors.*`, peer WIP, Drive `0xf4eba0`/`0xf4b540`.

Checkpoint (2026-10-19 20:01 CDT, bc-dc141fdf):
- OBS: unguarded vmovups copy; src=0x13132d0 dst=0x3353360 bits=~0,~0,~0,0; distinct cells.
- Commands: ctest -R vec_e05320 Passed; `python3 -I tools/lift/check_fe05320.py` OK.
- Push: `79ba84b` on origin/main (tip later advanced).
- Store: Project `internal/worker-claim-e05320.md`.

## ACTIVE CLAIM — 2026-10-19 20:01 CDT, worker bc-24e5fd45 (CPU-01d) — 0xe094e0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xe094e0** (survey index 50).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_e094e0.{hpp,cpp}`, `native/tests/test_vec_e094e0.cpp`,
`tools/lift/check_fe094e0.py`, `native/CMakeLists.txt` (+vec_e094e0; restore +vec_e085e0
dropped by peer race; drop broken remote `init_f4fd40` refs again).
Private dump (not published): `local/re/work/ce094e0.bin`.
Did NOT edit: `startup_ctors.*`, peer WIP / Drive files.
Checkpoint (2026-10-19 20:01 CDT, bc-24e5fd45):
- OBS: unguarded vmovups copy; src=0x1313e20 dst=0x3353380 bits=~0,~0,~0,0; index 50; distinct cells.
- Commands: ctest --test-dir local/build/native → **20/20**; `python3 -I tools/lift/check_fe094e0.py` OK.
- Logs: `/tmp/ctest_e094.log`, `/tmp/check_e094.log`.
- Next: allowlisted push; store `internal/worker-claim-e094e0.md`.

## ACTIVE CLAIM — 2026-10-19 20:00 CDT, worker bc-cb8d07c2 (CPU-01h) — COMPLETE

Task: CPU-01h — leaf ctor `0x24ebf0` → `zero_struct12` (cell @0x2eebc48).
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Owned: native/core/zero_struct12.*, tests/test_zero_struct12.cpp, check_f24ebf0.py, CMakeLists (+target).
Checkpoint: ctest zero_struct12 Passed; `python3 -I tools/lift/check_f24ebf0.py` OK.
Store: Project `internal/worker-claim-3e.md`.

## ACTIVE CLAIM — 2026-10-19 19:59 CDT, worker bc-cd298177 (claim-b76950) — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xb76950** (index 280).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push this checkpoint.
Owned: `native/core/vec_b76950.{hpp,cpp}`, `native/tests/test_vec_b76950.cpp`,
`tools/lift/check_fb76950.py`, `native/CMakeLists.txt` (+vec_b76950 only).
Private dump (not published): `local/re/work/cb76950.bin`.
Did NOT edit `startup_ctors.*`. Avoided Drive `0xf4eba0`/`0xf4b540` and peer in-flight RVAs.
Checkpoint: ctest `vec_b76950` Passed; `python3 -I tools/lift/check_fb76950.py` OK
(src=0x12d9350 dst=0x3350dd0 bits=~0,~0,~0,0). Store: `internal/worker-claim-b76950.md`.

## ACTIVE CLAIM — 2026-10-19 19:58 CDT, worker bc-71956f82 (CPU-01d) — 0xe0b180 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xe0b180** (survey index 49).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_e0b180.{hpp,cpp}`, `native/tests/test_vec_e0b180.cpp`,
`tools/lift/check_fe0b180.py`, `native/CMakeLists.txt` (+vec_e0b180 only).
Private dump (not published): `local/re/work/ce0b180.bin`.
Did NOT edit `startup_ctors.*` or peer lifts.
Checkpoint: ctest includes `vec_e0b180` Passed (suite 21/21 at check time);
`python3 -I tools/lift/check_fe0b180.py` OK (src=0x13141b0 dst=0x3353390 bits=~0,~0,~0,0).
Store: Project `internal/worker-claim-e0b180.md`.

## ACTIVE CLAIM — 2026-10-19 19:58 CDT, worker bc-dc141fdf (CPU-01d) — 0xef85e0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xef85e0** (index 20).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_ef85e0.{hpp,cpp}`, `native/tests/test_vec_ef85e0.cpp`,
`tools/lift/check_fef85e0.py`, `native/CMakeLists.txt` (+vec_ef85e0 only).
Private dump (not published): `local/re/work/cef85e0.bin`.
Did NOT edit: `startup_ctors.*`, peer vec_e085e0/vec_b747f0/zero_xmm_qword/Drive files.

Checkpoint (2026-10-19 19:59 CDT, bc-dc141fdf):
- OBS: unguarded vmovups copy; src=0x1324f80 dst=0x3354180 bits=~0,~0,~0,0; distinct cells.
- Commands: ctest -R vec_ef85e0 Passed; `python3 -I tools/lift/check_fef85e0.py` OK.
- Next: allowlisted push to origin/main.
- Push: `97062234d958a289584804834a84a73dbcfd1a89` on origin/main.
- Store: Project `internal/worker-claim-cpu01-ef85e0.md`.

## ACTIVE CLAIM — 2026-10-19 19:58 CDT, worker bc-cb8d07c2 (CPU-01g) — COMPLETE

Task: CPU-01g — leaf ctor `0x58bd10` → `zero_xmm_qword` (xmm@0x2f4e4b0 + qword@0x2f4e4d0).
Failed tries: 0. Status: FIXTURE_VERIFIED + allowlisted push.
Owned: native/core/zero_xmm_qword.*, tests/test_zero_xmm_qword.cpp, check_f58bd10.py, CMakeLists (+target).
Did NOT edit startup_ctors.* / peer WIP sources.
Checkpoint: ctest 18/18; `python3 -I tools/lift/check_f58bd10.py` OK. Store: internal/worker-claim-3d.md.

## ACTIVE CLAIM — 2026-10-19 19:57 CDT, worker bc-24e5fd45 (CPU-01d) — 0xe085e0 — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xe085e0** (index 51).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: `native/core/vec_e085e0.{hpp,cpp}`, `native/tests/test_vec_e085e0.cpp`,
`tools/lift/check_fe085e0.py`, `native/CMakeLists.txt` (+vec_e085e0 only).
Private dump (not published): `local/re/work/ce085e0.bin`.
Checkpoint: ctest **19/19** (incl. `vec_e085e0`); `python3 -I tools/lift/check_fe085e0.py` OK
(src=0x1313740 dst=0x3353370 bits=~0,~0,~0,0). Store: `internal/worker-claim-e085e0.md`.
Did NOT edit `startup_ctors.*` or peer lifts.

## ACTIVE CLAIM — 2026-10-19 19:56 CDT, worker bc-3fe140af (CPU-01d) — 0xb747f0 COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xb747f0** (index 281).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned (new only): `native/core/vec_b747f0.{hpp,cpp}`, `native/tests/test_vec_b747f0.cpp`,
`tools/lift/check_fb747f0.py`, `native/CMakeLists.txt` (+target only).
Private dump (not published): `local/re/work/cb747f0.bin`.
**Did NOT edit:** `startup_ctors.*`, peer vec/addr/allocator/clear_flag*, ASSETS.
Avoided: Drive `0xf4b510`/`0xf30460`/`0x196c90`, Claim-3 `0xdd41e0`, claim-2b `0xb52110`,
claim-2c `0xb743c0`, CPU-01e `0x3bd70`, CPU-01f `0xa3ade0`/`0x5777a0`, `0xf4fd40`.
Checkpoint (2026-10-19 19:57 CDT): ctest **18/18**; `python3 -I tools/lift/check_fb747f0.py` OK
(src=0x12d9100 dst=0x3350dc0 bits=~0,~0,~0,0; distinct from sibling mask cells).
GitHub already pushed (assist not needed); this commit is allowlisted lift only.
Store: Project `internal/worker-claim-b747f0.md`.

## ACTIVE CLAIM — 2026-10-19 19:55 CDT, worker bc-cd298177 (claim-2c) — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xb743c0** (index 282).
Failed tries: 0. Status: FIXTURE_VERIFIED; allowlisted push pending/done this checkpoint.
Owned: `native/core/vec_b743c0.{hpp,cpp}`, `native/tests/test_vec_b743c0.cpp`,
`tools/lift/check_fb743c0.py`, `native/CMakeLists.txt` (+vec_b743c0 only).
Private dump (not published): `local/re/work/cb743c0.bin`.
Did NOT edit `startup_ctors.*`. Avoided Drive/Claim-3/claim-2b RVAs.
Checkpoint: ctest includes `vec_b743c0` Passed; `python3 -I tools/lift/check_fb743c0.py` OK
(src=0x12d8d90 dst=0x3350db0 bits=~0,~0,~0,0). Store: `internal/worker-claim-2c.md`.

## Public remote (durable)

- **GitHub:** https://github.com/DehydratedFlask/SotC-Mac-Port (`main`)
- At **every** checkpoint / completed task claim: commit **allowlisted** changes only, clear
  message, then `git push origin main`. Pull/rebase if peers pushed; **never force-push**.
- Before push: `git status` + `python3 -I scripts/audit_publish_paths.py` (staged) and `--all`.
- **Never commit:** `Shadow of the Colossus/`, `local/` (incl. game dumps / `eboot.elf`),
  `reference/`, `logs/`, PKG/PSARC/eboot/SELF/ELF/sprx, secrets, AgentStores. See `.gitignore`
  and [AGENTS.md](AGENTS.md) / [ROADMAP.md](ROADMAP.md).
- Users must supply their own legally obtained game files; none ship in the repo.

## ACTIVE CLAIM — 2026-10-19 19:55 CDT, worker bc-71956f82 (publish-boundary assist) — COMPLETE

Task: Assist publish after AUD-01 — keep `local/manifests/assets/audio-headers.json`
gitignored/audit-denied; publish classifier scripts under `scripts/`.
Failed tries: 0. Status: COMPLETE.
Owned: `.gitignore` (comment under `/local/` only), `scripts/test_audit_publish_paths.py` (new),
`docs/ASSETS.md` (publish-boundary sentence). Classifiers already on remote from earlier push.
Did NOT claim CPU-01 lifts. Store: Project `internal/worker-assist-publish-aud01.md`.
Checkpoint: `git check-ignore` → `**/local/`; audit denies JSON / allows scripts;
`test_audit_publish_paths.py` 3/3; pushed `f1cd257` to `origin/main`.

## ACTIVE CLAIM — 2026-10-19 19:55 CDT, worker bc-cb8d07c2 (CPU-01f) — COMPLETE

Task: CPU-01f — leaf ctors `0xa3ade0` (store u32 0) + `0x5777a0` (vxorps/store xmm).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: native/core/store_zero32.*, zero_xmm16.*, tests, check_fa3ade0.py, check_f5777a0.py,
CMakeLists (+targets; also linked peer `vec_b743c0.cpp` into sotc_core when missing), 
local/re/work/c{a3ade0,5777a0}.bin.
Did NOT edit: startup_ctors.*, Drive clear_flag32/vec_neg036/xyz_mask_copy.

Checkpoint (2026-10-19 19:56 CDT, bc-cb8d07c2):
- OBS: 0xa3ade0 → cell 0x32f5980; 0x5777a0 → xmm cell 0x2f295b8.
- Commands: ctest **16/16** Passed; both binary ties OK.
- Store: Project `internal/worker-claim-3c.md`.

## ACTIVE CLAIM — 2026-10-19 19:54 CDT, worker bc-dc141fdf (GitHub publish) — COMPLETE

Task: Create public GitHub repo SotC-Mac-Port + allowlisted first commit/push + durable checkpoint rule.
Failed tries: 0. Status: COMPLETE.
Owned: `.gitignore`, `README.md`, `ROADMAP.md`, `AGENTS.md`, `HANDOFF.md` (this claim),
`scripts/audit_publish_paths.py`; Project store `docs/github-publish.md`.
Did NOT invent LICENSE (none pre-chosen). Skipped `graft/` (regenerable ~277M noise).
Exact check: staged + `--all` path audit OK; `git push -u origin main`.
**First push SHA:** `0138c79313cb2161dd26a99d4ad0c5c9e23d27a9` — https://github.com/DehydratedFlask/SotC-Mac-Port
Note: `gh api user` login is **DehydratedFlask** (John Wang); `gh auth status` also shows
Johnnywang2001 keyring label — canonical owner/URL uses DehydratedFlask.
Peer bc-24e5fd45 help claim: remote now exists; no need to recreate.
Path audit: `python3 -I scripts/audit_publish_paths.py --all` → OK (110 paths); no eboot/PKG/psarc/local/reference.

## ACTIVE CLAIM — 2026-10-19 19:53 CDT, worker bc-24e5fd45 (GitHub repo help) — COMPLETE

Task: Peer assist on SotC-Mac-Port — allowlisted stage/audit/first push + HANDOFF public-remote block.
Failed tries: 0. Status: COMPLETE (coordinated with bc-dc141fdf).
Pushed root commit `0138c79` (110 files) then HANDOFF note `2cfa87f` to
https://github.com/DehydratedFlask/SotC-Mac-Port. Path audit OK; **no** `local/` dumps.
Store: `internal/worker-claim-github.md`. No competing CPU-01d lift.

## ACTIVE CLAIM — 2026-10-19 19:52 CDT, worker bc-cd298177 (claim-2b) — COMPLETE

Task: CPU-01d — lift unguarded vec-copy ctor RVA **0xb52110** (index 290).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned files: `native/core/vec_b52110.{hpp,cpp}`, `native/tests/test_vec_b52110.cpp`,
`tools/lift/check_fb52110.py`, `native/CMakeLists.txt` (+target only), `local/re/work/cb52110.bin`.
Did NOT edit: `startup_ctors.*` (Drive registry), peer vec/addr/allocator files.
Avoided owned: Drive `0xf4b510`/`0xf30460`/`0x196c90`, Claim-3 `0xdd41e0`.
Checkpoint: ctest **12/12**; `python3 -I tools/lift/check_fb52110.py` OK
(src=0x12d6b20 dst=0x3350ce0 bits=~0,~0,~0,0; distinct from 0xf30460 cells).
Store: Project `internal/worker-claim-2b.md`.

## ACTIVE CLAIM — 2026-10-19 19:53 CDT, worker bc-cb8d07c2 (CPU-01e) — COMPLETE

Task: CPU-01e — clear-via-xchg ctor RVA 0x3bd70 (index 560); REX.W xchg u64 (NOT Drive 0x196c90).
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: native/core/clear_flag64.{hpp,cpp}, tests/test_clear_flag64.cpp, tools/lift/check_f3bd70.py,
CMakeLists (+clear_flag64 only), local/re/work/c3bd70.bin.
Did NOT edit: startup_ctors.*, clear_flag32.*, vec_b52110.*, allocator_region.*, addr.*.

Checkpoint (2026-10-19 19:54 CDT, bc-cb8d07c2):
- OBS: `xor eax,eax; xchg rax,[0x2ea4f20]; ret` (10 B). Distinct from Drive u32 xchg @0x2ee4530.
- Commands: ctest **13/13** Passed; `python3 -I tools/lift/check_f3bd70.py` OK.
- Store: Project `internal/worker-claim-3b.md`.

## ACTIVE CLAIM — 2026-10-19 19:50 CDT, worker bc-3fe140af (GPU-01 prep) — COMPLETE / IDLE

Task: GPU-01 prep only — offline shader/material member names from `local/manifests/assets/`.
Failed tries: 0. Status: COMPLETE (names inventory; full GPU-01 still WAITING). Worker IDLE.
Owned files: `docs/GPU01_PREP.md` (new), `docs/INDEX.md` (link row), `docs/TASKS.md`
(GPU-01 register note only), `local/re/work/gpu01-shader-material-{members.jsonl,summary.json}`.
Did NOT touch: `native/core/*`, `scripts/psarc.py`, `docs/ASSETS.md`.
Bounded outcome: `.csdr`/`.cmat` unique path list + counts/samples/filter.
Exact check: recount from `*.members.jsonl` → 44,757 unique (34,392 `.csdr` + 10,365 `.cmat`).
Skipped: GCN/codec invent, Metal, member extraction, GPU-02.
Store report: Project `internal/worker-claim-gpu01.md` (final + idle note 19:54).

Checkpoint (2026-10-19 19:52 CDT, bc-3fe140af):
- Observed: filter `.csdr`+`.cmat` only; raw rows 55,223 → unique 44,757; pak2/pak3 zero;
  three related `.cslt` under `shaders/_cmn/` sidelined (not in corpus).
- Command: offline Python scan of `local/manifests/assets/*.members.jsonl` (no extract).
- Inference: path taxonomy supports shader/material naming; not a GCN/Metal corpus.
- Remaining risk: cachedFiles overlay winner (DATA-02); TOC md5 unknown; stages/layouts open.
- Next exact action: none for this worker; full GPU-01 stays WAITING_FOR_DEPENDENCY.

Idle checkpoint (2026-10-19 19:54 CDT, bc-3fe140af):
- AUD-01 still open → skipped ASSETS. Next free CPU-01d/e already claimed (`0xb52110` done,
  `0x3bd70` in progress) → no new ctor. No commit while GitHub publish peer is in flight;
  would never stage `local/manifests`.

## ACTIVE CLAIM — 2026-10-19 19:49 CDT, worker bc-201701f2 (Drive) — CPU-02a + CPU-01

Task: CPU-02a `0x19ae0` + CPU-01 ctors `0xf4b510` / `0xf30460` / `0x196c90`.
Failed tries: 0. Status: FIXTURE_VERIFIED (turn checkpoint).
Owned files: `allocator_region.*`, `check_f19ae0.py`, `vec_neg036.*`, `check_ff4b510.py`,
`xyz_mask_copy.*`, `check_ff30460.py`, `clear_flag32.*`, `check_f196c90.py`, `startup_ctors` registry,
`docs/POINTERS.md` (region API row). **Not** `addr.*`. Store: `internal/worker-claim-drive.md`.
Checkpoint (19:52 CDT): ctest **11/11**; ties f19ae0 / ff4b510 / ff30460 / f196c90 OK.
Registry indices: 0, 7, 9, 48, 520, 538. Peer `vec_dd41e0` also in tree.

## ACTIVE CLAIM — 2026-10-19 19:51 CDT, worker bc-24e5fd45 (APP-01b) — COMPLETE

Task: APP-01b — honest NOT-BUILT native-core probe / diagnostics polish (no fake Play).
Failed tries: 0. Status: COMPLETE (tests green; Play still honestly disabled).
Owned files: `launcher/Sources/LauncherCore/{SystemChecks,ReadinessReport}.swift`,
`launcher/Sources/SoCLauncher/{ContentView,LauncherModel,ReadinessRow}.swift`,
`launcher/Tests/LauncherCoreTests/AssetValidatorTests.swift`, `launcher/README.md`,
Project store `internal/worker-claim-app01b.md`.
Checkpoint: `swift test --package-path launcher --scratch-path local/build/launcher` → 7/7 pass.
Missing/non-Mach-O/shell stub → `.notBuilt` with "NOT BUILT" + "Play disabled"; `canStart==false`.
Also this turn: merged RE-02 app-main delta into `local/re/boot-graph.json` (backup under work/).

## ACTIVE CLAIM — 2026-10-19 19:49 CDT, worker bc-24e5fd45 (RE-02 app-main) — SLICE DONE + boot-graph merged

Task: RE-02 app-main — helpers `0x536aa0`, option-parser, vtable slots +0x10/+0x18.
Failed tries: 0. Status: STATIC_ANALYSIS notes written; `boot-graph.json` merged from delta.
Owned files: `local/re/work/app-main-re02.{json,md}`, `local/re/work/app-main-boot-graph-delta.json`,
backup `local/re/work/boot-graph.json.bak-2026-10-19-re02-appmain`, merged `local/re/boot-graph.json`,
Project store `internal/worker-claim-re02.md`.
Checkpoint: vtable+0x10→`0xb11340`, +0x18→`0x25a8d0`; options helpers; ctor installs vptr `0x15c8cd8`.
Merge: replaced unresolved vtable lines; added nodes/edges from delta (2026-10-19 19:51).

## ACTIVE CLAIM — 2026-10-19 19:50 CDT, worker bc-cb8d07c2 (CPU-01d) — COMPLETE

Task: CPU-01d — guarded vec-copy ctor RVA 0xdd41e0 (index 67). Skipped 0xf4b510 / 0x196c90.
Failed tries: 0. Status: FIXTURE_VERIFIED.
Owned: native/core/vec_dd41e0.{hpp,cpp}, tests/test_vec_dd41e0.cpp, tools/lift/check_fdd41e0.py,
CMakeLists (+vec_dd41e0 target only), local/re/work/cdd41e0.bin.
Did NOT edit: startup_ctors.*, allocator_region.*, vec_neg036.*, xyz_mask_copy.*, addr.*.

Checkpoint (2026-10-19 19:52 CDT, bc-cb8d07c2):
- OBS: guarded_vec_copy same idiom as 0xf4b510; bits 0xbeb851ea×4; cells guard@0x3352ce0 value@0x3352cd0 source@0x130eac0 (distinct from f4b510).
- Commands: ctest 10/10 Passed; `python3 -I tools/lift/check_fdd41e0.py` OK; dispatcher tie still OK.

## ACTIVE CLAIM — 2026-10-19 19:48 CDT, worker bc-71956f82 (AUD-01 headers) — COMPLETE

Task: AUD-01 headers only — classify `.sxd`/`.sxd1`/`.sxd2`/`.hd`/`.bd` magic + Shannon
entropy from live PSARC members (via `scripts/psarc.py` read). Codec left UNKNOWN.
Failed tries: 0. Status: COMPLETE (headers slice; full AUD-01 still ACTIVE — codec/streams open).
Owned files: `scripts/classify_audio_headers.py`, `scripts/test_audio_headers.py` (new),
`local/manifests/assets/audio-headers.json` (new), `docs/ASSETS.md` (AUD-01 section),
`docs/EVIDENCE.md` (E25), `docs/TASKS.md` (AUD-01 register line).
Did NOT touch: `native/core/*`; no codec/NGS2 invent.
Bounded outcome met: 86 members → magic+entropy families; codec UNKNOWN everywhere.
Exact check: `python3 -W error -I scripts/test_audio_headers.py` (11 OK);
`python3 -W error -I scripts/test_psarc.py` (10 OK).
Store report: Project `internal/worker-claim-aud01.md`.

Checkpoint (2026-10-19 19:52 CDT, bc-71956f82):
- Observed families: 44 `xff\0` hd/bd; 17 high-entropy `8c0a`/`8c06` pakN sxd*; 12 LE-small-int
  sxd*; 2 `fbad9897` bridge.sxd; outliers `qath`/`80AE`/`3ebe7b`/etc. Corrected prior claim that
  all pakN sxd* start `8c0a` (false — subset only).
- Commands above + `python3 -I scripts/classify_audio_headers.py --out local/manifests/assets/audio-headers.json`.
- Inference: none for codec. Next: game bank reader (RE/Fios) or locate missing stream corpora.

## ACTIVE CLAIM — 2026-10-19 19:48 CDT, worker bc-cd298177 (claim-2) — COMPLETE

Task: DATA-02 overlay — base vs patch PSARC mount order + case policy.
Failed tries: 0. Status: COMPLETE (PROVISIONAL_OBS; Fios mount order unproven).
Owned files: scripts/psarc.py, scripts/test_psarc.py, docs/ASSETS.md,
local/manifests/assets/CUSA08809-cachedFiles-overlay.json.
Bounded outcome met: exact-path overlay (base→patch), casefold collision reject,
exact archive basenames; 10/10 `python3 -W error -I scripts/test_psarc.py`.
Observed: patch contains all 40213 base paths + 3 new; 35734 TOC digest changes;
sources after merge all patch; casefold collisions 0.
Skipped: CPU-02a 0x19ae0 (Drive), CPU-02b 0x66dec0 (claim-3).
Next READY suggestion: AUD-01 headers (classify .sxd/.hd/.bd magic; no codec guess)
or CPU-01d next survey ctor (avoid addr.* / 0x19ae0).

Checkpoint (2026-10-19 ~19:50 CDT, bc-cd298177):
- Commands: `python3 -W error -I scripts/test_psarc.py` -> 10 passed;
  `python3 -I scripts/psarc.py --overlay base … --overlay patch … --overlay-out …` -> exit 0.
- Inference vs OBS: mount order is OBS-consistent with full patch path set; not Fios-proven.

## ACTIVE CLAIM — 2026-10-19 19:46 CDT, worker bc-cb8d07c2 (claim-3) — COMPLETE

Task: CPU-02 slice — Tagged31 13-arm dispatcher at RVA 0x66dec0 (NOT allocator 0x19ae0).
Failed tries: 0. Status: FIXTURE_VERIFIED (dispatcher slice).
Owned files: native/core/addr.{hpp,cpp}, native/tests/test_addr.cpp (dispatcher additions only),
tools/lift/check_f66dec0_dispatch.py, docs/POINTERS.md (dispatcher row), local/re/work/dispatch-66dec0.bin.
Did NOT touch: native/core/allocator_region.* (Drive owns 0x19ae0).

Checkpoint (2026-10-19 19:49 CDT, bc-cb8d07c2):
- Observed jump table @0x1270354: type2→+0x0c; 3–6→+0x20; 7–11 nop@0x66dfba; 12/14→+0x18; 13→+0x14.
- Native: `retag_field_offset` + `dispatch_retag` in addr.*; ctest 7/7; `python3 -I tools/lift/check_f66dec0_dispatch.py` OK.
- Commands: cmake --build local/build/native; ctest --test-dir local/build/native --output-on-failure -> 7/7 Passed.
- Still open for CPU-02: untagged absolute meaning; asset structs; type-name/layout for the 13 types.

## ACTIVE CLAIM — 2026-10-19, principal (claude-sonnet-5-5)

Task RE-02 (imports, relocations, boot graph). Failed tries: 0. Status ACTIVE/partial.
Owned files: scripts/ps4_dynlink.py, scripts/test_ps4_dynlink.py, scripts/disasm_range.sh,
local/re/{imports.json,unresolved.jsonl,boot-graph.json,work/}. No new Ghidra process; the
existing REA MCP / Ghidra GUI processes (pids seen at start) were left untouched.

Checkpoint 1 (2026-10-19, claude-sonnet-5-5):
- Command: `python3 -I scripts/ps4_dynlink.py local/re/eboot.elf --out local/re/imports.json
  --unresolved local/re/unresolved.jsonl --contracts reference/shadPS4/src/core/libraries
  --name-roots reference/shadPS4/src reference/ShadowOfTheColossus-PC --thunk 0xf8f420 ...
  --ctors-from 0x2dd70e0` -> exit 0. Tests: `python3 -I scripts/test_ps4_dynlink.py` 6 passed.
- Observed: 49 modules / 51 import libs / 588 relocated symbols, all named (436 shadPS4-contract,
  152 NID-hash only). Entry chain: _init_env, atexit x2, ctor runner 0x20 (573 ctors from list
  top 0x2dd70e0), app main 0x536730 ("Shadow", shadowmain.sgpr), catchReturnFromMain, exit,
  quick_exit. 0xf8f330 is an atexit handler, body only partially read.
- Inferred (not proven): roles of 0x20 and 0x536730; provisional library classification in
  local/re/boot-graph.json (reach unverified, nothing declared unused).
- Pitfall: llvm-objdump rip-relative `#` annotations are off by the --adjust-vma base; compute
  targets from the displacement (as done in the script/notes) — do not trust the annotation.
- Remaining risk: ctor bodies, 0x536aa0 & option-parser helpers, vtable slots +0x10/+0x18,
  indirect-call coverage all unresolved. Segment 10 (metadata) still unclassified.
- Next exact action: read 0xf8f330 handler fully; disassemble the first ~10 constructors from
  local/re/imports.json `constructors.entries` (last-listed run first); resolve 0x536aa0 vtable.

Checkpoint 2 (2026-10-19, claude-sonnet-5-5) — DATA-02 / AUD-01 partial, no failed tries:
- New: scripts/psarc.py (+ test_psarc.py, 6 pass with `python3 -W error -I`), docs/ASSETS.md,
  local/manifests/assets/*. Command: `python3 -I scripts/psarc.py <all 7 archives> --out
  local/manifests/assets` -> exit 0 (~41 s, no member extracted).
- Observed: all manifests decode (NUL-separated); base vs patch cachedfiles: +3 files, 35,734
  digest changes. No .at9/.wav/etc. in any archive; sound containers .sxd/.sxd1/.sxd2/.hd/.bd found
  (~4 MB) but pakN ones are high-entropy with unidentified encoding (prefix 8c0a). Music/dialogue
  location is OPEN. TOC 16-byte digest is neither MD5(path) nor MD5(data) (39 samples).
- Reindex: `python3 scripts/project_index.py` 631 folders current; graft rebuilt (log
  logs/graft-build-2.log), `graft check` OK, psarc.py queryable. test_project_tools: 3 pass.
- Resolved after checkpoint 2: 0xf8f330 is a guarded dtor runner whose list (0x2dd70f8) is empty; ctor list ends 0x2dd70e0 (0 at +8), runner order high->low.
- Next exact action: CPU-01 feasibility — pick a small pure
  constructor/function from imports.json constructors; do not guess sxd codec before Fios/loader recovery.

Checkpoint 3 (2026-10-19, claude-sonnet-5-5) — CPU-01 first slice, no failed tries:
- First native recovered function: original RVA 0xf6cad0..0xf6cbee (286 B, first-run global ctor)
  -> `native/core/math_constants.{hpp,cpp}` `sotc::init_math_constants`. Recognizer
  tools/lift/guarded_const_ctor.py matches exact encodings only (no CPU model; unknown bytes stop it).
- Commands: `cmake -S native -B local/build/native -G Ninja && cmake --build local/build/native &&
  ctest --test-dir local/build/native --output-on-failure` -> 1/1 pass, arm64 Mach-O, ASan+UBSan;
  `python3 -I tools/lift/check_f6cad0.py` -> OK (native bits == binary bits; plus IEEE oracles in the test).
- Survey (local/re/ctor-vec-copies.json): 573 ctors = 227 single constant copies, 12 other idiom
  matches, ~275 real frame-prologue functions. Observation only; not coverage of startup.
- Status: FIXTURE_VERIFIED for one leaf. G1 NOT passed: no call chain, no service, nothing wired.
- Graft scopes now scripts/tools/native/reference (log logs/graft-build-3.log); native files queryable.
- Next exact action: pick a ctor with a normal prologue that only calls memory/libc imports
  (start from the 275 list), lift it by hand with an independent fixture; or CPU-02 pointer schemas.

Checkpoint 4 (2026-10-19, claude-sonnet-5-5) — APP-01 shell, end of this turn, no failed tries:
- launcher/ SwiftPM app (LauncherCore + SoCLauncher + tests). `swift build/test --package-path launcher
  --scratch-path local/build/launcher` -> build OK, 5 Swift Testing cases pass; app ran 4 s without crash
  (no screenshot reviewed; no VoiceOver/Dynamic Type check). "Native game core" reports NOT BUILT so Play
  is disabled — it cannot boot anything. Ownership model chosen: native child process (recorded in README).
- Verification at turn end: test_project_tools 3 OK, test_psarc 6 OK, test_ps4_dynlink 6 OK, ctest 1/1,
  check_f6cad0 OK, `graft check` OK (scopes scripts/tools/native/launcher/reference; logs/graft-build-4.log).
  Folder index regenerated after this edit (now includes local/build products: ~1.7k folders; FOLDERS.md 72 KB).
- Overall status: G0 PARTIAL; G1 NOT PASSED (one fixture leaf only); G2-G6 NOT RUN. No game boot, Metal
  frame, game audio, or save has been produced. Open threads: RE-02 ctor bodies/app-main vtables,
  CPU-02 pointer schemas (READY), AUD music/dialogue location + pakN encoding, PSARC TOC digest meaning.
- Not started: CPU-02, CPU-03, SRV-01, SAV-01, AUD-02+, GPU-*. Escalations: none (0 failed tries anywhere new).

Checkpoint 5 (2026-10-19, claude-sonnet-5-5) — CPU-01 second function, no failed tries:
- Recovered RVA 0x125990 (41 B, ctor index 538): memset(0x2000 zero) + seq_cst store 0 -> native/core/scratch_reset.
  Ties: `python3 -I tools/lift/check_f125990.py` OK (template-exact, memset NID verified, 0x2008 buffer->slot gap).
  ctest 3/3 (math_constants, scratch_reset, startup_ctors); TSan build of scratch test passes (manual command in native/README).
- native/core/startup_ctors: ctor runner that stops with blocked_on_rva at an unrecovered slot (no stub success).
- Ranked survey: tools/lift/survey_ctors.py -> local/re/work/ctor-survey.json. Next-simplest ctors: 0xe10490
  (50 B, vector copy + libm `_FSin(1.0f?)` arg unverified), 0x32c70 (sceKernelGetTscFrequency; needs native clock contract).
  Do NOT claim a startup chain: 537 ctors lie between index 0 and 538.
- Next exact action: decide libm policy (bit-exact `_FSin` vs std::sin tolerance) and lift 0xe10490, or start CPU-02.

Checkpoint 6 (2026-10-19, claude-sonnet-5-5) — CPU-01 third function + libm policy, no failed tries:
- Policy L1 (native/core/libm_policy.hpp): recovered code uses sotc::libm::*; double evaluation rounded once;
  fixtures <= 1 ULP vs independent long-double oracle; bit-exactness with the original NOT claimed or testable
  (needs a Sony module's code: not authorised). Hash/save/replay/network consumers must be flagged "libm-sensitive".
  Reconsider only if the user authorises offline analysis of libc.prx or a consumer proves bit-sensitivity.
- Recovered RVA 0xe10490 (50 B, ctor index 48) -> sotc::init_trig_constants. Ties: `python3 -I tools/lift/check_fe10490.py` OK.
  Inferred (not proven): _FSin first int arg 1 = cosine (call-site statistics, tools/lift/fsin_callsites.py).
- Registry native/core/startup_ctors now holds indices 0, 48, 538; ctest 4/4. 3 of 573 ctors recovered; no startup chain.
- Observed (rip-relative scan, opcode-filtered, may miss indirect uses): cos(3deg) global 0x33533b0 is read at ONE site, 0xe0e174
  (vmovss at 0xe0e170; disp byte at ...174); the xyz-mask copy 0x33533a0 at 5 sites 0xe0bd20..0xe102ca - all inside one tight region (0xe0bd..0xe10), likely one
  vector-math unit. Next: lift/inspect the function containing 0xe0e174 to learn whether the value is bit-sensitive.
- Next exact action: the 0xe0e174 reader above, or start CPU-02;
  0x32c70 (sceKernelGetTscFrequency) needs a native clock contract (SRV-01).

Checkpoint 7 (2026-10-19, claude-sonnet-5-5) — bit-sensitivity of cos(3deg) answered, no failed tries:
- Answer: NOT bit-sensitive in a persistent sense. The sole direct reader (load at 0xe0e170, in function 0xe0e140..0xe0e833)
  computes unfused dot=((ay*by)+(bx*ax))+(az*bz) and branches on `dot > cos3deg` (unordered -> skip). A libm 1-ULP
  difference only flips a borderline comparison. Policy L1 stands. Caveat: indirect/other readers not scanned.
- Lifted only that compare fragment -> native/core/normal_threshold.hpp (+ -ffp-contract=off for all native code, since FMA
  fusion would change results). The enclosing ~1.7 KB AVX function is NOT lifted (not needed for the question; large).
- Ties: `python3 -I tools/lift/check_fe0e140.py` OK; ctest 5/5. Mistake corrected: my first disassembly used a wrong base
  (+0x100); true function start is 0xe0e140, not 0xe0e240.
- Next exact action: CPU-02 (pointer/packed-data schemas, READY) or a full lift of 0xe0e140 once its record layout
  ([rdi+0x30] list of 16-byte vec4s, [rdi+0x20] second list) is typed; ctor runner still has 3 of 573 slots.

Checkpoint 8 (2026-10-19, claude-sonnet-5-5) — CPU-02 started, no failed tries:
- Established (byte-exact): Tagged31 decode/retag rule at 0x66dec0/0x66deff, cells @0x2f3b8c0 (base) and @0x2f08000 (limit).
  Native: native/core/addr.{hpp,cpp}, tests/test_addr.cpp (boundary/overflow/alias/null/GPU40); ctest 6/6;
  `python3 -I tools/lift/check_f66dec0.py` OK. Schema doc: docs/POINTERS.md (OBS/SRC/INF/OPEN labelled).
- Not established: writer of the two cells (645+115 direct refs are all `lea`; no store found), meaning of untagged
  absolute values (image base unproven), any 40-bit layout in game code (only SRC), asset-structure schemas.
- Allocator note: custom heap manager ~0x19640..0x1b7ff uses 0xffffffff0000 page masks (16-bit granule); calls
  sceKernelReserveVirtualRange/MapFlexibleMemory at 0x19891/0x198b1 - the likely source of the heap window; unverified.
- Next exact action: trace stores into the struct containing the base/limit cells (find lea of nearby struct members +
  store through register) in the allocator init; or lift the dispatcher at 0x66dec0 (13 type arms).

Checkpoint 9 (2026-10-19, claude-sonnet-5-5) — heap window cell writers traced, no failed tries:
- Answer: allocator region R (call 0x19ae0; start=R[0x60], size=R[0x58]) -> copy-out at 0x55fe4a/0x55fe8a into Region struct
  @0x2f3b8c8 (+0x30 start, +0x38 end) and cells 0x2f07ff8 (start) / 0x2f08000 (limit = start+size); init function 0x57d5a0 sets
  base cell 0x2f3b8c0 = [0x2f3b8f8] & ~0x7fffffff (2 GiB-aligned window). Proven byte-exact by `python3 -I tools/lift/check_heap_window.py`.
- Method note: stores via register after `lea` (not rip-relative stores) - search "lea reg,[cell]; mov [reg],..." within ~48 bytes.
  Allocator state pointer G=0x2dfca78 is the out-param of sceKernelMapDirectMemory at 0x18c8f (kernel-chosen), so allocator
  state is not static.
- Native: HeapWindow::from_region + tests (round trip, exact-limit exclusion, below-start taggable, straddle/overflow rejected).
  ctest 6/6. Deliberate native strictness: rejects regions exceeding the 31-bit window.
- Still open for CPU-02: meaning of untagged absolute values; asset struct schemas; 13-arm dispatcher at 0x66dec0 not lifted.
- Next exact action: lift allocator region API 0x19ae0 contract (args edi/esi/edx/ecx; R layout) - needed for CPU-03/SRV-01 memory
  service - or the dispatcher; or an AUD/DATA thread (pakN encoding).

## Mandatory first read

Read [AGENTS](AGENTS.md), [ROADMAP](ROADMAP.md), [INDEX](docs/INDEX.md),
[TASKS](docs/TASKS.md), [EVIDENCE](docs/EVIDENCE.md), then your task specification.
Update this HANDOFF at EVERY checkpoint, failed experiment, context handoff and turn end.
After three failed distinct tries across agents, mark NEEDS_PRINCIPAL and stop that task.
Use the [checkpoint](docs/checkpoints/TEMPLATE.md) and [escalation](docs/escalations/TEMPLATE.md) templates.

## Settled user requirements

- Native ARM64 Mach-O game core, direct Metal, SwiftUI/AppKit Mac launcher.
- Actual game boot, meaningful gameplay, actual music/dialogue/effects, save/relaunch/load.
- Zero emulation: no guest CPU/register dispatch/JIT, original executable loading, runtime
  PM4/GCN processor, Rosetta/Wine/Unicorn/QEMU, Vulkan/MoltenVK production path.
- No runtime eboot.bin, boot.bin/boot-linked.bin, Sony executable module or firmware.
- Offline analysis of locally supplied eboot.bin explicitly approved. No upload needed.
- All project folders documented/indexed. User installed Xcode and suggested ShadPS4Plus extraction.
- Native functions/data may be recovered offline; relabeled x86 blobs/guest-state AOT do not qualify.

## What is actually done

1. Source audit of pinned SoTC/Bloodborne/shadPS4, with 23 verified immutable source links.
2. Native ARM64 standalone ShadPS4Plus extractor built. Crypto++ 8.9.0 is the only new
   Homebrew formula this session. Source clone/build/install details in INSTALL_LOG.
3. All 6 original local PKGs hashed and extracted separately: 124 files; seven PSARC headers.
   Base CUSA08809 APP_VER 01.00; patch APP_VER 01.01 has no eboot. Four DLCs kept separate.
4. Plaintext SELF reconstructed with pinned reader. Required segments admitted; metadata
   segment 10 is missing/unclassified. Identity is not byte-exact reconstruction.
5. REA/Ghidra entry-function analysis COMPLETED. Canonical evidence, snapshot, observed boot
   calls and function index saved. Portable evidence projection also passed.
6. Package snd0 ATRAC9 decoding checked: 48 kHz/stereo, first 5 s -> finite nonzero PCM.
   Gameplay codecs, banks, voice graphs and live sound remain unproven.
7. Detailed task cards, native-platform spec, G0-G6 gates, checkpoint/escalation rules,
   source/folder navigation and small project utilities created.

## Exact private artifact pointers

Workspace: /Volumes/Extra/SoC Mac Native. Original inputs: Shadow of the Colossus/ (read-only).
Extracted base: local/extracted/CUSA08809; update: local/extracted/CUSA08809-patch;
DLCs: local/extracted/addcont/CUSA08809. Package/output inventory: local/manifests/packages.json.

SELF SHA256: 21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0
ELF SHA256: d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3
ELF: local/re/eboot.elf. Identity/segment map: local/re/eboot.identity.json.
ELF entry RVA: 0xf120. Ghidra entry FUN_0000f120, observed body 0xf120..0xf170.
Canonical dossier: local/re/evidence/entry.json.
Evidence ID: ev_51f10a737f14992cc6215e62b361b86a073fdce4a36455feb6d18635bc8a78b4
Profile: 769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa
Function index: local/re/functions.jsonl. Boot edges: local/re/boot-seed.json.
REA snapshot: local/re/rea-snapshot.json. No saved Ghidra .gpr; temporary project deleted on CLI exit.
Pinned reader metadata: local/re/pc-loader-reference/analysis.json (588 imports, 712333 relocations,
49 needed-module declarations). Generated x86 boot.bin/entry.bin there are NEVER production assets.

## Immediate next work

Principal/recovery worker: claim RE-02. Read REVERSE_ENGINEERING and EVIDENCE.
First inspect existing entry evidence without a new Ghidra import:

    /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea inspect-analysis-view local/re/evidence/entry-view-request.json --format json

Then create/reuse a persistent private Ghidra project using the documented recipe. Resolve the
four entry PLT/GOT thunk targets through PS4 dynamic relocations/NIDs. Inspect RVA 0x20 and
0x536730 plus callback/data reference 0xf8f330. Keep application-main/constructor labels inferred
until verified. Produce typed boot/import/unresolved-edge manifests; then CPU-01 native slice.

Independent READY tasks: DATA-02 archive/overlay schemas, CPU-02 packed data schemas,
AUD-01 actual audio asset formats, APP-01 native launcher shell. A shell cannot pass game boot.
Do not start an all-game pseudocode rewrite or performance port before CPU-01 feasibility.

## Attempts, blockers and pitfalls

- SETUP-01: 1 historical failed build (Linux fseeko64 spelling); CMake-only Darwin fix,
  second build and extraction passed. Upstream checkouts unchanged.
- RE-01: 1 historical startup resource failure. MCP default 330s/2G timed out; existing public
  CLI with process-local 1200s/8G passed. No analyzers disabled or client config changed.
- In Ghidra, generic ELF import reports stripped-dynamic interpretation and other function
  overlap/decompilation warnings. Completed analysis is not proof those functions are correct.
- SoTC PC directly executes x86 code; its NGS2 fallback is silence. Bundled libc/Fios modules
  also need native replacement. Do not copy Bloodborne GPU scheduling blindly: documented SoTC regressions.
- PSARC flags/manifest formats and cachedFiles versus patch cachedfiles override semantics remain open.
- Critical feasibility tasks: native game semantics, used NGS2 behavior, shader-stage wave64/WQM.
- Real native boot/audio/save/performance: NOT RUN. Current G0 PARTIAL; G1-G6 NOT RUN.

## Verification / ownership / continuation

Utility tests: 3 passed, logs/project-tools-tests.log. Actual package inventory assertions and
ATRAC9 decode probe passed. Markdown: 17 files, 71 local links, zero errors. All four reference
pins match and their trees are clean. Graft rebuilt with 63,874 nodes / 2,980 source files; optional
meaning/deep layer is not built. All 629 non-administrative folders indexed; freshness check passed.
Final verification record: local/manifests/project-checks.json; document log: logs/project-docs-check.log.
No workers active. reference_audit completed DOC-REFERENCE-AUDIT; native_plan_review completed
DOC-PLATFORM/DOC-VALIDATION; principal integrated their checkpoints and owns this handoff.

Root initially has no .git; reference checkouts are separate pinned repositories. Never broad-add
game data. local/, logs/, original inputs, reference clones and Graft cache are publication-excluded.
Read INSTALL_LOG before installing anything. Existing Unicorn/MoltenVK are not approved runtime choices.
Refresh folders with python3 scripts/project_index.py; verify with --check and run graft check.
Graft's persisted scopes are scripts/tools/reference; append future native/launcher/tests
explicitly without dropping existing scopes, rebuild and verify queryability. A check alone
does not reveal an excluded new directory.
Preserve private Evidence and original hashes when switching agents; a snapshot is not a Ghidra project.

Earlier checkpoint chronology: [planning archive](docs/checkpoints/2026-10-19-roadmap-session.md).

## Final checkpoint — 2026-10-19 00:51 CDT, principal

Plan/documentation deliverable complete; game implementation unstarted. RE-01 static evidence
and DATA-01 artifact inventory verified; G0 PARTIAL, G1-G6 NOT RUN. Default Ghidra resource
timeout resolved with process-local CLI allowance; original evidence retained and MCP target closed.
All new downloads/installs/builds recorded. No game code executed, no native app built/published.
Final indexing caught a new log and Finder .DS_Store; Finder metadata is now excluded as
administrative content, with stale-folder detection retained. Next task remains RE-02 above.
Final source-index correction: earlier onlyDirs=[reference] omitted new helpers. Expanded to
scripts/tools/reference with nested clones enabled; all four project Python helpers now indexed.
Final checks PASSED: three utility tests; 17 Markdown files / 71 local links / zero errors;
four clean reference pins; expanded Graft graph; 629-folder inventory and freshness. The Graft
caller trace confirms inventory -> generate -> CLI/test closure. Required native game gates
remain NOT RUN. No worker or analysis process is left running by this investigation.
