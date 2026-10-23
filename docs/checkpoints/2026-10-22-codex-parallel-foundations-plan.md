# Codex parallel foundations plan — 2026-10-22

Base: ff7b186ccc09167248355713408447a0f0d61807. Integrator: /root.
Publication target: cursor/init-hash-551140-9dfc, explicitly chosen by the user this session.
The user also requested removal of Claude contribution credit. The 299 branch-only commits
have no Claude/Anthropic author, committer, or co-author; 70 published ancestor commits contain
Claude co-author trailers. Prepare a tree/author-preserving cleanup for review; do not force-push
under the existing AGENTS prohibition without a recorded user override.

## Global Constraints

- Native Apple Silicon ARM64 Mach-O and direct Metal remain the target. No guest CPU runtime,
  original executable loading, Sony module dependency, or firmware substitution.
- No game content or original machine-code blobs in public code/docs. Original inputs read-only;
  private analysis, binary evidence, build products, and reports stay in local/ or ignored SDD scratch.
- G0 PARTIAL; G1 PARTIAL; G2–G6 NOT RUN. A fixture or byte check does not establish gameplay.
- Preserve Drive ownership of startup_ctors.*, Init/FNV/registry. No PR5/heap-worktree integration.
- Root alone edits CMake, HANDOFF/TASKS/EVIDENCE/INDEX/VALIDATION and publication metadata.
- Workers edit only their assigned files and their private report/logs. No subagents or commits.
- Three failed hypotheses per task across sessions require NEEDS_PRINCIPAL; no automatic fourth try.
- User requested file-disjoint parallel work; use separate source ownership, then scoped reviews.

## Task 1: Isolate atomic-save temporary files

ID: SAV-01-ATOMIC-WRITER-TEMP-ISOLATION-01. Parent: SAV-01-NATIVE-STREAM-01.
Owned files: native/core/save_stream.cpp, native/core/save_stream.hpp,
native/tests/test_save_stream_file.cpp. No CMake edits.
Spec: docs/PLATFORM.md section 8 and the atomic-writer header contract.
Observed defect: predictable path.tmp opened with O_TRUNC follows pre-existing symlink/hardlink
aliases and can truncate the previous save before replacement. Zero prior failed attempts found.
User selected unique exclusive same-directory temporary files. Implement that policy.
Preserve the existing 0644/umask contract, EINTR handling, sync/close/rename error results and
same-directory replacement. Never delete or modify a stale temp owned by another invocation.
Acceptance: failing-first regressions for stale temp symlink and hardlink (including original-save
alias); complete replacement bytes; failed rename removes only the invocation-owned temp;
unrelated stale files remain intact. Existing symlink, permission, overwrite and parser cases pass.
Run focused save_stream_file CTest under ASan/UBSan. A mutation restoring the old shared-temp
open must fail a new alias regression. No concurrency claim unless deterministically tested.
Report exact commands/exits/log hashes, changed files, observations/inference and limitations.

## Task 2: Recover shared-string release helper

ID: CPU-01-SHARED-STR-RELEASE-F590-01. Parent: CPU-01.
Owned files: native/core/shared_str_release_f590.hpp, native/core/shared_str_release_f590.cpp,
native/tests/test_shared_str_release_f590.cpp, tools/lift/check_ff590.py. No existing string/Init edits.
Root adds source/test CMake entries after worker prepares code. Zero prior failed body-lift attempts.
Read existing SharedStr layout in native/core/hashed_string_14c3a0.hpp and existing checker conventions.
Pinned original: /Volumes/Extra/SoC Mac Native/local/re/eboot.elf, SHA256
 d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3.
Observed range [0xf590,0xf5d9), 73 bytes, SHA256
55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47;
wrapper [0x170b0,0x170ba), 10 bytes, SHA256
bd6d6a49a0a57e9f8f358cd4c02a743796b0e138a0098a26091b0c4c95df3928.
Use REA/Ghidra evidence for shipped-code claims and preserve/export evidence privately. Do not
install tools or repeat provider failures beyond the documented three-try limit.
Native semantics: initial null ctl is a no-op; otherwise seq-cst 64-bit atomic decrement, free only
when previous bits equal 1. Deallocate data with kind 0, reload the object's ctl after that callback,
and if nonnull deallocate the reloaded control with kind 1. No object fields are cleared.
Use a required typed deallocation dependency; no default-null successful no-op. The 0x12af0
allocator body remains unresolved and external. Native functions/data only; no guest runtime.
Acceptance: null ctl; old count 2; old count 1 exact callback order/kinds; callback clears/replaces
ctl to prove reload; zero/negative/wrap values; untouched object fields. Avoid signed overflow UB.
Use independent byte-derived offline oracle and meaningful mutants; focused ASan/UBSan fixture.
No production Init wiring or game-gate promotion. Report evidence identity, commands, hashes,
unresolved allocator dependency and whether REA observation was available.

## Task 3: Track audio argument origins over the cached CFG

ID: AUD-02-NGS2-RACKQUERY-RDX-CFG-01. Parent: AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01.
Owned files: scripts/aud02_rackquery_cfg.py and scripts/test_aud02_rackquery_cfg.py.
Keep completed scripts/aud02_rackquery_args.py unchanged. Zero prior failed continuation attempts.
Inputs (read-only, absolute root paths):
- local/validation/aud02-root-replay/rackquery-args.json, SHA256
32e866ef4213d42bf353c7709ea57383abe2e7f8523386f0fd472dd74fcd5b88.
- local/re/evidence/aud02-ngs2-rackquery-buffersize-01/function-0094ebc2/function-94ebc2.stdout.json,
SHA256 6e1a0862a285d9347cc0bd8d37910a9fd9dcb6278511e308e94d42c3441e9bd9.
- local/re/eboot.elf with the Task 2 identity.
The cache holds 109 basic blocks / 1255 assembly entries; REA 6.7.0, Ghidra 12.1.4,
ev_0fd925bf55ace3db6ec209c0b8b14dc5ed51ac17a80e18f6352aa8534aac4f3c.
Build conservative reaching-definition analysis across actual CFG predecessors, joins and loops.
Preserve multiple origins and explicit unknowns; missing edges and unsupported writes must not
be treated as proof. Crossing calls requires an explicit recorded preservation assumption.
Hash-pin inputs and reconcile cached instructions against ELF bytes. Preserve E386's 15-site
coverage and distinguish the 11 previously unresolved pointer origins (TASKS says 8 incorrectly).
Tests: unique predecessor; equal/conflicting diamonds; loops; missing edges; call clobbers and
explicit preserved-register assumption; partial register writes; incorrect input/hash rejection.
Run synthetic tests and two private analyses, requiring byte-identical outputs. Raw per-site CFG,
assembly and reports remain private; only aggregate docs/public generic tooling may be published.
Do not claim codec, ABI, reachability, playback or runtime completion.

## Integration and review

Root publishes this claim before implementation. Each worker returns one report file; root
constructs a file-based diff package and dispatches independent spec/quality review. Root runs
focused relevant checks after integration, then one full native build/test run if justified by
cross-module CMake integration. Keep every failed hypothesis and its result in the ledger.
Publish allowlisted paths only, staged/full path audits, project index, graph freshness and
fetch-verified branch sync. New commits carry no Claude/Anthropic attribution.
