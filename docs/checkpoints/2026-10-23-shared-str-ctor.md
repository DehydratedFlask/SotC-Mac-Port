# Checkpoint — `0xf1a0` SharedStr constructor and the resolved vtable

- Local: 2026-10-23 (America/Chicago)
- Agent: static-RE worker; integrator
- Task: `CPU-01-SHARED-STR-CTOR-01`
- Gate: **FIXTURE_VERIFIED** for the constructor; the nine `0x533100` sites remain unwired

## Holder

`SharedStr` is 0x20 B: `data +0x00`, `size +0x08`, `capacity +0x10`, `control +0x18`. There is no
SSO: `size == 0` uses the shared zero byte at `0x2dd9f78` (`empty_string_f730()`), otherwise every
string is heap-allocated. The existing `SharedStr`/`HashedStr` declarations already match; the field
called `aux` is proven to be capacity by the `0xf3c0` resize comparison.

## Constructor `0xf1a0` (151 B, sha `954762ab…`)

```
size     = strlen(src)
alloc    = (size + 4) & ~3
capacity = alloc - 1
data     = 0x12820(alloc, tag 0, align 8, flags 0)
memcpy(data, src, size); data[size] = 0
control  = 0x17070(8) = 0x12820(8, tag 1, align 8, flags 1)
*control = 1 (xchg)
```

`0xf3c0` (459 B, sha `980c6911…`) is the resize/COW contract: COW when `*control != 1`, allocate
data only when capacity is short, and keep the control block otherwise.

## Vtable resolution

`[[0x2ee2cf8]]+0x18` is **`0x14e260`, `ResourcePool::GetResource`**, vtable `0x1598628`, not a
destructor. It returns a hit without touching the key, or inserts a pool copy and bumps the key
control by one on a miss. Therefore the release at the nine sites is a real `0xf590` with
`previous` 1 (hit) or 2 (miss); it must never be wired as a no-op. The existing
`shared_release_f590` is reused unchanged.

## Native work

- `shared_str_ctor_f1a0` with `AllocDispatchContract` hooks: data tag 0/align 8/flags 0, control
  tag 1/align 8/flags 1, atomic `*control = 1`, shared empty string, data cleanup on control
  failure.
- Fixture: exact size/capacity/tags, empty sharing, data/control allocation failures, missing
  allocator, and a real `shared_release_f590` that destroys the data and frees the control block.
- Checker: both body hashes, align4/capacity sequence, both allocation targets, the xchg, and the
  empty-string path.

## Remaining

The nine `vslot_40_533100_*_attach` fragments still use 8-byte `dtor_f590` stand-ins. They now have
the missing type and constructor; the next step is to replace each stand-in with a real `HashedStr`
key, run `0x14c2f0`'s `****` rewrite + FNV hash (or a documented hook), and release `key.str` with
`had_control == true` and `previous ∈ {1,2}`.
