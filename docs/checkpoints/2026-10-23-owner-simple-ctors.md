# Checkpoint — three trivial `0x2569d0` constructors

- Local: 2026-10-23 (America/Chicago)
- Agent: integrator
- Task: `CPU-01-OWNER-SIMPLE-CTORS-01`
- Gate: **FIXTURE_VERIFIED**

## Bodies

| ctor | size | recovered effect | native |
|---|---:|---|---|
| `0xa1cc0` | 1 B | `ret` | `init_owner_a1cc0` no-op |
| `0xfafb0` | 92 B | vptr; zero +0x08..+0x2007; `+0x2008 = obj+0x2028`; `+0x2010 = obj+0x82028`; `+0x2018 = obj+0x82028`; zero +0x2028..+0x82027 | `init_owner_fafb0` |
| `0x26f820` | 57 B | base ctor `0x3eba0`; vptr; `+0x18 = 0`; sub-object ctor `0x3b310` at +0x4020 | `init_owner_26f820` |

The guest vptrs (`0x1597ca8`, `0x159c9d8`) are host values in the native model and are never called.
`0xfafb0` zeroes +0x2020..+0x2027, which the original leaves uninitialised after a fresh
allocation; the native choice is deterministic and documented.

## Fixture

- `0xfafb0`: vptr, the three self-pointers, both zero ranges, and the padding.
- `0x26f820`: base-before-sub order, vptr, `+0x18`, sub-object at +0x4020, base failure stops before
  the vptr store, and both missing-hook failures.
- `0xa1cc0`: leaves the byte untouched.

## Checker

`check_fowner_simple_ctors.py` pins all three body hashes, the `0x2000`/`0x80000` fills, the three
pointer stores, and the `0x26f820` base/tail-call sequence.

## Limits

The other 15 non-trivial ctors remain hooks in `owner_members`.
