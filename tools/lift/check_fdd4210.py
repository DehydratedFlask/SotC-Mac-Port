#!/usr/bin/env python3
"""Tie native/core/guarded_dd4210 to original RVA 0xdd4210 (private; needs local/re/eboot.elf).

Decodes the body from the pinned ELF with tools/lift/guarded_const_ctor.decode_body (exact-encoding
walk; unknown bytes raise instead of guessing), pins the body SHA-256 and length, and compares the
native --dump output cell by cell.
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
RVA = 0xdd4210
CTOR_INDEX = 66
BODY_LENGTH = 486
BODY_SHA256 = 'fef6825be54953f290640dea0716bfd2e0545cb36dff607be7f229eed8f3db09'
TAIL_BASE = 0x3352d30
TAIL = '0000008000000080000000800000008007000080070000800700008007000080ffffffff0000000000000000000000000000000000000000ffffffff00000000000000000000000000000000ffffffff0000803f0000803f0000803f00000000cff7ff4017b7d1380000000000000000ffff7fffffff7fffffff7fff00000000'
EXPECTED_CELLS = [
    {"guard": 0x3351ef8, "value": 0x3351ef4, "width": 4, "bits": [0xbf800000]},
    {"guard": 0x3351f08, "value": 0x3351f00, "width": 4, "bits": [0x3f000000]},
    {"guard": 0x3351f18, "value": 0x3351f10, "width": 4, "bits": [0x40000000]},
    {"guard": 0x3351f28, "value": 0x3351f20, "width": 4, "bits": [0x40490fdb]},
    {"guard": 0x3351f38, "value": 0x3351f30, "width": 4, "bits": [0x34000000]},
    {"guard": 0x3351f48, "value": 0x3351f40, "width": 4, "bits": [0x7f7fffff]},
    {"guard": 0x3351f60, "value": 0x3351f50, "width": 16, "bits": [0xffffffff, 0x0, 0x0, 0x0]},
    {"guard": 0x3351f80, "value": 0x3351f70, "width": 16, "bits": [0xffffffff, 0xffffffff, 0xffffffff, 0x0]},
    {"guard": 0x3351f90, "value": 0x3351f88, "width": 4, "bits": [0x1]},
]
EXPECTED_STORES = [
    {"dest": 0x3352d90, "width": 4, "bytes": "cff7ff40"},
    {"dest": 0x3352d94, "width": 4, "bytes": "17b7d138"},
    {"dest": 0x3352d60, "width": 8, "bytes": "0000000000000000"},
    {"dest": 0x3352d70, "width": 8, "bytes": "0000000000000000"},
    {"dest": 0x3352d30, "width": 16, "bytes": "00000080000000800000008000000080"},
    {"dest": 0x3352d78, "width": 8, "bytes": "00000000ffffffff"},
    {"dest": 0x3352d40, "width": 16, "bytes": "07000080070000800700008007000080"},
    {"dest": 0x3352da0, "width": 8, "bytes": "ffff7fffffff7fff"},
    {"dest": 0x3352d50, "width": 8, "bytes": "ffffffff00000000"},
    {"dest": 0x3352d68, "width": 8, "bytes": "ffffffff00000000"},
    {"dest": 0x3352d80, "width": 8, "bytes": "0000803f0000803f"},
    {"dest": 0x3352d58, "width": 8, "bytes": "0000000000000000"},
    {"dest": 0x3352d88, "width": 8, "bytes": "0000803f00000000"},
    {"dest": 0x3352da8, "width": 8, "bytes": "ffff7fff00000000"},
]


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def native_executable(name):
    """The fixture may be built into the shared tree or into a worker-specific build dir."""
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
    require(image.read(RVA + BODY_LENGTH - 1, 1) == b'\xc3',
            'body does not end with the expected ret')

    records, end = g.decode_body(ROOT / 'local/re/eboot.elf', RVA)
    require(end == RVA + BODY_LENGTH, f'decoded body ends at {end:#x}')
    cells = [r for r in records if r['kind'] != 'store']
    stores = [dict(dest=r['dest'], width=r['width'], bytes=r['bytes'])
              for r in records if r['kind'] == 'store']
    decoded_cells = []
    for rec in cells:
        if rec['kind'] == 'guarded_scalar':
            decoded_cells.append(dict(guard=rec['guard'], value=rec['value'], width=4, bits=[rec['bits']]))
        else:
            decoded_cells.append(dict(guard=rec['guard'], value=rec['value'], width=16, bits=rec['bits']))
    require(decoded_cells == EXPECTED_CELLS, f'guarded cells changed: {decoded_cells}')
    require(stores == EXPECTED_STORES, f'unguarded stores changed: {stores}')

    image_tail = bytearray(0 if not stores else
                           max(s['dest'] + s['width'] for s in stores) - TAIL_BASE)
    for store in stores:
        offset = store['dest'] - TAIL_BASE
        image_tail[offset:offset + store['width']] = bytes.fromhex(store['bytes'])
    require(bytes(image_tail).hex() == TAIL, 'tail image differs')

    survey = ROOT / 'local/re/work/ctor-survey.json'
    if survey.is_file():
        row = json.loads(survey.read_text())[CTOR_INDEX]
        require(int(row['ctor'], 16) == RVA, f'ctor survey index {CTOR_INDEX} is not {RVA:#x}')

    exe = native_executable('test_guarded_dd4210')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
    require(native['rva'] == RVA, 'native RVA differs')
    require(native['body_length'] == BODY_LENGTH, 'native body length differs')
    require(native['cells'] == EXPECTED_CELLS, f'native cells differ: {native["cells"]}')
    require(native['tail_base'] == TAIL_BASE, 'native tail base differs')
    require(native['tail'] == TAIL, 'native tail bytes differ')

    readme = (ROOT / 'native/README.md').read_text()
    require('core/guarded_dd4210.{hpp,cpp}' in readme, 'README row missing')
    require('check_fdd4210.py' in readme, 'checker not listed in the README binary-tie list')

    print(f'OK guarded_dd4210: {BODY_LENGTH} bytes, sha256={BODY_SHA256[:16]}..., '
          f'{len(EXPECTED_CELLS)} guarded cells, tail {TAIL_BASE:#x} ({len(TAIL) // 2} bytes); native dump matches')


if __name__ == '__main__':
    main()
