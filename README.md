# SotC Mac Port

Public working tree for a **native Apple Silicon / Metal** port of the PS4 remake of
*Shadow of the Colossus*, plus a **SwiftUI/AppKit Mac launcher**.

This repository is the Mac native port core, launcher, port tooling, and collaboration docs.
**No Sony game assets, packages, or executables are included.** You must supply your own
legally obtained game files to extract assets and run anything beyond fixture tests.

The game port is **not** complete. There is no claim of cold boot, authentic audio, or
real save/load until the gates in [ROADMAP.md](ROADMAP.md) and [docs/VALIDATION.md](docs/VALIDATION.md)
are demonstrated on hardware.

## What you need

- A Mac with Apple Silicon and a recent Xcode / Swift toolchain
- CMake and a C++20-capable Apple Clang (for `native/`)
- Python 3 (for `scripts/` helpers and lift checks)
- **Your own** legally obtained PS4 remake files (kept outside this repo — never commit them)

## Build: native fixtures

```bash
cmake -S native -B local/build/native -DCMAKE_BUILD_TYPE=Debug
cmake --build local/build/native
ctest --test-dir local/build/native --output-on-failure
```

`local/` is gitignored private workspace (builds, extracted assets, RE dumps). Create it as needed.

## Build: launcher

```bash
swift test --package-path launcher --scratch-path local/build/launcher
# or open / build the Swift package under launcher/
```

The launcher is an honest shell: it must not pretend the native game core is ready when it is not.
See [launcher/README.md](launcher/README.md).

## Collaboration docs

| Doc | Purpose |
|---|---|
| [ROADMAP.md](ROADMAP.md) | Milestones, gates, public-remote / checkpoint commit rule |
| [HANDOFF.md](HANDOFF.md) | Live agent claims and checkpoints |
| [AGENTS.md](AGENTS.md) | Non-negotiable target and operating rules |
| [docs/INDEX.md](docs/INDEX.md) | Full documentation index |
| [docs/TASKS.md](docs/TASKS.md) | Bounded task cards |
| [docs/PLATFORM.md](docs/PLATFORM.md) | ARM64 / Metal / audio / save design |
| [docs/VALIDATION.md](docs/VALIDATION.md) | Pass/fail acceptance gates |

## Licence

This port is distributed under the **GNU General Public License, version 2 or later**
([LICENSE](LICENSE)). The owner chose GPL-2.0-or-later on 2026-11-01 so that GPL-2.0-or-later
prior-art code (the Metal command path and shader tooling from the public Bloodborne Mac ports,
which build on shadPS4) can be ported or vendored here with attribution instead of being
re-invented. Reused components keep their original authors and notices; see
[docs/PRIOR_ART.md](docs/PRIOR_ART.md) and [docs/ATTRIBUTION.md](docs/ATTRIBUTION.md).

No Sony game assets, packages or executables are included, and none may be committed.

## Public remote and agent checkpoints

- **GitHub:** https://github.com/DehydratedFlask/SotC-Mac-Port
- At **every** completed task / HANDOFF checkpoint: commit **allowlisted** changes only, then
  `git push origin main`. Never stage forbidden paths. Run
  `python3 -I scripts/audit_publish_paths.py` on the staged set (and `--all` before push).
- Forbidden: game PKGs/PSARC/eboot/SELF/ELF, `local/`, `reference/`, `logs/`, secrets, anything
  that reconstructs or runs the game without owning it. Details in `.gitignore` and AGENTS.md.

## Layout (published)

| Path | Contents |
|---|---|
| `native/` | C++ core slices, CMake, fixture tests |
| `launcher/` | SwiftUI / AppKit shell |
| `docs/` | Port plans, validation, task register (no game blobs) |
| `scripts/` | PSARC helpers, indexes, publish path audit, lift-adjacent tools |
| `tools/lift/` | Offline lift / fixture check helpers (no game binaries) |
| `tools/pkg-extractor/` | Out-of-tree build adapter notes (does not ship game content) |

Private on the workstation only: `Shadow of the Colossus/`, `local/`, `reference/`, `logs/`.
