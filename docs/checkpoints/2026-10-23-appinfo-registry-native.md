# Checkpoint — native app-info registry, owner/dispatch and container destructor

- Local: 2026-10-23 08:1x (America/Chicago)
- Agent: integrator
- Tasks: `CPU-01-APPINFO-REGISTRY-NATIVE-01`, `CPU-01-CONTAINER-DTOR-1BEDE0-01`
- Gate: **G1 PARTIAL — fixture-level native code; nothing executed against the game**
- Evidence: E564

## What was added

| File | Contents |
|---|---|
| `native/core/app_info_registry.{hpp,cpp}` | Native owner-side wrapper over the 0x60 B container: list-A sentinel view, entry count, `add_string_entry`, `add_callable_entry`, `clear`. String entries use non-owning `{ptr,len,0,null}` holders, the same shape the observed sites build. |
| `native/core/app_info_install.{hpp,cpp}` | Native counterpart of `0x25a4d0` (four product-info inserts in recovered order, platform/debug/camera selection) and `0xb10` (game entry). Values are host-supplied, borrowed views; unresolved producers are contracts. |
| `native/core/registry_owner.{hpp,cpp}` | Native counterpart of the `0x2569d0` registry block: one container plus list-3 delegate registration through the lifted `0x1beeb0` and a byte-checked FIFO dispatcher for `0x1bdf30`'s list-3 loop. |
| `native/core/container_dtor_1bede0.{hpp,cpp}` | 208 B container destructor: list C (vtable slot 3 = `0x19f0` bare ret) then lists B/A (`0xf590` ×2 each), freeing through tag 2, with sentinel/count reset. |
| `native/tests/test_app_info_registry.cpp` | ctor layout, string/callable insert, flag=1 skip, flag=0 erase, failure surface. |
| `native/tests/test_app_info_install.cpp` | five-entry order, NEO/STANDARD, debug True/False, camera conditional, failure site, borrowed ownership. |
| `native/tests/test_registry_owner.cpp` | FIFO delegate order, real `init_1290` handler coexistence, identity-only skip, null-fn failure, missing allocator. |
| `native/tests/test_container_dtor_1bede0.cpp` | C/B/A teardown counts, 6 releases, payload override, missing-hook failure. |
| `tools/lift/check_fcontainer_dtor_1bede0.py` | Pins 0x1bede0 extent/hash, the three loops, four `0xf590` calls, three `0x170c0` frees and list-C slot 3 = `0x19f0`. |
| `tools/lift/check_fapp_info_install.py` | Pins 0x25a4d0/0xb10 extents/hashes, five insert calls, value producers and twelve strings. |

## Design decisions

1. **Borrowed views by default.** The original shares refcounted buffers. The native model keeps
   `control == nullptr` for all product-info values and requires the supplied `string_view` to
   outlive the registry. This is stated in the header; it does not claim the original never owns.
2. **Identity-only nodes are never called.** `0x1290` stores original RVAs in its list-C handlers.
   Native delegates are tagged by `fn == 0`; dispatch skips nodes with non-zero `fn` and counts them
   in `skipped_identity`. The native dispatcher cannot execute a guest address.
3. **FIFO traversal.** `test_registry_owner` asserts the byte-derived order: product info first,
   "Game" last, final list front-to-back `Game, Camera, Debug, Platform, Build`.
4. **Container destructor resets the sentinels/counts.** The original destroys the object; the reset
   is a native reuse extension, documented in the header.

## Verification

```
cmake -S native -B local/build/orch-r22        # exit 0
cmake --build local/build/orch-r22 -j 12       # exit 0
ctest --test-dir local/build/orch-r22 -R '^(app_info_registry|app_info_install|registry_owner|container_dtor_1bede0)$'
python3 -I tools/lift/check_fcontainer_dtor_1bede0.py
python3 -I tools/lift/check_fapp_info_install.py
```

Both checkers exit 0 on `local/re/eboot.elf` sha256 `d0723afb…861d3`; the four fixtures pass under
ASan+UBSan. End-of-round full-suite and sweep counts are recorded in the round verification
checkpoint.

## Limits

`0x2569d0` itself is not lifted: only its `0x256f98..0x257021` registry block is modelled. The other
three callback installs, the previous-container teardown, camera chain, `0x10a90`, `0x3f4e0`,
`0x120c0`, `0x1c70b0/0x1c0f40/0x216480/0x50ba0`, list-A readers and dispatch timing remain open.
**G1 PARTIAL.**
