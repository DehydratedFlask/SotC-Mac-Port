# Init `0x531d60` trivial remainder leaves → measured startup (G1 PARTIAL)

- Date/time: 2026-10-22 ~10:25 CDT (America/Chicago).
- Agent: bc-bf254dd3-8d34-514b-a322-1684213b3fa9 (Drive).
- Task: `CPU-01-INIT-531D60-TRIVIAL-LEAVES-01`; gate: **G1**; status: **PARTIAL** (not PASSED).
- Failed hypotheses/tries: 0.
- Tip: `b19b879` on `cursor/init-hash-551140-9dfc` (PR #4). Private manifest sha256 `03b9adca…`.

## Claim boundary

**Claimed:** native ARM64 models of Init trivial callees from appmain triage
(flag pair `0x4c4950`, setters `0xa52b70`/`0xa599e0`/`0xa431e0`, one-byte `ret` stubs,
splash PLT `0xf90310` no-op) and a measured path that runs them then the E405 registry
fragment via tip `AllocatorFront`+FakeHeap.

**Not claimed:** G1 PASSED; base Initialize `0x255130`; `0xf08` singleton; pad
`0x1fc80`/`0x1e4c0`; pools/listeners; PR #5 heap merge.

## Private evidence

`local/validation/appmain-init-remainder-20261022/`

```sh
python3 -I local/validation/appmain-init-remainder-20261022/capture.py
```

## Observed

| Step | Result |
|---|---|
| ELF | `check_finit_531d60_trivial_leaves.py` OK |
| CTest | `appmain_init_remainder_slice` + `appmain_init_registry_slice` PASS |
| Measured | trivial flags/setters + registry entries=275; alloc_ops=3 |
| Pairs | still **155** |

## Next exact action

Base Initialize `0x255130` (or `0xf08` singleton install) for a fuller early Init path.
Goal remains open until G1 PASSED criteria.
