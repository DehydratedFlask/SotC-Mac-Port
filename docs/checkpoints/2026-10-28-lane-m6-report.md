# Lane M6 report: startup constructor slots 544 557 558 559 569 448 379 518 507 539

Date: 2026-10-28 23:33 CDT. Agent: `/root/m6_validate`. Source ELF: `local/re/eboot.elf`, SHA-256
`d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3`.

## Result

M6 is **PARTIAL**. Five slots (544, 557, 558, 559, 569) passed their byte-oracle checkers, native fixture tests,
and checker mutation runs. Five slots remain unverified: 448, 507, and 539 have native model/test files but no
checker and are absent from `native/lanes/m6.cmake`; 379 and 518 have only caller-tag constants/helpers in
`lane_m6_shapes`, with no slot-specific constructor model, fixture, checker, or CMake target. Do not wire or count
those five as recovered based on their current files/comments.

No M6-specific source edits were made in this validation. Original ELF bytes were read only by the offline
checkers; nothing was loaded, relocated, or executed. The slot 557 handler and all vtable/destructor addresses are
identity tags, not calls into original code.

The checker replay is byte-oracle evidence plus native fixture evidence. This subtask did not obtain REA/Ghidra
Evidence IDs, so it makes no Ghidra-backed reachability claim and does not upgrade the broader G1 gate.

## Slot status

| Slot | RVA / bytes | Status | Evidence or gap |
|---|---:|---|---|
| 544 | `0xd1b20` / 273 | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED | Checker passed; 126 ELF and 40 native mutants rejected; CTest passed. Models the zeroed block and ten named shadow-pass records. |
| 557 | `0x47490` / 296 | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED | Checker passed; 133 ELF and 15 native mutants rejected; CTest passed. Models command `mem`; handler identity tag is not invoked. |
| 558 | `0x428a0` / 230 | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED | Checker passed; 95 ELF and 22 native mutants rejected; CTest passed. Models five cells, an inline vector, and an owner cell. |
| 559 | `0x3f2d0` / 456 | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED | Checker passed; 202 ELF and 22 native mutants rejected; CTest passed. Models three inline vectors and `loggingIncludeTimestamp`. |
| 569 | `0x1cc80` / 491 | STATIC_ANALYSIS_VERIFIED + FIXTURE_VERIFIED | Checker passed; 242 ELF and 24 native mutants rejected; CTest passed. Models `mem_platformMemVerify` and its debug-menu toggle. |
| 448 | `0x4b8680` / 501 | NOT VERIFIED | Model/test exist, but `tools/lift/check_ctor_slot_448.py` is absent and the lane CMake file does not build its source/test. |
| 507 | `0x22f3c0` / 567 | NOT VERIFIED | Model/test exist, but `tools/lift/check_ctor_slot_507.py` is absent and the lane CMake file does not build its source/test. |
| 539 | `0x121940` / 571 | NOT VERIFIED | Model/test exist, but `tools/lift/check_ctor_slot_539.py` is absent and the lane CMake file does not build its source/test. |
| 379 | `0x6d8d20` / unknown | NOT IMPLEMENTED | Only `kFloatSiteTag_379` and generic `init_float_var_site` helper are present; there is no slot-specific lift, test, checker, or target. |
| 518 | `0x1a57d0` / unknown | NOT IMPLEMENTED | Only `kFloatSiteTag_518` and generic `init_float_var_site` helper are present; there is no slot-specific lift, test, checker, or target. |

The comment in `lane_m6_shapes.hpp` describes the float tags for 379/518 as equivalent to an already accepted tag.
No slot-379/518 checker exercised that claim in this replay; it remains an unverified implementation assertion.

## Observations vs inference

- Observed: the pinned ELF hash matched; M6 CMake currently includes `lane_m6_shapes.cpp` and slot/test targets
  544, 557, 558, 559, and 569 only. Files for 448, 507, and 539 exist but are omitted from that CMake file.
- Observed: normal checkers for 544/557/558/559/569 exited 0 and each reported all checker mutations rejected.
  Each `--tamper` invocation exited 1 with the expected pinned-body SHA failure.
- Observed: the five configured CTests all passed. These are native fixture results, not game execution.
- Inference not promoted: slot 448's two console variables, slot 507's two bool variables, and slot 539's name holders
  are described in unverified source comments; without checkers/replay they are not accepted here.
- Unknown: the full semantics, body extents, and native contracts of slots 379 and 518; gameplay consumers of all
  M6 data; any boot, renderer, audio, or save behavior.

## Commands and results

All logs are private and ignored under `local/validation/orch-20261028/m6/`.

```sh
cmake -S native -B local/build/lane-m6-verify -DSOTC_LANES=m6 > local/validation/orch-20261028/m6/configure.log 2>&1 # exit 0
cmake --build local/build/lane-m6-verify -j 12 --target test_ctor_slot_544 test_ctor_slot_557 test_ctor_slot_558 test_ctor_slot_559 test_ctor_slot_569 > local/validation/orch-20261028/m6/build.log 2>&1 # exit 0
ctest --test-dir local/build/lane-m6-verify -R '^ctor_slot_(544|557|558|559|569)$' --output-on-failure > local/validation/orch-20261028/m6/ctest.log 2>&1 # exit 0; 5/5 passed
python3 scripts/project_index.py # exit 0; 42,899 folders indexed; private index written
trail build --check # exit 0; wiring graph current at 10,725 nodes; deep layer not built
```

