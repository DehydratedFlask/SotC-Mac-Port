# Next-agent handoff — user-directed stopping point

- Recorded: 2026-10-20 22:12 CDT, America/Chicago; principal/integrator `/root`.
- User asked to reach a good stopping point and document continuation by another agent. Stop implementation and new provider queries. No new chat/task has been created; this repository handoff is the transfer record.
- Source checkpoint at drafting: `1f7c8e8` on `main`; all code changes already pushed. A final documentation commit follows. Current inputs/references remain read-only; all raw executable evidence/builds stay private.
- Gates: G0 PARTIAL; G1-G6 NOT RUN. No native game executable/boot, real game audio, actual progress save/relaunch/load or Metal gameplay is demonstrated. The project is incomplete.

## Completed in this work session

1. `SAV-01-NATIVE-CHECKSUM-01` FIXTURE_VERIFIED: explicit-seed native FNV-1a32 helper, source-bound checker and known-answer test integrated in CMake. Independent Luna review plus root223/223 full ASan+UBSan CTest pass, source checker/wrong-input rejection and Mach-O arm64 check. Published9232691. No serializer/save compatibility follows.
2. `SAV-01-LOADER-01` STATIC_ANALYSIS_VERIFIED for selected byte/call map: root28/28 checksum manifest and independent unchanged verifier replay pass, byte-identical worker output SHA64a6dac7ca8bd78958c6d03c912b2d145eeece9536f13973e88b17093ce30548. Two failed schema/claim checker attempts retained; corrected try3 passed. Native serializer/progress schema/transactions/real reload remain unimplemented. Raw GameData blob15576B vs containing record15596B.
3. `CPU-01-MUTEX-CONTRACT-01` API_CONFIGURATION_VERIFIED: installed Darwin accepts native recursive/priority-inherit attrs, reads them back and passes recursive lifecycle. Scheduler PI behavior under contention NOT TESTED; do not infer PS4 scheduling equivalence.
4. `SRV-01-RECURSIVE-MUTEX-01` FIXTURE_VERIFIED: noncopyable native pthread owner, explicit init/operation/close errors, retained ownership on EBUSY close, RAII cleanup. Unreportable destructor/attribute cleanup errors terminate visibly. Root3/3 focused sanitizer tests (`recursive_mutex`, `startup_ctors`, `save_checksum`) pass, arm64 verified. Published1f7c8e8.224 tests are registered, but the full224-test suite was NOT RUN; prior full223 was passed before adding the mutex.
5. `CPU-01-CTOR10-OBJECT-INIT-01` STATIC_ANALYSIS_VERIFIED within captured boundaries after root review: fresh corrected verifier134 assertions PASS, root stdout SHA63fbea4985613288e29067185ad710e7380604b47ee6d7d3a6ceb5f61e9c7125;44/44 private manifest entries pass. Both initializers/cleanup imports and allocation/registration operands are byte-bound. Dossier report SHAafddf4d2f5e1b71204e9fe2223db2ab2e1895bd6c3a8c07f30451e0dce7c42ac; verifier SHA380dba0033bc5c5bb93b6f1d488984a560323c37c33e46cae59d4b63d8bd278d; manifest SHA6698703ef9e84d8085cec67bcbf60a45ba67bfb3a25657e502d32d81dd54cd67. Pre-review copies retained. Worker verification.json is explicitly a review-only relabeling; the fresh run is root's separate output.
6. `CPU-01-CTOR10-DESCRIPTOR-01` STATIC_METADATA_VERIFIED: root115 assertions pass, byte-identical output SHA89e0b8a178d360e0cf0e25f768190f5c2787d5406d83a720bb26af5b7f79adfc. Final manifest checked after a prose correction (PHDR2 p_filesz0x17d3100); original report/manifest preserved under pre-review/. Current report SHA98b0e8beca612f9bc7037542f4eba6ef68e26ca5733dc5aa7037cfacff257c27; manifest SHA4dedb5238cb8fb2db74eb3f4e5f09c8bd3da1f200f7c89548234714fd853bff1. This proves publication/table metadata, not a native interface or method semantics.

## Settled choices and actual startup blocker

