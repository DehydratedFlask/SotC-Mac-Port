#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 505 (RVA 0x236030, 1476 B) and its two helpers
0x1621b0 (87 B min/max) / 0x162970 (8 B BSS seed accessor).

The body copies 2,704 bytes from 0xfc0580 in 84 ymm/xmm loads/stores, writes the qword
0x3f5415c8be50f81a at table +0xa90, copies the zero BSS seed 0x2ee3ca0 to bounds +0xa98, and calls
0x1621b0 exactly 226 times at stride 0xc. Offline only: reads the pinned private ELF, never loads,
relocates or executes an original RVA.
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from l2_slot_check import Spec, main, require  # noqa: E402
from slot_pin_util import ROOT  # noqa: E402
import ps4_dynlink as d  # noqa: E402

# ---- slot 505 operands -------------------------------------------------------------------------
PINS = [
    # 32-byte source loads: three initial + 81 pipelined loads.
    ("rip", 0x23603A, 8, 4, 0xFC0580, "source initial 0"),
    ("rip", 0x236042, 8, 4, 0xFC05A0, "source initial 1"),
    ("rip", 0x23604A, 8, 4, 0xFC05C0, "source initial 2"),
    # 32-byte stores: two initial + 79 pipelined + two tail stores.
    ("rip", 0x236064, 8, 4, 0x2EE9820, "table initial 0"),
    ("rip", 0x23606C, 8, 4, 0x2EE9840, "table initial 1"),
    ("rip", 0x23607C, 8, 4, 0x2EE9860, "table initial 2"),
]
for i in range(81):
    PINS.append(("rip", 0x236074 + 0x10 * i, 8, 4, 0xFC05E0 + 0x20 * i,
                 f"source loop {i}"))
for i in range(79):
    PINS.append(("rip", 0x23608C + 0x10 * i, 8, 4, 0x2EE9880 + 0x20 * i,
                 f"table loop {i}"))
PINS += [
    ("rip", 0x23657C, 8, 4, 0x2EEA260, "table penultimate store"),
    ("rip", 0x236584, 8, 4, 0x2EEA280, "table final 32-byte store"),
    # 16-byte source tail and table tail.
    ("rip", 0x236052, 8, 4, 0xFC1000, "source xmm tail"),
    ("rip", 0x23658C, 8, 4, 0x2EEA2A0, "table xmm tail"),
    ("rip", 0x236594, 7, 3, 0x2EEA2B0, "table qword tail"),
    # seed load/copy and loop anchors.
    ("rip", 0x2365A6, 7, 3, 0x2EE9820, "table base"),
    ("rip", 0x2365AD, 7, 3, 0x2EEA2B8, "bounds destination"),
    ("rip", 0x2365B4, 7, 3, 0x2EEA2C8, "bounds high qword"),
    ("rip", 0x2365BF, 8, 4, 0x2EEA2B8, "bounds low xmm"),
    # calls.
    ("call", 0x23659B, 0x162970, "seed accessor"),
    ("call", 0x2365D7, 0x1621B0, "min/max helper"),
    # semantic immediates / control flow.
    ("bytes", 0x236030, "554889e5415741565350", "prologue"),
    ("bytes", 0x23605A, "48b81af850bec815543f", "final vec3 tail constant"),
    ("bytes", 0x2365A0, "488b4810", "bounds +0x10 load"),
    ("bytes", 0x2365A4, "31db", "loop index zero"),
    ("bytes", 0x2365BB, "c5f81000", "bounds low load"),
    ("bytes", 0x2365D0, "498d341f", "element address"),
    ("bytes", 0x2365D4, "4c89f7", "bounds argument"),
    ("bytes", 0x2365DC, "4883c30c", "stride 0xc"),
    ("bytes", 0x2365E0, "4881fb980a0000", "limit 0xa98"),
    ("bytes", 0x2365E7, "75e7", "loop branch back"),
    ("bytes", 0x2365E9, "4883c4085b415e415f5dc3", "epilogue"),
    # ---- helper 0x1621b0: min/max order and ret ------------------------------------------------
    ("bytes", 0x1621B4, "c5fa5d06", "min x"),
    ("bytes", 0x1621C1, "c5fa5d4604", "min y"),
    ("bytes", 0x1621D0, "c5fa5d4608", "min z"),
    ("bytes", 0x1621DF, "c5fa5f06", "max x"),
    ("bytes", 0x1621ED, "c5fa5f4604", "max y"),
    ("bytes", 0x1621FC, "c5fa5f4608", "max z"),
    ("bytes", 0x162206, "c3", "helper ret"),
    # ---- helper 0x162970: BSS seed address and ret --------------------------------------------
    ("rip", 0x162970, 7, 3, 0x2EE3CA0, "seed cell"),
    ("bytes", 0x162977, "c3", "seed helper ret"),
]

