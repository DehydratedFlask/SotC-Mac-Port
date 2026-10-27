#!/usr/bin/env python3
"""ELF-tie the 19 "zero static object + __cxa_atexit" constructor slots of core/ctor_tiny_atexit_slots.

Offline only: opens the pinned private ELF, reads bytes, never loads, relocates or executes an original
RVA. For every slot it pins:
  * the constructor-table word (u64 at ELF file offset 0x2dda750 + (306 - index) * 8) == the body RVA;
  * the exact body bytes and their SHA-256 (the pins below are literals, not derived from the ELF);
  * a straight-line decode of those bytes (tools/lift/tiny_slot_decode.py): the only effects are zero
    stores into one or two .bss objects, each followed by exactly one call/tail-call to the
    __cxa_atexit import (PLT 0xf8f390, NID tsvEmnenz48) with (dtor, object, dso 0x1604218);
  * that the decoded (object, dtor, zero offset, zero length, body size) of every record equals the
    native table kTinyAtexitRows in native/core/ctor_tiny_atexit_slots.hpp, row by row and in order;
  * the destructors are file-backed code, the objects are mapped .bss (not file-backed), and the
    destructor is only ever an identity tag in native code (no call is made through it).
`SOTC_CHECK_TAMPER=1` corrupts one pinned expectation in memory to prove the checker is fail-visible.

Private: needs local/re/eboot.elf.
"""

from __future__ import annotations

import hashlib
import os
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
sys.path.insert(0, str(ROOT / "tools/lift"))
import ps4_dynlink as d  # noqa: E402
import tiny_slot_decode as t  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR_TABLE_FILE_OFFSET_306 = 0x2DDA750   # ELF FILE offset (not a vaddr); the table is stored backwards
DSO = 0x1604218
ATEXIT_PLT = 0xF8F390
ATEXIT_NID = "tsvEmnenz48"