- The user chose **StartupState RAII ownership** for the native mutex. Do not re-ask. The standalone owner exists; it is not yet a StartupState member and slot10 is not registered.
- Earlier user decisions also remain settled: testable LauncherCore lifecycle helper and system alert for folder-import errors; do not re-open them.
- Current ordered native runner completes callbacks0-9, then fails at slot10/RVA0xf09e60. Later leaf constructors are isolated fixtures, not a longer startup prefix. Do not resume leaf batches as a substitute for this blocker.
- Slot10 also publishes pointer0x15fb7c0 into global0x3356678 before creating the16-byte mutex-handle object and registering cleanup. The published interface lacks a native typed counterpart. A mutex alone cannot justify registering this constructor.
- E53 proves the three computed calls are skipped only on the exact constructed zero-local path:0xf09efb,0xe1ee0d,0xe1ee31. Their targets/effects for other states and runtime reachability remain unknown. Current object verifier alone is not the E53 proof.
- The pointer addresspoint is not inside a PT_LOAD, but **is file-backed in PHDR1**, using the project's community PT_SCE_RELRO label(type0x61000010). Do not read PT_SCE_DYNLIBDATA header8's vaddr0 as mapped program memory. Its role here is an on-disk metadata blob. PT_SCE labels are parser/community names, not official Sony SDK evidence.
- First8 byte/RELA-bound executable candidates:0xf09350,0xf093f0,0xf09440,0xf09d80,0xe1f010,0xf09e30,0xe20050,0xe1bae0. Function boundaries/receiver semantics are not recovered by metadata. A222-entry consecutive RELA run is not proof of a222-method class or exact vtable extent. No independent consumer body has been established in reviewed captures.
- If slot10 is eventually registered, add explicit null slot11/RVA0xf09160 at original table location0x2dd7088 before later registered leaves, or an equivalent strict original-order guard. The current sparse runner checks nulls but not continuity and otherwise jumps10 to20. Slots12/13 are0xf06340/0xf058f0.

## Attempt history — preserve across agents

| Task | Unsuccessful attempts retained | Current result / limit |
|---|---|---|
| SAV-01-NATIVE-CHECKSUM-01 |0 product failures | Root accepted fixture only |
| SAV-01-LOADER-01 |2 checker schema/claim failures | Try3/root replay passed |
| CPU-01-CTOR10-OBJECT-INIT-01 |2 documented targeted failures:Swift cache denial,checker root path;1 user interruption separately | Corrected root replay passed134;2 additional reported diagnostic-read errors lack command/traceback provenance, are not accepted evidence or audited distinct recovery hypotheses |
| CPU-01-CTOR10-DESCRIPTOR-01 |2 failed inspection assumptions; preserved logs disclose transcription provenance | Try3/root replay passed115 |
| SRV-01-RECURSIVE-MUTEX-01 |2 harness/config failures:unsupported LSan option,missing scratch include | Production checks and corrected mutant rejection pass;0 demonstrated product failures |
| RE-01-PERSISTENT-01 |0 failed import tries at claim | See terminal-state section below; never restart a live import |
| Reference Graft coverage |2 ineffective build/coverage attempts | Reference scope remains missing; diagnose before another variant |

At a third unsuccessful distinct targeted attempt stop, mark NEEDS_PRINCIPAL and create the required escalation. Existing escalated0xd71f60 remains untouched; no automatic fourth attempt. Do not erase failures by changing task/agent/model/timeout. No further REA query permits remain assigned.

## Evidence navigation and exact root checks

- Original inputs: SELF SHA21982727c6f97fd415b3af5e99c5e071619c4dcc2248787a701a31a45ab0a4e0; reconstructed ELF SHAd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3,missing SELF segment10 remains explicit. Never execute the original.
- CPU object packet:local/re/evidence/cpu01-ctor10-object-init-01/. Exact root check `python3 -I local/re/evidence/cpu01-ctor10-object-init-01/verify.py` writes JSON to stdout; preserve existing worker outputs. Root separate result local/checkpoints/root-20261020-cpu-object-verification.json; root44-file checksum audit local/checkpoints/root-20261020-cpu-object-checksums.json.
- Descriptor packet:local/re/evidence/cpu01-ctor10-descriptor-01/. Root command `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/cpu01-ctor10-descriptor-01/verify_descriptor.py --out local/checkpoints/root-20261020-ctor10-descriptor-replay.json`; log local/checkpoints/root-20261020-descriptor-replay.log. Final checksum log local/checkpoints/root-20261020-descriptor-final-checksums.log. Do not rerun these accepted checks without a changed input/reason.
- Successful REA evidence IDs:initializer ev_5244df9c0cdd09628007481ae7de7c3d4adf109c3e1e1956257103fe018e264e; cleanup ev_a10b3216697bfffc2aef1ea36fbaf8614139a792153c914681a6a1f339b8c3b9. Existing caller/E53 records are linked in the private reports.
- Mutex/source/build commands and hashes:local/build/recursive-mutex-worker/commands.txt,sha256.txt; local/checkpoints/root-20261020-native-mutex-manifest.json. Source wrapper and test are committed.
- Graft native/launcher/tools/scripts refreshed and queryable; reference remains missing even after explicit full-scope build. Preserve persisted scripts/tools/reference/native/launcher/tests scopes. Use Graft first, then verified live source when scope absent; do not infer full reference coverage from a sync check. No upgrade/deep pass performed.

