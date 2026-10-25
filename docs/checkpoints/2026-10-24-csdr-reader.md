# GPU-01-CSDR-READER-01 — raw shader-container importer checkpoint

Date: 2026-10-24 CDT. Agent: `/root/shader_reader`; root is the integrator.
Status: **FIXTURE_VERIFIED / ARTIFACT_VERIFIED** for the bounded reader milestone. Root's final
scoped re-review and independent full-corpus replay are recorded in HANDOFF; worker verified that
checkpoint before exiting. The broader native game port remains incomplete.
**G3 NOT RUN**. No instruction execution, MSL conversion, resource ABI or Metal frame is claimed.

## Changed files

- `native/core/csdr_container.hpp` / `.cpp`: bounded C++20 byte-span reader, explicit status/offset,
  checked count/token/code/BinaryInfo spans, no partial imports on failure. Endianness and alignment
  are explicit. Borrowed code views recheck bounds.
- `native/tests/test_csdr_container.cpp`: wholly synthetic fixtures and corruption tests.
- `tools/dev/csdr_inspect.cpp`: metadata JSON diagnostic with length-framed stream input.
- `tools/dev/verify_csdr_corpus.py`: corrected PSARC MD5(path) pairing and one native process,
  independently compared against the existing Python probe; all real evidence remains private.
- `native/CMakeLists.txt`: authorized source, fixture and tool additions under the shared build lock.

## Commands and observations

From the workspace root, all existing tools; no downloads or dependency changes:

```sh
tools/dev/with_build_lock.sh cmake -S native -B local/build/orch-r22
tools/dev/with_build_lock.sh cmake --build local/build/orch-r22 --target test_csdr_container csdr_inspect -j 12
tools/dev/with_build_lock.sh ctest --test-dir local/build/orch-r22 -R '^csdr_container$' --output-on-failure
local/build/orch-r22/test_csdr_container
python3 -I tools/dev/verify_csdr_corpus.py --native local/build/orch-r22/csdr_inspect --self-test
python3 -I tools/dev/verify_csdr_corpus.py --native local/build/orch-r22/csdr_inspect --output local/validation/csdr-reader-r30-v5 --expect-main-corpus local/extracted/CUSA08809/pak0.psarc local/extracted/CUSA08809/pak1.psarc local/extracted/CUSA08809/pakN.psarc
```

All final worker commands exit 0. Focused CTest **1/1**, fixture **563 checks**, **216 structural bit
flips** and **264 truncation prefixes** rejected, verifier **19 checks**. The fixture/tool are
Mach-O arm64; CMake sanitizers are enabled (ASan+UBSan), with zero corpus sanitizer diagnostics.

| Archive | Raw members | Accepted | Named exceptions | Compressed members rejected |
|---|---:|---:|---:|---:|
| pak0 | 1,039 | 1,039 | 0 | 2,270 |
| pak1 | 1,070 | 1,070 | 0 | 1,442 |
| pakN | 12,862 | 12,858 | 4 | 15,608 |
| Total | **14,971** | **14,967** | **4** | **19,320** |

All **21,861** accepted program metadata rows agree with the independent Python probe. They use
version7/source_type1. Four GS-named containers with declared1/observed2 and numeric type pair5/6
are fully structurally checked and rejected as `unsupported_gs_double_program`; 5/6 semantics
remain unestablished. Other count mismatches and type5/6 ordinary programs remain rejected.

Private logs: `local/orchestrator/r30/shader-reader-{build,ctest,selftest,fixture,corpus}-v5.log`.
Exact commands/results/source/archive identities and exceptions are in
`local/orchestrator/r30/shader-reader-report.md` and `local/validation/csdr-reader-r30-v5/summary.json`.
Private members manifest SHA256:
`d8cce9b50747df85846e86aa1bc850cc8b1092e04bc698af6650540104953117`;
per-program manifest SHA256:
`87a315f424ec54d41cec1fec0cb69024dcd402d0041d6397c15a82ce65aace87`.

## Failure record and limits

