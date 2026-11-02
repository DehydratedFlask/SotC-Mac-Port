#!/usr/bin/env python3
"""Pin the two Fios read slots and the 0x4b0 array teardown.

Verifies the bodies, the slot qwords, the type-8 relocations, the PLT targets the two reads call
(`sceFiosFHReadv` / `sceFiosFHPreadv`), the 0x50 element stride and the tag-2 free target.
Skips cleanly without the private inputs.
"""

from __future__ import annotations

import hashlib
import importlib.util
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF = ROOT / "local/re/eboot.elf"
PLT = ROOT / "local/validation/registry-analysis-agent/plt_map.json"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
FIOS_HEADER = ROOT / "native/core/vtable_15953f8_slot_a0_a8_fios_reads.hpp"
TEARDOWN_HEADER = ROOT / "native/core/vtable_1594dc0_slot10_4b0.hpp"
WAIT_HEADER = ROOT / "native/core/vtable_15953f8_slot18_31310.hpp"
CMAKE = ROOT / "native/CMakeLists.txt"

READV = (0x31870, 0x37, "413fe9fe5c6fd439b6a8b0cf4037203fbea678a8d4eb85805d323f1f795b7159")
PREADV = (0x318B0, 0x38, "492a600ef144610b01b6a8118f3cd33ae3a955a97959f8a2c38ee502fa16c830")
TEARDOWN = (0x4B0, 0x71, "a15bae38e545531a8c5769a079333bd4e0385846160e10a4b768827ecb0d8c8c")
WAIT_TEARDOWN = (0x31310, 0x69, "cabca85b66b8c3d789062c0bf99e3175d68105b9e7cbcc75c2495b4a2fc6ca49")
TEARDOWN_22A60 = (0x22A60, 0x79, "7862319966c5c08c943dccefd7d27aa28cf80abe0a83bbf08c1361577fade8fd")