## Persistent analysis terminal state — verified and stopped

`RE-01-PERSISTENT-01` BASELINE_REOPEN_VERIFIED. Original import terminated exit0 after 360.619s; log reports analysis succeeded, `/eboot.elf` saved and import succeeded. No overall analysis timeout was reported. Warnings/overlap and decompiler repair errors remain retained; no complete semantic/indirect-call coverage is inferred.

Private durable project: `local/ghidra/persistent-20261020/SotC.gpr` plus `SotC.rep/`; program `/eboot.elf`. Ghidra12.1.4/OpenJDK25.0.4.1, heap8G, max4 CPUs, timeout1200s; original ELF hash checked before import. This is a distinct default headless profile, not the REA profile. No tools installed or global tool configuration replaced. Full argv/nonsecret environment/UTC timestamps/duration/exit are in `local/checkpoints/root-20261020-persistent-run.json`.

Read-only reopen terminated exit0 in 2.997s and log confirms `Processing read-only project file: /eboot.elf`. Exact command:

```sh
env JAVA_HOME=/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home GHIDRA_HEADLESS_MAXMEM=8G /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/support/analyzeHeadless '/Volumes/Extra/SoC Mac Native/local/ghidra/persistent-20261020' SotC -process eboot.elf -noanalysis -readOnly -log '/Volumes/Extra/SoC Mac Native/local/checkpoints/root-20261020-persistent-reopen.log'
```

Both root sessions86455(import) and79709(reopen) are terminal. Do not repeat the import or overwrite/delete the saved project. Preserve project/program identity, review actual memory blocks and special-segment coverage before using it for new claims. Existing logs explicitly retain stripped-dynamic-table interpretation, overlapping function-repair failures and unresolved decompiler bytes. A successful saved baseline is not recovered game behavior or a passed G1.

Final immutable evidence/log locator manifest: `local/checkpoints/root-20261020-final-handoff-evidence.json`, SHA-256 `206bcc3cd6e6c84cce351ceb0f449f0ce5b5741129f314a54f5551eb35b3b8da`. It binds import/reopen logs/metadata, `.gpr` identity and both root CPU/descriptor checks; the bulk private database is not published. Preserve it for the next agent.

## First next-agent actions

1. Read current HANDOFF.md,docs/INDEX.md,ROADMAP.md,docs/TASKS.md and the relevant CPU/RE specifications; inspect git status/remote and this checkpoint. Prior history is archived, not discarded.
2. Use the verified saved baseline and retained terminal/reopen records. Use the existing saved project if valid; no repeated six-minute import by default.
3. Claim a bounded **descriptor consumer/method semantics** task. Prefer a demonstrated consumer or first required interface entry; the first8 candidate addresses are leads, not callable native functions. Use the saved Ghidra baseline/REA with a distinct documented profile and exact byte corroboration. No new query has been dispatched.
4. Once published descriptor semantics and mutex lifetime are both established, implement the ordinary native counterpart, embed mutex ownership in StartupState per settled choice, propagate initialization failure visibly, and preserve slot11 as the next blocker. Do not store original executable addresses for dispatch or substitute an opaque fake service.
5. Continue ROADMAP gates in dependency order. Commit/push only allowlisted source/docs/tooling after staged/full path audits. Preserve existing untracked tools/graft/ and all original inputs/references.

Changed in this handoff:HANDOFF.md,docs/TASKS.md,docs/EVIDENCE.md,INSTALL_LOG.md,this checkpoint,history archive,regenerated docs/FOLDERS.md; no additional native code. Final index/Graft/diff/path-audit/commit/push results and terminal analysis state are appended before ending.