# (slot, body RVA, body size, body SHA-256, body bytes hex)
BODIES = [
    (311, 0xa46b70, 37, "020224f653760eb815c33781c217079aca02a24323a181976417cf43728ebcbb",
     "488d3da9faffff488d159ad6bb00488d35ab0a8b0248c705a00a8b0200000000e9fb875400"),
    (425, 0x54b9c0, 77, "111c32ac415f34f5ab45c686f31a1210eef63ad72ad05ab7ef2f770854ceb0f7",
     "554889e55350488d1d4b880b01488d35743a9b02488d3dd542ffff4889da48c70600000000e8a639a400488d355f3a9b02488d3de842ffff4889da48c706000000004883c4085b5de98339a400"),
    (432, 0x51bc20, 42, "a176eaba1f4b395c1b570bac17c64d9a94dd515d1705c20628c3224e6e847524",
     "488d35090d9e02488d3d72f7ffff488d15e3850e01c5f857c0c5f8110648c7461000000000e94637a700"),
    (471, 0x3600f0, 43, "4a17255111a3563e0a9ca02d2cdb4348af2705e0a7e9c454f76ce3f2cbb147ac",
     "488d35d953b902488d3db2eeffff488d1513412a01c5f857c0c5f811460848c7461800000000e975f2c200"),
    (515, 0x1c00e0, 33, "37d79ce2907a04a6e207f14054ab58253aea4afd91ecd7ec5d40dacc4cc6e7e8",
     "488d35e167d202488d3df2d9ffff488d152341440148c70600000000e98ff2dc00"),
    (522, 0x16e960, 33, "a2038a891ab73a82b854a70f6a85c7b1fba775aaf544d4732c6c166ece8fdd84",
     "488d35a954d702488d3d32e4ffff488d15a358490148c70600000000e90f0ae200"),
    (523, 0x16cd50, 33, "7dc4c4dda7695e1f83887695cdaec341ddf70626a3335f983bd33241657c5b95",
     "488d35b170d702488d3de2ebffff488d15b374490148c70600000000e91f26e200"),
    (525, 0x166ef0, 33, "36f435a1b776af8ca57de50436d2a4280fa0c8c5725dc7a01fff953c7f2a3ac4",
     "488d35e1cdd702488d3dd2cbffff488d1513d3490148c70600000000e97f84e200"),
    (527, 0x157e60, 33, "b4a88afd6391ba7c21aebd817baa2617a1f232301ac27a046338872975c598f0",
     "488d35b9bdd802488d3d82f1ffff488d15a3c34a0148c70600000000e90f75e300"),
    (528, 0x156fb0, 33, "8e272f99d0de49b0757593e0f2e92802958ed166a7e0ff5e18010e64e0651728",
     "488d3561ccd802488d3d02f2ffff488d1553d24a0148c70600000000e9bf83e300"),
    (531, 0x1442c0, 33, "c26d0735db5c5e961e5f87a40fedc67288d03fb3b718ee59fc3904e3dee613f5",
     "488d3531ead902488d3dc2ffffff488d1543ff4b0148c70600000000e9afb0e400"),
    (533, 0x143300, 33, "ef1a0037bb9e7c347db968b7a01fcedb441fbde2a6ba3d7c0bc1088dbeab13d6",
     "488d3549f7d902488d3d02feffff488d15030f4c0148c70600000000e96fc0e400"),
    (542, 0xfb3d0, 33, "6c3faa0e74883e54abf295e133acf03065db7ed8bb20ad166e4efcc2b9a108dc",
     "488d35893bde02488d3da2fbffff488d15338e500148c70600000000e99f3fe900"),
    (546, 0xa25f0, 33, "a147298c876e1be8f55ff956e3685cdc8676fbf566ab39ddd8051f9fc6d70110",
     "488d35c911e302488d3da2f7ffff488d15131c560148c70600000000e97fcdee00"),
    (547, 0x9a970, 33, "16f45798a45dd60798103610659a6d3bada8403054c0af03510505469457e78b",
     "488d35398ee302488d3d42ebffff488d159398560148c70600000000e9ff49ef00"),
    (549, 0x96650, 33, "e0f5c805f97a95383b25f5fb8e545dc20be78cf3db4d2f5d5fb00a25ee86c91e",
     "488d3519d0e302488d3dc2efffff488d15b3db560148c70600000000e91f8def00"),
    (551, 0x8d740, 33, "aa273c8c8d57b4c7b40026b7997225d9682d6b7e971c022dab955a7dbcd5f4f3",
     "488d35a95ce402488d3dd2feffff488d15c36a570148c70600000000e92f1cf000"),
    (564, 0x31220, 33, "c1cc9ee99113f63c11a0cc363e0f2fa132b1a806e8459b254181b335766c5d7e",
     "488d3551c4dc02488d3df2eaffff488d15e32f5d0148c70600000000e94fe1f500"),
    (565, 0x2fcf0, 33, "ac26ab7ed9241a63dc23bdbac61eb4d83a70fe13a3f45def74846e20efa3f679",
     "488d35c1d7dc02488d3d9265ffff488d1513455d0148c70600000000e97ff6f500"),
]


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def native_rows(header: str):
    body = header[header.index("kTinyAtexitRows"):]
    body = body[:body.index("}};")]
    rows = []
    for m in re.finditer(
            r"\{(\d+), (0x[0-9a-f]+), (\d+), (\d), \{\{\{(0x[0-9a-f]+), (0x[0-9a-f]+), (\d+), (\d+)\}, "
            r"\{(0x[0-9a-f]+|0), (0x[0-9a-f]+|0), (\d+), (\d+)\}\}\}\}", body):
        g = m.groups()
        rec = [(int(g[4], 16), int(g[5], 16), int(g[6]), int(g[7]))]
        if int(g[3]) == 2:
            rec.append((int(g[8], 16), int(g[9], 16), int(g[10]), int(g[11])))
        else:
            require(g[8:] == ("0", "0", "0", "0"), f"slot {g[0]} unused record must be all zero")
        rows.append((int(g[0]), int(g[1], 16), int(g[2]), rec))
    return rows