Each of the following five checkers exited 0 and each `--tamper` command exited 1 as intended; per-slot stdout is
in `slot-<N>-checker.log` and `slot-<N>-tamper.log`:

```sh
python3 -I tools/lift/check_ctor_slot_544.py > local/validation/orch-20261028/m6/slot-544-checker.log 2>&1
python3 -I tools/lift/check_ctor_slot_544.py --tamper > local/validation/orch-20261028/m6/slot-544-tamper.log 2>&1
python3 -I tools/lift/check_ctor_slot_557.py > local/validation/orch-20261028/m6/slot-557-checker.log 2>&1
python3 -I tools/lift/check_ctor_slot_557.py --tamper > local/validation/orch-20261028/m6/slot-557-tamper.log 2>&1
python3 -I tools/lift/check_ctor_slot_558.py > local/validation/orch-20261028/m6/slot-558-checker.log 2>&1
python3 -I tools/lift/check_ctor_slot_558.py --tamper > local/validation/orch-20261028/m6/slot-558-tamper.log 2>&1
python3 -I tools/lift/check_ctor_slot_559.py > local/validation/orch-20261028/m6/slot-559-checker.log 2>&1
python3 -I tools/lift/check_ctor_slot_559.py --tamper > local/validation/orch-20261028/m6/slot-559-tamper.log 2>&1
python3 -I tools/lift/check_ctor_slot_569.py > local/validation/orch-20261028/m6/slot-569-checker.log 2>&1
python3 -I tools/lift/check_ctor_slot_569.py --tamper > local/validation/orch-20261028/m6/slot-569-tamper.log 2>&1
```

Not run: the three unwired native fixtures for 448/507/539, the full project CTest/checker sweep, startup
registration, or any real-game boot/audio/save validation.

## Integrator wiring for the five verified slots only

`native/lanes/m6.cmake` already includes their implementations and tests. Add these includes in
`native/core/startup_ctors.hpp`:

```cpp
#include "ctor_slot_544_d1b20.hpp"
#include "ctor_slot_557_47490.hpp"
#include "ctor_slot_558_428a0.hpp"
#include "ctor_slot_559_3f2d0.hpp"
#include "ctor_slot_569_1cc80.hpp"
```

Add in-place `StartupState` members:

```cpp
Slot544Objects slot544{}; Slot544Result slot544_result{};
Slot557Objects slot557{}; Slot557Result slot557_result{};
Slot558Objects slot558{}; Slot558Result slot558_result{};
Slot559Objects slot559{}; Slot559Result slot559_result{};
Slot569Objects slot569{}; Slot569Result slot569_result{};
```

Add `CtorSlot` rows at their corresponding original indices in `startup_ctors.cpp`:

```cpp
{0xd1b20, 544, [](StartupState& s) noexcept { s.slot544_result = init_slot544_d1b20(s.slot544); if (!s.slot544_result.ok) s.init_failed = true; }},
{0x47490, 557, [](StartupState& s) noexcept { s.slot557_result = init_slot557_47490(s.slot557, s.concmd); if (!s.slot557_result.ok) s.init_failed = true; }},
{0x428a0, 558, [](StartupState& s) noexcept { s.slot558_result = init_slot558_428a0(s.slot558, s.concmd); if (!s.slot558_result.ok) s.init_failed = true; }},
{0x3f2d0, 559, [](StartupState& s) noexcept { s.slot559_result = init_slot559_3f2d0(s.slot559, s.concmd); if (!s.slot559_result.ok) s.init_failed = true; }},
{0x1cc80, 569, [](StartupState& s) noexcept { s.slot569_result = init_slot569_1cc80(s.slot569, s.concmd, s.debug_menu); if (!s.slot569_result.ok) s.init_failed = true; }},
```

Do not register slots 448/507/539/379/518 until their missing checks and lane targets exist and pass. This report
does not wire any slot or advance G1.

## Checkpoint

- Task: `CPU-01-STARTUP-SLOT-LANES-M1-M6-INTEGRATE-01` (M6 validation subtask).
- Files changed: this report only. No M6 source, checker, test, CMake, original input, dependency, or shared
  integration file changed.
- Exact verification: commands and exit/count results above; private logs under `local/validation/orch-20261028/m6/`.
- Remaining risks: five of ten M6 entries remain unsupported by a static checker/replay; original constructor
  registration remains incomplete. No boot/audio/save claim.
- Next action: integrate only these five M6 slots after root's independent lane review, then handle missing M6
  entries as separate bounded work. Root owns shared handoff and publication.
- Status: `PARTIAL`; 0 failed verification hypotheses; no implementation attempt made on the five missing entries.
