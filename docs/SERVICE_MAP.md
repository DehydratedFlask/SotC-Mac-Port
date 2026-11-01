# Native service map (PLAT-01-SERVICE-MAP-01)

What PS4 system services this title imports, grouped into the areas our native layer must own.
This is the specification for the service layer, not a coverage claim: no area is implemented
beyond the modules listed in the "existing native modules" column.

- Source: the pinned plaintext ELF (`local/re/imports.json`, SHA-256 `d0723afb…61d3`).
- Generated table: [`service_map.json`](service_map.json) — library names, symbol names, NIDs,
  types and the manifest's own resolution status. No game code, bytes or addresses.
- Regenerate/verify: `python3 tools/lift/gen_service_map.py` / `--check`
  (the latter runs as `tools/lift/check_service_map.py` in the checker sweep).

## Totals

| Metric | Value |
|---|---|
| Libraries referenced | **51** |
| Imported symbols | **588** (manifest `symbol_count` 590) |
| Resolution status | 436 `contract-declared`, 152 `name-by-nid-hash-only` |
| Libraries with **zero** imports | 17 (listed below) |
| Service areas | **22** |

Seventeen libraries are referenced but import nothing in this build — including all
`libSceNp*` (network platform), `libSceVoice`, `libSceAudiodec`-adjacent `libSceAjm`,
`libScePlayGo`, `libSceRemoteplay`, `libSceRazorCpu`, `libSceCoredump`,
`libSceGameCustomDataDialog` and `libSceAudioIn`. Nothing in those may be treated as required by
this title.

## Areas

| Area | Imports | Libraries | What it forces | Existing native modules (hint only) |
|---|---:|---:|---|---|
| `libc` | 186 | 2 | Memory/string/math/printf semantics, `strcasecmp`, `__stack_chk_fail` | `native/core/lookup_1fc80.*`, allocator modules |
| `gnm` | 103 | 1 | PM4 submission, draws, compute, shader stages, resource registration, workload streams | `native/platform/pm4_decoder.*`, `pm4_state.*`, `pm4_indirect.*` |
| `kernel` | 66 | 1 | Threads, mutex/semaphore/cond, TLS, events, memory mapping, time | `native/core/allocator_*`, `critsec_*`, `thread_*`, `tls_*` |
| `net` | 61 | 16 | Sockets, SSL, HTTP, Np; online features | none |
| `file_io` | 34 | 1 | `libSceFios2` async file I/O and path resolution | `scripts/psarc.py` (archives only) |
| `video_out` | 21 | 1 | Display open, buffer registration, flip, vblank/flip events | none |
| `save` | 20 | 2 | `SaveData` transaction, mount, dialog status | `native/core/save_*` |
| `audio_out` | 18 | 4 | Audio output ports, mix levels, voice | `native/core/audio_*` |
| `input` | 18 | 2 | Pad, system gesture | `native/core/input_*`, `input_table_551140.*` |
| `dialog` | 16 | 5 | Message/IME/error/custom-data dialogs | none |
| `audio_ngs2` | 11 | 1 | NGS2 voices, samplers, reverbs | `native/core/audio_*` |
| `fiber` | 8 | 1 | User fibers: init, run, switch, finalize | `native/core/thread_*` |
| `system` | 6 | 2 | System service calls and coredump hooks | none |
| `user` | 5 | 1 | User ids and login state | none |
| `image` | 4 | 1 | PNG encoding | `native/platform/metal_capture.*` |
| `share` | 4 | 3 | Share utilities, live streaming, video recording | none |
| `app_content` | 2 | 2 | Add-on content enumeration, PlayGo | none |
| `screenshot` | 2 | 1 | In-game screenshot | `native/platform/metal_capture.*` |
| `sysmodule` | 2 | 1 | Module load/unload | none |
| `rtc` | 1 | 1 | Real-time clock | none |
| `audio_ajm` | 0 | 1 | AJM decoder interface (unused by this build) | none |
| `tools` | 0 | 1 | Razor CPU profiling (unused by this build) | none |

## Reading the table

- **`gnm` + `video_out` are one system.** Between them they define the frame: 103 GNM imports for
  command submission and 21 VideoOut imports for display and flip. The boundary contract is in
  [GPU_COMMAND_SURFACE](GPU_COMMAND_SURFACE.md).
- **`kernel` + `fiber` + `libc` are the runtime floor.** 260 imports. Nothing above them can run
  until threads, TLS, atomics and allocation behave. The CritSec and allocator work already here
  is the start of that layer.
- **`save` + `file_io` are the persistence floor.** 54 imports. Save acceptance is a gate (G5), so
  the transaction model must be native from the start rather than shimmed later.
- **`net` is large but optional for the first frame.** 61 imports across 16 libraries; nothing in
  the boot-to-gameplay route requires them. Keep them out of the critical path.
- **Dialogs and share features can fail visibly** in a first vertical slice; they must not block
  the frame loop.

## Limits

- Import census only. A symbol being imported does not prove it is called on the boot or frame
  path.
- 152 of 588 names are `name-by-nid-hash-only`: the name is inferred from the NID hash, not
  declared by the binary. Any behavioural claim about those needs separate evidence.
- No library here is executed. Every area must be re-implemented natively; the HLE classification
  in the prior-art ports is a reference for behaviour, not code to run.
