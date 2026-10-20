# Evidence ledger — 2026-10-19 baseline

Observations, upstream claims and planned behavior are separate. Raw copyrighted binary evidence
stays under `local/`; this ledger records identities, paths and limits. No native game boot,
Metal gameplay, live game sound or game save/load has been demonstrated.

## Verified observations

| ID | Evidence | What it establishes | Limits / next task |
|---|---|---|---|
| E01 | User request + explicit offline-analysis and folder-indexing replies; AGENTS/HANDOFF | Native ARM64/Metal/Swift target; offline local eboot analysis allowed; all folders indexed | Does not authorize publication or a weaker runtime |
| E02 | `xcodebuild -version`, `xcrun --find metal`, `sw_vers`, hardware profiler | M4 Pro 12-core/48 GB, macOS 27.0.1, Xcode 27.1/27A9275, Metal compiler available | Tool availability is not a build/game test |
| E03 | Four local Git pins in INDEX/INSTALL_LOG | Exact source baselines; new extractor checkout isolated | No unrecorded upstream update or initialized submodule |
| E04 | `logs/pkg-extractor-*.log`, `file` on built extractor | Native ARM64 standalone extractor; first build failed on fseeko64, second passed | File utility only, no game CPU execution |
| E05 | `logs/extract-*.log`, `local/manifests/packages.json`, inventory assertions | All 6 original PKGs hashed; 124 output files; base/update/DLC identities; seven bounded PSARC TOCs | Does not validate every archive member/asset schema |
| E06 | `local/re/eboot.identity.json`, `logs/unwrap-self.log`, SELF fixtures | Exact source/ELF hashes, entry 0xf120, 11 program headers, admitted plaintext segments | Metadata segment 10 unavailable; not byte-exact conversion |
| E07 | `local/re/evidence/entry.json` (REA/Ghidra) | Completed static entry-function dossier; assembly, decompilation, resolved callees/references | Inferred types; generic PS4 import/linkage handling incomplete; one function only |
| E08 | `local/re/evidence/entry-view.json` | Saved Ghidra Evidence can be projected with REA without repeating analysis | View is derived from E07, not independent behavior evidence |
| E09 | `local/re/pc-loader-reference/analysis.json`, `logs/pc-loader-inspection.log` | Pinned reader reports 588 imports, 712,333 relocations and 49 needed-module declarations | Reported NID name hints remain hints; not 49 installed modules or native replacements |
| E10 | `local/audio/probe.json`, FFprobe/FFmpeg command, ARM64 FFmpeg identity | One package sound is ATRAC9/48 kHz/stereo; first 5 s decodes to finite nonzero PCM | Package menu/splash asset only; gameplay banks/NGS2 not established |
| E11 | `docs/REFERENCE_AUDIT.md`, 23 pinned source links and local line checks | Source architecture: x86 execution, silent NGS2 fallback, x86 fibers/TLS, Vulkan, save limits | Source observations are not execution on this Mac |
| E12 | `docs/PLATFORM.md`, `docs/VALIDATION.md`, linked Apple/LLVM primary docs | Proposed native contracts and supported platform/tool constraints | DESIGN; implementation gates remain unrun |
| E13 | `logs/project-tools-tests.log` | Three stdlib tests pass: plaintext reconstruction/identity, encrypted rejection, overwrite preservation, deterministic index/stale detection | Project utilities only; no native game tests |
| E14 | `docs/FOLDERS.md`, `local/indexes/folders.json`, Graft check/build log | Folder navigation and source index state at final checkpoint | Regenerate when files/folders move; raw game data not a source-code index |
| E15 | `scripts/ps4_dynlink.py` -> `local/re/imports.json`, `local/re/unresolved.jsonl`; `scripts/test_ps4_dynlink.py` (6 pass) | Independent parse of PS4 dynamic tables from the exact ELF: 49 needed modules, 51 import libraries, 588 relocated symbols (571 JUMP_SLOT, 2704 R_X86_64_64 sites, 709,058 RELATIVE). All 588 NIDs named: 436 declared in shadPS4 contracts, 152 by exact NID-hash match of identifiers in the pinned references | Hash match proves spelling only, not semantics/module version/signature; contract declaration may be a stub upstream |
| E16 | `scripts/disasm_range.sh` over ELF bytes; `local/re/boot-graph.json` | Entry 0xf120 observed: `_init_env`, `atexit` x2 (rsi hook and local handler 0xf8f330), ctor runner 0x20, app main 0x536730, `catchReturnFromMain`, `exit`. 0x20 reverse-walks a -1-terminated list at 0x2dd70e0 holding 573 RELATIVE-relocated constructor pointers. 0x536730 builds app object "Shadow" with options (ResourceDependenciesFile, StartupScript=shadowmain.sgpr, VSyncWindow, nointro, startupArea), runs virtual slot +0x18, then `quick_exit` | Roles inferred from asm; constructor and callee bodies unrecovered; vtable calls unresolved |
| E17 | `scripts/psarc.py`, `scripts/test_psarc.py` (10 pass), `local/manifests/assets/*`, `docs/ASSETS.md` | All 7 PSARC TOCs and NUL-separated manifests decode with consistent counts; patch cachedfiles vs base: 3 new, 35,734 digest changes; no standard audio files; provisional overlay policy documented | Digest semantics, engine mount order, member schemas, music/dialogue location unproven |
| E25 | `scripts/classify_audio_headers.py`, `scripts/test_audio_headers.py` (11 pass), `local/manifests/assets/audio-headers.json`, `docs/ASSETS.md` AUD-01 section | 86 `.sxd`/`.sxd1`/`.sxd2`/`.hd`/`.bd` members classified by magic+Shannon entropy only: 44 `xff\\0` hd/bd, 17 high-entropy `8c0a`/`8c06` pakN sxd*, 12 LE-small-int sxd*, 2 `fbad9897` cachedFiles bridge.sxd, plus outliers (`qath`, `80AE`, near-empty `3ebe7b`). Codec field UNKNOWN for all; no NGS2 claim | Not a codec ID; music/dialogue streams still missing; game reader unrecovered |
| E18 | `tools/lift/guarded_const_ctor.py`, `tools/lift/check_f6cad0.py` (OK), `native/` ARM64 ctest (ASan/UBSan, 1/1 pass), `local/re/functions.jsonl`, `local/re/ctor-vec-copies.json` | First recovered function: constructor RVA 0xf6cad0 (286 bytes) matched byte-exactly by strict encodings (no CPU model); native `sotc::init_math_constants` equals the binary's stored bits (6 floats, 2 masks) and independent IEEE oracles. Survey: of 573 constructors, 227 are exactly one 16-byte constant copy + ret, 12 more match other idioms, 275 start with a normal frame prologue | FIXTURE only; one leaf; says nothing about the ~275 real constructors, heap/vtable/service calls, or reachability from startup. Not G1 |
| E19 | `launcher/` (`swift build`/`swift test`, scratch path local/build/launcher), README | SwiftUI shell builds arm64 with Swift 6.4; 5 Swift Testing cases pass (asset validation, PSAR magic, core never faked); binary ran 4 s without crash; no Vulkan/MoltenVK/Wine linkage in `otool -L` | No visual/VoiceOver review; not bundled; core ABI undefined; cannot boot a game |
| E20 | `tools/lift/survey_ctors.py`, `check_f125990.py` (OK), `native/` ctest 3/3 + TSan build of scratch test, `local/re/functions.jsonl` | Second recovered function: RVA 0x125990 (41 B) = memset(8 KiB zero) + seq_cst zero store, template-exact, memset import confirmed by NID hash; native `sotc::reset_scratch` passes independent canary/atomicity oracle. Ctor survey: only 32 prologue ctors have a detected end by heuristic; smallest call only libc/libm imports; typical ones call __cxa_atexit + internal helpers | 2 of 573 ctors; survey end-detection is a heuristic and its call-target print for far targets is wrong (unsigned wrap) — use for ranking only. Not G1 |
| E21 | `tools/lift/check_fe10490.py` (OK), `tools/lift/fsin_callsites.py`, `native/core/libm_policy.hpp`, ctest 4/4 | Third recovered ctor: RVA 0xe10490 (50 B) copies {~0,~0,~0,0} and stores `_FSin(0x3d567750 [=3 deg], 1, 0)`; thunk NID matches `_FSin` by hash. 383 direct `_FSin` call sites: 148 set edi=0, 110 set edi=1 (often adjacent same-argument pairs), 125 unclassified -> offset meaning (0 sin / 1 cos) INFERRED. Policy L1 chosen: double eval + single rounding, <=1 ULP fixtures | Original `_FSin` algorithm not examined (needs a Sony module; not authorised) so bit equality unknown; consumers of the stored value unrecovered; 2nd int arg (always 0 here) unexplained |
| E22 | `tools/lift/check_fe0e140.py` (OK), `native/tests/test_normal_threshold.cpp`, `local/re/work/fn140.txt` | Sole direct reader of the cos(3deg) cell is in function 0xe0e140 (0xe0e140..0xe0e833, AVX, 8-way unrolled loops over 16-byte records): it computes an unfused float dot product and tests `dot > cell`, unordered -> skip. Conclusion: threshold use only; a 1-ULP libm difference can flip only borderline comparisons; the value is not stored/hashed there | Only direct rip-relative readers were scanned (indirect/other-module uses missed); 1.7 KB function mostly NOT lifted; geometry meaning inferred. An earlier note citing reader address 0xe0e174 meant the displacement byte of the load at 0xe0e170 |
| E23 | `tools/lift/check_f66dec0.py` (OK), `tools/lift/check_f66dec0_dispatch.py` (all 13 arms byte-exact), `survey_packed_ptr.py`, `xref_rip.py`, `native/core/addr.*`, `native/tests/test_addr.cpp` (all-arm behavior), ctest, `docs/POINTERS.md` | Game packs 32-bit fields as bit31 = heap-relative + 31-bit offset: decode `([base cell]if bit31 else 0)+(v&0x7fffffff)`, retag `if untagged && off!=0 && base+off<limit`; cells at 0x2f3b8c0/0x2f08000. Dispatcher at 0x66dec0 has 13 observed arms; native behavior covers each mutating arm and verifies no-op/invalid arms preserve object bytes. SRC claim of 31-bit static pointers is consistent but untested; no 40-bit mask found in game code | Static-address meaning, type names and complete asset layouts remain open; census counts are leads only |
| E24 | `tools/lift/check_heap_window.py` (OK), `native/core/addr.*` (`HeapWindow::from_region`), ctest 6/6 | Heap window provenance: allocator region (start=R[0x60], size=R[0x58]) -> Region struct @0x2f3b8c8 -> limit cell 0x2f08000 = start+size; base cell 0x2f3b8c0 = start & ~0x7fffffff, i.e. Tagged31 offsets are relative to the 2 GiB-aligned window containing the heap region. Cell stores were found by tracing register stores after `lea` (each cell has exactly one writer) | Allocator API 0x19ae0 arguments and R layout beyond +0x58/+0x60 not recovered; runtime region placement unknown; one-sided limit test makes below-start offsets taggable by the original rule |
| E26 | `local/re/evidence/ghidra-d71f60.txt`, `ghidra-d71f60-import.log`, `ghidra-d71f60-script.log`; Ghidra 12.1.4 headless project `local/re/work/ghidra-projects/SotC_D71F60_20261019` | Analysis/import/post-script/save completed for the exact ELF SHA below; the exporter queried entry `0x0d71f60` at image base 0 and reported `function_found=false` | No target body, references, or decompilation were exported; this does not show the bytes are absent from the ELF; no CPU-01d leaf or G1 claim follows |
| E27 | `local/re/evidence/ghidra-arena-accounting.txt`, `ghidra-arena-accounting.log`, `ghidra-arena-accounting-script.log`; Ghidra 12.1.4 project for the exact ELF | Ghidra found `FUN_00019ae0` (372 body bytes, 97 instructions); call at `0x19b71` targets `0x19e00`; two direct incoming call references were recorded; decompilation completed | The wrapper body ends at `0x19c5a`, so it does not include the candidate accounting block at `0x1a123–0x1a15a`; inner target `0x19e00` and accounting semantics remain to be queried |
| E28 | `local/re/evidence/ghidra-arena-accounting.txt`, `ghidra-arena-accounting-inner.log`, `ghidra-arena-accounting-inner-script.log`; Ghidra 12.1.4, exact ELF SHA `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, image base 0, language `x86:LE:64:default` | `FUN_00019e00` body has 2,099 bytes/97 instructions; target block is present byte-exactly at `0x1a123–0x1a162`; 10 direct call instructions, zero unresolved computed calls; 4 incoming call references; decompilation completed. `r13` at the block is reloaded from the original `rcx` argument; wrapper `0x19ae0` forwards size in `rcx` | Fixture evidence only; no semantic names for `+0x80/+0x88/+0x90`, no runtime allocator proof or G1 claim; no REA Evidence ID (headless Ghidra evidence) |
| E29 | `native/core/arena_accounting.*`, `native/tests/test_arena_accounting.cpp`, `tools/lift/check_f19e00_accounting.py`; `local/build/native/arena_accounting-*.log` | Sanitizer-enabled ARM64 fixture exercises the raw-offset arithmetic, including exact-size and undersized spans; checker matches E28 instruction bytes and independently derives expected byte arrays without executing the ELF | Fixture models only the observed block; no field semantics, allocator integration, runtime behavior, or G1 proof |
| E30 | `native/core/vec_ce57b0.*`, `native/tests/test_vec_ce57b0.cpp`, `tools/lift/check_fce57b0.py`; `local/build/native/native-fixture-batch-*.log`, `check_fce57b0.log` | For ELF SHA `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`, raw RVA bytes decode a 17-byte load/store/ret body with source `0x12f8330`, destination `0x3351cb0`, and lanes `{~0,~0,~0,0}`; survey position 153, vector-copy manifest position 95; focused CTest and ELF checker pass | A saved survey disassembly comment conflicts with the addresses; checker recomputes signed RIP-relative targets from hash-pinned bytes. Synthetic destination fixture only; no startup reachability or G1 proof |
| E31 | `native/core/vec_ce0890.*`, `native/tests/test_vec_ce0890.cpp`, `tools/lift/check_fce0890.py`; `local/build/native/native-fixture-batch-*.log`, `check_fce0890.log` | For the same ELF SHA, raw RVA bytes decode a 17-byte load/store/ret body with source `0x12f7c00`, destination `0x3351c90`, and lanes `{~0,~0,~0,0}`; survey position 155 and vector-copy manifest position 97; survey reports no direct or indirect calls; focused CTest and ELF checker pass | Synthetic destination fixture only; no startup reachability or G1 proof |
| E32 | `native/core/vec_ce7300.*`, `native/tests/test_vec_ce7300.cpp`, `tools/lift/check_fce7300.py`; `local/build/native/ce7300-*.log` | For the same ELF SHA, the native fixture/checker match the load/store/ret at `0xce7300`, source `0x12f8bb0`, destination `0x3351cf0`, and lanes `{~0,~0,~0,0}`; checker independently verifies constructor survey position 149 and vector-copy manifest position 91; focused CTest passes | This metadata repair adds no runtime behavior or coverage. The native cell remains fixture evidence; no G1 proof follows |

## Binary analysis identity and boot seed

ELF SHA256: `d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.
REA 6.7.0, Ghidra 12.1.4; language selected by importer `x86:LE:64:default:gcc`.
Profile digest: `769518a8e9b57f953dd241adbb63672f0f486bb3d66359b9deef7c5e088ca7aa`.
Entry Evidence ID: `ev_51f10a737f14992cc6215e62b361b86a073fdce4a36455feb6d18635bc8a78b4`.
Portable view Evidence ID: `ev_7ac9be4584c52cd76cfed268e60de5d2476adf1e1f1e91c5a6b322577ba58e49`.

