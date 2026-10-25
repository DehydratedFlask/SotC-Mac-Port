# Checkpoint — native owner property bag

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-OWNER-PROP-BAG-01`
- Gate: **FIXTURE_VERIFIED**; the RB node bodies remain contracts

## Recovered bag

| item | value |
|---|---|
| owner field | `[this+0x3b48]` |
| size | `0x178` (`0xa2b966`) |
| factory | `0xa2b960`, 37 B, sha `f7bcfbea…` |
| derived ctor | `0xa2afb0`, 441 B, sha `252b37a3…` |
| base ctor | `0xac4de0`, 380 B, sha `73936f45…` |
| global alias | `lea rcx,[0x3305568]` at `0xac4f1c`, `mov [rcx],rbx` at `0xac4f33` |

The base ctor allocates four `0x30` sentinels and stores them at `+0x20`, `+0x38`, `+0x50`,
`+0x68`; the matching 0x18-byte handles are at `+0x18`, `+0x30`, `+0x48`, `+0x60`. The property
writers use handle A `+0x48` and handle B `+0x60`, matching E576.

## Native model

- `OwnerPropBag` holds two `std::multimap<std::uint32_t, ...>` trees. Multimap preserves duplicate
  keys and insertion order among equal keys, matching `insert_equal`; a `std::map` would drop the
  8 duplicate keys.
- `OwnerValueA {i32 i, f32 f, u32 tag}`, `OwnerValueB {f32 lo, f32 hi, f32 v, u32 tag}`.
- `OwnerPropBagSink` adapts the bag to `OwnerPropertySink`, so `apply_owner_properties` now feeds a
  real native object instead of a test-only sink.
- The model is behavioural, not the guest RB ABI: no guest code reads this memory.

## Fixture

- Gate 1: 134 inserts, 122 A + 12 B, 114 distinct A keys, eight duplicated keys `0x7b..0x82`,
  two `0x7b` rows in order (`i=0x0c`, then `i=0x11`), final B `{lo 0, hi 1, v 1, tag 5}`.
- Gate 0: 137 inserts, 125 A + 12 B, 117 distinct A keys.
- `none` inserts nothing; `mode2_unreachable` fails without inserting.
- Direct inserts show ordered iteration and duplicate preservation.

## Checker

`check_fowner_prop_bag.py` pins the factory/derived/base hashes, the `0x178` and `0x30`
allocations, the four sentinel stores, the `*(0x3305568)` republish, and the native constants.

## Limits

- Trees 0/1, the exact node layout, the RB fixups `0xac8a90`/`0xac8d40`, erase `0xac5aa0`, and the
  scatter `0xac62e0`/`0xac6360` remain contracts.
- The `0x178` object is not yet embedded in a native `0x2569d0` owner; `apply_owner_properties`
  still takes a caller-supplied bag.
