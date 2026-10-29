#!/usr/bin/env python3
"""ELF-tie startup constructor slots 296 (0xb2b6b0), 297 (0xb25470) and 300 (0xacc880).

Offline only: this script opens the pinned private ELF, reads bytes, and never loads, relocates or
executes an original RVA. It pins:
  * slot 296: the 12-byte tail thunk, its `lea` object target 0x334ac18 (BSS), its `jmp` callee
    0x323d0, the callee hash, the original-table word at 0x2dd67a0, and the native constants;
  * slot 297: the full body hash/bytes, the loop constants (stride 0x118, bound 0x1b690), the exact
    store/call order, the registered destructor 0xb24be0 contract bytes, and the native constants;
  * slot 300: the full 21-byte body, the bare-`ret` destructor 0xacc5b0, and the native constants;
  * the production rows in native/core/startup_ctors.cpp, in original index order (449 rows,
    including slots 296 through 306 and the newly wired existing-leaf rows).

Private: needs local/re/eboot.elf.
"""

from __future__ import annotations

import hashlib
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
TABLE_START = 0x2DD70E0

SLOT_296 = 0xB2B6B0
SLOT_296_SIZE = 12
SLOT_296_SHA256 = "d986eab2359568efdd00fa2ed9b8084b9ab5002d1904154ddc573a2f56f1ce80"
SLOT_296_BYTES = "488d3d61f58102e9146d50ff"
SLOT_296_OBJECT = 0x334AC18
SLOT_296_CALLEE = 0x323D0
SLOT_296_CALLEE_SIZE = 67
SLOT_296_CALLEE_SHA256 = "32da90bc4ce93a445148b51b3131ce7ca4ef6a5c88116c4c80cf0b87a437f70c"
SLOT_296_CALLEE_BYTES = (
    "554889e54156534c8d352223e7024889fb49833e00751be874d4f5004989060f31488d0d1023e702"
    "48c1e2204809c24889110f3148c1e2204809c24889135b415e5dc3"
)
TABLE_WORD_296 = 0x2DD70E0 - 8 * 296
TABLE_WORD_297 = 0x2DD70E0 - 8 * 297
TABLE_WORD_300 = 0x2DD70E0 - 8 * 300

SLOT_297 = 0xB25470
SLOT_297_SIZE = 105
SLOT_297_SHA256 = "8e2300c708f4ab249fdcc39e38b4b57d31d4b4457909b7d349bf6e7ff1670023"
SLOT_297_BYTES = (
    "554889e5415653bb100100004c8d35cda18002666666662e0f1f840000000000e8eb8362ff4889c74a"
    "894433f842c6043300e8097862ff42c68433f0feffff004881c3180100004881fb90b6010075d0488d"
    "1551edad00488d3d12f7ffff31f65b415e5de9b79e4600"
)
SLOT_297_TABLE = 0x332F650
SLOT_297_LOOP_SITE = 0xB25490
SLOT_297_LOOP_SIZE = 0x2E
SLOT_297_LOOP_BYTES = (
    "e8eb8362ff4889c74a894433f842c6043300e8097862ff42c68433f0feffff00"
    "4881c3180100004881fb90b60100"
)
SLOT_297_ACQUIRE = 0x14D880
SLOT_297_RETAIN = 0x14CCB0
SLOT_297_ATEXIT = 0xF8F390
SLOT_297_DSO = 0x1604218
SLOT_297_DTOR = 0xB24BE0
SLOT_297_DTOR_SIZE = 0x59
SLOT_297_DTOR_SHA256 = "c79c71fae27dc022d478d3ff38cf57f42e43d4e363fc4479864de6999a2cd1e4"
SLOT_297_DTOR_BYTES = (
    "554889e5415653bb80b501004c8d355daa8002666666662e0f1f84000000000042807c33f80074"
    "104a8b7c33f0e8be8662ff42c64433f8004a8b7c33f0e89e8062ff4ac74433f0000000004881c3e8"
    "feffff75cc5b415e5dc3"
)
SLOT_297_DTOR_RELEASE = 0x14D2D0
SLOT_297_DTOR_DROP_REF = 0x14CCC0

SLOT_300 = 0xACC880
SLOT_300_SIZE = 21
SLOT_300_SHA256 = "1b9392bdc540eb649e1f2de46d40f61f0e0899a02161310befeb31039dc3f87e"
SLOT_300_BYTES = "488d159179b300488d3d22fdffff31f6e9fb2a4c00"
SLOT_300_DTOR = 0xACC5B0
SLOT_300_DSO = 0x1604218
SLOT_300_ATEXIT = 0xF8F390


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL {message}")


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def lea_abs(site: int, img: d.Image) -> int:
    b = img.read(site, 7)
    require(b[0] in (0x48, 0x4C) and b[1] == 0x8D, f"missing lea at {site:#x}")
    return site + 7 + struct.unpack("<i", b[3:7])[0]


