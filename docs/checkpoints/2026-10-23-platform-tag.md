# Checkpoint — ResourceSystem platform-tag lookup

- Local: 2026-10-23 (America/Chicago)
- Agent: static-RE worker; integrator
- Task: `CPU-01-PLATFORM-TAG-147E20-01`
- Gate: **FIXTURE_VERIFIED**

## Lookup

`0x147e20` (`0x147e20..0x147ef0`, 208 B, sha `400cd009…`) takes `(ResourceSystem*, u32 fourcc)` and
returns a `const char*` tag. It performs two `std::map<u32,Entry>` `lower_bound` descents: the
exact fourcc, then the default key `"****"`; a miss falls back to the rodata `"_ps4"` at
`0xfa95d8`. No call, no store, no allocation.

`0x147e10` (`89 77 40 c3`) sets the default key; its only caller is `0x533430` with
`esi = 0x2a2a2a2a`. The entry ctor `0x144640` (268 B, sha `6d68e88f…`) writes `"_cmn"` at
`value+0x58`; five registrations overwrite it with `"_ps4"`.

## Table

25 rows: 22 `_cmn`, 3 `_ps4` (`cfon`, `cptx`, `cctx`), and the `"****"` wildcard `_cmn`.
`crmp` and `calt` are `_cmn`. An unpopulated registry returns `_ps4`; the nine `0x533100` sites
run after the 23 registrations, so their honest tag is `_cmn`, **not** `_ps4`.

## Native

- `platform_tag_147e20(fourcc, state)` and the one-argument
  `platform_tag_147e20_default(fourcc)`.
- `fourcc_of("crmp") == 0x63726d70`, matching the `movbe` value from the literal.
- The nine attach fragments now default their `platform_tag` hook to the native table; the
  integration fixture uses the real mapping and asserts the nine derived FNV-1a-32 hashes:
  `0x2128cdb9, 0xecf0a769, 0x2b44f9db, 0x3b87dd56, 0xf2163cc4, 0x94a8da3f, 0xbfa276e7,
  0x99117412, 0x13a0d114`.

## Checker

`check_fplatform_tag_147e20.py` pins the lookup/entry body hashes, `0x147e10`, the `0x1497f0`
0xe0 allocation with key/value offsets, and the `_cmn`/`_ps4` literals.

## Limits

The 0xe0 map nodes and the `0x144640`/`0x1497f0` bodies are not lifted; the RB order is not
observable for this lookup. `'cwks'` registers into a different object and is out of scope.
