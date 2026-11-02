#!/usr/bin/env python3
"""Resolve STRAGGLERS-01's third callsite, `0xa4bddb`, from the pinned bytes.

The 2026-10-20 row closed 2 of 3 callsites and left this one open because a REA/Ghidra
`function 0xa4bddb` query exited 1 with *"No procedure exists at the requested address:
0xa4bddb"*. That message is literal, and this checker shows why it is not a failure of the bytes:

* `0xa4bddb` is **not a function entry**. It is the 5-byte `call` at offset `+0x124b` inside
  `0xa4ab90`, whose `.eh_frame` FDE is `0xa4ab90 + 0x218c` and is the only FDE covering the
  address. No other FDE starts anywhere in `[0xa4ab90, 0xa4bddb)`, and no `55 48 89 e5` prologue
  (and no `f3 0f 1e fa`) appears in that interval either.
* The call targets the import stub `0xf90f90` (`jmp *0x1600db8`), whose `R_X86_64_JUMP_SLOT` row
  resolves to libSceSaveData `sceSaveDataDelete`, NID `S1GkePI17zQ` — the name the row already had.
* Ghidra did not report "a gap between two functions"; it never disassembled the region at all.
  `FUN_00a4ab90`'s body is 20 ranges totalling 6,125 B with holes, and the hole carrying this
  callsite (`0xa4bd94..0xa4be81`) is bracketed by the function's own ranges (`..0xa4bd93`,
  `0xa4be82..`). Its tail jumps back into the function (`0xa4be7d -> 0xa4af93`), and its head is a
  real `mov eax,[rip+disp]`. So the address had no procedure, no instruction and no data for the
  provider to return — the puzzle is a coverage artefact, and the callsite belongs to `0xa4ab90`.

Read-only and offline: the image is read through `ps4_dynlink.Image`; nothing is loaded, relocated
or executed. Skips cleanly (exit 0) when the private image is absent.
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
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
IMPORTS = ROOT / "local/re/imports.json"
MANIFEST_SHA256 = "78caaaae139b7c5d4d2618abab2e50e9f6918ea3cec73a90b81e0005c83b6744"
# Ghidra-side evidence is private; the checker degrades to the byte checks when it is missing.
CONTAINING_LOG = ROOT / "local/validation/stragglers-a4bddb-e1063/probe-containing-a4bddb.log"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

CALL_SITE = 0xA4BDDB
CONTAINER = 0xA4AB90
CONTAINER_SIZE = 0x218C
CONTAINER_SHA256 = "8ceb027587883b77887c8b0cf793d08a35d5e9ce8cdb583e191578bf755d1d27"
NEXT_FUNCTION = 0xA4CD20

STUB = 0xF90F90
STUB_BYTES = "ff2522fe6600"          # jmp qword ptr [rip + 0x66fe22]
STUB_GOT = 0x1600DB8
STUB_SYMBOL_INDEX = 0x10D
IMPORT_NAME = "sceSaveDataDelete"
IMPORT_NID = "S1GkePI17zQ"
IMPORT_LIBRARY = "libSceSaveData"

# All three `call 0xf90f90` sites in the image. Two sit inside 0xa4ab90, one inside 0xa48280.
STUB_CALLERS = (0xA48A6E, 0xA4BADB, 0xA4BDDB)

# Ghidra's holes in FUN_00a4ab90 that bracket the callsite.
HOLE_FIRST = 0xA4BD94
HOLE_LAST = 0xA4BE81                 # inclusive
HOLE_TAIL_JUMP = 0xA4BE7D
HOLE_TAIL_TARGET = 0xA4AF93
HOLE_HEAD_BYTES = "8b0596c28a02"     # mov eax, dword ptr [rip + 0x28ac296] -> 0x32f8030
HOLE_HEAD_TARGET = 0x32F8030


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def load_fde_ranges():
    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    ranges, _duplicates = census.load_fde_ranges(FDE)
    return ranges


def rel32(image, site: int) -> int:
    return site + 5 + struct.unpack("<i", image.read(site + 1, 4))[0]


def check_ghidra_log() -> str:
    """The provider's own answer, when it was saved: the address has no procedure/instruction."""
    if not CONTAINING_LOG.is_file():
        return "ghidra log absent"
    text = CONTAINING_LOG.read_text(errors="replace")
    require("CONTAIN function_count=38845" in text, "probe ran against a different program")
    require("CONTAIN 0xa4bddb is_entry=no" in text, "0xa4bddb unexpectedly has a function entry")
    require("CONTAIN 0xa4bddb is_instruction=no" in text,
            "Ghidra disassembled 0xa4bddb; the coverage-artefact reading would need revisiting")
    require("CONTAIN 0xa4bddb containing_any=none" in text,
            "0xa4bddb is now inside a Ghidra function body")
    require("CONTAIN 0xa4ab90 containing_size=6125" in text, "FUN_00a4ab90 body size changed")
    require("CONTAIN 0xa4ab90 containing_blocks=20" in text, "FUN_00a4ab90 range count changed")
    require("CONTAIN 0xa4ab90 range_12=0xa4bd60..0xa4bd93" in text, "range_12 no longer ends at the hole")
    require("CONTAIN 0xa4ab90 range_13=0xa4be82..0xa4bfe6" in text, "range_13 no longer starts at the hole")
    require("CONTAIN 0xa4af93 in_body=yes" in text, "hole tail-jump target left FUN_00a4ab90's body")
    return "ghidra log checked"


