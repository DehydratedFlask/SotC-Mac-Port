# SoC Mac Native — agent handoff

Updated: 2026-10-19 20:07 CDT (America/Chicago). Principal: root agent of this planning chat.
No native game executable exists. G1 NOT PASSED.

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
