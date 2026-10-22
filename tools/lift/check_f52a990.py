#!/usr/bin/env python3
"""Tie native/core/fnv_cstr_161fb0 to original RVA 0x52a990 -> 0x161fb0 (private; needs local/re/eboot.elf).

Both bodies are decoded from the pinned ELF with tools/lift/hash_helper_decode.py (exact encodings,
unknown bytes raise): the 10-byte wrapper is `mov esi,0x811c9dc5; jmp 0x161fb0` and the 57-byte body
has no call, no indirect branch, no stack use and no memory access except byte loads through rdi.
The pinned bytes are then INTERPRETED (offline model, ~15 opcodes) on many strings, including a null
pointer, the empty string, high bytes (sign extension) and the 9 original name strings read from the
ELF, and the results are compared with `test_fnv_cstr --dump` (wrapper and seeded body, seed
0x12345678 passed in esi to prove the wrapper ignores the incoming esi).
"""
import hashlib
import json
import random
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import hash_helper_decode as H  # noqa: E402
import leaf_check_common as base  # noqa: E402

require, ROOT = base.require, base.ROOT
WRAPPER_SHA = '0a633e2d30c9e38a4b9829a318723764d2c4cbeccaa47330c39bfcce9bd5d9f0'
BODY_SHA = '2a64a0c2fa7f86a2726a632b2a6e5e99e60baa185879ed381988cd33293dbda9'
ORIGINAL_NAME_ADDRS = [0x126fdb0, 0x128e808, 0x128e814, 0x128e824, 0x1289a20, 0x1289a2e, 0x12764b5, 0x12764c2, 0x12764d3]
CALLERS = [0x669450, 0x842450, 0x7dace0, 0x6dde20]


def main():
    elf, image = H.load_image()
    require(hashlib.sha256(elf).hexdigest() == H.ELF_SHA256, 'unexpected ELF SHA256')
    require(hashlib.sha256(image.read(0x52a990, 10)).hexdigest() == WRAPPER_SHA, 'wrapper bytes changed')
    require(hashlib.sha256(image.read(0x161fb0, 57)).hexdigest() == BODY_SHA, 'body bytes changed')

    wrapper = H.decode_helper(image, 0x52a990, 0x52a99a)
    require([(w[2], w[3]) for w in wrapper] == [('mov esi,imm32', 0x811c9dc5), ('jmp rel32', 0x161fb0)],
            f'wrapper decode changed: {wrapper}')
    body = H.decode_helper(image, 0x161fb0, 0x161fe9)
    names = [b[2] for b in body]
    require(len(body) == 19 and names.count('ret') == 2, f'body decode changed: {len(body)} instructions')
    require(not any(n.startswith('call') or 'push' in n or 'pop' in n for n in names), 'helper makes calls/uses stack')
    require(('imul eax,ecx,imm32', 0x01000193) in [(b[2], b[3]) for b in body], 'FNV prime multiply missing')
    require(image.read(0x161fe9, 1)[0] == 0x90, 'padding after the body changed')

    # Four direct callers (no others in the ctor table scan are claimed here).
    for rva in CALLERS:
        require(len(H.decode_ctor(image, rva)['calls']) >= 1, f'{rva:#x} no longer calls 0x52a990')

    rnd = random.Random(0x52A990)
    vectors = [None, b'', b'a', b'foobar', b'\x80', b'\xff', b'\x7f\x80\x81', b'ab\xe3\x81\x82z', b'x' * 300]
    vectors += [bytes(rnd.randrange(1, 256) for _ in range(rnd.randrange(1, 40))) for _ in range(40)]
    vectors += [H_read(image, a) for a in ORIGINAL_NAME_ADDRS]
    args, want = [], []
    for v in vectors:
        args.append('n' if v is None else 's:' + v.hex())
        e52, t = H.run_helper(image, 0x52a990, v or b'', rsi=0x12345678, null=v is None)
        e161, _ = H.run_helper(image, 0x161fb0, v or b'', rsi=0x12345678, null=v is None)
        require(0x161fb0 in t, 'wrapper trace did not reach 0x161fb0')
        want.append(dict(h52a990=e52, h161fb0_seed12345678=e161))
    require(want[0] == dict(h52a990=0, h161fb0_seed12345678=0), 'null pointer is not 0')
    require(want[1] == dict(h52a990=0x811c9dc5, h161fb0_seed12345678=0x12345678), 'empty string is not the seed')

    exe = base.native_executable('test_fnv_cstr')
    native = json.loads(subprocess.run([str(exe), '--dump'] + args, check=True, capture_output=True, text=True).stdout)
    require(native == want, 'native fnv_cstr differs from the interpreted machine code')

    readme = (ROOT / 'native/README.md').read_text()
    require('core/fnv_cstr_161fb0.{hpp,cpp}' in readme and 'check_f52a990.py' in readme, 'README row/checker listing missing')
    print(f'OK fnv_cstr_161fb0: wrapper 10 bytes + body 57 bytes ({len(body)} instructions, no calls), '
          f'{len(vectors)} vectors (null, empty, high bytes, 9 original names) match the interpreted machine code')


def H_read(image, addr):
    out = bytearray()
    while image.read(addr, 1)[0]:
        out.append(image.read(addr, 1)[0])
        addr += 1
    return bytes(out)


if __name__ == '__main__':
    main()
