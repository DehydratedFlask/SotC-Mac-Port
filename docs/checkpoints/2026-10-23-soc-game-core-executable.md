# Checkpoint — native `SoCGameCore` auxiliary executable

- Local: 2026-10-23 (America/Chicago)
- Agent: code worker; orchestrator verification
- Task: `APP-01-SOCGAMECORE-EXE-01`
- Gate: **FIXTURE_VERIFIED** for the protocol and exit codes; **NOT RUN** for an engine

Later slice: E618 adds three constructors, so the live count is 311/573 and the first gap is 298
(`0xb0e250`). The status JSON below is the measurement at this slice.

## What exists

`native/app/soc_game_core_main.cpp` + `native/app/core_status_json.{hpp,cpp}` build a native
arm64 Mach-O executable named `SoCGameCore` (CMake target `soc_game_core`, output name
`SoCGameCore`) linked against the versioned ABI (`sotc_api`) and `sotc_core`.

Protocol v1, one JSON object per line on stdout:

| mode | behaviour |
|---|---|
| `--status` | `sotc.core.status.v1`, exit 0 |
| `--coverage` | `sotc.core.coverage.v1`, exit 0 |
| `--validate <dir>` | `sotc.core.validate.v1`; exit 0, 5 on a hard I/O verdict |
| `--start` | `sotc.core.start.v1` with `ok=false`, error 4, exit **3** |
| `--serve` | `<id> <command> [argument]` lines, one JSON line per request, ids first |
| `--fixture <path>` | sets the slot-23 name fixture before any constructor runs |
| `--assets <dir>` | default assets directory for a one-shot start |

Measured `--status` at this tip:

```json
{"schema":"sotc.core.status.v1","abi":65536,"state":"idle","last_error":0,
 "last_error_name":"SOTC_OK","assets_validated":false,"assets_ready":false,
 "engine_implemented":false,"ctor":{"linked":true,"original_slots":573,
 "slots_registered":308,"slots_with_callback":308,"completed":308,"run_ok":true,
 "callback_failed":false,"blocked_on_rva":0,"first_unrecovered_index":296}}
```

- A missing fixture is fail-visible: `run_ok=false`, `callback_failed=true`,
  `blocked_on_rva=0xe71990`, `completed=23` (the slot-23 stop), exit 0.
- `--start` reports error 4 and exit 3. A bare `--start` also writes the ABI's immediate
  `SOTC_ERR_BAD_STATE` diagnostic to stderr; with `--assets` (validated) the ABI itself returns 4.
- `--serve` sequences (validate, start, status, stop, quit) answer with matching ids; after the
  refused start the state is `failed`, `last_error=4`, never `running`.

## Coverage rows

`startup_constructors` is PARTIAL with a computed detail (`N of 573 …, first unrecovered index I;
M registered slots ran OK`); `save_load` is PARTIAL for the fixture-verified transaction/metadata
only; `engine_boot`/`metal_renderer`/`audio` stay NOT_IMPLEMENTED. `metal_renderer` now says the
offscreen capture provider exists but no game frame is rendered. Nothing reports IMPLEMENTED; the
`sotc_ctor_prefix` ABI is unchanged.

## Tests

- `test_core_status_json` pins exact JSON, escaping and null handling.
- `tools/lift/check_fcore_status.py` spawns the real binary and pins schemas, key order, exit
  codes, serve framing, and the missing-fixture path. Registered as five ctests.
- Full ctest 381/381 at the round tip; the checker sweep includes the new checker.

## Limits

There is no engine: `engine_implemented` is always false and `start` never succeeds. `run_ok=true`
means the registered constructor subset ran, not all 573. The fixture is private user-derived data;
a packaged core without it reports `run_ok=false`. No Metal, audio or save claim.
