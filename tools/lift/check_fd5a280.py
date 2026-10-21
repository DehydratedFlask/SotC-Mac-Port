#!/usr/bin/env python3
"""Tie native/core/avx_d5a280 to original RVA 0xd5a280 (private; needs local/re/eboot.elf).

The body has a loop, so it is checked as a pinned byte template (disp32 fields masked) plus a
from-scratch Python simulation of the decoded loop: base lanes [0, 1, 2, 3, 4, 5, 6, 7], per-block offsets
8/16/24, per-iteration step 32. The simulation result must equal the native --dump bytes.
"""
import hashlib
import json
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
RVA = 0xd5a280
CTOR_INDEX = 105
BODY_LENGTH = 180
BODY_SHA256 = '6630dcb85afc4fffdcf91ef32266d29929ec54a3fa8cdc2450f6b802692e04f4'
BODY_TEMPLATE = 'c5f8100500000000c5fc102500000000c57a6f0d00000000c5fa6f0d00000000c5fa6f1500000000c5fa6f1d00000000488d0d0000000031c0c5f81105000000006666666666662e0f1f840000000000c4e37d19e501c4c159fef9c559fec1c5fe7f2481c4c151fef1c5d1fec2c4e34518f601c5d1fef9c5d1feebc4e33d18ff01c559fec2c5fc11748120c4e33d18c001c559fec3c5fc117c8140c5fc114481604883c020c4e33d18e501483d00020000759dc3'
MASKED_RANGES = [(4, 8), (12, 16), (20, 24), (28, 32), (36, 40), (44, 48), (51, 55), (61, 65)]
PREFIX_CELL = 0x3352120
TABLE_CELL = 0x3352130
TABLE_BYTES = 0x800
RODATA = {'A': 19935200, 'B': 19935456, 'C': 19935216, 'D': 19935232, 'E': 19935248, 'F': 19935264}
BASE_LANES = [0, 1, 2, 3, 4, 5, 6, 7]
BLOCK_ADDS = [0, 8, 16, 24]
STEP = 32


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def native_executable(name):
    for build in ('local/build/native', 'local/build/native-agent-a'):
        candidate = ROOT / build / name
        if candidate.is_file():
            return candidate
    raise SystemExit(f'FAIL {name} not built in local/build/native or local/build/native-agent-a')


def main():
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, 'unexpected ELF SHA256')
    _, headers = d.program_headers(elf)
    image = d.Image(elf, headers)
    body = image.read(RVA, BODY_LENGTH)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256,
            f'body bytes changed: {hashlib.sha256(body).hexdigest()}')
    require(body[-1] == 0xC3, 'body does not end with the expected ret')

    masked = bytearray(body)
    for lo, hi in MASKED_RANGES:
        require(lo + 4 == hi, 'mask range must cover one disp32')
        masked[lo:hi] = b'\0\0\0\0'
    require(masked.hex() == BODY_TEMPLATE, f'masked body differs: {masked.hex()}')

    # Re-derive every rip-relative target from the actual displacements.
    prefix_src = RVA + 8 + struct.unpack_from('<i', body, 4)[0]
    require(prefix_src == RODATA['A'], f'prefix source is {prefix_src:#x}')
    table_src = RVA + 0x30 + 7 + struct.unpack_from('<i', body, 0x33)[0]
    require(table_src == TABLE_CELL, f'table destination is {table_src:#x}')

    for label, offset, add in (('B', 0x08, None), ('C', 0x10, 8), ('D', 0x18, 16),
                               ('E', 0x20, 24), ('F', 0x28, 32)):
        source = RVA + offset + 8 + struct.unpack_from('<i', body, offset + 4)[0]
        require(source == RODATA[label], f'{label} source is {source:#x}')
        if add is not None:
            lanes = struct.unpack('<4I', image.read(source, 16))
            require(lanes == (add,) * 4, f'{label} is not a {add} splat: {lanes}')

    # From-scratch simulation of the decoded loop (independent of the C++ implementation).
    require(list(struct.unpack('<8I', image.read(RODATA['B'], 32))) == BASE_LANES, 'base lanes changed')
    table = bytearray(TABLE_BYTES)
    lanes = list(BASE_LANES)
    for i in range(16):
        for k, add in enumerate(BLOCK_ADDS):
            for lane in range(8):
                value = (lanes[lane] + add) & 0xffffffff
                offset = 0x80 * i + 0x20 * k + 4 * lane
                table[offset:offset + 4] = struct.pack('<I', value)
        lanes = [(lane + STEP) & 0xffffffff for lane in lanes]
    expected = image.read(RODATA['A'], 16).hex() + table.hex()

    survey = ROOT / 'local/re/work/ctor-survey.json'
    if survey.is_file():
        row = json.loads(survey.read_text())[CTOR_INDEX]
        require(int(row['ctor'], 16) == RVA, f'ctor survey index {CTOR_INDEX} is not {RVA:#x}')

    exe = native_executable('test_avx_d5a280')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
    require(native['rva'] == RVA, 'native RVA differs')
    require(native['body_length'] == BODY_LENGTH, 'native body length differs')
    require(native['prefix_base'] == PREFIX_CELL, 'native prefix base differs')
    require(native['table_base'] == TABLE_CELL, 'native table base differs')
    require(native['bytes'] == expected, 'native bytes differ from the simulated loop')

    readme = (ROOT / 'native/README.md').read_text()
    require('core/avx_d5a280.{hpp,cpp}' in readme, 'README row missing')
    require('check_fd5a280.py' in readme, 'checker not listed in the README binary-tie list')

    print(f'OK avx_d5a280: {BODY_LENGTH} bytes, sha256={BODY_SHA256[:16]}..., mask at {PREFIX_CELL:#x}, '
          f'identity table at {TABLE_CELL:#x} ({TABLE_BYTES} bytes); native dump matches the simulation')


if __name__ == '__main__':
    main()
