#!/usr/bin/env python3
"""Tie native/core/atomic_add_ref_14ccb0 to original helper RVA 0x14ccb0 (private; needs local/re/eboot.elf).

The 5 bytes are decoded from the pinned ELF with tools/lift/tiny_leaf_decode.py (exact encodings), pinned
by SHA-256, interpreted on a poisoned 0x80-byte image with the cell set to the same values that
`test_atomic_add_ref_14ccb0 --dump` uses, and compared byte-for-byte with the native dump. Also records that
the only memory access is one locked 4-byte RMW at rdi+0x30 and that no call/branch exists. The caller
count (call-rel32 sites from 0x14ccb0 targets, >= 800) and the ctor callers 0xb25470/0x7ec4c0 are re-scanned.
"""
import hashlib
import json
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common as base  # noqa: E402
import tiny_leaf_decode as T  # noqa: E402

require, ROOT = base.require, base.ROOT
RVA, LENGTH = 0x14ccb0, 5
BODY_SHA = hashlib.sha256(bytes.fromhex('f0ff4730c3')).hexdigest()


def count_callers(elf, image, target):
    n = 0
    for p in image.loads:
        if not p['flags'] & 1:
            continue
        b = elf[p['offset']:p['offset'] + p['filesz']]
        i = 0
        while True:
            i = b.find(b'\xe8', i)
            if i < 0 or i + 5 > len(b):
                break
            if p['vaddr'] + i + 5 + struct.unpack_from('<i', b, i + 1)[0] == target:
                n += 1
            i += 1
    return n


def main():
    elf, image = T.load_image()
    require(hashlib.sha256(elf).hexdigest() == T.ELF_SHA256, 'unexpected ELF SHA256')
    insns, body = T.decode(image, RVA)
    require(len(body) == LENGTH and hashlib.sha256(body).hexdigest() == BODY_SHA, f'body changed: {body.hex()}')
    require([i[2] for i in insns] == ['lock inc dword [rdi+disp8]', 'ret'] and insns[0][3] == 0x30, f'decode changed: {insns}')
    require(image.read(RVA - 1, 1) == b'\x90' and image.read(RVA + LENGTH, 1) == b'\x90', 'padding around body changed')

    callers = count_callers(elf, image, RVA)
    require(callers >= 800, f'caller count {callers}')
    for ctor in (0xb25470, 0x7ec4c0):
        code = image.read(ctor, 0x800)
        require(any(code[i] == 0xE8 and ctor + i + 5 + struct.unpack_from('<i', code, i + 1)[0] == RVA
                    for i in range(len(code) - 5)), f'ctor {ctor:#x} no longer calls the helper')

    exe = base.native_executable('test_atomic_add_ref_14ccb0')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
    require(native['rva'] == RVA and native['body_length'] == LENGTH and native['offset'] == 0x30, 'native header differs')
    require(len(native['cases']) >= 9, 'too few native cases')
    n = native['buffer']
    for case in native['cases']:
        mem = bytearray(b'\xaa' * n)
        mem[0x30:0x34] = (case['init'] & T.M32).to_bytes(4, 'little')
        res = T.interpret(insns, 0x1000, mem, rdi=0x1000)
        require(res['accesses'] == [('rmw', 0x1030, 4, True)], f'unexpected accesses {res["accesses"]}')
        require(res['mem'].hex() == case['bytes'], f'init {case["init"]}: native {case["bytes"][96:104]} vs machine code {res["mem"].hex()[96:104]}')
    readme = (ROOT / 'native/README.md').read_text()
    require('core/atomic_add_ref_14ccb0.{hpp,cpp}' in readme and 'check_f14ccb0.py' in readme, 'README row/checker listing missing')
    print(f'OK atomic_add_ref_14ccb0: 5-byte body sha256={BODY_SHA[:16]}... (lock inc [rdi+0x30]; ret), '
          f'{callers} call sites, {len(native["cases"])} cell values incl. wrap match the interpreted machine code byte-for-byte')


if __name__ == '__main__':
    main()
