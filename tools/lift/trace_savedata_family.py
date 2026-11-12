#!/usr/bin/env python3
"""Reproducible traces for the three 0xa54920 neighbours.

Reads exact FDE-bounded ranges from the pinned ELF, disassembles with llvm-objdump, and reports
instruction/call/branch/store/RIP counts. No guest code is loaded or executed.
"""
from __future__ import annotations

import hashlib
import json
import re
import shutil
import struct
import subprocess
import sys
from collections import Counter
from pathlib import Path

PROJECT = Path(__file__).resolve().parents[2]
ELF = PROJECT / "local" / "re" / "eboot.elf"

BODIES = {
    0xA55050: {"size": 0x15B, "sha256": "062b16fd7c32ca8f25c760e91c7f90e1855cd0ada958128706fee4e140a7456e"},
    0xA57C10: {"size": 0x12F, "sha256": "6c03fcb0da5ce003b10b190f91c46e6c1e49e899c38d156920c3f64f9dfd029c"},
    0xA586A0: {"size": 0x16F, "sha256": "723d9b307cdcc377080ddb78944172777def53b65e3df917ceacbc7a015b791f"},
    0xA57D40: {"size": 0x4B1, "sha256": "4f54c1a69b06da222423c7f9ce724823be6698d3fa8acbb699e2ab611f1b3f2e"},
    0xA58490: {"size": 0x4C, "sha256": "7003816aa67ab301f6d65354bbc84182d442f44743ab3316123002631943c24b"},
}


def _segments(data: bytes) -> list[tuple[int, int, int]]:
    phoff = struct.unpack_from("<Q", data, 0x20)[0]
    phentsize = struct.unpack_from("<H", data, 0x36)[0]
    phnum = struct.unpack_from("<H", data, 0x38)[0]
    out = []
    for i in range(phnum):
        o = phoff + i * phentsize
        _typ, _flags, off, va, _pa, fsz, _msz, _al = struct.unpack_from("<IIQQQQQQ", data, o)
        if fsz:
            out.append((va, fsz, off))
    return out


def body_bytes(rva: int, size: int, elf: Path = ELF) -> bytes:
    data = elf.read_bytes()
    for va, fsz, off in _segments(data):
        if va <= rva and rva + size <= va + fsz:
            return data[off + rva - va : off + rva - va + size]
    raise SystemExit(f"body 0x{rva:x}..0x{rva + size:x} is not mapped in {elf}")


def _objdump(rva: int, size: int, elf: Path = ELF) -> list[tuple[int, str]]:
    tool = shutil.which("llvm-objdump") or "/opt/homebrew/opt/llvm/bin/llvm-objdump"
    proc = subprocess.run(
        [tool, "--disassemble", f"--start-address=0x{rva:x}", f"--stop-address=0x{rva + size:x}", str(elf)],
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode != 0:
        raise SystemExit(f"objdump failed: {proc.stderr.strip()}")
    ins = []
    for line in proc.stdout.splitlines():
        m = re.match(r"\s*([0-9a-f]+):\s+((?:[0-9a-f]{2} )+)\s*(.*)", line)
        if m:
            ins.append((int(m.group(1), 16), m.group(3).strip()))
    if not ins:
        raise SystemExit("objdump produced no instructions")
    return ins


def trace(rva: int, elf: Path = ELF) -> dict:
    meta = BODIES[rva]
    size = meta["size"]
    blob = body_bytes(rva, size, elf)
    ins = _objdump(rva, size, elf)
    direct: Counter[int] = Counter()
    indirect: list[dict] = []
    branches: Counter[str] = Counter()
    stores: list[dict] = []
    rips: list[dict] = []
    for addr, text in ins:
        m = re.search(r"\bcallq?\s+(0x[0-9a-f]+)", text)
        if m:
            direct[int(m.group(1), 16)] += 1
        elif re.search(r"\bcallq?\s+\*", text):
            indirect.append({"rva": addr, "text": text})
        elif re.match(r"j\w+", text):
            branches[text.split()[0]] += 1
        if re.match(r"(?:mov|vmov|movabs)", text):
            parts = text.split(None, 1)
            if len(parts) == 2:
                operands = parts[1].split(",")
                if len(operands) >= 2 and "(" in operands[-1]:
                    stores.append({"rva": addr, "text": text})
        m = re.search(r"#\s*(0x[0-9a-f]+)", text)
        if m and "(%rip)" in text:
            rips.append({"rva": addr, "target": int(m.group(1), 16), "text": text.split("#")[0].strip()})
    return {
        "rva": rva,
        "size": size,
        "sha256": hashlib.sha256(blob).hexdigest(),
        "instructions": len(ins),
        "direct_calls": sum(direct.values()),
        "direct_histogram": {f"0x{k:06x}": v for k, v in sorted(direct.items())},
        "indirect_calls": len(indirect),
        "indirect_sites": indirect,
        "branches": sum(branches.values()),
        "branch_histogram": dict(branches),
        "memory_stores": len(stores),
        "store_sites": stores,
        "rip_operands": len(rips),
        "rip_sites": rips,
    }


def main() -> int:
    out = {}
    for rva in BODIES:
        result = trace(rva)
        if result["sha256"] != BODIES[rva]["sha256"]:
            print(f"body sha mismatch at 0x{rva:x}: {result['sha256']}", file=sys.stderr)
            return 1
        out[f"0x{rva:x}"] = result
    print(json.dumps(out, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
