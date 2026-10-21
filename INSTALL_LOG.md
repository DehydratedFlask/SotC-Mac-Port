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