def main() -> int:
    if not (ELF.is_file() and FDE.is_file()):
        print("SKIP check_stragglers_a4bddb: private image/FDE table not present")
        return 0

    raw = ELF.read_bytes()
    require(hashlib.sha256(raw).hexdigest() == ELF_SHA256, "pinned ELF digest changed")
    _, program_headers = d.program_headers(raw)
    image = d.Image(raw, program_headers)

    # ---- 1. the address is not an entry; it is inside one FDE, and only one ------------------
    ranges = load_fde_ranges()
    found = ranges.find(CALL_SITE)
    require(found is not None, "0xa4bddb is in no FDE range")
    require(found[0] == CONTAINER, f"containing FDE starts at {found[0]:#x}, pinned {CONTAINER:#x}")
    require(found[1] == CONTAINER_SIZE, f"containing FDE length {found[1]:#x}, pinned {CONTAINER_SIZE:#x}")
    require(CALL_SITE - CONTAINER == 0x124B, "callsite offset inside the function changed")
    starts = [start for start, _ in ranges.items if CONTAINER <= start < CALL_SITE]
    require(starts == [CONTAINER], f"an FDE starts between the function and the callsite: {starts}")
    following = [start for start, _ in ranges.items if start >= CONTAINER + CONTAINER_SIZE]
    require(following and following[0] == NEXT_FUNCTION,
            f"next function after 0xa4ab90 is {following[0]:#x} if following else None")
    body = image.read(CONTAINER, CONTAINER_SIZE)
    require(hashlib.sha256(body).hexdigest() == CONTAINER_SHA256, "0xa4ab90 body digest changed")

    # No competing entry form: one prologue in the whole prefix, at the function start only.
    prologues = [CONTAINER + i for i in range(CALL_SITE - CONTAINER - 3)
                 if image.read(CONTAINER + i, 4) == b"\x55\x48\x89\xe5"]
    require(prologues == [CONTAINER], f"competing frame prologues before the callsite: {prologues}")
    endbr = [CONTAINER + i for i in range(CALL_SITE - CONTAINER - 3)
             if image.read(CONTAINER + i, 4) == b"\xf3\x0f\x1e\xfa"]
    require(endbr == [], f"endbr64 entry markers before the callsite: {endbr}")

    # ---- 2. the callsite itself ----------------------------------------------------------------
    e = image.read(CALL_SITE, 5)
    require(e[0] == 0xE8, "0xa4bddb is not a relative call")
    require(rel32(image, CALL_SITE) == STUB, f"call target is {rel32(image, CALL_SITE):#x}")

    # ---- 3. the stub resolves to libSceSaveData sceSaveDataDelete -------------------------------
    require(image.read(STUB, 6).hex() == STUB_BYTES, "0xf90f90 stub bytes changed")
    got = STUB + 6 + struct.unpack("<i", image.read(STUB + 2, 4))[0]
    require(got == STUB_GOT, f"stub GOT slot is {got:#x}, pinned {STUB_GOT:#x}")
    relocs = {r["target"]: r for r in d.parse(raw)["relocs"]}
    row = relocs.get(got)
    require(row is not None, "no relocation for the stub's GOT slot")
    require(row["table"] == "JMPREL", f"slot relocation table is {row['table']}")
    require(int(row["type"]) == 7, f"slot relocation type is {row['type']} (want R_X86_64_JUMP_SLOT)")
    require(int(row["symbol"]) == STUB_SYMBOL_INDEX, "slot symbol index changed")

    if IMPORTS.is_file():
        require(hashlib.sha256(IMPORTS.read_bytes()).hexdigest() == MANIFEST_SHA256,
                "import manifest digest changed")
        manifest = json.loads(IMPORTS.read_text())
        rows = [i for i in manifest["imports"] if i.get("first_site") == hex(STUB_GOT)]
        require(len(rows) == 1, f"manifest rows for {STUB_GOT:#x}: {len(rows)}")
        entry = rows[0]
        require(entry["name"] == IMPORT_NAME, f"import name {entry['name']!r}")
        require(entry["nid"] == IMPORT_NID, f"import NID {entry['nid']!r}")
        require(entry["library"] == IMPORT_LIBRARY, f"import library {entry['library']!r}")

    # Every `call 0xf90f90` in the loadable image, so the row can say how many Delete sites exist.
    text = image.read(0, 0x1593978)
    callers: list[int] = []
    index = text.find(b"\xe8")
    while index != -1:
        if index + 5 <= len(text):
            if index + 5 + struct.unpack("<i", text[index + 1:index + 5])[0] == STUB:
                callers.append(index)
        index = text.find(b"\xe8", index + 1)
    require(tuple(callers) == STUB_CALLERS,
            f"call sites to the Delete stub changed: {[hex(x) for x in callers]}")

    # ---- 4. what the callsite does -------------------------------------------------------------
    # A 0x20-byte request block is zeroed at [rsp+0x700], seeded with one global dword, given a
    # null qword and a pointer to [rsp+0x6e0], and passed in rdi.
    setup = image.read(HOLE_FIRST, CALL_SITE - HOLE_FIRST)
    require(setup.hex().startswith(HOLE_HEAD_BYTES), "request block setup no longer starts with the global load")
    head_target = HOLE_FIRST + 6 + struct.unpack("<i", image.read(HOLE_FIRST + 2, 4))[0]
    require(head_target == HOLE_HEAD_TARGET, f"request block seed global is {head_target:#x}")
    for needle, why in (
        ("c5fc57c0", "ymm zero"),
        ("488dbc24" + "00070000", "lea rdi,[rsp+0x700]"),
        ("c5fc118424" + "00070000", "zero [rsp+0x700] (ymm)"),
        ("898424" + "00070000", "store the seed dword at [rsp+0x700]"),
        ("488d8424" + "e0060000", "lea rax,[rsp+0x6e0]"),
        ("48c78424" + "08070000" + "00000000", "zero [rsp+0x708]"),
        ("48898424" + "10070000", "store the [rsp+0x6e0] pointer at [rsp+0x710]"),
    ):
        require(needle in setup.hex(), f"request block setup missing {why}")

    # The instruction after the call overwrites rax, so this site does not test the return code.
    after = image.read(CALL_SITE + 5, 10)
    require(after.hex().startswith("488b442450"), "instruction after the callsite changed")
    require(after[0:2] != b"\x85\xc0" and after[0:2] != b"\x83\xf8", "callsite now tests eax")

    # ---- 5. the Ghidra hole is code, not padding, and it belongs to this function --------------
    require(image.read(HOLE_FIRST, 6).hex() == HOLE_HEAD_BYTES, "hole head bytes changed")
    tail = image.read(HOLE_TAIL_JUMP, 5)
    require(tail[0] == 0xE9, "hole tail is no longer a relative jump")
    require(rel32(image, HOLE_TAIL_JUMP) == HOLE_TAIL_TARGET,
            f"hole tail jump targets {rel32(image, HOLE_TAIL_JUMP):#x}")
    require(CONTAINER <= HOLE_TAIL_TARGET < CONTAINER + CONTAINER_SIZE,
            "hole tail jump leaves the containing function")
    require(HOLE_FIRST <= CALL_SITE <= HOLE_LAST, "callsite is no longer inside the Ghidra hole")

    # ---- 6. the only direct caller of 0xa4ab90 (static, one site) ------------------------------
    callers_container: list[int] = []
    index = text.find(b"\xe8")
    while index != -1:
        if index + 5 <= len(text):
            if index + 5 + struct.unpack("<i", text[index + 1:index + 5])[0] == CONTAINER:
                callers_container.append(index)
        index = text.find(b"\xe8", index + 1)
    require(callers_container == [0xA4D3E3],
            f"direct callers of 0xa4ab90 changed: {[hex(x) for x in callers_container]}")
    caller_fde = ranges.find(0xA4D3E3)
    require(caller_fde is not None and caller_fde[0] == 0xA4D330,
            "the caller of 0xa4ab90 no longer sits in 0xa4d330")

    # ---- 7. the other two straggler callsites stay inside their recorded functions -------------
    for site, start in ((0x954726, 0x954680), (0xA4A903, 0xA4A600)):
        hit = ranges.find(site)
        require(hit is not None and hit[0] == start,
                f"{site:#x} is no longer inside the function starting {start:#x}")

    provider = check_ghidra_log()
    print("OK check_stragglers_a4bddb: 0xa4bddb is a call inside 0xa4ab90 (FDE +0x124b, "
          f"not an entry), -> 0xf90f90 -> sceSaveDataDelete {IMPORT_NID}, 3 Delete sites, "
          f"return code unused, Ghidra hole {HOLE_FIRST:#x}..{HOLE_LAST:#x} is code "
          f"({provider})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