**Failed implementation hypothesis1:** initial inferred type2 GS identification failed at the
first real exception. v1 exit1 and private evidence are retained, not overwritten. Revised
attempt2 recognizes only the observed numeric pair as a rejection diagnostic, after both complete
records validate; acceptance counts were never relaxed.

**Failed implementation hypothesis2:** first-byte compression detection falsely matched valid raw
counts140 (`8c00`) and204 (`cc00`); review exposed this outside the real corpus. The intermediate
two-byte correction also collides with counts1676/2700/1740/2764. Root recorded a principal ruling
authorizing structural-first **attempt3**: a complete valid raw structure wins; only raw validation
failure permits the known block-prefix rejection fallback (`8c06`, `8c0a`, or `cc` with any second
byte). The verifier now supplies every complete member before classification. Native/Python
regressions pass all six raw counts and the four exact short signatures plus arbitrary `cc7f`.
Attempt3 succeeded; no third unsuccessful hypothesis or fourth attempt occurred. Artifact evidence
directories v1–v5 are run revisions, not semantic attempt numbers, and are preserved privately.

**FAILED environment check:** `ASAN_OPTIONS=detect_leaks=1` aborts before test execution with
`detect_leaks is not supported on this platform` (CTest exit8/direct exit134). Supported default
ASan+UBSan runs pass; LeakSanitizer **NOT RUN**. This is separate from semantic attempt accounting.

Hash/CRC are metadata only; checksum algorithm/coverage are unverified. Opaque container header
and trailer fields, input-usage tables and shader semantics remain unmodelled. Required spans
reject truncation, but an opaque trailer cannot be authenticated. Complete compressed-prefix
members are checked as candidate raw containers and rejected; no decompression or decoded-body
validation is claimed. Full-suite publication checks belong to
root. No original executable/module is required or executed by these tools.

Next exact action: root finishes shared indexing and publication audits, then integrates the
allowlisted changes. Independent review/replay and HANDOFF completion are already recorded. The
separate offline GCN worker may consume bounded code spans from the verified default shader pair.
No commit, tag or push was performed by this worker.

## Independent review finding — 2026-10-24 (historical; resolved below)

Root's read-only reviewer reproduced a P2 false positive in `native/core/csdr_container.cpp:39-40`
and `tools/dev/verify_csdr_corpus.py:158`: classification uses only the first byte. A valid
synthetic 140-program raw container begins with low count byte `0x8c`; the Python reference parses
it, while the native reader returns `unsupported_compression` before checking its count. The corpus
reader also tags every `0x8c`/`0xcc` first byte as compressed. No existing corpus row was affected.

The owning worker is fixing both classifiers and adding raw-count regressions while preserving the
compressed-prefix fixtures. Do not mark GPU-01 complete until the added tests, targeted native checks
and scoped re-review pass. Stage identities for numeric types 5/6 remain unverified.

## Structural-first resolution — 2026-10-24

The authorized attempt3 is implemented. Final worker replay (v5) exits0, **14,967 accepted +4 named
exceptions**, **19,320 full compressed-prefix members rejected**, **21,861 program rows**,
**34,291 member rows**, sanitizer stderr0B. All final source hashes match `summary.json` and both
manifest digests/row counts were rechecked. Per-program digest is unchanged from v2/v3; the members
digest changed because compressed rows now include complete-member metadata instead of two bytes.
Final native diagnostic SHA256:
`dec1e17a1e1a8bc9e2bc58d9e773716380279409a95be5bb8c71c78aeaa6cc5c`.
Run recorded `2026-10-24T14:56:18.658148-05:00` to `14:56:58.968744-05:00`, monotonic40.311s.
Root's final scoped re-review and independent full-corpus replay passed and are recorded in
HANDOFF. The review additionally checked all256 short `ccXX` signatures; root's corpus replay
reproduced the exact counts. This worker verified the HANDOFF completion record before exiting.
Next: root finishes shared indexing/publication audits and integrates the allowlisted changes;
the separately owned shader slice consumes the bounded parser output. G3 stays NOT RUN.
