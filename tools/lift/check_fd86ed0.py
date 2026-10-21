#!/usr/bin/env python3
"""Tie native/core/avx_d86ed0 to original RVA 0xd86ed0 (private; needs local/re/eboot.elf).

The body is decoded from the pinned ELF with tools/lift/guarded_const_ctor.decode_body, which
matches each instruction by exact encoding and raises on anything unknown. The body SHA-256 and
length are pinned, and the native --dump bytes are compared with the decoded store image.
"""
import hashlib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/lift'))
import guarded_const_ctor as g  # noqa: E402
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
RVA = 0xd86ed0
CTOR_INDEX = 81
BODY_LENGTH = 149
BODY_SHA256 = 'd217178f40e6145f68b5bd2f6e1bdfde8a10b8c958eeb6aa42ba5639b9cef66a'
REGION_BASE = 0x3352aa0
REGION = 'ffffffffffffffffffffffff00000000000000000000000000000000000000000000803f0000000000000000000000000000803f0000000000000000000000000000803f000080bf000000000000000000000000000080bf000000000000000000000000000080bf00000000000000000000000000000000000000000000000000000000f30435bff30435bff304353f00000000f30435bf00000000f304353ff30435bff30435bf00000000f30435bf00000000f304353ff304353ff304353f00000000f304353f00000000f30435bff304353ff30435bf00000000f304353ff304353ff30435bf00000000f304353ff304353f00000000f30435bff304353f00000000f30435bff30435bf00000000'
EXPECTED_STORES = [
    {"dest": 0x3352ac0, "width": 32, "bytes": "0000803f0000000000000000000000000000803f000000000000000000000000"},
    {"dest": 0x3352ae0, "width": 32, "bytes": "0000803f000080bf000000000000000000000000000080bf0000000000000000"},
    {"dest": 0x3352b00, "width": 4, "bytes": "00000000"},
    {"dest": 0x3352aa0, "width": 16, "bytes": "ffffffffffffffffffffffff00000000"},
    {"dest": 0x3352b04, "width": 4, "bytes": "000080bf"},
    {"dest": 0x3352b20, "width": 32, "bytes": "00000000f30435bff30435bff304353f00000000f30435bf00000000f304353f"},
    {"dest": 0x3352b40, "width": 32, "bytes": "f30435bff30435bf00000000f30435bf00000000f304353ff304353ff304353f"},
    {"dest": 0x3352b60, "width": 32, "bytes": "00000000f304353f00000000f30435bff304353ff30435bf00000000f304353f"},
    {"dest": 0x3352b80, "width": 32, "bytes": "f304353ff30435bf00000000f304353ff304353f00000000f30435bff304353f"},
    {"dest": 0x3352ba0, "width": 16, "bytes": "00000000f30435bff30435bf00000000"},
]


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

    records, end = g.decode_body(ROOT / 'local/re/eboot.elf', RVA)
    require(end == RVA + BODY_LENGTH, f'decoded body ends at {end:#x}')
    stores = [dict(dest=r['dest'], width=r['width'], bytes=r['bytes'])
              for r in records if r['kind'] == 'store']
    require(stores == EXPECTED_STORES, f'store list changed: {stores}')
    require(not [r for r in records if r['kind'] != 'store'], 'unexpected guarded records')

    region = bytearray(max(s['dest'] + s['width'] for s in stores) - REGION_BASE)
    for store in stores:
        offset = store['dest'] - REGION_BASE
        region[offset:offset + store['width']] = bytes.fromhex(store['bytes'])
    require(bytes(region).hex() == REGION, 'decoded region image differs')

    survey = ROOT / 'local/re/work/ctor-survey.json'
    if survey.is_file():
        row = json.loads(survey.read_text())[CTOR_INDEX]
        require(int(row['ctor'], 16) == RVA, f'ctor survey index {CTOR_INDEX} is not {RVA:#x}')

    exe = native_executable('test_avx_d86ed0')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
    require(native['rva'] == RVA, 'native RVA differs')
    require(native['body_length'] == BODY_LENGTH, 'native body length differs')
    require(native['base'] == REGION_BASE, 'native region base differs')
    require(native['bytes'] == REGION, 'native region bytes differ')

    readme = (ROOT / 'native/README.md').read_text()
    require('core/avx_d86ed0.{hpp,cpp}' in readme, 'README row missing')
    require('check_fd86ed0.py' in readme, 'checker not listed in the README binary-tie list')

    print(f'OK avx_d86ed0: {BODY_LENGTH} bytes, sha256={BODY_SHA256[:16]}..., '
          f'{len(stores)} stores into {REGION_BASE:#x} ({len(REGION) // 2} bytes); native dump matches')


if __name__ == '__main__':
    main()
