# Checkpoint: CPU-01-NATIVE-SLOT12-19-01 — constructor slots 12-19 registered natively; runner now stops at slot 23 (`0xe71990`)

- Time / timezone: 2026-10-21 (America/Chicago), macOS 27.0.1 arm64. Agent J. Owned `native/` and `tools/lift/`.
  `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md` were **not** edited; nothing committed or pushed;
  no Ghidra/REA session (all byte facts re-derived from the pinned ELF `d0723afb...5861d3` by the checkers).
- Task / status: CPU-01-NATIVE-SLOT12-19-01 **FIXTURE_VERIFIED**. 0 failed tries per slot (no escalation).
  G0 PARTIAL; G1-G6 NOT RUN. **No claim of engine startup or G1.**
- Inputs: slot 12/13/14-19 structure dossiers, ctor-family map, family-native-helper, agent I slot-11 checkpoint.

## Result in one paragraph

All eight thunks (slots 12-19) are registered natively **as registration only**. The task note said not to model the
forwarding callees (`0xf011f0 0xefe410 0xefd140 0xefad20 0xef8600 0xf030e0`) unless resolved; they were resolved *for the
thunk's use* by replaying the bytes, not by identifying the classes. The first still-unresolved slot is now **index 23
(`0xe71990`)**, an explicit null row; the runner stops there after 23 completed rows (indices 0-22).

## What the original thunk does (observed, replayed from the bytes)

New offline tracer `tools/lift/thunk_trace.py` (a throw-away abstract machine over 23 x86-64 mnemonics; stack bytes must be
written before read; imports intercepted by name; every store outside stack/fake heap is an event) runs each thunk and its
callees from the ELF. `tools/lift/check_ctor_thunks.py` asserts for all eight slots that the ONLY visible effects are, in order:

1. `store [A] = V` (V = vtable address point, 8 bytes; A a `.bss` global),
2. `malloc(0x10)` (size word `[0x12d2d1c] == 0x10` read from the image),
3. `store [A+0x10] = malloc result`,
4. `pthread_mutexattr_init(attr)`, `settype(attr, 2)`, `setprotocol(attr, 1)`, `pthread_mutex_init(block, attr)`,
   `pthread_mutexattr_destroy(attr)` (executed from `0xb30950` itself, arguments read from the bytes),
5. `__cxa_atexit(0xe44a10, A+0x10, 0x1604218)`.

Everything else the thunk executes is an object built on its own stack: base ctor `0xe1eda0` with (owner=0, field20=0x10,
field28=0x10) — asserted on every call — which skips the default-state path; base dtor `0xe1ede0`; plus the per-shape part:

| slot | thunk RVA | A (`.bss`) | V | probe shape | class-specific work (all effect-free, executed and traced) |
|---|---|---|---|---|---|
| 12 | `0xf06340` | `0x3355f88` | `0x15f9f00` | `base_only` | none (base ctor, store V, base dtor) |
| 13 | `0xf058f0` | `0x3355470` | `0x15f9298` | `callee_inline` | `0xf030e0` constructs, runs guarded inline dtor (count=0, owns=0), returns V |
| 14 | `0xf02ff0` | `0x3355028` | `0x15f8670` | `vslot0_dtor` | ctor `0xf011f0`; `call [V]` = `0xf01510` (guarded releases, `0xf01610`) |
| 15 | `0xf01100` | `0x3354e00` | `0x15f7a28` | `base_only` | none |
| 16 | `0xf00090` | `0x3354b58` | `0x15f6dc0` | `vslot0_dtor` | ctor `0xefe410`; `call [V]` = `0xefe820` (-> `0xefe890`) |
| 17 | `0xefe320` | `0x33549f0` | `0x15f63b8` | `vslot0_dtor` | ctor `0xefd140`; `call [V]` = `0xefd470` (-> `0xefd4c0`) |
| 18 | `0xefd020` | `0x3354660` | `0x15f54f8` | `vslot0_dtor` | ctor `0xefad20`; `call [V]` = `0xefb070` (-> `0xefb1c0`, `0xefb2b0`) |
| 19 | `0xefacd0` | `0x3354190` | `0x15f4448` | `callee_inline` | `0xef8600` (memset of its own 0x208-byte stack frame, construct, guarded dtor), returns V |

V has two zero header words (no RTTI) and `[V]` is the slot-0 destructor in the image. Every `call [rax+0x10/0xc0/0xd0]` owner-vtable
call in those functions is behind a zero-guard that is false for the zeroed probe; negative control (a) shows the tracer *does*
record such a call when the guard bytes are set (fake owner, `owns=1`, ptr `0x1234`), (b) shows the zeroed object makes none, (c)
shows an uninitialised stack read faults.

## Native model (deliberately narrow)

- `core/registration_thunks.{hpp,cpp}`: `RegistrationThunkDescriptor {ctor_index, original_thunk_rva, original_vtable_rva,
  original_global_rva, probe_shape}` (identity only, never dispatched), `kRegistrationThunkDescriptors[8]` (constexpr, static_assert
  consistency), `init_registration_thunk(StartupState&, 12..19)`: shared base-object probe `ClothingParamBase(nullptr,0x10,0x10)`
  construct+destroy, then `DescriptorRegistration::publish(&descriptor)` on `StartupState::thunk_registrations[i]` (owned recursive +
  priority-inherit mutex, settled slot-10 decision). Idempotent for the same descriptor; a different pre-published descriptor, a
  mutex-init failure, or an index outside 12-19 sets `init_failed` (runner reports `callback_failed` at that slot).
