#!/usr/bin/env python3
"""ELF-tie registry sorted unique-insert helpers 0x52d530 / 0x52d660 / 0x52d7e0 / 0x52d920.

Private: needs local/re/eboot.elf. Pins body digests, strides (8 / 16), grow callees
0x17090 (new[]) / 0x170c0 (delete[]), and that native headers document unique-insert + double growth.
Does NOT claim hint-iterator fidelity or stack-canary behaviour.
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
BODIES = {
    0x52D530: (303, "942babf361feea472203c159ca76c341a62cdcad12703bb349948bbc22d48cc3"),
    0x52D660: (373, "55beefa6f41f0ca986fa6db3ee2f2e3b9af906d7a555560a2170ee97d05c4547"),
    0x52D7E0: (320, "ad6ad61b95b519e7d819855e7dac0b3d7ca8cfd07dea57c972635c3d4482ced6"),
    0x52D920: (387, "a248d63aec22b75764b5f25c6f76c4be8f04ef71254b8e7504e40491af81ad0c"),
}
NATIVE = ROOT / "native/core/registry_sorted_insert.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_targets(rva: int, body: bytes) -> list[int]:
    out = []
    i = 0
    while i + 5 <= len(body):
        if body[i] == 0xE8:
            rel = struct.unpack("<i", body[i + 1 : i + 5])[0]
            out.append(rva + i + 5 + rel)
            i += 5
            continue
        i += 1
    return out


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    bodies = {}
    for rva, (size, digest) in BODIES.items():
        body = img.read(rva, size)
        require(len(body) == size, f"{rva:#x} size")
        got = hashlib.sha256(body).hexdigest()
        require(got == digest, f"{rva:#x} sha changed: {got}")
        bodies[rva] = body

    require(0x52D660 in call_targets(0x52D530, bodies[0x52D530]), "0x52d530 must call 0x52d660")
    require(0x52D920 in call_targets(0x52D7E0, bodies[0x52D7E0]), "0x52d7e0 must call 0x52d920")
    require(0x17090 in call_targets(0x52D660, bodies[0x52D660]), "0x52d660 must call new[] 0x17090")
    require(0x170C0 in call_targets(0x52D660, bodies[0x52D660]), "0x52d660 must call delete[] 0x170c0")
    require(0x17090 in call_targets(0x52D920, bodies[0x52D920]), "0x52d920 must call new[] 0x17090")
    require(0x170C0 in call_targets(0x52D920, bodies[0x52D920]), "0x52d920 must call delete[] 0x170c0")

    require(
        b"\x48\xc1\xfb\x03" in bodies[0x52D530] or b"\x48\xc1\xfa\x03" in bodies[0x52D530],
        "table A missing sar $3 (8-byte stride)",
    )
    require(
        b"\x48\xc1\xfb\x04" in bodies[0x52D7E0] or b"\x48\xc1\xfa\x04" in bodies[0x52D7E0],
        "table B missing sar $4 (16-byte stride)",
    )

    native = NATIVE.read_text()
    require("registry_insert_unique_a" in native and "registry_insert_unique_b" in native, "API")
    require("0x52d530" in native and "0x52d7e0" in native, "RVA comments")
    require(re.search(r"sizeof\(RegistryEntryA\)\s*==\s*8", native), "entry A size")
    require(re.search(r"sizeof\(RegistryEntryB\)\s*==\s*16", native), "entry B size")
    require("hint" in native.lower(), "must document hint omission")

    print(
        "OK check_f52d530: insert/grow A+B pinned; "
        f"0x52d530={BODIES[0x52D530][1][:12]}… 0x52d7e0={BODIES[0x52D7E0][1][:12]}…"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
