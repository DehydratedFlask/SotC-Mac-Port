# Checkpoint — ResourcePool / ResourceWrapper / ResourceRef

- Local: 2026-10-23 (America/Chicago)
- Agent: static-RE worker; integrator
- Task: `CPU-01-RESOURCE-POOL-01`
- Gate: **STATIC_ANALYSIS_VERIFIED**; native implementation in flight

## Naming correction

The 0x100 B pool-visible object is **`ResourceWrapper`**, not `Resource`. Its own debug literals name
`ResourceWrapper::RemoveReference` and `ResourcePool::MarkResourceWrapperDead`. `Resource` is the
payload owned at `wrapper+0x20`, with a different lifetime.

## Wrapper

No vtable (`+0x00` is the intrusive list link). Fields: `+0x00` next, `+0x08/+0x10` self links,
`+0x30` external refcount (atomic u32), `+0x40` dependency count, `+0x68` pool back-pointer,
`+0x70` `ResourceId` key copy, `+0x20` payload, `+0x98` state 11, `+0xa8` `vector<ResourceRef>`
children.

| function | span | behavior |
|---|---|---|
| `0x14ccb0` AddReference | 5 B | `lock inc dword [rdi+0x30]` |
| `0x14ccc0` RemoveReference | 563 B | decrement; at previous==1 erase the map node, then manager-delegate or inline destroy |
| `0x14d2d0` ReleaseDependency | 289 B | decrement `+0x40`; at zero recurse children; never frees |
| `0x14cf20` RequestState | 748 B | `+0x40 += 1` unconditionally; return early when `state <= priority` |

## Map

`pool+0x10` is a `std::map<ResourceId, ResourceWrapper*, cmp>`: node 0x50 B, comparator `0x14c850`
(unsigned u32 hash then `strcmp`), insert `0x150360` copies the key and stores a raw value pointer.
The map is a **non-owning weak index**; in-order iteration is observable, so `std::map` is faithful
and a hash map is not.

## Attach path

`GetResource`/`TryGetResource` return `ResourceRef{ptr, owned=false}`. A hit adds one `+0x30`; a miss
allocates a wrapper, copies the key (+1), inserts the map node and returns it. The nine sites use
only pointer identity, `+0x30` and `+0x40`. A fresh wrapper has state 11, so sites 1-8 (`priority
13`) take the early return; site 9 (`priority 9`) writes state 9.

## Limits

The `ResourceManager` delegation, the payload factory, the null wrapper singleton and the full
`RequestState` body remain hooks/contracts.
