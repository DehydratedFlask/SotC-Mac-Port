# Checkpoint — owner row-9 destructor `0xa5cb30`

- Local: 2026-10-23 (America/Chicago)
- Agent: code worker; integrator verification
- Task: `CPU-01-OWNER-DTOR-A5CB30-01`
- Gate: **FIXTURE_VERIFIED**

## Body

`0xa5cb30..0xa5cd81`, 594 B (`0x252`), 142 instructions, sha256 `0b5c4501…`, 14×`0x90` padding,
next body `0xa5cd90`. No stack canary. 22 calls: 19 direct + 3 indirect (`[vptr+8]` once,
`[vptr]` twice).

## Order

Event flag `+0xa510` -> event flag `+0xa500` -> `[+0xa4f8]` release -> `+0x8bd0` elements/free ->
`+0x72a8` elements/free -> tree `+0x7270` -> shared `+0x7250` -> `+0x6fe8` holders/free ->
`+0x6ac0` holders/free -> `+0x68a0` buffer -> drain `+0x5c60` -> `+0x28` element -> conditional
`0x14d2d0` -> unconditional `0x14ccc0` -> free `+0x28` -> head releases -> caller frees the object
(`0x256cf0`).

## Native implementation

`native/core/owner_dtor_a5cb30.*` reuses the ctor layout table and takes hooks for the event-flag
delete (`0x3b440` -> `sceKernelDeleteEventFlag`), the polymorphic releases, the tree destroy
(`0xb09690`), the shared releases (`0xf590`) and the tagged frees (`0x170b0`/`0x170c0`). A missing
hook is a named `failed_at`; the prefix stays as written. Double destroy repeats the event/tree
steps but not the cleared `+0xa4f8` release.

## Verification

- 8 fixture sections: constructed object (exact 24-step order and byte-identical object), armed
  object (2/1/1/2 elements, 6 shared releases, 6 frees, 3 last-reference destroys), precondition
  guards, every missing hook, failing hooks, ctor prefixes (`event_flag_0/1`, `name_record`,
  `tree_init`), double destroy, guard ring.
- Mutation: swapping the two event-flag releases fails step 0; restored.
- Checker pins the body, the 19 direct calls in order, the 3 indirect encodings, the stores and the
  event order.
- ctest 369/369, sweep 466/0/0 (with the concurrent save-metadata slice).

## Limits

- `0xb09690` does not null `[at+8]`; double-destroy is not idempotent and is documented, not fixed.
- The armed drain (`0xa62340`) and the element loops stay hooks.
- The dtor frees nothing itself; the caller `0x256cf0` performs the final `0x170b0`.
