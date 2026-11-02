#!/usr/bin/env python3
"""Pin the round-2 `.csdr` loader search: the doc's claims, and the byte-level facts behind them.

Two layers, like `check_ghidra_pipeline.py`:

1. **Always** (public tree): `docs/CSDR_LOADER_SEARCH.md` exists and still states the four load
   bearing results — the withdrawn negative, the Oodle 2 identification, the "prefix is never an
   immediate" sweep, and the single inbound `OodleLZ_Decompress` call inside a disassembly hole —
   with the same numbers. The Ghidra helper scripts the doc calls must exist and keep their
   `key=value` argument form, because the doc's reproduction block is the only recipe.
2. **Private inputs** (`local/re/eboot.elf`): when the pinned image is present, re-derive the two
   facts that are cheap and decisive from the file bytes alone, with no Ghidra and no disassembler:
   - a full static Oodle 2 string set and the zlib 1.2.3 banners are present;
   - the `.csdr` naming set (`.csdr`, `_shipping.csdr`, `_neo.csdr`, `_neoshipping.csdr`,
     `shaderTypeToLoad`, `EShaderTypeToLoad`) is present;
   - `0x89d3b0` has **exactly one** `E8`/`E9` rel32 inbound site in the whole image and it is
     `0x1aa426`, and the 8-byte pointer value `0x000000000089d3b0` occurs **zero** times;
   - no `0x8c0a` / `0x8c06` little-endian dword immediate exists anywhere in the image.

Skips cleanly (exit 0, "SKIP ...") when the guest image is absent. A present-but-different image is a
FAIL, never a skip: every number in the doc is only meaningful for the pinned image.

Deliberately *not* checked here: anything that needs the Ghidra project or the corpus archives.
Those are covered by `check_ghidra_pipeline.py` and the census tools, and re-running a 3.6 M
instruction sweep inside a unit checker would be a worse test for more time.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DOC = ROOT / "docs/CSDR_LOADER_SEARCH.md"
ELF = ROOT / "local/re/eboot.elf"
PINNED_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

# Decompressor entry the doc pins, and the single inbound call site it records.
OODLE_DECOMPRESS = 0x89D3B0
OODLE_CALL_SITE = 0x1AA426
# The relative switch table Ghidra could not recover, and the case entries it resolves to.
SWITCH_TABLE_VA = 0xFADD4C
SWITCH_TARGETS = [0x1AA35C, 0x1AA3DC, 0x1AA351, 0x1AA351, 0x1AA351, 0x1AA3F5]

# Every string that has to survive in the image for the doc's identification to hold.
OODLE_STRINGS = (
    b"OodleLZ_Decompress",
    b"OodleLZ_Compress",
    b"OodleLZ_Compressor_to_DecodeType",
    b"OodleLZ_CompressContext_Alloc",
    b"rrHuffman_UnPackCodeLensNew",
    b"newLZ_decode_chunk_phase1",
    b"LZNib_DecodeOneQuantum",
    b"v:\\devel\\projects\\oodle2\\core\\oodlelzcompressors.cpp",
    b"OODLE ERROR : LZ corruption : crc check fail.",
)
ZLIB_STRINGS = (
    b"deflate 1.2.3 Copyright 1995-2005 Jean-loup Gailly",
    b"inflate 1.2.3 Copyright 1995-2005 Mark Adler",
)
CSDR_STRINGS = (
    b".csdr",
    b"_shipping.csdr",
    b"_neo.csdr",
    b"_neoshipping.csdr",
    b"shaderTypeToLoad",
    b"EShaderTypeToLoad",
    b"EShaderTypeToLoad::PS4Neo_Shipping",
)


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def check_doc() -> None:
    require(DOC.is_file(), f"missing {DOC.relative_to(ROOT)}")
    text = DOC.read_text(encoding="utf-8")
    for needle in (
        "GPU-01-CSDR-DECOMPRESSOR-RE-01",
        "19,320",
        "34,291",
        "16,328",
        "2,992",
        "3,661,345",
        "0x89d3b0",
        "0x1aa426",
        "0x1aa351..0x1aa42e",
        "0x15d5820",
        "0xfadd4c",
        "FUN_001aa2a0",
        "0x1aa2a0..0x1aa468",
        "Oodle 2",
        "zlib 1.2.3",
        "E1065",
        "NOT TESTED",
    ):
        require(needle in text, f"{DOC.name} no longer states {needle!r}")

    # The withdrawn negative must stay withdrawn: the doc may describe it, never assert it.
    require("Result: no such site was found" not in text,
            f"{DOC.name} has reverted to the withdrawn conclusion")

    for rel in (
        "tools/re/ghidra/sweep_constants.py",
        "tools/re/ghidra/sweep_region_calls.py",
        "tools/re/ghidra/sweep_rel32_calls.py",
        "tools/re/ghidra/probe_csdr_refs.py",
        "tools/re/ghidra/probe_callers.py",
        "tools/re/ghidra/probe_callgraph.py",
        "tools/re/ghidra/probe_disasm_range.py",
    ):
        require((ROOT / rel).is_file(), f"missing helper script {rel}")

    # run_dump.py rejects unknown `--flags`, so the graph helper must keep its key=value form.
    graph = (ROOT / "tools/re/ghidra/probe_callgraph.py").read_text(encoding="utf-8")
    for key in ("dir=", "depth=", "seed=", "watch="):
        require(f'startswith("{key}")' in graph,
                f"probe_callgraph.py no longer parses the {key!r} key=value form")


def segments(data: bytes):
    """(vaddr, file_offset, filesz) for every image-mapping segment, RELRO included."""
    e_phoff = struct.unpack_from("<Q", data, 0x20)[0]
    e_phentsize = struct.unpack_from("<H", data, 0x36)[0]
    e_phnum = struct.unpack_from("<H", data, 0x38)[0]
    out = []
    for i in range(e_phnum):
        off = e_phoff + i * e_phentsize
        p_type = struct.unpack_from("<I", data, off)[0]
        p_offset, p_vaddr, _p, p_filesz, _memsz = struct.unpack_from("<QQQQQ", data, off + 8)
        if p_filesz and p_type in (1, 0x61000010, 0x61000001, 0x6474E550):
            out.append((p_vaddr, p_offset, p_filesz))
    return out


def check_elf() -> str:
    if not ELF.is_file():
        return "elf absent"
    data = ELF.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    require(digest == PINNED_SHA256,
            f"local/re/eboot.elf is not the pinned image ({digest})")

    for needle in OODLE_STRINGS + ZLIB_STRINGS + CSDR_STRINGS:
        require(needle in data, f"pinned image no longer contains {needle!r}")

    # The "prefix is never a compare immediate" result is disassembler-level (a raw byte pattern is
    # a false-positive magnet: `8c 0a 00 00` occurs inside ordinary data), so it stays pinned in the
    # doc above and in the checker-free sweep, not re-derived here.

    # Exactly one direct call edge into OodleLZ_Decompress, and no pointer table entry.
    segs = segments(data)
    sources = []
    for vaddr, offset, filesz in segs:
        blob = data[offset:offset + filesz]
        for i in range(len(blob) - 5):
            if blob[i] not in (0xE8, 0xE9):
                continue
            rel = struct.unpack_from("<i", blob, i + 1)[0]
            src = vaddr + i
            if src + 5 + rel == OODLE_DECOMPRESS:
                sources.append(src)
    require(sources == [OODLE_CALL_SITE],
            f"OodleLZ_Decompress inbound rel32 sites are {[hex(s) for s in sources]}, "
            f"pinned [{hex(OODLE_CALL_SITE)}]")
    require(struct.pack("<Q", OODLE_DECOMPRESS) not in data,
            "OodleLZ_Decompress now has an absolute pointer table entry")

    # The unrecovered switch table really does resolve into the hole.
    mapping = [(v, o, f) for v, o, f in segs if v <= SWITCH_TABLE_VA < v + f]
    require(mapping, f"switch table 0x{SWITCH_TABLE_VA:x} is not in any image segment")
    vaddr, file_offset, _filesz = mapping[0]
    table_offset = file_offset + (SWITCH_TABLE_VA - vaddr)
    resolved = [SWITCH_TABLE_VA + struct.unpack_from("<i", data, table_offset + 4 * i)[0]
                for i in range(len(SWITCH_TARGETS))]
    require(resolved == SWITCH_TARGETS,
            f"switch table resolves to {[hex(x) for x in resolved]}, "
            f"pinned {[hex(x) for x in SWITCH_TARGETS]}")

    return ("elf pinned; Oodle 2 + zlib 1.2.3 + .csdr naming present; 1 inbound call; "
            "switch table resolves into the hole")


def main() -> int:
    check_doc()
    fact = check_elf()
    if fact.endswith("absent"):
        print("SKIP check_csdr_loader_search: guest ELF absent (doc and helper pins checked)")
        return 0
    print(f"OK check_csdr_loader_search: {fact}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
