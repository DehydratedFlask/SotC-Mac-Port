# 2026-11-08 — batch 13 dummy and owned-dword byte masks, store trace

Task: CPU-01-AOT-CFG-SCALE-01-BATCH49. Evidence is the pinned `local/re/eboot.elf` disassembly
(`xcrun llvm-objdump -d --no-show-raw-insn`), not a constructor-shape guess.

## Dummy `option_unavailable` at 0x2ebf0d0 (0xd0 bytes)

- `0x20af0`: vptr `%rdi+0x00` 8 B; registered byte `+0x18` 1 B; flags dword `+0x1c` 4 B; name
  `+0x20..0x5f` 64 B = 77 B.
- `0x20d60` -> `0x20d80`: pending-list writes at `0x20ed4` (`+0x08`) and `0x20ed0` (`+0x10`),
  covering `+0x08..0x17` = 16 B. The `+0x18` byte is already counted.
- Site `0x8849d`: `0x8a584` vptr `+0x00` (overlap); `0x8a587` `vmovups` `+0x60..0x7f`;
  `0x8a5ac` `vmovups` `+0x80..0x8f`; `0x8a5b4` qword `+0x90..0x97` = 56 new bytes.
- Union: **149 written**. Holes: `+0x19..0x1b` (3) and `+0x98..0xcf` (56) = **59 holes**.
  Bytes `+0xa0..0x9f` do not exist; the command terminator is `+0x98..0x9f` and the 0x30 trailing
  stride padding is `+0xa0..0xcf`.

## Owned c580 rows at 0x2ebe3e8 / 0x2ebe4c0 / 0x2ebe598 (0xd8 bytes each)

- `0x20af0` + `0x20d80` give the same 93-byte base/link union.
- `0xc580` stores: `+0x00` 8; `+0x60..0x67` 8; `+0x6c..0x6f` 4; `+0x70` 1; `+0x74..0x77` 4;
  `+0x78` 1; `+0x80..0x9f` 32; `+0xb0..0xb7` 8; `+0xc0..0xc7` 8; `+0xc8..0xcf` 8. The functor
  copy at `0xc720`/`0xc740` writes `+0x10`/`+0x20` relative to `obj+0xa0`, i.e. `+0xb0`/`+0xc0`,
  so `+0xa0..0xaf` remains untouched.
- Caller after the call: `+0x68..0x6b` dword (default value) and `+0xd0..0xd3` dword (owned value);
  the latter is at `0x894f7`, `0x89619`, `0x89725`.
- Union: **175 written**. Holes: `+0x19..0x1b` (3), `+0x71..0x73` (3), `+0x79..0x7f` (7),
  `+0xa0..0xaf` (16), `+0xb8..0xbf` (8), `+0xd4..0xd7` (4) = **41 holes**.

The native constants and fixture assertions agree: dummy 149/59, owned 175/41, total 5,495/1,073.