def decode_records(img, rva: int, size: int):
    """-> list of (object, dtor, zero_offset, zero_length) from the straight-line effects."""
    events = t.run(img, rva, size)
    zeros, records = [], []
    for e in events:
        if e[0] == "store":
            require(e[3] == 0 and isinstance(e[1], int), f"{rva:#x}: non-zero or non-absolute store {e}")
            zeros.append((e[1], e[2]))
        elif e[0] in ("call", "tail"):
            require(e[1] == ATEXIT_PLT, f"{rva:#x}: transfer to {e[1]:#x}, not __cxa_atexit")
            dtor, obj, dso = e[2][0], e[2][1], e[2][2]
            require(dso == DSO, f"{rva:#x}: dso {dso} != {DSO:#x}")
            require(isinstance(dtor, int) and isinstance(obj, int), f"{rva:#x}: unresolved atexit args")
            lo = min([a for a, _ in zeros], default=obj)
            hi = max([a + n for a, n in zeros], default=obj)
            require(sum(n for _, n in zeros) == hi - lo, f"{rva:#x}: zero stores must be contiguous")
            require(not zeros or lo >= obj, f"{rva:#x}: zero store below the object")
            records.append((obj, dtor, lo - obj, hi - lo))
            zeros = []
        else:
            require(e[0] == "ret", f"{rva:#x}: unexpected event {e}")
    require(not zeros, f"{rva:#x}: zero stores after the last atexit")
    require(events[-1][0] in ("tail", "ret"), f"{rva:#x}: bad terminator")
    return records


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    parsed = d.parse(elf)
    img = d.Image(elf, parsed["ph"])

    bodies = list(BODIES)
    if os.environ.get("SOTC_CHECK_TAMPER") == "1":   # mutation sanity: one expected byte flipped
        slot, rva, size, sha, hx = bodies[4]
        bodies[4] = (slot, rva, size, sha, hx[:-2] + ("00" if hx[-2:] != "00" else "01"))

    require(t.import_name(parsed, img, ATEXIT_PLT) == ATEXIT_NID == d.nid("__cxa_atexit"),
            "0xf8f390 is not the __cxa_atexit import")

    header = (ROOT / "native/core/ctor_tiny_atexit_slots.hpp").read_text()
    source = (ROOT / "native/core/ctor_tiny_atexit_slots.cpp").read_text()
    cmake = (ROOT / "native/CMakeLists.txt").read_text()
    require("kTinyAtexitDsoRva = 0x1604218" in header and "kTinyAtexitPltRva = 0xf8f390" in header,
            "native dso/plt constants")
    require("kTinyAtexitRowCount = 19" in header, "native row count")
    native = native_rows(header)
    require(len(native) == len(BODIES) == 19, f"native rows {len(native)} != 19")

    all_dtors = set()
    for (slot, rva, size, sha, hx), (n_slot, n_rva, n_size, n_rec) in zip(bodies, native):
        tag = f"slot {slot} / {rva:#x}"
        word = struct.unpack_from("<Q", elf, CTOR_TABLE_FILE_OFFSET_306 + (306 - slot) * 8)[0]
        require(word == rva, f"{tag}: constructor-table word {word:#x}")
        body = img.read(rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"{tag}: body sha256")
        require(body.hex() == hx, f"{tag}: body bytes")
        require(body[-5] == 0xE9 or body[-1] == 0xC3, f"{tag}: body must end in jmp rel32 or ret")
        records = decode_records(img, rva, size)
        require(1 <= len(records) <= 2, f"{tag}: {len(records)} records")
        require((n_slot, n_rva, n_size) == (slot, rva, size), f"{tag}: native row header {n_slot} {n_rva:#x} {n_size}")
        require(records == n_rec, f"{tag}: decoded {records} != native {n_rec}")
        for obj, dtor, off, length in records:
            require(img.mapped(obj, off + length) , f"{tag}: object {obj:#x} not mapped")
            try:
                img.read(obj, 1)
                require(False, f"{tag}: object {obj:#x} must be .bss (not file-backed)")
            except ValueError:
                pass
            require(img.read(dtor, 1) != b"", f"{tag}: dtor {dtor:#x} not file-backed code")
            require(dtor < obj, f"{tag}: dtor above object")
            all_dtors.add(dtor)
    slots = [b[0] for b in BODIES]
    require(slots == sorted(slots) and len(set(slots)) == 19, "slots must be strictly increasing")

    # The 15 thirty-three-byte members agree with the pre-existing STATIC-CLEANUP-THUNKS checker pins.
    sys.path.insert(0, str(ROOT / "tools/lift"))
    import importlib.util
    spec = importlib.util.spec_from_file_location("vct", ROOT / "tools/lift/verify_cleanup_thunks.py")
    vct = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(vct)
    by_slot = {b[0]: b for b in BODIES}
    for index, rva_hex, sha, cell_hex, dtor_hex in vct.ENTRIES:
        slot, rva, size, my_sha, _ = by_slot[index]
        require((rva, size, my_sha) == (int(rva_hex, 16), 33, sha), f"slot {index}: disagrees with verify_cleanup_thunks.py")
        n = next(r for r in native if r[0] == index)
        require(n[3] == [(int(cell_hex, 16), int(dtor_hex, 16), 0, 8)], f"slot {index}: cell/dtor vs verify_cleanup_thunks")

    # Native source: destructors are tags only, no call through an original address.
    require("log.record(record.dtor_rva, object)" in source, "native must only RECORD the destructor")
    require("std::memset(object + record.zero_offset, 0, record.zero_length)" in source, "native zero store")
    require("ctor_tiny_atexit_slots" in cmake and "test_ctor_tiny_atexit_slots" in cmake, "CMake wiring")

    print(f"OK check_ctor_tiny_atexit_slots: {len(BODIES)} slots, "
          f"{sum(len(n[3]) for n in native)} atexit records, dso {DSO:#x}, import {ATEXIT_NID}; "
          f"{len(all_dtors)} distinct destructors (tags only)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
