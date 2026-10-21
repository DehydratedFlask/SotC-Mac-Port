# Checkpoint: CPU-01-NATIVE-SLOT11-01 — slot 11 (`0xf09160`) registered natively; runner now stops at slot 12 (`0xf06340`)

- Time / timezone: 2026-10-21 (America/Chicago), macOS 27.0.1 arm64. Agent I. Owned `native/` and
  `tools/lift/`. `HANDOFF.md`, `docs/TASKS.md`, `docs/EVIDENCE.md`, `INSTALL_LOG.md` were **not** edited;
  nothing committed or pushed; no Ghidra/REA session (all byte facts re-derived from the pinned ELF by the checker).
- Task / status: CPU-01-NATIVE-SLOT11-01 **FIXTURE_VERIFIED** (native model + tests + ELF-anchored checker).
  0 failed tries (no escalation). G0 PARTIAL; G1-G6 NOT RUN. **No claim of engine startup or G1.**
- Spec: `2026-10-21-agentG-ctor11-reflection-scope.md` (interface sketch) and
  `2026-10-20-root-cpu01-ctor11-structural-analysis.md`. Pattern: slot 10.

## What changed

New (all `native/`):
- `core/lazy_param_def_tree.{hpp,cpp}` — `LazyParamDefTree` (double-checked, `std::atomic<bool>`
  release/acquire, borrowed `RecursiveMutex`), `ParamTreeError {none, schema_not_available,
  allocation_failed, mutex_unavailable}`, placeholder `ParamDefTree {node_count}` (no node layout invented).
  `get()` never returns an empty-but-successful tree; failures are not cached; `release()` = original `0xf08d30`.
- `core/clothing_physical_mesh_parameters.{hpp,cpp}` — `ClothingPhysicalMeshParameters` (size `0x1b8`, align 8,
  base `ClothingParamBase` `0x48` + `PhysicalMeshData` at `+0x48`, 12 `ParamArrayBlock {ptr, owns, stride,
  count}` at offsets `0x60,0x78,0x90,0xa8,0xc0,0xd8,0xf0,0x110,0x140,0x158,0x178,0x190`, strides
  `12,12,12,12,2,4,1,4,40,52,40,52`), dtor/`reset()` release blocks through `ResourceOwner::release_derived`
  (original `0xf06a40`), `build_clothing_physical_mesh_schema` (always `schema_not_available`, allocates nothing),
  `kClothingPhysicalMeshParametersDescriptor`, and `init_clothing_physical_mesh_descriptor` (slot 11).
- `tests/test_lazy_param_def_tree.cpp`, `tests/test_clothing_physical_mesh_parameters.cpp`.

Edited: `core/startup_ctors.{hpp,cpp}` (slot 11 callback; new explicit blocker row `{0xf06340, 12, nullptr}`;
`StartupState::physmesh_registration` then `physmesh_tree`, declaration order matters), `tests/test_startup_ctors.cpp`
(null-slot-11 expectation replaced, not deleted), `CMakeLists.txt`, `README.md`, `tools/lift/check_ctor_slots.py`.
Side effect: `python3 scripts/project_index.py` (mandatory) rewrote `docs/FOLDERS.md`.

## Behavior and deliberate decisions

1. **Slot 11 callback** = original shape: stack probe `ClothingPhysicalMeshParameters(nullptr, 0x10, 0x10)` (ctor
   skips default init because `field20 != 0`; native object is fully zero there, which equals the original because
   slot 11 zeroes its stack first) + destructor, then `DescriptorRegistration::publish(&descriptor)` which owns the
   recursive + priority-inherit mutex. Flag `0x3356070` equivalent (`physmesh_tree.built()`) stays false; the 74 records
   are never built.
2. **Idempotence:** re-running the slot callback with the same descriptor returns true (the `already_published` result of
   a direct second `publish()` is still asserted). A different already-published descriptor, or a mutex that cannot be
   initialized (`mutex_init_failed`), sets `init_failed`, so the runner reports `callback_failed` at `0xf09160`; both are tested.
3. **Deviation from the sketch (documented):** `BuildFn` takes `std::unique_ptr<ParamDefTree>&` (the builder allocates)
   instead of `ParamDefTree&`, so the unavailable path provably allocates nothing. The descriptor does not carry a
   `LazyParamDefTree*`: the tree needs the state-owned mutex, so it is `StartupState::physmesh_tree`. Added
   `mutex_unavailable` so using the tree before the descriptor is published fails visibly instead of locking an
   uninitialized mutex.
