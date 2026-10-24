#!/usr/bin/env python3
"""ELF-tie the numeric deepen of ctor 0x551140 (275-entry input table).

Private: needs local/re/eboot.elf. Pins:
  - ELF / store-region digests (0x55141d..0x557fe4)
  - ctor body size + sha at 0x551140
  - accessors 0x54dc10 (lea → 0x2eff8e0) and 0x54dc20 (mov eax, 0x113)
  - committed numeric payload sha + byte-identity vs objdump-decoded immediates
  - native headers carry matching count/stride/null-callback policy

Does NOT claim name strings, callbacks, pre-objects, Init 0x531d60, or G1.
"""

from __future__ import annotations

import hashlib
import re
import struct
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR = 0x551140
CTOR_SIZE = 28370
CTOR_SHA256 = "a77e438645f9749e179d25b91d21639f0907e65b70af2594e62814a3c13e2533"
REGION_START = 0x55141D
REGION_END = 0x557FE4
REGION_SHA256 = "5ecf2186b5a7f35b384a8ec65b69ea2304ace415a19de9b1429212f78e9a3068"
TABLE_BASE = 0x2EFF8E0
COUNT = 275
STRIDE = 0x60
PAYLOAD_SHA256 = "61c85093797bb62b47f6299c320a3d05d7cf17577434f25360b5ba2c298b9dcb"
ACCESSOR_BASE = 0x54DC10
ACCESSOR_COUNT = 0x54DC20
NUMERIC_HDR = ROOT / "native/core/input_table_551140_numeric.inc.hpp"
NATIVE_HDR = ROOT / "native/core/input_table_551140.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def parse_imm(tok: str) -> int:
    s = tok[1:]
    if s.startswith("-0x"):
        return -int(s[3:], 16)
    if s.startswith("0x"):
        return int(s, 16)
    return int(s, 16)


def decode_numeric_rows(elf_path: Path) -> list[tuple[int, int, int, int]]:
    out = subprocess.check_output(
        [
            "objdump",
            "-d",
            f"--start-address={REGION_START}",
            f"--stop-address={REGION_END}",
            str(elf_path),
        ],
        text=True,
        stderr=subprocess.STDOUT,
    )
    gpr: dict[str, int] = {}
    imm: dict[int, dict[int, int]] = defaultdict(dict)
    line_re = re.compile(
        r"\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+[0-9a-f]{2})\s+(\S+)\s*(.*)$"
    )
    for line in out.splitlines():
        m = line_re.match(line)
        if not m:
            continue
        addr = int(m.group(1), 16)
        nbytes = len(m.group(2).split())
        mn = m.group(3)
        rest = m.group(4).split("#")[0].strip()
        ops = [o.strip() for o in rest.split(",")] if rest else []
        if mn == "leaq" and len(ops) == 2:
            mo = re.match(r"(-?0x[0-9a-f]+)\(%rip\)", ops[0])
            if mo:
                disp = (
                    int(mo.group(1), 16)
                    if not mo.group(1).startswith("-")
                    else -int(mo.group(1)[1:], 16)
                )
                gpr[ops[1].lstrip("%")] = addr + nbytes + disp
            continue
        if mn not in ("movl", "movq") or len(ops) != 2:
            continue
        if not ops[0].startswith("$") or not ops[1].endswith("(%rax)"):
            continue
        if gpr.get("rax") != TABLE_BASE:
            continue
        off_s = ops[1].split("(")[0]
        off = int(off_s, 16) if off_s else 0
        sz = 4 if mn == "movl" else 8
        val = parse_imm(ops[0]) & ((1 << (8 * sz)) - 1)
        entry, eo = divmod(off, STRIDE)
        if eo in (0x10, 0x14, 0x18, 0x50):
            imm[entry][eo] = val
    rows: list[tuple[int, int, int, int]] = []
    for idx in range(COUNT):
        cell = imm.get(idx, {})
        require(
            0x10 in cell and 0x14 in cell and 0x18 in cell,
            f"missing a/b/c at entry {idx}",
        )
        rows.append((cell[0x10], cell[0x14], cell[0x18], cell.get(0x50, 0)))
    return rows


def parse_committed_rows(text: str) -> list[tuple[int, int, int, int]]:
    rows: list[tuple[int, int, int, int]] = []
    for m in re.finditer(
        r"\{\s*(\d+)u,\s*(\d+)u,\s*(\d+)u,\s*(\d+)ull\s*\}", text
    ):
        rows.append((int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4))))
    return rows


def main() -> int:
    elf_path = ROOT / "local/re/eboot.elf"
    require(elf_path.is_file(), f"missing private ELF {elf_path}")
    elf = elf_path.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    ctor = img.read(CTOR, CTOR_SIZE)
    require(len(ctor) == CTOR_SIZE, "ctor size")
    require(hashlib.sha256(ctor).hexdigest() == CTOR_SHA256, "ctor sha256 changed")

    region = img.read(REGION_START, REGION_END - REGION_START)
    require(len(region) == REGION_END - REGION_START, "region length")
    require(hashlib.sha256(region).hexdigest() == REGION_SHA256, "region sha256 changed")

    base_body = img.read(ACCESSOR_BASE, 8)
    require(base_body[0:3] == bytes.fromhex("488d05"), "0x54dc10 not lea rax,[rip+d]")
    require(base_body[7] == 0xC3, "0x54dc10 missing ret")
    disp = struct.unpack("<i", base_body[3:7])[0]
    require(ACCESSOR_BASE + 7 + disp == TABLE_BASE, "0x54dc10 does not target 0x2eff8e0")

    count_body = img.read(ACCESSOR_COUNT, 6)
    require(count_body[0] == 0xB8, "0x54dc20 not mov eax,imm32")
    require(struct.unpack("<I", count_body[1:5])[0] == COUNT, "0x54dc20 imm != 275")
    require(count_body[5] == 0xC3, "0x54dc20 missing ret")

    decoded = decode_numeric_rows(elf_path)
    blob = b"".join(struct.pack("<IIIQ", *r) for r in decoded)
    require(hashlib.sha256(blob).hexdigest() == PAYLOAD_SHA256, "decoded payload sha changed")

    require(NUMERIC_HDR.is_file(), f"missing {NUMERIC_HDR}")
    numeric_text = NUMERIC_HDR.read_text()
    require(PAYLOAD_SHA256 in numeric_text, "numeric header missing payload sha")
    committed = parse_committed_rows(numeric_text)
    require(len(committed) == COUNT, f"numeric header has {len(committed)} rows, want {COUNT}")
    require(committed == decoded, "committed numeric rows != ELF-decoded immediates")

    native = NATIVE_HDR.read_text()
    require("kInputTable551140Count" in native, "native header missing count")
    require("0x113" in native, "native header missing accessor count imm")
    require("null here" in native.lower() or "null" in native, "native header should document null callbacks")
    require("init_input_table_551140" in native, "native header missing init")
    require("recovered_constructors" in native and "Not registered" in native,
            "must stay out of recovered_constructors (cliff)")

    print(
        "OK check_f551140: region+ctor+accessors pinned; "
        f"275 numeric rows match {PAYLOAD_SHA256[:12]}…"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
