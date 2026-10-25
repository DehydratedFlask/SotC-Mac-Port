# Checkpoint — native ResourceWrapper / ResourcePool / ResourceRef

- Local: 2026-10-23 (America/Chicago)
- Agent: code worker; integrator verification
- Task: `CPU-01-RESOURCE-POOL-NATIVE-01`
- Gate: **FIXTURE_VERIFIED**; not wired into the launcher

## Model

- `ResourceWrapper` is exactly 0x100 B and non-polymorphic: `+0x00` list link, `+0x30` external
  references (atomic), `+0x40` dependencies (atomic), `+0x50` guard, `+0x60` flags, `+0x68` pool,
  `+0x70` key, `+0x98` state 11, `+0xa8` children, `+0xc8` owner id.
- `ResourceIdLess` compares unsigned u32 hash, then `strcmp` over the key data.
- `ResourcePool` is a non-owning `std::map<HashedStr, ResourceWrapper*>`; `get_resource` returns a
  hit with one added reference, or allocates a wrapper, copies the key and inserts on a miss;
  `try_get_resource` returns the null wrapper on a miss; `erase` runs only at `+0x30 == 0`.
- `ResourceRef{ptr, owned}`; `owned` means one `+0x40` dependency reference.
- `request_state` always increments `+0x40`; a fresh wrapper (state 11) early-returns for priority
  13, writes state 9 for priority 9, writes state 8 for priority 12, and does nothing for 10/11/>13.
- Key holders release through the existing `0xf590` lift; the pool's map uses the host allocator.

## Hooks

Allocation/deallocation, payload factory/destroy, `ResourceManager` delete delegation and the engine
state notification. A missing or failing hook is a visible false, never a fake success. `get_resource`
never creates a payload (the attach path requires `payload == nullptr`).

## Fixture and checker

- 317 checks: hit/miss, key-control increments, erase at zero, manager delegation, inline destroy,
  dependency recursion, state table, lock/unlock refs, comparator equivalence and ordering, failure
  surfaces. 13 wrappers/11 key blocks/12 manager deletes/2 payloads, ASan+UBSan clean.
- Mutations: double `add_reference` on a miss and skipped erase both fail.
- `check_fresource_pool.py` pins 10 core + 4 supporting bodies and the structural offsets.

## Limits

- The map is non-owning; wrappers must not outlive the pool.
- The pool's recursive mutex and the second dependency tree are not modelled.
- A failed inline destroy leaves the wrapper alive with the map node already erased; retry is unsafe.
- `manager_delete` owns both manager calls and must release key holders itself.
- Single-threaded model; not wired into the launcher. G1 PARTIAL.
