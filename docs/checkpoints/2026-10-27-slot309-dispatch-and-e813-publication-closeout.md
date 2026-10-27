# Slot-309 route follow-up and E813 publication closeout

- **Date:** 2026-10-27
- **Tasks:** close out `DOC-INDEX-LAUNCHER-BUILD-CHURN-01`; continue `CPU-01-STARTUP-SLOT-309-CHARACTERIZATION-01` with a bounded, read-only route check.
- **E813 commit:** `442e59186a42f70501b7ae534ab4ec8cfecb996d`, parent `d72870a319d58710152e93144f512a56cd0b20c7`; author and committer Johnny Wang `<dehydratedflask@users.noreply.github.com>`.

## E813 publication

`git push origin main` exited 0 as a fast-forward. `git fetch origin main` exited 0 and confirmed `HEAD == origin/main == FETCH_HEAD == 442e59186a42f70501b7ae534ab4ec8cfecb996d`. Post-commit exact-range attribution and identity checks passed; the committed-tree path audit passed **3,034/3,034** and `git diff-tree --check HEAD^ HEAD` passed. The worktree is clean at this closeout.

A fresh scan of live GitHub refs parsed 9 branch heads and 12 pull-request heads, no tags, 15 unique tips and **2,428 reachable commits**. It found zero Claude/Anthropic author/committer identity matches and zero co-author/generated-by credit-line matches. The owner explicitly left private `refs/stash` unchanged; the 70 stash-only credit matches remain outside the public cleanup scope.

## E814 bounded CPU-01 static follow-up

The subagent used Ghidra 12.1.4 against the saved project with `-noanalysis -readOnly` and the pinned ELF SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. Its private report is `local/validation/orch-20261027/slot309/agent-slot309-dispatch/REPORT.md`, SHA-256 `4d0b7e5d23d5f211346ac563e2359a0b6f5b864ed358b926d3e93199cd4db941`. A second agent reviewed the report and approved its evidence/limitations; root verified the report hash and every cited log hash.

**Observed:** the expanded reference query found zero *analyzed incoming references* to each aligned cell in `0x15d5b60..0x15d5bc8`. The candidate table has outgoing pointers to strings, wrappers and methods. On one selected direct path, `0x531d60` calls `0x255130`, which reaches `0x255180 → 0x264e50`; that constructor writes vptr `0x159c958`. This path therefore does not install the queried `0x15d5b80` table. The table targets at `0x12cfea5` and `0x12cfeab` resolve to `Flag3`/`Flag4` string bytes.

**Inference and limit:** this narrows the selected direct path only. Zero analyzed refs cannot exclude indirect, data-driven, unmodeled or external dispatch. The producer/route for `0x15d5b80`, ordering of `0x240f0` cleanup callers, manager state at slot 309, and the general `+0x10` temporary ownership contract remain unresolved. Keep slot 309 unregistered and retain the hook boundary. G1 remains PARTIAL; no game code ran and no boot/audio/save/Metal acceptance gate advances.

Exact Ghidra commands, outputs, and hashes are in the private report. Verified log hashes:

- `expanded-refs.stdout.log`: `f902c9db9383ed33171288a8b6c5c2a21e437008faf59fd9f81669527ad8f138`
- `table-methods.stdout.log`: `863a16b5f187996980071603ebdec71a6567c486e20d2d12b05f08bad873ea18`
- `caller-chain.stdout.log`: `5de83fa2f4f9e5ae3907f362e4d4c29c0c465f279d5e7fda17a2f14ade371937`
- `new-object-constructors.stdout.log`: `5e8fceaf29635d0bd9214f903933e425f32dd2fd8505cf8388d05bdb0b6db61a`
- `table-entry-code-rerun.objdump.log`: `20e6818ae885d0661832203b460e46cf08736ebc426e0e26204a1abb10353634`

The only errors were tooling-only: an unavailable `/usr/bin/llvm-readelf` path and a zsh read-only `status` variable in a wrapper; corrected reruns succeeded. No semantic hypothesis failed.

## Repository gates and next action

The source/test changes were verified in E813: project-tools tests **13/13**, CMake configure/build exit 0, CTest **431/431**, lift sweep **516/0/0**, Trail wiring graph **9,877 nodes / 13,470 edges**, strict project index **31,921 folders**, staged/full path audit **8/8** and **3,034/3,034**. E814 document-closeout replay: project-tools tests **13/13**, CMake configure/build exited 0, CTest **431/431**, lift **516/0/0**, Trail graph check passed at **9,877 nodes / 13,470 edges**, and strict project index generation/check passed at **31,922 folders**. Staged/full E814 path audits passed **6/6** and **3,035/3,035**; precommit attribution, identity, diff, and project-index checks passed. Post-commit verification follows after commit. Logs are in `local/validation/orch-20261027/e814-closeout/`.

**Next exact action:** inspect each `0x240f0` caller's order relative to constructor-list dispatch and search for an explicit `0x15d5b80` producer/relocation/data-driven use. Do not register slot 309 or claim a native-lift boundary without that support.
