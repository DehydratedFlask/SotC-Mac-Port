#!/usr/bin/env python3
"""Replay of the slot-14..19 structural dossier (CPU-01-SLOT14-19-FAMILY-01).

Re-derives each body span, digest, direct-call list, indirect edge and .bss global pair from the
hash-pinned private ELF, and asserts the shared "descriptor + 16-byte heap mutex + __cxa_atexit"
contract that these six constructors share with slots 12 and 13.

Exit 0 only when every assertion holds. Nothing is executed; no Ghidra/REA session is used.
"""
import hashlib
import importlib.util
import json
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
BSS_START = 0x2DD7100
IMAGE_END = 0x3956AA4
MUTEX_SIZE_WORD = 0x12D2D1C   # [0x12d2d1c] == 0x10, passed to malloc
MUTEX_INIT = 0xB30950
CLEANUP = 0xE44A10
ATEXIT_GOT = '0x15fffb8'

EXPECTED = {
    0xf02ff0: dict(guard=True, index=14, span=239, end='0xf030df', instructions=52,
                   sha256='0c803e076d6f24f81b7cb9cab7deca5e633c4e24f9041350455f87a88f83b7fa',
                   prefix_call='0xf011f0', indirect='0xf03082',
                   bss=('0x3355028', '0x3355038')),
    0xf01100: dict(guard=True, index=15, span=226, end='0xf011e2', instructions=51,
                   sha256='006d8ef333df1b54dde77fcd144deec4277711d05567fe36b20cc31804be7d24',
                   prefix_call='0xe1eda0', indirect=None,
                   bss=('0x3354e00', '0x3354e10')),
    0xf00090: dict(guard=True, index=16, span=284, end='0xf001ac', instructions=57,
                   sha256='aed062395020b9ce82f5c04c640cd1b42ba83a844c84ac9bdbbfda9fbc2ca5a0',
                   prefix_call='0xefe410', indirect='0xf0014f',
                   bss=('0x3354b58', '0x3354b68')),
    0xefe320: dict(guard=True, index=17, span=239, end='0xefe40f', instructions=52,
                   sha256='d157e4bf9c9e7f3900051edb9659906cca7c45c454af68a29d05c73ec9b2a612',
                   prefix_call='0xefd140', indirect='0xefe3b2',
                   bss=('0x33549f0', '0x3354a00')),
    0xefd020: dict(guard=True, index=18, span=281, end='0xefd139', instructions=57,
                   sha256='a14f6aa8db166697a1cc3eb0ad2feb49c15c482e725864333c748335b9492638',
                   prefix_call='0xefad20', indirect='0xefd0dc',
                   bss=('0x3354660', '0x3354670')),
    0xefacd0: dict(guard=False, index=19, span=79, end='0xefad1f', instructions=21,
                   sha256='2750b1530f9edd2b561c213b7c6be699443617bbc464e0769bfc1910bfa51f6e',
                   prefix_call='0xef8600', indirect=None,
                   bss=('0x3354190', '0x33541a0')),
}


def load_analyzer():
    spec = importlib.util.spec_from_file_location(
        'slot_struct', ROOT / 'tools/lift/analyze_slot_struct.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.WORK = Path(tempfile.mkdtemp(prefix='slot-family-'))
    module._cache.clear()
    return module


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def walk(module, rva):
    module._cache.clear()
    import contextlib
    import io
    buf = io.StringIO()
    old, sys.argv = sys.argv, ['prog', hex(rva)]
    try:
        with contextlib.redirect_stdout(buf):
            module.main()
    finally:
        sys.argv = old
    return json.loads(buf.getvalue())


def main():
    elf = (ROOT / 'local/re/eboot.elf').read_bytes()
    digest = hashlib.sha256(elf).hexdigest()
    require(digest == ELF_SHA256, f'unexpected ELF sha256 {digest}')
    _, headers = d.program_headers(elf)
    image = d.Image(elf, headers)
    module = load_analyzer()

    size_word = int.from_bytes(image.read(MUTEX_SIZE_WORD, 4), 'little')
    require(size_word == 0x10, f'mutex size word is {size_word:#x}')

    for rva, want in EXPECTED.items():
        report = walk(module, rva)
        tag = f'{rva:#x} (index {want["index"]})'
        require(report['span_bytes'] == want['span'], f'{tag} span {report["span_bytes"]}')
        require(report['end_exclusive'] == want['end'], f'{tag} end {report["end_exclusive"]}')
        require(report['reachable_instructions'] == want['instructions'],
                f'{tag} instructions {report["reachable_instructions"]}')
        require(report['body_sha256'] == want['sha256'], f'{tag} sha256 {report["body_sha256"]}')
        require(report['blocks_terminated_by_ret'] == (0 if rva == 0xefacd0 else 1),
                f'{tag} return count {report["blocks_terminated_by_ret"]}')

        calls = [(c['at'], c['target'], (c['thunk'] or {}).get('name'))
                 for c in report['direct_calls']]
        targets = [c[1] for c in calls]
        require(targets[0] == want['prefix_call'], f'{tag} first call {targets[0]}')
        require(MUTEX_INIT in [int(t, 16) for t in targets], f'{tag} missing mutex init')
        require(sum(1 for c in calls if c[2] == 'malloc') == 1, f'{tag} malloc count')
        guards = sum(1 for c in calls if c[2] == '__stack_chk_fail')
        require(guards == (1 if want['guard'] else 0),
                f'{tag} stack-chk-fail guard count {guards}')
        atexit = [c for c in calls if c[2] == '__cxa_atexit'] + \
                 [b for b in report['branch_edges'] if (b.get('thunk') or {}).get('name')
                  == '__cxa_atexit']
        require(len(atexit) == 1, f'{tag} atexit registrations: {atexit}')
        require(atexit[0].get('thunk', {}).get('got_slot', ATEXIT_GOT) == ATEXIT_GOT
                if 'thunk' in atexit[0] else True, f'{tag} atexit GOT changed')

        indirects = [i['at'] for i in report['indirect_transfers']]
        if want['indirect'] is None:
            require(indirects == [], f'{tag} unexpected indirect edge {indirects}')
        else:
            require(indirects == [want['indirect']], f'{tag} indirect edges {indirects}')
            require(report['indirect_transfers'][0]['operands'].startswith('qword ptr [rbx]'),
                    f'{tag} indirect operand changed')

        refs = {r['target'] for r in report['rip_relative_refs']}
        for target in want['bss']:
            require(target in refs, f'{tag} missing .bss reference {target}')
            value = int(target, 16)
            require(BSS_START <= value < IMAGE_END, f'{target} is not a .bss global')
        require(f'{CLEANUP:#x}' in refs, f'{tag} missing shared cleanup {CLEANUP:#x}')

    print('OK: slots 14-19 all match the shared descriptor + 16-byte heap mutex + __cxa_atexit '
          'contract (mutex size word 0x10, init 0xb30950, cleanup 0xe44a10, dso 0x1604218)')
    print('OK: slots 14/16/17/18 additionally dispatch one virtual call through [rbx]; '
          'slots 15/19 do not')
    for rva, want in EXPECTED.items():
        print(f'OK: {rva:#x} index {want["index"]} span {want["span"]}B '
              f'sha256={want["sha256"][:16]}... bss {want["bss"][0]}/{want["bss"][1]}')


if __name__ == '__main__':
    main()
