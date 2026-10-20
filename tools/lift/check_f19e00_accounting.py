#!/usr/bin/env python3
"""Check E28 bytes and compare the native fixture with instruction-derived results.

Reads the private ELF as data. It does not execute ELF code; only the native
fixture executable is run to emit its before/after byte arrays.
"""
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d

ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
RAW_BYTES = 0x98
FIXTURE_BYTES = RAW_BYTES + 16
OFFSET_80 = 0x80
OFFSET_88 = 0x88
OFFSET_90 = 0x90
MASK64 = (1 << 64) - 1

INSTRUCTIONS = (
    (0x1a123, '4b 8b 84 0b 80 00 00 00', 'load qword at +0x80'),
    (0x1a12b, '4c 01 e8', 'add requested qword'),
    (0x1a12e, '4b 89 84 0b 80 00 00 00', 'store qword at +0x80'),
    (0x1a136, '4b 3b 84 0b 88 00 00 00', 'unsigned comparison source at +0x88'),
    (0x1a13e, '76 0b', 'JBE skips +0x88 store when result is unsigned lower/equal'),
    (0x1a140, '4b 8d 8c 0b 88 00 00 00', 'address of +0x88'),
    (0x1a148, '48 89 01', 'conditional qword store at +0x88'),
    (0x1a14b, '45 85 c0', 'mode zero test'),
    (0x1a14e, '74 31', 'JZ skips mode-specific subtraction'),
    (0x1a150, '41 83 f8 02', 'mode comparison with 2'),
    (0x1a154, '0f 85 d3 04 00 00', 'JNZ skips +0x90 subtraction'),
    (0x1a15a, '4f 29 ac 0b 90 00 00 00', 'subtract requested qword at +0x90'),
)

EXPECTED_CASES = {
    'mode2_updates': (0x10, 2, 0x20, 0x1f, 0x80),
    'mode0_preserves_90': (3, 0, 100, 128, 0x1122334455667788),
    'mode3_preserves_90': (7, 3, 0x100, 0x100, 0x8877665544332211),
    'unsigned_high_water': (1, 0, 0x7fffffffffffffff, 0x7ffffffffffffffe,
                            0x123456789abcdef0),
    'wrap_add_and_subtract': (8, 2, 0xfffffffffffffffc, 5, 4),
}


def check_instruction_bytes(elf):
    _, headers = d.program_headers(elf)
    image = d.Image(elf, headers)
    for address, template, description in INSTRUCTIONS:
        expected = bytes.fromhex(template)
        actual = image.read(address, len(expected))
        if actual != expected:
            raise SystemExit(
                f'FAIL {address:#x} {description}: expected {expected.hex()}, got {actual.hex()}')


def read_qword(raw, offset):
    return int.from_bytes(raw[offset:offset + 8], 'little')


def write_qword(raw, offset, value):
    raw[offset:offset + 8] = (value & MASK64).to_bytes(8, 'little')


def derive_from_observed_instructions(before, requested, mode):
    """Apply E28's qword ADD/CMP-JBE/conditional SUB; no original code runs."""
    expected = bytearray(before)
    next_value = (read_qword(before, OFFSET_80) + requested) & MASK64
    write_qword(expected, OFFSET_80, next_value)
    if next_value > read_qword(before, OFFSET_88):
        write_qword(expected, OFFSET_88, next_value)
    if mode == 2:
        write_qword(expected, OFFSET_90,
                    (read_qword(before, OFFSET_90) - requested) & MASK64)
    return bytes(expected)


def check_fixture(fixture_path):
    result = subprocess.run(
        [str(fixture_path), '--json'], cwd=ROOT, capture_output=True, text=True, check=True)
    try:
        payload = json.loads(result.stdout)
    except json.JSONDecodeError as exc:
        raise SystemExit(f'FAIL fixture JSON: {exc}: {result.stdout!r}') from exc

    cases = payload.get('cases')
    if not isinstance(cases, list):
        raise SystemExit('FAIL fixture output lacks a cases array')
    observed = {}
    for case in cases:
        name = case.get('name')
        if name in observed:
            raise SystemExit(f'FAIL duplicate fixture case: {name}')
        observed[name] = case
    if set(observed) != set(EXPECTED_CASES):
        raise SystemExit(f'FAIL fixture case set: {sorted(observed)}')

    for name, (requested, mode, at80, at88, at90) in EXPECTED_CASES.items():
        case = observed[name]
        if (case.get('requested'), case.get('mode')) != (requested, mode):
            raise SystemExit(f'FAIL {name}: unexpected input metadata')
        try:
            before = bytes.fromhex(case['before'])
            actual = bytes.fromhex(case['after'])
        except (KeyError, ValueError) as exc:
            raise SystemExit(f'FAIL {name}: invalid byte arrays: {exc}') from exc
        if len(before) != FIXTURE_BYTES or len(actual) != FIXTURE_BYTES:
            raise SystemExit(f'FAIL {name}: expected {FIXTURE_BYTES} input/output bytes')
        if (read_qword(before, OFFSET_80), read_qword(before, OFFSET_88),
                read_qword(before, OFFSET_90)) != (at80, at88, at90):
            raise SystemExit(f'FAIL {name}: fixture input bytes do not match the named case')
        expected = derive_from_observed_instructions(before, requested, mode)
        if actual != expected:
            mismatch = next(i for i, (left, right) in enumerate(zip(actual, expected))
                            if left != right)
            raise SystemExit(
                f'FAIL {name}: byte {mismatch:#x}: expected {expected[mismatch]:02x}, '
                f'got {actual[mismatch]:02x}')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--elf', type=Path, default=ROOT / 'local/re/eboot.elf')
    parser.add_argument('--fixture', type=Path,
                        default=ROOT / 'local/build/native/test_arena_accounting')
    args = parser.parse_args()
    elf_path = args.elf if args.elf.is_absolute() else ROOT / args.elf
    fixture_path = args.fixture if args.fixture.is_absolute() else ROOT / args.fixture

    elf = elf_path.read_bytes()
    digest = hashlib.sha256(elf).hexdigest()
    if digest != ELF_SHA256:
        raise SystemExit(f'FAIL ELF SHA256: expected {ELF_SHA256}, got {digest}')
    check_instruction_bytes(elf)
    check_fixture(fixture_path)
    print('OK: E28 bytes match; native fixture matches instruction-derived qword results; ELF not executed')


if __name__ == '__main__':
    main()
