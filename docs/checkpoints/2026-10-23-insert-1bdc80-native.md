# Checkpoint — list-B inserter `0x1bdc80` lifted; container destructor list-B type corrected

- Local: 2026-10-23 09:0x (America/Chicago)
- Agent: integrator
- Task: `CPU-01-INSERT-1BDC80-NATIVE-01`
- Gate: **FIXTURE_VERIFIED — no runtime claim**
- Evidence: E571

## Shape

`0x1bdc80..0x1bde20` (ret), **417 B**, sha256 `414863a3…250a`; canary tail at `0x1bde21`. Signature:
`rdi = container base`, `rsi = callable A`, `rdx = callable B`.

```
1bdcd0..1bdd60  copy/acquire A and B twice (four acquires)
1bdd90..1bdd9f  node = 0x17090(0x50)
1bddb2/bb       node+0x10 = A2, node+0x30 = B2
1bddc0..da      link at container+0x20; incq [container+0x30]
1bddb7..1bde02  zero A2/B2, release them (no-ops), release B/A (hand-off)
```

Node: prev `+0x00`, next `+0x08`, callable A `+0x10`, callable B `+0x30`; **no flag**, size `0x50`.
List B sentinel is `container+0x20`; count is 64-bit at `container+0x30`.

## Native work

- `native/core/insert_1bdc80.{hpp,cpp}` with `Node1bdc80`, `List1bdc80`, allocation/release hooks,
  reference hand-off and allocation-failure restore.
- Fixture `insert_1bdc80`: 0x50 allocation with tag 1/align 8/flags 1, list links, count, node
  payload, second insert head update, failure restore.
- `container_dtor_1bede0` now walks list B as `List1bdc80`/`Node1bdc80` instead of the 0x58 flag-node
  type; list A keeps `Node1bdeb0`. The fixture builds list B with the real `insert_1bdc80`.
- Checker `check_finsert_1bdc80.py` pins the extent/hash, 0x50 allocation, `+0x20` sentinel,
  `+0x30` count, node stores and the four `0xf590` hand-offs.

## Verification

`insert_1bdc80` and `container_dtor_1bede0` pass under ASan+UBSan; the checker exits 0 on the pinned
ELF. Round-25 full-suite and sweep counts are in the round-25 verification checkpoint.

## Limits

The list-B reader/clear (`0x1be2f7..0x1be49d`), `0x17090`/`0xf590` bodies and the canary path are
not modelled. **FIXTURE_VERIFIED.**
