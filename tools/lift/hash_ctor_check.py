#!/usr/bin/env python3
"""Shared binary-vs-native tie for the 0x52a990 name-hash ctors (private; needs local/re/eboot.elf).

Each tools/lift/check_f<rva>.py pins RVA, body length/SHA-256, instruction count and the ordered
{name address, name cell, hash cell} records, then calls run(spec). run():
  * re-decodes the body from the pinned ELF with tools/lift/hash_helper_decode.decode_ctor (exact
    encodings; the only call allowed targets 0x52a990; unknown bytes raise) and compares with the pins;
  * checks the record layout (16-byte stride, name at +0x00, hash at +0x08) against native
    hashed_name_ctors.hpp;
  * reads the name strings from the pinned ELF (private, offline only, never embedded), computes each
    expected hash by INTERPRETING the pinned machine code of 0x52a990 -> 0x161fb0 (not a re-implemented
    FNV), and compares with the native ctor run by `test_hashed_name_ctors --dump <rva> <s:hex>...`,
    which also proves the name cell holds the supplied pointer and the +0x0c bytes stay untouched.
"""
import hashlib
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import hash_helper_decode as H  # noqa: E402
import leaf_check_common as base  # noqa: E402

ROOT, require = base.ROOT, base.require


def read_cstr(image, addr):
    out = bytearray()
    while True:
        b = image.read(addr, 1)[0]
        if b == 0:
            return bytes(out)
        out.append(b)
        addr += 1


def run(spec):
    rva, name = spec['rva'], spec['name']
    elf, image = H.load_image()
    require(hashlib.sha256(elf).hexdigest() == H.ELF_SHA256, 'unexpected ELF SHA256')
    body = image.read(rva, spec['body_length'])
    require(hashlib.sha256(body).hexdigest() == spec['body_sha256'], f'body bytes changed: {hashlib.sha256(body).hexdigest()}')
    require(body[-1] == 0xC3, 'body does not end with ret')

    d = H.decode_ctor(image, rva)
    require(d['end'] == rva + spec['body_length'], f'decoded body ends at {d["end"]:#x}')
    require(len(d['insns']) == spec['instructions'], f'{len(d["insns"])} instructions decoded')
    require(len(d['calls']) == len(spec['records']), f'{len(d["calls"])} calls decoded')
    got = [(r['name_addr'], r['ptr_cell'], r['hash_cell']) for r in d['records']]
    require(sorted(got) == sorted(spec['records']), f'records changed: {[tuple(hex(x) for x in r) for r in got]}')

    base_addr = spec['region_base']
    for k, (_, ptr_cell, hash_cell) in enumerate(sorted(spec['records'], key=lambda r: r[1])):
        require(ptr_cell == base_addr + 0x10 * k and hash_cell == ptr_cell + 8, f'record {k} breaks the 16-byte layout')

    classes = ROOT / 'local/re/work/ctor-classes.json'
    if classes.is_file():
        row = json.loads(classes.read_text())['rows'][spec['ctor_index']]
        require(int(row['rva'], 16) == rva, f'ctor class index {spec["ctor_index"]} is not {rva:#x}')
        require(row['direct_calls'] == len(spec['records']) and row['indirect'] == 0 and row['returns'] == 1,
                'ctor-classes row disagrees with the decode')

    # helper identity: the callee is exactly the wrapper the helper checker pins
    w = image.read(H.HELPER, 10)
    require(hashlib.sha256(w).hexdigest() == spec['helper_sha256'], 'helper 0x52a990 bytes changed')

    ordered = sorted(d['records'], key=lambda r: r['ptr_cell'])
    strings = [read_cstr(image, r['name_addr']) for r in ordered]
    expected = []
    for s in strings:
        v, trace = H.run_helper(image, H.HELPER, s, rsi=0x5555_5555_5555)   # rsi junk: wrapper must override it
        require(trace[0] == H.HELPER and H.FNV_BODY in trace, 'helper trace did not reach 0x161fb0')
        expected.append(v)
    require(expected == spec['hashes'], f'machine-code hashes changed: {[hex(x) for x in expected]}')

    exe = base.native_executable('test_hashed_name_ctors')
    out = subprocess.run([str(exe), '--dump', f'{rva:x}'] + ['s:' + s.hex() for s in strings], check=True,
                         capture_output=True, text=True).stdout
    native = json.loads(out)
    require(native['count'] == len(strings) and native['stride'] == 16, 'native record count/stride differs')
    for k, rec in enumerate(native['records']):
        require(rec['name_is_input'], f'record {k}: native name cell is not the supplied pointer')
        require(rec['hash'] == expected[k], f'record {k}: native hash {rec["hash"]:#x} != machine code {expected[k]:#x}')
        require(rec['unwritten'] == 0xAAAAAAAA, f'record {k}: bytes +0x0c were written')

    readme = (ROOT / 'native/README.md').read_text()
    require('core/hashed_name_ctors.{hpp,cpp}' in readme and f'check_f{rva:x}.py' in readme, 'README row/checker listing missing')
    print(f'OK {name}: {spec["body_length"]} bytes, sha256={spec["body_sha256"][:16]}..., {len(strings)} record(s) '
          f'{[hex(r[0]) for r in sorted(spec["records"], key=lambda r: r[1])]} -> {[hex(x) for x in expected]}; '
          'native hashes (from caller-supplied strings) match the interpreted machine code')
