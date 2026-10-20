#!/usr/bin/env python3
"""Tie the native 0xce0890 vector-copy fixture to the private ELF."""

import hashlib
import json
import struct
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d


ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
CTOR_INDEX = 155
RVA = 0xCE0890
SOURCE_RVA = 0x12F7C00
DESTINATION_RVA = 0x3351C90
EXPECTED_BITS = (0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0)


def require(condition, message):
    if not condition:
        raise SystemExit(f"FAIL {message}")


elf = (ROOT / "local/re/eboot.elf").read_bytes()
digest = hashlib.sha256(elf).hexdigest()
require(digest == ELF_SHA256, f"unexpected ELF SHA256 {digest}")

_, headers = d.program_headers(elf)
image = d.Image(elf, headers)
body = image.read(RVA, 17)

# Compare all opcode/ModRM bytes and independently decode both RIP-relative displacements.
template = bytearray.fromhex("c5 f8 10 05 00 00 00 00 c5 f8 11 05 00 00 00 00 c3")
masked = bytearray(body)
masked[4:8] = b"\0" * 4
masked[12:16] = b"\0" * 4
require(bytes(masked) == bytes(template), f"unexpected constructor encoding {body.hex()}")

source = RVA + 8 + struct.unpack_from("<i", body, 4)[0]
destination = RVA + 16 + struct.unpack_from("<i", body, 12)[0]
require(source == SOURCE_RVA, f"source target is {source:#x}, expected {SOURCE_RVA:#x}")
require(destination == DESTINATION_RVA,
        f"destination target is {destination:#x}, expected {DESTINATION_RVA:#x}")
require(image.mapped(source, 16), f"source {source:#x} is not mapped")
require(image.mapped(destination, 16), f"destination {destination:#x} is not mapped")

source_bytes = image.read(source, 16)
bits = struct.unpack("<4I", source_bytes)
require(bits == EXPECTED_BITS, f"source bits are {[hex(x) for x in bits]}")

survey = json.loads((ROOT / "local/re/work/ctor-survey.json").read_text())
row = survey[CTOR_INDEX]
require(row.get("ctor") == hex(RVA) and row.get("bytes") == 17,
        f"survey position {CTOR_INDEX} does not identify the 17-byte ctor")
require(row.get("calls") == [] and row.get("indirect_calls") == 0,
        "survey reports a call or indirect call in this candidate")

exe = ROOT / "local/build/native/test_vec_ce0890"
native = json.loads(subprocess.run(
    [str(exe), "--dump"], check=True, capture_output=True, text=True).stdout)
require(native.get("bits") == list(bits), f"native bits differ: {native.get('bits')}")
require(native.get("source") == source, f"native source differs: {native.get('source')}")
require(native.get("destination") == destination,
        f"native destination differs: {native.get('destination')}")
require(native.get("rva") == RVA and native.get("index") == CTOR_INDEX,
        "native report has the wrong constructor identity")

private_dump = ROOT / "local/re/work/cce0890.bin"
if private_dump.is_file():
    require(private_dump.read_bytes() == body, "private body dump differs from the ELF")

print("OK vec_ce0890 binary tie")
