#!/usr/bin/env python3
"""Offline byte-derived oracle for release_shared_str_f590 (analysis tool only).

Requires an owner-supplied pinned ELF and the native fixture. Interpret only the
73-byte helper and 10-byte kind-1 wrapper; 0x12af0 is an explicit external
deallocator observation seam, whose implementation is NOT recovered here.
Original bytes are read privately and never copied into this checker.
"""
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import leaf_check_common as B  # noqa: E402
import x86_strict as X  # noqa: E402

RVA, WRAPPER, DEALLOCATOR = 0xf590, 0x170b0, 0x12af0
PINS = ((RVA, 73, '55eef47ed634189652e4cfc5397d8fa8ee58de4385a6cddc30f403de7ba40a47', 26),
        (WRAPPER, 10, 'bd6d6a49a0a57e9f8f358cd4c02a743796b0e138a0098a26091b0c4c95df3928', 2))
OBJ, CTR, REPLACEMENT, DATA = 0x60000010, 0x60000100, 0x60000200, 0x60000300
MASK = (1 << 64) - 1
CASES = (
    ('null', 0x123456789abcdef0, True, 'none', False),
    ('two', 2, False, 'none', False),
    ('one', 1, False, 'none', False),
    ('clear', 1, False, 'clear', False),
    ('replace', 1, False, 'replace', False),
    ('zero', 0, False, 'none', False),
    ('negative', MASK, False, 'none', False),
    ('minimum', 1 << 63, False, 'none', False),
    ('maximum', (1 << 63) - 1, False, 'none', False),
    ('null_data', 1, False, 'none', True),
)


def bits(value):
    return (value & MASK).to_bytes(8, 'little')


def read64(machine, address):
    return int.from_bytes(X.mem_bytes(machine, address, 8), 'little')


def pointer(value):
    B.require(value in (0, CTR, REPLACEMENT, DATA), f'unexpected pointer {value:#x}')
    return {0: 'null', CTR: 'initial', REPLACEMENT: 'replacement', DATA: 'data'}[value]


class ReleaseMachine(X.Machine):
    def __init__(self, image, mutation, poison):
        super().__init__(image, {}, poison=poison, scratch=[(0x60000000, 0x1000)])
        self.mutation, self.events, self.callback_writes = mutation, [], set()

    def thunk(self, target):
        return 'external_deallocator' if target == DEALLOCATOR else None

    def do_import(self, name, target):
        B.require(name == 'external_deallocator' and target == DEALLOCATOR, 'unexpected external call')
        allocation, kind = self.regs[7], self.regs[6]
        self.calls.append((name, target, dict(rdi=allocation, rsi=kind)))
        self.events.append(dict(allocation=pointer(allocation), kind=kind, counter=f'{read64(self, CTR):016x}'))
        B.require(kind in (0, 1), f'unknown allocation kind {kind}')
        if kind == 0 and self.mutation in ('clear', 'replace'):
            self.write(OBJ + 24, 8, 0 if self.mutation == 'clear' else REPLACEMENT)
            self.callback_writes.update(range(OBJ + 24, OBJ + 32))
        # An external allocator may clobber every volatile integer register and
        # flags. No success path may rely on its return value or argument regs.
        for reg in (0, 1, 2, 6, 7, 8, 9, 10, 11):
            self.regs[reg] = 0xabcdef0000000000 + reg
        self.undef = True


def oracle(image, case, poison):
    name, old, null_ctl, mutation, null_data = case
    machine = ReleaseMachine(image, mutation, poison)
    original = bits(0 if null_data else DATA) + bits(-7) + bits(0x123456789abcdef) + bits(0 if null_ctl else CTR)
    machine.poke(OBJ - 16, b'\xa5' * 16 + original + b'\x5a' * 16)
    machine.poke(CTR, bits(old))
    machine.poke(REPLACEMENT, bits(77))
    machine.run(RVA, {'rdi': OBJ})
    B.require(machine.accesses == ([] if null_ctl else [('rmw', CTR, 8, True)]), 'wrong locked counter access')
    # Exclude writes explicitly made by our callback: the recovered helper may
    # write only the initial counter, never SharedStr fields or adjacent bytes.
    helper_writes = {a for a, _ in machine.writes} - machine.callback_writes
    B.require(helper_writes == (set() if null_ctl else set(range(CTR, CTR + 8))), 'helper writes outside counter')
    return dict(name=name, counter=f'{read64(machine, CTR):016x}', replacement_counter=read64(machine, REPLACEMENT),
                data=pointer(read64(machine, OBJ)), size=int.from_bytes(X.mem_bytes(machine, OBJ + 8, 8), 'little', signed=True),
                aux=read64(machine, OBJ + 16), ctl=pointer(read64(machine, OBJ + 24)),
                object_unchanged=X.mem_bytes(machine, OBJ, 32) == original,
                guards_unchanged=X.mem_bytes(machine, OBJ - 16, 16) == b'\xa5' * 16 and
                    X.mem_bytes(machine, OBJ + 32, 16) == b'\x5a' * 16, events=machine.events)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--elf', type=Path, default=B.ROOT / 'local/re/eboot.elf')
    parser.add_argument('--native', type=Path)
    args = parser.parse_args()
    elf, image = X.load_image(args.elf)
    B.require(hashlib.sha256(elf).hexdigest() == X.ELF_SHA256, 'unexpected source ELF identity')
    for address, size, digest, count in PINS:
        B.require(X.body_sha(image, address, size) == digest, f'body {address:#x} identity mismatch')
        instructions = X.decode_range(image, address, size)
        B.require(len(instructions) == count, f'body {address:#x} instruction count mismatch')
        B.require(instructions[-1].mn in ('ret', 'jmp'), 'body boundary is not terminal')
        if address == RVA:
            B.require([(i.mn, i.lock) for i in instructions if i.mn == 'xadd'] == [('xadd', True)], 'missing locked xadd')
    executable = args.native or B.native_executable('test_shared_str_release_f590')
    native = json.loads(subprocess.run([str(executable), '--dump'], check=True, capture_output=True, text=True).stdout)
    expected = [oracle(image, case, 0) for case in CASES]
    B.require(expected == [oracle(image, case, 0x5a) for case in CASES], 'oracle depends on undefined entry registers')
    B.require(native == {'cases': expected}, f'native/oracle mismatch: native={native!r}; oracle={expected!r}')
    print('OK release_shared_str_f590: pinned ELF; 73-byte helper / 10-byte wrapper; 10 cases x 2 register poisons; '
          'locked 64-bit decrement, callback trace/reload, wrap and untouched fields match native; 0x12af0 remains external')


if __name__ == '__main__':
    main()
