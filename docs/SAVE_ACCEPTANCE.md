# SAV-04 — real-save acceptance procedure

Status: **NOT RUN**. No user-owned PS4 save exists in this workspace, so no retail `USR-DATA` has
been read. The transaction and metadata code is `FIXTURE_VERIFIED` only (E601, E604, E606); G2/G5
remain **NOT RUN**. This document defines the acceptance step and a read-only inspector; it does
not claim that the native loader consumes retail saves.

## Why this step exists

Every save byte currently exercised by the native tests was produced by the native encoder. The
container, FNV, chunk walk, metadata block and delete order are byte-verified against the recovered
code, but a real platform-written save is the only evidence that the contract matches retail data.
Until one is inspected, "save/load works" must not be claimed.

## What to run

Point the inspector at one slot directory (the one containing `USR-DATA`), or at the file itself.
It never writes to the save directory and never copies game data into the repository.

```sh
python3 -I tools/dev/save_inspect.py "/path/to/SAVEDATA01"
python3 -I tools/dev/save_inspect.py "/path/to/SAVEDATA01/USR-DATA" --json
```

The tool checks:

- the 16-byte outer header: version `1`, descriptor `4`, body size, FNV-1a-32 of the body;
- the `20005 + 4N` body form and the `0x6400000` ceiling;
- strict tagged-chunk tiling (`{u32 name_hash, u16 0xBEEF, u16 tag, u32 payload}`, no overrun);
- at least one GameData-sized chunk (`8 + 0x3cd8` payload);
- optional native `USR-META` (`SOTCMETA`/1/descriptor/`data_hash`/`0x530`, NUL-terminated texts,
  zero `0x2c` reserved tail) and an optional `icon0.png` signature under the native 1 MiB cap.

Exit code is `0` only when every check passes. A retail save is expected to **fail** the
`USR-META` checks, because that file is a native wrapper the guest never writes; inspect with
`--json` and read the individual lines.

## Safe reporting

The `USR-META` title/subtitle/detail may contain a player name or other personal text. Do not paste
them or commit the save. Report only the `PASS`/`FAIL` lines, the body size and the chunk
tag/count summary. The game data blob itself is never decoded by this tool.

## Self-test

The parser mechanics are exercised without game data:

```sh
python3 -I tools/dev/save_inspect.py --self-test
```

This builds a synthetic two-chunk container in a temporary directory and checks the header, FNV,
tiling, GameData-sized chunk and exit path. It is **FIXTURE_VERIFIED**, not acceptance.

## Native persistence (not acceptance)

The native transaction is cross-process persistent in the synthetic format: `test_save_relaunch`
writes a slot in one process and loads/enumerates it in two fresh processes (E615). That is still
produced by the native encoder, so it does not replace the retail-save check above.

## What a pass would and would not mean

A pass means the file matches the recovered container contract (header, hash, tiling, descriptor).
It does **not** by itself prove that the game's field readers accept the values, that the native
encoder reproduces retail bytes, or that the host save path is wired to the launcher. Those remain
separate steps. If a real save is supplied, record its SHA-256 privately, the tool output, and the
exact conclusion in `docs/EVIDENCE.md` before promoting any save/load claim.