VECTORS = (
    (0x15953F8, 0xA8, 0x318B0, 0x15954A0),
    (0x15953F8, 0xA0, 0x31870, 0x1595498),
    (0x15953F8, 0x18, 0x31310, 0x1595410),
    (0x1594DC0, 0x10, 0x4B0, 0x1594DD0),
    (0x1594DC0, 0x18, 0x22A60, 0x1594DD8),
)


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    fios = FIOS_HEADER.read_text()
    teardown = TEARDOWN_HEADER.read_text()
    wait_header = WAIT_HEADER.read_text()
    cmake = CMAKE.read_text()

    for needle in ("kVtable15953f8SlotA0_31870Rva = 0x31870", "kVtable15953f8SlotA8_318b0Rva = 0x318b0",
                   "kFiosReadvPlt = 0xf8f6f0", "kFiosPreadvPlt = 0xf8f6c0",
                   "kVtable15953f8Marker = 0xFFFFFFFF00000004ULL"):
        require(needle in fios, f"fios header missing {needle}")
    for needle in ("kVtable1594dc0Slot10_4b0Rva = 0x4b0",
                   "kVtable1594dc0Slot10ElementStride = 0x50",
                   "kVtable1594dc0Slot10InstalledTable = 0x1594dd0",
                   "kVtable1594dc0Slot18_22a60Rva = 0x22a60",
                   "kVtable1594dc0Slot18Reloc = 0x1594dd8",
                   "kSharedReleaseF590Rva = 0xf590"):
        require(needle in teardown, f"teardown header missing {needle}")
    for needle in ("kVtable15953f8Slot18_31310Rva = 0x31310",
                   "kVtable15953f8Slot18Reloc = 0x1595410",
                   "kVtable15953f8Slot18InstalledTable = 0x1595408",
                   "kVtable15953f8Slot18WaitPlt = 0xf8f7d0",
                   "kVtable15953f8Slot18FinishRva = 0x4a4a0"):
        require(needle in wait_header, f"wait header missing {needle}")
    for needle in ("core/vtable_15953f8_slot_a0_a8_fios_reads.cpp",
                   "core/vtable_1594dc0_slot10_4b0.cpp",
                   "core/vtable_15953f8_slot18_31310.cpp",
                   "test_vtable_15953f8_fios_and_4b0"):
        require(needle in cmake, f"cmake missing {needle}")

    if not (ELF.is_file() and PLT.is_file() and FDE.is_file()):
        print("SKIP check_fios_slots_and_4b0: private inputs not present")
        return 0

    elf = ELF.read_bytes()
    _, program_headers = d.program_headers(elf)
    image = d.Image(elf, program_headers)
    relocs = d.parse(elf)["relocs"]
    plt = json.loads(PLT.read_text())

    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    fdes, _duplicates = census.load_fde_ranges(FDE)

    for rva, size, sha in (READV, PREADV, TEARDOWN, WAIT_TEARDOWN, TEARDOWN_22A60):
        body = image.read(rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"0x{rva:x} body changed")
        found = fdes.find(rva)
        require(found is not None and found[0] == rva and found[1] == size,
                f"0x{rva:x} FDE range changed")

    readv = image.read(*READV[:2])
    preadv = image.read(*PREADV[:2])
    teardown_body = image.read(*TEARDOWN[:2])
    wait_body = image.read(*WAIT_TEARDOWN[:2])

    # The two reads call the Fios PLT stubs; the stubs name the imported services.
    require(readv and preadv, "read bodies were not read")
    require(plt["0xf8f6f0"][0] == "sceFiosFHReadv", "PLT 0xf8f6f0 is not sceFiosFHReadv")
    require(plt["0xf8f6c0"][0] == "sceFiosFHPreadv", "PLT 0xf8f6c0 is not sceFiosFHPreadv")
    require(plt["0xf8f6f0"][2] == "libSceFios2" and plt["0xf8f6c0"][2] == "libSceFios2",
            "fios PLT library changed")
    require(plt["0xf8f7d0"][0] == "sceFiosOpWait", "PLT 0xf8f7d0 is not sceFiosOpWait")
    require(plt["0xf8f7d0"][2] == "libSceFios2", "the wait PLT library changed")

    # The wait teardown installs its base table at +0x10, waits on the +0x50 handle, reads through
    # slots 0xa8/0xa0, gates slot 0x18 on the +0x8 dword, then frees `this`.
    require(bytes.fromhex("488d05") in wait_body, "wait teardown lost the vtable lea")
    require(bytes.fromhex("4883c010") in wait_body or bytes.fromhex("488d4010") in wait_body,
            "wait teardown lost the +0x10 base-table advance")
    require(bytes.fromhex("ff90a8000000") in wait_body, "wait teardown lost the 0xa8 positioned read")
    require(bytes.fromhex("ff90a0000000") in wait_body, "wait teardown lost the 0xa0 plain read")
    require(bytes.fromhex("837b0801") in wait_body, "wait teardown lost the +0x8 == 1 mode compare")
    require(bytes.fromhex("e9") in wait_body[-8:], "wait teardown no longer ends in a tail jmp")
    finish = image.read(0x4A4A0, 1)
    require(finish == b"\xc3", "0x4a4a0 is no longer a 1-byte retq")

    # Teardown structure: vtable lea + 0x10, the 0x50 stride, the two 0xf590 calls and the 0x170c0 free.
    require(bytes.fromhex("488d05") in teardown_body, "teardown lost the vtable lea")
    require(bytes.fromhex("4883c350") in teardown_body or bytes.fromhex("4881c350000000") in teardown_body,
            "teardown lost the 0x50 element advance")
    require(teardown_body.count(bytes.fromhex("e8")) >= 3, "teardown lost a call")
    require(bytes.fromhex("e9") in teardown_body[-8:], "teardown no longer ends in a tail jmp")

    for vtable, slot, target, reloc in VECTORS:
        qword = struct.unpack("<Q", image.read(vtable + slot, 8))[0]
        require(qword == target, f"slot 0x{vtable + slot:x} does not point at 0x{target:x}")
        rows = [row for row in relocs if row["target"] == reloc]
        require(rows, f"no relocation at 0x{reloc:x}")
        require(any(row["type"] == 8 and row["addend"] == target for row in rows),
                f"relocation at 0x{reloc:x} lost the 0x{target:x} addend")

    print("OK check_fios_slots_and_4b0: Fios read bodies and the sceFiosOpWait loop (PLT names verified), "
          "the 0x4b0/0x22a60 teardown structures, five slot qwords and their relocations all agree")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
