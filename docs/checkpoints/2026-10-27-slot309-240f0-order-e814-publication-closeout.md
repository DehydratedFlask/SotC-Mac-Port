# Slot-309 cleanup order and E814 publication closeout

- **Date:** 2026-10-27
- **Tasks:** close out E814 publication; continue `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` with a bounded, read-only cleanup-caller ordering trace.
- **Starting state:** clean `main == origin/main == b0193c28f3d168b2008bf43a7056eed6b2da47a5`.

## E814 publication closeout

E814 commit `b0193c28f3d168b2008bf43a7056eed6b2da47a5` (parent `442e59186a42f70501b7ae534ab4ec8cfecb996d`) has owner author and committer. Post-commit exact-range attribution and owner identity passed; committed-tree path audit passed **3,035/3,035**, and `git diff-tree --check HEAD^ HEAD` passed. `git push origin main` and `git fetch origin main` exited 0; `HEAD == origin/main == FETCH_HEAD`. Fresh remote census found 9 branches, 12 PR heads, no tags, 15 unique tips and **2,429 reachable commits**, with zero Claude/Anthropic author/committer identities and zero co-author/generated-by credit lines. Private `refs/stash` was not inspected or changed.

## E817 bounded caller-order trace

The private report is `local/validation/orch-20261027/slot309/agent-240f0-order/REPORT.md`, SHA-256 `e94d5c47f8a4f3ad45cc85d0b79f06482bc2a262b629bc324cef50fda3e8042f`. Independent read-only review approved the wording and evidence; root verified the report and cited log hashes. The pinned ELF is `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. All new Ghidra queries used `-noanalysis -readOnly`; no game code ran.

**Observed:** saved-project references show direct call `0x242a0 → 0x240f0` at `0x242a9` and tail jump `0x254bf0 → 0x240f0` at `0x2550e9`, plus a vtable data pointer at `0x1594ec8`. `0x242a0` is in `0x1594ed0`; constructor `0x23d60` installs `0x1594ec8` and is called by `0x2534c0 → 0x536aa0 → 0x536730`, which the entry function reaches after its `0xf120 → 0x20` constructor-list dispatch. The same selected setup chain writes `0x159c578` and `0x15c8cd8` after that dispatch.

Other callers reach `0x254bf0` through `0x2550f0`, `0x537f90`, and `0xb11360/0xb11370` wrappers. Data table `0x15d5b80/0x15d5b88` points to the latter wrappers. The candidate table's constructor/dispatch order remains unresolved. Therefore the evidence establishes post-slot ordering only for the directly examined chains; it does not establish whole-program cleanup order or exclude indirect/data-driven pre-slot dispatch. Keep slot 309 unregistered and G1 PARTIAL. The general `+0x10` ownership contract and native-lift boundary remain unresolved.

Exact commands and evidence hashes are recorded in the private report. Key SHA-256 values independently checked:

- `incoming-refs.stdout.log`: `428b43268c6027f6b4cc9626aa4dad590cc228c3347b59ade6b29867fcf78767`
- `caller-bodies.stdout.log`: `da021bc348f4d3dfbaa78ac711966788cbe66b1bba8d86678ed0dfad2f5c3c04`
- `known-constructor-paths.stdout.log`: `44fb3293725d83e2ba4d944f5c379383769811a894248b198f4bc47cfeb7431a`
- `536850-536890.objdump.log`: `35cce643913b06905016b6300700f0d5d1d957eedb6d90bb0c1df12816b1c524`
- `b11350-b11390.objdump.log`: `5e7824cac1207735b58253b35d5754be86051c21c7b89dc7bb38de0f5ed24ac0`

## E816 verification and handoff

E816 document-closeout replay: project-tools tests **13/13**, CMake configure/build exit 0, CTest **431/431**, lift **516/0/0**, Trail graph **9,877 nodes / 13,470 edges**, strict project index **31,924 folders**, staged/full path audits **6/6** and **3,036/3,036**; precommit attribution, identity, diff, and project-index checks passed. Logs are under `local/validation/orch-20261027/e816-closeout/`. CTest log SHA-256 `d78ecd54ea59da5a081d2e6670728a5abf039689ee83ea25eb0b8d0e0a1186d7`; lift log SHA-256 `f6bd4048e58d79d1b452cdba2b9c54abb1ccac1b37de894eaae8ae7b626862db`.

**Next exact action:** seek a concrete constructor/dispatcher for the `0x15d5b80/88` wrapper table, compare it with slot 309 dispatch, then resolve the general stack `+0x10` ownership contract. If no supported route appears, retain the hook boundary and keep slot 309 unregistered.