Observed entry body: `0xf120..0xf170`, 81 bytes, Ghidra name `FUN_0000f120`.
Seven direct CALL sites reach six unique targets: `0xf8f420`, `0xf8f430` (twice), `0x20`,
`0x536730`, `0xf8f440`, `0xf8f450`. Ghidra classifies four as thunks. A data reference at
`0xf141` points to `0xf8f330`. Entry ends with UD2. These are observed static instructions;
their library names and higher-level roles have not all been proven.

Inference only: the `(argc-like, argv-like, 0)` call to `0x536730` is an application dispatch
candidate; `0x20` is a startup/constructor-chain candidate. Keep them unnamed until inspected.
Next RE-02 action: map the PLT/GOT thunks through PS4 relocations/NIDs and inspect these two
functions plus their data/callback edges. See [REVERSE_ENGINEERING](REVERSE_ENGINEERING.md).

Index files: `local/re/functions.jsonl` (one reviewed entry record, no native implementation),
`local/re/boot-seed.json` (observed call/data edges), `local/re/rea-snapshot.json` (REA snapshot).
REA's temporary Ghidra project was deleted on CLI exit. No persistent .gpr has been created.
The reproducible persistent-project recipe is documented; do not assume a saved database exists.

## Import/asset evidence details

Pinned preparation reports relocation types 7: 571, 8: 709,058, 1: 2,704. The huge relative
relocation set makes native data/function-pointer recovery a real task, not a linker-flag edit.
Its 49 module declarations include GNM, Fiber, AudioOut, NGS2, AJM, Audio3D, SaveData, Fios and
libc as well as online/OS interfaces. Determine actually reached behavior; do not download modules.
The only bundled sce_module files observed are `libc.prx` and `libSceFios2.prx`; neither may be
used as executable code in the finished native app. No NGS2/ULT firmware was required to inspect.

