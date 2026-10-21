#!/usr/bin/env python3
"""Tie native/core/avx_d8fa40 to original RVA 0xd8fa40 (private; needs local/re/eboot.elf).

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
RVA = 0xd8fa40
CTOR_INDEX = 80
BODY_LENGTH = 93
BODY_SHA256 = 'f4580a78d505a58731e4fc518bf68da297fa634a1ea2d47e8797407d860b072f'
REGION_BASE = 0x3352bb0
REGION = 'ffffffffffffffffffffffff00000000000000800000008000000080000000805f7089305f7089305f7089305f708930000000000000000000000000000000000000004000000040000000400000004095bfd63395bfd63395bfd63395bfd633'
EXPECTED_STORES = [
    {"dest": 0x3352bb0, "width": 16, "bytes": "ffffffffffffffffffffffff00000000"},
    {"dest": 0x3352bf0, "width": 16, "bytes": "00000040000000400000004000000040"},
    {"dest": 0x3352c00, "width": 16, "bytes": "95bfd63395bfd63395bfd63395bfd633"},
    {"dest": 0x3352bc0, "width": 16, "bytes": "00000080000000800000008000000080"},
    {"dest": 0x3352bd0, "width": 16, "bytes": "5f7089305f7089305f7089305f708930"},
    {"dest": 0x3352be0, "width": 16, "bytes": "00000000000000000000000000000000"},
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

    exe = native_executable('test_avx_d8fa40')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
    require(native['rva'] == RVA, 'native RVA differs')
    require(native['body_length'] == BODY_LENGTH, 'native body length differs')
    require(native['base'] == REGION_BASE, 'native region base differs')
    require(native['bytes'] == REGION, 'native region bytes differ')

    readme = (ROOT / 'native/README.md').read_text()
    require('core/avx_d8fa40.{hpp,cpp}' in readme, 'README row missing')
    require('check_fd8fa40.py' in readme, 'checker not listed in the README binary-tie list')

    print(f'OK avx_d8fa40: {BODY_LENGTH} bytes, sha256={BODY_SHA256[:16]}..., '
          f'{len(stores)} stores into {REGION_BASE:#x} ({len(REGION) // 2} bytes); native dump matches')


if __name__ == '__main__':
    main()
