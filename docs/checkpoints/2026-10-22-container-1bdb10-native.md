# Checkpoint — native lift of container ctor `0x1bdb10`

- Local: 2026-10-23 01:50 CDT (America/Chicago)
- Agent: orchestrator (integrator)
- Task: `CPU-01-CONTAINER-1BDB10-NATIVE-01`
- Gate: **G1 PARTIAL — fixture-level; nothing executed against the game**
- Evidence: E534

## What was built

`native/core/container_1bdb10.{hpp,cpp}` + `native/tests/test_container_1bdb10.cpp` (330th test)
+ `tools/lift/check_f1bdb10.py`. The 0x60 B three-list container that `insert_1beeb0` (E531)
inserts into can now be created natively.

## OBS (integrator replay)

Body `0x1bdb10..0x1bdb48` = **56 B**, fn sha256 `056d7514…580a`; `ret` at `0x1bdb47`.
Eleven stores, byte-pinned at their exact offsets: `movq $0,[rdi+0x10]`, `[rdi]=rdi`,
`[rdi+0x08]=rdi`, `movq $0,[rdi+0x30]`, `[rdi+0x20]=rax` (`rax=rdi+0x20`),
`[rdi+0x28]=rax`, `movq $0,[rdi+0x50]`, `[rdi+0x40]=rcx` (`rcx=rdi+0x40`), `[rdi+0x48]=rcx`.
No store targets `+0x18`, `+0x38` or `+0x58` (the checker proves the absence of those store
encodings).

## NOT modelled

Nothing from the body itself — it is fully covered. The owner object that embeds this container is
still constructed elsewhere (`0x533100`/`0x2569d0`), so this does not by itself create a live
game owner.

## Verification

```
python3 -I tools/lift/check_f1bdb10.py            -> OK (56 B sha, all 11 stores, pad absence)
ctest --test-dir local/build/orch-r7              -> 100% tests passed out of 330
```

The fixture fills the object with 0xAA first, then proves the three self-referential sentinels,
zeroed counts, untouched `+0x18/+0x38/+0x58`, and that a container built by this ctor accepts the
lifted insert (count 1, list head = node, `prev` = sentinel, self-link true).

## Next exact action

Build the owner wrapper around this container (the `0x533100`/`0x2569d0` allocation path), then
give `init_1290` a natively constructed owner instead of a caller-supplied container.
