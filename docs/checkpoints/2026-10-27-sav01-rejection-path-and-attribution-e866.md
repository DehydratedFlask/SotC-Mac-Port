# E866 — SAV-01 rejection path and public attribution checkpoint

- Local date/time: 2026-10-27 20:07 CDT
- Agent: `/root`
- Tasks: `SAV-01-LOAD-PARTIAL-MUTATION-01`; `PUB-01-CLAUDE-CREDIT-CLEANUP-01`; `PUB-01-LOCAL-REF-ATTRIBUTION-CENSUS-01`
- Status: static path claim verified; E866 published in `fc97d6fd55e5ff02a08ccc47ecb08f5592cf6db5`; exact-range, push/fetch, and post-push attribution closeout are recorded in E867.

## Public commit attribution

At E865, `HEAD`, `origin/main`, and the live advertised `main` matched `2ca22f18d910677e5c5d37e6bc5222a134785554`. A fresh read-only scan covered 37 named non-stash refs and all 21 currently advertised GitHub refs (21 combined unique tips / 2,478 reachable commits). It found zero Claude/Anthropic author or committer identities, zero Claude/Anthropic credit lines, and zero assistant credit lines. GitHub's live contributors endpoint returned only `DehydratedFlask`.

Four assistant-alias identity headers remain on two historical non-Claude commits (`f6604a86e60e7fd18cb15548362b8f134e433f5d` and `d614808bbdf732b5a3ebcf26b6d4c2a38f8d93b2`), each using `orchestrator@local`. The user's explicit choice to leave private stashes unchanged remains in force: the read-only stash scan saw 21 reflog tips / 826 reachable commits / 70 Claude credit lines, and `refs/stash` stayed at `92ceba574bd1e8445b9aa4f728f878fbfd4f57f8`. The public claim excludes private stashes and unreachable objects.

Commands:

```sh
git fetch origin
python3 -I scripts/check_commit_attribution.py
python3 -I local/validation/orch-20261027/e866/attribution/scan_all_attribution_roots_e866.py
gh api repos/DehydratedFlask/SotC-Mac-Port/contributors --paginate --jq '.[].login'
```

All commands exited 0. The exact checker had an empty `origin/main..HEAD` range at the time. Scanner code, full report, and logs remain private under `local/validation/orch-20261027/e866/attribution/`; the contributors response is `local/validation/orch-20261027/e866-github-contributors.txt`.

Prepublication private artifact hashes: scanner `3453d30b01de2ed72c1ededfe0198c1be1a5df40fd81859a515804e0e2772489`; saved report `259cc04dda8a65f80e1828d9fe112236c39aa055b2553ac42c428021c1e6024a`; scanner log `9a95c4427e6e1d14f0928798487e158f25d0d52acf239756a95154b87128eb79`; GitHub contributors response `c26494de7a2b2ed933b58e8e5f3d580c5b2d8658b9f42eceeaae171b0004eeb9`.

## Bounded save-loader result

The existing q2 REA/Ghidra evidence is `ev_8f5eed1f8c9e216017843e816dfff6c4ff7d0ebc2f53faa13642d0d9627bc9b0`, Ghidra 12.1.4, analysis profile `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`, bound to `local/re/eboot.elf` SHA-256 `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`. The function body is 4,886 bytes across three non-contiguous ranges with 949 assembly rows. Relationship coverage is partial; indirect calls at `0x5435fc` and `0x543611` have unresolved targets, and there are no resolved callers. No new provider query was made.

The function prologue sets `R13` from the destination parameter. Existing q2 plus root replay against the pinned ELF show the GameData tag rejection at `0x543681` and expected-size rejection at `0x5436a4` both branch to `0x5437a1`. That cleanup block only passes stack-local state and a global pointer to its calls, sets `EBX=6`, and jumps to common cleanup at `0x543b64`; the common return moves that value to `EAX`. It contains no explicit store through `R13` and does not pass the destination pointer to a cleanup call. Earlier direct destination writes occur at `0x5428ff`, `0x54299e`, `0x542a02`, `0x542a5f`, `0x542aac`, `0x542b48`, `0x542b8d`, `0x542c29`, `0x542c73`, and `0x542d0b`. `R13` is rebound to a stack-local reader only on the success edge at `0x5436aa`.

