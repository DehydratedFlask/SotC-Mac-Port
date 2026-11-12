#!/usr/bin/env python3
"""Reproducible instruction/call/store trace for pinned RVA 0xa54920.

Reads the body bytes from the pinned ELF (program headers, including the nonstandard RELRO
segment), disassembles with llvm-objdump, and prints the measured shape as JSON. The oracle
`check_fsavedata_stats_a54920.py` imports `trace()` and compares its own result with the pins.

No guest code is loaded, relocated or executed.
"""
from __future__ import annotations

import hashlib
import json
import re
import shutil
import struct
import subprocess
import sys
import tempfile
from collections import Counter
from pathlib import Path

RVA = 0xA54920
SIZE = 0x730
SHA256 = "27c76ceb08e4aac8d9e67b1dfcdeb87c8833fa8e73ae4f56de2481c475167810"

PROJECT = Path(__file__).resolve().parents[2]
ELF = PROJECT / "local" / "re" / "eboot.elf"

EXPECTED_DIRECT = {
    0xF590: 12,
    0x10A90: 6,
    0x17070: 5,
    0x17090: 1,
    0x170B0: 2,
    0x20D60: 3,
    0x26EA0: 3,
    0x272D0: 3,
    0x3A980: 3,
    0xA55050: 1,
    0xA57C10: 1,
    0xA586A0: 1,
    0xF8F3A0: 1,
}


def _segments(data: bytes) -> list[tuple[int, int, int]]:
    """(vaddr, filesz, fileoff) for every mapped program header, whatever its p_type."""
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


def body_bytes(elf: Path = ELF) -> bytes:
    data = elf.read_bytes()
    for va, fsz, off in _segments(data):
        if va <= RVA and RVA + SIZE <= va + fsz:
            return data[off + RVA - va : off + RVA - va + SIZE]
    raise SystemExit(f"body 0x{RVA:x}..0x{RVA + SIZE:x} is not mapped in {elf}")


def _objdump(elf: Path) -> list[tuple[int, str]]:
    """Disassemble the body in place so RIP-relative comments keep their real targets."""
    tool = shutil.which("llvm-objdump") or "/opt/homebrew/opt/llvm/bin/llvm-objdump"
    if not Path(tool).exists():
        tool = shutil.which("objdump") or tool
    proc = subprocess.run(
        [tool, "--disassemble", f"--start-address=0x{RVA:x}", f"--stop-address=0x{RVA + SIZE:x}", str(elf)],
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


def trace(elf: Path = ELF) -> dict:
    blob = body_bytes(elf)
    digest = hashlib.sha256(blob).hexdigest()
    ins = _objdump(elf)
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
        "rva": RVA,
        "size": SIZE,
        "sha256": digest,
        "instructions": len(ins),
        "direct_calls": len([1 for _a, t in ins if re.search(r"\bcallq?\s+0x", t)]),
        "direct_histogram": {f"0x{k:06x}": v for k, v in sorted(direct.items())},
        "indirect_calls": len(indirect),
        "indirect_sites": indirect,
        "branches": sum(branches.values()),
        "branch_histogram": dict(branches),
        "memory_stores": len(stores),
        "store_sites": stores,
        "rip_operands": len(rips),
        "rip_sites": rips,
        "expected_direct_histogram": EXPECTED_DIRECT,
    }


def main() -> int:
    result = trace()
    if result["sha256"] != SHA256:
        print(f"body sha mismatch: {result['sha256']}", file=sys.stderr)
        return 1
    print(json.dumps(result, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
