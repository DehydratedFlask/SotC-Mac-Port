# Checkpoint - child transport carries unknown numeric statuses (APP-01-CORE-ABI-01 defect)

- Local: 2026-11-01 (America/Chicago)
- Agent: worker (Sonnet 5.5) under the orchestrator
- Task: `APP-01-CORE-ABI-01` open defect (child transport vs in-process transport)
- Gate: **FIXTURE_VERIFIED** (pure protocol/mapping; no engine, no boot, G4 NOT RUN)

## Root cause

`native/app/soc_game_core_main.cpp` emitted the bare text `unknown` for a subsystem/archive status
number it did not name, and `ProcessCoreClient` had no parse for it, so it threw a malformed-answer
failure. The in-process `NativeCoreClient` maps any number through `Status(code:)` to `.unknown(N)`,
so the same core answer was representable through one transport and an error through the other.

## State found at start

Commit `adca63bf` (a peer, 2026-11-01 20:59) had already fixed the happy path: emitter `unknown:<N>`,
Swift parser `unknown:<N>` -> `.unknown(N)`, one test. This checkpoint does **not** redo that; it
closes the gaps that commit left:

1. **Non-canonical text was accepted.** The parser used `UInt32(name.dropFirst(8))`, which accepts
   `unknown:+7`, `unknown:007`, and any known code (`unknown:1` became `.unknown(1)` while the
   in-process mapping of 1 is `.partial`). Decoding was therefore not the inverse of encoding and the
   two transports could disagree about a code.
2. **No native test existed for the emitter**; the naming functions lived in an anonymous namespace
   of the executable's `main.cpp`, unreachable from a unit test.
3. **No cross-transport agreement test**, no malformed-value negatives beyond bare `unknown`,
   no protocol documentation of the grammar.

## Wire grammar (now specified)

Known codes keep their names (subsystem 0..3 `not_implemented|partial|implemented|not_linked`;
archive 0..3 `ok|missing|unreadable|bad_magic`). Every other `uint32` N is `unknown:N`, canonical
decimal: ASCII digits only, no sign, no leading zeros, `0...4294967295`, never a code that has a name.
Anything else (bare `unknown`, case variants, whitespace, `+`/`-`, radix prefixes, non-ASCII digits,
overflow including wrap-around values like `4294967303`) is rejected as a malformed answer
(`.internalError`), never coerced. Documented in `launcher/README.md` ("Wire names for numeric
statuses").

## Files changed

- `native/app/core_status_json.{hpp,cpp}`: `subsystem_status_name` / `archive_status_name` moved here
  (exported in `soc::status_json`), comments updated.
- `native/app/soc_game_core_main.cpp`: uses the shared functions; `static_assert`s pin ABI constants
  0..3 to the numbers the shared names are keyed by.
- `native/tests/test_core_status_json.cpp`: `test_status_wire_names` (golden table of 14 codes,
  3000-code grammar sweep, the top of the uint32 range through wrap) and `test_unknown_status_in_lines`.
- `launcher/Sources/LauncherCore/ProcessCoreClient.swift`: `CoreWireStatus`, `Status(wireName:)` /
  `.wireName` for `CoreSubsystem.Status` and `CoreArchiveCheck.Status`; strict canonical parser;
  `ProcessCoreClient` now delegates to it.
- `launcher/Tests/LauncherCoreTests/CoreWireStatusTests.swift` (new, 7 tests, 1 suite): golden table
  shared byte for byte with the C++ test, lossless round trip for 0...300 and the uint32 edges for both
  enums, 34-entry malformed table, vocabulary separation, non-canonical in-memory unknown.
- `launcher/Tests/LauncherCoreTests/ProcessCoreClientTests.swift`:
  `childTransportAgreesWithInProcessMappingForEveryGoldenCode` (14 codes through a real pipe, each
  equal to `Status(code:)`, plus `problemCount`) and a 11-argument parameterized negative test over the
  child transport for coverage and validate.
- `launcher/README.md`: grammar section.
- No change to `native/CMakeLists.txt` (tests added to the existing `core_status_json` target).

## Commands and results

| command | result |
|---|---|
| `cmake --build local/build/orch-r22 --target test_core_status_json soc_game_core -j 8` (before the peer's startup-lane edits landed) | exit 0 |
| `ctest --test-dir local/build/orch-r22 -R "core_status_json\|soc_game_core\|sotc_core"` | 7/7 passed (core_status_json, soc_game_core_{status,coverage,start,serve,validate}, sotc_core_api); re-run after the later full-build failure, same 7/7 against the already-linked `SoCGameCore` (Nov 1 22:43) |
| `cd launcher && swift build` | exit 0, 0 warnings |
| `cd launcher && swift test` | exit 0, **176 tests / 23 suites, 0 failures** (peer commit `adca63bf` state was 167 / 22; +9 tests, +1 suite). This run includes `LauncherNativeCoreTests` (22 tests / 2 suites, `AbiContractTests` among them). The "188 / 24" figure quoted in the task row does not reproduce on this tree |
| `cmake --build local/build/orch-r22 -j 8` (full, later) | **exit 2, not caused by this change**: `-Werror,-Wmissing-field-initializers` ('hashed_names') in `native/core/ctor_slot_{361,379,467,536}_*.hpp`, uncommitted startup-lane work by a peer. The full 532-test ctest was therefore NOT RUN by this worker |

## Mutation evidence (each mutant applied alone, then reverted; sources diffed back to the originals)

Swift (filter `CoreWireStatusTests|ProcessCoreClientTests`, 34 tests): remove the leading-zero guard ->
FAILED (4 issues); allow known codes -> FAILED (14); drop the ASCII-digit guard (accept `+7`) -> FAILED
(4); bare `unknown` decodes to 5 -> FAILED (6); wrap overflow with `truncatingIfNeeded` -> FAILED (4).
Two earlier mutants (bare `unknown` -> code 0, wrap of exactly 2^32 -> 0) passed because the known-code
guard also rejects them; they are equivalent mutants and are *not* counted, and the table gained
`4294967303` and `2^64+7` so the wrap mutant is now killed.
C++ (`core_status_json` ctest): bare `unknown` -> FAILED; zero-padded `unknown:0N` -> FAILED; archive
code 3 renamed -> FAILED; signed cast of the code -> FAILED. Source restored, test passes 1/1.

## Observations versus inference

- Observed: both transports now yield the same `Status` for every golden code; the real `SoCGameCore`
  still only ever emits names for 0..3 (the core never produces an unknown code), so the unknown path is
  exercised only through the encoder unit test and the fake-child decoder tests, not by the real core.
- Inference: the C++ golden table and the Swift golden table are separate literals; they are kept equal
  by review, not by a shared file. A drift would fail exactly one of the two tests only if the changed
  side also changes its own table.

## Risks

- Full native build/ctest unverified on the current tree (peer breakage above). The edited native files
  compile and link standalone (built before that breakage).
- `Status(wireName:)` is an internal API in `LauncherCore`; `.unknown(N)` built by hand for a named code
  encodes to text the parser rejects (pinned by `nonCanonicalInMemoryUnknownDoesNotRoundTrip`).
- `tools/lift/check_fcore_status.py` only checks known names; it was not extended.

## Next exact action

Once the startup lane builds again: `cmake --build local/build/orch-r22 -j 12 && ctest --test-dir
local/build/orch-r22 -j 12 --output-on-failure`, then record the exact full-suite count.
Status of `APP-01-CORE-ABI-01` open defect: closed at FIXTURE_VERIFIED; attempts 0 failed.