def rel_target(site: int, img: d.Image, opcode: int) -> int:
    b = img.read(site, 5)
    require(b[0] == opcode, f"missing opcode {opcode:#x} at {site:#x}")
    return site + 5 + struct.unpack("<i", b[1:5])[0]


def call_target(site: int, img: d.Image) -> int:
    return rel_target(site, img, 0xE8)


def jmp_target(site: int, img: d.Image) -> int:
    return rel_target(site, img, 0xE9)


def const_value(text: str, name: str) -> int:
    match = re.search(rf"\b{re.escape(name)}\s*=\s*(0x[0-9a-fA-F]+|\d+)\s*;", text)
    require(match is not None, f"missing native constant {name}")
    return int(match.group(1), 0)


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "unexpected ELF SHA256")
    _, program_headers = d.program_headers(elf)
    img = d.Image(elf, program_headers)

    # ---------------------------------------------------------------- slot 296
    thunk = img.read(SLOT_296, SLOT_296_SIZE)
    require(digest(thunk) == SLOT_296_SHA256, "slot 296 thunk sha256")
    require(thunk.hex() == SLOT_296_BYTES, "slot 296 thunk bytes")
    require(lea_abs(SLOT_296, img) == SLOT_296_OBJECT, "slot 296 lea object 0x334ac18")
    require(jmp_target(SLOT_296 + 7, img) == SLOT_296_CALLEE, "slot 296 jmp 0x323d0")
    require(struct.unpack("<Q", img.read(TABLE_WORD_296, 8))[0] == SLOT_296, "table word 0x2dd67a0")
    require(img.mapped(SLOT_296_OBJECT, 8), "slot 296 object is inside a loadable segment")
    try:
        img.read(SLOT_296_OBJECT, 8)
        require(False, "slot 296 object must be BSS (not file-backed)")
    except ValueError:
        pass

    callee = img.read(SLOT_296_CALLEE, SLOT_296_CALLEE_SIZE)
    require(digest(callee) == SLOT_296_CALLEE_SHA256, "0x323d0 sha256")
    require(callee.hex() == SLOT_296_CALLEE_BYTES, "0x323d0 bytes")
    require(callee[-1] == 0xC3, "0x323d0 ends ret")
    require(lea_abs(0x323D7, img) == 0x2EA4700, "0x323d0 guard cell")
    require(lea_abs(0x323F1, img) == 0x2EA4708, "0x323d0 base cell")
    require(img.read(0x323E1, 4).hex() == "49833e00", "0x323d0 cmp [r14],0")
    require(img.read(0x323E5, 2).hex() == "751b", "0x323d0 jne init")
    require(call_target(0x323E7, img) == 0xF8F860, "0x323d0 call 0xf8f860")
    require(img.read(0x323EF, 2).hex() == "0f31", "0x323d0 rdtsc (init)")
    require(img.read(0x32402, 2).hex() == "0f31", "0x323d0 rdtsc (join)")
    require(img.read(0x3240B, 3).hex() == "488913", "0x323d0 store stamp")

    header_296 = (ROOT / "native/core/ctor_slot_296_b2b6b0.hpp").read_text()
    source_296 = (ROOT / "native/core/ctor_slot_296_b2b6b0.cpp").read_text()
    require(const_value(header_296, "kSlot296ThunkRva") == SLOT_296, "kSlot296ThunkRva")
    require(const_value(header_296, "kSlot296ThunkSize") == SLOT_296_SIZE, "kSlot296ThunkSize")
    require(const_value(header_296, "kSlot296ObjectRva") == SLOT_296_OBJECT, "kSlot296ObjectRva")
    require(const_value(header_296, "kSlot296CalleeRva") == SLOT_296_CALLEE, "kSlot296CalleeRva")
    require("store_tsc_323d0(&g.tsc, hooks)" in source_296, "slot 296 must reuse store_tsc_323d0")
    require("frequency_from_hook" in header_296 and "sentinel" in header_296.lower(),
            "slot 296 must report the partial frequency")

    # ---------------------------------------------------------------- slot 297
    body_297 = img.read(SLOT_297, SLOT_297_SIZE)
    require(digest(body_297) == SLOT_297_SHA256, "slot 297 body sha256")
    require(body_297.hex() == SLOT_297_BYTES, "slot 297 body bytes")
    require(struct.unpack("<Q", img.read(TABLE_WORD_297, 8))[0] == SLOT_297, "table word 0x2dd6798")
    require(lea_abs(0xB2547C, img) == SLOT_297_TABLE, "slot 297 table 0x332f650")
    require(img.read(0xB25477, 5).hex() == "bb10010000", "slot 297 start offset 0x110")
    require(img.read(0xB254B0, 7).hex() == "4881c318010000", "slot 297 stride 0x118")
    require(img.read(0xB254B7, 7).hex() == "4881fb90b60100", "slot 297 bound 0x1b690")
    require(img.read(0xB254BE, 2).hex() == "75d0", "slot 297 loop branch")
    require(call_target(0xB25490, img) == SLOT_297_ACQUIRE, "slot 297 acquire call")
    require(call_target(0xB254A2, img) == SLOT_297_RETAIN, "slot 297 retain call")
    require(lea_abs(0xB254C0, img) == SLOT_297_DSO, "slot 297 DSO handle")
    require(lea_abs(0xB254C7, img) == SLOT_297_DTOR, "slot 297 atexit destructor")
    require(jmp_target(0xB254D4, img) == SLOT_297_ATEXIT, "slot 297 __cxa_atexit")
    require(img.read(0xB254CE, 6).hex() == "31f65b415e5d", "slot 297 epilogue")
    loop = img.read(SLOT_297_LOOP_SITE, SLOT_297_LOOP_SIZE)
    require(loop.hex() == SLOT_297_LOOP_BYTES, "slot 297 store/call order")

    destructor = img.read(SLOT_297_DTOR, SLOT_297_DTOR_SIZE)
    require(digest(destructor) == SLOT_297_DTOR_SHA256, "0xb24be0 sha256")
    require(destructor.hex() == SLOT_297_DTOR_BYTES, "0xb24be0 bytes")
    require(destructor[-1] == 0xC3, "0xb24be0 ends ret")
    require(img.read(0xB24BE7, 5).hex() == "bb80b50100", "destructor start offset 0x1b580")
    require(lea_abs(0xB24BEC, img) == SLOT_297_TABLE, "destructor table 0x332f650")
    require(img.read(0xB24C00, 8).hex() == "42807c33f8007410", "destructor flag test")
    require(img.read(0xB24C08, 5).hex() == "4a8b7c33f0", "destructor load handle (release)")
    require(call_target(0xB24C0D, img) == SLOT_297_DTOR_RELEASE, "destructor call 0x14d2d0")
    require(img.read(0xB24C12, 6).hex() == "42c64433f800", "destructor clear flag")
    require(img.read(0xB24C18, 5).hex() == "4a8b7c33f0", "destructor load handle (drop)")
    require(call_target(0xB24C1D, img) == SLOT_297_DTOR_DROP_REF, "destructor call 0x14ccc0")
    require(img.read(0xB24C22, 9).hex() == "4ac74433f000000000", "destructor clear handle")
    require(img.read(0xB24C2B, 7).hex() == "4881c3e8feffff", "destructor reverse stride")
    require(img.read(0xB24C32, 2).hex() == "75cc", "destructor reverse branch")

    header_297 = (ROOT / "native/core/ctor_slot_297_b25470.hpp").read_text()
    source_297 = (ROOT / "native/core/ctor_slot_297_b25470.cpp").read_text()
    require(const_value(header_297, "kSlot297Rva") == SLOT_297, "kSlot297Rva")
    require(const_value(header_297, "kSlot297BodySize") == SLOT_297_SIZE, "kSlot297BodySize")
    require(const_value(header_297, "kSlot297Count") == 400, "kSlot297Count")
    require(const_value(header_297, "kSlot297Stride") == 0x118, "kSlot297Stride")
    require(const_value(header_297, "kSlot297HandleOffset") == 0x108, "kSlot297HandleOffset")
    require(const_value(header_297, "kSlot297FlagOffset") == 0x110, "kSlot297FlagOffset")
    require(const_value(header_297, "kSlot297TableRva") == SLOT_297_TABLE, "kSlot297TableRva")
    require(const_value(header_297, "kSlot297DtorRva") == SLOT_297_DTOR, "kSlot297DtorRva")
    require(const_value(header_297, "kSlot297DtorSize") == SLOT_297_DTOR_SIZE, "kSlot297DtorSize")
    loop_source = source_297[source_297.index("for (std::size_t i = 0;"):]
    order = ["hooks.acquire_shared_14d880()", "rec.node = handle", "rec.flag110 = 0",
             "hooks.retain_14ccb0(", "rec.flag00 = 0"]
    positions = [loop_source.index(token) for token in order]
    require(positions == sorted(positions) and len(set(positions)) == len(positions),
            "slot 297 native store/call order")

    # ---------------------------------------------------------------- slot 300
    body_300 = img.read(SLOT_300, SLOT_300_SIZE)
    require(digest(body_300) == SLOT_300_SHA256, "slot 300 body sha256")
    require(body_300.hex() == SLOT_300_BYTES, "slot 300 body bytes")
    require(lea_abs(SLOT_300, img) == SLOT_300_DSO, "slot 300 DSO handle")
    require(lea_abs(SLOT_300 + 7, img) == SLOT_300_DTOR, "slot 300 destructor")
    require(jmp_target(SLOT_300 + 16, img) == SLOT_300_ATEXIT, "slot 300 __cxa_atexit")
    require(struct.unpack("<Q", img.read(TABLE_WORD_300, 8))[0] == SLOT_300, "table word 0x2dd6780")
    require(img.read(SLOT_300_DTOR, 1) == b"\xc3", "0xacc5b0 must be a bare ret")

    header_300 = (ROOT / "native/core/ctor_slot_300_acc880.hpp").read_text()
    source_300 = (ROOT / "native/core/ctor_slot_300_acc880.cpp").read_text()
    require(const_value(header_300, "kSlot300Rva") == SLOT_300, "kSlot300Rva")
    require(const_value(header_300, "kSlot300BodySize") == SLOT_300_SIZE, "kSlot300BodySize")
    require(const_value(header_300, "kSlot300DtorRva") == SLOT_300_DTOR, "kSlot300DtorRva")
    require(const_value(header_300, "kSlot300DtorSize") == 1, "kSlot300DtorSize")
    require("++cleanup.registered" in source_300 and "return true" in source_300,
            "slot 300 must record one inert entry and always succeed")

    # ---------------------------------------------------------------- production rows
    startup_source = (ROOT / "native/core/startup_ctors.cpp").read_text()
    startup_header = (ROOT / "native/core/startup_ctors.hpp").read_text()
    array_match = re.search(r"static constexpr CtorSlot kSlots\[\]\s*=\s*\{(.*?)\n\s*\};",
                            startup_source, re.DOTALL)
    require(array_match is not None, "cannot find recovered constructor array")
    block = array_match.group(1)
    rows = [(int(rva, 16), int(index)) for rva, index in re.findall(
        r"^\s*\{\s*(0x[0-9a-fA-F]+)\s*,\s*(\d+)\s*,\s*\[", block, re.MULTILINE)]
    require(len(rows) == 449, f"expected 449 registered rows (296-306 and verified lane rows included), found {len(rows)}")
    indices = [index for _, index in rows]
    require(indices == sorted(indices) and len(indices) == len(set(indices)),
            "constructor rows must be in strictly increasing index order")
    for index, rva in ((296, SLOT_296), (297, SLOT_297), (300, SLOT_300)):
        require((rva, index) in rows, f"missing registered row {index} / {rva:#x}")
    positions = {index: indices.index(index) for index in (295, 296, 297, 300, 304, 306)}
    require(positions[295] < positions[296] < positions[297] < positions[300] < positions[304] < positions[306],
            "slots 296/297/300 must sit between 295 and 306 in index order")
    require("init_slot296_b2b6b0(s.slot296_stamp)" in block, "slot 296 row callback")
    require("init_slot297_b25470(s.slot297_table, default_slot297_hooks())" in block,
            "slot 297 row callback")
    require("init_slot300_acc880(s.slot300_cleanup)" in block, "slot 300 row callback")
    for member in ("Slot296Stamp slot296_stamp", "Slot297Table slot297_table",
                   "Slot300Cleanup slot300_cleanup"):
        require(member in startup_header, f"missing StartupState member: {member}")

    print(
        "OK check_fctor_slots_296_297_300: "
        f"296 thunk {SLOT_296_SHA256[:12]}… -> {SLOT_296_OBJECT:#x} / jmp {SLOT_296_CALLEE:#x} "
        f"({SLOT_296_CALLEE_SHA256[:12]}…); "
        f"297 body {SLOT_297_SHA256[:12]}… 400x0x118 @ {SLOT_297_TABLE:#x}, dtor "
        f"{SLOT_297_DTOR_SHA256[:12]}…; "
        f"300 body {SLOT_300_SHA256[:12]}… + ret-dtor {SLOT_300_DTOR:#x}; 449 rows"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
