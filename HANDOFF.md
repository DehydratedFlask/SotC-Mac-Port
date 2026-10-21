# SoC Mac Native — agent handoff

Updated: 2026-10-20 21:24 CDT (America/Chicago). Principal: `/root`.
No native game executable exists. Validation: G0 PARTIAL; G1-G6 NOT RUN.

## CURRENT CHECKPOINT — 2026-10-20 21:24 CDT, `/root` — native save-body checksum claimed

`SAV-01-NATIVE-CHECKSUM-01` is CLAIMED; worker `/root/aud02_waitloop_resume_audit` owns new `native/core/save_checksum.{hpp,cpp}`, `native/tests/test_save_checksum.cpp`, and `tools/lift/check_f161f50.py`. Root owns CMake/README and shared records/Git. Implement only `fnv1a32(span<const byte>, uint32_t seed)` as ordinary native C++ for the nonnegative raw-byte domain used by verified Save/Load callers; keep seed explicit. The original `0x161f50` helper zero-extends each byte, XORs with the 32-bit state and multiplies by `0x01000193` modulo 2^32; empty input returns seed. Save/Load use `0x811c9dc5` over body only. Chunk-name uppercasing belongs to a different helper and is outside this primitive.

Root reviewed the complete 38-byte helper assembly in retained REA Evidence `ev_34c436d7a66cc52871efcb9461acf9c5001062556f07f5fd8f562c87dafe103c` and replayed `PYTHONDONTWRITEBYTECODE=1 python3 -I local/re/evidence/sav01-producer-01/verify_producer.py --out local/checkpoints/root-20261020-save-checksum-producer.json` (exit0). Output matches the prior verification SHA-256 `442f3bbb438b4cdcf1826b442847e51e5916d413221bc60ec0d2c8a25284e838`; stdout log SHA-256 `56d399228d20f9b14daec28a2f287e9bca295ea479d104edc7032dc078b7c1c4`. Graft native search found no existing checksum implementation. No new provider query is assigned for this task. Failed implementation tries:0.

Acceptance: one runnable native test with independent known answers and raw high-bit/NUL/custom-seed/empty/split-update checks; original body/callsite source checker must reject wrong source input; root reviews the diff, builds ARM64 with ASan/UBSan, verifies the test binary architecture and runs appropriate CTest coverage. No complete serializer, save compatibility, startup-prefix, game or gate claim follows. G0 PARTIAL; G1-G6 NOT RUN.

CPU worker now owns the sole provider permit for the access-scoped `0xb30950` retry; the original permission failure is preserved in checkpoint `26d5282`. Save-loader worker continues private evidence construction. Reference-source Graft queries reported no indexed reference files; direct pinned-source verification was used and the 1,988-node sync result does not prove reference coverage. No dependency/install change for this checksum claim.

Exact next action: index/check/audit and publish this claim, then START the checksum worker. Root integrates CMake/README after reviewing the four new files and records real build/check outcomes before task completion.

## PRIOR CHECKPOINT — 2026-10-20 21:18 CDT, `/root` — slot-10 provider preparation failed; scoped remedy approved

`CPU-01-CTOR10-OBJECT-INIT-01` remains CLAIMED. First `rea function ... 0xb30950` invocation exited 1 in 4.9 s before target analysis: nested Ghidra stage `ownership_preparation` could not write the existing Swift module cache at `/Users/dehydratedflask/.cache/clang/ModuleCache/Swift-7JL1KBZ3A6V3.swiftmodule` (`Operation not permitted`), then could not load the arm64 macOS 27 standard library. Error says `execution_failure`, `retryable:false`; no snapshot or target Evidence ID was produced. This is one infrastructure failed try and zero failed analysis hypotheses. Exact command/capture hashes and remedy are in `docs/checkpoints/2026-10-20-root-cpu01-ctor10-provider-cache-failure.md` and private `local/re/evidence/cpu01-ctor10-object-init-01/rea-b30950.*`.

Principal reassessment: an unchanged sandbox retry cannot succeed. After this checkpoint is published, authorize one access-scoped retry using `exec_command` with `sandbox_permissions=require_escalated`, a separate verified pristine snapshot copy and `--full-output`; this directly addresses the observed cache denial. No installation, upgrade or tool configuration change. Preserve the failed capture; do not spend the second distinct function query on the same preparation failure. A further infrastructure failure must be recorded and reassessed before another query; the third unsuccessful distinct try triggers the project escalation rule.

Offline bytes already bind allocation size 16 and the initializer/cleanup call operands, but the helper/import contracts remain unresolved; no native slot-10 implementation/registration is permitted yet. The save worker continues private report/verifier construction from completed q1/q2 captures; it has no provider permit. G0 PARTIAL; G1-G6 NOT RUN.

Exact next action: index/check/audit and publish this failure checkpoint, then send CPU worker REMEDIATION START; independently review/replay all successful returned evidence before choosing a native implementation.

## PRIOR CHECKPOINT — 2026-10-20 21:13 CDT, `/root` — resume at the native startup blocker

`CPU-01-CTOR10-OBJECT-INIT-01` is CLAIMED by root with worker `/root/startup_critical_path_resume`. Worker owns only private `local/re/evidence/cpu01-ctor10-object-init-01/`; root owns shared records and Git. Characterize initializer `0xb30950`, allocation-size pointer helper `0xb30a10` and cleanup callback `0xe44a10` for original constructor slot 10. Existing evidence shows the examined zero-state path skips the unresolved computed calls, but object initialization and native ownership remain unestablished. The runner still stops after callbacks 0-9 at `{0xf09e60, 10, nullptr}`; later leaf fixtures do not advance that prefix. Do not register slot 10 until every required effect on this path is recovered. At most two distinct REA/Ghidra queries (`0xb30950`, `0xe44a10`), after the existing SAV query terminates; no automatic same-target retry. Specification: CPU-01 work packet and REVERSE_ENGINEERING Stage 2. No failed analysis hypotheses for this new task; inherited slot-10 verifier correction remains recorded. Escalated `0xd71f60` stays untouched.

`SAV-01-LOADER-01` is ACTIVE under root integration with worker `/root/sav01_loader_resume_audit`. q1 (`0xa81180`) has a saved successful provider record, but no loader report/checker/checksum manifest exists yet. q2 (`0x542880`) was confirmed live at 21:10:51 CDT: wrapper PID 17959, runner 21478, REA children 21480/21481; exact `pgrep -alf 'aud02-waitloop-01|sav01-loader-01|run_rea_function.py|run_q1_q2.sh'` capture is `local/checkpoints/root-20261020-resume-processes.log`, SHA-256 `874c5ffa12cb03b35af6071c6e5da2d7d3b01f29d6ff6acabf27e1f233b03258`. This is a verified wait, not a failure or permission to restart. Worker will complete only private semantic evidence after the run ends; root must inspect and replay it before accepting claims. No new provider queries for the save task.

Root subsequently inspected q2's terminal `run.json` and shell-exit record: exit 0, valid JSON, no timeout, 376.068 s, finished 21:12:38 CDT, Evidence `ev_8f5eed1f8c9e216017843e816dfff6c4ff7d0ebc2f53faa13642d0d9627bc9b0`. Run-record SHA-256 `8dec300cc1da0fcee24b38b451c9af1adc104d81fc5c17191aad023a530a296d`; stdout SHA-256 `9a6c62af293b9eba44944de5cb938c334e39646cc003f2999fdc68aba0725ed0`. The saved run is terminal, so the CPU provider permit can be released after claim publication. Save semantics remain unaccepted until the worker's checker is reviewed and root-replayed.

`python3 scripts/project_index.py` exited 0 (5,584 folders; 153 checkpoint files; log `local/checkpoints/root-20261020-resume-index.log`, SHA-256 `fc0f4ab584dfdd19c3fd0ea786425d8fa325af16b427b620d1839194eaf93e5f`). `graft check` exited 0, wiring graph in sync at 1,988 nodes; deep layer remains unbuilt (log SHA-256 `c74d5fe747cf7de34cc2fd604ea1b7b31a9bf004024dc8729ae033ddf8c3891c`). `git diff --check` exited 0 (empty-log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).

`AUD-02-WAITLOOP-01` has only a copied helper and no report/verifier/provider capture; no task-specific process was found by the same live process query, and REA MCP reports no open target/cleanup. It is READY for a later bounded claim; no completion or provider-failure count is inferred from missing artifacts.

Current source: `fe87a20c0e2a9355c9e94c1f6b8a4432bbf1d354`, clean tracked tree before this claim; pre-existing `tools/graft/` remains unstaged. `git fetch origin` exited 0; divergence 0/0. Root `sha256sum local/re/eboot.elf` matched `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. No install/dependency changes and no code/runtime result. Previous goal turn was progress (fixture and pause checkpoint published); this continuation has revalidated the live state and selected the remaining startup dependency. Full claim: `docs/checkpoints/2026-10-20-root-critical-path-resume-claim.md`.

Exact next action: index/check/audit and publish these claims, then authorize the save-evidence worker; release the CPU provider permit only after the existing save process has a terminal record. Independently replay returned checkers, publish the results, then decide the native slot-10 implementation boundary.

## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 24 (0xbc07f0 .. 0xbb9740) lifted

`CPU-01-LEAF-CTOR-BATCH-24` `FIXTURE_VERIFIED`; root replay 222/222 ctest pass, six checkers exit 0; README-vs-ELF cross-check over all vec rows: 0 mismatches. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-24-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `SAV-01-LOADER-01` (`local/re/evidence/sav01-loader-01/`) and `AUD-02-WAITLOOP-01` (`local/re/evidence/aud02-waitloop-01/`). Next constructor position 196 `0xbb82d0`.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — AUD-02 second tick driver characterized

`FUN_00997700` is a request-driven pull entry (guarded API wrapper `FUN_009975f0` with no static callers; tick call `FUN_009536d0` only when leftover count is zero); `FUN_00997970` pumps the tick inside a wait loop in `FUN_00986a50`. Roles inference; it is unknown whether the second driver ever runs. 2 REA successes; root replayed verifier (38/38 worker mutants rejected) and checksums. Details: `docs/checkpoints/2026-10-20-root-aud02-tickdriver-callers-complete.md`. No audio. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 216/216 (pushed `d7f42cd`).

Next exact action: `FUN_00986a50`'s callers (`0x986e1c`, `0x989556`) or the guard-counter initializer; SAV-01 bool-chunk reader `0xa81180`/profile writers/load deserializer `0x5435f0`; constructor batch 24 (next position 190 `0xbc07f0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — SAV-01 flush path and game-data blob structure decoded

Stream flush = three-class chain (slot `[vtbl+0x10]` → `0x4d420`, staging buffer `0x1000`, growable vector, length back-patch, verbatim memcpy into `[obj+0x60]`). Game-data blob producer `0x7e5bb0` is a 14-byte accessor (`.bss` `0x3260bb8`, `0x3cd8` bytes); chunk = tag `0xd` + u64 len + raw bytes (15596 B total); 48 bool chunks precede it. 2 REA successes; root replayed verifier (127/127 worker mutations caught) and checksums. Details: `docs/checkpoints/2026-10-20-root-sav01-blob-flush-complete.md`. Blob semantics unknown; no native serializer; no save works. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 216/216 (batch 23, pushed `d7f42cd`).