PSARC headers report v0x10004, zlib, 30-byte TOC entries, 65,536-byte blocks, flags 12.
Observed entry counts: cachedFiles 40,214; pak0 6,919; pak1 5,631; pak2 1,314; pak3 12,054;
pakN 64,109; patch cachedfiles 40,217. These are header counts, not validated unique assets.
Manifest decoding, flags, duplicates, patch overrides and actual audio/shader formats remain DATA-02.

The preparation script also emitted private x86 `boot.bin`/`entry.bin` and another ELF under
`local/re/pc-loader-reference`. These are historical loader-analysis outputs, NEVER native build
products or permissible runtime assets. They were not executed. Its empty `resources` heuristic
does not imply that the SoTC archives contain no assets.

## Failures and recovery

- Native extractor build 1: failed on fseeko64; CMake-only Darwin spelling fix; build 2 and
  full extraction passed. Logs retained. SETUP-01 has 1 historical resolved failed try.
- REA MCP default Ghidra startup: timeout at 330,000 ms with 2G heap; diagnostic preserved in
  `local/re/evidence/rea-mcp-timeout.json`. No completed overview from that attempt.
- Recovery: process-local existing REA CLI, 8G heap, 1,200,000 ms allowance; default analyzers
  unchanged, entry dossier completed with exit 0. RE-01 has 1 historical resolved resource failure.
