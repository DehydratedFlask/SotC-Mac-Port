# Task register and executable work packets

Statuses: `READY`, `ACTIVE`, `WAITING_FOR_DEPENDENCY`, `ARTIFACT_VERIFIED`, `STATIC_ANALYSIS_VERIFIED`,
`FIXTURE_VERIFIED`, `REAL_GAME_VERIFIED`, `DONE_DOC`, `NEEDS_PRINCIPAL`. No native game implementation exists.
Gates and test evidence take precedence over a status label. Integrator owns this file.

## Register — baseline 2026-10-19

| ID | Task | Depends on | Status / owner | Failed tries |
|---|---|---|---|---|
| RE-01-PERSISTENT-01 | Reusable private Ghidra baseline and read-only reopen | RE-01 | DONE (import exit0 in 360.6s; independent read-only `-noanalysis` reopen exit0, 38,845 functions, 9 blocks; warnings retained; artifacts private) / root | 0 failed import/reopen tries |
| SRV-01-RECURSIVE-MUTEX-01 | Native recursive priority-inherit pthread owner and error/lifetime contract | CPU-01-MUTEX-CONTRACT-01 | FIXTURE_VERIFIED (3/3 focused sanitizer tests, arm64; now owned by `StartupState` and initialized from the new slot-10 callback; scheduler PI under contention NOT TESTED) / root | 2 failed harness/config attempts; 0 product failures |
| CPU-01-CTOR10-DESCRIPTOR-01 | Recover published pointer/global metadata | CPU-01-CTOR10-ANALYSIS-01 | STATIC_METADATA_VERIFIED (root115 assertions, byte-identical result) / root | 2 inspection-command failures; try3/root replay pass |
| CPU-01-CTOR10-DESCRIPTOR-CONSUMER-01 | Recover a demonstrated consumer of global `0x3356678` and the published interface | CPU-01-CTOR10-DESCRIPTOR-01 | STATIC_ANALYSIS_VERIFIED (consumer `FUN_00ed5a90`, factory `FUN_00ed59a0`, class `ClothingPreviewParam` size `0xb0`/8, ctor `0xf09280`, dtor pair, `reset`; 45/45 byte verifier assertions; report/verifier/JSON hashes recorded) / orchestrator | 0 failed hypotheses; 3 helper-script compile slips |
| CPU-01-CTOR10-NATIVE-01 | Native counterpart of slot 10 with `StartupState`-owned mutex | CPU-01-CTOR10-DESCRIPTOR-CONSUMER-01, SRV-01-RECURSIVE-MUTEX-01 | FIXTURE_VERIFIED (native `ClothingPreviewParam` + descriptor; slot 10 registered; null slot 11 preserved; 252/252 CTest, focused 4/4 ASan+UBSan arm64; checker 104 pairs) / orchestrator | 0 failed implementation tries after 2 compile fixes |
| CPU-01-CTOR11-01 | Structural analysis of slot 11 / RVA `0xf09160` | CPU-01-CTOR10-NATIVE-01 | STATIC_ANALYSIS_VERIFIED (same descriptor+mutex+atexit pattern for class `ClothingPhysicalMeshParameters`; ctor `0xf06410`, vptr `0x15fa770`, globals `0x3356068`/`0x3356078`, consumer `0xed5880`, accessor `0xf08bc0`; singleton creator allocates `0x2bf0` = 74×`0x98` PhysX reflection records; 48/48 byte assertions). Deliberately NOT registered: requires the reflection subsystem. / orchestrator | 0 failed hypotheses; 1 verifier opcode-table omission corrected |
| CPU-01-LEAF-CTOR-BATCH-25..29 | Lift the final 27 unguarded 16-byte constant-copy ctors | CPU-01 | FIXTURE_VERIFIED (27/27 byte-exact; 251/251 CTest before slot-10 work; all 227 manifest entries now disposed) / subagent | 0 failed lifts |
| CPU-01-MUTEX-CONTRACT-01 | Characterize Darwin recursive pthread mutex plus priority-inherit configuration and lifecycle | CPU-01-CTOR10-OBJECT-INIT-01 | API_CONFIGURATION_VERIFIED (root replay:recursive/priority-inherit attrs and lifecycle pass; scheduler behavior NOT TESTED) / root+Luna | 0 failed tries |
| SAV-01-NATIVE-CHECKSUM-01 | Native explicit-seed save-body checksum | SAV-01-PRODUCER-01 | FIXTURE_VERIFIED (root CMake integration,223/223 sanitizer tests,source checker/wrong-input rejection,arm64 and independent review; no serializer/save claim) / root | 0 failed production tries |
| CPU-01-CTOR10-OBJECT-INIT-01 | Characterize slot10 initializer/size/cleanup | CPU-01-CTOR10-CALLER-STATE-01 | STATIC_ANALYSIS_VERIFIED (root134 assertions and44/44 checksums; descriptor/ABI/native wiring unresolved; handoff ready) / root | 2 documented targeted failures; 1 user interruption; 2 reported diagnostic errors lack provenance |
| SAV-01-LOADER-01 | Generic chunk lookup and selected load-side writes | SAV-01-BLOB-FLUSH-01 | STATIC_ANALYSIS_VERIFIED (28/28 checksums and byte-identical root replay; partial q2 coverage; native serializer/runtime pending) / root | 2 failed checker tries; try3 and root replay passed; provider successes2 |
| AUD-02-WAITLOOP-01 | Recover what the second mixer-driver wait loop waits for and its callers | AUD-02-TICKDRIVER-CALLERS-01 | STATIC_ANALYSIS_VERIFIED / PREMISE CORRECTED (no blocking wait in `FUN_00997700`; it is a frame-count top-up loop at `0x997880..0x997930` with one static caller `0x997657` in `FUN_009975f0`; 25/25 checks, 8/8 mutants rejected, REA query exit0 368.7s; checkpoint `2026-10-20-root-aud02-waitloop-01-complete.md`) / subagent | 0 failed tries; 2 provider/verifier operations both exit0 |
| G0-ENV-BASELINE-01 | Capture dated local architecture, OS, Xcode, Metal compiler path, and macOS SDK baseline | G0 | ARTIFACT_VERIFIED (six read-only commands exit 0; stdout/stderr/status/duration retained privately; matches dated machine baseline; G0 remains PARTIAL) / root | 0 |
| RE-02-MISSING-PHDR10-01 | Classify the omitted type `0x6fffff01` program-header record in the pinned SELF reconstruction | RE-02 | STATIC_ANALYSIS_VERIFIED (hash-bound SELF/ELF/header/descriptor verification and pinned `parse_self` behavior; semantic type/platform role remains unclassified) / root+phdr10 | 0 |
| RE-02-PHDR10-SOURCE-AUDIT-01 | Search primary public sources for an authoritative name/meaning for PHDR10 type `0x6fffff01` | RE-02-MISSING-PHDR10-01 | ARTIFACT_VERIFIED (bounded public-source search complete; no attributable first-party mapping found; generic ELF only places the value in the OS-specific range; PHDR10 meaning/runtime relevance UNCLASSIFIED) / root+`/root/phdr10_source_audit` | 0 |
| RE-02-PHDR8-SOURCE-AUDIT-01 | Search primary public sources for an authoritative name/meaning for program-header type `0x61000000` | RE-02, RE-02-APPMAIN-25A8D0-01 | ARTIFACT_VERIFIED (bounded official-domain search found no attributable Sony mapping; generic ELF assigns only OS-specific range; pinned shadPS4 names PT_SCE_DYNLIBDATA but is non-authoritative; meaning/loader role UNCLASSIFIED) / root+`/root/phdr8_source_audit` | 0 |
| RE-02-APPMAIN-25A8D0-01 | Characterize the app-main vtable `+0x18` target at `0x25a8d0` | RE-02 | STATIC_ANALYSIS_VERIFIED (worker and root PT_LOAD/body/reference verifier outputs match; 1,021-byte body, static +0x18 linkage, partial callee coverage, two unresolved indirect calls; no runtime/class-contract claim) / root+`/root/appmain_25a8d0` | 3 (verifier mapping assumptions; principal reassessment authorized the passing replay; 0 failed analysis hypotheses) |
| AUD-01-STS-BYTE-PROBE-01 | Characterize one paired `.sts_cp3` member's byte layout | AUD-01-STS-IDENTITY-01, DATA-01 | ARTIFACT_VERIFIED (root replay matches worker verifier; one base/patch pair identities and byte-layout observations verified; format/codec/runtime remain unclassified) / root+`/root/aud01_sts_byte_probe` | 0 |
| DOC-REFERENCE-AUDIT | Pinned source audit | Source pins | DONE_DOC / reference_audit | 0 |
| DOC-PLATFORM | Native subsystem specification | Source audit | DONE_DOC / native_plan_review | 0 |
| DOC-VALIDATION | Acceptance specification | Source audit | DONE_DOC / native_plan_review | 0 |
| SETUP-01 | Native PKG extractor | Xcode, Crypto++ | FIXTURE_VERIFIED / principal | 1 (resolved) |
| DATA-01 | Extract/hash all local packages | SETUP-01 | ARTIFACT_VERIFIED / principal | 0 |
| DATA-02 | Archive/asset schema and overlay map | DATA-01 | ACTIVE (partial: TOCs done; overlay/mount order PROVISIONAL_OBS; all-seven archive path collision audit ARTIFACT_VERIFIED in E75; all-seven exact-path/TOC-size pairwise overlap inventory ARTIFACT_VERIFIED in E77; pakN archive SHA-256 streamed and independently verified; exact `bridge.sxd` base/patch comparison ARTIFACT_VERIFIED in E42 but no schema; member formats, TOC digest, pakN encoding and selected-scene resolution open) / principal+claim-2+root | 0 |
| DATA-02-ARCHIVE-PATH-CASE-01 | Verify exact and Unicode-casefold member-path duplicates across all seven extracted archive manifests | DATA-01 | ARTIFACT_VERIFIED (root replay: 32/32 worker checksums pass; root checker output byte-matches both worker runs; 170,451 records, 21 archive pairs, 0 exact duplicates, 0 within-manifest casefold groups, 0 differently spelled cross-archive pairs; metadata-only, no loader policy inference) / root+`/root/data02_archive_path_case` | 0 analysis hypotheses (one worker wrapper correction and one root checksum working-directory correction) |
| DATA-02-ARCHIVE-OVERLAP-MAP-01 | Inventory exact member-path overlaps and TOC-declared-size relations across all seven archive manifests | DATA-01, DATA-02 | ARTIFACT_VERIFIED (root replay: 12/12 worker checksums pass; 11 tests reject the exact configured task path symlinked either outside or into the temporary repo's `scripts/` before build/write; two deterministic runs byte-match each other and worker outputs; 170,451 records, 21 pairs, 40,216 shared-path pair occurrences, 4,704 equal and 35,512 differing TOC-declared-size occurrences; no payload, mount-order or override inference) / root+`/root/data02_overlap_map` | 2 incomplete/superseded output-path guard hypotheses; 0 failed tests or analysis hypotheses |
| DATA-02-ARCHIVE-MANIFEST-INVALID-UTF8-01 | Add a synthetic invalid-UTF-8 manifest regression | DATA-02-ARCHIVE-OVERLAP-MAP-01 | FIXTURE_VERIFIED (worker and root replay pass 12 tests; `b"\xff"` raises `ManifestError` with the exact `manifest is not UTF-8` message; parser unchanged; synthetic-only) / root+`/root/next_fixture_scout` | 0 failed implementation tries |
| DATA-02-ARCHIVE-MANIFEST-NUL-PATH-01 | Add a synthetic JSONL manifest regression for an escaped NUL in a member path | DATA-02-ARCHIVE-OVERLAP-MAP-01 | FIXTURE_VERIFIED (worker and root each pass 13 tests; escaped `\\u0000` becomes an invalid path and raises exact `ManifestError`; parser unchanged; synthetic only) / root+`/root/data02_manifest_nul` | 0 failed implementation tries |
| DATA-02-PSARC-TRUNCATED-COMPRESSED-BLOCK-01 | Add a synthetic regression for an intact PSARC header/TOC with a truncated compressed payload block | DATA-02 | FIXTURE_VERIFIED (worker and root runs pass 13 tests; exact synthetic truncated block raises `PsarcError("truncated block")`; parser unchanged; no live archive or runtime claim) / root+`/root/app01_readiness` | 0 |
| DATA-02-PSARC-BLOCK-TABLE-RANGE-01 | Add a synthetic regression for a TOC entry whose first-block index exceeds the PSARC block table | DATA-02 | FIXTURE_VERIFIED (worker and root each pass 14 tests; exact out-of-range first-block index raises the expected `PsarcError`; parser unchanged; synthetic only) / root+`/root/data02_psarc_block_table_range` | 0 failed implementation tries |
| RE-01 | SELF/ELF identity + Ghidra seed | DATA-01 | STATIC_ANALYSIS_VERIFIED / principal | 1 (resolved resource limit) |
| RE-01-ELF-PHDR-TRUNCATION-FIXTURE-01 | Reject a plaintext synthetic ELF whose declared program-header count exceeds its bytes | RE-01 | FIXTURE_VERIFIED (worker and root replay pass 4 tests; `e_phnum=2` on the one-header 128-byte ELF raises `ValueError` and writes neither output nor sidecar; pinned parser unchanged; synthetic-only) / root+`/root/re01_phdr_truncation` | 0 failed implementation tries |
| RE-02 | Imports, relocations, boot graph | RE-01 | ACTIVE (partial: imports/relocs/entry chain done; ctor + app-main internals open; synthetic parser fixtures now cover JMPREL and nonempty RELA rows; runtime loader semantics remain unproven) / principal+root | 0 |
| RE-02-DYNLINK-01 | Synthetic ELF fixture for `parse()` import/relocation path | RE-02 | FIXTURE_VERIFIED (synthetic module/library/import fixture asserts one JMPREL and one RELA row; 7 tests pass; runtime loader semantics remain unproven) / root+/root/re02_triage | 0 |
| RE-02-DYNLINK-RELA-FIXTURE-01 | Exercise one nonempty DT_SCE RELA row in the synthetic parser fixture | RE-02-DYNLINK-01 | FIXTURE_VERIFIED (root replay: 7 tests pass; fixture asserts RELA table, target, type 6, symbol index and signed addend `-0x123`, preserving the JMPREL assertions; no live relocation or loader claim) / root+`/root/rela_fixture` | 0 failed analysis hypotheses |
| RE-02-DYNLINK-STRING-TERMINATOR-01 | Reject a synthetic dynamic string table that omits the final symbol-name NUL | RE-02-DYNLINK-01 | FIXTURE_VERIFIED (worker and root replay pass 8 tests; `DT_SCE_STRSZ` shortened by one excludes the final symbol-name NUL and `parse()` raises `ValueError`; parser unchanged; synthetic-only) / root+`/root/re02_dynlink_terminator` | 0 failed implementation tries |
| RE-02-DYNLINK-RELOC-SIZE-ALIGNMENT-01 | Reject synthetic JMPREL and RELA tables whose sizes are not multiples of 24 bytes | RE-02-DYNLINK-01 | FIXTURE_VERIFIED (worker and root each pass 9 tests; one test covers 23-byte `DT_SCE_PLTRELSZ` and `DT_SCE_RELASZ`, asserting exact size errors; parser unchanged; synthetic only) / root+`/root/re02_reloc_size` | 0 failed implementation tries |
| RE-02-DYNLINK-PT-DYNAMIC-TERMINATOR-01 | Reject a synthetic PT_DYNAMIC span without a DT_NULL entry | RE-02-DYNLINK-01 | FIXTURE_VERIFIED (`parse()` now raises `ValueError('unterminated PT_DYNAMIC segment')`; new test fails without the guard, 10 tests pass; synthetic only, see `docs/checkpoints/2026-10-20-root-re02-pt-dynamic-terminator-complete.md`) / root | 0 failed implementation tries |
| RE-02-DYNLINK-NID-NAME-HASH-FIXTURE-01 | Cover `name_by_hash()` NID token lookup with an isolated synthetic text-tree fixture | RE-02-DYNLINK-01 | FIXTURE_VERIFIED (worker and root each pass 19 tests; temporary tree resolves a requested token and omits an absent NID; production parser unchanged; see `docs/checkpoints/2026-10-21-root-re02-dynlink-name-by-hash-complete.md`) / root+`launcher_review` | 0 failed implementation tries |
| AUD-PUBLISH-EDGE-01 | Reject `..` traversal through allowlisted dirs in the publish-path audit | G0 | FIXTURE_VERIFIED (defect fixed; 4 tests pass; see `docs/checkpoints/2026-10-20-root-edge-fixtures-complete.md`) / root | 0 |
| DATA-02-PSARC-EDGE-01 | Reject zero-entry PSARC with `PsarcError` instead of `IndexError` | DATA-02 | FIXTURE_VERIFIED (defect fixed; 15 tests pass; same checkpoint) / root | 0 |
| APP-01-ASSET-VALIDATOR-EDGE-01 | Validator reports a directory named like a required archive as not found | APP-01 | FIXTURE_VERIFIED (19 launcher tests pass; same checkpoint) / root | 0 |
| AUD-PUBLISH-NESTED-01 | Reject forbidden directory names at any depth in the publish-path audit | AUD-PUBLISH-EDGE-01 | FIXTURE_VERIFIED (6 tests pass; `--all` exit 0; see `docs/checkpoints/2026-10-20-root-audit-nested-psarc-zlib-complete.md`) / root | 0 |
| DATA-02-PSARC-ZLIB-CORRUPT-01 | Corrupt zlib block raises `PsarcError` | DATA-02-PSARC-EDGE-01 | FIXTURE_VERIFIED (16 tests pass; same checkpoint) / root | 0 |
| AUD-01-XFF-EDGE-01 | Non-bytes XFF member input raises descriptive `TypeError` | AUD-01-XFF-01 | FIXTURE_VERIFIED (9 tests pass; see `docs/checkpoints/2026-10-20-root-rackquery-callsite-and-xff-edge-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-REMAINING-01 | Attribute the 98 remaining raw callsite candidates for the `sceNgs2RackGetVoiceHandle` thunk | AUD-02-NGS2-RACK-VOICEHANDLE-01 | STATIC_PATH_BYTE_VERIFIED (36 Ghidra-confirmed incl. `FUN_0094b2c0`, `FUN_0099b400`; 62 heuristic-only; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-remaining-candidates-complete.md`) / root | 0 (2 provider successes, 0 failures) |
| CPU-01-LEAF-CTOR-NEXT-01 | Lift leaf constructor RVA `0xce6130` (index 152) as an ordinary native function | CPU-01 | FIXTURE_VERIFIED (121/121 native tests; byte checker exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-ce6130-complete.md`) / root | 0 |
| CPU-01-LEAF-CTOR-NEXT-02 | Lift leaf constructor RVA `0xce38d0` (index 154); document `0xce6fb0`/`0xce67b0` in the native README | CPU-01 | FIXTURE_VERIFIED (122/122 native tests; byte checker exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-ce38d0-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-INTERVAL-950850-01 | Confirm the 5532-byte heuristic interval `0x950850..0x951deb` with one Ghidra function query | AUD-02-NGS2-VOICEHANDLE-REMAINING-01 | STATIC_PATH_BYTE_VERIFIED (`FUN_00950850` confirmed, 27 candidates upgraded; 35 heuristic-only remain; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-interval-950850-complete.md`) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-03 | Lift leaf constructors `0xcde740`, `0xcd4d60`, `0xcd0420` | CPU-01 | FIXTURE_VERIFIED (125/125 native tests; three byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-03-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-INTERVALS-02 | Confirm the two next-largest heuristic VoiceHandle intervals with Ghidra function queries | AUD-02-NGS2-VOICEHANDLE-INTERVAL-950850-01 | STATIC_PATH_BYTE_VERIFIED (`FUN_00951df0`, `FUN_0094aaf0` confirmed; 13 upgraded; 82/104 candidates Ghidra-confirmed, 22 heuristic-only; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-02-complete.md`) / root | 0 (2 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-04 | Lift leaf constructors `0xcceaa0`, `0xccd0f0`, `0xcc8ac0` | CPU-01 | FIXTURE_VERIFIED (128/128 native tests; three byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-04-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-INTERVALS-03 | Confirm two more heuristic VoiceHandle intervals with Ghidra function queries | AUD-02-NGS2-VOICEHANDLE-INTERVALS-02 | STATIC_PATH_BYTE_VERIFIED (`FUN_009530c0`, `FUN_00953ad0` confirmed; 6 upgraded; 88/104 Ghidra-confirmed, 16 heuristic-only; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-03-complete.md`) / root | 0 (2 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-05 | Lift leaf constructors `0xcc8330`, `0xcc5a10`, `0xcc07d0` | CPU-01 | FIXTURE_VERIFIED (131/131 native tests; three byte checkers exit 0; optional `--dump` sub-check skipped for missing private file; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-05-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-INTERVALS-04 | Confirm two more heuristic VoiceHandle intervals with Ghidra function queries | AUD-02-NGS2-VOICEHANDLE-INTERVALS-03 | STATIC_PATH_BYTE_VERIFIED (`FUN_0094a200`, `FUN_0094dc50` confirmed; 4 upgraded; 92/104 Ghidra-confirmed, 12 heuristic-only; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-04-complete.md`) / root | 0 (2 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-06 | Lift leaf constructors `0xcbb2c0`, `0xcb5cb0`, `0xcaeed0` | CPU-01 | FIXTURE_VERIFIED (134/134 native tests; three byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-06-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-INTERVALS-05 | Confirm two more heuristic VoiceHandle intervals with Ghidra function queries | AUD-02-NGS2-VOICEHANDLE-INTERVALS-04 | STATIC_PATH_BYTE_VERIFIED (`FUN_009549e0`, `FUN_0094e3f0` confirmed; 3 upgraded; 95/104 Ghidra-confirmed, 9 heuristic-only; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-05-complete.md`) / root | 0 (2 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-07 | Lift leaf constructors `0xcae740`, `0xcae240`, `0xcad450` | CPU-01 | FIXTURE_VERIFIED (137/137 native tests; three byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-07-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-INTERVALS-06 | Confirm the two largest remaining heuristic VoiceHandle intervals with Ghidra function queries | AUD-02-NGS2-VOICEHANDLE-INTERVALS-05 | STATIC_PATH_BYTE_VERIFIED (`FUN_0094dfe0`, `FUN_0094e270` confirmed; 2 upgraded; 97/104 Ghidra-confirmed, 7 heuristic-only; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-06-complete.md`) / root | 0 (2 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-08 | Lift leaf constructors `0xca4dc0`, `0xc9e8e0`, `0xc91b10` | CPU-01 | FIXTURE_VERIFIED (140/140 native tests; three byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-08-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-INTERVALS-07 | Confirm four more heuristic VoiceHandle intervals (07A/07B) with Ghidra function queries | AUD-02-NGS2-VOICEHANDLE-INTERVALS-06 | STATIC_PATH_BYTE_VERIFIED (`FUN_00949dc0`, `FUN_00949f00`, `FUN_00953220`, `FUN_00952e10` confirmed; 4 upgraded; 101/104 Ghidra-confirmed, 3 heuristic-only; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-07-complete.md`) / root | 0 (4 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-09 | Lift leaf constructors `0xc891e0`, `0xc7de60`, `0xc7c310` | CPU-01 | FIXTURE_VERIFIED (143/143 native tests; three byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-09-complete.md`) / root | 0 |
| AUD-02-NGS2-VOICEHANDLE-INTERVALS-08 | Confirm the last three heuristic VoiceHandle candidates (08A/08B) with Ghidra function queries | AUD-02-NGS2-VOICEHANDLE-INTERVALS-07 | STATIC_PATH_BYTE_VERIFIED (`FUN_00953340`, `FUN_009533d0`, `FUN_00953470` confirmed; all 104 raw candidates for thunk `0xf91410` now Ghidra-confirmed; root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-intervals-08-complete.md`) / root | 0 (3 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-10 | Lift leaf constructors `0xc786c0`, `0xc77070`, `0xc741c0`, `0xc70e20` | CPU-01 | FIXTURE_VERIFIED (147/147 native tests; four byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-10-complete.md`) / root | 0 |
| AUD-02-AUDIOOUT-OUTPUT-01 | Byte-check one callsite/dataflow path for `sceAudioOutOutput` (symbol 19 / NID `QOQtbeDqsT4` / slot `0x1600d30`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (callsite `0x9abdc2` in `FUN_009ab360`; 5 raw candidates, 2 attributed; root replay OK; see `docs/checkpoints/2026-10-20-root-audioout-and-savedata-callsites-complete.md`) / root | 0 (1 provider success) |
| SAV-01-SAVEDATA-MOUNT2-01 | Byte-check one callsite/dataflow path for `sceSaveDataMount2` (symbol 273 / NID `0z45PIH+SNI` / slot `0x1600dd8`) | RE-02 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0xa4aa60` in `FUN_00a4a940`; root replay OK; same checkpoint) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-11 | Lift leaf constructors `0xc6fdf0`, `0xc6d910`, `0xc6bad0`, `0xc69f70` | CPU-01 | FIXTURE_VERIFIED (151/151 native tests; four byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-11-complete.md`) / root | 0 |
| AUD-02-AUDIOOUT-OPEN-01 | Byte-check one callsite/dataflow path for `sceAudioOutOpen` (symbol 17 / NID `ekNvsT22rsY` / slot `0x1600d20`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0x9ac2dd` in `FUN_009ac1f0`; root replay OK; see `docs/checkpoints/2026-10-20-root-audioout-open-and-savedata-setparam-complete.md`) / root | 0 (1 provider success) |
| SAV-01-SAVEDATA-SETPARAM-01 | Byte-check one callsite/dataflow path for `sceSaveDataSetParam` (symbol 276 / NID `85zul--eGXs` / slot `0x1600df0`) | RE-02 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0xa4bcb1` in `FUN_00a4ab90`; root replay OK; same checkpoint) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-12 | Lift leaf constructors `0xc686e0`, `0xc66580`, `0xc65f80`, `0xc65da0` | CPU-01 | FIXTURE_VERIFIED (155/155 native tests; four byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-12-complete.md`) / root | 0 |
| SAV-01-SAVEDATA-INITIALIZE3-01 | Byte-check one callsite/dataflow path for `sceSaveDataInitialize3` (symbol 268 / NID `TywrFKCoLGY` / slot `0x1600db0`) | RE-02 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0xa4d295` in `FUN_00a4d250`; root replay OK; see `docs/checkpoints/2026-10-20-root-savedata-initialize3-complete.md`) / root | 0 (1 provider success) |
| SAV-01-SAVEDATA-UMOUNTWITHBACKUP-01 | Byte-check one callsite/dataflow path for `sceSaveDataUmountWithBackup` (symbol 278 / NID `VwadwBBBJ80` / slot `0x1600e00`) | RE-02 | STATIC_PATH_BYTE_VERIFIED (callsite `0xa48db9` in `FUN_00a48280`; second candidate `0xa4bcdb` in `FUN_00a4ab90`; root replay OK; see `docs/checkpoints/2026-10-20-root-savedata-umountwithbackup-complete.md`) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-13 | Lift leaf constructors `0xc64060`, `0xc61670`, `0xc5f6f0`, `0xc5c320`, `0xc5c1a0` | CPU-01 | FIXTURE_VERIFIED (160/160 native tests; five byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-13-complete.md`) / root | 0 |
| SAV-01-SAVEDATA-DIRNAMESEARCH-01 | Byte-check one callsite/dataflow path for `sceSaveDataDirNameSearch` (symbol 270 / NID `dyIhnXq-0SM` / slot `0x1600dc0`) | RE-02 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0xa47f58` in `FUN_00a47d60`; root replay OK; snapshot-provenance caveat in checkpoint `docs/checkpoints/2026-10-20-root-savedata-dirnamesearch-and-audioout-output-02-complete.md`) / root | 0 (1 provider success) |
| AUD-02-AUDIOOUT-OUTPUT-02 | Attribute the remaining `sceAudioOutOutput` callsites | AUD-02-AUDIOOUT-OUTPUT-01 | STATIC_PATH_BYTE_VERIFIED (all 5 raw candidates in `FUN_009ab360`, `FUN_009abed0`, `FUN_009aca40`; root replay OK; same checkpoint) / root | 0 (2 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-14 | Lift leaf constructors `0xc5c040`, `0xc5a400`, `0xc5a100`, `0xc555c0`, `0xc54d20` | CPU-01 | FIXTURE_VERIFIED (165/165 native tests; five byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-14-complete.md`) / root | 0 |
| AUD-02-AUDIO3D-PORTOPEN-01 | Byte-check one callsite/dataflow path for `sceAudio3dPortOpen` (symbol 11 / NID `XeDDK0xJWQA` / slot `0x1601098`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0x948e09` in `FUN_009485a0`; root replay OK; see `docs/checkpoints/2026-10-20-root-audio3d-portopen-and-portpush-complete.md`) / root | 0 (1 provider success) |
| AUD-02-AUDIO3D-PORTPUSH-01 | Byte-check one callsite/dataflow path for `sceAudio3dPortPush` (symbol 12 / NID `VEVhZ9qd4ZY` / slot `0x16010a0`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0x949f95` in `FUN_00949f80`; root replay OK; same checkpoint) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-15 | Lift leaf constructors `0xc54830`, `0xc53db0`, `0xc4e5e0`, `0xc4aa80`, `0xc47e00` | CPU-01 | FIXTURE_VERIFIED (170/170 native tests; five byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-15-complete.md`) / root | 0 |
| AUD-02-AUDIO3D-INITIALIZE-01 | Byte-check one callsite/dataflow path for `sceAudio3dInitialize` (symbol 4 / NID `UmCvjSmuZIw` / slot `0x1601060`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0x948db4` in `FUN_009485a0`, reusing an existing Ghidra record; root replay OK; see `docs/checkpoints/2026-10-20-root-audio3d-initialize-and-objectsetattributes-complete.md`) / root | 0 |
| AUD-02-AUDIO3D-OBJECTSETATTRIBUTES-01 | Byte-check one callsite/dataflow path for `sceAudio3dObjectSetAttributes` (symbol 6 / NID `4uyHN9q4ZeU` / slot `0x1601070`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (callsite `0x94da39` in `FUN_0094d900`; 3 raw candidates, 1 attributed; root replay OK; same checkpoint) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-16 | Lift leaf constructors `0xc451d0`, `0xc44830`, `0xc42b00`, `0xc3eaa0`, `0xc3cf00` | CPU-01 | FIXTURE_VERIFIED (175/175 native tests; five byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-16-complete.md`) / root | 0 |
| AUD-02-AUDIO3D-PORTADVANCE-01 | Byte-check one callsite/dataflow path for `sceAudio3dPortAdvance` (symbol 8 / NID `lw0qrdSjZt8` / slot `0x1601080`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (sole raw candidate `0x953a7c` in `FUN_009536d0`; root replay OK; see `docs/checkpoints/2026-10-20-root-audio3d-portadvance-and-objectreserve-complete.md`) / root | 0 (1 provider success) |
| AUD-02-AUDIO3D-OBJECTRESERVE-01 | Byte-check one callsite/dataflow path for `sceAudio3dObjectReserve` (symbol 5 / NID `jO2tec4dJ2M` / slot `0x1601068`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (callsite `0x94a6b8` in `FUN_0094a510`; 1 of 9 raw candidates attributed; root replay OK; same checkpoint) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-17 | Lift leaf constructors `0xc37020`, `0xc36a80`, `0xc23650`, `0xc22300`, `0xc20710` | CPU-01 | FIXTURE_VERIFIED (180/180 native tests; five byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-17-complete.md`) / root | 0 |
| AUD-02-AUDIO3D-OBJECTRESERVE-02 | Attribute the remaining eight `sceAudio3dObjectReserve` callsite candidates | AUD-02-AUDIO3D-OBJECTRESERVE-01 | STATIC_PATH_BYTE_VERIFIED (all eight in `FUN_0099b400` via existing evidence; 9/9 candidates attributed; root replay OK; see `docs/checkpoints/2026-10-20-root-audio3d-objectreserve-02-and-objectunreserve-complete.md`) / root | 0 (0 provider operations) |
| AUD-02-AUDIO3D-OBJECTUNRESERVE-01 | Byte-check one callsite/dataflow path for `sceAudio3dObjectUnreserve` (symbol 7 / NID `1HXxo-+1qCw` / slot `0x1601078`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (callsite `0x949e58` in `FUN_00949dc0`; 1 of 2 raw candidates attributed; root replay OK; same checkpoint) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-18 | Lift leaf constructors `0xc1a9a0`, `0xc191a0`, `0xc17a70`, `0xc152c0`, `0xc14f60`, `0xc0d720` | CPU-01 | FIXTURE_VERIFIED (186/186 native tests; six byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-18-complete.md`) / root | 0 |
| AUD-02-AUDIO3D-REMAINING-IMPORTS-01 | Byte-check one callsite each for `sceAudio3dPortSetAttribute`, `Terminate`, `PortGetQueueLevel`, `PortClose` and the second `ObjectUnreserve` candidate | AUD-02 | STATIC_PATH_BYTE_VERIFIED (5 sub-tasks; 2 new provider successes, 3 reuses of existing Ghidra records; root replay OK; all Audio3d imports now have a verified callsite; see `docs/checkpoints/2026-10-20-root-audio3d-remaining-imports-complete.md`) / root | 0 |
| CPU-01-LEAF-CTOR-BATCH-19 | Lift leaf constructors `0xc0bea0`, `0xc0bbd0`, `0xc0bb00`, `0xc0baa0`, `0xc0aae0`, `0xc09070`; fix five batch-18 README rows | CPU-01 | FIXTURE_VERIFIED (192/192 native tests; six byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-19-complete.md`) / root | 0 |
| SAV-01-SAVEDATA-REMAINING-IMPORTS-01 | Byte-check one callsite each for ten remaining `libSceSaveData` imports (DialogInitialize/Open/GetResult/UpdateStatus, GetMountInfo, SaveIcon, Delete, Terminate, CheckBackupData, RestoreBackupData) | SAV-01 | STATIC_PATH_BYTE_VERIFIED (1 new provider success, 9 reuses of existing Ghidra records; root replay OK; see `docs/checkpoints/2026-10-20-root-savedata-remaining-imports-complete.md`) / root | 0 |
| CPU-01-LEAF-CTOR-BATCH-20 | Lift leaf constructors `0xc060a0`, `0xbe5410`, `0xbe5270`, `0xbe50a0`, `0xbe4ff0`, `0xbd7a80` | CPU-01 | FIXTURE_VERIFIED (198/198 native tests on a fresh build; six byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-20-complete.md`) / root | 0 |
| SAV-01-REGION-DOSSIER-01 | Static dossier of the save-region functions (call order, literals, hash helper, dispatcher) | SAV-01 | STATIC_ANALYSIS_VERIFIED (verifier + checksums replayed; FNV-1a-form hash helper `FUN_00161f50` and task dispatcher `FUN_00a4d330` identified; serializer layout and checksum coverage still UNPROVEN; see `docs/checkpoints/2026-10-20-root-sav01-region-dossier-complete.md`) / root | 0 |
| STRAGGLERS-01 | Attribute unattributed callsites `0x954726` (Audio3d PortSetAttribute), `0xa4a903` (SaveData DialogOpen), `0xa4bddb` (SaveData Delete) | AUD-02, SAV-01 | PARTIAL (2 of 3 STATIC_PATH_BYTE_VERIFIED: `FUN_00954680`, `FUN_00a4a600`; `0xa4bddb` has no Ghidra procedure — REA exit 1, offline inference only; see `docs/checkpoints/2026-10-20-root-stragglers-complete.md`) / root | 0 hypotheses failed; 1 provider failure (no retry) |
| CPU-01-LEAF-CTOR-BATCH-21 | Lift leaf constructors `0xe094e0`, `0xdbfa60`, `0xd85490` (earlier manifest gaps) and `0xbd7950`, `0xbd60a0`, `0xbd4dd0` | CPU-01 | FIXTURE_VERIFIED (204/204 native tests on a fresh build; six byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-21-complete.md`) / root | 0 |
| SAV-01-SAVEDATA-DIALOG-HELPERS-01 | Byte-check one callsite each for `sceSaveDataDialogClose/GetStatus/IsReadyToDisplay/ProgressBarSetValue/Terminate` | SAV-01 | STATIC_PATH_BYTE_VERIFIED (1 new provider success, 4 reuses; root replay OK; every SaveData import now has a verified callsite; see `docs/checkpoints/2026-10-20-root-savedata-dialog-helpers-complete.md`) / root | 0 |
| SAV-01-PRODUCER-01 | Trace what the save/load paths hash, write and check | SAV-01-REGION-DOSSIER-01 | STATIC_ANALYSIS_VERIFIED (verifier + checksums replayed; header = 16 bytes {version 1, body FNV-1a-form hash, body size ≤ 0x6400000, dword 3}, only the body is hashed; body layout/serializer still UNKNOWN; see `docs/checkpoints/2026-10-20-root-sav01-producer-complete.md`) / root | 0 (1 provider success) |
| CPU-01-LEAF-CTOR-BATCH-22 | Lift leaf constructors `0xbd32f0`, `0xbd3190`, `0xbd06c0`, `0xbceda0`, `0xbccc00`, `0xbcc960` | CPU-01 | FIXTURE_VERIFIED (210/210 native tests; six byte checkers exit 0; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-22-complete.md`) / root | 0 |
| AUD-02-REGION-DOSSIER-01 | Static dossier of the audio code region (call graph, thread/tick structure, init/shutdown) | AUD-02 | STATIC_ANALYSIS_VERIFIED (call-graph facts verified; roles are inference; verifier + checksums replayed; see `docs/checkpoints/2026-10-20-root-aud02-region-dossier-complete.md`) / root | 0 (2 provider successes) |
| SAV-01-SERIALIZER-01 | Identify the save serializer behind the `[vtable+0x50]` call and decode its container format | SAV-01-PRODUCER-01 | STATIC_ANALYSIS_VERIFIED (two concrete classes resolved; tagged-chunk stream with 12-byte chunk headers and FNV-1a-hashed names decoded; verifier + checksums replayed; blob contents unknown; see `docs/checkpoints/2026-10-20-root-sav01-serializer-complete.md`) / root | 0 (2 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-23 | Lift leaf constructors `0xbcc4e0`, `0xbcc3f0`, `0xbc8210`, `0xbc7280`, `0xbc5b00`, `0xbc2800`; fix four batch-14 README rows | CPU-01 | FIXTURE_VERIFIED (216/216 native tests; six byte checkers exit 0; README rows now ELF-verified; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-23-complete.md`) / root | 0 |
| SAV-01-BLOB-FLUSH-01 | Decode the stream flush path and the game-data blob producer | SAV-01-SERIALIZER-01 | STATIC_ANALYSIS_VERIFIED (flush chain and 15596-byte game-data chunk structure decoded; verifier + checksums replayed; blob byte semantics unknown; see `docs/checkpoints/2026-10-20-root-sav01-blob-flush-complete.md`) / root | 0 (2 provider successes) |
| AUD-02-TICKDRIVER-CALLERS-01 | Characterize what feeds the second mixer-tick driver `FUN_00997700` | AUD-02-REGION-DOSSIER-01 | STATIC_ANALYSIS_VERIFIED (call structure verified; role is inference; entry has no static callers; verifier + checksums replayed; see `docs/checkpoints/2026-10-20-root-aud02-tickdriver-callers-complete.md`) / root | 0 (2 provider successes) |
| CPU-01-LEAF-CTOR-BATCH-24 | Lift leaf constructors `0xbc07f0`, `0xbc0670`, `0xbbf580`, `0xbbd5e0`, `0xbbba10`, `0xbb9740` | CPU-01 | FIXTURE_VERIFIED (222/222 native tests; six byte checkers exit 0; README rows ELF-verified, 0 mismatches; see `docs/checkpoints/2026-10-20-root-cpu01-leaf-ctor-batch-24-complete.md`) / root | 0 |
| CPU-01 | Native recovery feasibility slice | RE-02 | ACTIVE (many leaf ctors FIXTURE_VERIFIED incl. bc-cb8d07c2: 0xdd41e0, 0x3bd70, 0xa3ade0, 0x5777a0; fail-fast startup-prefix and constructor-index metadata gates FIXTURE_VERIFIED; slot-10 descriptor consumer/class semantics verified and a native slot-10 callback is registered at fixture level; function setup zeroes its guards and skips the indirect call; computed target provenance is bounded but remains unresolved outside the static image; direct helper `0xe1ede0` characterization claimed; constructor reachability/helper contracts remain open; engine chain not started; leaf 95 `0xd71f60` NEEDS_PRINCIPAL after 3 failed tries; `0xce7300`, `0xce57b0`, `0xce0890`, and `0xcdc760` fixture/checker slices verified; no startup chain/G1) / principal+root | 0 |
| CPU-01-CTOR-PREFIX-01 | Stop native constructor runner at first unresolved original slot | CPU-01, RE-02 | FIXTURE_VERIFIED (runner now completes 11 callbacks and stops at explicit null slot 11 / RVA `0xf09160`; checker verifies 104 registered RVA/index pairs against pinned ELF/table; no runtime startup proof) / root | 0 |
| CPU-01-CTOR10-ANALYSIS-01 | Characterize first unresolved constructor at RVA `0xf09e60` | CPU-01, RE-02 | STATIC_ANALYSIS_VERIFIED (hash-bound REA/Ghidra dossier plus independent byte/reference checker; seven direct calls, partial callee coverage, one unresolved computed call at `0xf09efb`, local helper contracts unknown; no runtime proof) / root+/root/cpu_slot10_analysis | 1 (corrected verifier snapshot-key assumption) |
| CPU-01-CTOR10-INIT-HELPER-01 | Characterize `0xe1eda0` writes to slot-10 caller guard fields | CPU-01-CTOR10-ANALYSIS-01 | STATIC_ANALYSIS_VERIFIED (62-byte helper range/hash, argument writes and caller guard path independently checked; guard byte remains zero and skips `0xf09efb` on the observed path; no constructor reachability/runtime proof; 1 failed shell wrapper corrected, 0 failed analysis hypotheses) / root+/root/cpu_ctor10_init_helper | 0 |
| CPU-01-CTOR10-INDIRECT-01 | Bound the dataflow and possible target for slot-10 indirect call `0xf09efb` | CPU-01-CTOR10-ANALYSIS-01, CPU-01-CTOR10-INIT-HELPER-01 | STATIC_ANALYSIS_VERIFIED (byte-checked branch and caller-local dataflow; target operand lies outside pinned PT_LOAD ranges with no relocation identifying it; unresolved edge retained; no constructor/runtime/ABI claim) / root+/root/srv01_page_map_19660+root | 0 |
| CPU-01-CTOR10-E1EDE0-01 | Characterize the direct slot-10 helper at `0xe1ede0` | CPU-01-CTOR10-ANALYSIS-01 | STATIC_ANALYSIS_VERIFIED (hash-bound REA/Ghidra dossier and root rerun of PT_LOAD/body/caller/reference verifier; two computed calls and data targets unresolved) / root+/root/srv01_page_map_19660 | 0 |
| CPU-01-CTOR10-CALLER-STATE-01 | Determine byte-backed stack-field state passed to helper `0xe1ede0` by constructor slot 10 | CPU-01-CTOR10-E1EDE0-01, CPU-01-CTOR10-ANALYSIS-01 | STATIC_ANALYSIS_VERIFIED (root reran hash-bound caller verifier; four callee-gate fields are zero on examined path; caller guard skips unresolved call `0xf09efb`; no broader reachability or runtime claim) / root+`/root/ctor10_caller_state` | 0 |
| CPU-01-CTOR-METADATA-01 | Correct original indices in leaf fixtures `0xd32720` and `0xd2ffe0` | CPU-01-CTOR-PREFIX-01 | FIXTURE_VERIFIED (six metadata fields now match original indices 119/120; both tests pass; both ELF checkers and the 104-pair table checker pass; function behavior unchanged) / root+/root/cpu_ctor_metadata | 0 |
| CPU-02 | Packed data/pointer schemas | RE-01 | ACTIVE (Tagged31 decode+retag+0x66dec0 mapping and all 13 behavior arms FIXTURE_VERIFIED; heap cells OBS; static-address meaning, asset structs, type names open) / principal+bc-cb8d07c2+root | 0 |
| CPU-03 | Native jobs/fibers/TLS | CPU-01, CPU-02 | WAITING_FOR_DEPENDENCY | 0 |
| SRV-01 | Native platform services / asset I/O | RE-02, DATA-02 | ACTIVE (partial: host_map + carve + split + list + free-count `@+0x48` + high-water `0x1a9bb` + exact-fit `0x1aada` + page-debit `0x1ae67` FIXTURE slices bc-9aa0fe88; distinct `0x19e00` block `0x1a123–0x1a15a` now FIXTURE_VERIFIED with CMake test and E28 byte checker, 1 earlier failed target hypothesis; `0x19660` static characterization E48; page bitmap / Fios semantics remain open) / principal+bc-9aa0fe88+root | 0 |
| SRV-01-PAGE-MAP-19660-01 | Characterize pinned function at `0x19660` for SRV-01 page-map work | SRV-01, RE-02 | STATIC_ANALYSIS_VERIFIED (hash-bound REA/Ghidra dossier and independent root PT_LOAD/body/reference verifier; static only, no allocator role or runtime claim) / root+/root/srv01_page_map_19660 | 0 |
| CPU-04 | Native startup and first world | CPU-01..03, SRV-01 | WAITING_FOR_DEPENDENCY | 0 |
| CPU-05 | Gameplay recovery batches | CPU-04 | WAITING_FOR_DEPENDENCY | 0 |
| AUD-01 | Audio assets/codec inventory | DATA-01 | ACTIVE (partial: 86 `.sxd`/`.hd`/`.bd` members have verified hashes/locators; XFF magic/length observations verified across 44 members; paired STS identity/byte comparison verified across 290 paths per layer; codec UNKNOWN; music/dialogue location open) / bc-71956f82+root+/root/audio_asset_survey | 0 |
| AUD-01-METADATA-SIDECAR-IDENTITY-01 | Verify decoded-byte identities for six sound metadata path families in cachedFiles and pakN | AUD-01, DATA-02 | ARTIFACT_VERIFIED (two corrected repeat runs byte-identical; 3 archive identities and 9 exact manifest/TOC locators, decoded lengths and member SHA-256 values verified; base/patch members differ in size and digest; no format/schema/codec/precedence/runtime claim) / root | 1 checker correction; 0 analysis hypotheses |
| AUD-01-XFF-01 | Characterize one paired XFF sound bank | AUD-01 | FIXTURE_VERIFIED (25 pair groups/50 members matched manifest identity; 44/44 XFF length words at 0x14 match decoded size; bounded reader + 6 synthetic tests pass; runtime use/codec UNKNOWN) / root+/root/audio_asset_survey | 0 |
| AUD-01-XFF-DECLARED-LENGTH-SHORTER-01 | Add a synthetic XFF regression for a declared length smaller than the full member buffer | AUD-01-XFF-01 | FIXTURE_VERIFIED (worker and root runs pass 7 tests; a declared word one byte shorter raises the existing length mismatch error; parser unchanged; no codec or playback claim) / root+`/root/app01_readiness` | 0 |
| AUD-01-XFF-BYTES-LIKE-INPUT-01 | Assert successful XFF parsing is identical for bytes, bytearray, and memoryview inputs | AUD-01-XFF-01 | FIXTURE_VERIFIED (worker and root each pass 8 tests; one valid fixture produces identical reports for all three input forms; parser unchanged; synthetic only) / root+`/root/aud01_xff_bytes_like` | 0 failed implementation tries |
| AUD-01-CLASSIFIER-SHORT-LE-PREFIX-01 | Add a synthetic regression for a truncated little-endian-looking audio prefix | AUD-01 | FIXTURE_VERIFIED (worker and root pass 13 tests; seven-byte prefix `01000000000000` falls back to `other-01000000`; classifier unchanged; no codec/runtime claim) / root+`/root/aud01_short_prefix` | 0 failed implementation tries |
| AUD-01-CLASSIFIER-ZLIB-OFFSET-02-FIXTURE-01 | Cover zlib audio-header decompression beginning at byte offset 2 | AUD-01 | FIXTURE_VERIFIED (worker and root each pass 20 tests; two-byte-prefixed synthetic zlib stream selects `zlib_offset2` and decodes 64 bytes; production classifier unchanged; see E140 and completion checkpoint; no codec/runtime claim) / root+review_host_proto | 0 failed tries |
| AUD-01-STS-IDENTITY-01 | Identify paired test_adpcm STS members | AUD-01, DATA-01 | FIXTURE_VERIFIED (290 exact paths in each cachedFiles layer; all locators checked and decoded hashes differ; no format or runtime claim) / root+/root/aud01_sts_cp3_identity | 0 |
| DATA-02-BRIDGE-SXD-01 | Compare the two cachedFiles `sound/shadow/bridge.sxd` members | DATA-01, DATA-02 | ARTIFACT_VERIFIED (private script validates exact archive/member identities and hashes and repeats byte-offset comparison; 619 aligned bytes, 152 equal/467 different, 19-byte patch suffix; no schema/override meaning; one corrected report-construction defect) / root+/root/data02_bridge_sxd | 1 |
| AUD-02 | Recover used voice/rack contracts | RE-02, AUD-01 | ACTIVE (partial: all NGS2/Audio3d/AudioOut imports have byte-verified callsites; region dossier gives init/shutdown, two worker threads and a mixer tick as labelled inference; formats, argument meanings, bank loading and contracts still unknown; no decoder/mixer yet) / root | 0 |
| AUD-02-NGS2-SYSCREATE-01 | Byte-check one callsite/dataflow path for `sceNgs2SystemCreateWithAllocator` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 26/26 dossier checksums and byte verifier PASS; direct caller-to-thunk path; runtime binding and reaching definition unresolved) / root | 0 |
| AUD-02-NGS2-RACKCREATE-01 | Byte-check one callsite/dataflow path for `sceNgs2RackCreate` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 45/45 dossier checksums and byte verifier PASS; direct caller-to-thunk path; unresolved indirect call/runtime binding remain) / root | 1 (worker checker correction; 0 failed analysis hypotheses) |
| AUD-02-NGS2-VOICE-CONTROL-01 | Byte-check one callsite/dataflow path for `sceNgs2VoiceControl` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 17/17 dossier checksums and byte verifier PASS; direct caller-to-thunk path; runtime binding and Ghidra adjacent-boundary edge remain unresolved) / root | 2 (MCP startup timeout; CLI provider not configured; 0 failed analysis hypotheses) |
| AUD-02-NGS2-SYSRENDER-01 | Byte-check one callsite/dataflow path for `sceNgs2SystemRender` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 72/72 dossier checksums and byte verifier PASS; slot `0x1601018` → thunk `0xf91450` → direct call `0x95383c` in `FUN_009536d0`; partial callee coverage, ABI/runtime unresolved) / root | 0 |
| AUD-02-NGS2-VOICE-GETSTATE-01 | Byte-check one callsite/dataflow path for `sceNgs2VoiceGetState` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 167/167 checksums, byte verifier and REA caller audit PASS; slot `0x1601028` → thunk `0xf91470` → direct call `0x94a27d` in `FUN_0094a200`; second edge `0x94a440` unverified; Ghidra thunk-boundary discrepancy retained) / root | 0 |
| AUD-02-NGS2-SYSDESTROY-01 | Byte-check one callsite/dataflow path for `sceNgs2SystemDestroy` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 68/68 checksums and verifier PASS; slot `0x1601010` → thunk `0xf91440` → direct call `0x9489d2` in `FUN_009485a0`; secondary caller `0x94a055` untraced; Ghidra adjacent-boundary/indirection edges retained) / root | 0 |
| AUD-02-NGS2-RACKCREATE-ALLOC-01 | Byte-check one callsite/dataflow path for `sceNgs2RackCreateWithAllocator` (symbol 236 / NID `U546k6orxQo` / slot `0x1600fe8`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay passed 93/93 dossier checksums and 13 instruction-byte records; slot `0x1600fe8` → thunk `0xf913f0` → direct call `0x9493f8` in `FUN_009485a0`; register/stack values captured without ABI labels) / root+worker | 0 failed targeted analysis/provider tries (one checker expectation correction) |
| AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01 | Byte-check one callsite/dataflow path for `sceNgs2RackQueryBufferSize` (symbol 239 / NID `0eFLVCfWVds` / slot `0x1601000`) | RE-02, AUD-01 | ACTIVE/PARTIAL (all 15 candidates now accounted for statically: 14 inside `FUN_0094ea10`, outlier `0x99fb7b` in `FUN_0099f9d0` (REA ev_8f3c6ca3…, root replay OK; see `docs/checkpoints/2026-10-20-root-ngs2-voicehandle-and-rackquery-second-callsite-complete.md`); callsite `0x94ebc2` in `FUN_0094ea10` (entry `0x94ea10`) now byte- and REA-verified, root replay of verifier + SHA256SUMS OK, 14 other candidates unattributed; root replay verified the thunk xrefs query and 16-file capture manifest; 15 returned addresses match 15 raw `E8 rel32` byte candidates, and independent ELF/PT_LOAD decoding confirms each displacement targets `0xf91420`; xrefs omit reference kind and raw bytes do not establish instruction boundaries or caller/function attribution; no caller/callsite accepted; no additional provider query released) / root+`/root/aud02_rackquery_buffersize` | 0 failed analysis hypotheses; 0 failed provider operations (1 root verifier assertion correction) |
| AUD-02-NGS2-RACK-VOICEHANDLE-01 | Byte-check one callsite/dataflow path for `sceNgs2RackGetVoiceHandle` (symbol 238 / NID `MwmHz8pAdAo` / slot `0x1600ff8`) | RE-02, AUD-01 | ACTIVE/PARTIAL (callsite `0x9496a8` in `FUN_009485a0` (`0x9485a0`, 5585 bytes) byte- and REA-verified via a function query (ev_7cd1fd53…, root replay OK; xrefs timeout not retried); 6 of 104 candidates in that body, other 98 unattributed; root offline replay verified 77/77 manifest entries and independently reproduced parser/byte scans; thunk candidate `0xf91410`, 104 raw CALL patterns; objdump windows have section-range warnings and do not establish instruction boundaries or caller/function attribution; one prior Ghidra 12.1.4 xrefs timeout at 330s remains the only provider failure; no retry released) / root+`/root/aud02_voiceflags_offline` | 1 provider failure; 0 hypothesis failures |
| AUD-02-NGS2-RACKDESTROY-01 | Byte-check one callsite/dataflow path for `sceNgs2RackDestroy` (symbol 237 / NID `lCqD7oycmIM` / slot `0x1600ff0`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root independently replayed function Evidence ID `ev_2b6853d79e403ff02e771eeab42c0c247cf4848362fe344cb5eb0341d02df601` and 66/66 dossier checksums; slot `0x1600ff0` → thunk `0xf91400` → direct call `0x94a01f` in `FUN_00949fe0`; no reachability/runtime/ABI/NGS2 behavior claim) / root+`/root/aud02_rackquery_buffersize` | 0 failed analysis hypotheses; 0 failed provider operations |
| AUD-02-NGS2-VOICE-GETSTATEFLAGS-01 | Byte-check one callsite/dataflow path for `sceNgs2VoiceGetStateFlags` (symbol 245 / NID `rEh728kXk3w` / slot `0x1601030`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 56/56 worker dossier checksums and root verifier PASS; bytes establish register setup at `0x9534c3`/`0x9534c7`, direct call `0x9534cb` in `FUN_00953470` → thunk `0xf91480` → RELRO/GOT slot `0x1601030` → symbol 245 / NID `rEh728kXk3w`; no ABI/parameter/runtime/NGS2/audio claim) / root+`/root/aud02_voiceflags_offline` | 2 root-verifier corrections; 0 failed analysis hypotheses; 0 failed provider operations |
| AUD-03 | Decoder/mixer/effect fixtures | AUD-02 | WAITING_FOR_DEPENDENCY | 0 |
| AUD-04 | Actual game audio integration | AUD-03, CPU-04 | WAITING_FOR_DEPENDENCY | 0 |
| SAV-01 | Recover game serializer/checksums | RE-02 | ACTIVE (partial: SaveData imports verified; dispatcher, 16-byte header, body hash, tagged-chunk container, stream flush chain and game-data chunk structure (15596 B from a `.bss` blob of `0x3cd8` bytes) decoded statically; blob semantics, profile chunk writers, load-side deserializer unknown; no native serializer, no real-save test) / root | 0 |
| SAV-02 | Durable native save services | SAV-01, SRV-01 | WAITING_FOR_DEPENDENCY | 0 |
| GPU-01 | Offline material/shader corpus | DATA-02, RE-02 | WAITING_FOR_DEPENDENCY (prep names DONE 2026-10-19 bc-3fe140af: 44,757 unique `.csdr`/`.cmat` paths in docs/GPU01_PREP.md + local/re/work/gpu01-*; no GCN/Metal) | 0 |
| GPU-02 | Prove GCN-to-MSL semantics | GPU-01 | WAITING_FOR_DEPENDENCY | 0 |
| GPU-03 | Metal resources/render graph | CPU-02, GPU-02 | WAITING_FOR_DEPENDENCY | 0 |
| GPU-04 | Real scene feature coverage | GPU-03, CPU-04 | WAITING_FOR_DEPENDENCY | 0 |
| APP-01 | Native Swift launcher | Documented native-core ABI | ACTIVE (shell builds; full suite 18 tests across 3 suites passes, including 9 lifecycle cases and 2 import-classifier cases; smoke-run 4 s; testable clean-exit/requested-Stop helper confirmed by user; one compile failure corrected; static UI review recorded; folder-import alert has no runtime exercise; sandbox/child-lifetime follow-ups remain; no core ABI or bundle) / principal+root | 1 |
| APP-01-TERMINATION-CLASSIFIER-01 | Move child-process termination classification into LauncherCore and test requested Stop, clean exit, and unexpected exit cases | APP-01 | FIXTURE_VERIFIED (pure helper delegates requested Stop, normal zero exit, nonzero exit, and signal outcomes; root filtered run: 11 tests/2 suites pass; full launcher suite: 18 tests/3 suites pass; no runtime process/UI claim) / root+`/root/app01_termination` | 0 failed implementation tries |
| APP-01-UI-A11Y-REVIEW-01 | Static macOS launcher UI/accessibility review | APP-01 | STATIC_ANALYSIS_VERIFIED (source paths and interaction findings recorded in E45; runtime UI, sandbox access, process-exit behavior, VoiceOver and keyboard validation excluded) / root+/root/app01_ui_review | 0 |
| APP-01-VOICEOVER-READINESS-01 | Inspect initial launcher readiness accessibility labels and announcements | APP-01 | WAITING_FOR_DEPENDENCY (computer-use app inventory reported Mac locked and automatic unlock failed; no app opened or UI action performed; waiting for user unlock) / root | 0 |
| APP-01-IMPORTER-ERROR-01 | Surface file-import errors in a system alert; keep cancellation silent | APP-01 | ARTIFACT_VERIFIED (testable LauncherCore classifier keeps Cocoa/Swift cancellation silent and returns localized text for ordinary errors; worker and root `swift test` each pass 14 tests across 3 suites; native alert interaction NOT RUN) / root+`/root/app01_import_cancel` | 1 (missing Foundation import corrected) |
| CPU-01-REGISTER-GAP-CTORS-01 | Register the four byte-verified constructors that were lifted but left unregistered | CPU-01 | FIXTURE_VERIFIED (indices 50 `0xe094e0`, 70 `0xdbfa60`, 82 `0xd85490`, 147 `0xce8440` now in `startup_ctors`; checker asserts all four and reports 108 RVA/index pairs; no runtime order change past the slot-11 blocker) / orchestrator | 0 |
| CPU-01-SLOT12-STRUCT-01 | Structural recovery of slot 12 / RVA `0xf06340` | CPU-01 | STATIC_ANALYSIS_VERIFIED (205-byte single-return body, 7 direct calls, 0 indirect; publishes vtable address point `0x15f9ef0+0x10` to `.bss 0x3355f88`, mallocs `0x10` bytes from `[0x12d2d1c]` for a mutex at `0x3355f98`, inits it via `0xb30950`, registers `0xe44a10` through `__cxa_atexit`; verifier exit 0) / orchestrator | 0 |
| CPU-01-SLOT13-STRUCT-01 | Structural recovery of slot 13 / RVA `0xf058f0` | CPU-01 | STATIC_ANALYSIS_VERIFIED (79-byte body, 4 direct calls, tail `__cxa_atexit`; singleton from `0xf030e0` published to `.bss 0x3355470`, heap mutex at `0x3355480`, shared cleanup `0xe44a10`; verifier exit 0) / orchestrator | 0 |
| CPU-01-SLOT14-19-FAMILY-01 | Structural recovery of slots 14–19 as one constructor family | CPU-01-SLOT12-STRUCT-01, CPU-01-SLOT13-STRUCT-01 | STATIC_ANALYSIS_VERIFIED (six bodies 79–284 B; all share the mutex-init + `malloc(0x10)` + `__cxa_atexit(0xe44a10, global, 0x1604218)` contract; slots 14/16/17/18 add one virtual `call [rbx]`; family verifier exit 0) / orchestrator | 0 |
| CPU-01-CTOR-FAMILY-MAP-01 | Classify all 573 constructor-table entries by reachable-body idiom | CPU-01 | STATIC_ANALYSIS_VERIFIED (227 `leaf_vec_copy`, 12 `guarded_const`, 34 `descriptor_mutex_atexit` in contiguous runs 10–19 and 24–47, 300 `other`, 0 `undecoded`; manifest `local/re/work/ctor-classes.json`; window-scan false positive `0xf53de0` and a byte-column parser truncation documented and fixed) / orchestrator | 0 |
| CPU-01-SLOT10-47-FAMILY-01 | Verify all 34 descriptor+mutex+atexit constructors as one contract | CPU-01-CTOR-FAMILY-MAP-01 | STATIC_ANALYSIS_VERIFIED (34/34 slots: exactly two `.bss` globals 0x10 apart, one `malloc(0x10)`, one `0xb30950` mutex init, exactly one `__cxa_atexit(0xe44a10)` call or tail call; `tools/lift/verify_ctor_family.py` exit 0; summary published in [CTOR_CLASSES](CTOR_CLASSES.md)) / orchestrator | 0 |
| CPU-01-LEAF-CTOR-BATCH-30 | Lift guarded-constant ctors `0xdd91b0` (idx 64), `0xdd7e30` (65), `0xdd4210` (66) | CPU-01-CTOR-FAMILY-MAP-01 | FIXTURE_VERIFIED (3/3 byte-exact: 9 guarded cells + unguarded tail per body; 330/396/486-byte spans; `check_fdd91b0.py`/`check_fdd7e30.py`/`check_fdd4210.py` exit 0; root re-decoded constants, masks and tail stores independently; 258/258 CTest) / subagent | 0 |
| CPU-01-LEAF-CTOR-BATCH-31 | Lift guarded-constant ctors `0xd39b00` (idx 115), `0xd366d0` (116), `0xd33b50` (117) | CPU-01-LEAF-CTOR-BATCH-30 | FIXTURE_VERIFIED (3/3 byte-exact: 467/302/302-byte spans; `0xd39b00` adds a 112-byte unguarded tail; checkers exit 0; 258/258 CTest) / subagent | 0 |
| CPU-01-LEAF-CTOR-BATCH-32 | Lift the AVX constant/table constructors `0xd8fa40` (idx 80), `0xd86ed0` (81), `0xd5a280` (105) | CPU-01-CTOR-FAMILY-MAP-01 | FIXTURE_VERIFIED (3/3 byte-exact: six 16-byte stores; five 32-byte YMM stores incl. 272-byte destination; and a 180-byte loop whose `add rax,0x20`/`cmp rax,0x200` count **dwords**, so the table is `0x800` bytes of the int32 identity `table[i]==i`, correcting an earlier `0x200` reading; checkers exit 0; 263/263 CTest) / subagent | 0 |
| CPU-01-LEAF-CTOR-BATCH-33 | Structural dossiers for `0xe71990` (idx 23), `0xdf94c0` (55), `0xde0160` (59); no lift | CPU-01-CTOR-FAMILY-MAP-01 | STATIC_ANALYSIS_VERIFIED (0 lifts by design: `0xe71990` 337 B, 14 `{rodata pointer, u32 hash}` records, no calls, hash function unknown; `0xdf94c0` 34 B calls `_FSin`; `0xde0160` **60 B** — the `ret` at `0xde021e` belongs to the next function at `0xde01a0` — calls `memset` and tail-jumps to `__cxa_atexit` with a `ret` stub and `0x1604218`; root re-derived all three spans/digests) / subagent | 0 |
| SAV-01-CHUNK-HEADER-01 | Native codec for the 12-byte tagged-chunk header | SAV-01-SERIALIZER-01 | FIXTURE_VERIFIED (`parse`/`encode`/`save_chunk_name_hash`; exact wire bytes, truncation 0..11, bad magic, zero/max payload, FNV vectors via a published vector plus an in-test loop; tag kept opaque; no serializer, save compatibility or real-save claim) / subagent | 0 |
| SAV-01-CHECKSUM-EDGE-01 | Boundary vectors for the native save checksum | SAV-01-NATIVE-CHECKSUM-01 | FIXTURE_VERIFIED (empty/single byte, published `"a"`/`"foobar"` vectors, seed sensitivity, 4 MiB span cross-checked against an independent in-test loop and chunked composition; `SIZE_MAX`-span case **NOT RUN** as not representable in memory; `save_checksum` unchanged) / subagent | 0 |
| AUD-01-AUDIO-HEADER-BOUNDARY-01 | Classification boundaries and input-type guard for `classify_audio_headers.family_of` | AUD-01 | FIXTURE_VERIFIED (6 tests, 13→19; empty/short/offset/anchoring cases; defect fixed: `None`/`''` returned `empty` and int/list leaked `AttributeError` → now `memoryview` coerced and other types raise `TypeError`; failing-before proven; signatures anchor at offset 0 and cannot collide, so branch order is defensive only) / subagent | 0 |
| AUD-01-XFF-LENGTH-EDGE-01 | Declared-length edges for the XFF member reader | AUD-01-XFF-01 | FIXTURE_VERIFIED (5 tests, 9→14; declared 0 rejected with the exact length message; unsigned `0xffffffff`/`0x80000000` preserved; 70,024-byte member accepted, killing a 16-bit assumption; short-buffer message pinned; parser unchanged) / subagent | 0 |
| DATA-02-MANIFEST-EMPTY-AND-WHITESPACE-01 | Degenerate and whitespace member-manifest inputs | DATA-02 | FIXTURE_VERIFIED (4 tests, 13→17; empty file is an error naming line 1 by design, not zero records; blank-only and trailing-blank cases name the right line; `{"path":""}` rejected; backslash paths accepted verbatim and flagged as deferred risk for any future consumer; audit script unchanged) / subagent | 0 |
| CPU-01-STATIC-CLEANUP-THUNKS-01 | Classify and verify the 33-byte static-cleanup constructor thunks | CPU-01-CTOR-FAMILY-MAP-01 | STATIC_ANALYSIS_VERIFIED (15 slots: 515, 522, 523, 525, 527, 528, 531, 533, 542, 546, 547, 549, 551, 564, 565; each 33 B with `lea rsi/rdi/rdx; mov qword [rsi],0; jmp __cxa_atexit`, dso `0x1604218`, one `.bss` cell + one destructor; body digests pinned; `tools/lift/verify_cleanup_thunks.py` exit 0; no lift because destructor semantics are unknown) / orchestrator | 0 |
| CPU-01-CTOR21-CONST-TABLE-01 | Lift slot 21 / RVA `0xee1130` as a byte-exact 128-byte constant table | CPU-01-CTOR-FAMILY-MAP-01 | FIXTURE_VERIFIED (182-byte call-free body, sha256 `0c983636…349d`; 15 stores cover `0x33540f0..0x3354170` with no gaps; checker models the four register/vector reloads and ties every byte to the ELF; 265/265 CTest; negative control fails then passes; **data lift only, cell meanings unknown**; not registered) / orchestrator | 0 |
| CPU-01-FAMILY-NATIVE-HELPER-01 | Native lifetime model for the 34-slot descriptor+mutex+atexit family | CPU-01-SLOT10-47-FAMILY-01, SRV-01-RECURSIVE-MUTEX-01 | FIXTURE_VERIFIED (`DescriptorRegistration` one-shot `publish` with explicit `null_descriptor`/`already_published`/`mutex_init_failed`, owns a recursive+PI mutex; tests cover null, re-publication, independence, recursion and cross-thread exclusion; 264/264 CTest; negative control fails then passes; **deliberately not** the original heap block/atexit ordering; not registered) / orchestrator | 0 |
| DOC-ARCHIVE-LINKS-01 | Repair the broken links in the archived handoff | DOC-REFERENCE-AUDIT | DONE_DOC (all 62 root-relative links in `docs/checkpoints/2026-10-20-handoff-archive-through-1f7c8e8.md` rebased to `../../`; repo-wide check now finds 235 internal links and 0 missing; content unchanged) / orchestrator | 0 |
| DATA-02-PSARC-OFFSET-BEYOND-01 | Reject a synthetic archive whose member offset exceeds the file | DATA-02 | FIXTURE_VERIFIED (17 psarc tests; exact `entry 1 offset beyond archive`; mutation caught) / subagent | 0 |
| DATA-02-PSARC-MANIFEST-COUNT-01 | Reject a manifest whose path count disagrees with the entry count | DATA-02 | FIXTURE_VERIFIED (18 psarc tests; both one-too-many and one-too-few; exact messages) / subagent | 0 |
| DATA-02-PSARC-BE-BOUNDS-01 | Pin `psarc.be()` bounds behaviour | DATA-02 | FIXTURE_VERIFIED (19 psarc tests; `pos+n==len` and `n==0` allowed, negative and past-end rejected) / subagent | 0 |
| DATA-02-PSARC-PARTIAL-READ-01 | Pin `Psarc.read(index, limit)` prefix semantics | DATA-02 | FIXTURE_VERIFIED (20 psarc tests; stored and zlib members, `limit 0 -> b''`, clamp, cross-block) / subagent | 0 |
| DATA-02-OVERLAY-EMPTY-01 | Reject an empty overlay layer and duplicate exact paths in a member JSONL | DATA-02 | FIXTURE_VERIFIED (23 psarc tests; empty-overlay and duplicate-path messages, single-layer stats 1/1/0 and exact resolve hit/miss) / subagent | 0 |
| RE-02-DYNLINK-SEGMENT-SPAN-01 | Pin `Image.read` segment-span and `filesz<memsz` rejection | RE-02 | FIXTURE_VERIFIED (14 dynlink tests; gap-crossing and bss-tail reads raise `not file-backed`; documented that size 0 at a segment end succeeds with `b''`) / subagent | 0 |
| RE-02-DYNLINK-PHDR-MACHINE-01 | Reject wrong ELF class/data/machine and truncated program-header tables | RE-02 | FIXTURE_VERIFIED (17 dynlink tests; exact `expected little-endian x86-64 ELF`; no `IndexError`) / subagent | 0 |
| DATA-02-MANIFEST-RECORD-SHAPE-01 | Malformed member-JSONL records raise `PsarcError` naming file and line (defect fix) | DATA-02 | FIXTURE_VERIFIED (26 psarc tests; non-object, missing `path`, non-string `path`; new tests fail on the old code) / subagent | 0 |
| APP-01-A11Y-LABELS-01 | Tested accessibility label/value for every readiness kind × status | APP-01 | FIXTURE_VERIFIED (42 launcher tests / 7 suites; NOT BUILT wording verbatim; runtime VoiceOver NOT RUN) / subagent | 0 |
| APP-01-DIAGNOSTICS-SUMMARY-01 | Deterministic four-subsystem diagnostics text | APP-01 | FIXTURE_VERIFIED (fixed order, all-ready/empty/partial cases, no omission; UI not wired) / subagent | 0 |
| APP-01-ASSET-VALIDATOR-REPORT-01 | Ordered asset problem report with `missingArchives` | APP-01 | FIXTURE_VERIFIED (temp-dir cases incl. chmod-000 as uid 501 and directory-named-like-archive regression; `validate` contract unchanged) / subagent | 0 |
| APP-01-LIFECYCLE-UX-01 | Honest lifecycle status line and recovery hint | APP-01 | FIXTURE_VERIFIED (3 outcomes + 7 states; requested Stop never a crash and no hint; `classifyTermination` unchanged) / subagent | 0 |
| INT-01 | Actual menu -> controllable world | CPU-04, GPU-04, APP-01 | WAITING_FOR_DEPENDENCY | 0 |
| INT-02 | Real audio/save/restart route | INT-01, AUD-04, SAV-02 | WAITING_FOR_DEPENDENCY | 0 |
| REL-01 | Assets-only, native-only package | INT-02 | WAITING_FOR_DEPENDENCY | 0 |
| PERF-01 | Measured native optimization | INT-02 correctness baseline | WAITING_FOR_DEPENDENCY | 0 |

## How to claim and close a task

Copy the task ID into HANDOFF with agent ID, prerequisite evidence, intended owned files,
one bounded outcome, exact check and baseline failed-try count. Never claim an entire subsystem
without a small measurable next checkpoint. A new native module must include a README mapping
its native API and test command. Update this register after each meaningful attempt.

At three failed tries, use [escalation template](escalations/TEMPLATE.md), mark NEEDS_PRINCIPAL,
record all hypotheses and stop that task. Repeated timeouts are not three new hypotheses.
The principal must explicitly reassess before any fourth attempt. Do not reset the counter.

## SETUP-01 / DATA-01 — tool and local input baseline

Scope: native extractor only; no game execution. Build from pinned ShadPS4Plus source with
the out-of-tree CMake adapter. Exact commands and known exit-code trap are in [EXTRACTION](EXTRACTION.md).
Verify Mach-O ARM64, parse package category, extract into separate base/update/DLC directories,
hash original PKGs, validate expected extracted sizes/headers, record title/version from SFO.

Deliver `local/manifests/packages.json`, extraction logs and a base/update precedence map.
Do not overlay `cachedFiles.psarc` and `cachedfiles.psarc` by case-blind copying. Do not treat
exit 0 alone as extraction proof: upstream sometimes prints failure then returns zero.
Exit: G0 artifact inventory complete; original PKGs unchanged; no keys in shared docs.

## DATA-02 — archive and asset schemas

Inventory PSARC headers, manifests, compression/encryption flags and content digests. Retain
base/update origins. Read only a bounded first manifest before deciding to expand the corpus.
Classify meshes, skeletons, animations, materials, shaders, sound banks/streams, localization,
collision, scripts and scene data by validated container bytes and readers, not just names.

Deliver a local asset manifest with archive hash, member ID/path, byte span, content hash,
parser version, schema confidence, override source and unresolved formats. Add one minimal
parser fixture for each admitted format. Reject out-of-range lengths, path traversal and
unknown compression explicitly. Exit: selected scene's required assets resolvable repeatably.

Verified slice (2026-10-19): the private `pakN.psarc` identity now records its streamed SHA-256
and exact archive byte count. This closes only archive identity; per-member schemas, TOC digest
meaning, `pakN` encoding, and selected-scene resolution remain open.

## RE-01 / RE-02 — seed analysis and native boot dependency graph

Read [REVERSE_ENGINEERING](REVERSE_ENGINEERING.md). Entry is ELF-relative `0xf120` for the
identified local ELF; do not transplant that address to a different hash or relocated image.
Retain original SELF and reconstructed ELF identities, load blocks/base and missing segments.

RE-01 delivers REA/Ghidra evidence export and a reproducible persistent-project recipe.
RE-02 parses PS4 dynamic symbol/NID and relocation tables with the pinned `prepare.py` as a
format reference, then maps entry, module constructors, global initialization, allocator,
thread/job startup, asset mounts, engine initialization, first scene, update and presentation.
Build a function manifest and unresolved edge list. Inspect callers/callees and data xrefs,
including callbacks/vtables; an unrecognized NID is not permission to return success.

Exit: an address/hash-bound startup slice with typed inputs/outputs and every required import
classified as native library contract, recovered game function, optional feature or blocker.
Record whether indirect-call coverage is complete; do not estimate full-game coverage from
Ghidra's discovered-function count.

## CPU-01 — principal-owned native recovery feasibility

Start with one pure, nontrivial function reached in initialization (bounded input, deterministic
output, no platform calls), then a small call chain with one native service. Use Ghidra
pseudocode plus original assembly and data flow; preserve arithmetic, SIMD, flags where
observable, layouts and aliasing. Hand recovery is the fallback if an offline lifter cannot
emit understandable typed native code. Do not choose a tool merely because it emits LLVM IR.

Deliver conventional C++/C native functions, original-to-native symbol records, independent
fixtures and an ARM64-only build. No guest CPU register struct/dispatch loop or executable
blob may remain. Do not execute original x86 on this Mac to manufacture a passing oracle.
Use mathematically independent fixtures, binary semantics, or explicitly separately authorized
recorded observations; state what remains uncorroborated.

Exit G1: demonstrate a native initialization call chain and identify a feasible path to
engine startup, or escalate the smallest failed function after three attempts. This task is
the first major feasibility decision. Do not bury it behind launcher/renderer polish.

## CPU-02 — layouts and pointer representation

Anchors: source probe.c:55-83, runtime_memory.c:1-44,704+, runtime_thread.c:287.
Recover every used 31-bit tagged image/heap offset separately from 40-bit GPU descriptor
addresses and ordinary host pointers. Record null/tag/base/width/overflow/alignment rules.
Use offset/handle decoding into native storage rather than assuming sub-2GiB mappings.
Prove any unavoidable low-address reservation on the signed Darwin executable before adopting it.

Exit: size/offset assertions and independent boundary/invalid-input/alias tests. Never serialize
native pointers or substitute host layouts for packed asset structures.

## CPU-03 — jobs, callbacks, TLS and fibers

Anchors: runtime_fiber.c:29-172; runtime_thread.c:1-108; runtime.h:18.
Recover used scheduling semantics first. Native task scheduler must preserve task state across
yield/migration, destination-worker TLS, correct ARM64 stack alignment/callee-saved SIMD state,
FP control and unwind rules. Replace x86 FS/GS rewriting and Linux arch_prctl completely.
Typed callback records replace executable-address casts. Do not use deprecated context APIs
without verifying supported Darwin/ARM64 behavior and lifetime semantics.

Exit: create/yield/resume/complete/finalize, migration, thousands of switches, cancellation,
TLS isolation, nested callbacks, guard pages, shutdown and a small real boot job graph pass.

## SRV-01 / CPU-04 / CPU-05 — native engine execution

SRV-01: implement the actually reached filesystem/Fios, memory, synchronization, timing,
controller, user/service and video interfaces. Normalize paths with an explicit case policy;
honor asynchronous completion, file offsets, short reads, cancellation and failure codes.
Use native clocks with identified units; do not transplant cycle-counter assumptions.

CPU-04: recover constructors and startup in boot-graph order. Replace libc/Fios/ULT and any
other used executable module with native code/contracts. Resolve vtables/function pointers
before they can run. Reach resource discovery, scene load and one real simulation update.
Stubs must fail visibly on required calls. Exit: G1/G2 startup chain can feed actual scene data.

CPU-05: split gameplay into hash/address-bound batches: scene streaming, player/camera/input,
animation/skinning, physics/collision, enemy/colossus logic, scripting, UI, progression and
shutdown. Each batch adds function coverage, data schema, fixtures and a real scenario when
available. All 16 colossi and required game modes need explicit eventual coverage; the first
controllable area is a vertical slice, not game completion. Never invent a universal function
count or timeline before resolving indirect code and data-driven behavior.

## AUD-01 / AUD-02 — identify sound and recover the used audio engine

AUD-01 begins with archive bytes and bank readers. `sce_sys/snd0.at9` is a package/menu audio
asset, not proof of gameplay codec coverage. Identify actual bank/stream headers, compression,
framing/configuration, channel layouts, rate, loops, seek state, event IDs and language variants.
Deliver a hash-bound audio inventory with at least one validated representative of every used
codec/container; state unparsed members. Do not infer NGS2 graph semantics from PCM output.

Verified slice (2026-10-19): all 86 classified members now carry full decoded-member SHA-256 and
PSARC entry ID/offset/first-block locators. Codec/container meaning, gameplay use, and
music/dialogue coverage remain open.

AUD-02 starts at imported sceNgs2 calls and their callers: system/rack creation, parameter
buffer construction, voice allocation/patching/control, render invocations and callbacks.
Decode command byte layouts and lengths, rack/voice state transitions, routing, expected
return/error behavior and lifetimes. Find Audio3D usage and determine actual ULT dependencies.
Deliver a bounded required API/command matrix linked to original call sites and real bank IDs.
Do not try to reconstruct an entire Sony firmware when the game only needs a used subset.

Exit: every operation used by the chosen scene is specified or an explicit blocker. Evidence
must be from game code/data and source contracts, not silent NGS2 stubs in either reference.

## AUD-03 / AUD-04 — native mixing and real game sound

AUD-03 implements native decoding, voice cursors, loops, pitch/resampling, gain/pan/channel
matrix, buses, needed effects, synchronization and completion callbacks. Use verified native
LibAtrac9 or other required codecs; initialize dependencies only after actual format evidence.
Use worker decoding/mixing and preallocated bounded PCM queues feeding Core Audio.
No allocation, file I/O, blocking locks or logging in the real-time callback.

Tests: known decode sample counts/duration, channels, valid sample range/NaN rejection, loop
boundaries, gain/pan, simultaneous voices, stop/steal/teardown, underrun behavior, rate change,
seek, and deterministic offline mix fixtures. Nonzero output alone is not correctness.

AUD-04 connects recovered game events to those voices; verify title/menu, dialogue/music,
ambience, footsteps, combat, positional movement, pause/resume and scene transitions. Save
source event/voice traces, output capture and an audible live check. Device reconnect/sleep,
shutdown and stress loading must pass. Exit: actual game sound with all Sony modules absent.

## SAV-01 / SAV-02 — game serialization and durable storage

SAV-01 traces used SaveData calls into the game's own serializer/checksum/version handling.
Determine files/slots, payload structure, progress variables, commit boundary, async callback,
expected errors and compatibility constraints. A working host directory is not a serializer.
Record exact native versus original-console-save compatibility goals separately.

SAV-02 maps native profiles to Application Support. Stage same-volume writes, validate payload,
commit transactionally, preserve previous valid generation and report errors. For multi-file
saves, use a generation/manifest transaction so an interruption cannot mix slot generations.
Do not copy raw host object memory or preserve live pointers. Import by copying, never altering
the user's source save. No Bloodborne-specific patch should enter SoTC save handling.

Exit: actual checkpoint/progress survives quit/relaunch; corrupted/truncated/unwritable/full-disk
and kill-before/during/after-commit tests preserve the last valid save. Strong power-loss
durability needs a verified flush protocol beyond atomic rename; do not claim it implicitly.

## GPU-01 / GPU-02 — offline shader/material recovery

GPU-01 inventories the real game's shaders, stages, GCN instructions, resource/vertex layouts,
material state and compute jobs. Preserve shader hashes and scene provenance. Reuse the
vendored GCN decoder/IR semantics and SoTC fixes as reference; generate native Metal shaders
and pipeline manifests offline. Production game-facing calls must use a native renderer API,
not a runtime PS4 PM4/GCN machine. Audit dynamic shader variants and provide an explicit offline
conversion/install strategy; any unreachable or generated-at-runtime case remains a blocker.

GPU-02 proves semantics before a large renderer. Test 64-lane EXEC/WQM masks and helper lanes,
cross-half shuffles/ballots/reductions, divergence/reconvergence, scalar loads, LDS/barriers,
atomics, interpolation and typed storage. Query actual SIMD width. Two SIMD groups do not
automatically equal one wave64; compute workgroup techniques do not transfer directly to
fragment/vertex shaders. Apple's Shader Converter accepts DXIL, not PS4 GCN.

Exit: every instruction/control/resource behavior used by the selected shader slice passes
independent fixtures. Unsupported lowering is diagnostic and blocks that material. Preserve
precise SoTC readbacks, water/WQM and streaming fixes before performance changes.

## GPU-03 / GPU-04 — direct Metal scene renderer

GPU-03 creates device, native command queues/buffers, drawable presentation, textures/buffers,
pipeline state and synchronization. Native render descriptions replace original GPU addresses
with checked resource handles/offsets. Prove aliasing, lifetimes, tiled texture conversion,
compressed/depth formats, mip/cube arrays, CPU/GPU ordering and readbacks. Select a Metal API
baseline explicitly; never mix Metal 3 tracked-hazard assumptions with Metal 4 queue behavior.

GPU-04 extends a real scene through vertex fetch, depth, shading, animation/skinning, shadows,
vegetation, compute, particles, water/reflections, postprocessing and UI. Keep an explicit
feature/scene coverage matrix. Validate on title, world streaming, water and save reloads.
Exit G3/G5: captures show direct Metal commands, correct content and no validation faults;
no Vulkan/MoltenVK link or loaded image. A triangle/converted static mesh remains a fixture.

## APP-01 — native Mac launcher

Can begin UI shell independently; cannot mark boot working before INT-01. Implement SwiftUI
with AppKit/MetalKit where needed, native file selector, settings/menu/shortcuts, input and
accessibility. Expose a small versioned C/Objective-C++ native-core API for validate/start/
stop/status/errors. Choose one ownership model (embedded game view or native child game
process) deliberately and record it; both must remain native and responsive. Avoid Python/Tk,
Electron or x86 preparation helpers in the installed app.

Model states validating/ready/starting/running/stopping/failed. Distinguish asset availability,
CPU recovery coverage, Metal compatibility and audio readiness in diagnostics. Keep startup and
asset work off MainActor. Finder launch must work with a clean environment. Verify resize,
Retina, fullscreen, display changes, focus, controller hotplug, cancellation, close and relaunch.

## INT-01 / INT-02 / REL-01 / PERF-01 — evidence before completion

INT-01 demonstrates original menu -> new game -> controllable first world using recovered
native logic, real assets and Metal. Record input and corresponding simulation/render outcome.
INT-02 adds actual audio plus meaningful save -> process exit -> relaunch -> load. Test repeat
visits, transitions, streaming, first colossus and increasing coverage through the game.

REL-01 stages an assets-only app install. Remove all original executables/modules and clear
the environment; audit Mach-O architecture/dependencies and file/loaded-image traces across
boot, gameplay, audio and save/load. Search for renamed embedded code blobs as well as filenames.
Package manifests must identify only native code, converted shaders and allowed data. Test
from Finder and a fresh profile. Record signing/notarization behavior separately from runtime.

PERF-01 begins after a correct repeatable baseline. Measure CPU/GPU times, p50/p95/p99 frame
intervals, shader compile stalls, memory pressure, load times and audio underruns, with actual
rendered/output resolutions and thermal/cache state. Compare one change at a time. Start
with batching/caches and CPU/GPU overlap justified by traces. Copied Bloodborne fence/readback
or pipeline changes require SoTC-specific boot/stream/save regression checks. Do not claim an
M4 frame rate from upstream RTX/RADV numbers or synthetic scenes.

## Implementation folder proposal (not implemented)

`native/core/` recovered game functions; `native/platform/` native service contracts;
`native/render/` Metal backend/MSL; `native/audio/` native voices/decoders/mixer;
`native/save/` serializer/services; `launcher/` Swift app; `tests/` fixtures/integration routes.
Create a directory only when its first task is claimed. Keep original source addresses and
artifact identities in local manifests; publishable code must not include original code blobs.

## Orchestrator session 2026-10-21 additions

| ID | Task | Depends on | Status / owner | Failed tries |
|---|---|---|---|---|
| CPU-01-LEAF-CTOR-BATCH-34 | Lift 12 call-free leaf ctors (idx 172/174/176/179/187/188/229/237/285/304/321/466) | CPU-01 | FIXTURE_VERIFIED (12 byte checkers + strict decoder `tools/lift/leaf_ctor_decode.py`; 281/281 CTest; unregistered; 6 tail-call bodies and 7 true leaves remain; see `docs/checkpoints/2026-10-21-agentA-native-batch-34.md`) / subagent A | 0 |
| SAV-01-NATIVE-STREAM-01 | Native tagged-chunk save container with outer header, hash and atomic file I/O | SAV-01-NATIVE-CHECKSUM-01 | FIXTURE_VERIFIED (`native/core/save_stream`; 6 mutations caught; payload semantics UNKNOWN; no real-save compatibility; see `docs/checkpoints/2026-10-21-agentD-sav01-native-stream.md`) / subagent D | 0 |
| APP-01-CORE-ABI-01 | Versioned C ABI for the native core plus Swift `CoreClient` | APP-01 | PARTIAL (`native/api`; `start()` = ENGINE_NOT_IMPLEMENTED, never running; ctest `sotc_core` 2/2; launcher not yet routed through CoreClient; G4 NOT PASSED; see `docs/checkpoints/2026-10-21-agentC-app01-core-abi.md`) / subagent C | 0 |
| CPU-01-NAMEHASH-E71990-01 | Identify the name hash behind ctor `0xe71990` | CPU-01 | STATIC_ANALYSIS_VERIFIED (`0xe70aa0` djb2-XOR, 0 remapped to 1; 13/13 constants recomputed; native `name_hash` tested; ctor has no call, premise corrected; see `docs/checkpoints/2026-10-21-agentB-namehash-e71990.md`) / subagent B | 0 |
| CPU-01-VTABLE-15F9EF0-01 | Map vtable `0x15f9ef0` (slot 12 class) | CPU-01-CTOR11-01 | STATIC_ANALYSIS_VERIFIED (point `0x15f9f00`, 222 slots, slot 12 = `0xf06330`; class identity is inference; 36 checks) / subagent B | 0 |
| APP-01-LAUNCHER-CORECLIENT-01 | Route LauncherModel through CoreClient | APP-01-CORE-ABI-01 | PARTIAL (implementation remains at 89 passing tests; native window/AX inspection is awaiting manual Mac unlock; VoiceOver/Finder/signing NOT RUN; see `docs/checkpoints/2026-10-21-root-app01-launcher-runtime-claim.md`) / root | 0 failed implementation tries |
| CPU-01-LEAF-CTOR-BATCH-35 | Lift remaining 7 true leaf ctors (idx 193/225/319/320/417/526/548) | CPU-01-LEAF-CTOR-BATCH-34 | FIXTURE_VERIFIED (symbolic decoder; 2 bodies take explicit runtime-state parameters; 0x56fae0 and 0x8bd40 are tail calls, skipped; see `docs/checkpoints/2026-10-21-agentF-batch-35-and-checkers.md`) / subagent F | 0 |
| CHECKERS-STALE-01 | Make every tools/lift checker pass | CPU-01-LEAF-CTOR-BATCH-34 | FIXTURE_VERIFIED (fb747f0/fb76950 registered in CMake; math_ctor_clones asserts the verified shared-cell fact; 287/287 exit 0) / subagent F | 0 |
| CPU-01-CTOR11-REFLECTION-SCOPE-01 | Bound the reflection contract for slot 11 | CPU-01-CTOR11-01 | STATIC_ANALYSIS_VERIFIED (667 assertions; records are a lazy schema tree built only by vtable getters; none reached from startup; see `docs/checkpoints/2026-10-21-agentG-ctor11-reflection-scope.md`) / subagent G | 0 |
| CPU-01-NATIVE-SLOT11-01 | Register slot 11 natively with a lazy param-def tree | CPU-01-CTOR11-REFLECTION-SCOPE-01 | FIXTURE_VERIFIED (`LazyParamDefTree::get()` = schema_not_available, allocation-free; 74 records NOT modelled; see `docs/checkpoints/2026-10-21-agentI-native-slot11.md`) / subagent I | 0 |
| CPU-01-NATIVE-SLOT12-19-01 | Register slots 12–19 (registration thunks) and slot 21 | CPU-01-NATIVE-SLOT11-01 | FIXTURE_VERIFIED (registration-only; opaque descriptors; classes behind vtables unidentified; runner now stops at idx 23 RVA `0xe71990`; ctest 294/294, checkers 287/287; see `docs/checkpoints/2026-10-21-agentJ-native-slot12-19.md`) / subagent J | 0 |
| SAV-01-PROFILE-CHUNKS-01 | Decode profile/progress chunk writers and loader policy | SAV-01-SERIALIZER-01 | STATIC_ANALYSIS_VERIFIED (232 checks, 47/47 mutations; blob length prefix inside payload_length; tag 4 vs 8 UNRESOLVED; case folding inference; see `docs/checkpoints/2026-10-21-agentH-sav01-profile-chunks.md`) / subagent H | 0 task-level |
| SAV-01-CHUNK-CODEC-01 | Typed chunk codec, spec tables, descriptor gate | SAV-01-NATIVE-STREAM-01, SAV-01-PROFILE-CHUNKS-01 | FIXTURE_VERIFIED (15/15 mutations killed; no real-save claim; see `docs/checkpoints/2026-10-21-agentK-sav01-chunk-codec.md`) / subagent K | 0 |
| CPU-01-NATIVE-SLOT24-47-01 | Model descriptor/mutex registration thunks for slots 24–47 behind a test-only ahead table | CPU-01-SLOT10-47-FAMILY-01, CPU-01-NATIVE-SLOT12-19-01 | FIXTURE_VERIFIED (focused 1/1; all native tests except separately escalated host test pass 295/295; 34-row/source checkers pass; production still stops at slot 23; see `docs/checkpoints/2026-10-21-root-cpu01-slot24-47-complete.md`) / root | 1 stale CMake generation |
| APP-01-CORE-HOST-01 | Harden the native child-process host protocol and pipe tests | APP-01-CORE-ABI-01 | NEEDS_PRINCIPAL (three failed targeted tries; attempt 3 does not meet expected bad-stdin exit 4; actual wait status not printed; no fourth try until reassessment; see `docs/escalations/APP-01-CORE-HOST-01.md`) / root + worker | 3 |
| CPU-01-SLOT23-CONSUMERS-01 | Determine whether slot 23's name/hash registry requires its original strings | CPU-01-LEAF-CTOR-BATCH-33, CPU-01-NAMEHASH-E71990-01, RE-01 | PARTIAL (pointer path to `0xe70af0` observed; reachable `0xe5cd78` hash path reaches per-object lookup `0xe715d0`; 31 indirect transfers and string necessity unresolved; Ghidra 12.1.4 startup timed out at 330 s, no function Evidence; no string import/native registration/G1; see `docs/checkpoints/2026-10-21-root-cpu01-slot23-consumers-partial.md`) / root | 0 hypothesis failures; 1 provider failure |
| DATA-02-PAKN-TOC-DIGEST-UNIQUENESS-01 | Count repeated 16-byte TOC `md5` field values in pakN's private member manifest | DATA-02 | ARTIFACT_VERIFIED (root ran two byte-identical metadata-only aggregations: 64,108 rows and unique values, no repeated groups; digest semantics remain UNKNOWN; see E122 and completion checkpoint) / root | 0 hypothesis failures |
| DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01 | Compare TOC `md5` field values for exact-path overlaps across the seven archive manifests | DATA-02, DATA-02-ARCHIVE-OVERLAP-MAP-01 | ARTIFACT_VERIFIED (root twice replayed 21 archive pairs; 40,216 exact-path overlap occurrences, 4,479 equal and 35,737 different TOC values; identities unchanged; no payload read or member paths emitted; see E123 and completion checkpoint) / root | 0 hypothesis failures |
| DATA-02-PSARC-DECODED-LENGTH-MISMATCH-01 | Pin the `Psarc.read` failure when a compressed block decodes to the wrong byte count | DATA-02, DATA-02-PSARC-PARTIAL-READ-01 | FIXTURE_VERIFIED (synthetic compressed member asserts exact error `block 4 decoded to 16 bytes, expected 15`; focused PSARC suite 27/27; parser unchanged; see E124 and completion checkpoint) / root | 0 failed tries |
| DATA-02-TOC-VALUE-DISTINCT-PATH-REUSE-01 | Count opaque TOC values that recur across distinct exact member paths in the seven archive manifests | DATA-02, DATA-02-ARCHIVE-OVERLAP-MAP-01, DATA-02-ARCHIVE-OVERLAP-TOC-DIGEST-01 | ARTIFACT_VERIFIED (root replay: 170,451 rows; 35,737 TOC values span multiple paths; 4,479 same-path cross-archive equal pairs; byte-identical aggregate and identity records; aggregate-only outputs; no payload or digest-semantics claim; E126 and checkpoint) / root + `launcher_review` | 0 failed tries |
| DATA-02-MANIFEST-JSON-SYNTAX-01 | Make malformed nonblank member-JSONL records raise `PsarcError` with manifest path and 1-based line | DATA-02, DATA-02-MANIFEST-RECORD-SHAPE-01 | FIXTURE_VERIFIED (old parser: 28 tests, 2 expected JSONDecodeError errors; fixed parser + root replay: 29/29; blank lines preserve prior JSONDecodeError behavior; E125 and completion checkpoint) / root + `slot23_raw_xrefs` | 0 failed implementation tries |
| AUD-01-XFF-READER-IDENTITY-FIXTURE-01 | Add synthetic identity-validation and cleanup coverage for `audio_bank_header.inspect_members` | AUD-01-XFF-01 | FIXTURE_VERIFIED (worker retry 2 and root replay each pass 16/16; one failed fixture attempt expected a resolved summary path, corrected without parser change; success asserts full identity and reader close, mismatch asserts error and reader close; private logs and hashes in checkpoint E127; no live archive or codec claim) / root + `review_host_proto` | 1 failed fixture attempt |
| RE-02-DYNLINK-PHDR-ENTRY-SIZE-01 | Reject an ELF64 program-header table whose `e_phentsize` is not 56 | RE-02, RE-02-DYNLINK-PHDR-MACHINE-01 | FIXTURE_VERIFIED (synthetic `e_phentsize=55` test fails before guard; worker and root each pass 18/18 after guard; parser/test hashes and private logs in E128; no live ELF or loader claim) / root + `slot23_raw_xrefs` | 1 successful implementation attempt; 0 failed tries |
| DATA-02-OVERLAP-MAP-UNSHARED-SIZE-VALIDATION-01 | Validate every member-map size, including paths unshared by any archive pair | DATA-02-ARCHIVE-OVERLAP-MAP-01 | FIXTURE_VERIFIED (synthetic `-1` and `True` unshared paths fail before the fix; worker and root each pass 18/18 after whole-map validation; E129 records hashes/logs; no real manifest/archive claim) / root + `launcher_review` | 1 successful implementation attempt; 0 failed tries |
| DATA-02-PSARC-STORED-BLOCK-TRUNCATION-01 | Pin rejection of a truncated full-size raw PSARC block | DATA-02, DATA-02-PSARC-PARTIAL-READ-01 | FIXTURE_VERIFIED (synthetic 16-byte raw member truncated by one byte asserts exact decoded-length error; worker and root each pass 30/30; parser unchanged; E130 has hashes/logs; no real archive claim) / root + `slot23_raw_xrefs` | 0 failed tries |
| DATA-02-PSARC-BLOCK-TABLE-WIDTH-01 | Cover two-byte PSARC block-table entries in a synthetic archive fixture | DATA-02-PSARC-PARTIAL-READ-01 | FIXTURE_VERIFIED (worker and root each pass 31 tests; block size 257 with raw table value 256 exercises the nonzero high byte and roundtrips; parser unchanged; see `docs/checkpoints/2026-10-21-root-data02-psarc-block-table-width-complete.md`) / root+`launcher_review` | 0 failed implementation tries |
| DOC-FOLDER-INDEX-ADMIN-DIR-01 | Cover ignored `.git` and `__pycache__` trees in generated folder-index checks | None | FIXTURE_VERIFIED (synthetic temp-root test passes worker/root 5/5; no source change; E131 and completion checkpoint; live index behavior unclaimed) / root + `slot23_raw_xrefs` | 0 failed tries |
| RE-01-UNWRAP-INPUT-SIDECAR-COLLISION-01 | Refuse an identity-sidecar path that aliases the SELF input | RE-01, RE-01-ELF-PHDR-TRUNCATION-FIXTURE-01 | FIXTURE_VERIFIED (pre-write same-file guard and synthetic regression; worker and root each pass 6/6; E132 and completion checkpoint record hashes/logs; no game or live-SELF claim) / root + `slot23_raw_xrefs` | 0 failed implementation tries |
| AUD-PUBLISH-SECRET-NAME-FIXTURE-01 | Cover nested case-insensitive .env and credentials filename deny rules | AUD-PUBLISH-NESTED-01 | FIXTURE_VERIFIED (synthetic nested .ENV.local and CREDENTIALS.txt path cases; worker and root each pass 7/7; E133 and completion checkpoint record hashes/logs; production audit unchanged) / root + launcher_review | 0 failed tries |
| DATA-02-OVERLAP-MAP-JSON-CONSTANTS-01 | Cover rejection of NaN and Infinity constants in synthetic archive member JSONL | DATA-02-ARCHIVE-OVERLAP-MAP-01 | FIXTURE_VERIFIED (worker and root each pass 19 tests; exact malformed-record message asserted for all three constants; production parser unchanged; E134 and completion checkpoint; synthetic-only) / root + `launcher_review` | 0 failed tries |
| AUD-01-XFF-EXACT-PATH-CARDINALITY-01 | Cover zero and duplicate exact PSARC path matches in `inspect_members` | AUD-01-XFF-01, AUD-01-XFF-READER-IDENTITY-FIXTURE-01 | FIXTURE_VERIFIED (worker and root each pass 18 tests; exact errors, zero member reads, reader closure; production parser unchanged; E135 and completion checkpoint; synthetic-only) / root + `slot23_raw_xrefs` | 0 failed tries |
| DOC-FOLDER-INDEX-SYMLINK-01 | Cover non-traversal of directory symlinks in generated folder indexes | DOC-FOLDER-INDEX-ADMIN-DIR-01 | FIXTURE_VERIFIED (worker and root each pass 7 tests; outside-root directory symlink and child omitted; production indexer unchanged; E136 and completion checkpoint; synthetic-only) / root + `review_host_proto` | 0 failed tries |
| DOC-FOLDER-INDEX-CHECK-READONLY-01 | Ensure stale-index check mode reports missing outputs without creating directories or files | None | FIXTURE_VERIFIED (worker and root each pass 8 tests; empty temporary root raises the exact stale-index error and leaves `docs/` and `local/` absent; indexer unchanged; see `docs/checkpoints/2026-10-21-root-doc-folder-index-check-readonly-complete.md`) / root+`slot23_raw_xrefs` | 0 failed implementation tries |
| RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01 | Cover single-segment containment for Image.mapped() across segment ends and gaps | RE-02-DYNLINK-01, RE-02-DYNLINK-SEGMENT-SPAN-01 | FIXTURE_VERIFIED (worker and root each pass 20 tests; contained range maps, crossing and gap ranges do not; parser unchanged; E141 and completion checkpoint; synthetic-only) / root+launcher_review | 0 failed tries |
| DOC-ASSETS-AUD-01-XFF-COVERAGE-COUNT-01 | Correct stale XFF synthetic test-count wording in docs/ASSETS.md | AUD-01-XFF-01, AUD-01-XFF-READER-IDENTITY-FIXTURE-01, AUD-01-XFF-EXACT-PATH-CARDINALITY-01 | FIXTURE_VERIFIED (corrected six to 18 synthetic test methods; count command returned 18; worker and root pass 18 tests; E142 and completion checkpoint; docs-only, no runtime claim) / root+slot23_raw_xrefs | 0 failed tries |
| DATA-02-ARCHIVE-MANIFEST-CRLF-01 | Cover CRLF-delimited records in the synthetic archive manifest parser fixture | DATA-02-ARCHIVE-OVERLAP-MAP-01 | FIXTURE_VERIFIED (worker and root each pass 20 tests; two CRLF records preserve exact paths and sizes; parser unchanged; E143 and completion checkpoint; synthetic-only) / root+review_host_proto | 0 failed tries |
| RE-02-DYNLINK-RELRO-IMAGE-FIXTURE-01 | Cover Image.mapped/read acceptance of a synthetic PT_SCE_RELRO segment | RE-02-DYNLINK-01, RE-02-DYNLINK-SEGMENT-SPAN-01 | FIXTURE_VERIFIED (worker and root each pass 21 tests; mapped/read accepts synthetic PT_SCE_RELRO bytes; parser unchanged; E144 and completion checkpoint; no live ELF/loader claim) / root+launcher_review | 0 failed tries |
| AUD-01-CLASSIFIER-RAW-INFLATE-OFFSET-00-FIXTURE-01 | Cover successful raw DEFLATE decompression at byte offset 0 | AUD-01 | FIXTURE_VERIFIED (worker and root each pass 21 tests; raw DEFLATE offset-zero branch succeeds for a synthetic 64-byte payload; E145 and completion checkpoint; no codec identity or gameplay claim) / root+slot23_raw_xrefs | 0 failed tries |
| RE-02-DYNLINK-MAPPED-ZERO-LENGTH-01 | Pin zero-length Image.mapped() behavior at a segment's one-past-end address | RE-02-DYNLINK-01, RE-02-DYNLINK-SEGMENT-SPAN-01, RE-02-DYNLINK-MAPPED-RANGE-FIXTURE-01 | FIXTURE_VERIFIED (worker and root each pass 21 tests; empty range at segment end maps; parser unchanged; E146 and completion checkpoint; synthetic-only) / root+launcher_review | 0 failed tries |
| DATA-02-PSARC-BLOCK-SIZE-RANGE-01 | Cover synthetic PSARC block-size lower and upper bounds | DATA-02, DATA-02-PSARC-PARTIAL-READ-01 | FIXTURE_VERIFIED (worker and root each pass 32 tests; exact error at zero and above 2^24; parser unchanged; E147 and completion checkpoint; synthetic-only) / root+slot23_raw_xrefs | 0 failed tries |
| AUD-PUBLISH-GIT-NUL-FILENAME-FIXTURE-01 | Pin NUL-delimited staged filename parsing when a path contains an embedded newline | AUD-PUBLISH-EDGE-01, AUD-PUBLISH-NESTED-01 | FIXTURE_VERIFIED (worker and root each pass 8 tests; one embedded newline remains in one NUL-delimited name; exact command/cwd asserted; audit unchanged; E148 and completion checkpoint) / root+review_host_proto | 0 failed tries |
| APP-01-ASSET-VALIDATOR-SHORT-MAGIC-FIXTURE-01 | Pin rejection of a synthetic PSARC header shorter than the four-byte magic | APP-01-ASSET-VALIDATOR-REPORT-01, APP-01-ASSET-VALIDATOR-EDGE-01 | ACTIVE (synthetic short-header fixture only; one not-PSARC problem, no missing archive) / root | 0 failed tries |