CALLEES = {
    "minmax_1621b0": (0x1621B0, 87,
                       "58179eefd816f131c46c70c5f3da52cb4f2253f8ec3495a5be59ec1294915023"),
    "seed_162970": (0x162970, 8,
                     "2dfb9b38c67fee3a4978fa36d36f6254f2c119bbb43de0149c66cca30089795e"),
}


def elf_extra(e) -> None:
    # The 2,704-byte source table is file-backed rodata and pinned by its full SHA256.
    e.body_hash("source_505", 0xFC0580, 0xA90,
                "f060faa67715256fec5d1141b90bbbaaa589b5cb4aec3777e12779117f85a0ab",
                always=True)
    require(e.read(0xFC0580, 8).hex() == "67d20fbe9d4c60be", "source table head")
    require(e.read(0xFC100C, 4).hex() != "00000000", "source final float")

    # 0x2ee3ca0 is zero-filled BSS, not file-backed: slot 505 copies 24 zero bytes.
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    _, headers = d.program_headers(elf)
    covering = [p for p in headers
                if p["type"] == d.PT_LOAD and p["vaddr"] <= 0x2EE3CA0 < p["vaddr"] + p["memsz"]]
    require(bool(covering), "seed cell in a loadable segment")
    require(all(not (p["vaddr"] <= 0x2EE3CA0 and 0x2EE3CA0 + 0x18 <= p["vaddr"] + p["filesz"])
                for p in covering),
            "seed cell 0x2ee3ca0 must be zero-filled BSS")


SPEC = Spec(
    name="slot_505",
    index=505,
    base=0x236030,
    size=0x5C4,
    sha="b2d267ef8390855bbfb668cd8a8e5d7cacf60dae9e7e81d4f7c786443c9d8033",
    pins=PINS,
    callees=CALLEES,
    elf_extra=elf_extra,
    pad_byte=0x90,
    native_files={
        "header": "native/core/ctor_slot_505_236030.hpp",
        "helper_header": "native/core/bounds_minmax_1621b0.hpp",
        "source": "native/core/ctor_slot_505_236030.cpp",
        "helper_source": "native/core/bounds_minmax_1621b0.cpp",
        "test": "native/tests/test_ctor_slot_505.cpp",
        "cmake": "native/CMakeLists.txt",
    },
    header_constants={
        "kSlot505Rva": 0x236030,
        "kSlot505Size": 0x5C4,
        "kSlot505TableWordRva": 0x2DD6118,
        "kSlot505SourceRva": 0xFC0580,
        "kSlot505SourceCopySize": 0xA90,
        "kSlot505TableRva": 0x2EE9820,
        "kSlot505TableSize": 0xA98,
        "kSlot505PointCount": 226,
        "kSlot505PointStride": 0xC,
        "kSlot505TailOffset": 0xA90,
        "kSlot505TailQword": 0x3F5415C8BE50F81A,
        "kSlot505BoundsRva": 0x2EEA2B8,
        "kSlot505ObjectSize": 0xAB0,
        "kSlot505LoopStep": 0xC,
        "kSlot505LoopLimit": 0xA98,
        "kSlot505SeedCellRva": 0x2EE3CA0,
        "kSlot505MinMaxRva": 0x1621B0,
        "kSlot505SeedHelperRva": 0x162970,
        "kSlot505RetRva": 0x2365F3,
        "kSlot505PadByte": 0x90,
        "kBoundsMinMax1621b0Rva": 0x1621B0,
        "kBoundsMinMax1621b0Size": 87,
        "kBoundsSeed162970Rva": 0x162970,
        "kBoundsSeed162970Size": 8,
        "kBoundsSeedCellRva": 0x2EE3CA0,
        "kBoundsRecordSize": 0x18,
    },
    header_tokens=["0x2ee3ca0", "0x3f5415c8be50f81a", "226", "VMINSS", "VMAXSS"],
    source_order=["std::memcpy", "kSlot505SourceCopySize", "kSlot505TailQword",
                  "bounds_seed_162970", "kSlot505PointCount", "bounds_minmax_1621b0"],
    test_tokens=["null source", "NaN", "signed zero", "kSlot505TailQword",
                 "bounds_minmax_1621b0", "all_sentinel"],
    cmake_tokens=["core/bounds_minmax_1621b0.cpp", "core/ctor_slot_505_236030.cpp",
                  "add_test(NAME ctor_slot_505 COMMAND test_ctor_slot_505)"],
    summary="2704-byte source copy + qword tail + BSS seed + 226 min/max calls; no atexit/menu/strings",
)

if __name__ == "__main__":
    raise SystemExit(main(SPEC))