- Import warnings include stripped-dynamic interpretation, function overlap/body repairs and
  unresolved decompiler instructions elsewhere. Default auto-analysis completion does not prove
  their correctness; seed dossier records only its own observed facets/coverage.

## Explicit unresolved questions

| Priority | Question / blocker | Owner task / proof needed |
|---|---|---|
| Critical | Can reachable engine/game logic be recovered into ordinary native functions at useful scope? | CPU-01 feasibility slice, then CPU-04/05 coverage |
| Critical | Which NGS2 commands/racks/effects and Audio3D behavior are actually used? | AUD-02 call-site/parameter recovery |
| Critical | Can all used wave64/WQM/helper/cross-lane behavior be lowered for each Metal shader stage? | GPU-01/02 real corpus + fixtures |
| High | Actual audio bank formats/codecs/events beyond snd0 | DATA-02/AUD-01 format-aware samples |
| High | PS4 dynamic imports/relocations, module init, TLS and indirect function pointers | RE-02, CPU-02/03 |
| High | PSARC manifest/flags/case/patch overlay semantics | DATA-02; no case-blind merge |
| High | Game save fields/checksum/version/commit boundary | SAV-01 then real restart/load |
| High | Missing metadata program header 10 significance | RE-02 byte-format classification |
| High | Real Metal/CPU memory lifetime and feedback/readback ordering | GPU-03; actual scene tests |
| High | Complete gameplay/content coverage, stability and Mac performance | INT-01/02, REL-01, PERF-01 |

Current gate summary: G0 PARTIAL (artifact/tool/source navigation established; asset/member and
full linkage schemas not yet recovered). G1-G6 NOT RUN. There is no native game executable yet.

Final source-index build: Graft wiring graph reports 63,874 nodes across 2,980 source files,
including the four new Python project helpers. The previous persisted `onlyDirs=[reference]`
excluded them despite a passing check; the scope was explicitly expanded to scripts/tools/reference.
Build parsed 4 new files and replayed 2,976 cached files; log: `logs/graft-build.log`. Its optional
LLM meaning/deep layer is not built (0%); source queries use wiring/lexical evidence and the
reviewed documentation. Do not call this a completed semantic graph or 63,859 reviewed functions.
The folder index excludes administrative .git/Python caches/Finder metadata; it retains every
other folder, with private game/analysis details in the local-only JSON inventory. Configuration
files without a Graft language parser remain documented by folder contracts/build recipes.
Final folder count: 629; freshness check passed after final logs/manifests were written.
Final verification summary: `local/manifests/project-checks.json`; document/link check log:
`logs/project-docs-check.log` (17 Markdown files, 71 local links, zero errors).
