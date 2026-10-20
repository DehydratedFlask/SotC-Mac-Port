# Task register and executable work packets

Statuses: `READY`, `ACTIVE`, `WAITING_FOR_DEPENDENCY`, `ARTIFACT_VERIFIED`, `STATIC_ANALYSIS_VERIFIED`,
`FIXTURE_VERIFIED`, `REAL_GAME_VERIFIED`, `DONE_DOC`, `NEEDS_PRINCIPAL`. No native game implementation exists.
Gates and test evidence take precedence over a status label. Integrator owns this file.

## Register — baseline 2026-10-19

| ID | Task | Depends on | Status / owner | Failed tries |
|---|---|---|---|---|
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
| AUD-PUBLISH-EDGE-01 | Reject `..` traversal through allowlisted dirs in the publish-path audit | G0 | FIXTURE_VERIFIED (defect fixed; 4 tests pass; see `docs/checkpoints/2026-10-20-root-edge-fixtures-complete.md`) / root | 0 |
| DATA-02-PSARC-EDGE-01 | Reject zero-entry PSARC with `PsarcError` instead of `IndexError` | DATA-02 | FIXTURE_VERIFIED (defect fixed; 15 tests pass; same checkpoint) / root | 0 |
| APP-01-ASSET-VALIDATOR-EDGE-01 | Validator reports a directory named like a required archive as not found | APP-01 | FIXTURE_VERIFIED (19 launcher tests pass; same checkpoint) / root | 0 |
| CPU-01 | Native recovery feasibility slice | RE-02 | ACTIVE (many leaf ctors FIXTURE_VERIFIED incl. bc-cb8d07c2: 0xdd41e0, 0x3bd70, 0xa3ade0, 0x5777a0; fail-fast startup-prefix and constructor-index metadata gates FIXTURE_VERIFIED; slot-10 static dossier and caller-local initializer dataflow verified; function setup zeroes its guards and skips the indirect call; computed target provenance is bounded but remains unresolved outside the static image; direct helper `0xe1ede0` characterization claimed; constructor reachability/helper contracts remain open; engine chain not started; leaf 95 `0xd71f60` NEEDS_PRINCIPAL after 3 failed tries; `0xce7300`, `0xce57b0`, `0xce0890`, and `0xcdc760` fixture/checker slices verified; no startup chain/G1) / principal+root | 0 |
| CPU-01-CTOR-PREFIX-01 | Stop native constructor runner at first unresolved original slot | CPU-01, RE-02 | FIXTURE_VERIFIED (runner stops after 10 callbacks at unresolved slot 10 / RVA `0xf09e60`; checker verifies 103 registered RVA/index pairs against pinned ELF/table; no allocator/atexit implementation or runtime startup proof) / root+/root/cpu_leaf_survey | 0 |
| CPU-01-CTOR10-ANALYSIS-01 | Characterize first unresolved constructor at RVA `0xf09e60` | CPU-01, RE-02 | STATIC_ANALYSIS_VERIFIED (hash-bound REA/Ghidra dossier plus independent byte/reference checker; seven direct calls, partial callee coverage, one unresolved computed call at `0xf09efb`, local helper contracts unknown; no runtime proof) / root+/root/cpu_slot10_analysis | 1 (corrected verifier snapshot-key assumption) |
| CPU-01-CTOR10-INIT-HELPER-01 | Characterize `0xe1eda0` writes to slot-10 caller guard fields | CPU-01-CTOR10-ANALYSIS-01 | STATIC_ANALYSIS_VERIFIED (62-byte helper range/hash, argument writes and caller guard path independently checked; guard byte remains zero and skips `0xf09efb` on the observed path; no constructor reachability/runtime proof; 1 failed shell wrapper corrected, 0 failed analysis hypotheses) / root+/root/cpu_ctor10_init_helper | 0 |
| CPU-01-CTOR10-INDIRECT-01 | Bound the dataflow and possible target for slot-10 indirect call `0xf09efb` | CPU-01-CTOR10-ANALYSIS-01, CPU-01-CTOR10-INIT-HELPER-01 | STATIC_ANALYSIS_VERIFIED (byte-checked branch and caller-local dataflow; target operand lies outside pinned PT_LOAD ranges with no relocation identifying it; unresolved edge retained; no constructor/runtime/ABI claim) / root+/root/srv01_page_map_19660+root | 0 |
| CPU-01-CTOR10-E1EDE0-01 | Characterize the direct slot-10 helper at `0xe1ede0` | CPU-01-CTOR10-ANALYSIS-01 | STATIC_ANALYSIS_VERIFIED (hash-bound REA/Ghidra dossier and root rerun of PT_LOAD/body/caller/reference verifier; two computed calls and data targets unresolved) / root+/root/srv01_page_map_19660 | 0 |
| CPU-01-CTOR10-CALLER-STATE-01 | Determine byte-backed stack-field state passed to helper `0xe1ede0` by constructor slot 10 | CPU-01-CTOR10-E1EDE0-01, CPU-01-CTOR10-ANALYSIS-01 | STATIC_ANALYSIS_VERIFIED (root reran hash-bound caller verifier; four callee-gate fields are zero on examined path; caller guard skips unresolved call `0xf09efb`; no broader reachability or runtime claim) / root+`/root/ctor10_caller_state` | 0 |
| CPU-01-CTOR-METADATA-01 | Correct original indices in leaf fixtures `0xd32720` and `0xd2ffe0` | CPU-01-CTOR-PREFIX-01 | FIXTURE_VERIFIED (six metadata fields now match original indices 119/120; both tests pass; both ELF checkers and the 103-pair table checker pass; function behavior unchanged) / root+/root/cpu_ctor_metadata | 0 |
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
| AUD-01-STS-IDENTITY-01 | Identify paired test_adpcm STS members | AUD-01, DATA-01 | FIXTURE_VERIFIED (290 exact paths in each cachedFiles layer; all locators checked and decoded hashes differ; no format or runtime claim) / root+/root/aud01_sts_cp3_identity | 0 |
| DATA-02-BRIDGE-SXD-01 | Compare the two cachedFiles `sound/shadow/bridge.sxd` members | DATA-01, DATA-02 | ARTIFACT_VERIFIED (private script validates exact archive/member identities and hashes and repeats byte-offset comparison; 619 aligned bytes, 152 equal/467 different, 19-byte patch suffix; no schema/override meaning; one corrected report-construction defect) / root+/root/data02_bridge_sxd | 1 |
| AUD-02 | Recover used voice/rack contracts | RE-02, AUD-01 | ACTIVE (partial: eight root-verified static import-to-call paths; three bounded probes remain open; RackDestroy function result is now root-replayed; VoiceHandle offline packet independently root-replayed; one Ghidra startup timeout remains; command/parameter semantics, runtime binding, reachability, audio behavior, and use remain unclassified) / root+workers | 0 |
| AUD-02-NGS2-SYSCREATE-01 | Byte-check one callsite/dataflow path for `sceNgs2SystemCreateWithAllocator` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 26/26 dossier checksums and byte verifier PASS; direct caller-to-thunk path; runtime binding and reaching definition unresolved) / root | 0 |
| AUD-02-NGS2-RACKCREATE-01 | Byte-check one callsite/dataflow path for `sceNgs2RackCreate` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 45/45 dossier checksums and byte verifier PASS; direct caller-to-thunk path; unresolved indirect call/runtime binding remain) / root | 1 (worker checker correction; 0 failed analysis hypotheses) |
| AUD-02-NGS2-VOICE-CONTROL-01 | Byte-check one callsite/dataflow path for `sceNgs2VoiceControl` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 17/17 dossier checksums and byte verifier PASS; direct caller-to-thunk path; runtime binding and Ghidra adjacent-boundary edge remain unresolved) / root | 2 (MCP startup timeout; CLI provider not configured; 0 failed analysis hypotheses) |
| AUD-02-NGS2-SYSRENDER-01 | Byte-check one callsite/dataflow path for `sceNgs2SystemRender` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 72/72 dossier checksums and byte verifier PASS; slot `0x1601018` → thunk `0xf91450` → direct call `0x95383c` in `FUN_009536d0`; partial callee coverage, ABI/runtime unresolved) / root | 0 |
| AUD-02-NGS2-VOICE-GETSTATE-01 | Byte-check one callsite/dataflow path for `sceNgs2VoiceGetState` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 167/167 checksums, byte verifier and REA caller audit PASS; slot `0x1601028` → thunk `0xf91470` → direct call `0x94a27d` in `FUN_0094a200`; second edge `0x94a440` unverified; Ghidra thunk-boundary discrepancy retained) / root | 0 |
| AUD-02-NGS2-SYSDESTROY-01 | Byte-check one callsite/dataflow path for `sceNgs2SystemDestroy` | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 68/68 checksums and verifier PASS; slot `0x1601010` → thunk `0xf91440` → direct call `0x9489d2` in `FUN_009485a0`; secondary caller `0x94a055` untraced; Ghidra adjacent-boundary/indirection edges retained) / root | 0 |
| AUD-02-NGS2-RACKCREATE-ALLOC-01 | Byte-check one callsite/dataflow path for `sceNgs2RackCreateWithAllocator` (symbol 236 / NID `U546k6orxQo` / slot `0x1600fe8`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay passed 93/93 dossier checksums and 13 instruction-byte records; slot `0x1600fe8` → thunk `0xf913f0` → direct call `0x9493f8` in `FUN_009485a0`; register/stack values captured without ABI labels) / root+worker | 0 failed targeted analysis/provider tries (one checker expectation correction) |
| AUD-02-NGS2-RACKQUERY-BUFFERSIZE-01 | Byte-check one callsite/dataflow path for `sceNgs2RackQueryBufferSize` (symbol 239 / NID `0eFLVCfWVds` / slot `0x1601000`) | RE-02, AUD-01 | ACTIVE/PARTIAL (root replay verified the thunk xrefs query and 16-file capture manifest; 15 returned addresses match 15 raw `E8 rel32` byte candidates, and independent ELF/PT_LOAD decoding confirms each displacement targets `0xf91420`; xrefs omit reference kind and raw bytes do not establish instruction boundaries or caller/function attribution; no caller/callsite accepted; no additional provider query released) / root+`/root/aud02_rackquery_buffersize` | 0 failed analysis hypotheses; 0 failed provider operations (1 root verifier assertion correction) |
| AUD-02-NGS2-RACK-VOICEHANDLE-01 | Byte-check one callsite/dataflow path for `sceNgs2RackGetVoiceHandle` (symbol 238 / NID `MwmHz8pAdAo` / slot `0x1600ff8`) | RE-02, AUD-01 | ACTIVE/PARTIAL (root offline replay verified 77/77 manifest entries and independently reproduced parser/byte scans; thunk candidate `0xf91410`, 104 raw CALL patterns; objdump windows have section-range warnings and do not establish instruction boundaries or caller/function attribution; one prior Ghidra 12.1.4 xrefs timeout at 330s remains the only provider failure; no retry released) / root+`/root/aud02_voiceflags_offline` | 1 provider failure; 0 hypothesis failures |
| AUD-02-NGS2-RACKDESTROY-01 | Byte-check one callsite/dataflow path for `sceNgs2RackDestroy` (symbol 237 / NID `lCqD7oycmIM` / slot `0x1600ff0`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root independently replayed function Evidence ID `ev_2b6853d79e403ff02e771eeab42c0c247cf4848362fe344cb5eb0341d02df601` and 66/66 dossier checksums; slot `0x1600ff0` → thunk `0xf91400` → direct call `0x94a01f` in `FUN_00949fe0`; no reachability/runtime/ABI/NGS2 behavior claim) / root+`/root/aud02_rackquery_buffersize` | 0 failed analysis hypotheses; 0 failed provider operations |
| AUD-02-NGS2-VOICE-GETSTATEFLAGS-01 | Byte-check one callsite/dataflow path for `sceNgs2VoiceGetStateFlags` (symbol 245 / NID `rEh728kXk3w` / slot `0x1601030`) | RE-02, AUD-01 | STATIC_PATH_BYTE_VERIFIED (root replay: 56/56 worker dossier checksums and root verifier PASS; bytes establish register setup at `0x9534c3`/`0x9534c7`, direct call `0x9534cb` in `FUN_00953470` → thunk `0xf91480` → RELRO/GOT slot `0x1601030` → symbol 245 / NID `rEh728kXk3w`; no ABI/parameter/runtime/NGS2/audio claim) / root+`/root/aud02_voiceflags_offline` | 2 root-verifier corrections; 0 failed analysis hypotheses; 0 failed provider operations |
| AUD-03 | Decoder/mixer/effect fixtures | AUD-02 | WAITING_FOR_DEPENDENCY | 0 |
| AUD-04 | Actual game audio integration | AUD-03, CPU-04 | WAITING_FOR_DEPENDENCY | 0 |
| SAV-01 | Recover game serializer/checksums | RE-02 | WAITING_FOR_DEPENDENCY | 0 |
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
