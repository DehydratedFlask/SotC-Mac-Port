#!/usr/bin/env python3
"""Tie the 0xee1130 native constant-table lift to the pinned ELF bytes.

Decodes the 182-byte body with an explicit instruction model (exact opcode templates, displacement
targets derived from the raw bytes), simulates the register/vector state in original order, and
compares the resulting 0x80-byte image with the native test's dump. Nothing is executed.

Exit 0 only when every assertion holds.
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

RVA = 0xEE1130
END = 0xEE11E6
BODY_SHA256 = '0c983636dcbe529cd2852757a9368123db371ce189a777562cab666de172349d'
ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
BASE = 0x33540F0
TABLE_BYTES = 0x80

# (opcode+modrm template, instruction length, action, rip target)
SEQ = [
    ('488b05', 7, 'load_rax', 0x1323B50),
    ('488b15', 7, 'load_rdx', 0x1323B70),
    ('488b0d', 7, 'load_rcx', 0x1323B80),
    ('c5f857c0', 4, 'zero_xmm0', None),
    ('c5f176c9', 4, 'ones_xmm1', None),
    ('c5fa1015', 8, 'vmovss_xmm2', 0x1323BDC),
    ('c5f81305', 8, 'store_q_xmm0', 0x33540F0),
    ('c5f9d60d', 8, 'store_q_xmm1', 0x3354100),
    ('c5f9d60d', 8, 'store_q_xmm1', 0x3354110),
    ('c5f81305', 8, 'store_q_xmm0', 0x3354108),
    ('c5f81305', 8, 'store_q_xmm0', 0x3354150),
    ('c5fa1005', 8, 'vmovss_xmm0', 0x1323BE0),
    ('488915', 7, 'store_rdx', 0x3354140),
    ('488905', 7, 'store_rax', 0x3354120),
    ('c5f9d60d', 8, 'store_q_xmm1', 0x3354128),
    ('c5f8100d', 8, 'vmovups_xmm1', 0x1323B60),
    ('48890d', 7, 'store_rcx', 0x3354148),
    ('488b15', 7, 'load_rdx', 0x1323B90),
    ('488b0d', 7, 'load_rcx', 0x1323BA0),
    ('488905', 7, 'store_rax', 0x33540F8),
    ('c5f81315', 8, 'store_q_xmm2', 0x3354118),
    ('48890d', 7, 'store_rcx', 0x3354160),
    ('c5f8110d', 8, 'store_xmm1_16', 0x3354130),
    ('488915', 7, 'store_rdx', 0x3354158),
    ('c5f81305', 8, 'store_q_xmm0', 0x3354168),
    ('c3', 1, 'ret', None),
]


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def dword_qword_zero_extended(image, addr):
    return int.from_bytes(image.read(addr, 4), 'little')


def build_expected(image):
    rax = int.from_bytes(image.read(0x1323B50, 8), 'little')
    table = bytearray(b'\xa5' * TABLE_BYTES)
    xmm = {0: 0, 1: (1 << 128) - 1, 2: 0}
    rdx = rcx = 0
    pc = RVA
    for template, length, action, target in SEQ:
        chunk = image.read(pc, length)
        require(chunk[:len(bytes.fromhex(template))].hex() == template,
                f'instruction at {pc:#x} does not match {template}')
        if target is not None and length > len(bytes.fromhex(template)):
            disp = struct.unpack('<i', chunk[-4:])[0]
            actual = pc + length + disp
            require(actual == target,
                    f'instruction {action} at {pc:#x} targets {actual:#x}, expected {target:#x}')
        if action == 'load_rax':
            rax = int.from_bytes(image.read(target, 8), 'little')
        elif action == 'load_rdx':
            rdx = int.from_bytes(image.read(target, 8), 'little')
        elif action == 'load_rcx':
            rcx = int.from_bytes(image.read(target, 8), 'little')
        elif action == 'zero_xmm0':
            xmm[0] = 0
        elif action == 'ones_xmm1':
            xmm[1] = (1 << 128) - 1
        elif action in ('vmovss_xmm0', 'vmovss_xmm2'):
            reg = 0 if action == 'vmovss_xmm0' else 2
            xmm[reg] = dword_qword_zero_extended(image, target)   # vmovss zeroes the upper lanes
        elif action == 'vmovups_xmm1':
            xmm[1] = int.from_bytes(image.read(target, 16), 'little')
        elif action.startswith('store_q_xmm'):
            reg = int(action[-1])
            offset = target - BASE
            table[offset:offset + 8] = (xmm[reg] & ((1 << 64) - 1)).to_bytes(8, 'little')
        elif action == 'store_xmm1_16':
            offset = target - BASE
            table[offset:offset + 16] = (xmm[1] & ((1 << 128) - 1)).to_bytes(16, 'little')
        elif action.startswith('store_'):
            reg = {'store_rax': rax, 'store_rdx': rdx, 'store_rcx': rcx}[action]
            offset = target - BASE
            table[offset:offset + 8] = reg.to_bytes(8, 'little')
        elif action == 'ret':
            pass
        else:
            raise SystemExit(f'unknown action {action}')
        pc += length
    require(pc == END, f'body ends at {pc:#x}, expected {END:#x}')
    return bytes(table)


def main():
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    digest = hashlib.sha256(elf).hexdigest()
    require(digest == ELF_SHA256, f'unexpected ELF sha256 {digest}')
    _, headers = d.program_headers(elf)
    image = d.Image(elf, headers)

    body = image.read(RVA, END - RVA)
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256,
            f'body sha256 is {hashlib.sha256(body).hexdigest()}')
    expected = build_expected(image)

    executable = ROOT / 'local/build/native/test_const_table_ee1130'
    require(executable.is_file(), 'build the native test first (local/build/native)')
    result = subprocess.run([str(executable), '--dump'], check=True, capture_output=True, text=True)
    native = bytes.fromhex(json.loads(result.stdout)['sha256_payload'])
    require(len(native) == TABLE_BYTES, f'native dump is {len(native)} bytes')
    require(native == expected, 'native table differs from the value derived from the ELF')

    print(f'OK: 0xee1130 body {END - RVA} B sha256={BODY_SHA256[:16]}...; '
          f'15 stores into {BASE:#x}..{BASE + TABLE_BYTES:#x} reproduce the native table')
    for offset in range(0, TABLE_BYTES, 8):
        print(f'  +{offset:#04x} {expected[offset:offset + 8].hex()}')


if __name__ == '__main__':
    main()
