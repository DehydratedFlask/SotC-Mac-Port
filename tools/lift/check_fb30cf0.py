#!/usr/bin/env python3
"""Tie native/core/atomic_xadd_b30cf0 to original helpers RVA 0xb30cf0 (12 bytes) and 0xb30d00 (10 bytes).

Private; needs local/re/eboot.elf. Both bodies are decoded with tools/lift/tiny_leaf_decode.py (exact
encodings), pinned by SHA-256, interpreted on a 4-byte cell (rdi=0x1000, rsi=delta, upper rax poisoned)
for every (cell, delta) pair that `test_atomic_xadd_b30cf0 --dump` covers, and compared with the native
return value and final cell. Only one locked 4-byte RMW at rdi is allowed.
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
SPECS = {
    'dec': dict(rva=0xb30cf0, hex='b8fffffffff00fc107ffc8c3',
                names=['mov eax,imm32', 'lock xadd [rdi],eax', 'dec eax', 'ret']),
    'add': dict(rva=0xb30d00, hex='89f0f00fc1078d0430c3',
                names=['mov eax,esi', 'lock xadd [rdi],eax', 'lea eax,[rax+rsi]', 'ret']),
}


def main():
    elf, image = T.load_image()
    require(hashlib.sha256(elf).hexdigest() == T.ELF_SHA256, 'unexpected ELF SHA256')
    exe = base.native_executable('test_atomic_xadd_b30cf0')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)
    total = 0
    for key, spec in SPECS.items():
        insns, body = T.decode(image, spec['rva'])
        require(body.hex() == spec['hex'], f'{key} body changed: {body.hex()}')
        require([i[2] for i in insns] == spec['names'], f'{key} decode changed: {insns}')
        require(image.read(spec['rva'] - 1, 1) == b'\x90' and image.read(spec['rva'] + len(body), 1) == b'\x90',
                f'{key} padding changed')
        require(native[key]['rva'] == spec['rva'] and native[key]['body_length'] == len(body), f'{key} native header differs')
        for case in native[key]['cases']:
            mem = bytearray((case['init'] & T.M32).to_bytes(4, 'little'))
            delta = case.get('delta', 0)
            res = T.interpret(insns, 0x1000, mem, rdi=0x1000, rsi=delta & T.M32)
            require(res['accesses'] == [('rmw', 0x1000, 4, True)], f'{key} unexpected accesses {res["accesses"]}')
            require(res['upper_zero'], f'{key} rax upper half not zero-extended')
            require(T.s32(res['eax']) == case['ret'], f'{key} {case}: machine code returns {T.s32(res["eax"])}')
            require(T.s32(int.from_bytes(res['mem'], 'little')) == case['mem'], f'{key} {case}: final cell differs')
            total += 1
    readme = (ROOT / 'native/README.md').read_text()
    require('core/atomic_xadd_b30cf0.{hpp,cpp}' in readme and 'check_fb30cf0.py' in readme, 'README row/checker listing missing')
    print(f'OK atomic_xadd_b30cf0: 0xb30cf0 (12 bytes) and 0xb30d00 (10 bytes), {total} (cell,delta) cases '
          f'match the interpreted machine code (return value and final cell, with wraparound)')


if __name__ == '__main__':
    main()
