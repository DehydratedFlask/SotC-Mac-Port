#!/usr/bin/env python3
"""AUD-01: where the ATRAC9 configuration word comes from, and the geometry derived from it.

The question this checker closes (HANDOFF "unrecovered ATRAC9 config word at [wave+0x5f8]"):

  * `0x987950(ctx, id, inline)` is NOT a table lookup. It is `id ? [ctx+0x20] + id : inline`; its
    inverse `0x987940(ctx, ptr)` is `ptr ? ptr - [ctx+0x20] : 0`.
  * `[wave+0x5f8]` is written by exactly one *data-driven* site, `0x980f39`: the handler for **field
    id 0x0A** of the field list that starts at `record+0x28` of a `WAVE` record (21-entry
    self-relative dispatch table at `0x12b5274`). It stores `0x987940(ctx, field+4)`, so the config
    word is the u32 at `field+4` inside the loaded `.sxd1` image, and `[wave+0x600]` = min(count,16).
  * The other writers of `0x5f8` in the wave-object code are three zero-stores (the per-wave reset
    `0x980772` and the two alternative-source branches `0x980a34`, `0x980b12`). Those two branches
    take the configuration inline from the *caller's* spec (`spec+0x38/+0x3c` with count 2, or
    `spec+0x40` with count 1) instead of from a `WAVE` record: `spec[+0x28] == 1` / `== 2`.
  * The ATRAC9 arm `0x98413e..0x9842cf` of the format dispatcher `0x983ea0` validates each config
    word (`top byte == 0xFE`), measures it with three `bextr` reads and two tables, and tail-jumps to
    the shared finaliser `0x9840f8`. It calls no decoder.
  * Bit layout read by the arm (general-knowledge labels in brackets are INFERENCE, the widths and
    tables are OBSERVED): `[31:24]` tag 0xFE, `[23:20]` index into `0x12b5700` (frame samples),
    `[19:17]` index into `0x12b5720` (channels contributed by this word) [ATRAC9 channel-config
    index], `[15:5]+1` bytes (this word's frame bytes) [ATRAC9 frame size], `[4:3]` -> multiplier
    `1<<n` [ATRAC9 superframe index: frames per superframe].
  * Asset half: every one of the 566 format-0x42 `WAVE` records in the five pinned `.sxd1` files
    carries field 0x0A (cnt 1, size 6); the six distinct words reproduce the record's own channel
    count and sample rate, and the derived superframe size divides every segment length of its group
    and equals the group gcd.

Evidence labels used in output and in docs/AUDIO_FORMAT.md: STATIC_ANALYSIS_VERIFIED (pinned ELF
bytes), ARTIFACT_VERIFIED (pinned `.sxd1` bytes), INFERENCE (labelled). No audio is decoded, no
`.sxd2` byte is read (only the manifest's pinned sizes), nothing is written, and the only asset-
derived values printed are 32-bit configuration words and counts.

Private inputs: `local/re/eboot.elf` and (for the asset half) the pinned `.sxd1` members named in
`local/manifests/assets-md5pairing-r30/audio-headers.json`. When an input is ABSENT the matching half
prints `SKIP` and the run exits 0 (like the other private-input checkers; `--require-private` makes
the skip fatal). A PRESENT input that is wrong is always a failure, never a skip.

Usage:
    python3 -I tools/lift/check_atrac9_config_provenance.py             # checks + mutation probes
    python3 -I tools/lift/check_atrac9_config_provenance.py --no-probes
    python3 -I tools/lift/check_atrac9_config_provenance.py --report    # also print the geometry table
    python3 -I tools/lift/check_atrac9_config_provenance.py --require-private
    python3 -I tools/lift/check_atrac9_config_provenance.py --elf PATH  # e.g. a mutated copy

Exit codes: 0 all checks (and probes) passed or skipped, 1 a check or probe failed, 2 usage/IO or
`--require-private` with an input missing.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import shutil
import struct
import subprocess
import sys
from collections import Counter
from math import gcd
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_DEFAULT = ROOT / "local/re/eboot.elf"
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
MANIFEST = ROOT / "local/manifests/assets-md5pairing-r30/audio-headers.json"
ARCHIVE_REL = "local/extracted/CUSA08809/pakN.psarc"

# (p_type, p_flags, p_offset, p_vaddr, p_filesz) of the three image-mapping program headers.
PHDR_PINS = [
    (1, 5, 0x4000, 0x0, 0x1593978),
    (0x61000010, 4, 0x1598000, 0x1594000, 0x6D190),
    (1, 6, 0x1608000, 0x1604000, 0x17D3100),
]

SXD1 = [
    "sound/shadow/music.sxd1",
    "sound/shadow/str_amb.sxd1",
    "sound/shadow/str_bgm.sxd1",
    "sound/shadow/str_boss.sxd1",
    "sound/shadow/str_event.sxd1",
]
WAVE_COUNTS = {SXD1[0]: 33, SXD1[1]: 414, SXD1[2]: 133, SXD1[3]: 194, SXD1[4]: 56}
# Observed census, ARTIFACT_VERIFIED: {(format byte, channels): records} over all 830 records.
FORMAT_CENSUS = {
    (0x20, 1): 1, (0x21, 1): 230, (0x21, 2): 33,
    (0x42, 1): 17, (0x42, 2): 131, (0x42, 4): 3, (0x42, 6): 415,
}
# Observed census of configuration words over the 566 ATRAC9 records.
WORD_CENSUS = {
    0xFE761DF0: 266, 0xFE761DE0: 149, 0xFE740FF0: 123,
    0xFE740FE0: 8, 0xFE7A17F0: 3, 0xFE7007F0: 17,
}
# Per-config-word geometry expected from the arm's arithmetic (OBSERVED code, hand-checked):
# word -> (channels, samples/frame, frame bytes, frames per superframe)
GEOMETRY = {
    0xFE7007F0: (1, 256, 64, 4),
    0xFE740FE0: (2, 256, 128, 1),
    0xFE740FF0: (2, 256, 128, 4),
    0xFE7A17F0: (4, 256, 192, 4),
    0xFE761DE0: (6, 256, 240, 1),
    0xFE761DF0: (6, 256, 240, 4),
}
MUSIC_SXD2_BYTES = 172_367_136

# Tables read by the arm (OBSERVED bytes at the pinned addresses).
SAMPLES_BY_INDEX = [0, 64, 0, 0, 128, 0, 0, 256]      # u16 at 0x12b5700, indexed by bits[23:20]
CHANNELS_BY_INDEX = [1, 2, 2, 6, 8, 4, 0, 0]          # u8  at 0x12b5720, indexed by bits[19:17]

# Pinned code, STATIC_ANALYSIS_VERIFIED. (check id, address, hex bytes, description)
CODE_PINS = [
    ("E01a", 0x987940, "31c04885f67407482b77204889f0c3",
     "0x987940 = ptr ? ptr - [ctx+0x20] : 0  (xor/test/je/sub 0x20(%rdi)/mov/ret)"),
    ("E01b", 0x987950, "4885f67407480377204889f24889d0c3",
     "0x987950 = id ? [ctx+0x20] + id : inline  (test/je/add 0x20(%rdi)/mov rdx->rax)"),
    ("E02a", 0x980816, "458b45284183f8020f847d0200004183f8010f84ae0100004585c00f",
     "0x980620 dispatches on spec[+0x28]: ==2 -> 0x980aa1, ==1 -> 0x9809dc, ==0 -> WAVE lookup"),
    ("E02b", 0x98084e, "488bbba0000000e83671000049",
     "kind 0: mov 0xa0(%rbx),%rdi ; call 0x987990 (WAVE table lookup in the same ctx)"),
    ("E02c", 0x980896, "e8a57000004889838005000041",
     "kind 0: call 0x987940 ; mov %rax,0x580(%rbx) (record offset in the same arena)"),
    ("E02d", 0x9808a2, "418b470488837905000088a37a050000c1",
     "kind 0: format dword -> 0x579 (low byte = codec selector), ah -> 0x57a (channels)"),
    ("E03a", 0x980772,
     "48c783f805000000000000c6830006000000c5f81183e0050000c683010600000048c783f005000000000000"
     "c6830206000000c78320060000",
     "per-wave reset: 0x5f8=0, 0x600=0, 0x5e0..0x5ef=0, 0x601=0, 0x5f0=0, 0x602=0"),
    ("E04a", 0x9809aa, "41f607014c897db00f85280200",
     "field list is entered only when record[+0] & 1 (else straight to 0x98120a)"),
    ("E04b", 0x980be0,
     "c5fa100d5046930031c04d89fe4c8d2580469300488945a031c04983c6284889458831c04889459831c0"
     "c745ac0000000048894590eb19c5",
     "r14 = record; r12 = 0x12b5274 (field table); r14 += 0x28 (field list starts at record+0x28)"),
    ("E04c", 0x980c30, "410fb7060fb6c880f9140f87b005000049630c8c0fb6c44c01e1ffe10f",
     "field loop: u16 header, id = low byte (> 0x14 rejected), jmp [table + id*4] self-relative"),
    ("E04d", 0x9811f0, "490fbf46024885c04e8d7430024c0f44f04d85f60f8526faff",
     "field loop tail: r14 += 2 + s16 size; size == 0 ends the list"),
    ("E05a", 0x980f29,
     "488bbba0000000498d7604e8076a0000488983f8050000418a46013c107202b010c5fa100de6429300888300060000e9",
     "field 0x0A: [obj+0x5f8] = 0x987940([obj+0xa0], field+4); [obj+0x600] = min(count, 16)"),
    ("E07a", 0x980a1c, "4c89b368050000c783ac000000ffffffff4488a379050000",
     "spec kind 1: [obj+0x568] = [ctx+0x4e8] ; [obj+0xac] = -1 ; [obj+0x579] (format byte) = spec[+0x30]"),
    ("E07b", 0x980a34, "48c783f8050000000000004489abe0050000",
     "spec kind 1: [obj+0x5f8] = 0 ; [obj+0x5e0] = spec[+0x38] (inline word 0)"),
    ("E07c", 0x980a5b, "8983e4050000c6830006000002",
     "spec kind 1: [obj+0x5e4] = spec[+0x3c] (inline word 1) ; [obj+0x600] = 2"),
    ("E07d", 0x980b12, "48c787f80500000000000089b7e005000048c1ee20c68700060000",
     "spec kind 2: [obj+0x5f8] = 0 ; [obj+0x5e0] = low dword of the u64 at spec[+0x40]"),
    ("E07e", 0x980b27, "c6870006000001", "spec kind 2: [obj+0x600] = 1 (one inline word)"),
    ("E10a", 0x9840F8, "0fb68b8b050000488b8390050000",
     "finaliser: ecx = [obj+0x58b] (multiplier); rax = u64 at [obj+0x590]"),
    ("E10b", 0x984106, "89ca0fafd048c1e8200fafc1",
     "finaliser: edx = mult * low32 ; eax = mult * high32"),
    ("E10c", 0x984112, "66899392050000", "finaliser: [obj+0x592] = dx  (samples per superframe)"),
    ("E10d", 0x984119, "66898396050000", "finaliser: [obj+0x596] = ax  (bytes per superframe)"),
    ("E12a", 0x983977, "0fb78b96050000" "31d2" "48f7f1",
     "stream path: byte position / [obj+0x596] = superframe index"),
]

# Raw scan: every `f8 05 00 00` (disp32 0x5f8) in 0x940000..0x9a0000, minus coincidental immediates.
# Each is (position of disp32, kind) with kind decided from the opcode byte before the ModRM.
SCAN_LO = 0x940000
DISP_5F8 = bytes.fromhex("f8050000")
EXPECTED_DISP_HITS = [
    0x95AA1A, 0x95AD56, 0x95AE0A, 0x971131, 0x97114A,     # coincidental imm32 / imul imm (not accesses)
    0x980775, 0x980A37, 0x980B15, 0x980F3C,               # stores to [obj+0x5f8]
    0x981D5B, 0x981EE0, 0x982555, 0x983A3B, 0x984148,    # loads from [obj+0x5f8]
]
EXPECTED_STORES = {0x980772: "reset to 0", 0x980A34: "kind 1 zero", 0x980B12: "kind 2 zero",
                   0x980F39: "field 0x0A (the data-driven writer)"}
EXPECTED_LOADS = [0x981D58, 0x981EDD, 0x982552, 0x983A38, 0x984145]
COINCIDENCE = {0x95AA1A, 0x95AD56, 0x95AE0A, 0x971131, 0x97114A}

ARM_LO, ARM_HI = 0x98413E, 0x9842D0
ARM_SHA256 = "eef99805f1e46bbc7dc2a78ee40d309003e04c93a7715b88ee58a6b11d9712a3"
FINALISER_SHA256 = "acb315a5f47c7272a54b7b4da5126e6df5b94b01f27bf8815adb82a25ac1e5db"
SETUP_SHA256 = "aa2fe5ae1b94ae29a98f2562e299dac63a8170477d42ad7bb47798852c71338b"  # 0x980620..0x9819fe
SETUP_LO, SETUP_HI = 0x980620, 0x9819FE
FIELD_TABLE = 0x12B5274


class Result:
    __slots__ = ("cid", "ok", "text", "detail")

    def __init__(self, cid: str, ok: bool, text: str, detail: str = "") -> None:
        self.cid, self.ok, self.text, self.detail = cid, ok, text, detail


class Collector:
    def __init__(self) -> None:
        self.results: list[Result] = []

    def check(self, cid: str, ok: bool, text: str, detail: str = "") -> bool:
        self.results.append(Result(cid, bool(ok), text, detail))
        return bool(ok)

    def failed(self) -> set[str]:
        return {r.cid for r in self.results if not r.ok}


# ------------------------------------------------------------------------------------- ELF view

class Elf:
    def __init__(self, data: bytes) -> None:
        self.data = data
        self.phdrs: list[tuple[int, int, int, int, int]] = []
        if len(data) >= 0x40 and data[:4] == b"\x7fELF":
            e_phoff, = struct.unpack_from("<Q", data, 0x20)
            e_phnum, = struct.unpack_from("<H", data, 0x38)
            self.e_shnum, = struct.unpack_from("<H", data, 0x3C)
            self.e_phnum = e_phnum
            for i in range(e_phnum):
                p_type, p_flags, p_off, p_va, _pa, p_fsz, _msz, _al = struct.unpack_from(
                    "<IIQQQQQQ", data, e_phoff + 56 * i)
                self.phdrs.append((p_type, p_flags, p_off, p_va, p_fsz))
        else:
            self.e_shnum = self.e_phnum = -1

    def off(self, va: int) -> int:
        for p_type, _fl, p_off, p_va, p_fsz in self.phdrs:
            if p_type in (1, 0x61000010) and p_va <= va < p_va + p_fsz:
                return p_off + (va - p_va)
        raise ValueError(f"{va:#x} outside a file-backed image-mapping program header")

    def rd(self, va: int, n: int) -> bytes:
        o = self.off(va)
        b = self.data[o:o + n]
        if len(b) != n:
            raise ValueError(f"short read at {va:#x}")
        return b

    def u16(self, va: int) -> int:
        return struct.unpack("<H", self.rd(va, 2))[0]

    def s32(self, va: int) -> int:
        return struct.unpack("<i", self.rd(va, 4))[0]


def _safe(fn, default=None):
    try:
        return fn()
    except (ValueError, struct.error, IndexError):
        return default


# ------------------------------------------------------------------------------- the ELF half

def rel32_target(e: Elf, va: int, opcode: int) -> int | None:
    b = _safe(lambda: e.rd(va, 5))
    if b is None or b[0] != opcode:
        return None
    return va + 5 + struct.unpack_from("<i", b, 1)[0]


def elf_checks(e: Elf, c: Collector, *, identity: bool = True) -> None:
    if identity:
        sha = hashlib.sha256(e.data).hexdigest()
        c.check("E00a", sha == ELF_SHA256, "ELF is the pinned image (SHA-256)", sha[:16])
    c.check("E00b", e.e_shnum == 0 and e.e_phnum == 11,
            "no section headers and eleven program headers",
            f"shnum={e.e_shnum} phnum={e.e_phnum}")
    c.check("E00c", all(p in e.phdrs for p in PHDR_PINS),
            "PT_LOAD 0, the R-only RELRO window (type 0x61000010) and PT_LOAD 2 are the pinned ones")

    for cid, addr, hx, text in CODE_PINS:
        want = bytes.fromhex(hx)
        got = _safe(lambda a=addr, n=len(want): e.rd(a, n), b"")
        c.check(cid, got == want, text, f"{addr:#x}: {got.hex()[:60]}")

    # The setup routine and the arm are pinned whole: any byte change anywhere in them fails.
    setup = _safe(lambda: e.rd(SETUP_LO, SETUP_HI - SETUP_LO), b"")
    c.check("E03b", hashlib.sha256(setup).hexdigest() == SETUP_SHA256,
            "0x980620..0x9819fe (wave setup incl. field loop and all 21 handlers) is byte-identical")

    fin = _safe(lambda: e.rd(0x9840F8, 0x30), b"")
    c.check("E10e", hashlib.sha256(fin).hexdigest() == FINALISER_SHA256,
            "shared finaliser 0x9840f8..0x984127 is byte-identical")

    # --- E04: the 21-entry self-relative field-id table
    tbl = []
    for i in range(21):
        s = _safe(lambda i=i: e.s32(FIELD_TABLE + 4 * i))
        tbl.append(None if s is None else FIELD_TABLE + s)
    c.check("E04e", all(t is not None and SETUP_LO <= t < SETUP_HI for t in tbl),
            "field-id table 0x12b5274: 21 self-relative targets inside the setup routine",
            str([hex(t) if t else None for t in tbl][:3]) + "...")
    c.check("E04f", len(tbl) == 21 and tbl[0x0A] == 0x980F29 and tbl[0x0C] == 0x980F78
            and tbl[0x0F] == 0x98106F and tbl[0x0B] == 0x980F5D,
            "field 0x0A -> 0x980f29, 0x0B -> 0x980f5d, 0x0C -> 0x980f78, 0x0F -> 0x98106f",
            f"{tbl[0x0A]:#x} {tbl[0x0C]:#x}" if all(tbl) else "")
    # 0x980bed loads the table: lea disp(%rip),%r12
    lea = _safe(lambda: e.rd(0x980BED, 7), b"")
    c.check("E04g", lea[:3] == bytes.fromhex("4c8d25")
            and 0x980BF4 + struct.unpack_from("<i", lea, 3)[0] == FIELD_TABLE,
            "0x980bed `lea 0x12b5274(%rip),%r12` loads the field table")
    # 0x987940 is called by exactly the two writers (kind-0 record offset, field 0x0A)
    c.check("E02e", rel32_target(e, 0x980896, 0xE8) == 0x987940
            and rel32_target(e, 0x980F34, 0xE8) == 0x987940,
            "0x980896 and 0x980f34 both `call 0x987940` (pointer -> arena offset)")

    # --- E06: census of every disp32 == 0x5f8 in the audio region
    region = _safe(lambda: e.rd(SCAN_LO, 0x9A0000 - SCAN_LO), b"")
    hits, i = [], 0
    while region and (i := region.find(DISP_5F8, i)) >= 0:
        hits.append(SCAN_LO + i)
        i += 1
    c.check("E06a", hits == EXPECTED_DISP_HITS,
            "the raw disp32 0x5f8 census of 0x940000..0x9a0000 is exactly the pinned 14 hits",
            f"{len(hits)} hits")
    stores, loads, other = [], [], []
    for pos in hits:
        if pos in COINCIDENCE:
            continue
        rex_op = _safe(lambda p=pos: e.rd(p - 3, 3), b"\0\0\0")
        rex, op, modrm = rex_op
        if (rex & 0xF0) == 0x40 and op in (0x89, 0xC7) and (modrm & 0xC0) == 0x80:
            stores.append(pos - 3)
        elif (rex & 0xF0) == 0x40 and op == 0x8B and (modrm & 0xC0) == 0x80:
            loads.append(pos - 3)
        else:
            other.append(pos)
    c.check("E06b", sorted(stores) == sorted(EXPECTED_STORES) and not other,
            "exactly four stores to [wave+0x5f8] exist in the audio region: reset, kind-1 zero, "
            "kind-2 zero, and the field-0x0A handler",
            f"stores={[hex(s) for s in sorted(stores)]} other={[hex(o) for o in other]}")
    c.check("E06c", sorted(loads) == EXPECTED_LOADS,
            "exactly five loads of [wave+0x5f8]; the arm's load is the one at 0x984145",
            f"loads={[hex(s) for s in sorted(loads)]}")
    c.check("E06d", 0x980F39 in stores and _safe(lambda: e.rd(0x980F39, 3)) == bytes.fromhex("488983"),
            "the only store whose value is NOT the constant 0 is `mov %rax,0x5f8(%rbx)` at 0x980f39")
    # the three zero stores really are `movq $0,0x5f8(...)` (opcode c7)
    zero_ok = all(_safe(lambda a=a: e.rd(a, 3)[1]) == 0xC7 and _safe(lambda a=a: e.rd(a + 7, 4)) == b"\0\0\0\0"
                  for a in (0x980772, 0x980A34, 0x980B12))
    c.check("E06e", zero_ok, "the reset, kind-1 and kind-2 stores are `movq $0, 0x5f8(reg)`")

    # --- E09: the ATRAC9 arm
    arm = _safe(lambda: e.rd(ARM_LO, ARM_HI - ARM_LO), b"")
    c.check("E09a", hashlib.sha256(arm).hexdigest() == ARM_SHA256,
            "ATRAC9 arm 0x98413e..0x9842cf is byte-identical")
    calls = {a: rel32_target(e, a, 0xE8) for a in (0x984153, 0x9841BB, 0x9842B1, 0x9842CA)}
    c.check("E09b", calls == {0x984153: 0x987950, 0x9841BB: 0x987950, 0x9842B1: 0x983CF0,
                               0x9842CA: 0xF8F3A0},
            "the arm's four calls: 0x987950 (config), 0x987950 (descriptor), 0x983cf0 (logger), "
            "0xf8f3a0 (stack-canary handler)",
            str({hex(k): hex(v) if v else None for k, v in calls.items()}))
    c.check("E09c", rel32_target(e, 0x984299, 0xE9) == 0x9840F8,
            "the arm's only jump out is the tail `jmp 0x9840f8` (shared finaliser)")
    # No other E8/E9 transfer *instruction* can be proven without a disassembler; this raw count is
    # an upper bound on false positives and is cross-checked by objdump (E14) when available.
    c.check("E09d", _safe(lambda: e.rd(0x984145, 7)) == bytes.fromhex("488bb3f8050000")
            and _safe(lambda: e.rd(0x98414C, 7)) == bytes.fromhex("488d93e0050000")
            and _safe(lambda: e.rd(0x98413E, 7)) == bytes.fromhex("488bbba0000000"),
            "the arm reads rdi=[obj+0xa0], rsi=[obj+0x5f8], rdx=&[obj+0x5e0] for 0x987950")
    c.check("E09e", _safe(lambda: e.rd(0x98415B, 5)) == bytes.fromhex("b914040000")
            and _safe(lambda: e.rd(0x984163, 5)) == bytes.fromhex("c4e270f7d0")
            and _safe(lambda: e.rd(0x984212, 6)) == bytes.fromhex("41bb11030000")
            and _safe(lambda: e.rd(0x98425D, 5)) == bytes.fromhex("ba050b0000"),
            "bextr controls: 0x414 (start 20 len 4), 0x311 (start 17 len 3), 0xb05 (start 5 len 11)")
    c.check("E09f", _safe(lambda: e.rd(0x984246, 16)) == bytes.fromhex("4489c281e2000000ff81fa000000fe75"),
            "every config word is tested `(w & 0xff000000) == 0xfe000000` (else log + error)")
    c.check("E09g", _safe(lambda: e.rd(0x98416A, 19)) == bytes.fromhex(
        "b801000000c0e90380e103d3e088838b050000"),
            "multiplier = 1 << ((word >> 3) & 3) stored at [obj+0x58b]")
    c.check("E09h", _safe(lambda: e.rd(0x984264, 29)) == bytes.fromhex(
        "c4c268f7d0ffc2440fafd2c4c220f7d0420fb614224401d66641897500"),
            "per word: bytes += ([15:5]+1) * desc.mult ; channels += table[[19:17]] ; [obj+0x594] = bytes")
    c.check("E09i", _safe(lambda: e.rd(0x9841E6, 25)) == bytes.fromhex(
        "8a837a050000b9010000004c8dab940500008845ccc645cd01"),
            "inline descriptor = {channels = [obj+0x57a], mult = 1}, count 1")
    msg = _safe(lambda: e.rd(0x12B5660, 36), b"")
    c.check("E09j", msg.startswith(b"Invalid ATRAC9 configure data (%08X)")
            and 0x9842A5 + _safe(lambda: e.s32(0x9842A1), 0) == 0x12B5660,
            "the failure arm logs 'Invalid ATRAC9 configure data (%08X)' with the offending word")

    # --- E11 tables
    samples = [_safe(lambda i=i: e.u16(0x12B5700 + 2 * i), -1) for i in range(8)]
    chans = list(_safe(lambda: e.rd(0x12B5720, 8), b""))
    c.check("E11a", samples == SAMPLES_BY_INDEX,
            "0x12b5700 (u16 x8, indexed by bits[23:20]) = [0,64,0,0,128,0,0,256]", str(samples))
    c.check("E11b", chans == CHANNELS_BY_INDEX,
            "0x12b5720 (u8 x8, indexed by bits[19:17]) = [1,2,2,6,8,4,0,0]", str(chans))

    # --- E13 no configuration word in the image
    known = [struct.pack("<I", w) for w in WORD_CENSUS]
    c.check("E13a", not any(k in e.data for k in known),
            "none of the six observed configuration words occurs as raw bytes anywhere in the image")
    relro = _safe(lambda: e.rd(0x1594000, 0x6D190), b"")
    c.check("E13b", bool(relro) and not any(relro[i + 3] == 0xFE for i in range(0, len(relro) - 3, 4)),
            "the RELRO window holds no dword with top byte 0xfe")
    c.check("E13c", struct.pack("<Q", 0x987940) not in e.data and struct.pack("<Q", 0x987950) not in e.data,
            "0x987940/0x987950 are never stored as pointers (they are called, not tabled)")


def objdump_crosscheck(e: Elf, c: Collector, elf_path: Path) -> str:
    """Second provider: LLVM objdump must see exactly these transfers in the arm. Optional."""
    tool = shutil.which("objdump")
    if not tool:
        return "SKIP E14 objdump not found"
    try:
        out = subprocess.run(
            [tool, "-d", "--triple=x86_64", "--no-show-raw-insn", f"--start-address={ARM_LO:#x}",
             f"--stop-address={ARM_HI:#x}", str(elf_path)],
            capture_output=True, text=True, timeout=60).stdout
    except (OSError, subprocess.SubprocessError):
        return "SKIP E14 objdump failed to run"
    xfers = []
    for line in out.splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[0].endswith(":") and parts[1].startswith(("call", "jmp")):
            tgt = next((p for p in parts[2:] if p.startswith("0x")), "")
            xfers.append((parts[0].rstrip(":"), parts[1], tgt))
    calls = [(a, t) for a, m, t in xfers if m.startswith("call")]
    direct_jmps = [(a, t) for a, m, t in xfers if m == "jmp"]
    ok = (calls == [("984153", "0x987950"), ("9841bb", "0x987950"), ("9842b1", "0x983cf0"),
                    ("9842ca", "0xf8f3a0")] and direct_jmps[-1:] == [("984299", "0x9840f8")])
    c.check("E14", ok, "objdump (independent disassembler) sees exactly those four calls and the "
            "tail jump in the arm and no indirect call", f"calls={calls}")
    return ""


# ---------------------------------------------------------------------------- arm model + assets

def arm_model(words: list[int], desc: list[tuple[int, int]]) -> dict | str:
    """Replays 0x984210..0x984292 + 0x9840f8 for one wave. Returns geometry or an error string."""
    if not words:
        return "no config word"
    w0 = words[0]
    si, pos = (w0 >> 20) & 0xF, 0
    if si >= len(SAMPLES_BY_INDEX) or SAMPLES_BY_INDEX[si] == 0:
        return f"rate/frame-size index {si} has no entry in the 0x12b5700 table"
    samples = SAMPLES_BY_INDEX[si]
    mult = 1 << ((w0 >> 3) & 3)
    total = 0
    got_channels = 0
    for chans, m in desc:
        if chans == 0:
            continue
        acc = 0
        while True:
            if pos >= len(words):
                return "descriptor needs more config words than the field carries"
            w = words[pos]
            pos += 1
            if (w & 0xFF000000) != 0xFE000000:
                return f"Invalid ATRAC9 configure data ({w:08X})"
            total += (((w >> 5) & 0x7FF) + 1) * m
            acc += CHANNELS_BY_INDEX[(w >> 17) & 7]
            if acc >= chans:
                break
        got_channels += acc
    return {"samples_per_frame": samples, "frame_bytes": total & 0xFFFF, "frames_per_super": mult,
            "super_samples": (mult * samples) & 0xFFFF, "super_bytes": (mult * total) & 0xFFFF,
            "channels_covered": got_channels, "words_used": pos}


def u32(d: bytes, o: int) -> int:
    return struct.unpack_from("<I", d, o)[0]


class Sxd1Error(ValueError):
    pass


def parse_sxd1(d: bytes) -> list[dict]:
    """WAVE records of one `.sxd1` (grammar: docs/AUDIO_FORMAT.md; field loop: 0x980c30)."""
    pos, wave = 0x60, None
    while pos + 8 <= len(d):
        tag, size = d[pos:pos + 4], u32(d, pos + 4)
        if tag == b"WAVE":
            wave = pos
        if size == 0 and tag != b"WAVE":
            raise Sxd1Error("zero-size chunk before end")
        pos += 8 + size
    if pos != len(d):
        raise Sxd1Error(f"chunk list ends at {pos:#x}, file is {len(d):#x}")
    if wave is None:
        raise Sxd1Error("no WAVE chunk")
    count, table = u32(d, wave + 0x0C), wave + 0x10
    recs = []
    for k in range(count):
        ent = table + 4 * k
        rec = ent + struct.unpack_from("<i", d, ent)[0]
        if not (0 < rec and rec + 0x30 <= len(d)):
            raise Sxd1Error(f"record {k} outside file")
        fmt = u32(d, rec + 4)
        fields: list[tuple[int, int, int, int]] = []     # (id, count, size, word-or--1)
        if u32(d, rec) & 1:
            p = rec + 0x28
            for _ in range(64):
                if p + 4 > len(d):
                    raise Sxd1Error("field list runs off the file")
                fid, cnt = d[p], d[p + 1]
                size = struct.unpack_from("<h", d, p + 2)[0]
                word = u32(d, p + 4) if size >= 6 and p + 8 <= len(d) else -1
                fields.append((fid, cnt, size, word))
                if size == 0:
                    break
                p += 2 + size
            else:
                raise Sxd1Error("field list does not terminate")
        recs.append({"k": k, "rec": rec, "flags": u32(d, rec), "fmt": fmt, "rate": u32(d, rec + 8),
                     "length": u32(d, rec + 0x20), "start": u32(d, rec + 0x24), "fields": fields,
                     "wave": wave})
    return recs


def asset_checks(files: dict[str, bytes], sizes: dict[str, int], c: Collector) -> dict:
    parsed: dict[str, list[dict]] = {}
    for name in SXD1:
        try:
            parsed[name] = parse_sxd1(files[name])
        except (Sxd1Error, struct.error, KeyError, IndexError) as exc:
            c.check("A01", False, f"{name}: parses as SXDF chunks + WAVE offset table", str(exc))
            return {}
    c.check("A01", all(len(parsed[n]) == WAVE_COUNTS[n] for n in SXD1),
            "all five .sxd1 parse; WAVE counts are 33/414/133/194/56 (830 records)",
            str({n.split('/')[-1]: len(parsed[n]) for n in SXD1}))

    census = Counter()
    for recs in parsed.values():
        for r in recs:
            census[(r["fmt"] & 0xFF, (r["fmt"] >> 8) & 0xFF)] += 1
    c.check("A02", dict(census) == FORMAT_CENSUS and all(
        (r["fmt"] >> 16) == 0x00FF for rs in parsed.values() for r in rs),
        "format-dword census: 0x42 x{1:17,2:131,4:3,6:415}=566, 0x21 x263, 0x20 x1; byte 2 = 0xff",
        str({(hex(k[0]), k[1]): v for k, v in sorted(census.items())}))

    at9, problems, other_0a = [], [], 0
    for name, recs in parsed.items():
        for r in recs:
            ids = [f[0] for f in r["fields"]]
            is9 = (r["fmt"] & 0xFF) == 0x42
            if not is9:
                other_0a += ids.count(0x0A)
                continue
            f0a = [f for f in r["fields"] if f[0] == 0x0A]
            if len(f0a) != 1 or f0a[0][1] != 1 or f0a[0][2] != 6 or (f0a[0][3] >> 24) != 0xFE:
                problems.append((name, r["k"], "field 0x0A shape/tag"))
                continue
            if 0x0C in ids or 0x0F in ids:
                problems.append((name, r["k"], "field 0x0C/0x0F present"))
            r["word"], r["channels"] = f0a[0][3], (r["fmt"] >> 8) & 0xFF
            at9.append((name, r))
    c.check("A04", not problems and len(at9) == 566,
            "all 566 format-0x42 records carry exactly one field 0x0A {cnt 1, size 6, tag 0xFE} and "
            "no field 0x0C/0x0F (so the arm always takes the inline-descriptor path)",
            f"{len(at9)} records; problems={problems[:3]}")
    c.check("A04b", other_0a == 0,
            "no record of any other format (0x20/0x21) carries field 0x0A (it is ATRAC9-specific)",
            str(other_0a))
    words = Counter(r["word"] for _n, r in at9)
    c.check("A05", dict(words) == WORD_CENSUS,
            "configuration-word census over the 566 records is the six pinned words",
            str({hex(k): v for k, v in sorted(words.items())}))
    music = Counter(r["word"] for n, r in at9 if n == SXD1[0])
    c.check("A05b", dict(music) == {0xFE761DF0: 33}, "music.sxd1 is 33/33 at 0xFE761DF0",
            str({hex(k): v for k, v in music.items()}))

    # A06: replay the arm per record
    bad = []
    geo_seen: dict[int, dict] = {}
    for n, r in at9:
        g = arm_model([r["word"]], [(r["channels"], 1)])
        if isinstance(g, str):
            bad.append((n, r["k"], g))
            continue
        geo_seen[r["word"]] = g
        want = GEOMETRY.get(r["word"])
        if (g["channels_covered"] != r["channels"] or g["words_used"] != 1 or want is None
                or (r["channels"], g["samples_per_frame"], g["frame_bytes"], g["frames_per_super"]) != want):
            bad.append((n, r["k"], "geometry/channels mismatch"))
    c.check("A06", not bad and set(geo_seen) == set(GEOMETRY),
            "replaying 0x984210 per record: each single word covers exactly the record's channel "
            "count ([19:17] table) and yields the pinned (ch, samples, frame bytes, frames/superframe)",
            f"{len(at9)} records; bad={bad[:3]}")
    # bits the arm does not read: validation bit 16, reserved [2:0], sync bit
    c.check("A06b", all(((r["word"] >> 16) & 1) == 0 and (r["word"] & 7) == 0 for _n, r in at9),
            "bit 16 and bits [2:0] are zero in every word (the arm ignores them)")
    c.check("A06c", all(r["rate"] == 48000 and ((r["word"] >> 20) & 0xF) == 7 for _n, r in at9),
            "the record's own sample rate is 48000 and the word's [23:20] index is 7 in all 566 "
            "(0x12b5700[7] = 256 samples per frame)")

    # A07: derived superframe size vs the segment lengths
    groups: dict[tuple[int, int], list[int]] = {}
    for _n, r in at9:
        groups.setdefault((r["channels"], r["word"]), []).append(r["length"])
    rows, ok7 = [], True
    for (ch, w), lens in sorted(groups.items()):
        g = arm_model([w], [(ch, 1)])
        blk = g["super_bytes"] if isinstance(g, dict) else 0
        gg = 0
        for L in lens:
            gg = gcd(gg, L)
        good = bool(blk) and all(L % blk == 0 for L in lens) and gg == blk
        ok7 &= good
        rows.append(f"{ch}ch/{w:#010x}: n={len(lens)} gcd={gg} super_bytes={blk}")
    c.check("A07", ok7 and len(groups) == 6,
            "for each of the six (channels, word) groups the derived superframe size (0x596) divides "
            "every segment length and equals the group gcd", "; ".join(rows))

    # A08: ranges against the pinned .sxd2 sizes
    ends = {n: max(r["start"] + r["length"] for r in rs) for n, rs in parsed.items()}
    music_recs = sorted(parsed[SXD1[0]], key=lambda r: r["start"])
    tiled = (music_recs[0]["start"] == 0x60 and all(
        music_recs[i]["start"] + music_recs[i]["length"] == music_recs[i + 1]["start"]
        for i in range(len(music_recs) - 1)))
    sx = {n: sizes.get(n[:-1] + "2", -1) for n in SXD1}
    c.check("A08", all(ends[n] == sx[n] for n in SXD1) and tiled and sx[SXD1[0]] == MUSIC_SXD2_BYTES
            and sum(r["length"] for r in music_recs) == MUSIC_SXD2_BYTES - 0x60,
            "the furthest segment end equals the pinned .sxd2 size in all five files, and the 33 "
            "music segments tile 0x60..end exactly (172,367,040 B = 179,549 x 960)",
            str({n.split('/')[-1]: (ends[n], sx[n]) for n in SXD1}))
    c.check("A08b", all(r["start"] + r["length"] <= sx[n] for n, rs in parsed.items() for r in rs),
            "no record of any format extends past its .sxd2")
    return {"at9": at9, "geo": geo_seen, "groups": groups, "words": words}


def load_assets(root: Path) -> tuple[dict[str, bytes], dict[str, int], str] | str:
    """(files, sizes, "") on success; a string reason when the private inputs are absent."""
    if not MANIFEST.exists():
        return "manifest absent"
    sys.path.insert(0, str(root / "scripts"))
    try:
        from psarc import Psarc  # type: ignore
    except ImportError:
        return "scripts/psarc.py unavailable"
    arch = root / ARCHIVE_REL
    if not arch.exists():
        return "pakN.psarc absent"
    man = json.loads(MANIFEST.read_text())
    by_path = {m["path"]: m for m in man["members"]}
    files, sizes = {}, {}
    p = Psarc(arch)
    try:
        p.members()
        for name in SXD1:
            m = by_path[name]
            data = p.read(p.entry_index(name))
            files[name] = data
            files[name + ".__sha"] = m["member_sha256"].encode()
            sizes[name[:-1] + "2"] = by_path[name[:-1] + "2"]["size"]
    finally:
        p.close()
    return files, sizes, ""


def asset_identity(files: dict[str, bytes], c: Collector) -> None:
    bad = [n for n in SXD1 if hashlib.sha256(files[n]).hexdigest().encode() != files[n + ".__sha"]]
    c.check("A00", not bad, "the five .sxd1 members hash to the manifest's pinned SHA-256", str(bad))


# ------------------------------------------------------------------------------ mutation probes

def _patch_elf(e: Elf, va: int, new: bytes) -> Elf:
    d = bytearray(e.data)
    o = e.off(va)
    d[o:o + len(new)] = new
    return Elf(bytes(d))


ELF_PROBES = [
    # (name, mutator(e)->Elf, check ids that MUST fail)
    ("0x987950 adds [ctx+0x28] instead of [ctx+0x20]",
     lambda e: _patch_elf(e, 0x987958, b"\x28"), {"E01b"}),
    ("0x987940 loses its null guard (je -> nop)",
     lambda e: _patch_elf(e, 0x987945, b"\x90\x90"), {"E01a"}),
    ("field 0x0A handler stores to 0x5f0 instead of 0x5f8",
     lambda e: _patch_elf(e, 0x980F3C, b"\xf0"), {"E05a", "E03b", "E06a"}),
    ("field 0x0A handler stores the record pointer, not 0x987940(...) (call -> nop)",
     lambda e: _patch_elf(e, 0x980F34, b"\x90\x90\x90\x90\x90"), {"E05a", "E02e"}),
    ("field-table entry for id 0x0A retargeted to the 0x0C handler",
     lambda e: _patch_elf(e, FIELD_TABLE + 4 * 0x0A,
                          struct.pack("<i", 0x980F78 - FIELD_TABLE)), {"E04f"}),
    ("field-list start moved from record+0x28 to record+0x2c",
     lambda e: _patch_elf(e, 0x980BFD, b"\x2c"), {"E04b"}),
    ("field loop advance changed from 2+size to 4+size",
     lambda e: _patch_elf(e, 0x9811FC, b"\x04"), {"E04d"}),
    ("per-wave reset no longer clears 0x5f8 (disp -> 0x5f0)",
     lambda e: _patch_elf(e, 0x980775, b"\xf0"), {"E03a", "E06a"}),
    ("a reader of [wave+0x5f8] becomes a writer (mov load -> mov store at 0x984145)",
     lambda e: _patch_elf(e, 0x984146, b"\x89"), {"E06b", "E06c", "E09a"}),
    ("kind-2 branch zero-store removed (disp 0x5f8 -> 0x5f0)",
     lambda e: _patch_elf(e, 0x980B15, b"\xf0"), {"E07d", "E06a"}),
    ("kind-1 inline word count changed 2 -> 3",
     lambda e: _patch_elf(e, 0x980A67, b"\x03"), {"E07c"}),
    ("spec-kind dispatch compares against 3 instead of 2",
     lambda e: _patch_elf(e, 0x98081D, b"\x03"), {"E02a"}),
    ("arm: sample-index bextr length 4 -> 5",
     lambda e: _patch_elf(e, 0x98415D, b"\x05"), {"E09a", "E09e"}),
    ("arm: sync tag compare 0xfe -> 0xfd",
     lambda e: _patch_elf(e, 0x984254, b"\xfd"), {"E09a", "E09f"}),
    ("arm: failure message pointer no longer reaches 'Invalid ATRAC9 ...'",
     lambda e: _patch_elf(e, 0x12B5660, b"X"), {"E09j"}),
    ("channel table 0x12b5720[3] 6 -> 5",
     lambda e: _patch_elf(e, 0x12B5723, b"\x05"), {"E11b"}),
    ("sample table 0x12b5700[7] 256 -> 255",
     lambda e: _patch_elf(e, 0x12B570E, b"\xff"), {"E11a"}),
    ("finaliser writes 0x594 instead of 0x596",
     lambda e: _patch_elf(e, 0x98411C, b"\x94"), {"E10d", "E10e"}),
    ("stream path divides by 0x594 instead of 0x596",
     lambda e: _patch_elf(e, 0x98397A, b"\x94"), {"E12a"}),
    ("a configuration word is planted in RELRO (0xfe top byte)",
     lambda e: _patch_elf(e, 0x1594000, bytes.fromhex("f01d76fe")), {"E13a", "E13b"}),
    ("a configuration word is planted in code padding",
     lambda e: _patch_elf(e, 0x12B5750, bytes.fromhex("f00f74fe")), {"E13a"}),
    ("0x987950 appears as a table pointer",
     lambda e: _patch_elf(e, 0x12B5760, struct.pack("<Q", 0x987950)), {"E13c"}),
    ("program-header pin broken (RELRO filesz)",
     lambda e: _patch_phdr(e), {"E00c"}),
]


def _patch_phdr(e: Elf) -> Elf:
    d = bytearray(e.data)
    e_phoff, = struct.unpack_from("<Q", d, 0x20)
    for i in range(e.e_phnum):
        p_type, = struct.unpack_from("<I", d, e_phoff + 56 * i)
        if p_type == 0x61000010:
            struct.pack_into("<Q", d, e_phoff + 56 * i + 32, 0x6D188)
    return Elf(bytes(d))


def _poke(files: dict[str, bytes], name: str, off: int, new: bytes) -> dict[str, bytes]:
    out = dict(files)
    b = bytearray(out[name])
    b[off:off + len(new)] = new
    out[name] = bytes(b)
    return out


def asset_probes(files: dict[str, bytes], sizes: dict[str, int], info: dict) -> list:
    at9 = info["at9"]
    music = [r for n, r in at9 if n == SXD1[0]]
    amb = [r for n, r in at9 if n == SXD1[1]]

    def field_off(name, rec, fid):
        d = files[name]
        p = rec["rec"] + 0x28
        while True:
            if d[p] == fid:
                return p
            p += 2 + struct.unpack_from("<h", d, p + 2)[0]

    m0 = music[0]
    f0a = field_off(SXD1[0], m0, 0x0A)
    w = struct.pack("<I", m0["word"])
    probes = [
        ("config word tag 0xfe -> 0xfd (music rec 0)",
         lambda: _poke(files, SXD1[0], f0a + 7, b"\xfd"), sizes, {"A04", "A05"}),
        ("frame bytes in the word 240 -> 224 (music rec 0)",
         lambda: _poke(files, SXD1[0], f0a + 4, struct.pack("<I", (m0["word"] & ~(0x7FF << 5)) | (223 << 5))),
         sizes, {"A05", "A06", "A07"}),
        ("rate/frame index 7 -> 6 (table entry is 0)",
         lambda: _poke(files, SXD1[0], f0a + 4, struct.pack("<I", (m0["word"] & ~(0xF << 20)) | (6 << 20))),
         sizes, {"A05", "A06"}),
        ("channel-config index 3 -> 2 (word now covers 2 of 6 channels)",
         lambda: _poke(files, SXD1[0], f0a + 4, struct.pack("<I", (m0["word"] & ~(7 << 17)) | (2 << 17))),
         sizes, {"A05", "A06"}),
        ("superframe index 2 -> 1 (frames per superframe 4 -> 2)",
         lambda: _poke(files, SXD1[0], f0a + 4, struct.pack("<I", (m0["word"] & ~(3 << 3)) | (1 << 3))),
         sizes, {"A05", "A06", "A07"}),
        ("validation bit 16 set",
         lambda: _poke(files, SXD1[0], f0a + 6, bytes([w[2] | 1])), sizes, {"A05", "A06b"}),
        ("field id 0x0A -> 0x0D in one ATRAC9 record",
         lambda: _poke(files, SXD1[0], f0a, b"\x0d"), sizes, {"A04"}),
        ("field 0x0A count 1 -> 2",
         lambda: _poke(files, SXD1[0], f0a + 1, b"\x02"), sizes, {"A04"}),
        ("field 0x0A size 6 -> 8",
         lambda: _poke(files, SXD1[0], f0a + 2, b"\x08"), sizes, {"A04", "A05"}),
        ("record channel byte 6 -> 2",
         lambda: _poke(files, SXD1[0], m0["rec"] + 5, b"\x02"), sizes, {"A02", "A06"}),
        ("record format 0x42 -> 0x21 (loses its codec)",
         lambda: _poke(files, SXD1[0], m0["rec"] + 4, b"\x21"), sizes, {"A02", "A04"}),
        ("record segment length +1 byte",
         lambda: _poke(files, SXD1[0], m0["rec"] + 0x20, struct.pack("<I", m0["length"] + 1)),
         sizes, {"A07", "A08"}),
        ("record sample rate 48000 -> 44100",
         lambda: _poke(files, SXD1[0], m0["rec"] + 8, struct.pack("<I", 44100)), sizes, {"A06c"}),
        ("WAVE count field 33 -> 32",
         lambda: _poke(files, SXD1[0], m0["wave"] + 0x0C, struct.pack("<I", 32)), sizes, {"A01"}),
        ("member bytes differ from the manifest hash only",
         lambda: _poke(files, SXD1[0], 0x20, b"Z"), sizes, {"A00"}),
        ("pinned .sxd2 size shrunk by 960",
         lambda: files, {**sizes, "sound/shadow/music.sxd2": MUSIC_SXD2_BYTES - 960}, {"A08", "A08b"}),
    ]
    a0a = field_off(SXD1[1], amb[0], 0x0A)
    probes.append(("str_amb record: config word swapped for another real word (0xFE740FF0 on a 6ch wave)",
                   lambda: _poke(files, SXD1[1], a0a + 4, struct.pack("<I", 0xFE740FF0))
                   if amb[0]["word"] != 0xFE740FF0 else _poke(files, SXD1[1], a0a + 4, b"\xf0\x1d\x76\xfe"),
                   sizes, {"A05", "A06"}))
    return probes


def run_probes(elf: Elf | None, files: dict[str, bytes] | None, sizes, info) -> tuple[int, int, list[str]]:
    ran = bad = 0
    lines: list[str] = []
    if elf is not None:
        for name, mut, must in ELF_PROBES:
            c = Collector()
            try:
                elf_checks(mut(elf), c, identity=False)
            except Exception as exc:  # a probe must never crash the checker silently
                lines.append(f"PROBE-ERROR  {name}: {exc!r}")
                bad += 1
                continue
            ran += 1
            failed = c.failed()
            if must <= failed:
                lines.append(f"PASS  probe[elf]   {name}  -> failed {sorted(must & failed)}")
            else:
                bad += 1
                lines.append(f"FAIL  probe[elf]   {name}  -> expected {sorted(must)} got {sorted(failed)}")
    if files is not None and info:
        for name, mk, sz, must in asset_probes(files, sizes, info):
            c = Collector()
            mutated = mk()
            asset_identity(mutated, c)
            asset_checks(mutated, sz, c)
            ran += 1
            failed = c.failed()
            if must <= failed:
                lines.append(f"PASS  probe[asset] {name}  -> failed {sorted(must & failed)}")
            else:
                bad += 1
                lines.append(f"FAIL  probe[asset] {name}  -> expected {sorted(must)} got {sorted(failed)}")
    return ran, bad, lines


# ------------------------------------------------------------------------------------------ main

def print_results(c: Collector) -> int:
    n = 0
    for r in c.results:
        print(f"{'PASS' if r.ok else 'FAIL'}  {r.cid:<5} {r.text}" + (f"   [{r.detail}]" if r.detail and not r.ok else ""))
        n += 0 if r.ok else 1
    return n


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--elf", type=Path, default=ELF_DEFAULT)
    ap.add_argument("--no-probes", action="store_true", help="skip the built-in mutation probes")
    ap.add_argument("--require-private", action="store_true", help="a missing private input is fatal")
    ap.add_argument("--report", action="store_true", help="print the derived geometry table")
    args = ap.parse_args(argv)

    c = Collector()
    elf = None
    if args.elf.exists():
        elf = Elf(args.elf.read_bytes())
        elf_checks(elf, c)
        note = objdump_crosscheck(elf, c, args.elf)
    else:
        note = ""
        print(f"SKIP  ELF half: {args.elf} is absent")

    loaded = load_assets(ROOT)
    files = sizes = None
    info: dict = {}
    if isinstance(loaded, str):
        print(f"SKIP  asset half: {loaded}")
    else:
        files, sizes, _ = loaded
        asset_identity(files, c)
        info = asset_checks(files, sizes, c)

    failures = print_results(c)
    if note:
        print(note)

    if args.report and info:
        print("\nconfig word   ch  samples/frame  frame B  frames/superframe  superframe samples  superframe B  records")
        for w, (ch, s, fb, m) in sorted(GEOMETRY.items()):
            print(f"{w:#010x}  {ch:>2}  {s:>13}  {fb:>7}  {m:>17}  {s * m:>18}  {fb * m:>12}  {info['words'][w]:>7}")

    probe_bad = 0
    if not args.no_probes and (elf is not None or files is not None):
        if failures:
            print("\nprobes not run: baseline checks already failing")
        else:
            ran, probe_bad, lines = run_probes(elf, files, sizes, info)
            print()
            for ln in lines:
                print(ln)
            print(f"probes: {ran - probe_bad}/{ran} detected")
            if ran == 0:
                probe_bad = 1

    if args.require_private and (elf is None or isinstance(loaded, str)):
        print("FAILED: --require-private and a private input is missing")
        return 2
    total = len(c.results)
    if failures or probe_bad:
        print(f"\nFAILED: {failures} of {total} checks failed, {probe_bad} probe(s) not detected")
        return 1
    if elf is None and files is None:
        print("\nSKIP: no private inputs present; nothing checked")
        return 0
    print(f"\nOK: {total} checks passed"
          + ("" if args.no_probes else " and every mutation probe was detected"))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
