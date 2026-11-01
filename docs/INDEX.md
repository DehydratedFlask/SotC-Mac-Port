# Project documentation index

Workspace: `/Volumes/Extra/SoC Mac Native`. Start with [HANDOFF](../HANDOFF.md),
[AGENTS](../AGENTS.md), and [ROADMAP](../ROADMAP.md). The live files are implementation truth.

## Authoritative navigation

| Document | Purpose / owner |
|---|---|
| [TASKS](TASKS.md) | Dependency-ordered task register and detailed work packets; integrator |
| [EVIDENCE](EVIDENCE.md) | Verified artifact/source/tool facts and unresolved questions; integrator |
| [AUD-02 RackQuery jump-entry review (2026-10-29)](checkpoints/2026-10-29-aud02-rackquery-jump-entry-94eb4c.md) | E968/E970; slot 11 resolves to `0x94eb4c`; selected path passes a caller-stack pointer through RBX to `0x94ebc2` (static only) |
| [AUD-02 RackQuery post-call stack-cell claim (2026-10-29)](checkpoints/2026-10-29-aud02-rackquery-postcall-stack-94ebc2.md) | E972 claim; byte-check caller-side post-call references to the stack region passed at `0x94ebc2` on slot-11 route only |
| [AUD-02 RackQuery post-call caller-slice result (2026-10-29)](checkpoints/2026-10-29-aud02-rackquery-postcall-stack-result-94ebc2.md) | E973; 10 caller instructions / 53 bytes and direct edges verified for the selected static slot-11 candidate; no runtime/API/audio claim |
| [AUD-02 VoiceHandle one-callsite status reconciliation (2026-10-29)](checkpoints/2026-10-29-aud02-ngs2-voicehandle-static-path-status-e978.md) | E977/E978; root replay confirms one `0x9496a8` call path to the VoiceHandle import thunk; runtime, ABI, API, reachability, and audio remain unknown |
| [Task-register duplicate row cleanup (2026-10-29)](checkpoints/2026-10-29-doc-task-register-slot309-duplicate-row-e976.md) | E976; removed one duplicate slot-309 task row while retaining the E894-enriched row and E911 stop condition |
| [Slot-309 app-main receiver route review (2026-10-29)](checkpoints/2026-10-29-slot309-receiver-provenance-e1015.md) | E1015; the saved `0x33390` helper shows no receiver access, but later-call effects, current vptr, and ordering relative to slot 309 remain unresolved |
| [Slot-309 receiver-review publication closeout (2026-10-29)](checkpoints/2026-10-29-slot309-receiver-provenance-publication-e1016.md) | E1016; E1015 was pushed and fetch-verified with repository checks green; no product gate advanced |
| [Slot-309 save-condition consumer edge (2026-10-29)](checkpoints/2026-10-29-slot309-save-condition-consumer-e1018.md) | E1018; two static save-path callsites pass the slot-309 object into a complex helper; callback and lifetime remain unresolved |
| [Slot-309 save-consumer callback recheck and boot triage (2026-10-29)](checkpoints/2026-10-29-slot309-save-condition-callback-recheck-e1019.md) | E1019; the constructor's zero callback descriptor skips `0xa4de23` for the initial state; later writes/order and ownership remain open, and no native game-start path exists |
| [Startup follow-up: slot-309 callers, ownership, and slot 312 (2026-10-29)](checkpoints/2026-10-29-startup-followup-slot309-slot312-e1020.md) | E1020-E1022; no caller/order edge to slot 309, alias mutation remains unresolved, and slot 312 has no bounded boot lift |
| [Startup save-thread callback route (2026-10-29)](checkpoints/2026-10-29-startup-save-thread-route-e1030.md) | E1030; static `scePthreadCreate` setup for `Save Task Thread` reaches callback `0xa4d330`; owner/order to slot 309 remains unresolved |
| [App-main Init body boundary (2026-10-30)](checkpoints/2026-10-30-re02-499f80-body-result-e1031.md) | E1031; `0x499f80` body and caller are characterized; allocator/lifecycle contracts stop native liftability, with no product gate change |
| [App-main Init callee-contract boundary (2026-10-30)](checkpoints/2026-10-30-re02-499f80-callee-contract-boundary-e1032.md) | E1032; four allocator calls match existing native wrappers; mutex type/handle/lifecycle remains the exact lift boundary |
| [Init hook inventory (2026-10-30)](checkpoints/2026-10-30-init-hook-inventory-e1035.md) | E1035; 36 unmodelled Init callees and slot 309 ranked for leverage; static analysis only |
| [Init shared lookup specification (2026-10-30)](checkpoints/2026-10-30-init-1e4c0-luna-spec-e1037.md) | E1037; pins the shared `0x1e4c0` config lookup boundary and separates the sndx args-vector shape |
| [Init `0x21620` lookup specification (2026-10-30)](checkpoints/2026-10-30-init-21620-luna-spec-e1042.md) | E1042c; implementation remains blocked by descriptor, fallback-tree, and lock-lifetime stop rules |
| [Init leverage ranking and driver join (2026-10-30)](checkpoints/2026-10-30-init-next-leverage-sonnet-e1042.md) | E1042d/E1043; ranks follow-up work; driver join is fixture verified with fail-closed unknown paths |
| [Init driver join 1b/1c (2026-10-31)](checkpoints/2026-10-31-init-driver-join-e1043.md) | E1043; alt→C, shared `r12b`, unknown-predicate side-effect guards; fixture verification only |
| [CritSec sidecar contention repair (2026-10-31)](checkpoints/2026-10-31-critsec-contention-e1044.md) | E1044; stable mutex storage, per-operation leases, bounded owner/waiter fixture, 20-repeat pass |
| [Trail source-scope refresh (2026-10-31)](checkpoints/2026-10-31-trail-scope-index-e1045.md) | E1045; graph scopes include scripts/tools/reference/native/launcher/tests; rebuild and native query passed |
| [`0x499f80` failure-cleanup audit (2026-10-31)](checkpoints/2026-10-31-499f80-failure-cleanup-audit-e1046.md) | E1046; retail body has no error branches; host-model partial allocations need an explicit ownership policy |
| [ConCmd comparator target audit (2026-10-31)](checkpoints/2026-10-31-concmd-comparator-audit-e1047.md) | E1047; four pinned calls resolve to a same-ELF import entry labeled `strcasecmp`; case-folding semantics remain an API-name inference |
| [lookup_1fc80 casefold model correction claim (2026-10-31)](checkpoints/2026-10-31-lookup-casefold-model-e1049-claim.md) | E1049; pinned call target/import label hash-checked; model correction task claimed with case-fold semantics kept as inference |
| [lookup_1fc80 casefold model correction (2026-10-31)](checkpoints/2026-10-31-lookup-casefold-model-e1049.md) | E1049; focused default-comparator fixture and parent byte checkers pass; non-ASCII locale/runtime behavior not established |
| [CritSec slot-414 teardown coverage audit claim (2026-10-31)](checkpoints/2026-10-31-critsec-slot414-teardown-audit-e1050-claim.md) | E1050; read-only audit claimed to determine whether host shutdown consumes atexit log entries or invokes slot-414 destruction |
| [CritSec slot-414 teardown coverage audit (2026-10-31)](checkpoints/2026-10-31-critsec-slot414-teardown-audit-e1050.md) | E1050; no current host drain or slot-414 destruction path; lifecycle owner/order remain future work |
| [Validation checkpoint pointer sync (2026-10-31)](checkpoints/2026-10-31-validation-checkpoint-sync-e1051.md) | E1051; validation summary identifies E1051 as the current docs checkpoint, E1050 as latest substantive source audit, and E1005 as latest verified launcher bundle; G0–G6 unchanged |
| [Slot-309 save-dispatch jump-table route claim (2026-10-31)](checkpoints/2026-10-31-slot309-save-dispatch-jumptables-e1052-claim.md) | E1052 claim; principal reassessment resumes the bounded PIC-relative jump-table route from E1020; root verification pending |
| [Slot-309 save-dispatch Ghidra timeout (2026-10-31)](checkpoints/2026-10-31-slot309-save-dispatch-jumptables-e1052-attempt1.md) | E1052 attempt 1; Ghidra MCP times out at 330s/2G without a function dossier; raw ELF/Rizin checks remain separate |
| [Slot-309 save-dispatch dispatcher A (2026-10-31)](checkpoints/2026-10-31-slot309-save-dispatch-jumptables-e1052-dispatch-a.md) | E1052; Ghidra verifies the 17-case table and case-2 direct call to `0xa4a600`; no runtime/order relation to slot 309 |
| [Slot-309 save-dispatch bounded result (2026-10-31)](checkpoints/2026-10-31-slot309-save-dispatch-jumptables-e1052-result.md) | E1052 PARTIAL; raw tables are 17/19 entries, Ghidra's second typed map is 15/19, selected direct calls are static, and no edge/order to slot 309 was found |
| [Task availability revalidation (2026-10-31)](checkpoints/2026-10-31-orchestration-ready-revalidation-e1053.md) | E1053 ARTIFACT_VERIFIED; no READY task or safe boot/render work was found; existing evidence and external-state stops remain |
| [Validation checkpoint pointer synchronization (2026-10-31)](checkpoints/2026-10-31-validation-checkpoint-sync-e1054.md) | E1054 ARTIFACT_VERIFIED; validation summary points to the current checkpoint and separates host-source, startup-binary, task-availability, and artifact evidence; gates unchanged |
| [ConCmd casefold model correction (2026-10-31)](checkpoints/2026-10-31-concmd-casefold-model-e1048.md) | E1048; focused model fixture/checker and source graph checks pass; case-fold behavior remains inferred from the NID-derived import name |
| [ConCmd casefold model correction claim (2026-10-31)](checkpoints/2026-10-31-concmd-casefold-model-e1048-claim.md) | Original task claim and scope |
| [Startup follow-up publication closeout (2026-10-29)](checkpoints/2026-10-29-startup-followup-publication-e1023.md) | E1023; owner-authored result commit was pushed and fetch-verified; repository checks passed without advancing a game gate |
| [AUD-02 RackQuery call-clobber review (2026-10-29)](checkpoints/2026-10-29-aud02-rackquery-call-clobber-954f183.md) | E966/E967; callee returns RDX in `0..14`; RackQuery argument at `0x94ebc2` remains UNKNOWN |
| [SAV-01 nested result constructor `0x8db10` (2026-10-29)](checkpoints/2026-10-29-sav01-result-constructor-8db10.md) | E952/E953; byte-verified payload pointer/length flow and published closeout; consumer semantics were characterized in follow-up E955 |
| [SAV-01 payload reader `0x8eba0` (2026-10-29)](checkpoints/2026-10-29-sav01-payload-read-primitive-8eba0.md) | E955/E956; published static cursor and caller analysis; ordinary reads are length-gated, short reads wrap without reporting failure |
| [SAV-01 payload reader caller `0x543462` (2026-10-29)](checkpoints/2026-10-29-sav01-payload-read-caller-bound-543462.md) | E958/E959; byte-verified caller checks tag `0xd` before the qword read and checks its value only afterward; no pre-read caller length guard |
| [SAV-01 remaining payload-reader callers (2026-10-29)](checkpoints/2026-10-29-sav01-payload-read-remaining-callers.md) | E960/E961/E962; three remaining known `0x8eba0` callsites lack a caller-side eight-byte bounds proof; result published and fetch-verified |
| [POINTERS](POINTERS.md) | Address classes, tagged 31-bit fields, evidence labels (CPU-02 partial); integrator |
| [ASSETS](ASSETS.md) | PSARC archive findings and audio-asset leads (DATA-02/AUD-01 partial); integrator |
| [GPU01_PREP](GPU01_PREP.md) | Offline `.csdr`/`.cmat` member-name inventory from manifests (GPU-01 prep); GPU worker |
| [METAL_FRAME_PATH](METAL_FRAME_PATH.md) | Renderer boundary, bounded importer/Metal work, and synthetic harness evidence; G3 PARTIAL, no game frame |
| [PSARC MD5 reconciliation](checkpoints/2026-10-24-psarc-md5-reconciliation.md) | Correct path-key pairing and superseded audio/asset claims; E621 |
| [Startup slots 302/303/305](checkpoints/2026-10-24-startup-slots-302-303-305.md) | 317/573 registered, first gap 306; E623; fixture/static verification only |
| [Resume integration and attribution review](checkpoints/2026-10-26-resume-integration-attribution-review.md) | Pre-publication 431/431 CTest, 516/0/0 lift sweep, and preserved-tree rewrite review; publication recorded below |
| [Claude attribution cleanup published](checkpoints/2026-10-26-attribution-cleanup-published.md) | Both stale GitHub refs rewritten atomically with exact leases; 2,341 reachable remote commits scanned, zero Claude/Anthropic attribution |
| [Startup leaf rows 315-560](checkpoints/2026-10-26-startup-leaf-rows-315-560.md) | 11 existing leaves wired; 397/573 constructors, first gap 309; full CTest and ELF checker sweep passed; G1 PARTIAL |
| [Startup slot 309 characterization claim](checkpoints/2026-10-26-slot309-characterization-claim.md) | Read-only task for first gap `0xa505e0`; analyzer confirms the 1,891 B extent, while 3 indirect calls and REA correlation remain open |
| [Startup slot 309 preliminary dossier](checkpoints/2026-10-26-slot309-preliminary-dossier.md) | Static analyzer and root RELA replay resolve the three root indirect slots; three Ghidra startup timeouts leave helper semantics and G1 partial |
| [Startup slot 309 saved-project Ghidra coverage](checkpoints/2026-10-27-slot309-ghidra-coverage.md) | E748; saved-project coverage maps the 1,891 B root body and its three one-byte RET targets; custom helper dispatch remains unresolved |
| [Startup slot 309 `0x26ea0` virtual calls](checkpoints/2026-10-27-slot309-concmd-vcall-resolution.md) | E750; both indirect slots resolve for all four slot-309 callers; custom helper edges remain open |
| [Startup slot 309 custom helper dispatch](checkpoints/2026-10-27-slot309-helper-dispatch-a4f4-a502.md) | E751; selected table-dispatched calls across all three root/nested helper paths resolve for observed slot-309 stack objects; remaining indirect edges and lifetime contracts stay open |
| [Attribution scan after cf1d019a](checkpoints/2026-10-27-post-cf1d019a-attribution-scan.md) | E752; all 9 remote heads and 2,374 reachable commits scan clear of Claude/Anthropic identities and credit lines |
| [Attribution scan after 5c5c2758](checkpoints/2026-10-27-post-5c5c2758-attribution-scan.md) | E753; all 9 remote heads and 2,375 reachable commits scan clear of Claude/Anthropic identities and credit lines |
| [Startup slot 309 helper targets and registration boundary](checkpoints/2026-10-27-slot309-helper-contracts-post7068-scan.md) | E754; selected helper indirect targets resolved for observed objects; registration has a static list/tree shape, while ownership and teardown remain open |
| [Attribution scan after 7068f06a](checkpoints/2026-10-27-slot309-helper-contracts-post7068-scan.md) | E755; all 9 remote heads and 2,376 reachable commits scan clear of Claude/Anthropic identities and credit lines |
| [Slot 309 lifecycle callbacks and attribution after f914bcb2](checkpoints/2026-10-27-slot309-lifecycle-callbacks-post-f914.md) | E756/E757; public refs through `f914bcb2` scan clean; manager ownership, exit callbacks, `0x27470`, and one callback edge are bounded while branch ordering and temporary lifetime remain open |
| [Attribution scan after 329046c7](checkpoints/2026-10-27-post-329046c7-attribution-scan.md) | E758; all 9 fetched remote heads and 2,378 reachable commits scan clear of Claude/Anthropic identities and credit lines |
| [Slot 309 temporary lifetime, writer ordering, and attribution after d71c3ccf](checkpoints/2026-10-27-slot309-rbp38-writer-order-post-d71.md) | E759/E760; 2,379 public commits scan clear; local temporary cleanup and writer invocation order remain unresolved |
| [Slot 309 boundary recheck and local/public attribution audit](checkpoints/2026-10-27-slot309-stash-attribution-audit.md) | E761/E762; saved incoming xref unavailable; 2,380 public commits scan clear; 70 Claude matches remain only in private stash refs pending scope decision |
| [Attribution scan after 1a44ce55](checkpoints/2026-10-27-post-1a44ce55-attribution-scan.md) | E763; all 9 fetched heads and 2,381 reachable public commits scan clear of Claude/Anthropic attribution |
| [Attribution scan after 60b5947c](checkpoints/2026-10-27-post-60b5947c-attribution-scan.md) | E764; all 9 fetched heads and 2,382 reachable public commits scan clear of Claude/Anthropic attribution |
| [Slot 309 table-consumer census and post-32f scan](checkpoints/2026-10-27-slot309-table-consumer-post-32f-scan.md) | E765/E766; two table-base LEAs mapped, but `0x531d60` consumer/order and slot-309 registration branch remain unresolved; 2,383 public commits scan clean |
| [Attribution scan after 4b21a234](checkpoints/2026-10-27-post-4b21a234-attribution-scan.md) | E767; all 9 fetched heads and 2,384 reachable public commits scan clear of Claude/Anthropic attribution |
| [Attribution scan after 9d87ef5b](checkpoints/2026-10-27-post-9d87ef5b-attribution-scan.md) | E768; all 9 fetched heads and 2,385 reachable public commits scan clear of Claude/Anthropic attribution |
| [Attribution scope closure after 7e67df61](checkpoints/2026-10-27-post-7e67df61-attribution-scope-closure.md) | E769; all 9 fetched heads and 2,386 reachable public commits scan clear; owner explicitly leaves stash-only refs unchanged |
| [Shutdown TERM fixture, public attribution, and slot-309 report review](checkpoints/2026-10-27-app01-shutdown-term-escalation.md) | E773-E783; 2,393 public commits scan Claude-free; final owner commit passed attribution checks; slot 309 ownership/order remain unresolved |
| [Attribution scan after 109c9f9f](checkpoints/2026-10-27-post-109c9f9-attribution-scan.md) | E784; all 9 fetched GitHub heads and 2,395 reachable commits scan clear of Claude/Anthropic identities and credit lines |
| [Attribution scan and publication closeout after 6fcef564](checkpoints/2026-10-27-post-6fcef564-attribution-scan.md) | E785/E786; all 9 fetched heads and 2,396 reachable commits scan clear through `6fcef564`; `8aa90e39` separately range-audited and fetch-verified on main |
| [E789 publication closeout and post-push scan](checkpoints/2026-10-27-e789-publication-closeout.md) | E790; `9ff7cf7b` range/path/identity audited and pushed; fresh scan includes it in 2,401 commits with zero Claude/Anthropic matches; 70 stash-only matches unchanged by owner choice |
| [SXDF/SXDS saved-evidence census](checkpoints/2026-10-27-aud01-sxdf-reader-census.md) | E791; 3,453 hash-bound text exports; `.sxd` is only a platform-tag row, no reader candidate; E792 post-push scan covers 2,403 public commits with zero Claude/Anthropic matches |
| [E791 publication closeout and post-push scan](checkpoints/2026-10-27-e791-publication-closeout.md) | E792; `e7921208` range/path/identity audited and pushed; fresh 2,403-commit scan is Claude-free; 70 stash-only matches unchanged by owner choice |
| [All-ref Claude attribution census](checkpoints/2026-10-27-local-ref-attribution-census.md) | E793; 25 non-stash refs and 2,404 unique commits scan clear; 70 matches remain in the user-excluded stash |
| [All-ref attribution census publication closeout](checkpoints/2026-10-27-local-ref-attribution-push-closeout.md) | E794; owner commit `3797ef7e` pushed and verified; 2,405 public commits and 2,405 non-stash local commits scan clear |
| [E794 final push closeout](checkpoints/2026-10-27-e794-final-push-closeout.md) | E795; E794 commit fetched cleanly; 2,406 public commits and 2,406 non-stash local commits scan clear |
| [E795 final publication closeout](checkpoints/2026-10-27-e795-final-publication-closeout.md) | E796; fresh scans through E795 cover 2,407 public and 2,407 non-stash local commits, with zero Claude/Anthropic matches |
| [All-archive sound-path census claim](checkpoints/2026-10-27-aud01-seven-archive-sound-path-claim.md) | E797; claims metadata-only path census over all seven corrected manifests; 170,451 rows; no payload reads |
| [All-archive sound-path census completion](checkpoints/2026-10-27-aud01-seven-archive-sound-path-complete.md) | E798; 1,221 metadata candidates; 86 header rows reconciled, 1,135 outside subset; no payload reads |
| [E798 publication and attribution closeout](checkpoints/2026-10-27-e798-publication-closeout.md) | E799; verifies E798 on GitHub, 2,410 fetched commits scanned with zero Claude/Anthropic matches, and no newly eligible native task |
| [E799 publication and all-ref attribution closeout](checkpoints/2026-10-27-e799-publication-closeout.md) | E800; verifies E799 on GitHub, 2,411 fetched commits scanned with zero Claude/Anthropic matches, and current task blockers |
| [E800 publication and current-ref closeout](checkpoints/2026-10-27-e800-publication-closeout.md) | E801; verifies E800 on GitHub, 2,412 fetched commits scanned with zero Claude/Anthropic matches, and current task blockers |
| [E804-E808 publication and attribution closeout](checkpoints/2026-10-27-e804-publication-closeout.md) | E809; verifies E808 on GitHub, 2,420 advertised public commits and local non-stash commits scanned with zero Claude/Anthropic matches; 70 private stash matches unchanged |
| [Music/BGM path census and attribution checkpoint](checkpoints/2026-10-27-aud01-music-bgm-census.md) | E789; four corrected-manifest music/BGM path-name candidates, no content/codec/routing claim; subsequent public-ref scan at `9ff7cf7b` covers 2,401 commits with zero Claude/Anthropic matches |
| [Next-task triage after attribution closeout](checkpoints/2026-10-27-next-task-triage.md) | E787/E788; initial triage found no ready CPU/DATA/AUD/SAV/SRV child packet; E789 adds an evidence-bounded AUD-01 path-name task and updates the fetched-ref scan to 2,400 commits |
| [Slot 309 saved-project Ghidra census and post-83f scan](checkpoints/2026-10-27-slot309-saved-project-xrefs-post-83f.md) | E770/E771; bounded saved-project xrefs add a table-data reference but no code caller/order link; 2,387 public commits remain Claude-free |
| [Startup slot 312 characterization claim](checkpoints/2026-10-26-slot312-characterization-claim.md) | Parallel read-only analyzer task to identify the pinned RVA and map preliminary control flow; no native model claimed |
| [Startup slot 312 preliminary dossier](checkpoints/2026-10-26-slot312-preliminary-dossier.md) | Root replay matches pinned table/analyzer output; two indirect calls remain unresolved, G1 partial |
| [Startup slot 314 characterization claim](checkpoints/2026-10-26-slot314-characterization-claim.md) | Read-only analyzer task for pinned constructor row 314 / `0xa41810`; no semantics or liftability claim |
| [Startup slot 314 preliminary dossier](checkpoints/2026-10-27-slot314-preliminary-dossier.md) | Root analyzer replay matches worker; 8 indirect calls remain unresolved, G1 partial |
| [Startup slot 316 characterization claim](checkpoints/2026-10-27-slot316-characterization-claim.md) | Read-only analyzer claim for pinned row 316 / `0xa33280`; registration remains behind earlier gaps |
| [Attribution scan after 30121388](checkpoints/2026-10-26-post-30121388-attribution-scan.md) | All 9 remote heads and 2,348 reachable commits scan clean for Claude/Anthropic identities and credit trailers |
| [Attribution scan after 97890a11](checkpoints/2026-10-26-post-97890a11-attribution-scan.md) | All 9 remote heads and 2,349 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Attribution scan after d0faf04e](checkpoints/2026-10-27-post-d0faf04-attribution-scan.md) | All 9 remote heads and 2,350 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Attribution scan after 209ff474](checkpoints/2026-10-27-post-209ff474-attribution-scan.md) | All 9 remote heads and 2,351 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Startup slot 316 preliminary dossier](checkpoints/2026-10-27-slot316-preliminary-dossier.md) | Root replay matches analyzer-only census for row 316; 5 indirect calls unresolved, G1 PARTIAL |
| [Attribution scan after d0b454b6](checkpoints/2026-10-27-post-d0b454b6-attribution-scan.md) | All 9 remote heads and 2,352 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Atomic save temp isolation closeout](checkpoints/2026-10-27-sav01-atomic-temp-isolation-closeout.md) | E726 closes the fixture-only atomic temp ownership task; the parent real-save gate remains open |
| [Attribution scan after e6c8d33e](checkpoints/2026-10-27-post-e6c8d33e-attribution-scan.md) | All 9 remote heads and 2,353 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [RDX cached-CFG closeout](checkpoints/2026-10-27-aud02-rdx-cfg-closeout.md) | E728 closes bounded offline argument-origin analysis at static/fixture scope; parent AUD-02 remains partial |
| [RackQuery poison-edge review](checkpoints/2026-10-29-aud02-ngs2-rackquery-poison-edge.md) | E964 verifies the stack-check failure edge; RDX at `0x94ebc2` remains unknown and parent AUD-02 remains partial |
| [Attribution scan after 9e61358b](checkpoints/2026-10-27-post-9e61358b-attribution-scan.md) | All 9 remote heads and 2,354 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Attribution scan after 53634a15](checkpoints/2026-10-27-post-53634a15-attribution-scan.md) | All 9 remote heads and 2,355 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Attribution scan after 4682a337](checkpoints/2026-10-27-post-4682a337-attribution-scan.md) | All 9 remote heads and 2,356 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Attribution scan after c8f23500](checkpoints/2026-10-27-post-c8f23500-attribution-scan.md) | All 9 remote heads and 2,357 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Attribution scan after cf94e831](checkpoints/2026-10-27-post-cf94e831-attribution-scan.md) | All 9 remote heads and 2,358 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Attribution scan after 5315489c](checkpoints/2026-10-27-post-5315489c-attribution-scan.md) | All 9 remote heads and 2,359 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Attribution scan after 690eda93](checkpoints/2026-10-27-post-690eda93-attribution-scan.md) | All 9 remote heads and 2,360 reachable commits scan clean for Claude/Anthropic identities and credit lines |
| [Startup slot 306 survey](checkpoints/2026-10-25-startup-slot-306-survey.md) | E676 hash-bound table/body/call-footprint characterization; semantics UNKNOWN, 11 indirect calls unresolved, G1 PARTIAL |
| [Slot 303 checker publication repair](checkpoints/2026-10-24-slot303-publication-repair.md) | Complete function reconstruction removed from public checker; E624 |
| [Integration PR #9 final closeout](checkpoints/2026-10-25-integration-pr9-final-closeout.md) | PR #9 merged at `a8bd805`; clean-main baseline and branch cleanup verified; E643 |
| [Metal fragment derivative fixture](checkpoints/2026-10-25-gpu02-metal-quad-derivative-fixture.md) | Synthetic Metal helper-lane derivative and implicit/explicit sampling check; one mip only; E644 |
| [Logo PS IMAGE_SAMPLE prerequisite](checkpoints/2026-10-25-gpu02-logo-ps-image-sample.md) | E645 synthetic multi-mip implicit/explicit Metal sampler prerequisite |
| [Logo PS IMAGE_SAMPLE completion](checkpoints/2026-10-25-gpu02-logo-ps-image-sample-complete.md) | E646 pinned fixed-template MSL replay and synthetic Metal/CPU-oracle result; retail and G3 limits remain |
| [Logo PS principal reassessment](escalations/GPU-02-GCN-LOGO-PS-IMAGE-SAMPLE-01.md) | Three unsuccessful test hypotheses, recorded root reassessment, and passed fourth focused replay |
| [Logo PS PR #12 merge closeout](checkpoints/2026-10-25-gpu02-logo-ps-merge-pr12.md) | PR #12 merged as `a07d451`; local and remote task branches removed; clean local `main` verified |
| [GFX7 default-pair inventory and Metal harness](checkpoints/2026-10-24-gcn-default-pair.md) | E625 inventory plus E627 bounded default VS/PS MSL; overall G3 is PARTIAL from later synthetic Metal harness work |
| [Raw default-material selector](checkpoints/2026-10-25-gpu02-raw-material-selector.md) | E682/E683 claim; E684 raw pins/spans; E686 verifier/tests/Metal replay; E687 project gates; E688 publication; E689 read-only triage found no next READY Metal or startup task |
| [CSDR reader corpus and review](checkpoints/2026-10-24-csdr-reader.md) | E626 structural-first attempt3, arbitrary `cc xx`, full corpus v5 and independent review |
| [Round 30 integration](checkpoints/2026-10-24-round30-integration.md) | Root replay: CTest 388/388, lift checkers 476/0/0, Python 228/228, shader harness and outstanding game gates; E628 |
| [GPU-03 L1 asset-derived frame claim](checkpoints/2026-10-24-gpu03-claim.md) | E629 pins raw `cube_1m`/`cone` geometry; native reader and direct-Metal draw are active; no game-frame/G3 PASS claim |
| [GPU-03 native raw mesh reader](checkpoints/2026-10-24-gpu03-cmsh-reader.md) | E630 bounded POS0/U16 parser, malformed synthetic fixtures, CMake integration and full native verification; real member bytes remain untested |
| [GPU-03 typed RenderOp API](checkpoints/2026-10-24-gpu03-renderops-api.md) | E631 host handles and fail-visible typed indexed draw op validation; E632 separately records the synthetic direct-Metal fixture; real asset frame remains NOT RUN |
| [GPU-03 synthetic direct-Metal frame](checkpoints/2026-10-24-gpu03-metal-synthetic-frame.md) | E632 pinned default-pair MSL, synthetic indexed/depth frame, CPU per-pixel validation and Metal API/GPU validation; no real mesh bytes or game-frame claim |
| [GPU-03 synthetic parser-to-Metal flow](checkpoints/2026-10-24-gpu03-synthetic-parser-metal-flow.md) | E633 synthetic `.cmsh` bytes parsed and rendered through the default MSL path; output exactly matches a direct draw |
| [GPU-02 logo WQM analysis](checkpoints/2026-10-24-gpu02-logo-wqm-analysis.md) | E634 byte-bound WQM/EXEC and texture-sample characterization; no logo MSL or G3 pass |
| [GPU-03 texture/sampler binding fixture](checkpoints/2026-10-24-gpu03-texture-sampler-complete.md) | E635 typed fragment resource binding and synthetic RGBA8 sample; no logo lowering, real asset binding, or WQM equivalence claim |
| [GPU-02 logo vertex fetch lowering claim](checkpoints/2026-10-24-gpu02-logo-vs-fetch-claim.md) | Original bounded task claim and preflight; completion evidence is E636 below |
| [GPU-02 logo vertex fetch lowering completion](checkpoints/2026-10-25-gpu02-logo-vs-fetch-complete.md) | E636 synthetic float2/float3/float4 MUBUF lowering and private Metal/CPU-oracle replay; no retail descriptor or G3 claim |
| [E28 zero-request checker parity](checkpoints/2026-10-25-srv01-f19e00-checker-parity.md) | Existing fixture's zero-request case is now included in the instruction-derived checker; E368; no game execution |
| [PR #5 allocator bootstrap rollback and fixed quantum](checkpoints/2026-10-25-pr5-allocator-rollback-fixed-quantum.md) | E638 records rollback, publish-last concurrency safety, fixed 1 MiB request quantum and verification; allocator hierarchy remains fixture-only and not wired into startup |
| [GPU-03 raw CSMH member import](checkpoints/2026-10-25-gpu03-raw-cmsh-import.md) | E639 exact MD5(path)-mapped raw-member import and private size/hash verification; parser and Metal draw were verified separately in E640 |
| [GPU-03 logo PNG texture sample](checkpoints/2026-10-25-gpu03-logo-png-texture-sample.md) | E656 exact PNG/MSL pins and synthetic slot-0 texel-center Metal replay; retail bindings and WQM equivalence remain unverified |
| [GPU-03 logo VS/PS Metal pipeline](checkpoints/2026-10-25-gpu03-logo-vs-ps-metal-pipeline.md) | E669 fixture-verified composed E636/E646 Metal pipeline; E670 records publication on main; retail/game-frame limits remain |
| [GPU-03 logo VS/PS on raw CMSH meshes](checkpoints/2026-10-25-gpu03-logo-vs-ps-raw-cmsh-frame.md) | E678 fixture review; E679 full gates and precommit audits passed; E681 published to main; G3 PARTIAL |
| [F590 shared-string release review](checkpoints/2026-10-25-f590-release-review-recovery.md) | E680 source/ASan/UBSan/byte-oracle review; E685 confirms the exact five-mutant inputs remain absent |
| [F590 replacement mutation acceptance](checkpoints/2026-10-27-f590-replacement-mutants-acceptance.md) | E731 root reassessment accepts the replacement set at fixture scope while preserving the missing-five and toolchain-specific limits |
| [GPU-03 logo PNG and raw mesh composite](checkpoints/2026-10-25-gpu03-logo-png-cmsh-composite.md) | E658 exact-hash raw cube/cone POS0/U16 plus pinned logo PNG/MSL through the direct-Metal asset-derived harness; retail mapping and G3 remain unverified |
| [GPU-03 CMSH TEX0 format code 6 evidence](checkpoints/2026-10-25-gpu03-cmsh-tex0-format-6.md) | E660 records the hash-pinned TEX0 spans; E662 records completed shipped-code tracing with no decoder found, so coordinate semantics remain UNKNOWN |
| [GPU-03 logo VS/PS Metal pipeline](checkpoints/2026-10-25-gpu03-logo-vs-ps-metal-pipeline.md) | E664 claim and E666 publication of the closed synthetic composition of exact-hash E636/E646 stages; implementation is active |
| [0x8C asset decoder deferral](checkpoints/2026-10-25-asset-block-decoder-deferral.md) | E665 records the owner decision to defer a decoder and continue with supported raw assets only |
| [GPU-03 asset-derived frame attempt 4](checkpoints/2026-10-25-gpu03-asset-frame-attempt4.md) | E649 index mutation changes interpolated `param0` and Metal PNG while CPU coverage/draw ownership stay stable; L1 fixture verified, G3 remains PARTIAL |
| [GPU-03 index-mutation escalation history](escalations/GPU-03-RENDEROPS-ASSET-FRAME-01.md) | E640 records three failed coverage-based searches; root reassessment and successful fourth interpolant-sensitive check are preserved |
| [E28 checker build-directory selection fix](checkpoints/2026-10-25-srv01-f19e00-build-dir-env.md) | E641 `check_f19e00_accounting.py` now honors `SOTC_NATIVE_BUILD`; target and all 481 lift checkers pass |
| [Integration PR #8 closeout](checkpoints/2026-10-25-integration-pr8-closeout.md) | PR #8 merged at `61c96f6`; integration branch deleted; workspace returned to clean `main`; no game boot/audio/save claim |
| [CTOR_CLASSES](CTOR_CLASSES.md) | 573-entry constructor-table classification by reachable-body idiom plus the slot 10–19/24–47 family contract; CPU-01 |
| [INPUTTABLE_551140](INPUTTABLE_551140.md) | Numeric-only deepen of Init-hash 275-entry table (`0x551140`); CPU-01 |
| [INPUTTABLE_551140_NAMES_FIXTURE](INPUTTABLE_551140_NAMES_FIXTURE.md) | Private 275-name fixture recipe for registry fill `0x52a220`; CPU-01 |
| [EXTRACTION](EXTRACTION.md) | Repeatable native extractor, inputs, SELF reconstruction; artifact worker |
| [REVERSE_ENGINEERING](REVERSE_ENGINEERING.md) | Ghidra procedure, symbol manifests, boot/audio/save recovery; RE worker |
| [REFERENCE_AUDIT](REFERENCE_AUDIT.md) | Pinned SoTC/Bloodborne/shadPS4 source anchors; source-audit worker |
| [PRIOR_ART](PRIOR_ART.md) | Assessment of three public Apple-Silicon PS4 ports: execution models, lifting/GPU tooling, licences, reuse verdicts |
| [PLATFORM](PLATFORM.md) | Native architecture and subsystem requirements; native-core owner |
| [VALIDATION](VALIDATION.md) | Measurable gates, failures, real-game proof; validation owner |
| [G1 PARTIAL pack (2026-10-22)](checkpoints/2026-10-22-g1-partial-evidence-pack.md) | Phase C evidence; G1 PARTIAL not PASSED; E404 |
| [App-main Init registry slice (2026-10-22)](checkpoints/2026-10-22-appmain-init-registry-slice.md) | Measured Init-hash edge via tip AllocatorFront; PR5 merge gap; E405; G1 PARTIAL |
| [Init trivial remainder leaves (2026-10-22)](checkpoints/2026-10-22-init-531d60-trivial-leaves.md) | Init remainder beyond fragment; E406; G1 PARTIAL |
| [Base Initialize 0x255130 (2026-10-22)](checkpoints/2026-10-22-base-initialize-255130.md) | Orchestrator + 0x357f0 leaf; E407; G1 PARTIAL |
| [Prologue 0x255180 early (2026-10-22)](checkpoints/2026-10-22-prologue-255180-early.md) | Early fragment + 0x335a0; E408; G1 PARTIAL |
| [Prologue 0x255180 tail (2026-10-22)](checkpoints/2026-10-22-prologue-255180-tail.md) | 0x98a90 / app+0x3b58/+0x3b40; E409; G1 PARTIAL |
| [Ctor 0x3be050 header (2026-10-22)](checkpoints/2026-10-22-ctor-98a90-3be050.md) | Nested 0x48 + hooks; E410; G1 PARTIAL |
| [Ctor 0x3be050 buf grow (2026-10-22)](checkpoints/2026-10-22-ctor-98a90-3be050-bufgrow.md) | Empty-path 0x80 vec grow; E411; G1 PARTIAL |
| [Ctor 0x3be050 listwalk (2026-10-22)](checkpoints/2026-10-22-ctor-98a90-3be050-listwalk.md) | Null-head cells 0x2ef63f8/0x2ef6400; E412; G1 PARTIAL |
| [TSC leaf 0x323d0 (2026-10-22)](checkpoints/2026-10-22-store-tsc-323d0.md) | Host monotonic tsc; E413; G1 PARTIAL |
| [Ensure 0x3c2f70 (2026-10-22)](checkpoints/2026-10-22-ensure-3c2f70.md) | Empty-path 0xa800 ensure; E414; G1 PARTIAL |
| [Sub-init 0x3be930 (2026-10-22)](checkpoints/2026-10-22-init-sub-3be930.md) | 0x98-stride banks; E415; G1 PARTIAL |
| [Ctor 0x1ab550 0x40 (2026-10-22)](checkpoints/2026-10-22-ctor-40-1ab550.md) | Prologue-tail 0x40 ctor + base; E416; G1 PARTIAL |
| [Prologue gates 0x2552d4 (2026-10-22)](checkpoints/2026-10-22-prologue-gates-2552d4.md) | Debug gates + empty ba40; E417; G1 PARTIAL |
| [Vslot 0x534130 (2026-10-22)](checkpoints/2026-10-22-vslot-30-534130.md) | Base Init +0x30 bool gate; E418; G1 PARTIAL |
| [Mid 0x255340 (2026-10-22)](checkpoints/2026-10-22-mid-255340.md) | Base Init debug-menu mid; E419; G1 PARTIAL |
| [Singleton 0xf08 (2026-10-22)](checkpoints/2026-10-22-singleton-f08-531d84.md) | Init 0xf08 singleton install; E420; G1 PARTIAL |
| [Vslot 0x533100 header (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-header.md) | Bounded Base Init vslot+0x40 header; E421; G1 PARTIAL |
| [Vslot 0x533100 post (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-post.md) | Post-header + 0x1fc80 show_areaInfo; E422; G1 PARTIAL |
| [Vslot 0x533100 miss (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-miss.md) | Miss path r_fps after show_areaInfo; E423; G1 PARTIAL |
| [Vslot 0x533100 join (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-join.md) | Join+ Position at self+0x3b60; E424; G1 PARTIAL |
| [Vslot 0x533100 gfx (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-gfx.md) | Post-join gfx-cell 0x2ee2d00 prelude; E425; G1 PARTIAL |
| [Vslot 0x533100 shadow (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadow.md) | Shadowmodels load-attempt; E426; G1 PARTIAL |
| [Vslot 0x533100 shadow attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadow-attach.md) | Shadowmodels attach fail-path + install; E427; G1 PARTIAL |
| [Vslot 0x533100 shadowskels (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowskels.md) | Shadowskels load-attempt; E428; G1 PARTIAL |
| [Vslot 0x533100 skels attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-skels-attach.md) | Shadowskels attach fail-path + install f08+0x10; E429; G1 PARTIAL |
| [Vslot 0x533100 shadowanims (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowanims.md) | Shadowanims load-attempt; E430; G1 PARTIAL |
| [Vslot 0x533100 anims attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-anims-attach.md) | Shadowanims attach fail-path + install f08+0x20; E431; G1 PARTIAL |
| [Vslot 0x533100 shadowbanks (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowbanks.md) | Shadowbanks load-attempt; E432; G1 PARTIAL |
| [Vslot 0x533100 banks attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-banks-attach.md) | Shadowbanks attach fail-path + install f08+0x40; E433; G1 PARTIAL |
| [Vslot 0x533100 shadowstreams (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowstreams.md) | Shadowstreams load-attempt; E434; G1 PARTIAL |
| [Vslot 0x533100 streams attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-streams-attach.md) | Shadowstreams attach fail-path + install f08+0x50; E435; G1 PARTIAL |
| [Vslot 0x533100 shadowrandomevents (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowrandomevents.md) | Shadowrandomevents load-attempt; E436; G1 PARTIAL |
| [Vslot 0x533100 randomevents attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-randomevents-attach.md) | Shadowrandomevents attach fail-path + install f08+0x60; E437; G1 PARTIAL |
| [Vslot 0x533100 shadowbossmusic (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-shadowbossmusic.md) | Shadowbossmusic load-attempt; E438; G1 PARTIAL |
| [Vslot 0x533100 bossmusic attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-bossmusic-attach.md) | Shadowbossmusic attach fail-path + install f08+0x70; E439; G1 PARTIAL |
| [Vslot 0x533100 cp3Characters (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-cp3characters.md) | cp3Characters.calt load-attempt; E440; G1 PARTIAL |
| [Vslot 0x533100 cp3Characters attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-cp3characters-attach.md) | cp3Characters attach fail-path + install self+0x3cb8; E441; G1 PARTIAL |
| [Vslot 0x533100 cp3shadowprecache (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-cp3shadowprecache.md) | cp3shadowprecache.calt load-attempt; E442; G1 PARTIAL |
| [Vslot 0x533100 cp3shadowprecache attach (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-cp3shadowprecache-attach.md) | cp3shadowprecache attach fail-path + install self+0x3cc8; E443; G1 PARTIAL |
| [Vslot 0x533100 tail 0x140d90 (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-tail-140d90.md) | Tail 0x140d90(self+0x3cc8,9)+f590 call-site; E444; G1 PARTIAL |
| [Vslot 0x533100 stackchk (2026-10-22)](checkpoints/2026-10-22-vslot-40-533100-stackchk.md) | Stack-check success epilogue through ret; E445; G1 PARTIAL |
| [Init post-base 0x499f80 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-499f80.md) | Init continue 0x499f80(self+0x3c70) call-site; E446; G1 PARTIAL |
| [Init continue 0x56dcb0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-56dcb0.md) | Init continue 0x56dcb0 call-site; E447; G1 PARTIAL |
| [Init continue 0x5c64c0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-5c64c0.md) | Init continue 0x5c64c0 call-site; E448; G1 PARTIAL |
| [Init continue 0x1290 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-1290.md) | Init continue 0x1290 call-site; E449; G1 PARTIAL |
| [Init continue 0x56db70 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-56db70.md) | Init continue 0x56db70 call-site (ret-nop); E450; G1 PARTIAL |
| [Init continue 0x5b3d00 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-5b3d00.md) | Init continue 0x5b3d00 call-site (ret-nop); E451; G1 PARTIAL |
| [Init continue 0x5b3cf0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-5b3cf0.md) | Init continue 0x5b3cf0 call-site (ret-nop); E452; G1 PARTIAL |
| [Init continue AVX BSS (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-avx-bss.md) | Init continue AVX BSS copy (0x1b1dd40→dsts); E453; G1 PARTIAL |
| [Init continue 0x7f7220 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-7f7220.md) | Init continue 0x7f7220 call-site (staffroll helper); E454; G1 PARTIAL |
| [Init continue 0x56fad0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-56fad0.md) | Init continue 0x56fad0 call-site (ret-nop); E455; G1 PARTIAL |
| [Init continue 0xf90310 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f90310.md) | Init continue 0xf90310 call-site (splash-hide PLT); E456; G1 PARTIAL |
| [Init continue ymm BSS (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-ymm-bss.md) | Init continue ymm BSS seed (0x1014a80→BSS); E457; G1 PARTIAL |
| [Init continue ymm inter-cell (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-ymm-intercell.md) | Init continue ymm inter-cell BSS copy; E458; G1 PARTIAL |
| [Init continue xmm seed (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-xmm-seed.md) | Init continue xmm BSS seed (0x1014be0→BSS); E459; G1 PARTIAL |
| [Init continue ymm reload (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-ymm-reload.md) | Init continue ymm reload (0x160430c→0x160433c); E460; G1 PARTIAL |
| [Init continue 0x26800/0xf590 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-26800-f590.md) | Init continue 0x26800/0xf590 call-sites; E461; G1 PARTIAL |
| [Init continue Shadow Boss Jump (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-sbj.md) | Init continue Shadow Boss Jump call-sites; E462; G1 PARTIAL |
| [Init continue Shadow Boss Outro Jump (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-sboj.md) | Init continue Shadow Boss Outro Jump call-sites; E463; G1 PARTIAL |
| [Init continue 0x52eb50 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-52eb50.md) | Init continue 0x52eb50 call-site; E464; G1 PARTIAL |
| [Init continue lea rbx / vmovq (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-lea-vmovq.md) | Init continue lea rbx / vmovq seed; E465; G1 PARTIAL |
| [Init continue 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0.md) | Init continue 0x532ca0 call-site; E466; G1 PARTIAL |
| [Init continue virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18.md) | Init continue virtual-call [rax+0x18]; E467; G1 PARTIAL |
| [Init continue second 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0-u2.md) | Init continue second 0x532ca0 call-site; E468; G1 PARTIAL |
| [Init continue second virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-u2.md) | Init continue second virtual-call [rax+0x18]; E469; G1 PARTIAL |
| [Init continue third 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0-u3.md) | Init continue third 0x532ca0 call-site; E470; G1 PARTIAL |
| [Init continue third virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-u3.md) | Init continue third virtual-call [rax+0x18]; E471; G1 PARTIAL |
| [Init continue fourth 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0-u4.md) | Init continue fourth 0x532ca0 call-site; E472; G1 PARTIAL |
| [Init continue fourth virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-u4.md) | Init continue fourth virtual-call [rax+0x18]; E473; G1 PARTIAL |
| [Init continue fifth 0x532ca0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ca0-u5.md) | Init continue fifth 0x532ca0 call-site; E474; G1 PARTIAL |
| [Init continue fifth virtual-call +0x18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-u5.md) | Init continue fifth virtual-call [rax+0x18]; E475; G1 PARTIAL |
| [Init continue first 0x532ed0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ed0.md) | Init continue first 0x532ed0 call-site; E476; G1 PARTIAL |
| [Init continue first 0x532ed0-series vcall18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-532ed0.md) | Init continue first 0x532ed0-series virtual-call [rax+0x18]; E477; G1 PARTIAL |
| [Init continue second 0x532ed0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-532ed0-u2.md) | Init continue second 0x532ed0 call-site; E478; G1 PARTIAL |
| [Init continue second 0x532ed0-series vcall18 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall18-532ed0-u2.md) | Init continue second 0x532ed0-series virtual-call [rax+0x18]; E479; G1 PARTIAL |
| [Init continue first 0x17070 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-17070.md) | Init continue first 0x17070 call-site; E480; G1 PARTIAL |
| [Init continue first 0x544fe0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-544fe0.md) | Init continue first 0x544fe0 call-site; E481; G1 PARTIAL |
| [Init continue cell-install 0x2eff450 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-cell-2eff450.md) | Init continue cell-install 0x2eff450; E482; G1 PARTIAL |
| [Init continue second 0x17070 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-17070-u2.md) | Init continue second 0x17070 call-site; E483; G1 PARTIAL |
| [Init continue first 0x53fd10 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-53fd10.md) | Init continue first 0x53fd10 call-site; E484; G1 PARTIAL |
| [Init continue cell-install 0x2eff448 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-cell-2eff448.md) | Init continue cell-install 0x2eff448; E485; G1 PARTIAL |
| [Init continue vcall [rax+0x38] (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-vcall38.md) | Init continue vcall [rax+0x38]; E486; G1 PARTIAL |
| [Init continue lea r13 pin 0x32f4fe0 (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-lea-32f4fe0.md) | Init continue lea r13 pin 0x32f4fe0; E487; G1 PARTIAL |
| [Init continue first 0xa2caf0 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2caf0.md) | Init continue first 0xa2caf0 call-site; E488; G1 PARTIAL |
| [Init continue first 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00.md) | Init continue first 0xa2ed00 call-site; E489; G1 PARTIAL |
| [Init continue post-a2ed00 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00.md) | Init continue post-a2ed00 0xf590 call-site; E490; G1 PARTIAL |
| [Init continue second post-a2ed00 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u2.md) | Init continue second post-a2ed00 0xf590 call-site; E491; G1 PARTIAL |
| [Init continue second 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u2.md) | Init continue second 0xa2ed00 call-site; E492; G1 PARTIAL |
| [Init continue first post-a2ed00-u2 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u3.md) | Init continue first post-a2ed00-u2 0xf590 call-site; E493; G1 PARTIAL |
| [Init continue second post-a2ed00-u2 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u4.md) | Init continue second post-a2ed00-u2 0xf590 call-site; E494; G1 PARTIAL |
| [Init continue third 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u3.md) | Init continue third 0xa2ed00 call-site; E495; G1 PARTIAL |
| [Init continue first post-a2ed00-u3 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u5.md) | Init continue first post-a2ed00-u3 0xf590 call-site; E496; G1 PARTIAL |
| [Init continue second post-a2ed00-u3 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u6.md) | Init continue second post-a2ed00-u3 0xf590 call-site; E497; G1 PARTIAL |
| [Init continue fourth 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u4.md) | Init continue fourth 0xa2ed00 call-site; E498; G1 PARTIAL |
| [Init continue first post-a2ed00-u4 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u7.md) | Init continue first post-a2ed00-u4 0xf590 call-site; E499; G1 PARTIAL |
| [Init continue second post-a2ed00-u4 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u8.md) | Init continue second post-a2ed00-u4 0xf590 call-site; E500; G1 PARTIAL |
| [Init continue fifth 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u5.md) | Init continue fifth 0xa2ed00 call-site; E501; G1 PARTIAL |
| [Init continue first post-a2ed00-u5 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u9.md) | Init continue first post-a2ed00-u5 0xf590 call-site; E502; G1 PARTIAL |
| [Init continue second post-a2ed00-u5 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u10.md) | Init continue second post-a2ed00-u5 0xf590 call-site; E503; G1 PARTIAL |
| [Init continue sixth 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u6.md) | Init continue sixth 0xa2ed00 call-site; E504; G1 PARTIAL |
| [Init continue first post-a2ed00-u6 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u11.md) | Init continue first post-a2ed00-u6 0xf590 call-site; E505; G1 PARTIAL |
| [Init continue second post-a2ed00-u6 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u12.md) | Init continue second post-a2ed00-u6 0xf590 call-site; E506; G1 PARTIAL |
| [Init continue seventh 0xa2ed00 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-a2ed00-u7.md) | Init continue seventh 0xa2ed00 call-site; E507; G1 PARTIAL |
| [Init continue first post-a2ed00-u7 0xf590 call-site (2026-10-22)](checkpoints/2026-10-22-init-531d60-post-base-f590-a2ed00-u13.md) | Init continue first post-a2ed00-u7 0xf590 call-site; E508; G1 PARTIAL |
| [FOLDERS](FOLDERS.md) | Generated public/source folder inventory; `scripts/project_index.py` |
| [INSTALL_LOG](../INSTALL_LOG.md) | Existing and newly installed tools, downloads and removals |
| [Checkpoint template](checkpoints/TEMPLATE.md) | Required record at each checkpoint |
| [Escalation template](escalations/TEMPLATE.md) | Three-try handback to principal |
| [Function schema](schemas/function.example.json) | Address-to-native-symbol recovery record |

## Current parallel work

- [Parallel foundations plan](checkpoints/2026-10-22-codex-parallel-foundations-plan.md)
- [Prepared contributor cleanup](checkpoints/2026-10-22-codex-contributor-cleanup-prepared.md)

## Folder contracts

| Folder | What belongs here | Index / protection |
|---|---|---|
| Root | Agent rules, current handoff, roadmap, README, dependency log | This index; root initially has no `.git` |
| `docs/` | Reviewable plans/evidence summaries, task status, schemas, checkpoint/escalation records | This file + generated FOLDERS |
| `scripts/` | Small stdlib project/analysis helpers; never production game code | FOLDERS + source docstrings; tests |
| `native/` | Recovered ARM64 C++ (core/), tests/, README with build/test command; no original bytes or guest state | FOLDERS + native/README; Graft scope added |
| `launcher/` | SwiftPM SwiftUI launcher shell (LauncherCore + app + tests); builds go to local/build/launcher | launcher/README; Graft scope added |
| `tools/` | Reproducible tool build adapters and analysis recipes | FOLDERS + INSTALL_LOG |
| `reference/ShadowOfTheColossus-PC/` | SoTC PC source, unchanged pinned checkout | Graft; REFERENCE_AUDIT |
| `reference/bloodborne_pc/` | Bloodborne performance/architecture reference, unchanged | Graft; REFERENCE_AUDIT |
| `reference/shadPS4/` | Service/GPU semantics reference, unchanged; its AGENTS applies there | Graft; REFERENCE_AUDIT |
| `reference/shadPS4Plus-pkg/` | Standalone extractor source, unchanged | Graft + EXTRACTION |
| `Shadow of the Colossus/` | Original base/update/DLC PKGs, metadata, images; READ ONLY | Private inventory; excluded from publication |
| `local/` | Extracted copyrighted data, reconstructed ELF, raw RE evidence, Ghidra state, builds | `local/indexes/folders.json`; excluded |
| `logs/` | Raw install/build/extraction/verification logs | INSTALL_LOG + evidence links; excluded |
| `graft/` | Regenerable source graph; not canonical documentation | `graft check`; excluded |

Every existing subfolder is listed in generated indexes. Build/cache internals and `.git`
administration and Finder metadata are deliberately summarized, not treated as source documentation. Original
game/private analysis folders have a local-only inventory. Do not hand-author thousands of
duplicate source summaries; Graft is the source-level navigation layer.

## Source identities

| Source | Commit | Role |
|---|---|---|
| [AjncSystem/ShadowOfTheColossus-PC](https://github.com/AjncSystem/ShadowOfTheColossus-PC/tree/70bb43859cd7ff181a6d409540b757866184d60d) | `70bb43859cd7ff181a6d409540b757866184d60d` | Main behavior reference |
| [Supermedo/bloodborne_pc](https://github.com/Supermedo/bloodborne_pc/tree/de2e38af055f09f819946951075c6bb119e62ae6) | `de2e38af055f09f819946951075c6bb119e62ae6` | Optimization methodology; inspect subtree licenses |
| [shadps4-emu/shadPS4](https://github.com/shadps4-emu/shadPS4/tree/0ebea15c24253b60f514fc37a348dabc8d0bcb90) | `0ebea15c24253b60f514fc37a348dabc8d0bcb90` | PS4 interface/format reference |
| [AzaharPlus/shadPS4Plus](https://github.com/AzaharPlus/shadPS4Plus/tree/771a8d58aff540448c7cbace7c40a66a5daf010d) | `771a8d58aff540448c7cbace7c40a66a5daf010d` | PKG_EXTRACTOR_1_1 extraction source |

Verify with `git -C reference/<name> rev-parse HEAD` and `git -C reference/<name> status --short`.
Do not fetch or update a pin silently. Uninitialized submodules are not installed dependencies.
Pinned source URLs support auditability; live upstream pages are separately dated evidence.

## Source / binary indexing workflow

1. Source question: `graft ask "specific question" --source --in reference/ShadowOfTheColossus-PC/`.
2. Need all uses: `graft grep "symbol" --in reference/ShadowOfTheColossus-PC/`.
3. Before changes: `graft callers Symbol --depth all`; respect all connected platform files.
4. Ghidra finding: append its original RVA, file offset, artifact SHA, evidence ID, native
   symbol/path, confidence and unresolved edges to the local function manifest.
5. New/moved folders or handoff: `python3 scripts/project_index.py`; then `graft check`.
6. If Graft is stale: rebuild and report status. Current persisted scope recipe is
   `graft build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir native --only-dir launcher --only-dir reference`.
   When first creating native/launcher/tests, append their scopes while retaining all current
   ones. `graft check` only checks configured scopes; prove new files queryable with skeleton/grep.
   Never index extracted binaries, original game archives or generated pseudocode as source.

Future implementation directories (`native/`, `launcher/`, `tests/`) are proposed, not created
as fake completed components. Their first task must add a folder contract and update the indexes.

## Skills/tools to use for each workstream

| Workstream | Available skill / tool | Boundary |
|---|---|---|
| Binary recovery | `reverse-engineer-anything`, REA Ghidra provider / headless Ghidra | One shared MCP binary-session owner; export evidence; static is not runtime proof |
| Source exploration | `graft` CLI / graph tools | Query before source search; map callers before changes; private binary index stays separate |
| Native core/tool utilities | `ponytail`, Apple Clang/CMake/Ninja | Reuse stdlib/native facilities, keep scripts bounded; no guest-state shortcuts |
| Swift launcher | `macos-design-guidelines`, `swiftui-pro:swiftui-pro`, Xcode/Swift tools | Read skills at implementation time; native macOS lifecycle, not an iOS simulator test |
| Native UI validation | `computer-use` by main agent, native REA observations, Xcode/Instruments | Actual native app/Finder behavior; browser-only testing cannot certify Mac UI |
| Independent bounded work | Collaboration subagents / existing agent workflows | Separate ownership, one HANDOFF integrator, preserved failed-try counts |
| Shared project knowledge | `@wiki`, config-resolved Projects topic | Read index first; durable promotion only when explicitly requested |

Other tools may be installed only when a concrete task needs them. Record version/source/path,
command, architecture, purpose and removal boundary in INSTALL_LOG, then verify the live result.
Do not install a Sony SDK, an emulator or a second analysis engine just to avoid the native task.
| [Init 0x1290 body (2026-10-22)](checkpoints/2026-10-22-init-1290-body-analysis.md) | 195 B full body: guard read-only, `void`, one no-op vcall, two descriptor registrations; E509; G1 PARTIAL |
| [Init continue batch A (2026-10-22)](checkpoints/2026-10-22-init-531d60-continue-batch-a.md) | 22 units / 95 B `0x532720..0x53277f`, 5 calls + 3 lea + `je` pinned; E512; G1 PARTIAL |
| [Repo-health round (2026-10-22)](checkpoints/2026-10-22-repo-health-round.md) | Full suite 326/326 after stale ctor-test fix; publish-audit final-component gap fixed; E510/E511 |
| [Init 0x1290 body (2026-10-22)](checkpoints/2026-10-22-init-1290-body-analysis.md) | 195 B full body: guard read-only, `void`, one no-op vcall, two descriptor registrations; E516; G1 PARTIAL |
| [Init continue batch A (2026-10-22)](checkpoints/2026-10-22-init-531d60-continue-batch-a.md) | 22 units / 95 B `0x532720..0x53277f`, 5 calls + 3 lea + `je` pinned; E519; G1 PARTIAL |
| [Repo-health round (2026-10-22)](checkpoints/2026-10-22-repo-health-round.md) | Full suite green after stale ctor-test fix; publish-audit final-component gap fixed; E517/E518 |
| [Init continue batch B (2026-10-22)](checkpoints/2026-10-22-init-531d60-continue-batch-b.md) | 14 units / 50 B `0x53277f..0x5327b1`, calls `0x1e4c0`/`0x4c4950`, `je->0x5328e0` pinned; E520; G1 PARTIAL |
| [Fixture-suite replay (2026-10-22)](checkpoints/2026-10-22-int-validation-fixture-replay.md) | 326/326 with fixtures; "325/327 FAILED" does not reproduce; E521 |
| [Init insert path 0x1bde30 (2026-10-22)](checkpoints/2026-10-22-init-1bde30-insert-path.md) | 0x20 B descriptor copy at node `+0x10`; `+0x40` = list-3 sentinel; 4-slot vtable + names; no null guard; E522; G1 static |
| [Init 0x1e4c0 contract (2026-10-22)](checkpoints/2026-10-22-init-1e4c0-contract.md) | Config-bool lookup proving `bl` ∈ {0,1}; branch rule `bl=1→0x532904`; E524; G1 static |
| [ATTRIBUTION](ATTRIBUTION.md) | Owner-only contributor policy, pre-push attribution gate, audit state; E525 |
| [Init continue batch C (2026-10-22)](checkpoints/2026-10-22-init-531d60-continue-batch-c.md) | 38 units / 153 B `0x532904..0x53299d`, 3 calls + 3 lea, `je->0x5329ae` pinned; E526; G1 PARTIAL |
| [Init 0x1290 native lift (2026-10-22)](checkpoints/2026-10-22-init-1290-native-lift.md) | Ordinary native `init_1290`, 0x20 B descriptors, order + `&A+0x20` asserted; E527; G1 PARTIAL |
| [Init je 0x5328e0 branch (2026-10-22)](checkpoints/2026-10-22-init-branch-5328e0.md) | Branch modelled from `0x1e4c0` boolean; taken path stops honestly; E528; G1 PARTIAL |
| [Init 0x5328e0 preamble (2026-10-22)](checkpoints/2026-10-22-init-branch-5328e0.md) | 36 B taken-path preamble modelled; `sete r14b` feeds the merge; E529; G1 PARTIAL |
| [Init 0x2efe318 cells (2026-10-22)](checkpoints/2026-10-22-init-2efe318-cells.md) | `0x2efe318` = vtable-ptr global → `0x15976b0`; slot0 `0x91210` lazy bind; `0x2efe340` = `+0x28`; `sete` flag `[+0x60]`; E530 |
| [Insert 0x1beeb0 native (2026-10-22)](checkpoints/2026-10-22-insert-1beeb0-native.md) | 0x20 B descriptor copy executed: new(0x60), self-link, list links, count; E531 |
| [Insert 0x1beeb0 native (2026-10-22)](checkpoints/2026-10-22-insert-1beeb0-native.md) | 0x20 B descriptor copy executed: new(0x60), self-link, list links, count; E531/E532 |
| [Insert 0x21cd0 native (2026-10-22)](checkpoints/2026-10-22-insert-21cd0-native.md) | Bump-allocator B insert: 0x20 B entry, begin/end + grow hooks; E533; G1 PARTIAL |
| [Container 0x1bdb10 native (2026-10-22)](checkpoints/2026-10-22-container-1bdb10-native.md) | Three-list 0x60 B container ctor, self loops + counts; E534; G1 PARTIAL |
| [Init continue batch D (2026-10-22)](checkpoints/2026-10-22-init-531d60-continue-batch-d.md) | 17 B fall-through `0x53299d..0x5329ae`, lea `0x32f79a8`, call `0xa473b0`; E535 |
| [Init tail 0x53299d (2026-10-22)](checkpoints/2026-10-22-init-tail-53299d.md) | Fall-through 17 B; target block 350 B; callees named (capture setter, save-data ctor); E537 |
| [Init merge je 0x5329ae (2026-10-22)](checkpoints/2026-10-22-init-branch-5329ae.md) | Merge branch modelled from `[+0x60]` byte; taken path stops honestly; E536 |
| [Init tail provenance (2026-10-23)](checkpoints/2026-10-23-init-32f79a8-2eff450-provenance.md) | `0x32f79a8` lazy singleton writers; `0x2eff450` vtable slot1 `0x54b250`; E538 |
| [Capture 0xa473b0 native (2026-10-23)](checkpoints/2026-10-23-capture-a473b0-native.md) | 134 B lifted over `CaptureServices` contracts; no fake success; E539 |
| [Save-data callees (2026-10-23)](checkpoints/2026-10-23-savedata-callees.md) | `0x54b250` stat query; `0xa54920` ConVar counter registrar, no save-data imports; E540 |
| [Init continue batch E (2026-10-23)](checkpoints/2026-10-23-init-531d60-continue-batch-e.md) | 350 B / 53 units to the registry fragment; 3 registrars + 3 no-op vcalls; E541 |
| [Orchestrator session 2 handoff (2026-10-23)](checkpoints/2026-10-23-orchestrator-session-2-handoff.md) | Session summary, corrections, verification commands, open work; E509–E543 |
| [Init continue batch F (2026-10-23)](checkpoints/2026-10-23-init-531d60-continue-batch-f.md) | Alternate path 327 B / 70 units + failure exit; coverage 98.23%; E544 |
| [Native 0x1290 in the slice (2026-10-23)](checkpoints/2026-10-22-init-1290-native-lift.md) | Slice executes the lifted 0x1290 with both inserts; E545 |
| [Init entry gaps (2026-10-23)](checkpoints/2026-10-23-init-entry-gaps.md) | Four remaining spans; Init byte coverage 100% of the window; E546 |
| [Stat query 0x54b250 native (2026-10-23)](checkpoints/2026-10-23-stat-query-54b250-native.md) | 119 B wrapper over `StatsEngine` contract; no-op slot elided; E547 |
| [Insert callable helpers (2026-10-23)](checkpoints/2026-10-23-insert-callable-helpers.md) | Copy/move/destroy vcalls are three 20-byte helpers on a shared table; E548 |
| [Owner dtor 0x1bdae0 native (2026-10-23)](checkpoints/2026-10-23-owner-dtor-1bdae0.md) | 46 B destroy→delete→clear with visible failures; E551 |
| [Shared release 0xf590 native (2026-10-23)](checkpoints/2026-10-23-shared-release-f590.md) | 73 B shared-ownership release: atomic decrement, last-ref destroy+free; E552 |
| [Allocator wrappers (2026-10-23)](checkpoints/2026-10-23-alloc-wrappers.md) | `0x17070`/`0x170b0`/`0x170c0` tail wrappers; `0x12af0` is tagged deallocate; E553 |
| [List dtor 0x1bdeb0 decoded (2026-10-23)](checkpoints/2026-10-23-list-dtor-1bdeb0-decoded.md) | Erase-and-release loop decoded; not lifted (node type conflict at +0x50); E555 |
| [List dtor 0x1bdeb0 resolved (2026-10-23)](checkpoints/2026-10-23-list-dtor-1bdeb0-resolved.md) | Two node types (0x58 flag / 0x60 callable); single caller on the 0x2ee68c8 singleton; E558 |
| [List dtor 0x1bdeb0 native (2026-10-23)](checkpoints/2026-10-23-list-dtor-1bdeb0-native.md) | 0x58 flag nodes; skip/unlink/release/free/count; E559 |
| [Insert 0x1bdb50 native (2026-10-23)](checkpoints/2026-10-23-insert-1bdb50-native.md) | 0x58-node inserter; acquire hand-off; dtor round-trip; E560 |
| [Registry semantics (2026-10-23)](checkpoints/2026-10-23-registry-semantics-e561.md) | The 0x58 list is an app-info registry; five sites; not the ConVar path; E561 |
| [HANDOFF archive 2026-10-23](checkpoints/HANDOFF-archive-2026-10-23.md) | Full pre-tidy handoff preserved verbatim; live state is HANDOFF.md |
| [App-info table owner (2026-10-23)](checkpoints/2026-10-23-appinfo-table-owner.md) | RELRO pointer-slice census; `0x2569d0` consumer; E562 |
| [App-info installer semantics (2026-10-23)](checkpoints/2026-10-23-appinfo-installer-semantics.md) | `0x25a4d0` entries, E561 amendments, FIFO dispatch; E563 |
| [App-info registry native (2026-10-23)](checkpoints/2026-10-23-appinfo-registry-native.md) | Native registry, installer, owner dispatch, container dtor; E564 |
| [0xf590 holder wiring (2026-10-23)](checkpoints/2026-10-23-f590-holder-wiring.md) | 10,168-site census; 17 Init modules wired; E565 |
| [Parallel-foundations integration (2026-10-23)](checkpoints/2026-10-23-parallel-foundations-integration.md) | Exclusive save temp, shared-string release, rackquery CFG; E566 |
| [Owner installer 0x2569d0 (2026-10-23)](checkpoints/2026-10-23-owner-installer-2569d0.md) | 12,621 B / 87-block mapping; registry-slot native; E567 |
| [Round 24 verification (2026-10-23)](checkpoints/2026-10-23-round-24-verification.md) | 345/345 ctest, 442/0/0 sweep, audits, new fixtures/checkers; E568 |
| [App-info list reader (2026-10-23)](checkpoints/2026-10-23-appinfo-list-reader.md) | `0x1bdf30` serializes list A and drains flag==1; snapshotBugData gate; E569 |
| [Owner rows 0x2569d0 (2026-10-23)](checkpoints/2026-10-23-owner-rows-2569d0.md) | RetailBuild gate, mode==1 proof, 259-row property table; E570 |
| [List-B inserter 0x1bdc80 (2026-10-23)](checkpoints/2026-10-23-insert-1bdc80-native.md) | 0x50-node list-B insert; dtor list-B type; E571 |
| [Camera chain (2026-10-23)](checkpoints/2026-10-23-camera-chain.md) | Euler/format pure lift; live pose chain blocked; E573 |
| [Round 25 verification (2026-10-23)](checkpoints/2026-10-23-round-25-verification.md) | 349/349 ctest, 446/0/0 sweep; E569–E573 published |
| [`0xf590` rank-1 wiring (2026-10-23)](checkpoints/2026-10-23-f590-rank1-wiring.md) | Four proven-zero `0x533100` sites on the real inert release; `0x5333e3` stays a hook; E574 |
| [List-B reader/report tree (2026-10-23)](checkpoints/2026-10-23-appinfo-listb-report.md) | FIFO `"Attachment"` serialization, deep-copy report, value-then-name clear; E575 |
| [Owner map sinks (2026-10-23)](checkpoints/2026-10-23-owner-map-sink.md) | Consumers, not tables; B is a float range; duplicate keys; E576 |
| [Owner property bag (2026-10-23)](checkpoints/2026-10-23-owner-prop-bag.md) | 0x178 bag, four sentinels, multimap insert_equal model; E577 |
| [App-info snapshot path (2026-10-23)](checkpoints/2026-10-23-appinfo-snapshot.md) | Phases 2/4/5/6 composed: FieldEntry, Attachment, drain, clear; E578 |
| [Owner member driver (2026-10-23)](checkpoints/2026-10-23-owner-members.md) | 21 install rows, three destroy idioms, hook-driven ctors; E579 |
| [Simple owner ctors (2026-10-23)](checkpoints/2026-10-23-owner-simple-ctors.md) | 0xa1cc0/0xfafb0/0x26f820 native; E580 |
| [SharedStr ctor 0xf1a0 (2026-10-23)](checkpoints/2026-10-23-shared-str-ctor.md) | 0x20 B holder, control init, GetResource vtable; E581 |
| [ResourceId 0x14c2f0 (2026-10-23)](checkpoints/2026-10-23-resource-id-14c2f0.md) | `****` rewrite + FNV-1a-32; E582 |
| [Camera delegate writer (2026-10-23)](checkpoints/2026-10-23-camera-delegate-writer.md) | `_Root` zeroes the delegate; no static writer; E583 |
| [App-info run path (2026-10-23)](checkpoints/2026-10-23-appinfo-run.md) | Dispatch + snapshot composition; E584 |
| [SnapshotBugData gate (2026-10-23)](checkpoints/2026-10-23-snapshot-bug-data.md) | Gate clear, synthetic Command, Screenshot attachment, adoption; E585 |
| [`0xf590` key wiring (2026-10-23)](checkpoints/2026-10-23-f590-key-wiring.md) | Nine real SharedStr keys/releases; E586 |
| [SharedStr resize 0xf3c0 (2026-10-23)](checkpoints/2026-10-23-shared-str-resize.md) | Four COW paths, return-true only on reallocation; E587 |
| [Frame root (2026-10-23)](checkpoints/2026-10-23-frame-root.md) | Lazy Frame/frameNum, adoption; E588 |
| [Platform tag 0x147e20 (2026-10-23)](checkpoints/2026-10-23-platform-tag.md) | `_cmn`/`_ps4` table, nine real key hashes; E589 |
| [Owner bag ctor (2026-10-23)](checkpoints/2026-10-23-owner-bag-ctor.md) | 1..4 per-user input devices, not language tables; E590 |
| [Bag user selection (2026-10-23)](checkpoints/2026-10-23-owner-bag-users.md) | 1..4 input-device cap/dedupe; E591 |
| [Bag construct (2026-10-23)](checkpoints/2026-10-23-owner-bag-construct.md) | Hooks, 1..4 selection, mode/publish; E592 |
| [ResourcePool (2026-10-23)](checkpoints/2026-10-23-resource-pool.md) | ResourceWrapper refs/deps, non-owning map; E593 |
| [ResourcePool native (2026-10-23)](checkpoints/2026-10-23-resource-pool-native.md) | Wrapper/refs/deps/map model; E594 |
| [ResourcePool wiring (2026-10-23)](checkpoints/2026-10-23-resource-pool-wiring.md) | Nine attach sites on the real pool; E595 |
| [Owner slots +0x48/+0x58 (2026-10-23)](checkpoints/2026-10-23-owner-slots-48-58.md) | Singleton ensure/activate, gated tree insert; E596 |
| [GPU readback contract (2026-10-23)](checkpoints/2026-10-23-gpu-readback-contract.md) | PNG encoder, socket sink, frameNum blind spot; E597 |
| [SaveData contract (2026-10-23)](checkpoints/2026-10-23-savedata-contract.md) | Flat progress blob, transaction API, 29 cases; E598 |
| [Camera writer full (2026-10-23)](checkpoints/2026-10-23-camera-writer-full.md) | No writer; +0x2e8 is a list node in `_Root`; E599 |
| [Screenshot contract (2026-10-23)](checkpoints/2026-10-23-screenshot-contract.md) | attach_screenshot decision table; E600 |
| [Save transaction (2026-10-23)](checkpoints/2026-10-23-save-transaction.md) | ProgressV1, atomic store/load, golden 20,013 B; E601 |
| [Camera class identity (2026-10-23)](checkpoints/2026-10-23-camera-class-identity.md) | D is Camera, not `_Root`; debug-only pose sync; E602 |
| [Owner ctor a5c850 (2026-10-23)](checkpoints/2026-10-23-owner-ctor-a5c850.md) | 721 B ctor, zero-based row 9 / one-based row 10, partial-init layout; E603 |
| [Save metadata contract (2026-10-23)](checkpoints/2026-10-23-savedata-metadata-contract.md) | 0x530 block, shared icon, delete/dialog; E604 |
| [Camera root registration (2026-10-23)](checkpoints/2026-10-23-camera-root-registration.md) | +0x10 is a pass-through, no camera attach; E605 |
| [Owner dtor a5cb30 (2026-10-23)](checkpoints/2026-10-23-owner-dtor-a5cb30.md) | 594 B teardown, zero-based row 9 / one-based row 10; E607 |
| [Save metadata (2026-10-23)](checkpoints/2026-10-23-save-metadata.md) | 0x530 block, USR-META, delete; E606 |
| [Ctor coverage batch (2026-10-23)](checkpoints/2026-10-23-ctor-coverage-batch.md) | 308/573 registered, first gap 296; E608 |
| [SoCGameCore executable (2026-10-23)](checkpoints/2026-10-23-soc-game-core-executable.md) | arm64 core, protocol v1, no engine; E609 |
| [Metal capture provider (2026-10-23)](checkpoints/2026-10-23-metal-capture-provider.md) | offscreen RGBA/PNG, synthetic; E610 |
| [Owner row 14 a597d0 (2026-10-23)](checkpoints/2026-10-23-owner-row14-a597d0.md) | 0x202 B row-14 ctor; E611 |
| [Owner row 21 4a690 (2026-10-23)](checkpoints/2026-10-23-owner-row21-4a690.md) | queue drain into an ordered tree; E612 |
| [Launcher process client (2026-10-23)](checkpoints/2026-10-23-launcher-process-client.md) | locator + v1 client + factory; E613 |
| [Concurrent process-client request claim](checkpoints/2026-10-27-app01-client-concurrent-requests-claim.md) | Test-only status/coverage concurrency and response-ID correlation through the child-process fake |
| [Concurrent process-client request fixture](checkpoints/2026-10-27-app01-client-concurrent-requests-complete.md) | E736 test-only concurrency fixture; 21 focused and 154 launcher tests pass, real-core integration skipped |
| [Post-0fec9e9e remote attribution scan](checkpoints/2026-10-27-post-0fec9e9e-attribution-scan.md) | E737; 9 branch heads and 2,361 commits scanned with zero Claude/Anthropic matches |
| [APP-01 pending-request shutdown fixture claim](checkpoints/2026-10-27-app01-shutdown-pending-request-claim.md) | Test-only task claim; controlled shell fake will verify `.badState` before `quit` |
| [Post-07e2055f remote attribution scan](checkpoints/2026-10-27-post-07e2055f-attribution-scan.md) | E738; 9 branch heads and 2,362 commits scanned with zero Claude/Anthropic matches |
| [Post-3d9f58b1 remote attribution scan](checkpoints/2026-10-27-post-3d9f58b1-attribution-scan.md) | E739; 9 branch heads and 2,363 commits scanned with zero Claude/Anthropic matches |
| [APP-01 pending-request shutdown fixture](checkpoints/2026-10-27-app01-shutdown-pending-request-complete.md) | E740; 22 focused and 155 launcher tests pass; real-core integration skipped |
| [Post-447557f5 remote attribution scan](checkpoints/2026-10-27-post-447557f5-attribution-scan.md) | E741; 9 branch heads and 2,364 commits scanned with zero Claude/Anthropic matches |
| [Post-ff90a389 remote attribution scan](checkpoints/2026-10-27-post-ff90a389-attribution-scan.md) | E742; 9 branch heads and 2,365 commits scanned with zero Claude/Anthropic matches |
| [APP-01 in-flight child-exit fixture claim](checkpoints/2026-10-27-app01-inflight-exit-failure-claim.md) | Test-only claim; child logs status then exits without replying, caller must receive non-timeout `.io` |
| [Post-ca87a357 remote attribution scan](checkpoints/2026-10-27-post-ca87a357-attribution-scan.md) | E743; 9 branch heads and 2,366 commits scanned with zero Claude/Anthropic matches |
| [APP-01 in-flight child-exit fixture](checkpoints/2026-10-27-app01-inflight-exit-failure-complete.md) | E745; 23 focused and 156 launcher tests pass; real-core integration skipped |
| [Post-4cb0afcd remote attribution scan](checkpoints/2026-10-27-post-4cb0afcd-attribution-scan.md) | E744; 9 branch heads and 2,367 commits scanned with zero Claude/Anthropic matches |
| [Post-f033ed84 remote attribution scan](checkpoints/2026-10-27-post-f033ed84-attribution-scan.md) | E746; 9 branch heads and 2,368 commits scanned with zero Claude/Anthropic matches |
| [Post-c4582888 remote attribution scan](checkpoints/2026-10-27-post-c4582888-attribution-scan.md) | E747; 9 branch heads and 2,369 commits scanned with zero Claude/Anthropic matches |
| [Post-fc522ee7 remote attribution scan](checkpoints/2026-10-27-post-fc522ee7-attribution-scan.md) | E749; 9 branch heads and 2,371 commits scanned with zero Claude/Anthropic matches |
| [Slot 309 saved-project Ghidra coverage](checkpoints/2026-10-27-slot309-ghidra-coverage.md) | E748; root static coverage verified, helper dispatch and lifetime boundary remain open |
| [Slot 309 `0x26ea0` virtual-call resolution](checkpoints/2026-10-27-slot309-concmd-vcall-resolution.md) | E750; four caller vptrs and constant-returning targets mapped, static only |
| [Post-fc522ee7 remote attribution scan](checkpoints/2026-10-27-post-fc522ee7-attribution-scan.md) | E749; 9 branch heads and 2,371 commits scanned with zero Claude/Anthropic matches |
| [Launcher bundle with core (2026-10-24)](checkpoints/2026-10-23-launcher-bundle-core.md) | signed app with SoCGameCore; E614 |
| [Save relaunch (2026-10-23)](checkpoints/2026-10-23-save-relaunch.md) | cross-process write/load/enumerate; E615 |
| [Save acceptance tool (2026-10-23)](SAVE_ACCEPTANCE.md) | read-only SAV-04 inspector; E616/NOT RUN |
| [Startup slots 296/297/300 (2026-10-24)](checkpoints/2026-10-24-startup-slots-296-297-300.md) | TSC thunk, 400-record table, inert atexit; E618 |
| [E621 superseded metadata task/evidence reconciliation](checkpoints/2026-10-27-e621-task-evidence-reconciliation.md) | Reconciles stale DATA-02/AUD-01 task and evidence summaries with corrected E621 locators; no game gate |
| [Slot 309 manager creation order after the constructor list](checkpoints/2026-10-27-slot309-manager-order-after-init.md) | E809-E812; direct vtable installation order narrowed after the constructor list; indirect `0x15d5b80` path and general temporary ownership remain unresolved |
| [Project index and Swift build churn](checkpoints/2026-10-27-project-index-build-churn.md) | E813; only ignored `launcher/.build` is excluded; unrelated folders still trigger stale-index detection |
| [Slot-309 route follow-up and E813 publication closeout](checkpoints/2026-10-27-slot309-dispatch-and-e813-publication-closeout.md) | E814-E815; E813 is fetch-verified and the `0x15d5b80` direct route remains unresolved under bounded Ghidra evidence |
| [Slot-309 cleanup ordering and E814 publication closeout](checkpoints/2026-10-27-slot309-240f0-order-e814-publication-closeout.md) | E816-E817; E814 is fetch-verified; known direct cleanup-table paths follow dispatch, while the `0x15d5b80/88` wrapper route remains unordered |
| [Slot-309 wrapper-table relocations and unresolved ordering](checkpoints/2026-10-27-slot309-table-relocations-e818.md) | E818-E819; RELA slots and wrapper cleanup edges verified with no consumer/order; E818 pushed and fetch-verified, and 2,431 live remote commits scan Claude-free |
| [Slot-309 payload ownership boundary](checkpoints/2026-10-27-slot309-payload-ownership-e821.md) | E821; shallow `+0x10` copy and no-op cleanup confirmed on one path; ownership remains unresolved |
| [E821 publication and attribution closeout](checkpoints/2026-10-27-e821-publication-closeout.md) | E822; E821 fetched on `main`; 2,434 remote commits scanned with zero Claude/Anthropic hits |
| [Claim: revalidate three corrected pakN sidecar members](checkpoints/2026-10-27-aud01-pakn-sidecar-revalidate-claim-e823.md) | E823; pinned archive/manifest identities; selected decoded-member verification not yet run |
| [Revalidated corrected pakN sidecar identities](checkpoints/2026-10-27-aud01-pakn-sidecar-revalidate-e824.md) | E824; three exact paths, TOC locators, decoded lengths and SHA-256 values verified; no format or runtime claim |
| [Claim: check SELF program-header 10 recoverability](checkpoints/2026-10-27-gpu03-tex0-segment10-claim-e825.md) | E825; bounded in-memory check against the pinned plaintext reader; descriptor mapping and recoverability not yet run |
| [SELF program-header 10 recoverability result](checkpoints/2026-10-27-gpu03-tex0-segment10-complete-e826.md) | E826; no plaintext descriptor maps to PHDR 10; the pinned reader reports it missing; no segment contents inspected |
| [Claim: profile three pinned pakN sound-sidecar structures](checkpoints/2026-10-27-aud01-pakn-structure-probe-claim-e827.md) | E827; in-memory structural profile only after E824 byte identity checks; CXML/CRMP semantics remain unknown |
| [Bounded pakN sidecar structure probe](checkpoints/2026-10-27-aud01-pakn-structure-probe-complete-e828.md) | E828; three member identities and CRMP byte profiles verified; CXML root/name structure remains UNKNOWN |
| [Claim: trace slot-309 nonzero `+0x10` consumer](checkpoints/2026-10-27-slot309-nonzero-consumer-claim-e829.md) | E829; read-only bounded continuation using pinned saved Ghidra/export evidence; ownership remains UNKNOWN pending a concrete consumer/release path |
| [Slot-309 nonzero consumer trace](checkpoints/2026-10-27-slot309-nonzero-consumer-complete-e830.md) | E830; copy-only vtable methods and no-op cleanup verified; no concrete nonzero producer/reader/release path; ownership UNKNOWN |
| [Claim: census slot-309 helper callers](checkpoints/2026-10-27-slot309-helper-caller-census-claim-e831.md) | E831; read-only saved-project caller census for `0xa50280` and `0xa50440`; source `+0x10` argument values remain to be verified |
| [Slot-309 helper caller census](checkpoints/2026-10-27-slot309-helper-caller-census-complete-e832.md) | E832; one saved caller per helper; observed record's `+0x10` is zeroed and copied to `+0x90`; semantic ownership UNKNOWN |
| [Slot-309 indirect-copy lead disposition](checkpoints/2026-10-27-slot309-indirect-vtable-copy-stopped-e833.md) | E833; proposed call fails the E832 continuation gate; analysis not run; ownership UNKNOWN |
| [E833 publication and Claude-attribution closeout](checkpoints/2026-10-27-e833-publication-closeout-e834.md) | E834; E833 pushed and fetch-verified; all advertised GitHub refs remain Claude/Anthropic-clean |
| [E834 publication and Claude-attribution closeout](checkpoints/2026-10-27-e834-publication-closeout-e835.md) | E835; E834 pushed and fetch-verified; 2,447 reachable GitHub commits scan clear |
| [All-lane readiness triage](checkpoints/2026-10-27-ready-lane-triage-e836.md) | E836; CPU, service/data/audio/save, app, GPU, and RE scouts found no newly READY task |
| [E836 publication and attribution closeout](checkpoints/2026-10-27-e836-publication-closeout-e837.md) | E837; E836 pushed and fetch-verified; 2,449 advertised-ref reachable commits scan clear; stash left unchanged |
| [E837 publication and attribution closeout](checkpoints/2026-10-27-e837-publication-closeout-e838.md) | E838; E837 pushed and fetch-verified; 2,450 advertised-ref reachable commits scan clear; stash left unchanged |
| [E838 publication and attribution closeout](checkpoints/2026-10-27-e838-publication-closeout-e839.md) | E839; E838 pushed and fetch-verified; 2,451 advertised-ref reachable commits scan clear; stash left unchanged |
| [E839 publication closeout](checkpoints/2026-10-27-e839-publication-closeout-e840.md) | E840; E839 pushed and fetch-verified; exact-range attribution passed; stash left unchanged |
| [E840 publication closeout](checkpoints/2026-10-27-e840-publication-closeout-e841.md) | E841; E840 pushed and fetch-verified; exact-range attribution passed; stash left unchanged |
| [Post-E841 live GitHub attribution census](checkpoints/2026-10-27-post-e841-github-attribution-census-e842.md) | E842; 2,454 advertised-ref reachable commits scan clear; 70 stash-only matches remain by owner choice |
| [Claim: app-main Init import map for 0xb11290 (2026-10-30)](checkpoints/2026-10-30-re02-b11290-import-map-claim-e1024.md) | E1024; bounded task claim, closed by E1025 |
| [app-main Init import map result for 0xb11290](checkpoints/2026-10-29-re02-b11290-import-map-e1025.md) | E1025; three PLT stubs mapped to imports with pinned GOT/NID evidence; arguments and runtime effects remain unknown |
| [Claim: `0xb11290` system-module loader arguments](checkpoints/2026-10-29-re02-b11290-sysmodule-args-claim-e1026.md) | E1026; recover 15 numeric IDs and cite only supported module names; no service-effect or boot inference |
| [`0xb11290` system-module loader argument result](checkpoints/2026-10-29-re02-b11290-sysmodule-args-e1027.md) | E1027; 14 CALLs and one tail JMP, all 15 IDs byte-verified; labels attributed to pinned shadPS4 table; no runtime semantics |
| [Claim: `0x499f80` app-main Init body liftability](checkpoints/2026-10-29-re02-499f80-body-liftability-claim-e1028.md) | E1028; claimed function-level characterization; E1029 records the provider block, with no body Evidence ID or liftability conclusion |
| [`0x499f80` REA query boundary](checkpoints/2026-10-29-re02-499f80-rea-timeout-e1029.md) | E1029; three function-evidence query paths stalled at the Ghidra provider; no body Evidence ID or liftability conclusion; task BLOCKED |
| [Claim: app-main Init callee `0xb11290` structure](checkpoints/2026-10-27-re02-b11290-claim-e843.md) | E843; bounded hash-pinned structural analysis claimed; no runtime or G1 claim |
| [Partial app-main Init callee `0xb11290` structure](checkpoints/2026-10-27-re02-b11290-partial-e844.md) | E844; offline byte profile replay matches; REA function query timed out; no semantic or G1 claim |
| [E844 publication closeout](checkpoints/2026-10-27-e844-publication-closeout-e845.md) | E845; E844 pushed and fetch-verified; fresh advertised-ref census covers 2,457 commits with zero Claude/Anthropic matches; stash left unchanged |
| [b11290 resource reassessment and E846 attribution census](checkpoints/2026-10-27-re02-b11290-resource-reassessment-e846.md) | E846; current advertised-ref scan covers 2,458 commits with zero Claude/Anthropic matches; attempt 2 authorized but not run; stash left unchanged |
| [b11290 REA recovery and E846 publication closeout](checkpoints/2026-10-27-re02-b11290-rea-recovery-e847.md) | E847; attempt 2 completed and the 15-field structural profile replay matches; fresh advertised-ref scan covers 2,459 commits with zero Claude/Anthropic matches; stash left unchanged |
| [E847 publication closeout and live attribution census](checkpoints/2026-10-27-e847-publication-closeout-e848.md) | E848; E847 pushed and exact-range verified; live census covers 2,460 commits with zero Claude/Anthropic matches; stash left unchanged |
| [E848 publication closeout and current live attribution census](checkpoints/2026-10-27-e848-publication-closeout-e849.md) | E849; E848 pushed and exact-range verified; live census covers 2,461 commits with zero Claude/Anthropic matches; stash left unchanged |
| [E850 current attribution census and music SXD1 profile claim](checkpoints/2026-10-27-aud01-music-sxd1-probe-claim-e850.md) | E850; E849 publication reconciled; live census covers 2,462 advertised-ref commits with zero Claude/Anthropic matches; private stash left unchanged; exact music.sxd1 byte-profile task claimed |
| [E851 private music SXD1 byte profile and refreshed attribution](checkpoints/2026-10-27-aud01-music-sxd1-profile-e851.md) | E851; independent byte-profile replay matched all 23 fields; live advertised-ref census covers 2,463 commits with zero Claude/Anthropic matches; private stash unchanged |
| [E852 E851 publication closeout and refreshed attribution](checkpoints/2026-10-27-publication-closeout-e851-e852.md) | E852; E851 published/fetch-verified; live advertised-ref census covers 2,464 commits with zero Claude/Anthropic matches; private stash unchanged |
| [E853 E852 publication closeout and current attribution](checkpoints/2026-10-27-publication-closeout-e852-e853.md) | E853; E852 published/fetch-verified; live advertised-ref census covers 2,465 commits with zero Claude/Anthropic matches; private stash unchanged |
| [E854 E853 publication closeout and final task handoff](checkpoints/2026-10-27-publication-closeout-e853-e854.md) | E854; E853 published/fetch-verified; E853 census and individually audited later commits show zero Claude/Anthropic attribution; current product blockers documented |
| [E855 E854 publication closeout and live attribution census](checkpoints/2026-10-27-publication-closeout-e854-e855.md) | E855; fresh live scan covers 2,467 advertised-ref commits with zero Claude/Anthropic matches; private stash left unchanged by user direction; task scouts found no independent READY lane |
| [E856 E855 publication closeout and live attribution census](checkpoints/2026-10-27-publication-closeout-e855-e856.md) | E856; E855 commit pushed and fetch-verified; fresh live scan covers all 2,468 advertised-ref commits with zero Claude/Anthropic matches; private stash left unchanged |
| [E857 E856 publication and all-ref attribution closeout](checkpoints/2026-10-27-publication-closeout-e856-e857.md) | E857; independently replayed checker-pattern scan finds zero Claude/Anthropic hits across 2,469 named/advertised commits; private stash remains unchanged |
| [E858 E857 publication closeout and post-push attribution](checkpoints/2026-10-27-e857-publication-closeout-e858.md) | E858; E857 pushed and fetch-verified; fresh all-ref scan covers 2,470 commits with zero Claude/Anthropic hits; private stash remains unchanged |
| [E859 E858 publication closeout and post-push attribution](checkpoints/2026-10-27-e858-publication-closeout-e859.md) | E859; E858 pushed and fetch-verified; fresh all-ref scan covers 2,471 commits with zero Claude/Anthropic hits; private stash remains unchanged |
| [E860 E859 publication closeout and post-push attribution](checkpoints/2026-10-27-e859-publication-closeout-e860.md) | E860; E859 pushed and fetch-verified; fresh all-ref scan covers 2,472 commits with zero Claude/Anthropic hits; private stash remains unchanged |
| [E861 Post-E860 public-ref attribution census](checkpoints/2026-10-27-e860-publication-closeout-e861.md) | E861; fresh census and independent replay cover 2,473 reachable commits on current GitHub refs with zero Claude/Anthropic hits; stash unchanged by user direction |
| [E862 E861 publish closeout and live attribution](checkpoints/2026-10-27-e861-publish-closeout-e862.md) | E862; E861 pushed and fetch-verified; fresh replay covers 2,474 reachable commits on current GitHub refs with zero Claude/Anthropic hits; stash unchanged by user direction |
| [E863 Final public attribution state and handoff](checkpoints/2026-10-27-e862-final-attribution-closeout-e863.md) | E863; fresh replay covers 2,475 reachable commits at E862's published tip with zero Claude/Anthropic hits; E863 commit `1c2088bf` separately exact-checked and pushed; stash unchanged |
| [E864 Reconcile E863 publication status in task register](checkpoints/2026-10-27-e863-task-register-closeout-e864.md) | E864; updates the stale task-register publication status using the exact-range check and fetch-verified E863 commit; docs-only |
| [E865 Reconcile APP-01 status and current attribution census](checkpoints/2026-10-27-app01-attribution-registry-closeout-e865.md) | E865; corrects stale launcher status rows; fresh 2,477-commit advertised-ref census has zero Claude/Anthropic attribution; stash unchanged |
| [E866 SAV-01 rejection path and public attribution](checkpoints/2026-10-27-sav01-rejection-path-and-attribution-e866.md) | E866; both bad-tag and bad-length edges converge on cleanup without an explicit destination restore in the loader; E865-head GitHub refs cover 2,478 commits with zero Claude attribution; private stashes unchanged by user choice |
| [E867 E866 publication and post-push attribution closeout](checkpoints/2026-10-27-e866-publication-closeout-e867.md) | E867 checkpoint `45af5f28` published, exact-range checked and fetch-verified; post-push contributors endpoint lists only DehydratedFlask |
| [E868 launcher readiness accessibility observation](checkpoints/2026-10-27-app01-voiceover-readiness-e868.md) | Runtime AX labels/details observed in the workspace bundle; focus traversal and spoken VoiceOver output remain NOT VERIFIED; no folder or Play interaction |
| [E869 VoiceOver caption/focus follow-up](checkpoints/2026-10-27-app01-voiceover-readiness-e869.md) | VoiceOver temporarily enabled and restored off; caption preference already on; CUA observed no caption text or focus movement; no folder or Play interaction |
| [E870 post-E869 public attribution census](checkpoints/2026-10-27-post-e869-public-attribution-census-e870.md) | 2,482 reachable commits across all 21 advertised refs scan free of Claude/Anthropic identity and credit lines; stash unchanged by user direction; E870 checkpoint exact-range checked and fetch-verified |
| [E871 E870 publication closeout and next-task triage](checkpoints/2026-10-27-e870-publication-closeout-e871.md) | Verifies E870 commit `c8d7fb5c` on live `main`; current contributor endpoint remains owner-only; no product task is READY |
| [E872 E871 publication closeout and next-task triage](checkpoints/2026-10-27-e871-publication-closeout-e872.md) | Verifies E871 commit `5e5d6bbf` on live `main`; contributor endpoint remains owner-only; no product task is READY |
| [E873 Post-E872 attribution census and APP-01 accessibility follow-up](checkpoints/2026-10-27-e873-post-e872-attribution-census.md) | Independent 2,485-commit public-ref census has zero Claude/Anthropic attribution; stash unchanged; Tab moves visible launcher focus, VoiceOver speech/caption remain unverified |
| [E874 E873 publication closeout and next-task triage](checkpoints/2026-10-27-e873-publish-closeout-e874.md) | Verifies E873 commit `72eb3f52` on live `main`; contributor endpoint remains owner-only; stash unchanged; no product task is READY |
| [E875 E874 publication closeout and current task triage](checkpoints/2026-10-27-e874-publish-closeout-e875.md) | Verifies E874 commit `017b6f31` on live `main`; contributor endpoint remains owner-only; stash unchanged; no product task is READY |
| [E876 APP-01 VoiceOver third-attempt claim](../HANDOFF.md) | Published the bounded third observation claim before shared task/evidence updates |
| [E877 APP-01 VoiceOver third-attempt escalation](checkpoints/2026-10-27-app01-voiceover-escalation-e877.md) | CUA confirmed VoiceOver on/off and caption preference, but could not verify actual cursor movement, speech, or desktop-wide caption output; task stopped at `NEEDS_PRINCIPAL` |
| [E878 E877 publication closeout](checkpoints/2026-10-27-e877-publication-closeout-e878.md) | Verifies E877 commit `7a5ae224` on live `main`; no product task is READY; G4 remains PARTIAL |
| [E879 slot-23 indirect-site structure checkpoint](checkpoints/2026-10-28-slot23-e70af0-indirect-structure-e879.md) | Root LLVM and independent Rizin agree on one indirect-call register path; reachability/semantics remain unknown; also records an out-of-scope CXML parser retry with no counts |
| [E981/E982 slot-23 caller reachability result and publication](checkpoints/2026-10-29-slot23-caller-reachability-e981.md) | Commit `2193f712` publishes the byte-verified relative dispatch table and conditional path to `0xe5c3f5`; caller/startup/runtime reachability remains unknown |
| [E983/E984 RackQuery successor block claim and publication](checkpoints/2026-10-29-aud02-rackquery-successor-94ecd6-claim.md) | Commit `e9827d71` claims the unexamined direct successor at `0x94ecd6`; inspect only through its first transfer; no Ghidra project or runtime claim |
| [E985/E986 RackQuery successor result and publication](checkpoints/2026-10-29-aud02-rackquery-successor-94ecd6-result-e985.md) | Owner-authored commit `d85b4bdf` records the byte-verified direct call `0x94ecd6 -> 0x99d8e0`, independent replay, and publication; analysis stops at that call |
| [E987/E988 RackQuery direct-call target first-block claim and publication](checkpoints/2026-10-29-aud02-rackquery-callee-99d8e0-claim-e987.md) | Owner-authored claim commit `d24ea5b7` is pushed and fetch-verified; inspect only the block at `0x99d8e0` through its first transfer with a 32-instruction / 128-byte cap |
| [E989/E990 RackQuery direct-call target result and publication](checkpoints/2026-10-29-aud02-rackquery-callee-99d8e0-result-e989.md) | Commit `e01c3f66` records Rizin, LLVM and PT_LOAD agreement on `mov eax, 0x8190; ret` at `0x99d8e0`; analysis stops at the return |
| [E991/E992 RackQuery caller continuation claim and publication](checkpoints/2026-10-29-aud02-rackquery-caller-continuation-94ecdb-claim-e991.md) | Owner-authored commit `08af7e78` is pushed and fetch-verified; inspect only the caller block at `0x94ecdb` through its first transfer, capped at 32 instructions / 128 bytes |
| [E993 RackQuery caller continuation result](checkpoints/2026-10-29-aud02-rackquery-caller-continuation-result-e993.md) | Rizin, LLVM and PT_LOAD bytes agree on four instructions ending in `call 0x959620`; the static slice stops at that call |
| [E994 RackQuery caller continuation publication closeout](checkpoints/2026-10-29-aud02-rackquery-caller-continuation-publication-e994.md) | Verifies E993 result commit `e7360550` on live `main`; one static block only, no product gate advanced |
| [E995 Slot-23 direct-call target entry-block claim](checkpoints/2026-10-29-cpu01-slot23-direct-callee-e70a60-claim-e995.md) | Claims only the entry block at conditional direct-call target `0xe70a60`; stop at first transfer; slot 23 remains unregistered |
| [E996 SAV-01 load-dispatch target-origin claim](checkpoints/2026-10-29-sav01-load-dispatch-5435fc-claim-e996.md) | Claims only the target origin for indirect call `0x5435fc`; stop at target entry or unresolved provenance; no real-save claim |
| [E997 Slot-23 direct-callee provider-timeout result](checkpoints/2026-10-29-cpu01-slot23-direct-callee-e70a60-provider-e997.md) | Records the E995 Ghidra startup timeout; no instruction bytes returned; task blocked, slot 23 unregistered |
| [E998 SAV-01 load-dispatch provider-timeout result](checkpoints/2026-10-29-sav01-load-dispatch-5435fc-provider-e998.md) | Records the E996 REA timeout and incomplete headless fallback; no target bytes; Ghidra project index metadata changed during fallback |
| [E999 APP-01 importer cancellation-state fixture claim](checkpoints/2026-10-29-app01-importer-cancel-state-claim-e999.md) | Claims a testable LauncherCore result/state handler wired to ContentView; cancellation preserves prior directory/readiness; no native picker interaction |
| [E1000 APP-01 importer cancellation-state fixture result](checkpoints/2026-10-29-app01-importer-cancel-state-result-e1000.md) | Reducer and model handler preserve prior directory/readiness on cancellation; root focused 5/5 and full launcher 160/21 pass; G4 remains partial |
| [E1001 E1000 publication closeout](checkpoints/2026-10-29-app01-importer-cancel-state-publication-e1001.md) | Fetch-verifies c0891f04 on main, audits pass, project index current, Trail query proves the new handler is indexed; G4 remains partial |
| [E1002 RackQuery target byte-slice claim](checkpoints/2026-10-29-aud02-rackquery-callee-959620-claim-e1002.md) | Claims only the entry block at E993's direct target `0x959620`; stop at first transfer/gap; no transfer-target, API, runtime or audio claim |
| [E1003 RackQuery target byte-slice result](checkpoints/2026-10-29-aud02-rackquery-callee-959620-result-e1003.md) | Rizin/LLVM/PT_LOAD agree on a preliminary 11-instruction slice, but required Ghidra read timed out without bytes/Evidence; task remains blocked and no shipped-code claim is made |
| [E1004 APP-01 current launcher bundle rebuild claim](checkpoints/2026-10-29-app01-current-bundle-rebuild-claim-e1004.md) | Claims a fresh private release bundle from current source and bounded artifact checks; excludes Finder launch, game readiness, and G4 completion |
| [E1005 APP-01 current launcher bundle artifact result](checkpoints/2026-10-29-app01-current-bundle-rebuild-result-e1005.md) | Current Release launcher/core bundle is artifact-verified as thin ARM64, signed, system-linked, and free of forbidden game files; engine startup and G4 remain incomplete |
| [E1006 AUD-02 SXDF/SXDS ELF candidate-scan claim](checkpoints/2026-10-29-aud02-sxdf-raw-elf-candidate-scan-claim-e1006.md) | Claims a hash-pinned raw-tag census and bounded dual-disassembler candidate check; no codec, reader, audio-use, or shipped-code claim |
| [E1007 AUD-02 SXDF/SXDS ELF candidate-scan result](checkpoints/2026-10-29-aud02-sxdf-raw-elf-candidate-scan-result-e1007.md) | Eight raw SXDF hits classified as six instruction immediates, one unresolved overlap, and one string-like data occurrence; no reader or audio semantics claimed |
| [E1008 AUD-01 SXDF music sample-header correlation claim](checkpoints/2026-10-29-aud01-sxdf-music-sample-header-correlation-claim-e1008.md) | Claims live archive/member hash verification and an in-memory 48-byte header comparison; no extraction or codec/audio-use claim |
| [E1009 AUD-01 SXDF music sample-header correlation result](checkpoints/2026-10-29-aud01-sxdf-music-sample-header-correlation-result-e1009.md) | Corrected archive/member hashes and TOC identity pass; four-byte SXDF tag matches, full 48-byte sequences differ; no decoder/audio-use claim |
| [E1010 AUD-02 SXDF code-to-data reference claim](checkpoints/2026-10-29-aud02-sxdf-data-reference-ghidra-claim-e1010.md) | Claims an exact program identity check and two bounded REA queries at the candidate instruction and data addresses; no reader/audio/runtime claim |
| [E1011 E1010 claim publication closeout](checkpoints/2026-10-29-aud02-sxdf-data-reference-ghidra-publication-e1011.md) | E1010 claim commit `e8cf85ba` is pushed and fetch-verified; claim-only, no REA analysis query yet |
| [E1012 SXDF REA instruction query result](checkpoints/2026-10-29-aud02-sxdf-data-reference-ghidra-result-e1012.md) | First instruction query timed out at 330,000 ms; no bytes or Evidence ID; no retry or second query |
| [E1013 SXDF provider-status recheck](checkpoints/2026-10-29-aud02-sxdf-provider-status-recheck-e1013.md) | Ghidra provider remains open, but its status snapshot still exposes no image base; no new code query or gate advance |
| [E1014 E1013 publication closeout](checkpoints/2026-10-29-aud02-sxdf-provider-publication-e1014.md) | E1013 owner-authored commit pushed and fetch/live-ref verified; analysis task and product gates remain unchanged |
| [E880 E879 publication closeout](checkpoints/2026-10-28-e879-publication-closeout-e880.md) | Verifies owner-authored E879 on live `main`, the current contributors endpoint, and unchanged private stash; no product gate advanced |
| [E881 E880 publication closeout and save-path feasibility](checkpoints/2026-10-28-e880-publication-closeout-e881.md) | Verifies E880 on live `main`; one typed Ghidra xref query timed out without a result, so no save task or gate was advanced |
| [E882 E881 publication closeout and APP-01 reassessment](checkpoints/2026-10-28-e881-publication-closeout-e882.md) | Verifies E881 on live `main`; one principal-authorized VoiceOver follow-up is waiting for manual unlock, with no fourth attempt started |
| [E883 E882 attribution census and closeout](checkpoints/2026-10-28-e882-attribution-census-e883.md) | Refreshes the live advertised-ref scan at E882 with zero Claude/Anthropic matches; stash remains unchanged by owner direction; APP-01 still waits for manual unlock |
| [E884 E883 publication closeout](checkpoints/2026-10-28-e883-publish-closeout-e884.md) | Verifies E883 on live `main`; a fresh 2,496-commit census still finds zero Claude/Anthropic attribution; stash and APP-01 unlock boundary remain unchanged |
| [E885 E884 publication closeout and current attribution census](checkpoints/2026-10-28-e884-publish-closeout-e885.md) | Verifies E884 and refreshes the 2,497-commit census; stash unchanged; APP-01 still waits for manual unlock; E885 publication is recorded separately after push |
| [E886 E885 publication closeout and current attribution census](checkpoints/2026-10-28-e885-publish-closeout-e886.md) | Verifies E885 on live main; 2,498 reachable commits scan free of Claude/Anthropic matches; stash unchanged; no product task is READY |
| [E887 E886 publication closeout and current attribution census](checkpoints/2026-10-28-e886-publish-closeout-e887.md) | Verifies E886 on live main; 2,499 reachable commits scan free of Claude/Anthropic matches; stash unchanged; no product task is READY |
| [E888 E887 publication closeout and current attribution census](checkpoints/2026-10-28-e887-publish-closeout-e888.md) | Verifies E887 on live main; 2,500 reachable commits scan free of Claude/Anthropic matches; stash unchanged; APP-01 still waits for manual unlock |
| [E889 E888 publication closeout and current attribution census](checkpoints/2026-10-28-e888-publish-closeout-e889.md) | Corrects stale E888 publication-pending handoff; verifies owner-authored E888 on live main, a fresh 2,501-commit Claude-free census, owner-only GitHub contributors, and unchanged stash; APP-01 still waits for manual unlock |
| [E890 APP-01 native core coverage count reconciliation](checkpoints/2026-10-28-app01-core-coverage-reconcile-e890.md) | Current native `SoCGameCore --coverage` reports 397/573 with first gap 309; E865/E868 counts remain historical artifact snapshots; G1/G4 PARTIAL |
| [E891 E890 publication closeout and attribution census](checkpoints/2026-10-28-e890-publish-closeout-e891.md) | Verifies E890 on live main; fresh 2,504-commit non-stash scan finds no Claude/Anthropic identities or credit trailers; contributors lists only DehydratedFlask; stash unchanged |
| [E892 E891 publication closeout and attribution census](checkpoints/2026-10-28-e891-publish-closeout-e892.md) | Verifies E891 on live main; fresh 2,505-commit non-stash scan finds no Claude/Anthropic identities or credit signatures; contributors lists only DehydratedFlask; stash unchanged |
| [E893 E892 live-state reconciliation](checkpoints/2026-10-28-e892-current-state-reconcile-e893.md) | Rechecks all 21 advertised branch/PR refs across 2,506 reachable commits; Claude/Anthropic-free; E892 heading corrected; task register has no READY row and APP-01 remains gated by locked Mac |
| [E894 slot-309 raw-literal census](checkpoints/2026-10-28-slot309-raw-literal-census-e894.md) | Full-file raw u32/u64 scan finds only 24-byte relocation-shaped metadata rows; no consumer or native-lift boundary, so slot 309 remains unregistered |
| [E895 E894 publication closeout](checkpoints/2026-10-28-e894-publish-closeout-e895.md) | Verifies E894 on live main and scans 2,508 commits across all 21 advertised branch/PR refs with no Claude/Anthropic identities or credit; stash unchanged |
| [APP-01 VoiceOver escalation](escalations/APP-01-VOICEOVER-READINESS-02.md) | Three prior observations remain inconclusive; a principal-authorized fourth is waiting for Mac unlock and a new observable surface |
