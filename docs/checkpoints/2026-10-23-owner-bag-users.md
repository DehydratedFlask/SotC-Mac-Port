# Checkpoint — bag input-device user selection

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-OWNER-BAG-USERS-01`
- Gate: **FIXTURE_VERIFIED**

## Function

`select_bag_users(initial_user, login_ids[4], login_list_ok)` models `0xa2afb0`'s 1..4 table
selection:

- site A is unconditional and stores `initial_user`, even when it is -1;
- `login_list_ok == false` leaves exactly that one entry;
- negative login ids are skipped;
- only a login id equal to the initial user is deduped; login ids are **not** deduped among
  themselves;
- sites D/E check `r13 <= 2` (`r13 == count - 1`), so at most four entries are stored and the last
  of four distinct login ids is dropped.

## Fixture

- no user / failed list → 1 entry `{-1}`;
- `initial=-1, login={10,11,12,13}` → `{-1,10,11,12}` (13 dropped);
- `initial=10, login={10,11,12,13}` → `{10,11,12,13}` (initial deduped, cap allows all);
- negatives skipped; duplicate login ids preserved;
- the report's exact cap case `initial=5, login={0,1,2,3}` → `{5,0,1,2}`.

## Checker

`check_fowner_bag_ctor.py` pins `0xa2afb0` (441 B, sha `252b37a3…`), the base/PadInit/user-service/
gesture/pad call targets, both `cmp r13d,2 / jg` sites, the `+0x10 = 1` write and the `0xc88`
allocation.

## Limits

The 0xc88 input-device object and its ctor/dtor are not lifted. The function is the user-selection
logic only; the native bag does not own the devices yet.
