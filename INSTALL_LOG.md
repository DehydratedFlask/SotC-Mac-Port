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
