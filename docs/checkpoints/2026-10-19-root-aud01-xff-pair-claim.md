# AUD-01-XFF-01 paired XFF sound-bank structure claim

- Date/time: 2026-10-19 23:09 CDT (America/Chicago)
- Integrator: `/root`; implementation owner: `/root/audio_asset_survey`
- Task ID: AUD-01-XFF-01, characterize one `.hd`/`.bd` bank pair with a bounded structural reader
- Files changed in claim: `HANDOFF.md`, `docs/TASKS.md`, this checkpoint. No implementation code
  has changed for this slice.
- Prior failed tries: 0.

## Evidence and boundaries

The implementation agent's read-only scan verified the target pair through `Psarc.read()` against
the private asset manifest:

| Item | Archive | Entry | Offset | First block | Decoded bytes | SHA-256 |
|---|---|---:|---:|---:|---:|---|
| `cp3shadow/sound/se/com_sys.hd` | `pak2.psarc` | 1311 | 7,884,070 | 825 | 1,908 | `39ebb27c1d469853e12ffc439b17e8641d6258f9cc0f1029303c92bb9683ffe3` |
| `cp3shadow/sound/se/com_sys.bd` | `pak2.psarc` | 1310 | 6,989,994 | 664 | 3,672 | `137e64c118ada84b4dd9bd73bf9e4b7e9e1ef061fa2574dc304d5f7cdd1ebd81` |

The archived input is 16,557,548 bytes with SHA-256
`9c655de637baba5f2c35250a2a7024f405f2f95c19fb4739ff084fa95cbdf0b5`. Both decoded members begin
`78 66 66 00 00 00 00 00 00 00 00 00 01 00 00 00`; interpreting the first four bytes as `xff\0`
is direct ASCII observation. Other field meanings are open.

Graft-first navigation identified `Psarc.read` in `scripts/psarc.py`, including block truncation,
decompression and decoded-size checks. Command:
`graft ask "What existing PSARC APIs can safely read a named sound-bank member and what bounds checks are already established?" --source --in scripts/`.
The source graph is current; the optional deep meaning layer is not built.

## Bounded implementation and acceptance

Owned implementation paths:

- `scripts/audio_bank_header.py`
- `scripts/test_audio_bank_header.py`

First compare the pair and any equivalent pair found in the same existing manifest. Implement only
fields supported by repeated byte layout and bounded spans; preserve unknown field labels and
codec=`UNKNOWN`. Do not modify game files or write decoded content into public paths. The fixture
must cover a valid header, bad magic, truncation and any validated span/length errors.

Exact checks:

1. `python3 -I scripts/test_audio_bank_header.py`
2. `python3 -I scripts/audio_bank_header.py --archive local/extracted/CUSA08809/pak2.psarc --member cp3shadow/sound/se/com_sys.hd --member cp3shadow/sound/se/com_sys.bd`

The live command must report the manifest-matched archive/member identities and supported byte
structure only. A parser/fixture pass does not establish NGS2 semantics, codec, gameplay use, or
playback.

## Status

ACTIVE, implementation try count 0. Remaining uncertainties: whether the pair contains a validated
header/data index relation beyond its common prefix; exact format, codec, and use are unknown.
Next exact action: implementation owner cross-checks the pair bytes, adds the scoped parser and
fixture, and returns command outputs/log paths to the integrator. The integrator updates shared
documents and HANDOFF.