- **Not modelled / what the original also does:** the class behind each V (unidentified; no layout, name, hooks); the derived ctor/dtor
  bodies of 13-19 (effect-free here, not lifted); the 16-byte heap block, its size word and atexit order; V's numeric value. The base type
  is named `ClothingParamBase` only because slots 10/11 were recovered first; `0xe1eda0/0xe1ede0` is shared by unidentified classes.
- Because the thunks leave the runner unblocked, two further edits were required so it does not report a false success: slot 21
  (`0xee1130`, already-lifted `const_table_ee1130`) is now registered (`StartupState::const_table_ee1130`), and slot 23 (`0xe71990`) is the
  explicit null blocker (needs the `0xe70aa0` name hash AND the 14 original strings, an open data-import decision). Without it the runner
  would have returned `ok()` after silently skipping 23-47.

## Files

New: `native/core/registration_thunks.{hpp,cpp}`, `native/tests/test_registration_thunks.cpp`, `tools/lift/thunk_trace.py`,
`tools/lift/check_ctor_thunks.py`. Edited: `native/core/startup_ctors.{hpp,cpp}` (8 thunk rows, slot 21 row, slot-23 null row, two new
`StartupState` members), `native/tests/test_startup_ctors.cpp` and `native/tests/test_clothing_physical_mesh_parameters.cpp`
(expectations updated: `completed 12 / 0xf06340` -> `23 / 0xe71990`; "13-19 and 21 absent" -> present), `native/CMakeLists.txt`,
`native/README.md`, `tools/lift/check_ctor_slots.py` (118 pairs; contiguous 0-23; only null row = 23; per-slot `init_registration_thunk(s, N)`
callbacks; slot-21 callback; body sha256 of the eight thunks and `0xe71990`; native table RVAs equal the ELF constructor table).
Side effect: `python3 scripts/project_index.py` rewrote `docs/FOLDERS.md`.

## Commands and results (CWD `/Volumes/Extra/SoC Mac Native`; build `local/build/native-agent-j`)

| command | exit / result |
|---|---|
| `cmake -S native -B local/build/native-agent-j -G Ninja` | 0 (`local/build/native-agent-j-configure.log`) |
| `cmake --build local/build/native-agent-j` (-Wall -Wextra -Werror, ASan+UBSan) | 0 (`...-build.log`) |
| `ctest --test-dir local/build/native-agent-j -j8` | 0; **294/294 passed** (+1 `registration_thunks`; the tree also holds another worker's `save_chunk_codec`) (`...-ctest.log`) |
| `SOTC_NATIVE_BUILD=local/build/native-agent-j python3 -I <each tools/lift/check_*.py>` | **287/287 exit 0** (286 + `check_ctor_thunks.py`) (`...-checkers.log`) |
| `python3 -I tools/lift/check_ctor_thunks.py` / `check_ctor_slots.py` | 0 / 0 (118 RVA/index pairs; first unresolved index 23) |
| `python3 -I tools/lift/verify_slot12_13.py`, `verify_slot_family_14_19.py` | 0 / 0 (earlier dossiers still hold) |
| `python3 scripts/project_index.py` | 0 |

Native negative controls (each run, then restored byte-for-byte; final tree rebuilt, 294/294): slot 14 callback -> `nullptr`: `startup_ctors`,
`registration_thunks`, `clothing_physical_mesh_parameters` ctest FAIL and `check_ctor_slots.py` fails ("indices 0-22 must have callbacks");
`publish()` replaced by a stub success: `registration_thunks` and `startup_ctors` FAIL; header V of slot 16 changed by 8: `check_ctor_thunks.py`
fails ("native row 16 differs from the traced values").

## Observation vs inference / limits

- Observed: the nine effects per slot, the base-ctor arguments, the vtable-slot-0 targets, and the effect-freedom of the probes, all by tracing
  the pinned bytes. The tracer is an offline verification aid for a fixed small instruction subset, not an emulator of the game; it is not
  linked into the product and cannot prove anything about code it did not execute (guard-false branches are *not* proven dead, only not taken
  under owner = null / zeroed object, which is exactly how the thunks call them).
- Inferred: that V is a class vtable (no RTTI), that A/A+0x10 are (descriptor, mutex) globals; consumers of A are unknown, so whether other code
  expects numeric V or a mutex *pointer* at A+0x10 is not covered.
- NOT done: any class model for 12-19, consumers, ordering vs the atexit stack, slots 23-47, `trail` rescoping (unchanged: no new top-level dir).

## Next exact action and recommended TASKS.md row

Next: slot 23 (`0xe71990`): resolve the name-hash string import decision (djb2-XOR `0xe70aa0` is native `name_hash`), then 24-47 can reuse
`DescriptorRegistration` (thunk shapes 79/205-284/233 B; the same tracer can verify them).

`CPU-01-NATIVE-SLOT12-19-01 | FIXTURE_VERIFIED | 0 failed tries | slots 12-19 registered natively as registration-only (opaque descriptor + owned recursive/PI mutex; classes behind V unidentified) + slot 21 registered + explicit null row slot 23 (0xe71990); runner stops after 23 rows | ctest 294/294, checkers 287/287 (new check_ctor_thunks.py, tracer tools/lift/thunk_trace.py) | docs/checkpoints/2026-10-21-agentJ-native-slot12-19.md`
