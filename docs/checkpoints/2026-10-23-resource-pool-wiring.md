# Checkpoint — ResourcePool wired into the nine 0x533100 attach sites

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-F590-533100-RESOURCE-POOL-WIRED-01`
- Gate: **FIXTURE_VERIFIED**; not wired into a launcher

## Corrected reading (E595)

The earlier fragment model read `[out]` as a caller-supplied `loaded` pointer and `[out+8]` as a
caller "success flag", then called an invented `notify_95580`. Byte decode of `0x533461..0x533582`
shows that `[out]`/`[out+8]` are the `ResourceRef{ptr, owned}` sret of `GetResource`, and `owned` is
always 0. The real order is:

1. `0x14c2f0` key;
2. `GetResource` (hit +1 / miss new wrapper refs 1);
3. `AddReference(W)` `0x14ccb0`;
4. dead `if (ref.owned) RequestState(13)` — `owned == 0`;
5. join `0x5334f9`: `if (S.ptr != W) { if (S.owned) ReleaseDependency(old); S.ptr = W;
   AddReference(W); RemoveReference(old); }` (the report's `S.ptr != null_wrapper && S.ptr != W`
   condition is corrected: the null-wrapper compare only gates the old release, not the assign);
6. dead `if (used) { Lock(S); ReleaseDependency(W); }`;
7. `RemoveReference(W)`;
8. dead `if (ref.owned) ReleaseDependency(W)`;
9. `RemoveReference(W)`;
10. release the caller key (`0xf590`);
11. `Lock(S, 13)` for sites 1-8; `Lock(S, 9)` at the site-9 tail `0x53403c`.

Steady state after a first attach: `refs == 1`, `deps == 1`, one map node holding the wrapper key.
A second attach with the same key changes neither `deps` nor the map size.

## Native model

- `LockedResourceRef` (`resource_pool.*`): `assign` / `lock` / `release`. Slots start
  `{null_resource_wrapper(), false}`; a zeroed slot is initialized explicitly by
  `vslot_40_533100_init_slots(f08, self)` (the original app-ctor slot initialization is not lifted).
- `vslot_40_533100_attach_common.*`: one implementation of the recovered sequence, with hooks for
  the pool, allocation, payload, manager delete, engine state write and the key allocator/release.
  A missing pool or slot is a visible `ok == false`.
- Nine fragments now pin their literal, extent and slot offset and delegate to the common model.
  Site 9 defers the lock to `vslot_40_533100_tail_140d90`, which can now lock the real slot
  (`lock_resource_ref`, priority 9). Site 8 locks priority 13.
- The appmain ctor slice owns one native pool plus the nine slots and asserts: miss key release
  `previous == 3` (caller + wrapper `+0x70` + map node), `refs == 1`, `deps == 1`, state 11; a second
  attach is a hit (`previous == 1`, same wrapper, idempotent lock); site 9 state 9; teardown frees
  all nine wrappers, empties the map, and balances allocate/free at 9/9.

## Verification

- `ctest --test-dir local/build/orch-r22 -j 12`: 364/364 passed.
- `SOTC_NATIVE_BUILD=local/build/orch-r22 tools/lift/run_all_checkers.sh`: see E595 (all nine
  rewritten checkers pin the new model).
- Nine old checkers that asserted the withdrawn success/notify reading were replaced.

## Mutation

- Removing `add_reference` from `LockedResourceRef::assign` aborts under ASan (the slot's only
  reference is lost).
- Skipping the second `RemoveReference` in the common attach fails the `refs_after == 1` check.
- Both mutations were restored; the full suite is green again.

## Limits

- The pool map is non-owning and single-threaded; the recursive mutex is not modelled.
- The key allocator is a host hook; production must supply a real allocator.
- The `if (ref.owned)` paths remain dead in this build and are documented, not executed.
- The ResourceManager delete delegation, payload factory and engine state notification remain hooks.
