#!/usr/bin/env python3
"""Independent replay of the slot-12 / slot-13 structural dossiers (CPU-01-SLOT12/13-STRUCT-01).

Re-derives every published fact from the hash-pinned private ELF and the offline disassembler:
body span and SHA-256, reachable-instruction count, direct call list, tail-call thunk, RIP-relative
globals, the heap mutex size word, and the helper contracts (mutex init, atexit destructor). No
Ghidra/REA session and no execution of any original code is involved.

Exit 0 only when every assertion holds.
"""
import hashlib
import importlib.util
import json
import struct
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
BSS_START = 0x2DD7100   # first byte not file-backed in PT_LOAD #2 (filesz 0x17d3100 from 0x1604000)
IMAGE_END = 0x3956AA4   # memsz end of PT_LOAD #2


def load_analyzer():
    spec = importlib.util.spec_from_file_location(
        'slot_struct', ROOT / 'tools/lift/analyze_slot_struct.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    module.WORK = Path(tempfile.mkdtemp(prefix='slot12-13-'))
    module._cache.clear()
    return module


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def walk(module, image, rva):
    module._cache.clear()
    argv = ['prog', hex(rva)]
    old = sys.argv
    sys.argv = argv
    try:
        import contextlib
        import io
        buf = io.StringIO()
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

    slot12 = walk(module, image, 0xF06340)
    slot13 = walk(module, image, 0xF058F0)

    require((slot12['span_bytes'], slot12['end_exclusive']) == (205, '0xf0640d'),
            f"slot 12 span is {slot12['span_bytes']} bytes ending {slot12['end_exclusive']}")
    require(slot12['body_sha256'] ==
            'a16f79f44e9fa082e8e7a25d300210dd6538aa4f5f1341d66a6e30769b3dcb53',
            f"slot 12 body sha256 is {slot12['body_sha256']}")
    require(slot12['reachable_instructions'] == 49,
            f"slot 12 reachable instructions = {slot12['reachable_instructions']}")
    require(slot12['blocks_terminated_by_ret'] == 1, 'slot 12 must have exactly one return')
    require(slot12['indirect_transfers'] == [], 'slot 12 must have no indirect transfer')

    calls12 = [(c['at'], c['target'], (c['thunk'] or {}).get('name')) for c in slot12['direct_calls']]
    require(calls12 == [
        ('0xf06395', '0xe1eda0', None),
        ('0xf063ad', '0xe1ede0', None),
        ('0xf063bc', '0xb30a10', None),
        ('0xf063c3', '0xf90210', 'malloc'),
        ('0xf063d5', '0xb30950', None),
        ('0xf063eb', '0xf8f390', '__cxa_atexit'),
        ('0xf06408', '0xf8f3a0', '__stack_chk_fail'),
    ], f'slot 12 direct calls changed: {calls12}')

    globals12 = sorted({r['target'] for r in slot12['rip_relative_refs']})
    for target in (0x15F9EF0, 0x15FE518, 0x3355F88, 0x3355F98, 0xE44A10, 0x1604218):
        require(hex(target) in globals12, f'slot 12 rip-relative target {target:#x} missing')
    for target in (0x3355F88, 0x3355F98):
        require(BSS_START <= target < IMAGE_END, f'{target:#x} is not a .bss global')

    require((slot13['span_bytes'], slot13['end_exclusive']) == (79, '0xf0593f'),
            f"slot 13 span is {slot13['span_bytes']} bytes ending {slot13['end_exclusive']}")
    require(slot13['body_sha256'] ==
            'd021c860c421a25ee64f18d6007daa897d963d4a9be62d51928061b0eee6675b',
            f"slot 13 body sha256 is {slot13['body_sha256']}")
    calls13 = [(c['at'], c['target'], (c['thunk'] or {}).get('name')) for c in slot13['direct_calls']]
    require(calls13 == [
        ('0xf058f6', '0xf030e0', None),
        ('0xf05905', '0xb30a10', None),
        ('0xf0590c', '0xf90210', 'malloc'),
        ('0xf0591e', '0xb30950', None),
    ], f'slot 13 direct calls changed: {calls13}')
    require(slot13['indirect_transfers'] == [], 'slot 13 must have no indirect transfer')
    tails = [b for b in slot13['branch_edges'] if b.get('tail_call')]
    require(len(tails) == 1 and tails[0]['target'] == '0xf8f390'
            and tails[0]['thunk']['name'] == '__cxa_atexit',
            f'slot 13 tail call changed: {tails}')
    globals13 = sorted({r['target'] for r in slot13['rip_relative_refs']})
    for target in (0x3355470, 0x3355480, 0xE44A10, 0x1604218):
        require(hex(target) in globals13, f'slot 13 rip-relative target {target:#x} missing')
    for target in (0x3355470, 0x3355480):
        require(BSS_START <= target < IMAGE_END, f'{target:#x} is not a .bss global')

    # Helper contracts.
    size_word = struct.unpack('<I', image.read(0x12D2D1C, 4))[0]
    require(size_word == 0x10, f'mutex block size word is {size_word:#x}, expected 0x10')
    init = walk(module, image, 0xB30950)
    names = [(c['thunk'] or {}).get('name') for c in init['direct_calls']]
    require(names == ['pthread_mutexattr_init', 'pthread_mutexattr_settype',
                      'pthread_mutexattr_setprotocol', 'pthread_mutex_init',
                      'pthread_mutexattr_destroy', '__stack_chk_fail'],
            f'mutex initializer calls changed: {names}')
    dtor = walk(module, image, 0xE44A10)
    dtor_targets = [c['target'] for c in dtor['direct_calls']]
    require(dtor_targets == ['0xb309d0'], f'destructor calls changed: {dtor_targets}')
    tail = [b for b in dtor['branch_edges'] if b.get('tail_call')]
    require(len(tail) == 1 and tail[0]['thunk']['name'] == 'free',
            f'destructor tail call changed: {tail}')

    print('OK: slot12 0xf06340 205B sha256=a16f79f4... body: 7 calls, 1 ret, 0 indirect; '
          'globals 0x3355f88/0x3355f98; heap mutex size 0x10')
    print('OK: slot13 0xf058f0 79B body: 4 calls + tail __cxa_atexit; '
          'globals 0x3355470/0x3355480')
    print(f"OK: observed slot13 body sha256={slot13['body_sha256']}")
    print('OK: 0xb30950 = pthread mutex init (attr init/type/protocol/init/attr destroy); '
          '0xe44a10 = pthread_mutex_destroy + free')


if __name__ == '__main__':
    main()
