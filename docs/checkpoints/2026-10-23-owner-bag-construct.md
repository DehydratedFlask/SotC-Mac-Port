# Checkpoint — native observable `owner_bag_construct`

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-OWNER-BAG-CONSTRUCT-01`
- Gate: **FIXTURE_VERIFIED**

## Sequence

`owner_bag_construct(OwnerPropBag&, OwnerBagConstructHooks)` models the observable `0xa2afb0`
order:

1. `scePadInit` hook;
2. initial-user hook (default -1);
3. gesture-init hook;
4. login-id-list hook; failure leaves one entry;
5. `select_bag_users` (1..4, negative/dedupe/cap);
6. `+0x10 = 1` last on every path;
7. `*(0x3305568)` publish hook.

The four 0xc88 input devices are opaque and null-safe for every analyzed reader, so the native model
records `tables_created`/`user_ids` but does not allocate them.

## Fixture

- Full hooks: initial 5 + login {0,1,2,3} → `{5,0,1,2}`, 4 tables, mode 1, publish called with mode
  1.
- Missing initial hook: `ok=false` but the unconditional site-A `{-1}` entry is still recorded and
  mode is still 1.

## Checker

`check_fowner_bag_ctor.py` now also checks the `OwnerBagConstructHooks`/`owner_bag_construct`/
`mode_10` tokens and the `set_mode_10`/`publish` source path.

## Limits

The 0xc88 device ctor/dtor and the live user-service calls remain contracts. The native bag does
not own input devices; input is not part of the startup report path.
