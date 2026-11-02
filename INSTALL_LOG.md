# Install log

Everything installed or downloaded on this Mac (Johnny's M4 Pro, macOS 27.0.1) for this project.

## 2026-10-18

### Homebrew formulae (`brew install`)

Full output: `logs/brew-install-2026-10-18.log`

| Package | Version |
|---|---|
| ghidra | 12.1.4 |
| ninja | 1.13.2 |
| rizin | 0.9.1_2 |
| molten-vk | 1.4.2 |
| vulkan-headers | 1.4.363.0 |
| vulkan-loader | 1.4.363.0 |
| spirv-cross | 1.4.363.0 |
| spirv-tools | 1.4.363.0 |
| glslang | 16.6.0 |
| ccache | 4.14.2 |
| unicorn | 2.1.4 |

Already present before this project: cmake 4.4.4, llvm 23.1.3, openjdk@25, python@3.14, sdl3 3.4.18, git-lfs, gh, ffmpeg, rust, go, node.

To remove everything above:

```bash
brew uninstall ghidra ninja rizin molten-vk vulkan-headers vulkan-loader spirv-cross spirv-tools glslang ccache unicorn
```

### Git clones (shallow, into `reference/`)

| Repo | Commit |
|---|---|
| https://github.com/AjncSystem/ShadowOfTheColossus-PC | 70bb43859cd7ff181a6d409540b757866184d60d |
| https://github.com/Supermedo/bloodborne_pc | de2e38af055f09f819946951075c6bb119e62ae6 |
| https://github.com/shadps4-emu/shadPS4 | 0ebea15c24253b60f514fc37a348dabc8d0bcb90 |

Submodules were not initialized.

### Installed by Johnny

- **Xcode 27.1** (build 27A9275), at `/Applications/Xcode.app`, and set as the active developer directory. Installed from the App Store on 2026-10-19.

## 2026-10-19 — roadmap / reverse-engineering session

### Verified existing tools (not installed again)

- `xcodebuild -version`: Xcode 27.1, build 27A9275. `xcrun --find metal`:
  `/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/metal`.
- Connected REA server `rea-agents` 6.7.0, skill version 42. Package under
  `/Users/dehydratedflask/.npm/_npx/a1722018f28f9df2/node_modules/rea-agents`.
- REA's Ghidra provider: 12.1.4 at
  `/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC`; native decompiler is Mach-O ARM64.
  Java 25.0.4.1 at `/opt/homebrew/Cellar/openjdk@25/25.0.4.1/libexec/openjdk.jdk/Contents/Home`.
- Existing global Graft 0.21.1 reused; no upgrade performed.
- Existing CMake, Ninja, Apple Clang, Python, Git, GitHub CLI, zlib SDK reused.

### Installed: Crypto++ 8.9.0

- Purpose: build the user's suggested standalone ShadPS4Plus PKG extractor natively on ARM64.
- Command: `HOMEBREW_NO_AUTO_UPDATE=1 brew install cryptopp`.
- Location: `/opt/homebrew/Cellar/cryptopp/8.9.0` (Homebrew reports 14.7 MB).
- Log: `logs/brew-cryptopp-2026-10-19.log`; no additional formula dependency installed.
- Removal if no longer used elsewhere: `brew uninstall cryptopp`.

### Downloaded: ShadPS4Plus standalone extractor source

- Upstream: https://github.com/AzaharPlus/shadPS4Plus
- Release: `PKG_EXTRACTOR_1_1`; commit `771a8d58aff540448c7cbace7c40a66a5daf010d`.
- Command: `git clone --depth 1 --branch PKG_EXTRACTOR_1_1 https://github.com/AzaharPlus/shadPS4Plus.git reference/shadPS4Plus-pkg`.
- Location: `reference/shadPS4Plus-pkg`; upstream unchanged, submodules not initialized.
- Reason: release assets are Windows/Linux; native macOS build uses the same extraction sources.
- Build adaptation: `tools/pkg-extractor/CMakeLists.txt` locates installed Crypto++, SDK zlib,
  and CoreFoundation instead of upstream's fixed build-tree static-library path.
- Build commands and resulting status are recorded in `docs/EXTRACTION.md`.
- Removal boundary: this newly added reference directory and `local/build/pkg-extractor` only.
  Keep original PKGs and any needed extracted analysis state.

The earlier installed Unicorn, MoltenVK and Vulkan packages are not part of the accepted final
runtime design. They were not removed because this session preserves existing installations.
No Sony SDK, firmware module, game download, Rosetta, VM, or emulator was installed by this session.

### Generated local analysis and verification outputs (not new dependencies)

- Standalone extractor: `local/build/pkg-extractor/pkg_extractor`, native ARM64. CMake adapter
  uses SDK zlib 1.2.12; does not require upstream's hardcoded cryptopp build path or ZLIB 1.3 check.
- Extracted base, update and four DLCs: `local/extracted/`; 124 files inventoried. Original
  packages are unchanged and all six SHA256 values are in `local/manifests/packages.json`.
- Reconstructed ELF and segment identities: `local/re/eboot.elf`, `local/re/eboot.identity.json`.
- REA/Ghidra entry dossier, portable view, snapshot and boot/function indexes: `local/re/`.
  Initial default 330s/2G startup timed out; process-local 1200s/8G CLI run passed. No permanent
  client configuration changed. Temporary Ghidra projects were removed by REA; no saved .gpr.
- Pinned SoTC preparation/inspection output: `local/re/pc-loader-reference/`, including
  analysis.json and x86 boot/entry blobs. These are prohibited production-runtime inputs.
- ATRAC9 decode probe: `local/audio/`; existing ARM64 FFmpeg reused, no decoder installed.
- Project scripts: `scripts/unwrap_self.py`, `inventory_game.py`, `project_index.py` and
  stdlib verification `test_project_tools.py`. No pip/npm package or test framework installed.

Private generated outputs can be regenerated from the documented inputs/recipes. Preserve
needed evidence before removing any `local/` subtree. No user PKG, image, metadata or iCloud
project was moved/deleted. No app was published, signed, notarized, or claimed game-ready.

### Source index refresh

The previous Graft fingerprint only included reference/. Expanded and persisted the scopes:
`graft build --follow-nested-repos --only-dir scripts --only-dir tools --only-dir reference`.
Result: 2,980 source files, 63,874 wiring nodes; four new project Python files parsed, 2,976
replayed. Log: `logs/graft-build.log`. This uses the existing 0.21.1 tool; no upgrade/deep LLM
pass/paid provider was used. Generated local cache only. Future native/launcher/tests folders
need explicit added scopes; do not accidentally replace the existing scope list.

## 2026-10-19 — RE-02/DATA-02 session (claude-sonnet-5-5)

No downloads or installs. Created stdlib-only helpers scripts/ps4_dynlink.py, psarc.py, disasm_range.sh (uses Xcode's llvm-objdump + clang assembler on dumped bytes in local/re/work; remove local/re/work to clean). Graft rebuilt: logs/graft-build-2.log.

## 2026-10-20 21:18 CDT — REA ownership-helper build blocked by sandbox (CPU-01-CTOR10-OBJECT-INIT-01)

Existing REA CLI 6.7.0/Ghidra12.1.4 offline function query at `0xb30950` spawned its Swift ownership-preparation helper and failed before target analysis (exit1, 4.9s): compiler could not write `/Users/dehydratedflask/.cache/clang/ModuleCache/Swift-7JL1KBZ3A6V3.swiftmodule`, then could not load arm64-macOS27 standard library. Exact command/JSON/hashes: `docs/checkpoints/2026-10-20-root-cpu01-ctor10-provider-cache-failure.md`; private captures `local/re/evidence/cpu01-ctor10-object-init-01/rea-b30950.*`. No tool was installed/upgraded and no dependency/config was replaced. Purpose: prepare offline Ghidra analysis ownership. Removal boundary: task-local private captures/caches; preserve shared installed tools/compiler cache. Principal authorized an access-scoped retry after checkpoint publication, preserving the original failed evidence. Compiler version was not separately recaptured for this failed helper preparation; do not treat it as a successful build or analysis.

## 2026-10-20 21:32 CDT — checksum standalone build and safe analysis stop

Existing Apple clang21.0.0 (`clang-2100.3.34.2`, arm64-apple-darwin27.0.0), Python3.15.0 and installed REA6.7.0/Ghidra12.1.4 were reused; no install/download/dependency change. Worker built the standalone WIP checksum test with `clang++ -std=c++20 -arch arm64 -DNDEBUG -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -Wall -Wextra -Wpedantic -I native native/core/save_checksum.cpp native/tests/test_save_checksum.cpp -o local/build/save-checksum-worker/test_save_checksum` (reported exit0); standalone run reported exit0, source checker exit0, deliberate prime mutant rejected exit1. Exact commands, logs/hashes and incomplete integration boundary are in `docs/checkpoints/2026-10-20-root-user-stop-checkpoint.md`. Root read logs but did not rerun/accept tests. Removal boundary: only generated task-local `local/build/save-checksum-worker/`; preserve published sources and needed evidence.

Scoped elevated b30950 retry succeeded (370727ms, exit0), addressing the earlier existing Swift-cache permission denial. The second distinct e44a10 offline query was interrupted by Ctrl-C at21:29:19 CDT on user stop; no task-owned REA/Ghidra child remained. Captures/snapshots remain under `local/re/evidence/cpu01-ctor10-object-init-01/`, including partial output; do not treat it as complete evidence. No installed tools/shared compiler cache were removed or replaced. No original game executable was run.

## 2026-10-20 21:48 CDT — checksum integration build and index refresh

Existing CMake4.4.4 and Apple clang21.0.0(clang-2100.3.34.2, arm64-apple-darwin27.0.0) reused. `cmake -S native -B local/build/native -DSOTC_SANITIZE=ON`, `cmake --build local/build/native -j 8`, and `ctest --test-dir local/build/native --output-on-failure` all exit0;223/223 pass20.46s,including checksum. Mach-O arm64 test; ASan+UBSan ON. No installation/download. Purpose:integrate standalone checksum fixture, not game runtime. Generated removal boundary:local/build/native only, preserve sources/evidence. Root loader verifier uses existing Apple LLVM21 offline byte-disassembly helper; no original-code execution/provider query. Logs/hashes/limits:[checkpoint](docs/checkpoints/2026-10-20-root-checksum-loader-complete.md).

`graft build` reused0.21.1 and persisted scopes, parsed4 new files/938 cached,2002 nodes. No upgrade/deep pass; reference scope is absent and remains a documented gap. Generated cache under graft/ stays private/regenerable.

## 2026-10-20 21:55 CDT — native pthread contract probe

Existing `/usr/bin/clang`21.0.0,macOS27.0.1/SDK27.0 reused. Command `/usr/bin/clang -std=c11 -arch arm64 -Wall -Wextra -O2 -pthread local/validation/cpu01-mutex-contract-01/probe.c -o local/validation/cpu01-mutex-contract-01/probe` exit0; worker/root run exit0. Purpose:host recursive/priority-inherit API configuration/lifetime; scheduling not tested. All environment/command/result hashes in private task directory; no install/download. Removal boundary:generated probe binary and private scratch only, preserve evidence as needed. Additional explicit all-scope Graft build still omitted reference sources; no source/tool configuration or pins were replaced. See checkpoint2026-10-20-root-mutex-contract-and-descriptor-claims.md.

## 2026-10-20 22:05 CDT — native recursive mutex owner build

Existing Apple clang21.0.0, SDK27.0, CMake4.4.4 reused; no install/download. Worker standalone build (exact commands in local/build/recursive-mutex-worker/commands.txt) uses C++20/arm64/-DNDEBUG/ASan+UBSan/-pthread. Root CMake configure and three targeted builds/tests passed; all3 focused tests pass, Mach-O arm64. Unsupported detect_leaks option failed before test body, corrected same binary passed; scratch include-path failure and corrected mutant rejection retained. Full details/log hashes/removal boundary:[checkpoint](docs/checkpoints/2026-10-20-root-native-mutex-and-persistent-claim.md). Build outputs stay under local/build/; preserve source/evidence. Persistent headless-analysis project is only CLAIMED, not yet executed at this checkpoint.

## 2026-10-20 22:22 CDT — persistent Ghidra baseline and stopping handoff

Existing Ghidra12.1.4 and OpenJDK25.0.4.1 reused; no install/download. Created only private `local/ghidra/persistent-20261020/`, project SotC/program `/eboot.elf`. Original pinned ELF imported with heap8G,max4 CPUs,analysis timeout1200s; exit0 in360.619s,analysis/save/import success logged. Read-only `-process eboot.elf -noanalysis -readOnly` reopen exit0 in2.997s. Exact argv, environment, timestamps, diagnostics and hashes:[next-agent checkpoint](docs/checkpoints/2026-10-20-root-next-agent-handoff.md) and private root persistent-run/reopen JSON/logs. Ordinary default headless profile is separate from REA; overlap/repair and unresolved-decompiler warnings retained. No original code executed.

Removal boundary: only this generated project directory and task-local logs if deliberately no longer needed; preserve them for the next agent now. Do not remove original inputs, existing tools/caches, references or other projects. All import/reopen sessions are terminal. Archived previous HANDOFF content under docs/checkpoints/2026-10-20-handoff-archive-through-1f7c8e8.md before replacing the current file with concise next-agent instructions. No durable memory/wiki update.

## 2026-10-21 13:14 CDT — round 57 focused fixture verification

Existing Apple Swift 6.4.0 (swift-driver 1.168.6), Apple clang 21.0.0, CMake 4.4.4, Python 3.15.0,
and macOS 27.0.1 were reused; no install, download, dependency, or configuration change. Commands:
`swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests`
(worker and root exit 0, 21 tests across 2 suites),
`PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_ps4_dynlink.py` (root exit 0, 22 tests),
`cmake --build local/build/native --target test_host_map` (worker/root exit 0), and
`ctest --test-dir local/build/native -R '^host_map$'` (worker/root exit 0, 1/1). The worker's Python
test output reported `OK`; its zsh wrapper returned 1 only after test completion when it attempted
to assign the read-only `status` variable. Detailed commands, hashes, and limitations: [APP-01
checkpoint](docs/checkpoints/2026-10-21-root-app01-core-readiness-query-failure-complete.md),
[RE-02 checkpoint](docs/checkpoints/2026-10-21-root-re02-dynlink-unmapped-symbol-type-complete.md),
and [SRV-01 checkpoint](docs/checkpoints/2026-10-21-root-srv01-host-map-roundup-overflow-complete.md).
Purpose: synthetic launcher mapping, ELF-symbol parser, and host-map overflow fixture coverage only;
no game execution/runtime evidence. Build products remain in `local/build/launcher` and
`local/build/native`; private test logs remain under `local/validation/` and `logs/`. No tool or
project dependency was installed.

## 2026-10-21 13:25 CDT — round 58 focused fixture verification

Existing Apple Swift 6.4.0, Python 3.15.0, CMake 4.4.4, Apple clang 21.0.0 and macOS 27.0.1 were
reused; no install, download, dependency, or configuration change. Worker and root commands:
swift test --package-path launcher --scratch-path local/build/launcher --filter LauncherStatusCopyTests
(exit 0, 7 tests in 1 suite),
PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_archive_overlap_map.py (exit 0, 22 tests),
cmake --build local/build/native --target test_region_list (exit 0), and
ctest --test-dir local/build/native -R '^region_list$' (exit 0, 1/1). Exact private logs and
hashes are recorded in the three Round 58 completion checkpoints. These commands verify synthetic
copy, archive-map validation, and intrusive-list behavior only; they add no game boot/audio/save or
Metal gameplay evidence. Build products remain under local/build/; validation output stays in
local/validation/. No dependency was installed.
## 2026-10-21 14:31 CDT — Round 63 focused fixture verification

Existing Apple Swift 6.4.0 (`swift-driver` 1.168.6), Xcode 27.1 (27A9275), CMake 4.4.4,
Apple clang 21.0.0, Python 3.15.0, and macOS 27.0.1 were reused; no install, download, dependency,
or configuration change. Commands from the repository root:
`swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests`
(root exit 0, 22 tests across 2 suites), `PYTHONDONTWRITEBYTECODE=1 python3 -I scripts/test_psarc.py`
(exit 0, 34 tests), `cmake --build local/build/native --target test_free_list_carve` (exit 0), and
`ctest --test-dir local/build/native -R '^free_list_carve$'` (exit 0, 1/1). Purpose: replay three
synthetic test fixtures, not game runtime behavior. Logs and source hashes are in
[the Round 63 completion checkpoint](docs/checkpoints/2026-10-21-root-round63-fixtures-complete.md).
Build products remain under `local/build/launcher` and `local/build/native`; validation logs remain
under `local/validation/root-round63-replay/`. Preserve evidence until no longer needed; no new
dependency or install removal boundary was created.

## 2026-10-21 14:37 CDT — Round 64 focused fixture baselines

Existing Apple Swift 6.4 / swift-driver 1.168.6, Xcode 27.1 (27A9275), CMake 4.4.4, Apple clang
21.0.0, Python 3.15.0, and macOS 27.0.1 were reused; no install, download, dependency, or
configuration change. Commands from the repository root:
`swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests`
(exit 0, 22 tests across 2 suites), `PYTHONDONTWRITEBYTECODE=1 python3 -I
scripts/test_archive_overlap_map.py` (exit 0, 22 tests), `cmake --build local/build/native --target
test_host_map` (exit 0), and `ctest --test-dir local/build/native -R '^host_map$'` (exit 0, 1/1).
Purpose: establish clean synthetic-fixture baselines before test-only edits; no game execution or
runtime behavior is established. Exact commands, output hashes, and removal boundaries:
`docs/checkpoints/2026-10-21-root-round64-fixture-claims.md`. Build products stay under
`local/build/`; baseline logs are under `local/validation/root-round64-scout/`. No dependency or
installation removal boundary was created.

## 2026-10-21 14:44 CDT — Round 64 fixture verification and source-index refresh

Existing Apple Swift 6.4 / swift-driver 1.168.6, Xcode 27.1 (27A9275), CMake 4.4.4, Apple clang
21.0.0, Python 3.15.0, macOS 27.0.1, and Graft CLI 0.21.1 were reused. No install, download,
dependency, or configuration replacement occurred. Root independently ran
`swift test --package-path launcher --scratch-path local/build/launcher --filter CoreClientTests`
(exit 0, 22 tests across 2 suites), `PYTHONDONTWRITEBYTECODE=1 python3 -I
scripts/test_archive_overlap_map.py` (exit 0, 23 tests), `cmake --build local/build/native --target
test_host_map` (exit 0), and `ctest --test-dir local/build/native -R '^host_map$'` (exit 0, 1/1).
The existing source graph was rebuilt with `trail build --follow-nested-repos --only-dir scripts
--only-dir tools --only-dir reference --only-dir native --only-dir launcher --only-dir tests`
(exit 0, 3,561 nodes / 4,813 edges / 1,236 indexed files) and verified with `trail build --check`
(exit 0). Project indexing via `python3 scripts/project_index.py` and `--check` exited 0; 14,441
folders were indexed. Exact replay logs and hashes are in
`docs/checkpoints/2026-10-21-root-round64-fixtures-complete.md`. Purpose: verify synthetic test
fixtures and source-index freshness only. Build products remain under `local/build/`; validation
logs remain under `local/validation/root-round64-replay/`; no new removal boundary was created.

## 2026-10-22 05:03 CDT — SRV-01 region-split no-carve-fit fixture verification

Existing CMake 4.4.4, Ninja 1.13.2, Apple clang 21.0.0 targeting arm64-apple-darwin27.0.0, and
macOS 27.0.1 were reused; no install, download, dependency, or system configuration change. Baseline,
worker, and independent root commands from the repository root:
`cmake -S native -B local/build/native-root-srv01-region-split-no-fit-baseline-20261022 -G Ninja
-DCMAKE_BUILD_TYPE=Debug`, `cmake --build
local/build/native-root-srv01-region-split-no-fit-baseline-20261022 --target test_region_split`,
and `ctest --test-dir local/build/native-root-srv01-region-split-no-fit-baseline-20261022
--output-on-failure -R '^region_split$'` (all exit 0; focused CTest 1/1);
`cmake -S native -B local/build/native-worker-srv01-region-split-no-fit-impl-20261022 -G Ninja
-DCMAKE_BUILD_TYPE=Debug`, followed by its matching build and `ctest --test-dir
local/build/native-worker-srv01-region-split-no-fit-impl-20261022 --output-on-failure -R
'^region_split$'` (worker exit 0; CTest 1/1); the exact worker build was `cmake --build
local/build/native-worker-srv01-region-split-no-fit-impl-20261022 --target test_region_split`; and
`cmake -S native -B
local/build/native-root-srv01-region-split-no-fit-verify-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`,
`cmake --build local/build/native-root-srv01-region-split-no-fit-verify-20261022 --target
test_region_split`, and `ctest --test-dir
local/build/native-root-srv01-region-split-no-fit-verify-20261022 --output-on-failure -R
'^region_split$'` (root exit 0; CTest 1/1). Detailed logs, toolchain, source hashes, and result scope
are in E335/E337/E338 and `docs/checkpoints/2026-10-22-root-srv01-region-split-no-carve-fit-complete.md`.
Purpose: baseline and verify one synthetic wrapper no-fit/node-preservation fixture only; no caller
reachability or game runtime behavior is demonstrated. Build products remain in the three
`local/build/` directories above; validation logs remain under
`local/validation/20261022-root-fixture-baselines/`. No removal boundary was created.

## 2026-10-22 05:43 CDT — SRV-01 host-map requested-size fixture verification

Existing CMake 4.4.4, Ninja 1.13.2, Apple clang 21.0.0 targeting arm64-apple-darwin27.0.0, and
macOS 27.0.1 from the E325 run were reused; no install, download, dependency, or system
configuration change. From the repository root, `cmake -S native -B
local/build/native-root-srv01-map-size-verify-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`,
`cmake --build local/build/native-root-srv01-map-size-verify-20261022 --target test_host_map`, and
`ctest --test-dir local/build/native-root-srv01-map-size-verify-20261022 --output-on-failure -R
'^host_map$'` all exited 0; the focused CTest passed 1/1. Purpose: verify one synthetic request where
allocator-region size `0x200001` remains distinct from the page-rounded host mapping length; this
does not prove allocator reachability or game behavior. Build products remain under
`local/build/native-root-srv01-map-size-verify-20261022/`; logs are under
`local/validation/srv01-host-map-requested-vs-rounded/`. No removal boundary was created.

## 2026-10-22 05:50 CDT — SAV-01 save-stream directory errno fixture verification

Existing CMake 4.4.4, Ninja 1.13.2, Apple clang 21.0.0 targeting arm64-apple-darwin27.0.0, and
macOS 27.0.1 previously recorded in E325 were reused; no install, download, dependency, or system
configuration change. From the repository root, `cmake -S native -B
local/build/native-root-sav01-dir-errno-verify-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`,
`cmake --build local/build/native-root-sav01-dir-errno-verify-20261022 --target
test_save_stream_file`, and `ctest --test-dir
local/build/native-root-sav01-dir-errno-verify-20261022 --output-on-failure -R
'^save_stream_file$'` all exited 0; the focused CTest passed 1/1. Purpose: pin synthetic directory-read
`os_errno == EISDIR` reporting; no real-save compatibility or game behavior is established. Build
products remain under `local/build/native-root-sav01-dir-errno-verify-20261022/`; logs are under
`local/validation/sav01-native-stream-file-directory-errno/`. No removal boundary was created.

## 2026-10-22 06:05 CDT — APP-01 Metal readiness fixture verification

Existing Swift 6.4.0.34.1 and Xcode 27.1 (27A9275), captured in
`local/validation/app01-metal-readiness/swift-toolchain.log`, were reused; no install, download,
dependency, or system configuration change. From the repository root, worker command
`swift test --package-path launcher --scratch-path local/build/native-root-app01-metal-readiness-20261022 --filter LauncherModelValidationTests`
and independent root command
`swift test --package-path launcher --scratch-path local/build/native-root-app01-metal-readiness-root-20261022 --filter LauncherModelValidationTests`
both exited 0 with 9 tests passing. Logs are `local/validation/app01-metal-readiness/swift-test.log`
and `root-replay.log`; source and toolchain hashes are recorded in E352. Purpose: verify a synthetic
missing-Metal readiness gate. Build products remain under the two exact `local/build/` directories
named in the commands. No removal boundary was created.

## 2026-10-22 06:05 CDT — SRV-01 exact-fit write-boundary fixture verification

Existing CMake 4.4.4, Ninja 1.13.2, Apple clang 21.0.0 targeting arm64-apple-darwin27.0.0, and
macOS 27.0.1 recorded in E325 were reused; no install, download, dependency, or system configuration
change. The worker ran `cmake -S native -B local/build/native-root-srv01-exact-fit-write-boundary-20261022 -G Ninja -DCMAKE_BUILD_TYPE=Debug`, `cmake --build local/build/native-root-srv01-exact-fit-write-boundary-20261022 --target test_exact_fit_finalize`, and `ctest --test-dir local/build/native-root-srv01-exact-fit-write-boundary-20261022 --output-on-failure -R '^exact_fit_finalize$'`. The independent root replay used the same commands with build directory `local/build/native-root-srv01-exact-fit-write-boundary-root-20261022`. All commands exited 0; focused CTest passed 1/1 in each run. Worker logs are under `local/validation/srv01-exact-fit-write-boundary/`; independent root replay logs are under its `root-replay/` subdirectory. Purpose: verify a synthetic `RegionNode` write boundary only. Build products remain under the two exact `local/build/` directories named above. No removal boundary was created.

## 2026-10-22 06:13 CDT — CPU-01 lazy-tree failed-release fixture verification

Existing CMake, Ninja, and Apple clang recorded in E325 were reused; no install, download,
dependency, or system configuration change. Worker and independent root each ran Debug/Ninja
configure, `test_lazy_param_def_tree` build, and focused `lazy_param_def_tree` CTest 1/1. Worker and
root build directories are `local/build/native-worker-cpu01-lazy-release-preserve-20261022/` and
`local/build/native-root-cpu01-lazy-release-preserve-20261022/`; logs are under
`local/validation/cpu01-lazy-release-preserve-20261022/`. Purpose: verify synthetic cache retention
after `release()` fails to lock a closed mutex. Build products remain in those directories; no
removal boundary was created.

## 2026-10-22 06:23 CDT — SAV-01 bad-magic zeroed-fields fixture verification

Existing CMake, Ninja, and Apple clang recorded in E325 were reused; no install, download,
dependency, or system configuration change. Worker and independent root each ran Debug/Ninja
configure, `test_save_chunk_header` build, and focused `save_chunk_header` CTest 1/1. Build
directories are `local/build/native-worker-sav01-chunk-header-bad-magic-zeroed-fields-20261022/`
and `local/build/native-root-sav01-chunk-header-bad-magic-zeroed-fields-20261022/`; logs are under
`local/validation/sav01-chunk-header-bad-magic-zeroed-fields-20261022/`. Purpose: verify zeroed
decoded header fields on a synthetic wrong-magic parse error. No real-save compatibility or game
save/load behavior is established. Build products remain in those directories; no removal boundary
was created.

## 2026-10-22 06:36 CDT — CPU-01 normal-threshold infinity fixture verification

Existing CMake, Ninja, and Apple clang recorded in E325 were reused; no install, download,
dependency, or system configuration change. Worker and independent root each ran Debug/Ninja
configure, `test_normal_threshold` build, and focused `normal_threshold` CTest 1/1. Build directories
are `local/build/native-worker-cpu01-normal-threshold-infinity-20261022/` and
`local/build/native-root-cpu01-normal-threshold-infinity-20261022/`; logs are under
`local/validation/cpu01-normal-threshold-infinity-20261022/`. Purpose: verify positive- and
negative-infinity comparisons in the synthetic normal-threshold helper. Build products remain in
those directories; no removal boundary was created. No game reachability or gameplay behavior is
established.

## 2026-10-22 06:48 CDT — SRV-01 zero-request peak-update fixture verification

Existing CMake 4.4.4, Ninja 1.13.2, Apple clang 21.0.0 targeting arm64, and macOS 27.0.1
recorded in E325 were reused; no install, download, dependency, or system configuration change. The
worker ran `cmake -S native -B local/build/native-worker-srv01-f19e00-zero-request-20261022 -G
Ninja -DCMAKE_BUILD_TYPE=Debug`, `cmake --build
local/build/native-worker-srv01-f19e00-zero-request-20261022 --target test_arena_accounting`, and
`ctest --test-dir local/build/native-worker-srv01-f19e00-zero-request-20261022
--output-on-failure -R '^arena_accounting$'`. Independent root replay used the corresponding
`local/build/native-root-srv01-f19e00-zero-request-20261022/` directory. All commands exited 0; each
focused CTest passed 1/1. Purpose: verify a synthetic zero-request high-water repair helper case;
the byte-preservation fixture checks the prefix and suffix around the raw span but not every interior
gap. No allocator reachability or game behavior is established. Build products remain under the two
exact `local/build/` directories named above; logs are under
`local/validation/srv01-f19e00-zero-request-20261022/`. No removal boundary was created.
## 2026-10-22 07:00 CDT — SAV-01 reader fixture verification

Existing CMake, Ninja, Apple clang, and macOS toolchain versions recorded in E325 were reused; no
install, download, dependency, or system configuration change. For
`SAV-01-READER-DUPLICATE-NAME-CROSS-TAG-FIXTURE-01`, worker and independent root each ran Debug/Ninja
configure, built `test_save_stream`, and passed focused `save_stream` CTest 1/1. For
`SAV-01-CHUNK-CODEC-READER-BORROWED-BYTES-VIEW-FIXTURE-01`, worker and independent root each ran
Debug/Ninja configure, built `test_save_chunk_codec`, and passed focused `save_chunk_codec` CTest
1/1. Exact commands and log directories are recorded in their completion checkpoints and E372/E373;
worker/root build products remain in the corresponding `local/build/native-worker-*` and
`local/build/native-root-*` directories, with logs under
`local/validation/sav01-reader-duplicate-cross-tag-20261022/` and
`local/validation/sav01-reader-borrowed-bytes-20261022/`. These runs verify synthetic reader policy
and borrowed-view behavior only; no real-save compatibility or game behavior is established. No
removal boundary was created.
## 2026-10-22 07:09 CDT — SAV-01 symlink-source reader fixture verification

Existing CMake, Ninja, Apple clang, and macOS versions recorded in E325 were reused; no install,
download, dependency, or system configuration change. Worker ran:
`cmake -S native -B local/build/native-worker-sav01-stream-symlink-source-read-20261022 -G Ninja
-DCMAKE_BUILD_TYPE=Debug`;
`cmake --build local/build/native-worker-sav01-stream-symlink-source-read-20261022 --target
test_save_stream_file`;
`ctest --test-dir local/build/native-worker-sav01-stream-symlink-source-read-20261022
--output-on-failure -R '^save_stream_file$'`.
Independent root replay used the same three commands with build directory
`local/build/native-root-sav01-stream-symlink-source-read-20261022`. All commands exited 0 and each
focused CTest passed 1/1. Logs are in
`local/validation/sav01-reader-stream-symlink-source-read-20261022/`; E377 records hashes. Purpose:
verify synthetic symlink-source parsing and reader behavior only. Build products remain in the two
exact build directories. No removal boundary was created.

## 2026-10-22 07:09 CDT — CPU-01 trig-constants baseline

Existing CMake, Ninja, and Apple clang recorded in E325 were reused; no install, download,
dependency, or system configuration change. Commands:
`cmake -S native -B local/build/native-root-cpu01-trig-preseed-baseline-20261022 -G Ninja
-DCMAKE_BUILD_TYPE=Debug`;
`cmake --build local/build/native-root-cpu01-trig-preseed-baseline-20261022 --target
test_trig_constants`;
`ctest --test-dir local/build/native-root-cpu01-trig-preseed-baseline-20261022 --output-on-failure
-R '^trig_constants$'`.
All exited 0; baseline CTest passed 1/1. Logs are in
`local/validation/cpu01-trig-preseed-20261022/`. Purpose: establish the existing synthetic helper
test passes before adding a stale-state initialization case. Build products remain in the exact
`local/build/` directory. No removal boundary was created.


## 2026-10-22 07:25 CDT — CPU-01 trig-constants fixture verification

Existing CMake, Ninja, and Apple clang recorded in E325 were reused; no install, download,
dependency, or system configuration change. Worker commands used
local/build/native-worker-cpu01-trig-preseed-20261022/; independent root replay used
local/build/native-root-cpu01-trig-preseed-verify-20261022/. Each run configured Debug/Ninja,
built target test_trig_constants, and ran focused CTest
ctest --test-dir <build-dir> --output-on-failure -R '^trig_constants$'. All commands exited 0;
each focused CTest passed 1/1. Logs are under
local/validation/cpu01-trig-preseed-20261022/. Purpose: verify synthetic overwrites from
preseeded helper state only. Build products remain in the two exact local/build/ directories.
No removal boundary was created.

## 2026-10-22 07:25 CDT — SRV-01 region-list candidate baseline

Existing CMake, Ninja, and Apple clang recorded in E325 were reused; no install, download,
dependency, or system configuration change. Candidate baseline commands:
cmake -S native -B local/build/native-root-srv01-region-list-uncounted-head-baseline-20261022
-G Ninja -DCMAKE_BUILD_TYPE=Debug;
cmake --build
local/build/native-root-srv01-region-list-uncounted-head-baseline-20261022 --target
test_region_list;
ctest --test-dir
local/build/native-root-srv01-region-list-uncounted-head-baseline-20261022 --output-on-failure
-R '^region_list$'.
All exited 0; baseline CTest passed 1/1. Logs are under
local/validation/srv01-region-list-uncounted-head-20261022/. Purpose: evaluate a potential
synthetic intrusive-list head-unlink fixture. No task was claimed and no implementation changed.
Build products remain in the exact local/build/ directory. No removal boundary was created.

## 2026-10-22 22:12 CDT — isolated save-temp verification

No installs/downloads/dependency changes. Worker reused Apple clang 21.0.0 arm64, CMake 4.4.4
and Ninja. Debug ASan/UBSan configure and focused test_save_stream_file build/run are recorded
in docs/checkpoints/2026-10-22-codex-save-temp-worker.md; build products and red/mutation
evidence remain in the managed worktree local/build/task1-save-temp-20261022 and
local/validation/task1-save-temp-20261022. No global removal boundary. LeakSanitizer unsupported;
ASan/UBSan pass with detect_leaks=0.

## 2026-10-22 22:24 CDT — integrated build and verification recovery

No new install/download. Root reused clang/CMake/Ninja for managed local/build/root-parallel-foundations-20261022
Debug ASan/UBSan full build(1082 steps). Three suite approaches and exact fixture-copy identities
are in docs/escalations/INT-VALIDATION-PARALLEL-FOUNDATIONS-01.md; logs remain managed local/validation/root-parallel-foundations-20261022.
Private fixtures copied read-only from original inputs to managed local/fixtures after SHA checks;
no originals changed. Release worker reused saved Ghidra12.1.4/OpenJDK25 project readOnly/noanalysis
(success3.807s) after one REA default-import timeout. No configuration or saved database changed.

## 2026-10-22 22:28 CDT — release/audio worker verification

No installs/downloads/config changes. Release worker reused clang21/CMake/Ninja Debug ASan/UBSan
in managed local/build/task2-release (428steps, final2step rebuild), focused1/1 and byte-oracle
checks plus private mutants. Ghidra12.1.4/OpenJDK25 saved-project readOnly/noanalysis export
exit0 in3.807s; full argv/provenance retained in local/validation/task2-release. REA default
330s import timeout and empty evidence export retained as limitations. Audio worker reused
Python stdlib and Xcode llvm-objdump found by xcrun;20 synthetic tests and two deterministic
private analyses pass. Exact commands/tool versions/evidence paths in worker reports/checkpoint.

## 2026-10-24 15:53 CDT — GPU-03 direct-Metal synthetic-frame verification

No install, download, or dependency change. Reused Apple clang 21.0.0 targeting arm64,
CMake 4.4.4, Ninja 1.13.2, Xcode 27.1 (27A9275), macOS 27.0.1 (26A434), and the Apple M4 Pro.
Purpose: build and verify the direct-Metal typed-op executor, its synthetic indexed/depth frame,
and checked ImageIO PNG path. CMake registration enables Metal API and GPU validation for the
fixture test. Exact commands from the workspace root:

```sh
cmake -S native -B local/build/orch-r22
cmake --build local/build/orch-r22 -j 12
ctest --test-dir local/build/orch-r22 -j 12 --output-on-failure
ctest --test-dir local/build/orch-r22 -V -R '^metal_asset_frame$'
ctest --test-dir local/build/orch-r22 -R '^(metal_capture|metal_asset_frame)$' -j 2 --output-on-failure
SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh
python3 scripts/project_index.py
trail build --check
trail build
trail build --check
trail grep 'MetalAssetFrameRenderer'
```

All exited 0: CTest 391/391 and lift checkers 476 passed, 0 skipped, 0 failed. Complete logs are
under `local/orchestrator/gpu03-r31/metal-frame/`; generated MSL, PNG and build products stay in
`local/` (MSL SHA-256 `5524537e36b2481798abe5bf6c311e37082aed26b136c4590f7d685e671a455e`, PNG
SHA-256 `35039e0fe8a9700ccfbe8bb66181426c16ea41849677633cea81ac99c0d21a94`). No installed tool or
system configuration was changed; the removal boundary is limited to these task-local build and
evidence artifacts. Preserve original game archives and extracted inputs.
Trail 0.23.0 was reused with no upgrade. The first `trail build --check` reported `STALE` for
the four edited test nodes and one new `fragment_classes_match` node; `trail build` regenerated
8,292 wiring nodes and 11,219 edges, the next check passed, and `trail grep` found the new renderer
API in its header and test. The graph cache is the existing git-ignored `graft/` directory; no
downloaded tool or cache file is being published.

## 2026-11-01 17:20 CDT — Metal Toolchain component (GPU-01-MSL-OFFLINE-01)

- **Purpose / task:** GPU-01-MSL-OFFLINE-01. `xcrun metal` was missing from this Xcode install, so
  the offline GCN -> MSL archive step could not compile anything. Apple's own component installer
  was used; no third-party download.
- **Source:** Apple, via `xcodebuild -downloadComponent MetalToolchain` (Xcode 27.2, macOS 27.0.1).
  Component **27B47**, asset `022-22476-056.dmg`, 838.9 MB, Apple-signed.
- **Install command:** `xcodebuild -downloadComponent MetalToolchain`
- **Install path (managed by Xcode):**
  `/System/Library/AssetsV2/com_apple_MobileAsset_MetalToolchain/e5b5544bf11850c8bd817f7702339b202c00661b.asset`
- **Verified:** `xcrun -sdk macosx metal --version` -> `Apple metal version 32023.922
  (metalfe-32023.922.4)`, target `air64-apple-darwin27.0.0`.
- **Smoke test:** `python3 -I tools/dev/test_msl_pipeline_archive.py` (9 tests, OK) and a real
  two-shader compile through `tools/dev/msl_pipeline_archive.py` (`PASSED archive` /
  `PASSED check`, metallib digests `ae0ac5e5…7451` and `398276e4…e4d3`). Private temp dir only.
- **Removal boundary:** `xcodebuild -deleteComponent MetalToolchain`. Nothing else in this repo
  depends on it yet; generated `.air`/`.metallib` files stay under private `local/` or temp paths.
- **Notes:** `.air` embeds the resolved absolute source path, so identical sources compiled from a
  different directory produce different AIR digests while the metallib digests stay identical.
  `-fdebug-prefix-map` / `-ffile-prefix-map` do not change this; record the AIR digest as
  path-sensitive evidence only.

## 2026-11-01 20:15 CDT — PyGhidra python venv for the decompilation pipeline (RE-GHIDRA-PIPE-01)

- **Purpose / task:** produce a reproducible Ghidra decompiler dossier for the five largest
  unmodelled guest bodies. Ghidra 12 no longer ships Jython, so `.py` GhidraScripts need PyGhidra,
  and `analyzeHeadless` cannot start it (measured: "Ghidra was not started with PyGhidra").
- **Source:** the wheels bundled **inside the already-installed Ghidra 12.1.4**
  (`Ghidra/Features/PyGhidra/pypkg/dist/`). No network access, no download, no third-party tool.
  Ghidra 12.1.4 and OpenJDK 25.0.4.1 are the versions already logged on 2026-10-18 and reused here.
- **Versions:** pyghidra 3.1.0, Jpype1 1.5.2, packaging 25.0 (interpreters: `/opt/homebrew/bin/python3.13`
  — the system `python3` is 3.15 and has no matching JPype wheel).
- **Install command:**
  ```sh
  /opt/homebrew/bin/python3.13 -m venv "local/venv-pyghidra"
  ./local/venv-pyghidra/bin/pip install --no-index \
      --find-links /Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC/Ghidra/Features/PyGhidra/pypkg/dist \
      pyghidra
  ```
- **Checksums of the wheels used** (SHA-256, from the Ghidra install):
  `pyghidra-3.1.0-py3-none-any.whl` `f4c138098e63f7d6b3648ff8fba0619aabf088ce1d1e8692808fa1df9d8921a0`;
  `jpype1-1.5.2-cp313-cp313-macosx_10_13_universal2.whl`
  `4acb098cb1698b14b6e5c79e275f4c70dcc01b0fb93425f206d0a5e380e43c66`;
  `packaging-25.0-py3-none-any.whl` `29572ef2b1f17581046b3a2227d5c611fb25ec70ca1ba8554b24b0e69331a484`.
- **Safety:** a task-local python venv under private `local/` (15 MB), built `--no-index` from files
  already on disk, so nothing was fetched and no installer ran. It only starts Ghidra in-process;
  it does not execute guest code, load `eboot.bin` at runtime, or reconstruct a PS4 environment.
  Smoke test: `--check-project` listed `/eboot.elf` from the existing project and exited 0.
- **Removal boundary:** `rm -rf local/venv-pyghidra`. Nothing else depends on it; the checkers keep
  working without it (they SKIP when the private project is absent). Do not delete
  `local/ghidra/persistent-20261020`, which predates this entry.
- **Not installed:** no other package, no Homebrew formula, no Ghidra upgrade (0.28.x-style upgrade
  banners for graft/trail were ignored on purpose).
