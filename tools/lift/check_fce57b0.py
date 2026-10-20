#!/usr/bin/env python3
"""Match the native fixture to the private ELF constructor at RVA 0xce57b0."""

import hashlib
import json
import struct
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
ELF_PATH = ROOT / "local/re/eboot.elf"
NATIVE_PATH = ROOT / "local/build/native/test_vec_ce57b0"
RVA = 0xCE57B0
ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
EXPECTED_SOURCE = 0x12F8330
EXPECTED_DESTINATION = 0x3351CB0
EXPECTED_BITS = [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0]


def require(condition, message):
    if not condition:
        raise SystemExit(f"FAIL {message}")


def load_segments(elf):
    require(elf[:6] == b"\x7fELF\x02\x01", "input is not little-endian ELF64")
    phoff = struct.unpack_from("<Q", elf, 32)[0]
    phentsize, phnum = struct.unpack_from("<HH", elf, 54)
    require(phentsize >= 56, f"unexpected program-header size {phentsize}")
    require(phoff + phentsize * phnum <= len(elf), "program headers exceed ELF length")

    segments = []
    for index in range(phnum):
        offset = phoff + index * phentsize
        p_type, flags, p_offset, p_vaddr, _, p_filesz, p_memsz, _ = struct.unpack_from(
            "<IIQQQQQQ", elf, offset
        )
        if p_type == 1:  # PT_LOAD
            segments.append((p_vaddr, p_offset, p_filesz, p_memsz, flags))
    require(segments, "ELF has no PT_LOAD segments")
    return segments


def file_offset(segments, address, size):
    matches = [
        p_offset + address - p_vaddr
        for p_vaddr, p_offset, p_filesz, _, _ in segments
        if p_vaddr <= address and address + size <= p_vaddr + p_filesz
    ]
    require(len(matches) == 1, f"VA {address:#x}+{size:#x} is not in one file-backed PT_LOAD")
    return matches[0]


def read_va(elf, segments, address, size):
    offset = file_offset(segments, address, size)
    return elf[offset : offset + size]


def main():
    elf = ELF_PATH.read_bytes()
    actual_sha = hashlib.sha256(elf).hexdigest()
    require(actual_sha == ELF_SHA256, f"wrong ELF SHA256: {actual_sha}")
    segments = load_segments(elf)

    body = read_va(elf, segments, RVA, 17)
    require(body[:4] == bytes.fromhex("c5f81005"), f"unexpected first instruction {body[:4].hex()}")
    require(body[8:12] == bytes.fromhex("c5f81105"), f"unexpected second instruction {body[8:12].hex()}")
    require(body[16] == 0xC3, f"unexpected constructor terminator {body[16]:#x}")

    source = RVA + 8 + struct.unpack_from("<i", body, 4)[0]
    destination = RVA + 16 + struct.unpack_from("<i", body, 12)[0]
    require(source == EXPECTED_SOURCE, f"RIP-relative source is {source:#x}")
    require(destination == EXPECTED_DESTINATION, f"RIP-relative destination is {destination:#x}")

    source_bytes = read_va(elf, segments, source, 16)
    bits = list(struct.unpack("<4I", source_bytes))
    require(bits == EXPECTED_BITS, f"source lanes are {[hex(value) for value in bits]}")

    destination_segments = [
        segment
        for segment in segments
        if segment[0] <= destination and destination + 16 <= segment[0] + segment[3]
    ]
    require(len(destination_segments) == 1, "destination cell is not in one PT_LOAD memory range")

    result = subprocess.run(
        [str(NATIVE_PATH), "--dump"], check=False, capture_output=True, text=True
    )
    require(result.returncode == 0, f"native fixture exited {result.returncode}: {result.stderr.strip()}")
    try:
        native = json.loads(result.stdout)
    except json.JSONDecodeError as error:
        raise SystemExit(f"FAIL invalid native report: {error}") from error
    require(native.get("bits") == bits, f"native lanes differ: {native.get('bits')}")
    require(native.get("source") == source, f"native source address differs: {native.get('source')}")
    require(native.get("value") == destination, f"native destination differs: {native.get('value')}")
    require(native.get("rva") == RVA, f"native RVA differs: {native.get('rva')}")
    require(native.get("index") == 153, f"native constructor index differs: {native.get('index')}")

    print(
        "OK vec_ce57b0 binary tie: "
        f"source={source:#x} destination={destination:#x} bits={[hex(value) for value in bits]}"
    )


if __name__ == "__main__":
    main()