4. **Not invented:** scalar/array field names, the 74 records, names, hints, type codes. Fields are offset-named; the
   scalar granularity of the `0x54..0x63` and `0x128..0x137` zeroing stores (u32 words) is inferred; `+0x50` is never
   written by the original ctor/reset (zero here).
5. **Slot 12 blocker row.** Without it the runner would silently jump 11 -> 20 (slots 12-19 are unregistered).
   `{0xf06340, 12, nullptr}` makes it stop with `completed == 12`, `blocked_on_rva == 0xf06340`.
   Slot 12 is a descriptor+mutex+atexit initializer (vtable `0x15f9ef0`, globals `0x3355f88`/`0x3355f98`) per
   `2026-10-21-root-cpu01-slot12-struct.md`; it is not lifted here (out of scope).

## Commands and results (CWD `/Volumes/Extra/SoC Mac Native`; build dir `local/build/native-agent-i`)

| command | exit / result |
|---|---|
| `cmake -S native -B local/build/native-agent-i -G Ninja` | 0 (`local/build/native-agent-i-configure.log`) |
| `cmake --build local/build/native-agent-i` (-Wall -Wextra -Werror, ASan+UBSan) | 0 (`...-build.log`); first attempt failed on `-Wunnecessary-virtual-specifier` (`final` + virtual `reset`), fixed by dropping `final` |
| `ctest --test-dir local/build/native-agent-i -j8` | 0; **292/292 passed** (was 290; +`lazy_param_def_tree`, +`clothing_physical_mesh_parameters`) (`...-ctest.log`) |
| `SOTC_NATIVE_BUILD=local/build/native-agent-i` + every `tools/lift/check_*.py` (`python3 -I`) | **286/286 exit 0**, 0 failures (`...-checkers.log`) |
| `python3 -I tools/lift/check_ctor_slots.py` | 0: 109 registered RVA/index pairs, slot 11 recovered, first unresolved index 12 `0xf06340` |
| `python3 scripts/project_index.py` | 0 |

Negative controls (all run, then restored; final tree re-built and 292/292 green):
- slot 11 callback set back to `nullptr`: `startup_ctors` and `clothing_physical_mesh_parameters` ctest FAIL; checker fails
  ("indices 0-11 must have callbacks").
- header stride `52 -> 51` for block 11: checker fails ("native block strides differ"); with matching constants the ELF byte check
  fails ("block 11 qword stride/count store missing").

## Checker additions (`tools/lift/check_ctor_slots.py`)

Still verifies every registered RVA/index pair against the ELF table (109 pairs). Now also: indices 0-12 contiguous;
0-11 have callbacks; 11 is `0xf09160` and calls `init_clothing_physical_mesh_descriptor`; 12 is the blocker `0xf06340`
and the first null row; 13-19 and 21 absent; body sha256 of `0xf09160` (281 B), `0xf06410` (1021 B), `0xf069f0` (44 B),
`0xf06340` (205 B); header `kBlockOffsets`/`kBlockStrides` equal the verified lists; and for each of the 12 blocks the
ELF constructor `0xf06410` and reset `0xf08f00` contain the owns=1 / stride / count=0 stores (disp8 or disp32 `mov`
encodings; last block as one REX.W qword store).

## Observation vs inference / limits

- Observed (tests + checker): native layout asserts (`offsetof`, `sizeof == 0x1b8`), stores in the ELF, runner behavior,
  tree error semantics, zero allocations on the unavailable path (global `operator new` counter), single build under 8
  racing threads.
- Inferred: the semantic role of every field; that slot-10-style `StartupState` ownership is the intended long-term shape.
- NOT done: reflection records/names (principal decision on schema provenance still open), any consumer of the tree,
  factory/`vslot 124` path, the 34-slot family beyond 10-11, serializer-side getter callers.
- Native objects are not binary-compatible with the original and no original address is used for dispatch.

## Next exact action and recommended TASKS.md row

Next: slot 12 (`0xf06340`, RVA verified by the checker), same descriptor+mutex shape with vtable `0x15f9ef0`; it needs a
class model first (only base ctor `0xe1eda0` + vptr + dtor `0xe1ede0` are called in its body). Indices 13-19 still need analysis.

`CPU-01-NATIVE-SLOT11-01 | FIXTURE_VERIFIED | 0 failed tries | slot 11 registered (ClothingPhysicalMeshParameters descriptor + DescriptorRegistration mutex; lazy schema = schema_not_available); runner stops at idx 12 RVA 0xf06340 after 12 slots | ctest 292/292, checkers 286/286 | docs/checkpoints/2026-10-21-agentI-native-slot11.md`