Next exact action: bool-chunk reader `0xa81180`, profile chunk writers (`0x548d30`..), load-side deserializer `0x5435f0`; pending `AUD-02-TICKDRIVER-CALLERS-01` worker; constructor batch 24 (next position 190 `0xbc07f0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 23 lifted; four more README rows fixed

`CPU-01-LEAF-CTOR-BATCH-23` (`0xbcc4e0` .. `0xbc2800`) `FIXTURE_VERIFIED`; root replay 216/216 ctest pass, six checkers exit 0. All `vec_*` README rows were cross-checked against the ELF; four batch-14 rows had wrong src/dst and are fixed (code/tests were fine). Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-23-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `SAV-01-BLOB-FLUSH-01` (`local/re/evidence/sav01-blob-flush-01/`) and `AUD-02-TICKDRIVER-CALLERS-01` (`local/re/evidence/aud02-tickdriver-callers-01/`). Next constructor position 190 `0xbc07f0`.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — SAV-01 serializer container format decoded statically

The save body is a tagged-chunk stream (12-byte chunk headers: name hash u32 / `0xBEEF` / tag u16 / back-patched length u32; FNV-1a-32 over uppercased field names). Two concrete save-object classes resolved (vtables `0x15c97a8`, `0x15c9830`; slot `+0x50` targets `0x543f40`, `0x548d30`). 2 REA successes; root replayed verifier (55/55 worker mutations caught) and checksums. Details (field names private): `docs/checkpoints/2026-10-20-root-sav01-serializer-complete.md`. Blob contents unknown; no native serializer; no save works. G0 PARTIAL, G1-G6 NOT RUN.

Next exact action: game-data blob producer `0x7e5bb0` and stream flush slot `[vtable+0x10]` of vtable `0x15d2450`; AUD-02 callers of `FUN_00997700`; constructor batch 23 (next position 184 `0xbcc4e0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — AUD-02 audio region dossier verified

Audio call graph (36 functions, 26 direct edges; byte scan = Ghidra): init `FUN_009485a0` / shutdown `FUN_00949fe0`, an Audio3d push thread (`FUN_00949f80`), a mixer tick (`FUN_009536d0`: SystemRender + queue level + advance) driven by an AudioOut thread (`FUN_009ac390`) and a second driver `FUN_00997700`. Roles are inference. 2 REA successes; root replayed verifier and checksums. Details (strings private): `docs/checkpoints/2026-10-20-root-aud02-region-dossier-complete.md`. No decoder/mixer, no audio. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 210/210 (batch 22, pushed `19bdb6c`).

Next exact action: record `FUN_009975f0`/`FUN_009979af` (callers of `FUN_00997700`) or the 8 unrecorded audio callsites; pending `SAV-01-SERIALIZER-01` worker (`local/re/evidence/sav01-serializer-01/`); constructor batch 23 (next position 184 `0xbcc4e0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 22 (0xbd32f0 .. 0xbcc960) lifted

`CPU-01-LEAF-CTOR-BATCH-22` `FIXTURE_VERIFIED`; root replay 210/210 ctest pass, six checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-22-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `SAV-01-SERIALIZER-01` (`local/re/evidence/sav01-serializer-01/`) and `AUD-02-REGION-DOSSIER-01` (`local/re/evidence/aud02-region-dossier-01/`). Next constructor position 184 `0xbcc4e0`.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — SAV-01 header/hash semantics established statically

Save/Load paths traced: 16-byte header {1, FNV-1a-form hash of body only, body size ≤ `0x6400000`, dword 3 from `[[this+8]]`}; Load checks version, size cap and body hash; header/body producers `FUN_00a56080` / `FUN_00a55450`. 1 REA success; root replayed verifier (27/27 mutations caught per worker) and checksums. Details: `docs/checkpoints/2026-10-20-root-sav01-producer-complete.md`. Body serializer is the vtable slot `+0x50` target — unresolved; no native serializer, no save works. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 204/204 (batch 21, pushed `5abd813`).

Next exact action: resolve the vtable holding slots `+0x40/+0x50` for `this` at `FUN_00a56080`'s callers (`0x5400f3`, `0x5452d3`, `0x54b4c4`), then one REA query on the slot `+0x50` target; constructor batch 22 (next 178 `0xbd32f0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 21 lifted; three session-limit interruptions recovered

Batch 21 (`0xe094e0`, `0xdbfa60`, `0xd85490`, `0xbd7950`, `0xbd60a0`, `0xbd4dd0`) `FIXTURE_VERIFIED`; root replay on a fresh build 204/204 ctest pass, six checkers exit 0. Gap audit: earlier manifest gaps 4/18/28 now lifted; 41 (`0xd71f60`) stays `NEEDS_PRINCIPAL`. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-21-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN.

Session limit hit mid-round; workers resumed. The five SaveData dialog helpers are now root-replayed and verified (`docs/checkpoints/2026-10-20-root-savedata-dialog-helpers-complete.md`; every SaveData import has a verified callsite). `SAV-01-PRODUCER-01` (`local/re/evidence/sav01-producer-01/`) is still unreviewed — replay its verifier before any claim. Next constructor candidate: position 178 `0xbd32f0`.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — stragglers: 2 attributed, 1 provider failure

`0x954726` → `FUN_00954680`, `0xa4a903` → `FUN_00a4a600` (2 REA successes; root replayed verifiers and checksums). `0xa4bddb` (SaveData Delete): REA exit 1 "No procedure exists" (address lies in a gap of `FUN_00a4ab90`); one provider failure, not retried; offline inference only. Details: `docs/checkpoints/2026-10-20-root-stragglers-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 198/198.

Next exact action: SAV-01 producer-of-body/header step (caller of `FUN_00a51180`); remaining dialog helper imports; constructor batch 21 (next 175 `0xbd7950`). `0xa4bddb` would need a different provider operation (e.g. a function-creation-free disassembly window), not a retry of the same query.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — SAV-01 region dossier verified

First function-level save-region characterization: dispatcher `FUN_00a4d330` (task dword → `FUN_00a47d60` / `FUN_00a4ab90` / `FUN_00a48280`), load/save call orders, and hash helper `FUN_00161f50` (FNV-1a 32-bit form, offset basis passed by both load and save). 2 REA successes; root replayed verifier and checksums. Details (strings kept private): `docs/checkpoints/2026-10-20-root-sav01-region-dossier-complete.md`. Header layout and hash coverage are inference only; no save works. SAV-01 → ACTIVE. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 198/198 (batch 20, pushed `0d2b855`).

Next exact action: producer-of-body/header step (caller of buffer-object constructor `FUN_00a51180`, one REA query + byte verification); pending `STRAGGLERS-01` worker (`local/re/evidence/stragglers-*-01/`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 20 (0xc060a0 .. 0xbd7a80) lifted

`CPU-01-LEAF-CTOR-BATCH-20` `FIXTURE_VERIFIED`; root replay on a fresh build 198/198 ctest pass, six checkers exit 0; manifest position 169 (`0xc060a0`) had been missed by batch 19 and is now covered. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-20-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `STRAGGLERS-01` (`local/re/evidence/stragglers-*-01/`, 3 REA queries) and `SAV-01-REGION-DOSSIER-01` (`local/re/evidence/sav01-region-dossier-01/`, ≤2 REA queries).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — SaveData imports have verified callsites

Ten more `libSceSaveData` imports are byte-verified (1 new REA success for `sceSaveDataTerminate`, 9 reuses; root replayed all verifiers and checksums). Most callsites cluster in `FUN_00a48280`, `FUN_00a4a940`, `FUN_00a4ab90` (save-data mount/dialog/icon code region `0xa47d60..0xa4d326`). Details: `docs/checkpoints/2026-10-20-root-savedata-remaining-imports-complete.md`. Static only — no reachability/runtime/ABI/save-format claim. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 192/192 (batch 19, pushed `1443b27`).

Next exact action: constructor batch 20 (next 169 `0xbe5410`), unattributed stragglers (`0x954726`, `0xa4a903`, `0xa4bddb`), remaining dialog helpers (`sceSaveDataDialogClose/GetStatus/IsReadyToDisplay/ProgressBarSetValue/Terminate`), or the SAV-01 serializer/checksum recovery that these callsite bodies now make approachable (needs a fresh claim and function-level decompilation).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 19 (0xc0bea0 .. 0xc09070) lifted; README rows fixed

`CPU-01-LEAF-CTOR-BATCH-19` `FIXTURE_VERIFIED`; root replay 192/192 ctest pass, six checkers exit 0. Also corrected five wrong batch-18 README rows (code/tests were fine). Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-19-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: SaveData dialog-import probes (`local/re/evidence/sav01-savedata-dialog*-01/` etc.) and SaveData misc-import probes (`getmountinfo`, `saveicon`, `delete`, `terminate`, `checkbackupdata`, `restorebackupdata`), up to 3 REA queries each.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — all Audio3d imports have a verified callsite

`PortSetAttribute` (`0x948e79`), `Terminate` (`0x948e16`), `PortGetQueueLevel` (`0x953a68`), `PortClose` (`0x94a11e` in `FUN_00949fe0`) and the second `ObjectUnreserve` callsite (`0x99b880` in `FUN_0099b850`) are byte-verified (2 new REA successes, 3 reuses; root replayed all verifiers and checksums). Details: `docs/checkpoints/2026-10-20-root-audio3d-remaining-imports-complete.md`. Static only — no reachability/runtime/ABI claim. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 186/186 (batch 18, pushed `4320b2e`).

Next exact action: SaveData dialog imports (`sceSaveDataDialog*`, `sceSaveDataSaveIcon`, `sceSaveDataGetMountInfo`, `sceSaveDataCheckBackupData`, `sceSaveDataDelete`, `sceSaveDataRestoreBackupData`, `sceSaveDataTerminate`), remaining unattributed Audio3d candidates (`0x954726`), or constructor batch 19 (next 163 `0xc0bea0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 18 (0xc1a9a0 .. 0xc0d720) lifted

`CPU-01-LEAF-CTOR-BATCH-18` `FIXTURE_VERIFIED`; root replay 186/186 ctest pass, six checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-18-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: Audio3d `PortGetQueueLevel`/`PortClose`/`ObjectUnreserve-02` (`local/re/evidence/aud02-audio3d-portgetqueuelevel-01/`, `-portclose-01/`, `-objectunreserve-02/`) and `PortSetAttribute`/`Terminate` (`-portsetattribute-01/`, `-terminate-01/`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — Audio3d ObjectReserve fully attributed; ObjectUnreserve callsite verified

All nine `sceAudio3dObjectReserve` candidates are in Ghidra bodies (eight in `FUN_0099b400` from existing evidence); `sceAudio3dObjectUnreserve` callsite `0x949e58` is in `FUN_00949dc0` (also holds NGS2 VoiceHandle callsite `0x949df4`; `0x99b880` unattributed). 1 new REA success; root replayed both verifiers and checksums. Details: `docs/checkpoints/2026-10-20-root-audio3d-objectreserve-02-and-objectunreserve-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 180/180 (batch 17, pushed `2163b26`).

Next exact action: remaining Audio3d imports (`PortGetQueueLevel`, `PortClose`, `PortSetAttribute`, `Terminate`), `0x99b880`, SaveData dialog imports, or constructor batch 18 (next 157 `0xc1a9a0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 17 (0xc37020 .. 0xc20710) lifted

`CPU-01-LEAF-CTOR-BATCH-17` `FIXTURE_VERIFIED`; root replay 180/180 ctest pass, five checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-17-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `AUD-02-AUDIO3D-OBJECTRESERVE-02` (`local/re/evidence/aud02-audio3d-objectreserve-02/`) and `AUD-02-AUDIO3D-OBJECTUNRESERVE-01` (`local/re/evidence/aud02-audio3d-objectunreserve-01/`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — Audio3d PortAdvance and ObjectReserve callsites verified

`sceAudio3dPortAdvance` sole callsite `0x953a7c` (in `FUN_009536d0`) and `sceAudio3dObjectReserve` callsite `0x94a6b8` (in `FUN_0094a510`; 8 of 9 candidates unattributed) are byte- and REA-verified (2 provider successes on pristine snapshot copies; root replayed both verifiers and checksums). Details: `docs/checkpoints/2026-10-20-root-audio3d-portadvance-and-objectreserve-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 175/175 (batch 16, pushed `29a9fb7`).

Next exact action: remaining Audio3d imports (`ObjectUnreserve`, `PortGetQueueLevel`, `PortClose`, `PortSetAttribute`, `Terminate`), the eight `0x99b5xx..0x99b6xx` ObjectReserve candidates (probably one function), SaveData dialog imports, or constructor batch 17 (next 152 `0xc37020`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 16 (0xc451d0 .. 0xc3cf00) lifted

`CPU-01-LEAF-CTOR-BATCH-16` `FIXTURE_VERIFIED`; root replay 175/175 ctest pass, five checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-16-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `AUD-02-AUDIO3D-PORTADVANCE-01` (`local/re/evidence/aud02-audio3d-portadvance-01/`) and `AUD-02-AUDIO3D-OBJECTRESERVE-01` (`local/re/evidence/aud02-audio3d-objectreserve-01/`), one REA query each.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — Audio3d Initialize and ObjectSetAttributes callsites verified

`sceAudio3dInitialize` sole callsite `0x948db4` (in `FUN_009485a0`, shared with PortOpen and six NGS2 VoiceHandle callsites) and `sceAudio3dObjectSetAttributes` callsite `0x94da39` (in `FUN_0094d900`; 2 of 3 candidates still unattributed) are byte-verified; 1 new REA success; root replayed both verifiers and checksums. Details: `docs/checkpoints/2026-10-20-root-audio3d-initialize-and-objectsetattributes-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 170/170 (batch 15, pushed `d88f529`).

Next exact action: remaining Audio3d imports (`PortAdvance`, `ObjectReserve`, `ObjectUnreserve`, `PortGetQueueLevel`, `PortClose`, `PortSetAttribute`, `Terminate`), SaveData dialog imports, or constructor batch 16 (next 147 `0xc451d0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 15 (0xc54830 .. 0xc47e00) lifted

`CPU-01-LEAF-CTOR-BATCH-15` `FIXTURE_VERIFIED`; root replay 170/170 ctest pass, five checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-15-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `AUD-02-AUDIO3D-INITIALIZE-01` (`local/re/evidence/aud02-audio3d-initialize-01/`) and `AUD-02-AUDIO3D-OBJECTSETATTRIBUTES-01` (`local/re/evidence/aud02-audio3d-objectsetattributes-01/`), one REA query each.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — Audio3d PortOpen and PortPush callsites verified

`sceAudio3dPortOpen` sole callsite `0x948e09` (in `FUN_009485a0`, which also holds six `sceNgs2RackGetVoiceHandle` callsites) and `sceAudio3dPortPush` sole callsite `0x949f95` (in `FUN_00949f80`) are byte- and REA-verified (2 provider successes on pristine snapshot copies; root replayed both verifiers and all checksum sets). Details: `docs/checkpoints/2026-10-20-root-audio3d-portopen-and-portpush-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN. Native suite last verified 165/165 (batch 14, pushed `c03c24a`).

Next exact action: remaining Audio3d imports (`sceAudio3dInitialize`, `PortAdvance`, `ObjectReserve`, `ObjectSetAttributes`, ...), SaveData dialog imports, or constructor batch 15 (next 142 `0xc54830`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 14 (0xc5c040 .. 0xc54d20) lifted

`CPU-01-LEAF-CTOR-BATCH-14` `FIXTURE_VERIFIED`; root replay 165/165 ctest pass, five checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-14-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `AUD-02-AUDIO3D-PORTOPEN-01` (`local/re/evidence/aud02-audio3d-portopen-01/`) and `AUD-02-AUDIO3D-PORTPUSH-01` (`local/re/evidence/aud02-audio3d-portpush-01/`), one REA query each.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — DirNameSearch callsite and all AudioOutOutput callsites verified

`sceSaveDataDirNameSearch` sole callsite `0xa47f58` (in `FUN_00a47d60`) and all 5 `sceAudioOutOutput` callsites (`FUN_009ab360`, `FUN_009abed0`, `FUN_009aca40`) are byte- and REA-verified (3 provider successes; root replayed verifiers and checksums). Caveat: the DirNameSearch run used a modified snapshot copy (hash recorded). Details: `docs/checkpoints/2026-10-20-root-savedata-dirnamesearch-and-audioout-output-02-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.

Next exact action: `sceAudio3d*` import probes, more constructor batches (next 137 `0xc5c040`), or the remaining SaveData dialog imports.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors batch 13 (0xc64060 .. 0xc5c1a0) lifted

`CPU-01-LEAF-CTOR-BATCH-13` `FIXTURE_VERIFIED`; root replay 160/160 ctest pass, five checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-13-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending workers: `SAV-01-SAVEDATA-DIRNAMESEARCH-01` (`local/re/evidence/sav01-savedata-dirnamesearch-01/`) and `AUD-02-AUDIOOUT-OUTPUT-02` (`local/re/evidence/aud02-audioout-output-02/`), up to 3 REA queries total.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — SaveData Initialize3 verified; usage limit reached

`SAV-01-SAVEDATA-INITIALIZE3-01` `STATIC_PATH_BYTE_VERIFIED` (callsite `0xa4d295` in `FUN_00a4d250`; root replayed verifier and checksums): `docs/checkpoints/2026-10-20-root-savedata-initialize3-complete.md`. Native suite last verified 155/155 (batch 12, pushed `07eac0d`). G0 PARTIAL, G1-G6 NOT RUN.

RESOLVED after the limit notice: `SAV-01-SAVEDATA-UMOUNTWITHBACKUP-01` finished, was replayed by root (verifier exit 0, checksums OK) and is `STATIC_PATH_BYTE_VERIFIED` (`0xa48db9` in `FUN_00a48280`): `docs/checkpoints/2026-10-20-root-savedata-umountwithbackup-complete.md`. No workers remain open. Next constructor candidate: manifest position 132 `0xc64060`. Other unprobed imports: `sceSaveDataDirNameSearch`, `sceAudio3d*`, three remaining `sceAudioOutOutput` callsites.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xc686e0, 0xc66580, 0xc65f80, 0xc65da0 lifted

`CPU-01-LEAF-CTOR-BATCH-12` `FIXTURE_VERIFIED`; root replay 155/155 ctest pass, four checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-12-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: `SAV-01-SAVEDATA-INITIALIZE3-01` and `SAV-01-SAVEDATA-UMOUNTWITHBACKUP-01` workers (one REA query each; evidence under `local/re/evidence/sav01-savedata-initialize3-01/` and `local/re/evidence/sav01-savedata-umountwithbackup-01/`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — AudioOutOpen and SaveDataSetParam callsites verified

`sceAudioOutOpen` sole callsite `0x9ac2dd` (in `FUN_009ac1f0`) and `sceSaveDataSetParam` sole callsite `0xa4bcb1` (in `FUN_00a4ab90`) are byte- and REA-verified (2 provider successes; root replayed both verifiers and all checksum sets). Details: `docs/checkpoints/2026-10-20-root-audioout-open-and-savedata-setparam-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.

Next exact action: remaining unprobed imports (`sceSaveDataInitialize3`, `sceSaveDataDirNameSearch`, `sceSaveDataUmountWithBackup`, `sceAudio3d*`, remaining `sceAudioOutOutput` candidates) or more constructor batches (next 128 `0xc686e0`, 129 `0xc66580`, 130 `0xc65f80`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xc6fdf0, 0xc6d910, 0xc6bad0, 0xc69f70 lifted

`CPU-01-LEAF-CTOR-BATCH-11` `FIXTURE_VERIFIED`; root replay 151/151 ctest pass, four checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-11-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: `AUD-02-AUDIOOUT-OPEN-01` and `SAV-01-SAVEDATA-SETPARAM-01` workers (one REA query each; evidence under `local/re/evidence/aud02-audioout-open-01/` and `local/re/evidence/sav01-savedata-setparam-01/`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — AudioOut and SaveData first callsites verified

`sceAudioOutOutput` callsite `0x9abdc2` (in `FUN_009ab360`) and `sceSaveDataMount2` sole callsite `0xa4aa60` (in `FUN_00a4a940`) are byte- and REA-verified (2 provider successes; root replayed both verifiers and all checksum sets). Details: `docs/checkpoints/2026-10-20-root-audioout-and-savedata-callsites-complete.md`. Static only — no reachability/runtime/ABI/save-format claim. G0 PARTIAL, G1-G6 NOT RUN.

Next exact action: other unprobed imports (`sceAudioOutOpen`, `sceSaveDataSetParam`, `sceSaveDataInitialize3`, Audio3d) or more constructor batches (next candidates 124 `0xc6fdf0`, 125 `0xc6d910`, 126 `0xc6bad0`).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xc786c0, 0xc77070, 0xc741c0, 0xc70e20 lifted

`CPU-01-LEAF-CTOR-BATCH-10` `FIXTURE_VERIFIED`; root replay 147/147 ctest pass, four checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-10-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: new workers `AUD-02-AUDIOOUT-OUTPUT-01` (`sceAudioOutOutput` callsite) and `SAV-01-SAVEDATA-MOUNT2-01` (`sceSaveDataMount2` callsite), each one REA query, evidence under `local/re/evidence/aud02-audioout-output-01/` and `local/re/evidence/sav01-savedata-mount2-01/`.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — all 104 VoiceHandle candidates Ghidra-confirmed

Last three heuristic candidates confirmed (`FUN_00953340`, `FUN_009533d0`, `FUN_00953470`; 3 REA successes; root replayed both verifiers and checksum sets). The `sceNgs2RackGetVoiceHandle` thunk `0xf91410` candidate list is statically complete: 104 of 104 Ghidra-confirmed. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-08-complete.md`. Static only — no reachability/runtime/ABI claim. G0 PARTIAL, G1-G6 NOT RUN.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xc891e0, 0xc7de60, 0xc7c310 lifted

`CPU-01-LEAF-CTOR-BATCH-09` `FIXTURE_VERIFIED`; root replay 143/143 ctest pass, three checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-09-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: two VoiceHandle final-interval workers (9a, 9b; 3 REA queries).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — VoiceHandle intervals 07A/07B confirmed

Four more Ghidra bodies confirmed (`FUN_00949dc0`, `FUN_00949f00`, `FUN_00953220`, `FUN_00952e10`; 4 REA successes; root replayed both verifiers and checksum sets). 101 of 104 candidates Ghidra-confirmed; 3 heuristic-only (`0x95338e, 0x95341e, 0x9534be`). Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-07-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xca4dc0, 0xc9e8e0, 0xc91b10 lifted

`CPU-01-LEAF-CTOR-BATCH-08` `FIXTURE_VERIFIED`; root replay 140/140 ctest pass, three checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-08-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: two VoiceHandle interval workers (8a, 8b; REA queries).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — VoiceHandle intervals 0x94dfe0 and 0x94e270 confirmed

`FUN_0094dfe0` and `FUN_0094e270` Ghidra-confirmed (2 REA successes; root replayed verifier and checksums). 97 of 104 candidates Ghidra-confirmed, 7 heuristic-only. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-06-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xcae740, 0xcae240, 0xcad450 lifted

`CPU-01-LEAF-CTOR-BATCH-07` `FIXTURE_VERIFIED`; root replay 137/137 ctest pass, three checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-07-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: VoiceHandle intervals-06 worker (2 REA queries).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — VoiceHandle intervals 0x9549e0 and 0x94e3f0 confirmed

`FUN_009549e0` and `FUN_0094e3f0` Ghidra-confirmed (2 REA successes; root replayed verifier and checksums). 95 of 104 candidates Ghidra-confirmed, 9 heuristic-only across 9 intervals. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-05-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xcbb2c0, 0xcb5cb0, 0xcaeed0 lifted

`CPU-01-LEAF-CTOR-BATCH-06` `FIXTURE_VERIFIED`; root replay 134/134 ctest pass, three checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-06-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: VoiceHandle intervals-05 worker (2 REA queries).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — VoiceHandle intervals 0x94a200 and 0x94dc50 confirmed

`FUN_0094a200` and `FUN_0094dc50` Ghidra-confirmed (2 REA successes; root replayed verifier and checksums). 92 of 104 candidates Ghidra-confirmed, 12 heuristic-only across 11 intervals. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-04-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xcc8330, 0xcc5a10, 0xcc07d0 lifted

`CPU-01-LEAF-CTOR-BATCH-05` `FIXTURE_VERIFIED`; root replay 131/131 ctest pass, three checkers exit 0 (optional dump sub-check skipped). Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-05-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: VoiceHandle intervals-04 worker (2 REA queries).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — VoiceHandle intervals 0x9530c0 and 0x953ad0 confirmed

`FUN_009530c0` and `FUN_00953ad0` Ghidra-confirmed (2 REA successes; root replayed verifier and checksums). 88 of 104 candidates Ghidra-confirmed, 16 heuristic-only across 13 intervals. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-03-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xcceaa0, 0xccd0f0, 0xcc8ac0 lifted

`CPU-01-LEAF-CTOR-BATCH-04` `FIXTURE_VERIFIED`; root replay 128/128 ctest pass, three checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-04-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: VoiceHandle intervals-03 worker (2 REA queries).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — VoiceHandle intervals 0x951df0 and 0x94aaf0 confirmed

`FUN_00951df0` and `FUN_0094aaf0` Ghidra-confirmed (2 REA successes, 0 failures; root replayed verifier and checksums). 82 of 104 candidates now Ghidra-confirmed, 22 heuristic-only across 15 intervals. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-02-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctors 0xcde740, 0xcd4d60, 0xcd0420 lifted

`CPU-01-LEAF-CTOR-BATCH-03` `FIXTURE_VERIFIED`; root replay 125/125 ctest pass, three checkers exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-03-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: VoiceHandle intervals worker (2 REA queries).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — VoiceHandle interval 0x950850 confirmed

`FUN_00950850` (`0x950850..0x951deb`) Ghidra-confirmed; 27 heuristic candidates upgraded (35 heuristic-only remain across 17 intervals, largest `0x951df0..0x952e01`). 1 REA success; root replayed verifier and checksums. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-interval-950850-complete.md`. Static only. G0 PARTIAL, G1-G6 NOT RUN.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctor 0xce38d0 lifted

`CPU-01-LEAF-CTOR-NEXT-02` `FIXTURE_VERIFIED`: `native/core/vec_ce38d0.*`, test, `tools/lift/check_fce38d0.py`; root replay 122/122 ctest pass, checker exit 0; README now documents `0xce6fb0`/`0xce67b0` too. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-ce38d0-complete.md`. No runtime claim; G0 PARTIAL, G1-G6 NOT RUN. Pending: VoiceHandle interval `0x950850..0x951deb` REA worker.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — leaf ctor 0xce6130 lifted

`CPU-01-LEAF-CTOR-NEXT-01` `FIXTURE_VERIFIED`: `native/core/vec_ce6130.*`, test, and `tools/lift/check_fce6130.py`; root replay 121/121 ctest pass, checker exit 0. Details: `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-ce6130-complete.md`. Not registered in `startup_ctors`; no runtime claim. G0 PARTIAL, G1-G6 NOT RUN. Pending: worker for VoiceHandle interval `0x950850..0x951deb` (one REA query).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — VoiceHandle remaining 98 candidates attributed

`AUD-02-NGS2-VOICEHANDLE-REMAINING-01`: 98 candidates -> 36 Ghidra-confirmed (new functions `FUN_0094b2c0`, `FUN_0099b400`), 62 heuristic-only (largest interval `0x950850..0x951deb`, 27 candidates, unqueried), 0 unattributed. 2 REA successes, 0 failures; root replayed verifier and checksums. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-remaining-candidates-complete.md`. Static only; no reachability/runtime/ABI claim. G0 PARTIAL, G1-G6 NOT RUN.

Next exact action: optionally confirm the 5532-byte heuristic interval with one function query, or pick another READY task with a fresh claim.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — NGS2 VoiceHandle and RackQuery second callsites verified

VoiceHandle callsite `0x9496a8` -> thunk `0xf91410` in `FUN_009485a0` and RackQuery outlier `0x99fb7b` -> `0xf91420` in `FUN_0099f9d0` are byte- and REA-verified (2 provider successes, 0 failures; root replayed both verifiers and checksums). All 15 RackQuery candidates are now statically accounted for. Details: `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-and-rackquery-second-callsite-complete.md`. Static only; no reachability/runtime/ABI claim. G0 PARTIAL, G1-G6 NOT RUN; failed tries 0.

Next exact action: remaining AUD-02 bounded probes (98 unattributed VoiceHandle candidates) or other READY work; each needs a fresh claim.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — RackQuery callsite attributed; XFF edge test verified

`AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01`: callsite `0x94ebc2` -> thunk `0xf91420`, inside `FUN_0094ea10` (`0x94ea10`, 7554 bytes); one REA query succeeded (361.9 s), root replayed the verifier (exit 0) and SHA256SUMS. 14 other candidates still unattributed; no reachability/runtime/ABI claim. `AUD-01-XFF-EDGE-01` `FIXTURE_VERIFIED` (9 tests). Details: `docs/checkpoints/2026-10-20-root-rackquery-callsite-and-xff-edge-complete.md`. G0 PARTIAL, G1-G6 NOT RUN; failed tries 0.

Next exact action: the AUD-02 VOICEHANDLE probe (prior Ghidra xrefs timeout) or attributing remaining RackQuery candidates, each needing a fresh claim.


## CURRENT CHECKPOINT — 2026-10-20, `/root` — nested-path audit and PSARC zlib handling verified

`AUD-PUBLISH-NESTED-01` and `DATA-02-PSARC-ZLIB-CORRUPT-01` are `FIXTURE_VERIFIED` (6 and 16 tests pass, `--all` audit exit 0). Details and hashes: `docs/checkpoints/2026-10-20-root-audit-nested-psarc-zlib-complete.md`. Prior edge-fixture batch published as `8be3aa1`. Synthetic only; G0 PARTIAL, G1-G6 NOT RUN; failed tries 0.

Next exact action: choose further bounded tasks (e.g. AUD-02 partial NGS2 probes need live REA/Ghidra and a fresh claim).


## CURRENT CHECKPOINT — 2026-10-20, `/root` — three edge fixtures verified (2 defects fixed)

AUD-PUBLISH-EDGE-01 (audit now rejects `..` traversal), DATA-02-PSARC-EDGE-01 (zero-entry archive -> `PsarcError`), APP-01-ASSET-VALIDATOR-EDGE-01 (directory-as-archive test) are `FIXTURE_VERIFIED`; root re-ran all three suites (4, 15, 19 tests pass). Hashes/logs in `docs/checkpoints/2026-10-20-root-edge-fixtures-complete.md`. Synthetic only; G0 PARTIAL, G1-G6 NOT RUN; failed tries 0. Open follow-ups: nested forbidden dir under allowlisted top passes audit; corrupt zlib block raises `zlib.error`.

Next exact action: pick further bounded tasks (e.g. the two open follow-ups above, AUD-02 partial NGS2 probes).


## CURRENT CHECKPOINT — 2026-10-20 (session resumed), `/root` — RE-02 PT_DYNAMIC terminator verified

`RE-02-DYNLINK-PT-DYNAMIC-TERMINATOR-01` is `FIXTURE_VERIFIED`. Root made the change directly (prior worker unreachable): `parse()` in `scripts/ps4_dynlink.py` rejects a `PT_DYNAMIC` span with no `DT_NULL` (`ValueError('unterminated PT_DYNAMIC segment')`); one new test in `scripts/test_ps4_dynlink.py`. `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` exit 0, 10 tests; new test fails with the parser change stashed. Log SHA-256 `643c7cfc3921e5830e047bed28578bc933677c101c9c5a9f6610b6207052d03c`. Details: `docs/checkpoints/2026-10-20-root-re02-pt-dynamic-terminator-complete.md`. Synthetic only; G0 PARTIAL, G1-G6 NOT RUN. Failed tries: 0.

Next exact action: pick the next unowned bounded task from `docs/TASKS.md`.

## PRIOR CHECKPOINT (superseded header) — RE-02 claim published; goal paused

(07:18 CDT, `/root`)

`RE-02-DYNLINK-PT-DYNAMIC-TERMINATOR-01` is claimed by `/root`; worker `/root/re02_pt_dynamic_terminator` owns only `scripts/ps4_dynlink.py` and `scripts/test_ps4_dynlink.py`. The bounded change is for `parse()` to reject a `PT_DYNAMIC` declared span that contains no `DT_NULL` tag, with a synthetic fixture shortening both `p_filesz` and `p_memsz` by 16 bytes. Expected error: `ValueError('unterminated PT_DYNAMIC segment')`. The ELF gABI defines `DT_NULL` as the dynamic-array terminator and ignores its value: https://refspecs.linuxfoundation.org/elf/gabi4%2B/ch5.dynamic.html. The linked project instructions are in `docs/REVERSE_ENGINEERING.md`, Stage 1, which requires validating dynamic-table lengths/offsets and rejecting malformed data.

Baseline `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` exited 0 with 9 tests, log `local/checkpoints/root-2026-10-20-re02-pt-dynamic-terminator-claim-baseline.log`, SHA-256 `d9d734ee1c2de67b748aeef1c4126433aa438ad8375ca953916f12b1df11414d`. Baseline parser SHA-256 `3de247fcd7f42f0358c00fb4beb58bc005ae89aeb54c12707793cbf8f8afd701`; test SHA-256 `24c3f839f15dcf8c0842e8c2d523e61884ee982411cd68a3bedea918ce52ce62`. A root preclaim probe shortened `p_filesz`/`p_memsz` at bytes 96/104 and confirmed the current parser accepts the malformed span (1 module, 2 relocation rows), log SHA-256 `7f71a671e182e3b3ad44674de33b07e716f99c37e6b21a377e3bc4b5953e40c9`. The initial probe printer asked for a missing `imports` result field and raised `KeyError`; it was corrected without changing the parser or hypothesis, log SHA-256 `7eb90e2bfcda83640c37bc2e354a2fe3d0e21121836fa7a93faa9a17b285f172`. Failed implementation hypotheses: 0.

Before the symbol change, `graft callers parse --depth all --in scripts/` exited 0 and found callers `build`, `main`, and the module-level caller; Graft refreshed one changed source card. Log `local/checkpoints/root-2026-10-20-re02-pt-dynamic-terminator-callers.log`, SHA-256 `aab33e8f9705e0ba779189edfb8a1e69b5ef76fb85c5709b81779d7ad04bb5fd`. Pinned `reference/ShadowOfTheColossus-PC` is at `70bb43859cd7ff181a6d409540b757866184d60d` and clean. Its `prepare.py` and `link_modules.py` recognize the zero tag but do not enforce that one occurs; hashes are `9459d439056e9ca61758ad8f3023f0c1e46b085dd1524644446af00374856890` and `c68d123aea4f1eab5598ee9fd84cfe2e12bd2d9027b1ec3970eefd6c5092d8f0`.

`python3 scripts/project_index.py` exited 0 and indexed 4,166 folders; `docs/FOLDERS.md` lists 95 checkpoint files (log `local/checkpoints/root-2026-10-20-re02-pt-dynamic-terminator-claim-project-index.log`, SHA-256 `f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`; index SHA-256 `7e0114b551f15ad1106ee771513a10f0a703bf75fee9afec5c733c256f797dd9`). `graft check` exited 0 with the graph in sync at 1,265 nodes / 530 files; optional deep meaning layer remains unbuilt (log `local/checkpoints/root-2026-10-20-re02-pt-dynamic-terminator-claim-graft-check.log`, SHA-256 `f3c5f7d91083bcb9d4b8b2b338a9ac6e5702e754c8f43f89a53f1017f7fa77d1`). `git diff --check` exited 0 (log `local/checkpoints/root-2026-10-20-re02-pt-dynamic-terminator-claim-diff-check.log`, SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).

Stop-checkpoint publication prep: staged and full path audits passed (5 staged paths, log SHA-256 `5abbb0b1376a432dc926abc55a95a11f57fd946d84788a3497a17579e93ca47e`; 653 tracked paths, log SHA-256 `f2d1c3e18db303cf2cd3a809eaaf5e9c1528cc722c327607e6e181b9bd61e2b8`). Worktree and cached diff checks passed with empty logs (both SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). `git fetch origin` exited 0; precommit divergence was 0/0 at `b45bb2ce483c30e3df574ab75bfa7923483a6499` (fetch log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; divergence log SHA-256 `4b14c7b5549e560a68e9c4daaae3fdb8a28fe32d02470525e7c5a3b2b3271c`).

The paused RE-02 claim checkpoint was committed as `d195f1aa96f9d21e40aecb5ce934bcdbdd03812b` (`docs: checkpoint paused RE-02 terminator claim`) and pushed to `origin main` (push log SHA-256 `b0e8af49dee4c0f95c96927c19438ed2346202391abb70391657f80ead6d0154`). Post-push fetch exited 0; `HEAD` and `origin/main` matched with 0/0 divergence at that commit (fetch log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; divergence log SHA-256 `4b14c7b5549e560a68e9c4daaae3fdb8a28fe32d02470525e7c5a3b2b3271c`).

The preceding NUL manifest fixture completion is published as `b45bb2ce483c30e3df574ab75bfa7923483a6499`; post-push fetch confirmed `HEAD` and `origin/main` match with 0/0 divergence. Completion details are in `docs/checkpoints/2026-10-20-root-data02-manifest-nul-path-complete.md`. No dependency/install facts changed. This claim is synthetic parser work only; it does not establish live ELF or loader behavior, game reachability, or G1. G0 remains PARTIAL; G1-G6 remain NOT RUN.

At the user's request, work stopped before the worker was dispatched. `/root/re02_pt_dynamic_terminator` was interrupted while idle; no parser or test source changed. The independent read-only scout `/root/next_fixture_scout` could not complete because its usage limit was reached; it made no repository changes.

Exact next action when this goal is resumed: send `/root/re02_pt_dynamic_terminator` the start signal, then inspect its two-file diff and independently replay the targeted suite. Do not claim fixture completion before that evidence exists.

## PRIOR CHECKPOINT — 2026-10-20 06:20 CDT, `/root` — DATA-02 manifest NUL-path fixture verified

`DATA-02-ARCHIVE-MANIFEST-NUL-PATH-01` is `FIXTURE_VERIFIED`. Worker `/root/data02_manifest_nul` added one test to `scripts/test_archive_overlap_map.py`; root reviewed the one-method diff and independently replayed the suite. Worker and root each ran `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py` and exited 0 with 13 tests. Logs `local/checkpoints/data02-manifest-nul-path-test.log` and `local/checkpoints/root-2026-10-20-data02-manifest-nul-path-review.log` are byte-identical, SHA-256 `ef4919956bf4bad821df68eb69871efbf1af9027b43ba8733ffe627dfb248155`. Test SHA-256 `571faf8d212ffe7095041155783f6d8ae80511bcbd1ad78eee1c633394a2b781`; unchanged parser `scripts/audit_archive_overlap_map.py` SHA-256 `28330049faf0c7366159e7439ce80b8f4354413908c98bd3d7c95c6addbc7bf7`.

The regression passes JSONL bytes containing the JSON escape `\\u0000` and asserts exact `ManifestError('synthetic: invalid path field at line 1')`. The parser already rejects decoded U+0000, so production code is unchanged. This verifies one synthetic manifest path only; no live manifest/archive, payload, mount, runtime, or game behavior follows.

`python3 scripts/project_index.py` exited 0 and indexed 4,166 folders; `docs/FOLDERS.md` now lists 94 checkpoint files (log `local/checkpoints/root-2026-10-20-data02-manifest-nul-complete-project-index.log`, SHA-256 `f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`; index SHA-256 `c283679050064eb745ca33969cbe41457c0c1d530cd272496f862f738dc0f708`). `graft check` exited 0 with the wiring graph in sync at 1,265 nodes / 530 files; the optional deep meaning layer remains unbuilt (log `local/checkpoints/root-2026-10-20-data02-manifest-nul-complete-graft-check.log`, SHA-256 `f3c5f7d91083bcb9d4b8b2b338a9ac6e5702e754c8f43f89a53f1017f7fa77d1`). `git diff --check` exited 0 (log `local/checkpoints/root-2026-10-20-data02-manifest-nul-complete-diff-check.log`, SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`).

Publication checks: cached diff check exited 0 (empty-log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`); staged audit accepted exactly 6 allowlisted paths (log SHA-256 `7b64e32df64783456b291ba9b9d11eb771c0222072afaf832df7b2f5a7ea13f3`); full audit accepted 652 paths (log SHA-256 `4c6e2897f71694cfe669388c65b4fc50c88904af635dff0a9a59aabc88e84ccf`). `git fetch origin` exited 0; precommit divergence was 0/0 at `c9fa8be7a22705c570b31c00fe5cc23d9804784b` (divergence log SHA-256 `4b14c7b5549e560a68e9c4daaae3fdb8a28fe32d0247050325e7c5a3b2b3271c`; fetch log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). The published claim commit `061a0de04d2b82102fe16661ff4834fe96e77434` and documentation sync `c9fa8be7a22705c570b31c00fe5cc23d9804784b` are on `origin/main`. Claim checkpoint `docs/checkpoints/2026-10-20-root-data02-manifest-nul-path-claim.md` records the 12-test baseline and its hashes.

The earlier NUL claim checkpoint remains below. No dependency/install facts changed. Failed implementation tries: 0. G0 remains PARTIAL; G1-G6 remain NOT RUN.

Completion publication: the six audited paths were committed as `b45bb2ce483c30e3df574ab75bfa7923483a6499` (`test: reject NUL paths in archive manifests`) and pushed to `origin main` (push log `local/checkpoints/root-2026-10-20-data02-manifest-nul-complete-push.log`, SHA-256 `abe53366d0e90738b0e611a2c8d4667b65fafaa933eefc32391e10fb2a0f9482`). Post-push `git fetch origin` exited 0; `git rev-list --left-right --count HEAD...origin/main` reported 0/0 at that commit. Fetch log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`; divergence log `local/checkpoints/root-2026-10-20-data02-manifest-nul-complete-post-push-divergence.log`, SHA-256 `4b14c7b5549e560a68e9c4daaae3fdb8a28fe32d0247050325e7c5a3b2b3271c`.

Exact next action: publish and dispatch the RE-02 PT_DYNAMIC missing-terminator parser/test task.

## PRIOR CHECKPOINT — 2026-10-20 06:17 CDT, `/root` — DATA-02 manifest NUL-path claim published

`DATA-02-ARCHIVE-MANIFEST-NUL-PATH-01` was claimed by root with worker `/root/data02_manifest_nul`. Scope was one synthetic JSONL escaped-NUL path test in `scripts/test_archive_overlap_map.py`; the parser already rejected U+0000. Baseline from `/Volumes/Extra/SoC Mac Native`: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py` exited 0 with 12 tests. Log `local/checkpoints/root-2026-10-20-data02-manifest-nul-claim-baseline.log`, SHA-256 `e912336b9a2279cf34b06f205b51b3eb28c54d31e6a078047b5b6fe0fa0f6617`; baseline test SHA-256 `9de9f0084ce829d0e5bb386f5cdf4bf15a5ac625b64eb597755d3e69590e10d9`; parser SHA-256 `28330049faf0c7366159e7439ce80b8f4354413908c98bd3d7c95c6addbc7bf7`. Claim commit `061a0de04d2b82102fe16661ff4834fe96e77434` and sync `c9fa8be7a22705c570b31c00fe5cc23d9804784b` were pushed. Failed implementation tries: 0. Exact next action at claim time was to dispatch, review and independently replay the fixture.

## PRIOR CHECKPOINT — 2026-10-20 06:09 CDT, `/root` — PSARC block-table fixture published

`DATA-02-PSARC-BLOCK-TABLE-RANGE-01` is `FIXTURE_VERIFIED`. Worker `/root/data02_psarc_block_table_range` changed only `scripts/test_psarc.py`, adding one test that sets entry 1's first-block field at bytes `78:82` to `0xffffffff` and asserts the exact `PsarcError('entry references blocks outside the block table')`.

Worker and root each ran `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`; both exited 0 with 14 tests. Test SHA-256 `66c0a732287614b897ea8a08eb35f114329f92d3e7dc88f780c6d1586fb881d4`; unchanged parser SHA-256 `3bef8a66705c34ab43c321b688ec675cc3cd7aa0e07ec3afdb0f956e3e80cdc9`. Worker log SHA-256 `62e6bd3a77554540929dd26eab6204ab86056dc07bacce00a71e4482e9f4bcb8`; root replay log SHA-256 `0fa6e9196f096183c02cc2acc9c66c1dc944647981973bb5cc84a89647a36d1e`; outputs match apart from elapsed time (`0.005s` vs `0.006s`).

Graft initially reported the graph stale for this new method; `graft build` exited 0, refreshed one changed file (529 cached), and `graft check` then passed at 1,264 nodes / 530 files. `graft grep` finds the new test at `scripts/test_psarc.py:L89-L95`; the optional deep meaning layer remains unbuilt. No install or dependency changes.

This verifies synthetic TOC/block-table range rejection only. No live archive, loader/runtime, game use, or G1/G2 behavior is inferred. Failed implementation tries: 0; G1 NOT PASSED; G2 NOT RUN. Full checkpoint: [PSARC block-table boundary fixture](docs/checkpoints/2026-10-20-root-data02-psarc-block-table-range-complete.md).

Claim commit `72f3a23` and dispatch checkpoint syncs `bfb5ab5` / `dc02551` were pushed. The latest post-push fetch confirmed `HEAD` and `origin/main` match at `dc02551c046f53af459afcca428a267668cb5f24`, 0/0 divergence (log SHA-256 `0fd968c619e597674fff78231aafdead9076559e2f97885a338ac2e887499ddf`).

`python3 scripts/project_index.py` exited 0 and indexed 4,166 folders; `docs/FOLDERS.md` lists 92 checkpoint files (log `local/checkpoints/root-2026-10-20-data02-psarc-block-table-range-complete-project-index.log`, SHA-256 `f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`).

`git diff --check` and `git diff --cached --check` exited 0 (empty-log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Staged audit accepted 7 allowlisted paths (log SHA-256 `3077d5e9fc639fef603ad0a4ceb35433356800711e93f47143edefad14f54253`); full audit accepted 650 paths (log SHA-256 `9b6fa4bea4256f9da19a4d59dfb76ad6c6a7f3a437aa1888a35936ce018ee20e`). Precommit fetch confirmed 0/0 at `dc02551c046f53af459afcca428a267668cb5f24` (log SHA-256 `0fd968c619e597674fff78231aafdead9076559e2f97885a338ac2e887499ddf`).

Completion commit `9fc588fcb38e549dd01194852a0e8df62da1db46` (`test: reject PSARC block indices beyond table`) was pushed. Post-push fetch confirmed `HEAD` and `origin/main` match at that commit, 0/0 divergence (log SHA-256 `669d7f4c28f681c87d4ddc5d582c3c479e2c3218b9e112e1a3b6a83700ade705`).

Exact next action: evaluate the read-only scout's independent fixture candidates and claim the next unowned task. Generated/untracked `tools/graft/` stays excluded.

## PRIOR CHECKPOINT — 2026-10-20 05:46 CDT, `/root` — truncated ELF PHDR fixture verified

`RE-01-ELF-PHDR-TRUNCATION-FIXTURE-01` is `FIXTURE_VERIFIED`. Worker `/root/re01_phdr_truncation`
changed only `scripts/test_project_tools.py`; worker and root each passed 4 tests with
`PYTHONDONTWRITEBYTECODE=1 python3 scripts/test_project_tools.py`. The test makes the synthetic
128-byte ELF claim two program headers and confirms `unwrap()` rejects before writing the ELF or
identity sidecar. Test SHA-256 `6310c5fa44959697aebe8b34c51836712e61e2f052dc78bf8ebb0d7b00930328`;
worker/root log hashes `0292b91377e3f5de5fc72b83a91ddacbecbc2c226b761793f9b4babe85f7ad63` and
`dcf0a7fc200cab9ab36e5d3037278eb791f53b95cc73af6040c74a0a911416bc`. Commit `6b25210` and
sync `c2e6407` were pushed; `origin/main` verified at `c2e64072b893c1cda1190ffbcab0f1c2196c9728`,
0/0 divergence. This is synthetic parser coverage only; G1 NOT PASSED; G2 NOT RUN.
## PRIOR CHECKPOINT — 2026-10-20 05:39 CDT, `/root` — dynamic string terminator fixture verified

`RE-02-DYNLINK-STRING-TERMINATOR-01` is `FIXTURE_VERIFIED`. Worker `/root/re02_dynlink_terminator`
changed only `scripts/test_ps4_dynlink.py`; root reviewed the exact one-test diff and independently
replayed `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py`. Worker and root each
passed 8 tests. The fixture shortens `DT_SCE_STRSZ` by one byte, omitting the final symbol-name NUL;
`parse()` raises `ValueError`. Test SHA-256 `5802c92c056b876ea91f857431c2084108c8d418c1831aaf345d69ad0f343fa1`; unchanged parser
SHA-256 `3de247fcd7f42f0358c00fb4beb58bc005ae89aeb54c12707793cbf8f8afd701`. Worker log
`local/checkpoints/re02-dynlink-string-terminator-test.log`, SHA-256
`698d16d7b93c4bd9707d3cfacda6fb1e0fea99ed0cda6e69bedc42772940540a`; root replay log
`local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-review.log`, SHA-256
`75ff54e727b57e4bd7b88956ed0c92c4b9a416e6f0976707e8fb54aa385ac0bd`.

`python3 scripts/project_index.py` exited 0 at 4,166 folders (log
`local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-complete-project-index.log`, SHA-256
`f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`); `graft check` exited 0,
graph synced at 1,260 nodes (optional deep meaning layer unbuilt; log
`local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-complete-graft-check.log`, SHA-256
`a4d55100d0ac4eda3f43b8c4a3e931bb916a6e2d10bfe3469707e7aac7c943f6`); `git diff --check` exited 0.
Completion details are in [the checkpoint](docs/checkpoints/2026-10-20-root-re02-dynlink-string-terminator-complete.md).
This synthetic string-table fixture does not establish live ELF, loader/runtime, relocation application,
or game behavior; parent RE-02 remains active. Failed implementation tries: 0; install/dependencies
unchanged; G1 NOT PASSED; G2 NOT RUN.

`git diff --check` and cached diff check passed. `python3 -I scripts/audit_publish_paths.py` accepted
the 6 staged paths (log SHA-256 `59db525646a3c5ba8e2c0a8994248d9690ea171f43b0b201f197121bc766e904`);
`--all` accepted 642 paths (log SHA-256 `124219623ff31ea9616737f28b6ea701e4edcfc6205cd740a9341d849e182994`).
Precommit fetch exited 0 with divergence 0/0 at `595ca47`. Commit `38fad54` (`test: reject unterminated
dynamic import names`) was pushed to `origin main` (exit 0). Post-push fetch exited 0; `HEAD` and
`origin/main` both resolve to `38fad54730c4ac902a56aa36ccf4b3fbf103cb85`, divergence 0/0 (log
`local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-complete-post-push-fetch.log`, SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Tracked worktree is clean;
untracked `tools/graft/` was not touched.

Next exact action: claim the independent truncated-PHDR fixture. The change was limited to the test
and allowlisted port docs; no installation facts changed. G1 remains NOT PASSED; G2 remains NOT RUN.

## PRIOR CHECKPOINT — 2026-10-20 05:37 CDT, `/root` — dynamic string terminator fixture worker dispatched

`RE-02-DYNLINK-STRING-TERMINATOR-01` is claimed by root; worker `/root/re02_dynlink_terminator`
was dispatched after publication and owns only `scripts/test_ps4_dynlink.py`. Root owns shared
records and Git. Parent
`RE-02-DYNLINK-01` is `FIXTURE_VERIFIED` and
the new test task was not previously registered. The one test will mutate only the synthetic
fixture's `DT_SCE_STRSZ`, shortening it to omit the final symbol-name NUL, and assert that
`d.parse()` rejects the unterminated name with `ValueError`. Production parser changes are out of
scope unless the test exposes an independently demonstrated defect.

Baseline `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` exited 0 with 7 tests;
log `local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-claim-baseline.log`, SHA-256
`83166b089fbcb82fac63f22522fa6ae99bb5e46368cd35d756277fd463dd6346`. Parser SHA-256 is
`3de247fcd7f42f0358c00fb4beb58bc005ae89aeb54c12707793cbf8f8afd701`; test SHA-256 is
`7d487b19736d12e9d7f1e23eb269cde15bbf920bd77abc1b65ecc42a0c6e76af`. Graft finds the fixture at
`scripts/test_ps4_dynlink.py:L18-L94`, its existing importer test at `L102-L138`, and `parse()` at
`scripts/ps4_dynlink.py:L72-L120`, where the nested string reader searches for NUL with
`strings.index(0, off)`. Failed implementation tries: zero.

Coverage is one synthetic ELF string-table boundary only; no live ELF, loader/runtime semantics,
game reachability, or G1/G2 behavior is inferred. No dependency change. Prior short-prefix audio
fixture was published in `3be02ec`, with documentation sync `ddd8585`; post-push fetch confirmed
`origin/main` at `ddd8585`, 0/0 divergence. `python3 scripts/project_index.py` exited 0 at 4,166
folders (log `local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-claim-project-index.log`,
SHA-256 `f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`); `graft check` exited
0 at 1,259 nodes (log
`local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-claim-graft-check.log`, SHA-256
`e15e6d217bbb31ada46d24290684c83a073816c320d047ffe187fe0433e3e1ec`; optional deep layer
unbuilt); `git diff --check` exited 0 (empty log SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). `docs/FOLDERS.md` lists
83 checkpoint files. Staged publish audit accepted 4 paths (log
`local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-claim-publish-audit-staged.log`,
SHA-256 `a6d103c4f38a1a76d799927468b7a36dd3f7127d0ad9ec3af911a247f185561b`); full audit accepted
641 paths (log
`local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-claim-publish-audit-all.log`,
SHA-256 `6f16ef84096320705be9e98fe44da6581008ae232ae87ca4549f5e9bb431d739`). `git diff --cached
--check` exited 0 (empty log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). `git fetch
origin` exited 0 with divergence 0/0 at `ddd8585`. Claim commit `f8de17e` (`docs: claim dynamic
string terminator fixture`) was pushed (exit 0); post-push fetch confirmed `HEAD` and `origin/main`
at `f8de17e` with 0/0 divergence (empty log
`local/checkpoints/root-2026-10-20-re02-dynlink-string-terminator-claim-post-push-fetch.log`, SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Documentation sync
`3c614bb` was pushed and its post-push fetch confirmed `HEAD`/`origin/main` at `3c614bb`, 0/0
divergence. Next exact action: review the worker's single-file diff and replay the targeted suite,
then update evidence and the completion checkpoint.

## PRIOR CHECKPOINT — 2026-10-20 05:32 CDT, `/root` — short-prefix audio classifier fixture published

`AUD-01-CLASSIFIER-SHORT-LE-PREFIX-01` is `FIXTURE_VERIFIED`; `/root/aud01_short_prefix` changed
only `scripts/test_audio_headers.py`, and root owns shared docs and Git. Parent `AUD-01` remains
ACTIVE. The linked [ASSETS spec](docs/ASSETS.md#L59) labels classifier output as prefix observations
and keeps codecs UNKNOWN. `FamilyTests.test_le_small_ints_short_prefix` passes seven bytes
`01000000000000` to `family_of` and asserts fallback label `other-01000000`. The classifier is
unchanged.

Baseline `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py` exited 0 with 12
tests. Log `local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-claim-baseline.log`,
SHA-256 `72f09eca554e87a45b7156c6109520cf15e8b6ec157dc5c033610649b49b8698`. Classifier SHA-256
`a7dd214317b4797c31961a6055ec0e73a3c738c8282f35f3a309ef5db085a2bf`; test SHA-256
`37226b294bbeb4b2add57e938a3cc88ebe4395a18555c11b9881850e57027b9f`. `graft grep "family_of"`
maps `scripts/classify_audio_headers.py:L47-L69` and current family fixtures in
`scripts/test_audio_headers.py:L17-L43`. Failed implementation tries: zero.

`python3 scripts/project_index.py` exited 0 at 4,166 folders; log
`local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-claim-project-index.log`, SHA-256
`f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`. `graft check` exited 0 with
the graph in sync at 1,258 nodes; its optional deep meaning layer remains unbuilt. Log
`local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-claim-graft-check.log`, SHA-256
`b01a0073eeb6d43d739dd026eaf37725a4be48cb2a96c5582cb72340bdd96680`. `git diff --check` exited 0
(empty log `local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-claim-diff-check.log`).
`docs/FOLDERS.md` lists 81 checkpoint files. No install/dependency change. Staged/full publish-path
audits passed: `python3 -I scripts/audit_publish_paths.py` accepted the 4 staged paths (log
`local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-claim-publish-audit-staged.log`,
SHA-256 `6ee697a796bbe20e4b91487826c6cfa2dee55f74fb80277639402e09a134ed55`); `--all` accepted 639
paths (log `local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-claim-publish-audit-all.log`,
SHA-256 `7697ade7ea7a8bd5ef2191bc207b3eb35d57e5bb408a7a881f5b8f027f2cfe0a`).
`git diff --cached --check` exited 0 (empty log
`local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-claim-diff-cached-check.log`).
Before publication, `git fetch origin` exited 0 and divergence was 0/0. Claim commit `d150661`
(`docs: claim audio prefix boundary fixture`) was pushed with `git push origin main` (exit 0);
post-push fetch confirmed both `HEAD` and `origin/main` at `d150661`, with 0/0 divergence. Only
after publication, `/root/aud01_short_prefix` was dispatched to edit
`scripts/test_audio_headers.py` only. `/root/fixture_scout_data` is doing read-only discovery in a
different test area and may not edit or claim work. Next exact action: review the worker's test
diff and replay result, then update the completion evidence/checkpoint.

Worker and root each ran `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_headers.py`; both
exited 0 with 13 tests. Worker log `local/checkpoints/aud01-classifier-short-le-prefix-test.log`
and root replay log `local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-review.log`
have identical SHA-256 `b986091cdd4d7aeb1185de9a51d74065913d71702d10fa5dada3e4c584a2ddd7`. Test
SHA-256 is `abd12598c65183a3de54865b298f0ae1d0ad806ddf7714be3515399d5e7d9217`; unchanged classifier
`scripts/classify_audio_headers.py` SHA-256 is
`a7dd214317b4797c31961a6055ec0e73a3c738c8282f35f3a309ef5db085a2bf`. The exact diff adds one
assertion and no implementation change. Failed implementation tries: zero.

This is one synthetic prefix-boundary assertion only; it does not identify a codec or establish
runtime/audio behavior. G1 NOT PASSED, G2 NOT RUN; no dependency change. The claim scope and
pre-edit baseline are recorded in [the claim checkpoint](docs/checkpoints/2026-10-20-root-aud01-classifier-short-prefix-claim.md).
Completion record: `python3 scripts/project_index.py` exited 0 at 4,166 folders (log
`local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-complete-project-index.log`,
SHA-256 `f5e40539e482e9841a3f5631ca3e0e53f76cea9afef85423aaf910c0a8ba55aa`); `graft check` exited
0 at 1,259 nodes (log
`local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-complete-graft-check.log`,
SHA-256 `e15e6d217bbb31ada46d24290684c83a073816c320d047ffe187fe0433e3e1ec`); `git diff --check`
exited 0 (empty log `local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-complete-diff-check.log`,
SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). `docs/FOLDERS.md`
lists 82 checkpoint files. Graft's optional deep meaning layer remains unbuilt. Staged publish-path
audit accepted 7 allowlisted paths (log
`local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-complete-publish-audit-staged.log`,
SHA-256 `30393f3fe0641e875ca71ed54e51eb360b8fd51a4addbdde074d8a512be9c56c`); full audit accepted
640 paths (log
`local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-complete-publish-audit-all.log`,
SHA-256 `594d67310f7bcf44774461fc9e611195fc925d4299e4bb0489270c8a7640d240`). Cached diff check
exited 0 (empty log SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). `git fetch
origin` exited 0; precommit divergence was 0/0 (`HEAD` `d150661` and `origin/main` `d150661`).
Completion commit `3be02ec` (`test: cover short audio prefix fallback`) was pushed to `origin main`
(exit 0). Post-push fetch confirmed `HEAD` and `origin/main` at `3be02ec`, with 0/0 divergence
(log `local/checkpoints/root-2026-10-20-aud01-classifier-short-prefix-complete-post-push-fetch.log`,
SHA-256 `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). Next exact action:
claim `RE-02-DYNLINK-STRING-TERMINATOR-01` after verifying the fixture candidate against live files,
then publish its claim before dispatching the test-only worker.

## PRIOR CHECKPOINT — 2026-10-20 05:23 CDT, `/root` — invalid UTF-8 manifest fixture published

`DATA-02-ARCHIVE-MANIFEST-INVALID-UTF8-01` is `FIXTURE_VERIFIED` and was published in commit
`1a31774` (`test: reject invalid UTF-8 manifests`), followed by documentation-sync commit
`a187a90`. Worker and root replay passed 12 tests; the parser is unchanged and coverage is
synthetic-only. Details, hashes and audit logs are recorded in
[`docs/checkpoints/2026-10-20-root-data02-manifest-invalid-utf8-complete.md`](docs/checkpoints/2026-10-20-root-data02-manifest-invalid-utf8-complete.md).
Post-push fetch confirmed `origin/main` at `a187a90`, 0/0 divergence. Untracked `tools/graft/`
remains untouched and unstaged.

## PRIOR CHECKPOINT — 2026-10-20 05:17 CDT, `/root` — XFF short declared-length fixture verified

`AUD-01-XFF-DECLARED-LENGTH-SHORTER-01` is `FIXTURE_VERIFIED`. Worker `/root/app01_readiness`
added one test in `scripts/test_audio_bank_header.py` only. It starts from the valid XFF fixture,
writes `len(member) - 1` into the little-endian declared-size word at offset `0x14`, and confirms
`AudioBankFormatError` with the existing `decoded member length` message. Parser unchanged.
Worker and root `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py` each exited
0 with 7 tests. Worker log `local/checkpoints/aud01-xff-declared-length-shorter-test.log` SHA-256
`fe836cbecd3b6b4f70bec51b58d30c3e45ad72ae7fb3cb63421a0f7337e651ae`; root log
`local/checkpoints/root-2026-10-20-xff-short-length-review.log` has the same SHA-256.

Test SHA-256 `250f829edfaa4cc6a7f74dd557f617cfa25228105b062adddf2cfce4e2d75531`; parser SHA-256
remains `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`. Graft finds the test
at `scripts/test_audio_bank_header.py:L58-L63`; code graph check passes at 1,257 nodes / 530 source
files, with optional deep meaning layer unbuilt. This establishes synthetic length-word rejection
only, not codec, game audio, runtime use, or any game gate. No failed tries or dependency change.
G1 NOT PASSED; G2 NOT RUN. See [completion checkpoint](docs/checkpoints/2026-10-20-root-aud01-xff-short-length-complete.md).

The claim was published before the edit in commit `290c5c6`. Root review and worker verification
are complete; the worker confirmed this checkpoint before exiting. `python3 scripts/project_index.py` exited 0 at 4,166 folders; `graft check` passed at
1,257 nodes / 530 source files with the optional deep layer unbuilt; `git diff --check` passed.
Logs are `local/checkpoints/root-2026-10-20-xff-short-length-complete-project-index.log` and
`local/checkpoints/root-2026-10-20-xff-short-length-complete-graft-check.log`. E82 and the
completion checkpoint are recorded; `docs/FOLDERS.md` lists 78 checkpoint files. Staged audit
passed for 6 paths (log `local/checkpoints/root-2026-10-20-xff-short-length-complete-publish-audit-staged.log`,
SHA-256 `7efd305835931509aad51b357bc3875bf875a47a808016d3a0689af75f959e9b`); full audit passed
for 636 tracked paths (log `local/checkpoints/root-2026-10-20-xff-short-length-complete-publish-audit-all.log`,
SHA-256 `e3d3d32b2a7830e4ee979c790b7a247b4d6100605f1c3a2b8974a71abe5031ac`). Fetch showed 0/0
divergence. Next exact action: publish completion, then claim
`DATA-02-ARCHIVE-MANIFEST-INVALID-UTF8-01` if still unclaimed.

## PRIOR CHECKPOINT — 2026-10-20 05:14 CDT, `/root` — XFF short declared-length test claimed

`AUD-01-XFF-DECLARED-LENGTH-SHORTER-01` is claimed by root with `/root/app01_readiness` as the
test-only implementation worker. The worker owns only `scripts/test_audio_bank_header.py`; root
owns shared docs and Git. The bounded regression will give `parse_xff_member` a valid `xff\0`
member whose u32le declared length at offset `0x14` is smaller than the full buffer, and assert it
raises `AudioBankFormatError`. Existing parser logic already requires declared length to equal the
member length; no parser change is expected.

Baseline `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_audio_bank_header.py` exited 0 with
6 tests. Log: `local/checkpoints/root-2026-10-20-xff-short-length-claim-baseline.log`, SHA-256
`dcd2147edfda2291b456dfb6d9f3021f0fed07760d5a1c8869fc5446f30e28fe`. Baseline hashes:
`scripts/audio_bank_header.py` `bdad7d192b0ecdb929daf5147a139e7545fd5c546f01ed4ffb4005f2cea9f9c0`;
`scripts/test_audio_bank_header.py` `cffef962db441ef64c8435b53205b9106e5fb3af6360d14e26a751df5a1fcb4c`.
Graft evidence from the read-only scout places the parser at L33-L68 and synthetic tests at L20-L62;
the existing suite covers an oversized length and wrong byte order, not a smaller length word.
Failed implementation tries: zero.

`python3 scripts/project_index.py` exited 0 with 4,166 folders (`local/checkpoints/root-2026-10-20-xff-short-length-claim-project-index.log`); `graft check` passed with 1,256 nodes / 530 source files, optional deep meaning layer unbuilt (`local/checkpoints/root-2026-10-20-xff-short-length-claim-graft-check.log`); and `git diff --check` passed. `docs/FOLDERS.md` lists 77 checkpoint files.

Publication preflight passed: the staged-path audit accepted 4 paths (log
`local/checkpoints/root-2026-10-20-xff-short-length-claim-publish-audit-staged.log`, SHA-256
`bf0cd63a37385613169f23d20b945f577a08420aeac746f86fb2844bf160f313`); the full audit accepted 635
tracked paths (log `local/checkpoints/root-2026-10-20-xff-short-length-claim-publish-audit-all.log`,
SHA-256 `1e5d538c25e2e8d8141a2c31cf2208fb2ff2abea74c7595e229a348ce5c16728`). Fetch showed branch
divergence 0/0. Next exact action: commit and push the claim, then dispatch the worker.

This fixture can prove rejection of one malformed synthetic length field only; it does not establish
codec, audio playback, runtime use, or any game gate. No install/dependency change. Next exact
action after claim publication is to dispatch the worker, review the one-file diff, and rerun the
acceptance command. G1 NOT PASSED; G2 NOT RUN.

## PRIOR CHECKPOINT — 2026-10-20 05:12 CDT, `/root` — PSARC truncated-block fixture verified

`DATA-02-PSARC-TRUNCATED-COMPRESSED-BLOCK-01` is `FIXTURE_VERIFIED`. Worker
`/root/app01_readiness` added one test in `scripts/test_psarc.py` only: a valid synthetic archive
with an intact header/TOC and a final compressed payload shortened by one byte must raise exactly
`PsarcError("truncated block")` on read. The parser was not changed. Worker and root acceptance
command `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` each exited 0 with 13 tests.
Worker log `local/checkpoints/data02-psarc-truncated-compressed-block-test.log` has SHA-256
`2f7b850c977121d8774a5cdcc47ed550746f0edb17a5b087b7ace1ccd82a6307`; root replay
`local/checkpoints/root-2026-10-20-psarc-truncated-compressed-block-review.log` has SHA-256
`26e4fc70ea6b6384bf46375bf00ad2ce38024aabcad5482770bd95e72ba6bbc6`.

Test SHA-256 is `0ef9ee140c70f0be835170e60b1d3e728c39ecd3e78ec2cb75701128e964b179`; parser SHA-256
remains `3bef8a66705c34ab43c321b688ec675cc3cd7aa0e07ec3afdb0f956e3e80cdc9`. Graft finds the new
test at `scripts/test_psarc.py:L93-L95`. This proves only synthetic exact-length rejection, not
live-archive behavior, asset semantics, runtime behavior, or any game gate. No failed tries, install,
or dependency change. G1 NOT PASSED; G2 NOT RUN. See [completion checkpoint](docs/checkpoints/2026-10-20-root-data02-psarc-truncation-complete.md).

The claim checkpoint was published before implementation in commit `4f60856`. Root acceptance and
review are complete. `python3 scripts/project_index.py` exited 0 at 4,166 folders;
`graft check` passed at 1,256 nodes / 530 source files (optional deep meaning layer unbuilt), and
`git diff --check` passed. Logs are `local/checkpoints/root-2026-10-20-psarc-truncation-complete-project-index.log`
and `local/checkpoints/root-2026-10-20-psarc-truncation-complete-graft-check.log`. E81 and the
completion checkpoint are recorded; generated `docs/FOLDERS.md` now lists 76 checkpoint files.
The worker verified this checkpoint in the shared tree before exiting. Publication checks pass:
`python3 -I scripts/audit_publish_paths.py` accepted the 6 staged paths (log
`local/checkpoints/root-2026-10-20-psarc-truncation-complete-publish-audit-staged.log`, SHA-256
`fff4a49e546d1ff8277a1fcdaba8760f367ff09ebe73ae03f6b856e4b19ae41c`); the `--all` audit accepted
634 tracked paths (log `local/checkpoints/root-2026-10-20-psarc-truncation-complete-publish-audit-all.log`,
SHA-256 `d7167ea915aa99fedda659d059347e3994f149f025a0f75cc9204a1ed56d19de`). Branch divergence
was 0/0 before commit. Next exact action: commit and push the completion checkpoint, then claim a
separate unblocked task.

## PRIOR CHECKPOINT — 2026-10-20 05:09 CDT, `/root` — PSARC truncated-block fixture claimed

`DATA-02-PSARC-TRUNCATED-COMPRESSED-BLOCK-01` is claimed by root with `/root/app01_readiness`
as the test-only implementation worker. The worker owns only `scripts/test_psarc.py`; root owns
the shared docs and Git. The bounded regression will build a valid synthetic archive with an
intact header/TOC, truncate its compressed payload block by one byte, and assert the reader raises
`PsarcError("truncated block")`. No parser change is expected.

Baseline `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py` exited 0 with 12 tests.
Log: `local/checkpoints/root-2026-10-20-psarc-truncation-claim-baseline.log`, SHA-256
`7c4ce78e536c0c2cdb85730c9c8381839f365b3a2b9d77c5cc9eaede9427b0df`. Baseline source hashes:
`scripts/psarc.py` `3bef8a66705c34ab43c321b688ec675cc3cd7aa0e07ec3afdb0f956e3e80cdc9` and
`scripts/test_psarc.py` `6fab97c9fda00081bf40414d363882ed10dd4191e9f95a6fcc371ac814b85434`.
No implementation attempt has started; cumulative failed tries remain zero.

The required project index ran successfully with 4,166 folders (`local/checkpoints/root-2026-10-20-psarc-truncation-claim-project-index.log`); `graft check` passed at 1,255 nodes / 530 source files, with the optional deep meaning layer still unbuilt (`local/checkpoints/root-2026-10-20-psarc-truncation-claim-graft-check.log`). `git diff --check` passed. `docs/FOLDERS.md` now reflects the new checkpoint file.

The test can establish only that the parser rejects this synthetic short block at its exact-length
guard. It does not establish behavior for live archives or any asset/runtime/game gate. No install
or dependency change. Claim publication precedes worker edits; next exact action is to dispatch the
worker, review the single-file diff and run the targeted suite, then complete/publish the checkpoint.

## PRIOR CHECKPOINT — 2026-10-20 05:07 CDT, `/root` — APP-01 termination classifier verified

`APP-01-TERMINATION-CLASSIFIER-01` is `FIXTURE_VERIFIED`. `/root/app01_termination` added
`LauncherLifecycle.classifyTermination(stopWasRequested:reason:status:)` in LauncherCore and
changed `LauncherModel.processEnded` to delegate to it. Requested Stop has precedence; only
Foundation `.exit` with status 0 maps to `.cleanExit`; all other outcomes retain the unexpected
status. Four tests cover Stop precedence, normal zero exit, nonzero exit, and signal termination.
Root reviewed the three-file diff and Graft references (6 hits across 3 files; the helper has one
production caller and four tests).

Worker and root filtered command
`swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherLifecycleTests`
both exited 0 with 11 tests across 2 suites. Root full command
`swift test --package-path launcher --scratch-path local/build/launcher` exited 0 with all 18 tests
across 3 suites on `arm64e-apple-macos14.0`. Logs: worker
`local/checkpoints/app01-termination-classifier-test.log`; root filtered
`local/checkpoints/root-2026-10-20-app01-termination-root-review.log`; root full suite
`local/checkpoints/root-2026-10-20-app01-termination-full-suite.log`. Source hashes and log hashes
are recorded in E80 and [the completion checkpoint](docs/checkpoints/2026-10-20-root-app01-termination-classifier-complete.md).

`graft check` and `git diff --check` passed; the wiring graph is current at 1,255 nodes / 530
source files, optional deep meaning layer unbuilt. `python3 scripts/project_index.py` exited 0 at
4,166 folders; log `local/checkpoints/root-2026-10-20-app01-termination-complete-project-index.log`.
The classification tests do not exercise an actual process launch, signal delivery, Stop request,
Finder app, sandbox, quit lifetime, or any game gate. No implementation failures; G1 NOT PASSED and
G2 NOT RUN.

Read-only scout `/root/app01_readiness` identified a separate candidate:
`DATA-02-PSARC-TRUNCATED-COMPRESSED-BLOCK-01`, a synthetic test in `scripts/test_psarc.py` for an
intact TOC with a short final compressed payload block. Existing code has an exact-length guard but
current tests cover only header truncation; the candidate is unclaimed, no code/test run occurred,
and attempt count is unchanged. The scout and its evidence/limits are recorded in the completion
checkpoint.

Files recorded in this checkpoint are `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
`docs/FOLDERS.md`, the completion checkpoint, and the three APP-01 source/test files. No install or
dependency change. Next exact action after publication: claim the PSARC fixture candidate in its
own checkpoint before any edit.

## PRIOR CHECKPOINT — 2026-10-20 05:02 CDT, `/root` — APP-01 termination classifier claim

`APP-01-TERMINATION-CLASSIFIER-01` is claimed by root with `/root/app01_termination` as the
implementation worker. The user selected a pure, testable `LauncherCore` helper. Graft mapped the
only `processEnded` caller to `LauncherModel.start`; `LauncherCore` already owns
`LauncherProcessOutcome` and `LauncherLifecycle.state`. The bounded implementation will add
classification from Stop-request state, Foundation termination reason and status, call it from
`LauncherModel.processEnded`, and test requested Stop precedence, normal zero exit, nonzero exit,
and signal termination.

Baseline command `swift test --package-path launcher --scratch-path local/build/launcher
--filter LauncherLifecycleTests` exited 0: 7 tests in 2 suites passed on `arm64e-apple-macos14.0`.
Log: `local/checkpoints/root-2026-10-20-app01-termination-baseline.log`. Pre-change SHA-256:
`LauncherModel.swift` `d452846b54e6cc28e1726f6db1e017b1290a36f3fe554db948221cc0763fc848`,
`LauncherLifecycle.swift` `6b0007a38fecac6645911b2e848328b3821c97eb426caa61dbbbaca9e23f31fb`, and
`LauncherLifecycleTests.swift` `666f35a315fc81394435428d80207c6e8c633824099895e861a73aa1844293f3`.

`python3 scripts/project_index.py` exited 0 with 4,166 folders indexed; log:
`local/checkpoints/root-2026-10-20-app01-termination-claim-project-index.log`. `graft check` and
`git diff --check` exited 0. The wiring graph is current at 1,250 nodes / 530 source files; the
optional deep meaning layer remains unbuilt.

Worker-owned files are `launcher/Sources/LauncherCore/LauncherLifecycle.swift`,
`launcher/Sources/SoCLauncher/LauncherModel.swift`, and
`launcher/Tests/LauncherCoreTests/LauncherLifecycleTests.swift`; root owns shared documentation
and Git. Acceptance command: `swift test --package-path launcher --scratch-path
local/build/launcher --filter LauncherLifecycleTests`. No failed implementation tries; only the
baseline has run. App/Finder behavior, actual child termination, sandbox distribution, and quit
lifetime remain outside this slice; G1 is NOT PASSED and G2 is NOT RUN.

Claim checkpoint was published as commit `d4c0e69` before source edits. Implementation and root
acceptance are now complete; see the current checkpoint above.

## PRIOR CHECKPOINT — 2026-10-20 05:00 CDT, `/root` — overlap guard final replay and publication readiness

The RackQuery thunk xrefs root replay is recorded below and remains `ACTIVE/PARTIAL`; it accepts
no caller/callsite. A parallel read-only review by `/root/review_recent_fixtures` found two
incomplete output-path guard hypotheses: repository containment alone missed symlinks to another
in-repo folder, and comparing resolved configured/expected paths missed a symlink at the configured
task path. The final `_output_path` guard compares resolved `PRIVATE_OUTPUT_DIR` with the lexical
`EXPECTED_PRIVATE_OUTPUT_DIR`, then checks the configured path under `ROOT.resolve()`.
`OutputPathTests.test_rejects_symlinked_private_output_root_before_build_or_write` patches `ROOT`
and both path constants to a temporary repository and rejects configured-root symlinks to both an
outside directory and in-repo `scripts/` before invoking `build_report` or creating output.

Verification: `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py` exited 0
(11 tests; current replay captured in `local/checkpoints/root-2026-10-20-publication-overlap-tests.log`).
`PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` exited 0 (7 tests; captured
in `local/checkpoints/root-2026-10-20-publication-rela-tests.log`).
Both exact report commands exited 0:
`PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/audit_archive_overlap_map.py --out local/validation/data02-archive-overlap-map-01/root-review/final-run1.json`
and the same command with `final-run2.json`; all three `cmp` checks (root runs together and each
against worker runs) exited 0. Report SHA-256 remains
`d2d9a7a05f53cff142979578030c5f55b4720654d32e3ff12b38651ad5d641a3`. Final audit script
SHA-256 is `28330049faf0c7366159e7439ce80b8f4354413908c98bd3d7c95c6addbc7bf7`; test script
SHA-256 is `6f13a329f1572716107639691b36f9f488ab66b665e59ed37e4074f8377110a1`; updated root
verification JSON SHA-256 is `0b56e401e1a61ef89f24feed7fbcbc77b13d44c247cc5e2d12a4531f2af5c7d1`.

`python3 scripts/project_index.py` exited 0 (4,165 folders). After the source corrections, `graft
check` reported STALE as expected; root rebuilt with all five configured source scopes using
`graft build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir native --only-dir launcher --only-dir reference`.
The rebuilt graph has 1,250 nodes / 530 source files; `graft check` now reports OK, and `graft
skeleton scripts/test_archive_overlap_map.py` confirms the symlink regression test is indexed.
The optional deep meaning layer remains unbuilt. `python3 scripts/project_index.py` exited 0 with
4,165 folders indexed; log: `local/checkpoints/root-2026-10-20-publication-project-index.log`.
`git diff --check` passed. No private or generated Graft files will be staged.

Parallel `/root/app01_readiness` read-only scout confirmed the already-user-approved lifecycle
helper and system-alert importer slices are present in `HEAD`; prior launcher tests passed, native
alert interaction remains NOT RUN. No files changed and no tests ran in that scout. A narrower
possible follow-up is to move process termination reason/status/Stop-request classification into
LauncherCore and test the input classification; sandbox distribution, child-process lifetime on
quit, and unlocked-Mac VoiceOver validation remain unresolved. See current task rows and prior
APP-01 checkpoint; no dependent policy was inferred.

Pending publication includes `HANDOFF.md`, `docs/ASSETS.md`, `docs/EVIDENCE.md`, `docs/FOLDERS.md`,
`docs/TASKS.md`, three checkpoint records, `scripts/audit_archive_overlap_map.py`,
`scripts/test_archive_overlap_map.py`, and `scripts/test_ps4_dynlink.py`. DATA overlap counts and
report bytes are unchanged; E77 records both incomplete/superseded guard hypotheses and the final
11-test replay. No test or provider operation failed. G1 NOT PASSED; G2 NOT RUN.

Published as commit `5aa7dfa` (`feat: add metadata-only archive overlap audit`) and pushed to
`origin/main`; post-push divergence was zero. The current APP-01 claim is recorded above.

## PRIOR CHECKPOINT — 2026-10-20 04:50 CDT, `/root` — RackQuery thunk xrefs root replay

`AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01` remains `ACTIVE/PARTIAL`. The one authorized REA 6.7.0 /
Ghidra 12.1.4 query (`rea xrefs local/re/eboot.elf 0xf91420 --provider ghidra --snapshot
local/re/evidence/aud02-ngs2-rackquery-thunk-xrefs-01/rea-snapshot-thunk-query.json --format
json --full-output`) exited 0 after 360.800293 s, Evidence ID
`ev_5e06a3c5d3b5cce415bd1770751f626f9e1b08fd1182190a95114347ca0c868a`; raw stdout SHA-256
`f2fbba372830fa89772956d26489d00d67c82f0c442c6759d2b8863b29d94f50`, stderr empty. Root
independently verified the 16-file capture manifest, pinned ELF/import hashes, shared snapshot
unchanged, private snapshot provenance, provider envelope, and redacted environment (2 verifying-
identity variables, both redacted). Root replay report:
`local/re/evidence/aud02-ngs2-rackquery-thunk-xrefs-01/root-review/verification.json`, SHA-256
`23ea86375a90f818386ae75777f5716d646e19a5ff5c0b5555253f63b99aa4b3`; manifest SHA-256
`c58b4845463aecc18460c226a8d81d9be9bbd33a6a1757b8bca0e2c3ec84b720`.

The provider returned 15 addresses, exactly matching the prior 15 raw `E8 rel32` candidates.
Root independently parsed the pinned ELF's file-backed `PT_LOAD` ranges and confirmed each
address begins with `E8` and its signed displacement targets `0xf91420`. The address-only xrefs
projection does not provide reference kinds; raw bytes do not establish instruction boundaries
or caller/function attribution. No caller/callsite is accepted and no runtime binding, reachability,
ABI, NGS2 behavior, or audio behavior is established. No additional REA operation was made.

The first root verifier run exposed a checker assertion comparing only the ELF/import input hashes
against a dictionary that also contained the shared-snapshot hash. Root corrected that assertion
and reran `PYTHONDONTWRITEBYTECODE=1 python3 -I
local/re/evidence/aud02-ngs2-rackquery-thunk-xrefs-01/root-review/verify_thunk_xrefs.py`; it
exited 0 with `ROOT_REPLAY_PASS_XREFS_CAPTURED_NO_CALLER_KIND`. This was a verifier correction,
not a failed analysis hypothesis or provider operation. No source/game input changed.

Files changed for this checkpoint: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
`docs/FOLDERS.md`, and the new checkpoint record. Private replay artifacts and the verifier remain
under excluded `local/`; the earlier DATA overlap and RELA fixture changes are recorded below.
`python3 scripts/project_index.py` and `graft check` remain to be refreshed for this checkpoint.

Next exact action: run project-index/Graft and diff checks, stage only allowlisted source/docs,
run staged and full publication audits, verify zero branch divergence, then commit and push. After
that, continue with a separately claimed bounded task; G1 remains NOT PASSED and G2 NOT RUN.

## PRIOR CHECKPOINT — 2026-10-20 04:45 CDT, `/root` — overlap map and RELA fixture root replay

`DATA-02-ARCHIVE-OVERLAP-MAP-01` is root-replayed and ready to record as `ARTIFACT_VERIFIED`.
The final checker reads only the seven named member JSONL manifests. Worker checksum manifest
verified 12/12 entries; root reran the synthetic suite (10 tests), reran the report twice, and
compared both root runs and the worker report byte-for-byte. The result covers 170,451 records,
21 archive pairs, and 40,216 exact-path pair occurrences: 4,704 have equal and 35,512 different
PSARC TOC-declared sizes. Root report SHA-256 is
`d2d9a7a05f53cff142979578030c5f55b4720654d32e3ff12b38651ad5d641a3`; root verification JSON is
`local/validation/data02-archive-overlap-map-01/root-review/verification.json` (SHA-256
`1a6f7adbd72578ff65ae669074e946ce473879ef839e4da38a36103ed0c7a448`). Equal size does not
establish equal contents. No archive/member payload, Fios, overlay, mount-order, precedence, or
runtime claim is made. The checker and tests are new scripts under `scripts/`.

`RE-02-DYNLINK-RELA-FIXTURE-01` is root-replayed and ready to record as `FIXTURE_VERIFIED`.
`scripts/test_ps4_dynlink.py` now gives the synthetic ELF one JMPREL and one nonempty RELA row;
both rows' table, target, relocation type, symbol index and signed addend are asserted. Root's
`PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` exited 0 (7 tests); source
SHA-256 is `7d487b19736d12e9d7f1e23eb269cde15bbf920bd77abc1b65ecc42a0c6e76af`. Root verification
is `local/validation/re-02-dynlink-rela-fixture-01/root-review/verification.json` (SHA-256
`f1677ac1aa610afd27f7da3d705aa046b87c7757707c14d8874115d7ab0af1c5`). This is synthetic parser
coverage only, not relocation application or runtime loader evidence.

`python3 scripts/project_index.py` exited 0 with 4,162 folders indexed. Graft initially reported
STALE after a scope rebuild that omitted `native/` and `launcher/`; root immediately rebuilt with
all configured scopes (`scripts`, `tools`, `native`, `launcher`, `reference`), then `graft check`
reported OK. Graft skeleton confirms the new archive audit script is indexed. Its optional deep
meaning layer remains unbuilt. The generated `graft/` cache and existing untracked `tools/graft/`
are not staged.

At this checkpoint the single already-authorized RackQuery xrefs query at thunk `0xf91420` had
completed on the worker and root replay was pending; see the next checkpoint above for the replay
and limitations. No follow-on query was released. APP-01 sandbox distribution, child-process
lifetime on quit, and the locked-Mac VoiceOver check remain open.

Next exact action: refresh the folder index after the new checkpoint, run `git diff --check`,
verify Graft freshness, stage only allowlisted source/documentation paths, run staged and full
publish-path audits, confirm zero divergence, then commit and push this checkpoint.

## CURRENT CHECKPOINT — 2026-10-20 04:39 CDT, `/root` — parallel DATA-02 and RELA fixture claims

Two independent provider-free tasks are claimed for parallel work. `/root/data02_overlap_map` owns `DATA-02-ARCHIVE-OVERLAP-MAP-01`: a deterministic exact-path overlap and TOC-declared-size relation report across the seven existing member manifests and 21 archive pairs, with synthetic comparison tests. It will read metadata only and cannot establish byte equality, loader behavior, patch priority, mount order or game use. `/root/rela_fixture` owns `RE-02-DYNLINK-RELA-FIXTURE-01`: extend only the synthetic `scripts/test_ps4_dynlink.py` fixture with a nonempty RELA row and assertions for table, target, type, symbol index and signed addend. Existing parser already enumerates JMPREL and RELA; no production parser change is planned unless a real defect is independently demonstrated. The one RackQuery follow-up xrefs query at `0xf91420` was separately authorized by the pushed 91c9752 checkpoint and is running as the only provider operation.

Claim-time navigation: Graft `ask` confirmed the fixture has `RELA=0` / `RELASZ=0` today and the parser enumerates both tables; Graft `callers dynamic_import_elf_fixture --depth all` found one test caller. Graft also identified the existing exact-path overlay test/API, but the new DATA audit will not call overlay policy code or infer precedence. `python3 scripts/project_index.py` exited 0 (4,159 folders; `local/checkpoints/root-parallel-claims-project-index.*`); `graft check` exited 0 (graph synced; deep layer unbuilt, 1,225 nodes pending; `local/checkpoints/root-parallel-claims-graft-check.*`); `git diff --check` passed. Root owns all shared docs and integration. Workers must not edit shared docs, source pins, private game inputs or Git state.

The previous 04:37 checkpoint is preserved below. Its DATA-02 path audit is complete, RackQuery slot-to-thunk evidence was root-replayed, and the public checkpoint is `91c9752` on `origin/main`. The single follow-up REA xrefs operation at `0xf91420` is now running in `/root/rackquery_slot_xrefs` in its own private dossier; no other provider operation is in flight.

Next exact action: stage and publish the four allowlisted claim/index docs after staged and full path audits and a zero-divergence fetch. Then dispatch `/root/data02_overlap_map` and `/root/rela_fixture` with disjoint file ownership while the existing single RackQuery query finishes. Root must replay every result before accepting task status changes.

## PRIOR CHECKPOINT — 2026-10-20 04:37 CDT, `/root` — DATA-02 audit replay and RackQuery slot-to-thunk review

`DATA-02-ARCHIVE-PATH-CASE-01` is now `ARTIFACT_VERIFIED`. Root verified all 32 entries in the worker checksum manifest, reran the metadata-only checker, and byte-compared the result against both worker runs. The seven manifests contain 170,451 records across 21 archive pairs: zero exact duplicate path groups, zero within-manifest Unicode `casefold()` groups, and zero differently spelled cross-archive equivalent pairs. Root verification JSON: `local/validation/data02-archive-path-case-01-root-review/verification.json`, SHA-256 `b706b935c28ed3915ac9c860d3fd2160a63c23ddba93ed765f499e3a5c054019`. The worker's zsh `status` variable capture error and root's initial wrong-working-directory checksum attempt were both corrected; neither was a failed analysis hypothesis. This does not establish Fios, filesystem, overlay, mount-order, title-loader, or runtime behavior.

`AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01`'s single authorized REA xrefs query completed successfully (REA 6.7.0 / Ghidra 12.1.4; 357.771501 s; Evidence ID `ev_0a6ab20e10665242c44f9ad982d80cc7dd87e4af4729fbda2cc985b413d4dcd1`). It queried slot `0x1601000` and returned only address `0xf91420`. Root replay passed 17/17 capture checks; offline byte verifier exited 0 and independently confirmed thunk bytes `ff25dafb6600` resolve from `0xf91420` to slot `0x1601000`. The provider's address-only xrefs projection omits reference kind. Fifteen raw `E8` byte-pattern candidates remain; `0x94fc2d` is still only a candidate from a disassembly slice with a section-boundary warning. No caller/callsite is accepted. A second read-only review agreed. It found two opaque `*_VERIFYING_IDENTITY` values missed by the worker's redactor; root replaced both values in the private environment capture and then built and verified a 143-file checksum manifest. No original identity value is retained. Root records: `local/re/evidence/aud02-ngs2-rackquery-buffersize-01/root-review/provider-replay.json` (SHA-256 `2543c5ab730c61a45c9960ffe8865b391786fdcd3fb849c20f07724bd136d994`) and `root-review/offline-verifier.json` (SHA-256 `4da98060c3e2bd2741615958296e2b5f254b9fb2596d7e0b23df8f4b6432e340d`).

APP-01 choices already made remain: testable `LauncherCore` lifecycle helper and a system alert for non-cancellation import errors. Sandbox distribution and child-process lifetime choices remain unanswered, so no dependent code change is claimed. The initial-readiness VoiceOver check remains waiting because the Mac was locked; no app was opened. G1 is NOT PASSED and G2 is NOT RUN.

The prior recorded root Graft tally was approximately 389,919 tokens across 14 calls; `graft check` remains wiring-synced, with the deep layer unbuilt and 1,225 nodes pending. `python3 scripts/project_index.py` exited 0 with 4,159 folders indexed; `graft check` exited 0 (wiring graph in sync); `git diff --check` passed. Logs are `local/checkpoints/root-data02-rackquery-project-index.*` and `root-data02-rackquery-graft-check.*`.

Next exact action: finish the documentation checkpoint, refresh the folder index and Graft check, run staged and full publish-path audits, fetch and verify zero divergence, then commit and push allowlisted docs. Only after that checkpoint is public, authorize exactly one REA xrefs query for thunk address `0xf91420` to enumerate references. Do not issue any additional provider operation until that query is root-reviewed.

## PREVIOUS CHECKPOINT — 2026-10-20 04:28 CDT, `/root` — DATA-02 command-capture wrapper correction

The last claim checkpoint was pushed as `7560229` (`docs: claim all-archive path case audit`). AUD-02 has nine root-verified static import-to-call paths; RackQueryBufferSize and RackGetVoiceHandle remain partial, with no command semantics, runtime binding, reachability, ABI, audio behavior, or game use established. G1 is NOT PASSED and G2 is NOT RUN.

`AUD-02-NGS2-VOICE-GETSTATEFLAGS-01` completed one serialized REA function query at `0x9534cb`, exit 0, Evidence ID `ev_3f98dc6164521a74e7e9c2ff6f3a389c3945923ba312b1287343c59ba2298b99`. Root verified 56/56 worker dossier checksums and independently replayed the function and bytes: register setup at `0x9534c3`/`0x9534c7`, direct call `0x9534cb -> 0xf91480`, thunk `0xf91480 -> 0x1601030`, and symbol 245 / NID `rEh728kXk3w` `R_X86_64_JUMP_SLOT` in file-backed `PT_SCE_RELRO`. This remains static evidence only. Exactly one RackQueryBufferSize xrefs query was released to `/root/rackquery_slot_xrefs` for slot `0x1601000`. Preflight confirmed the pinned ELF/import hashes and no REA CLI/headless operation in flight, then found the existing dossier snapshot differed from the shared baseline. The worker is preparing a fresh private copy; query result/status is pending. No second provider query is authorized. RackVoiceHandle remains partial after one prior xrefs timeout; no retry is released.

`AUD-01-METADATA-SIDECAR-IDENTITY-01` completed: two corrected runs were byte-identical (`cmp` exit 0; report SHA-256 `2ce90160cdc38a883d459e66d96be2673a66b264da468ba47b1d69e750ea33c0`). Three archive identities, nine member IDs/paths/TOC locators, decoded lengths and payload hashes were verified; base/patch differ for all three cachedFiles members. See [the completion checkpoint](docs/checkpoints/2026-10-20-root-aud01-sound-metadata-identity-complete.md). No schema, codec, overlay, runtime or audio claim was made.

`DATA-02-ARCHIVE-PATH-CASE-01` audits exact and Unicode-casefold duplicate paths across all seven extracted archive member manifests; current `docs/ASSETS.md` covers only the cachedFiles pair. Worker `/root/data02_archive_path_case` confirmed seven inputs and Python 3.15.0 and started a private checker in `local/validation/data02-archive-path-case-01/`. One shell-wrapper error occurred while recording `python3 --version`: zsh rejected assignment to its reserved `status` variable after the version command ran. The worker is preserving that error and rerunning with a non-reserved variable; no audit hypothesis or targeted check has failed. It reads metadata paths only, not archive payloads, and cannot prove Fios, mount order or runtime behavior. See [the claim checkpoint](docs/checkpoints/2026-10-20-root-data02-archive-path-case-claim.md).

APP-01 has no new source change because sandbox distribution and child-process-on-quit choices remain unanswered; root asked the user for those choices. The initial-readiness VoiceOver check is waiting for an unlocked Mac: CUA explicitly reported it locked and automatic unlock failed. No app was opened. See [the environment checkpoint](docs/checkpoints/2026-10-20-root-app01-voiceover-environment-pending.md). User-selected testable `LauncherCore` lifecycle helper and system alert for non-cancellation import errors are reflected in existing artifacts; the 14 launcher unit tests and short smoke run passed in the earlier checkpoint. Native alert interaction was not exercised.

The recorded Graft total remains approximately 389,919 tokens across 14 calls; no price is inferred. The wrapper-correction checkpoint `python3 scripts/project_index.py` exited 0 (4,157 folders) and `graft check` exited 0 (wiring synced; deep layer unbuilt, 1,225 nodes pending); `git diff --check` passed. Logs: `logs/root-data02-case-wrapper-*.log`. The prior claim's staged audit passed 4 paths, full audit passed 623 tracked paths, diff check passed and fetch/divergence was `0 0`. Wrapper checkpoint pre-push checks passed: staged diff check, staged path audit for 3 allowlisted docs paths, full audit for 623 tracked paths, and fetch/divergence `0 0`; logs are `logs/root-data02-case-wrapper-*.log`. Its staged paths are HANDOFF, FOLDERS and the updated claim checkpoint. `tools/graft/` remains unstaged.

Next exact action: commit and push this wrapper-correction checkpoint, then await both bounded workers' outputs. Root will independently verify hashes and replay results before updating shared evidence. RackQuery remains the sole serialized provider operation. APP-01 sandbox/quit decisions and the locked-Mac VoiceOver check remain pending user/environment input.

Prior related checkpoints: [RackDestroy function root replay](docs/checkpoints/2026-10-20-root-aud02-rackdestroy-function-replay.md), [VoiceHandle offline root replay](docs/checkpoints/2026-10-20-root-aud02-voicehandle-offline-replay.md), [RackDestroy xrefs root replay](docs/checkpoints/2026-10-20-root-aud02-rackdestroy-xrefs-replay.md), and [RackCreateWithAllocator/root offline replay](docs/checkpoints/2026-10-20-root-aud02-last-two-offline-replay.md).

## Archived handoff history

The three-callpath root-replay checkpoint was committed as `6673495`
(`docs: root-verify three NGS2 call paths`) and pushed to `origin/main` from `d552ad5`. Pre-push
checks passed: staged path audit accepted seven documentation paths, full audit accepted 604
tracked paths, both diff checks passed, and fetch showed `0 0` divergence. The earlier system-create checkpoint was committed as `85fec59`
(`docs: verify NGS2 system-create path`) and pushed from `357ed0c`; staged/full publish audits
passed 4/600 paths, and `origin/main` had no divergence.

Input identity: `local/re/eboot.elf` SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; import manifest
`local/re/imports.json` SHA-256
`78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744`. The manifest contains 11
NGS2 import records. `first_site` is an import relocation/slot address, not a game callsite; each
packet must resolve the import chain and byte-check any chosen caller against the exact ELF.
Acceptance is limited to static callsite/argument-dataflow observations with unresolved pointers
and indirect edges retained. No NGS2 command contract, audio format, playback, ABI, or runtime
behavior is inferred. Prerequisites are the existing RE-02 import manifest, AUD-01 inventory and
Stage 3 instructions in `docs/REVERSE_ENGINEERING.md`; G1 remains NOT PASSED.

The claim checkpoint was committed as `e71c55a` (`docs: claim NGS2 callsite probes`) and pushed
from `45223f6` to `e71c55a`. Before commit, `python3 scripts/project_index.py` exited 0 with 4,141
folders indexed, `graft check` passed the wiring freshness check (deep layer remains unbuilt,
1,225 nodes pending), `git diff --check` and `git diff --cached --check` passed, staged path audit
accepted four allowlisted documentation paths, full audit accepted 600 tracked paths, and fetch
showed divergence `0 0`. `git status --short` showed only those four staged documentation paths
plus the pre-existing untracked `tools/graft/.cache/telemetry-repo-id.json`; the latter was not
staged.

### Rack-create root review — 2026-10-20 02:01 CDT, `/root`

- Task: `AUD-02-NGS2-RACKCREATE-01`; bounded status: `STATIC_PATH_BYTE_VERIFIED`; worker analysis
  recorded one checker correction failure and zero failed analysis hypotheses. Root independently
  checked all 45 dossier manifest entries and replayed the byte verifier.
- Exact commands, from `local/re/evidence/aud02-ngs2-rackcreate-01/`: `shasum -a 256 -c
  SHA256SUMS.txt` (exit 0); `python3 -I relocation_probe.py` (exit 0, JSON `status=PASS`). Root
  records are outside the worker dossier: `local/re/evidence/aud02-ngs2-rackcreate-01-root-review.command.txt`,
  `.exit.txt`, `-root-checksums.log` (SHA-256
  `f69469305e050a01078d2608eec06897fa62a7e6aa3989c4c79bf85be8f1f7cd`), and
  `-root-verification.json` (SHA-256
  `9c4dc2ecdffd331f165cd4b51cc25876531ca8a7077213c4016f3499a778ce29`; stderr empty).
- Observation: exact bytes verify the path `FUN_0099f9d0` → direct call `0x99fbda` → thunk
  `0xf913e0` → import slot `0x1600fe0` → symbol index 235 / NID `cLV4aiT9JpA`. Ghidra Evidence IDs
  are `ev_a9e34cdb3a950ac6383904cc81900d855e237014bcce91559c6ab3c3ed33700b` and
  `ev_2deb0a5ce856ce8e4a229183e5ab1a5819498ebe9398990d786bcb62f2f95e8b`; the private report is
  `local/re/evidence/aud02-ngs2-rackcreate-01/report.md` (SHA-256
  `75d2acf0c42524de299d4ae1216bc9af2ed6c83d3740068aab53789c4b13e86c`).
- Limits: unresolved `CALL RAX` at `0x99fa67`, runtime import binding, reachability, ABI, NGS2
  meaning, and runtime behavior. No inference beyond static bytes. No game execution.
- Shared files updated: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint
  file. `INSTALL_LOG.md` unchanged. Next: continue independent `sceNgs2SystemCreateWithAllocator`
  work; no further rack-create implementation is needed for this bounded packet.

### VoiceControl slot-reference checkpoint — 2026-10-20 02:01 CDT, `/root`

- Task: `AUD-02-NGS2-VOICE-CONTROL-01`; status ACTIVE; failed provider attempts remain 2 (MCP
  timeout and CLI missing `GHIDRA_INSTALL_DIR`); failed analysis hypotheses 0. The third provider
  invocation succeeded, so it does not increment the failed-try count.
- Exact check: `env GHIDRA_INSTALL_DIR=/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC
  GHIDRA_HEADLESS_MAXMEM=8G REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000
  /Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea xrefs
  "/Volumes/Extra/SoC Mac Native/local/re/eboot.elf" 0x1601020 --provider ghidra --snapshot
  "/Volumes/Extra/SoC Mac Native/local/re/evidence/aud02-ngs2-voice-control-01/rea-snapshot.json"
  --format json --full-output`, with stdout redirected to
  `local/re/evidence/aud02-ngs2-voice-control-01/root-cli-xrefs-attempt3.json` and stderr to
  `root-cli-xrefs-attempt3.stderr.log`. Exit 0 after 368,841 ms. REA 6.7.0/Ghidra 12.1.4; input
  ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; profile digest
  `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`; result SHA-256
  `1e92beb96166045a5400d31b8c26bf3fe0c9023f988fb87b5c7fb27e493857a7`; stderr empty. Snapshot
  SHA-256 after the query: `a9be5c00599247ba1d1141f4b1b05434485727106f493a26af5f4eb1dd3eb9c5`.
- REA Evidence ID `ev_6a01174bbe75c6fd1ee3aab2a3c6e3f9f05ff63a84cb77ee9d9d9423635ac22c`; query
  address `0x1601020` returned the single reference address `0xf91460`, with observed confidence.
  This address-only xrefs result does not expose the reference kind or identify the callsite or
  caller; it is not a VoiceControl path yet.
- Remaining risks: no caller, function, argument bytes, NGS2 behavior, ABI, or runtime binding has
  been established. Exact next action: use REA CLI `function` on `0xf91460` with the same ELF,
  Ghidra configuration, and private snapshot; retain complete output and unresolved edges. If that
  next targeted provider check fails, the third unsuccessful try requires stopping and escalating
  `AUD-02-NGS2-VOICE-CONTROL-01` under AGENTS.md.
- No source or game bytes changed. Shared files updated: `HANDOFF.md`, `docs/TASKS.md`,
  `docs/EVIDENCE.md`, and this checkpoint file. `INSTALL_LOG.md` unchanged.

### System-create root review — 2026-10-20 02:08 CDT, `/root`

- Task: `AUD-02-NGS2-SYSCREATE-01`; bounded status `STATIC_PATH_BYTE_VERIFIED`; failed
  hypothesis-driven tries 0. Worker’s optional `inspect-binary-layout` capability check returned
  unsupported for its Linux-x64-only profile and was not used in the analysis.
- Exact root commands from `local/re/evidence/aud02-ngs2-syscreate-01/`: `shasum -a 256 -c
  SHA256SUMS` (exit 0; 26 entries); `python3 -I verify.py` (exit 0; JSON `result=PASS`). Root log
  hashes: checksum output `16d6c9a282536ea7b28fa73056eb7ec470ef3cae22e543c8f675ec58905d1b65`, verification
  JSON `135ced0d0c69bd6148b15742d98748802d803d5a9222da822cc9a6050b359807`, stderr empty.
- Observation: the exact ELF/import manifest bytes verify the path from slot `0x1601008` (JMPREL
  `R_X86_64_JUMP_SLOT`, symbol index 240 / NID `mPYgU4oYpuY`) through thunk `0xf91430` to a direct
  caller instruction at `0x94932e` in `FUN_009485a0`. Checked pre-call range `0x949307..0x949332`
  (44 bytes), SHA-256 `99e458a9170b402ead55f3b0444dc66758a1e425325a65ffc8ad39d0e4f94887`. REA Evidence
  IDs: `ev_4b730eb76f6268025a4a4435ddc14dc9ec2b474bda906a67ee269b4cfd06192b`,
  `ev_3dcf38cb48579627b09c576918571f0b2fef7efd747306dea1b01b2616affab0`,
  `ev_3c328e924109833a20ed5f3ccb54b5395b4aa0561ab1974c4e77ff97a457385f`. Private report
  `local/re/evidence/aud02-ngs2-syscreate-01/REPORT.md` SHA-256
  `a8021ebd058884f8de5df9e58b7fcf31a582d7513d9ea59babe9a68d53f94e4d`.
- Limits: runtime slot binding and callee remain unresolved; the loaded EAX value from
  `[RBP-0x5fc]` has no resolved reaching definition; Ghidra caller high-p-code has at least one
  omitted operation and 729 omitted edges. No argument roles, ABI, NGS2 contract, runtime use, or
  game behavior is inferred.
- Root review files are private under `local/re/evidence/aud02-ngs2-syscreate-01-root-*`; shared
  files updated: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, and this checkpoint file.
  `INSTALL_LOG.md` unchanged. Exact next action: finish the in-flight VoiceControl function query
  at `0xf91460`; if it fails, it is the third unsuccessful targeted try and requires escalation.

### VoiceControl root review — 2026-10-20 02:13 CDT, `/root`

- Task: `AUD-02-NGS2-VOICE-CONTROL-01`; bounded status `STATIC_PATH_BYTE_VERIFIED`; failed provider
  attempts remain 2 (the earlier MCP timeout and CLI configuration failure); failed analysis
  hypotheses 0. Both later CLI queries and the root byte verifier succeeded.
- The configured xref query for `0x1601020` exited 0 after 368,841 ms. It returned
  `0xf91460` (Evidence ID `ev_6a01174bbe75c6fd1ee3aab2a3c6e3f9f05ff63a84cb77ee9d9d9423635ac22c`).
  REA `function` at `0xf91460` exited 0 after 364,994 ms and reported 126 incoming references
  across 22 caller functions; result JSON SHA-256
  `9a0f4d55f19cf441dbedeb61eff6d45bf804dbb3da6d646fa075b548709861a6`, Evidence ID
  `ev_87eece642729f1ee3dcea0bb20a8601d59b5ae3f6c0af3be617c37e6109bb3ac`. The exact function
  command is preserved in `local/re/evidence/aud02-ngs2-voice-control-01/root-function-attempt1.command.txt`.
- Root byte verifier `python3 -I
  local/re/evidence/aud02-ngs2-voice-control-01/verify.py` exited 0 with
  `STATIC_PATH_BYTE_VERIFIED`; SHA256SUMS verification passed 17/17 entries. Verifier SHA-256
  `8dd9d5a6308247bc04628c3f24ef1d9e4120a078b8af3c705e414d7108f2e7de`, output SHA-256
  `ca3a24f2795f2312062b946ca6d017d6d211dded2f65ec85ef68dd79e1db0e2e`; dossier report SHA-256
  `cfcd998e0817970168054dacf4e0a36ca3514b369c09b04d209ac15e992f2a16`.
- Byte-checked path: import slot `0x1601020`, symbol index 243 / NID `uu94irFOGpA`; thunk bytes at
  `0xf91460` are `ff25bafb6600` and decode to the slot; caller `FUN_009485a0` has a direct call at
  `0x9496d9`, bytes `e8827d6400`, decoding back to the thunk. Selected call-preparation range
  `[0x9496ad,0x9496de)` is 49 bytes, SHA-256
  `485a817c02f7641cb0062cce59e2dbb88b3832a6243b0cce5885025e0cb225b7`. The caller function
  Evidence ID `ev_3c328e924109833a20ed5f3ccb54b5395b4aa0561ab1974c4e77ff97a457385f` is shared with
  the system-create dossier; root verifier confirms this same call edge and exact ELF hash.
- Limits: the thunk's raw instruction jumps through the slot, but the slot's runtime binding and
  callee remain unknown. Ghidra labels the six-byte procedure `thunk_FUN_00f91466` and also emits a
  computed-jump edge to adjacent address `0xf91466`; both observations are preserved. No argument
  roles, ABI, NGS2 meaning, call reachability, runtime behavior, or audio output inferred.
- Shared files updated: HANDOFF, TASKS, EVIDENCE, this checkpoint. Private dossier is under
  `local/re/evidence/aud02-ngs2-voice-control-01/`. `INSTALL_LOG.md` unchanged. Next exact action:
  dispatch the two newly claimed probes after this checkpoint is published.
- `python3 scripts/project_index.py` exited 0 and reported 4,145 folders indexed; log
  `local/checkpoints/2026-10-20-root-aud02-voice-project-index.log` SHA-256
  `4455cc1f3a80e3d44a602e9447ed0f93c9b9aeece8300fc9c4d7fe367a25bff8`; generated
  `docs/FOLDERS.md` has no content diff.

### Follow-up import probe claims — 2026-10-20 02:13 CDT, `/root`

- Tasks: `AUD-02-NGS2-SYSRENDER-01` claims one static path for `sceNgs2SystemRender`, symbol index
  242, NID `i0VnXM-C9fc`, slot `0x1601018`; `AUD-02-NGS2-VOICE-GETSTATE-01` claims one static path
  for `sceNgs2VoiceGetState`, symbol index 244, NID `-TOuuAQ-buE`, slot `0x1601028`.
- Owners: `/root/aud02_sysrender` and `/root/aud02_voicestate`, to be dispatched after publication
  on separate private evidence paths. Each must resolve slot → thunk → direct caller from the pinned
  ELF, use the existing parser, preserve unresolved edges, and avoid ABI or runtime inference.
- At claim time both tasks have 0 failed tries and no analysis started. No code, binaries, or game
  data changed. Exact next action: dispatch both workers with `gpt-6-luna`, Extra High; root remains
  the sole integrator. G1 remains NOT PASSED.

### Dispatch checkpoint — 2026-10-20 01:40 CDT, `/root`

`AUD-02-NGS2-SYSCREATE-01` is assigned to `/root/aud02_syscreate`;
`AUD-02-NGS2-RACKCREATE-01` is assigned to `/root/aud02_rackcreate`. Both agents were dispatched
with `gpt-6-luna`, Extra High, on separate private evidence paths and a process-local REA CLI
requirement (no shared MCP REA session). Root owns `AUD-02-NGS2-VOICE-CONTROL-01`. They may not
edit shared docs or publish; root is the sole integrator and will independently review/verify
evidence. Task-specific prompts and paths are in the
[claim checkpoint](docs/checkpoints/2026-10-20-root-aud02-ngs2-callsite-claims.md). At dispatch,
analysis had not started and all failed-try counts were zero. Files changed in that checkpoint:
HANDOFF/TASKS and the claim checkpoint only. The dispatch checkpoint was committed as `3e5ecea` (`docs: record NGS2
probe assignments`) and pushed from `d243989` to `3e5ecea`; staged path audit passed three allowlisted
paths, full audit passed 600, `git diff --cached --check` passed, fetch showed divergence `0 0`.

### VoiceControl timeout checkpoint — 2026-10-20 01:47 CDT, `/root`

Root opened `local/re/eboot.elf` through REA with exact SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`; Ghidra 12.1.4 was
selected on ARM64. The first `mcp__rea__xrefs({address:"0x1601020"})` request failed with
`provider_timeout` after the 330,000 ms startup deadline; configured heap was 2 GB and no xref
result/Evidence ID was returned. Run ID `ae5b3b78-9f1a-4d73-aea9-33c5719e55c4`, PID 56090.
The provider reported global function-overlap repair and decompiler warnings, but none were tied
to this slot. Temporary runtime paths under `/var/folders/.../rea-ghidra-CkuzlE/` were removed
before they could be copied. Private diagnostic summary:
`local/re/evidence/aud02-ngs2-voice-control-01/root-mcp-xrefs-timeout.json` (SHA-256
`4a51b441a28a1968948f0ee3f066f02b346615f3643673854bd1719e0c99f9ef`).

Observation: the targeted xref query did not finish because whole-image Ghidra import/analysis
exceeded the MCP startup deadline. It supplies no evidence for or against a VoiceControl caller.
Failed provider attempts for this task: 1; failed binary-analysis hypotheses: 0. This remains a
single recorded resource failure; a new agent/model or larger timeout will not erase the count.
Exact next action: use the existing process-local REA CLI with the previously documented
`GHIDRA_HEADLESS_MAXMEM=8G` and `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000` recovery, preserving a
fresh private snapshot/log. Keep the scope to one byte-checked caller path and retain all unresolved
edges. Workers continue their separate system/rack probes. No code, game payload, reference, or
runtime behavior was changed or tested; G1 NOT PASSED.

### CLI recovery preparation — 2026-10-20 01:50 CDT, `/root`

After the timeout, `mcp__rea__close_binary({})` returned `result: null`; there was no successful
query Evidence to export. `rea` is not on this shell's PATH (`rea --help` exited 127), but the
existing server binary at `/Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea`
is REA 6.7.0 and its `--help` plus `xrefs --help` both exited 0. Existing private
`local/re/rea-snapshot.json` exactly matches the target SHA above and Ghidra 12.1.4 profile digest
`769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa` (one cached entry); its SHA-256
is `d80232c4fe22d7e75654eade8db44af093507a94723f80dd5ce67605dd382354`. Root copied it to
`local/re/evidence/aud02-ngs2-voice-control-01/rea-snapshot.json`; both hashes matched. The first
absolute-path CLI query then failed before analysis because its standalone environment lacked
`GHIDRA_INSTALL_DIR`; details follow.

### CLI provider-configuration failure — 2026-10-20 01:51 CDT, `/root`

Exact command used `GHIDRA_HEADLESS_MAXMEM=8G` and `REA_GHIDRA_STARTUP_TIMEOUT_MS=1200000` with
`/Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/.bin/rea xrefs
"/Volumes/Extra/SoC Mac Native/local/re/eboot.elf" 0x1601020 --provider ghidra --snapshot
"/Volumes/Extra/SoC Mac Native/local/re/evidence/aud02-ngs2-voice-control-01/rea-snapshot.json"
--format json --full-output`; exit 1 in 1.48 s. Stdout is retained as
`local/re/evidence/aud02-ngs2-voice-control-01/root-cli-xrefs.json` (SHA-256
`d5767fb41ab4ad16791bc31155b1b50b73d0fa604da59f260478e35d6442966f`); stderr is empty (SHA-256
`e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`). REA returned
`provider_unavailable` / `not_configured`: `GHIDRA_INSTALL_DIR` was not set. No Ghidra process,
caller result or Evidence ID was produced. The copied snapshot remained byte-identical to its
source. This is the second failed provider invocation for the task; failed analysis hypotheses
remain 0.

The already-installed Ghidra path from REA's live session inventory is
`/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC`. No install/download is needed. The
failure checkpoint was committed as `d7cc85d` (`docs: record NGS2 CLI config failure`) and pushed
from `d78fd8c` to `d7cc85d`; staged path audit accepted three allowlisted paths, full audit
accepted 600, `git diff --cached --check` passed, and fetch showed divergence `0 0`. Exact next
action: make one CLI invocation with `GHIDRA_INSTALL_DIR` set to that path, preserving the
existing 8 GB/1,200,000 ms limits and private stdout/stderr. This is provider attempt three; if it
fails, stop this task and escalate as required. The CLI config change does not reset prior failures.
Workers continue their independent system/rack packets.

## PREVIOUS CHECKPOINT — 2026-10-20 01:35 CDT, /root — importer and PHDR8 checkpoints published

### Publication record — preceding checkpoint

The PHDR8 source-audit claim and regenerated folder index were committed as `e60fe1e`
(`docs: claim PHDR8 source audit`) and pushed (`41d9b41..e60fe1e`). Before publication,
`python3 scripts/project_index.py` exited `0` (4,137 folders indexed); `graft check` exited `0`
(wiring graph in sync; meaning layer unbuilt, 1,225 nodes pending); `git diff --cached --check`
passed; staged path audit accepted four allowlisted paths; full path audit accepted 599; fetch
succeeded and divergence was `0 0`. Commit changed only HANDOFF.md, docs/FOLDERS.md, docs/TASKS.md,
and the PHDR8 claim checkpoint. Earlier app-main publication remains `41d9b41`.

After the importer worker started Swift tests, root reran `python3 scripts/project_index.py`; it
exited `0` but indexed 5,211 folders and generated 1,074 extra lines for `launcher/.build`. Root
restored `docs/FOLDERS.md` to the artifact-free version at `e60fe1e`; no build output was staged.
The worker later reran the index at 5,212 folders with the same 1,074 build-cache lines. It checked
`lsof -nP +D launcher/.build` (no open files), removed only the test-created build tree, and
regenerated the index: `python3 scripts/project_index.py` exited `0` with 4,139 folders. Root then
removed its own test cache using a guarded Python `shutil.rmtree` on `launcher/.build` after
`lsof -nP +D launcher/.build` returned no open files, and reran `python3 scripts/project_index.py`
(exit `0`, 4,139 folders). `docs/FOLDERS.md` has no diff from the committed index and
`launcher/.build` is absent. Worker/root test logs are under `local/validation/`; the root
index-cache recovery log is `local/checkpoints/2026-10-20-root-index-build-artifact.log`.

The combined APP-01 importer fix and PHDR8 audit result were committed as `524de07`
(`fix: classify launcher folder import cancellation`) and pushed (`e60fe1e..524de07`). Staged
path audit accepted eight allowlisted paths; full audit accepted 599; `git diff --cached --check`
passed; `git fetch origin main` succeeded with divergence `0 0`. This includes E57/E58, both task
status updates, completion checkpoints, and the three launcher source/test changes. The prior
claim-only commit was `e60fe1e`.

### RE-02-APPMAIN-25A8D0-01 — STATIC_ANALYSIS_VERIFIED

Worker `/root/appmain_25a8d0` completed the private dossier under
`local/re/evidence/re02-appmain-25a8d0/`. Its exact pinned input is
`local/re/eboot.elf`, SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
REA 6.7.0/Ghidra 12.1.4 Evidence ID is
`ev_167b8234fc082d63d7a17a8ea4ee218ff0b8559c2397945ae599ba0c657b72ba`. Worker report SHA-256
is `f8634cf2eca0b4bd1fcfcba4d8fa96ce628170743d9be6e50f1124519edf20c3`; verifier SHA-256 is
`45fa51c850e3aba9a91bf6ca8d0d2f5fd403e5430639da79f165ad2985f6d85b`.

Root independently ran
`python3 -I local/re/evidence/re02-appmain-25a8d0/verify.py > local/re/evidence/re02-appmain-25a8d0/root-verification.json 2> local/re/evidence/re02-appmain-25a8d0/root-verification.stderr.log; result_code=$?; printf '%s\n' "$result_code" > local/re/evidence/re02-appmain-25a8d0/root-verification.exit.txt; exit "$result_code"`;
it exited `0`, stderr is empty, and root output is byte-identical to worker output (SHA-256
`173a22dab0846535c1947627942087e98fe1a09c535b5db0615d6bdfa9b356cd`). The verifier checks the
1,021-byte `0x25a8d0..0x25accc` body through PT_LOAD #0 (file offset `0x25e8d0`, SHA-256
`8d23572ada7de95be980c85cfa0faa873efe69c1226c141f01c02f19b1fffb08`), 218 instruction bytes,
24 direct calls/11 callees, 15 branches, 70 data references, and three incoming pointer words.
Static app-main code sets RSI to `-1` then calls slot `+0x18`; the constructed vtable word at
`0x15c8cf0` points to `0x25a8d0`. Two calls at `0x25aa06` and `0x25ac9f` remain unresolved.
PHDR #8 (`p_type=0x61000000`) overlaps the function with an alternate file view; its meaning is
unclassified and the verifier uses only PT_LOAD #0 for executable bytes. Strings are literal
observations only. No runtime reachability, class/ABI contract, native implementation, or G1
claim follows. See [E56](docs/EVIDENCE.md) and the
[completion checkpoint](docs/checkpoints/2026-10-20-root-re02-appmain-25a8d0-complete.md).

Three verifier assumptions failed during the task: requiring file bytes for a zero-fill reference,
requiring unique raw file offsets across overlapping headers, and requiring a unique all-header
mapping for the body. These were verifier-coverage failures, not failed analysis hypotheses. The
third failure reached the escalation threshold; as principal, root reviewed the corrected rule
(unique PT_LOAD for code, prefer PT_LOAD for data, use a unique non-PT_LOAD mapping only when no
PT_LOAD covers the reference) and authorized one final independent replay. That replay passed.
The [escalation record](docs/escalations/RE-02-APPMAIN-25A8D0-01.md) preserves all attempts and
the limit on further scope. Failed verifier tries: 3; failed analysis hypotheses: 0.

### APP-01-IMPORTER-ERROR-01 — ACTIVE, cancellation behavior needs a focused fix

The user chose a system alert for folder-import errors and silent cancellation. Graft located
`launcher/Sources/SoCLauncher/ContentView.swift:L52-L65`; live code sets the alert message for every
`.failure`, without classifying user cancellation. This does not yet establish that every picker
cancellation reaches that branch, so the exact error identity must be covered. Root claims a
bounded fix: put Cocoa cancellation/error classification in a testable `LauncherCore` helper,
keep localized alerts for operational failures, wire the helper into `ContentView`, and add focused
unit cases. Preserve selection/readiness on import failure. No app runtime interaction has been
tested. Failed tries: 0. Claim details:
[checkpoint](docs/checkpoints/2026-10-20-root-app01-importer-error-reclaim.md).

Worker `/root/app01_import_cancel` (`gpt-6-luna`, Extra High) added the helper and focused tests in
launcher source/tests only. Its first `swift test` attempt failed while compiling the test target:
Foundation names (`NSError`, `NSCocoaErrorDomain`, `NSUserCancelledError`, and
`NSLocalizedDescriptionKey`) were out of scope because the test file lacked an explicit Foundation
import. This is a build failure, not evidence of application behavior. The second `swift test` run
passed all 14 tests across three suites, including cancellation and localized-error cases.
`graft check` and `git diff --check` also passed, the working tree has no build cache, and the
worker changed only the two launcher source files plus focused tests. Root independently ran
`swift test > ../local/validation/app01-importer-error-01-root-swift-test.log 2>&1; result_code=$?; printf 'exit status: %s\n' "$result_code" >> ../local/validation/app01-importer-error-01-root-swift-test.log; exit "$result_code"`
from `launcher/`; exit `0`, all 14 tests across three suites passed, and the root log SHA-256 is
`d1492ba66cccfc3e5f5c2536be8c4f40e4f55260295bff1ecbebf07e38b55705`. Failed tries: 1. Native
alert interaction remains NOT RUN. No game files,
binary, executable modules, or runtime were opened or modified for this claim. G1 remains NOT
PASSED.

### RE-02-PHDR8-SOURCE-AUDIT-01 — ARTIFACT_VERIFIED

The app-main verifier found program header #8 at raw type `0x61000000`, with file-backed range
`p_offset=0x2ddb100`, `p_vaddr=0`, `p_filesz=0x1055ab0`, and `p_memsz=0`. It overlaps the function
at `0x25a8d0` through a second file view but is not used for executable-byte validation. The live
`reference/shadPS4` checkout is clean at pinned commit
`0ebea15c24253b60f514fc37a348dabc8d0bcb90`. That source calls the value `PT_SCE_DYNLIBDATA`, maps
it to “SCE Dynamic Library Data,” and copies its `p_filesz` into an emulator dynamic-data buffer.
Those are shadPS4 implementation observations only. OpenOrbis community notes use the same name.
Root searched `site:playstation.com "0x61000000" ELF`,
`site:developer.playstation.net "PT_SCE_DYNLIBDATA"`, `site:sony.com
"PT_SCE_DYNLIBDATA"`, and the generic ELF program-header range. No attributable first-party Sony
mapping appeared. The [ELF ABI](https://gabi.xinuos.com/elf/07-pheader.html) reserves
`PT_LOOS..PT_HIOS` for OS-specific semantics, which includes this value but does not provide Sony
semantics. The Sony-specific type name and PHDR #8 loader/runtime role remain UNCLASSIFIED. Search
coverage is bounded and gated SDK documentation was not accessed. Failed analysis tries: 0.
Worker report SHA-256 `e6e459202dc4f14263f2aed24ad5e29bcc448662634a2e46f7714e980a0c60e6`; root review
SHA-256 `13115545b0a898c4521d9234e5f0d7625c4f180aa39b44ce08662c049af04a61` under
`local/re/evidence/re02-phdr8-source-audit/`. See [checkpoint](docs/checkpoints/2026-10-20-root-re02-phdr8-source-audit-claim.md).

This checkpoint was recorded 2026-10-20 01:35 CDT by `/root`. APP-01-IMPORTER-ERROR-01 passed
the worker's and root's `swift test` runs (14 tests, three suites); one corrected compile failure
is retained, UI interaction remains NOT RUN. PHDR8 audit is ARTIFACT_VERIFIED with official meaning
and loader role still UNCLASSIFIED. Files changed: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`,
the existing PHDR8 claim checkpoint, the existing importer claim checkpoint, and three launcher
files. Private evidence/logs: `local/re/evidence/re02-phdr8-source-audit/`,
`local/validation/app01-importer-error-01-{swift-test,root-swift-test}.log`, and
`local/checkpoints/2026-10-20-root-index-build-artifact.log`. Exact root validation commands:
`swift test` from `launcher/` (exit 0, 14 tests passed), guarded removal of the test cache followed
by `python3 scripts/project_index.py` (exit 0, 4,139 folders), `graft check` (exit 0; wiring graph
synced, deep layer unbuilt with 1,225 nodes pending), `git diff --check` (exit 0), and root evidence
hash verification (all four manifest files passed). The changes are published in `524de07` and
pushed to `origin/main`; the post-push fetch showed no divergence. Next exact action: select the
next independent task from the live roadmap/task register, read its specification, and claim it in
HANDOFF/TASKS before dispatch or edits. Risks: no UI interaction or real game runtime validation;
G1 remains NOT PASSED.

## PREVIOUS CHECKPOINT — 2026-10-20 01:17 CDT, /root — audio byte pair verified; app-main raw verification pending

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

At 01:14, worker `/root/appmain_25a8d0` reports that REA/Ghidra returned successfully (exit `0`,
stderr empty), Evidence ID `ev_167b8234fc082d63d7a17a8ea4ee218ff0b8559c2397945ae599ba0c657b72ba`.
The returned function record provisionally gives a contiguous body `0x25a8d0..0x25accc`
inclusive (1,021 bytes), 218 assembly entries, 24 basic blocks, 24 call sites, 11 unique resolved
callees, two targetless indirect calls (`0x25aa06`, `0x25ac9f`), and three incoming data references,
including vtable slot `0x15c8cf0`. Callee relationship coverage is PARTIAL. These remain Ghidra
observations until raw PT_LOAD/body/reference verification passes. The independent command
`xcrun llvm-objdump -d --x86-asm-syntax=intel --start-address=0x25a8d0 --stop-address=0x25b200 "/Volumes/Extra/SoC Mac Native/local/re/eboot.elf"` exited `0`; its window is wider than the claimed body and does not itself establish the function end. Exact REA command, JSON, exit status, stderr and LLVM disassembly are preserved under `local/re/evidence/re02-appmain-25a8d0/`. Root's next action: independently rerun the worker's PT_LOAD/body/call/data/vtable verifier after it is written. No class contract or runtime reachability is inferred.

At 01:15, the app-main worker noted a raw-format mapping issue now included in its pending verifier:
the function body maps to PT_LOAD #0, while incoming pointer words (including `0x15c8cf0`) fall
outside PT_LOAD and inside file-backed program header #1 (`p_type=0x61000010`, `p_vaddr=0x1594000`,
`p_offset=0x1598000`, `p_filesz=0x6d190`, `p_memsz=0x70000`). The ELF has no section-header table
(`e_shnum=0`). Root has not yet checked these values against the raw ELF; PHDR1's meaning remains
unclassified. Exact next action remains to review and independently run the worker's completed
verifier, which must derive file offsets from unique program-header coverage.

### AUD-01-STS-BYTE-PROBE-01 — ARTIFACT_VERIFIED, one pair only

Parent AUD-01, prerequisites DATA-01 and `AUD-01-STS-IDENTITY-01`. The probe is limited to the
paired asset `cp3shadow/sound/test_adpcm/b_info_01.sts_cp3`, read in memory from both verified
cachedFiles archives through `Psarc.read()`. The identity artifact is
`local/validation/aud01-sts-identity-01.json`, SHA-256
`566275c3c104dfc473f8cb3c227641a16b10e2b0da7ffb2811e22b00f877e654`; base archive SHA-256
`373828b9a6e5c689a66b05a01aa14b295026976438aa7796e9baa789ae03dda0`, member ID `10472`, size
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
`Psarc.read()` source before verifying the live reader. Root independently ran
`python3 -I local/re/evidence/aud01-sts-byte-probe-01/verify.py > local/re/evidence/aud01-sts-byte-probe-01/root-verification.json 2> local/re/evidence/aud01-sts-byte-probe-01/root-verification.stderr.log`; exit `0`, stderr empty, and the root JSON SHA-256
`5fae4da501c239e8583bd13effb7d4d5ca68df77932b2e65d819621eabff2471` matches the worker output
byte-for-byte. It confirms the corrected base archive digest and patch identity, manifest locators,
live member IDs/hashes, and byte comparison: 39 equal / 305 different aligned positions, 4-byte
prefix, 2-byte suffix, 101 shifted exact spans of at least 4 bytes (longest 47 bytes). The report
SHA is `76ba1c935ba33282c1a59a1a0ba6b434a8f6688623628c6672a5bf0908c1ebe3`; verifier SHA is
`4deea41d8f63928ec049b61244667a279a1b4e857927d95eb8f6bb1ca1cfed2b`. Task
`AUD-01-STS-BYTE-PROBE-01` is ARTIFACT_VERIFIED, with format/schema/codec/runtime/playback
unclassified. Root added E55 and updated `docs/ASSETS.md` and `docs/TASKS.md`; no asset payloads
were written or emitted. Commit `ed3db01` (`docs: verify paired STS byte layout`) was pushed;
post-push tree is clean and `HEAD` equals `origin/main` at
`ed3db01348a20d6e33154068b4f05b1b49c1fac5`, divergence `0 0`. Exact next action: finish root
verification of the independent app-main function dossier. No game execution or audio playback.
G1 NOT PASSED.

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