**Claim:** `STATIC_ANALYSIS_VERIFIED` that `FUN_00542880` has no explicit destination restoration on either of these rejection paths. This does not prove object-level atomicity or runtime behavior: callee/global side effects and caller-level discard or rollback are unresolved, and the external meaning of result `6` is not asserted.

Root commands and results:

- `shasum -a 256 -c local/re/evidence/sav01-loader-01/SHA256SUMS` — exit 0.
- `xcrun llvm-objdump --version` — Apple LLVM 21.0.0.
- `xcrun llvm-objdump -d --x86-asm-syntax=intel --start-address=0x542880 --stop-address=0x543b96 local/re/eboot.elf` — exit 0; private output SHA-256 `981ae4507b88e62c913b087acfa9b43b6c01f4a18429fe5d2bc6927fa7ad8b56` at `local/validation/orch-20261027/e866/sav01-loader-542880-543b96.llvm-objdump.txt`.
- `xcrun llvm-objdump -p local/re/eboot.elf` — exit 0; `PT_LOAD#0` has virtual address 0 (image base 0); private log SHA-256 `e60358da562562144958e1704b9777f039fcf6de0c0b0d1faea692b53ed4831b`.
- `xcrun llvm-readobj --file-headers --program-headers local/re/eboot.elf` — exit 72 because that utility is absent from the installed Xcode toolchain. The available `llvm-objdump -p` supplied the needed segment map; no dependency was installed.

## Files, checks, and limits

Allowlisted source changes are documentation only: `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `docs/INDEX.md`, generated `docs/FOLDERS.md`, and this checkpoint. No native or launcher source, original game input, private evidence payload, or Git ref was changed. `INSTALL_LOG.md` is unchanged because no tool or dependency was installed.

Required prepublication gates completed:

| Command | Result |
|---|---|
| `cmake -S native -B local/build/orch-r22` | exit 0 |
| `cmake --build local/build/orch-r22 -j 12` | exit 0; Ninja had no work |
| `ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure` | exit 0; 431/431 passed |
| `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh` | exit 0; 516 passed / 0 skipped / 0 failed |
| `trail build --check` | exit 0; 9,877-node wiring graph current; deep meaning layer remains unbuilt |
| `python3 scripts/project_index.py` then `python3 scripts/project_index.py --check` | final run exit 0; 32,009 folders indexed/current. Two early checks, before all E866 evidence/log paths existed, reported stale `local/indexes/folders.json`; regeneration after paths were created resolved it. |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0; 3,082 currently tracked paths allowed (pre-stage) |
| `python3 -I scripts/check_commit_attribution.py` | exit 0 at E865 head; `origin/main..HEAD` empty before E866 commit |
| `python3 -I scripts/audit_publish_paths.py` | exit 0; all six staged paths allowed |
| `python3 -I scripts/audit_publish_paths.py --all` | exit 0; all 3,083 tracked paths allowed after staging |
| `git diff --check` and `git diff --cached --check` | exit 0 |

Logs are under `local/validation/orch-20261027/e866/`. E866 commit `fc97d6fd55e5ff02a08ccc47ecb08f5592cf6db5` was authored and committed by Johnny Wang, passed the exact-range attribution checker, was pushed to `origin main`, and fetch verification matched local, remote-tracking, and live advertised `main`. The post-E866 census covers 2,479 current advertised commits with zero Claude/Anthropic identities or credits; GitHub lists only `DehydratedFlask`; the user-excluded stash ref remains unchanged. E867 records that publication closeout. Overall game acceptance remains PARTIAL; no actual save/load or runtime behavior is claimed.
