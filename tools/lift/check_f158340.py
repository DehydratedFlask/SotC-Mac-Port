#!/usr/bin/env python3
"""Tie native/core/store_value_clear_flag_158340 to original helper RVA 0x158340 (7 bytes; private, needs the ELF).

Decoded with tools/lift/tiny_leaf_decode.py, pinned by bytes, interpreted over the same 16-byte poisoned
images (fill 0xAA and 0x55) and values as `test_store_value_clear_flag_158340 --dump`, and compared
byte-for-byte. Allowed accesses: a plain 4-byte store at rdi and a plain 1-byte store at rdi+4 (no lock).
"""
import hashlib
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common as base  # noqa: E402
import tiny_leaf_decode as T  # noqa: E402

require, ROOT = base.require, base.ROOT
RVA, HEX = 0x158340, '8937c6470400c3'


def main():
    elf, image = T.load_image()
    require(hashlib.sha256(elf).hexdigest() == T.ELF_SHA256, 'unexpected ELF SHA256')
    insns, body = T.decode(image, RVA)
    require(body.hex() == HEX, f'body changed: {body.hex()}')
    require([i[2] for i in insns] == ['mov [rdi],esi', 'mov byte [rdi+4],0', 'ret'], f'decode changed: {insns}')
    require(image.read(RVA - 1, 1) == b'\x90' and image.read(RVA + 7, 1) == b'\x90', 'padding changed')
    exe = base.native_executable('test_store_value_clear_flag_158340')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
    require(native['rva'] == RVA and native['body_length'] == 7, 'native header differs')
    for case in native['cases']:
        mem = bytearray([case['fill']] * native['buffer'])
        res = T.interpret(insns, 0x1000, mem, rdi=0x1000, rsi=case['value'])
        require(res['accesses'] == [('w', 0x1000, 4, False), ('w', 0x1004, 1, False)], f'accesses {res["accesses"]}')
        require(res['mem'].hex() == case['bytes'], f'{case}: machine code {res["mem"].hex()}')
    readme = (ROOT / 'native/README.md').read_text()
    require('core/store_value_clear_flag_158340.{hpp,cpp}' in readme and 'check_f158340.py' in readme, 'README row/checker listing missing')
    print(f'OK store_value_clear_flag_158340: 7-byte body, {len(native["cases"])} (value, preimage) cases match the interpreted machine code byte-for-byte')


if __name__ == '__main__':
    main()
