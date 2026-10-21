#!/usr/bin/env python3
"""Binary-vs-native tie for the batch-35 leaf constructor lifts that need the symbolic simulator.

Counterpart of leaf_check_common.py (private; needs local/re/eboot.elf) for bodies decoded with
tools/lift/leaf_ctor_decode_sym.py: a counted loop (0x9b75a0), a copy of run-time state (0x575d40)
and a copy of run-time state plus address-valued constants (0x99350).

run(spec) re-decodes the body from the pinned ELF, compares body sha256/length/instruction counts,
simulated step count, the full ordered store list (pinned as compact strings, or as count + sha256
for the 513-store loop) and the run-time sources with the pins, then runs the native test with
--dump. The dump declares which run-time source bytes and which address-constant values the native
call was given; the decoded symbolic stores are substituted with those values, so the expected
region image is derived from the original code and compared with the native output, both from a
zero-initialised and from a 0xAA-poisoned struct (bytes no store covers must stay 0xAA).
"""
import hashlib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/lift'))
import leaf_check_common as c  # noqa: E402
import leaf_ctor_decode_sym as Y  # noqa: E402

require = c.require


def enc(tokens):
    """Compact string for a token list: hex runs, 'S:<addr>+<n>' runs, 'A:<value>+<n>' runs."""
    out, i = [], 0
    while i < len(tokens):
        t = tokens[i]
        if isinstance(t, int):
            j = i
            while j < len(tokens) and isinstance(tokens[j], int):
                j += 1
            out.append(bytes(tokens[i:j]).hex())
        elif t.startswith('S:'):
            a = int(t[2:], 16)
            j = i
            while j < len(tokens) and tokens[j] == f'S:{a + (j - i):#x}':
                j += 1
            out.append(f'S:{a:#x}+{j - i}')
        else:
            _, v, k = t.split(':')
            require(int(k) == 0, f'address constant run does not start at byte 0: {t}')
            j = i
            while j < len(tokens) and tokens[j] == f'A:{v}:{j - i}':
                j += 1
            out.append(f'A:{v}+{j - i}')
        i = j
    return '|'.join(out)


def store_lines(stores):
    return [f'{s["dest"]:#x}:{s["width"]}:{enc(s["bytes"])}' for s in stores]


def run(spec):
    rva, name = spec['rva'], spec['name']
    elf, image = Y.load_image()
    require(hashlib.sha256(elf).hexdigest() == Y.ELF_SHA256, 'unexpected ELF SHA256')
    body = image.read(rva, spec['body_length'])
    require(hashlib.sha256(body).hexdigest() == spec['body_sha256'],
            f'body bytes changed: {hashlib.sha256(body).hexdigest()}')
    require(body[-1] == 0xC3, 'body does not end with ret')

    d = Y.decode(image, rva)
    require(d['end'] == rva + spec['body_length'], f'decoded body ends at {d["end"]:#x}')
    require(len(d['insns']) == spec['instructions'], f'{len(d["insns"])} static instructions decoded')
    require(d['steps'] == spec['steps'], f'{d["steps"]} simulated steps')
    stores = d['stores']
    lines = store_lines(stores)
    require(len(stores) == spec['store_count'], f'{len(stores)} stores decoded')
    if 'stores' in spec:
        require(lines == spec['stores'], f'store list changed: {lines}')
    digest = hashlib.sha256('\n'.join(lines).encode()).hexdigest()
    require(digest == spec['stores_sha256'], f'ordered store list digest is {digest}')
    require(d['sources'] == spec['sources'], f'sources changed: {d["sources"]}')

    base, region = Y.region(stores, 0)
    _, covered = Y.region([dict(s, bytes=[1] * s['width']) for s in stores], 0)
    require(base == spec['region_base'], f'region base is {base:#x}')
    require(len(region) == spec['region_length'], f'region is {len(region)} bytes')
    require(sum(covered) == spec['covered_bytes'], f'{sum(covered)} covered bytes')

    classes = ROOT / 'local/re/work/ctor-classes.json'
    if classes.is_file():
        row = json.loads(classes.read_text())['rows'][spec['ctor_index']]
        require(int(row['rva'], 16) == rva, f'ctor class index {spec["ctor_index"]} is not {rva:#x}')
        require(row['class'] == 'other' and row['direct_calls'] == 0 and row['indirect'] == 0,
                'ctor-classes row is not a call-free other body')
        require(row['instructions'] == spec['instructions'] and row['span_bytes'] == spec['body_length'],
                'ctor-classes instruction/span counts differ')

    exe = c.native_executable(f'test_{name}')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True,
                                       text=True).stdout)
    require(native['rva'] == rva, 'native RVA differs')
    require(native['body_length'] == spec['body_length'], 'native body length differs')
    require(native['base'] == base, 'native region base differs')

    # Which run-time bytes / address constants the native call was given.
    src_map = {}
    for chunk in native.get('sources', []):
        for k, b in enumerate(bytes.fromhex(chunk['bytes'])):
            src_map[chunk['addr'] + k] = b
    addr_map = {int(k, 16): int(v, 16) for k, v in native.get('addresses', {}).items()}
    wanted_src = {int(t[2:], 16) for s in stores for t in s['bytes'] if isinstance(t, str) and t[0] == 'S'}
    wanted_addr = {int(t.split(':')[1], 16) for s in stores for t in s['bytes'] if isinstance(t, str) and t[0] == 'A'}
    require(set(src_map) == wanted_src, 'native run-time source coverage differs from the decoded loads')
    require(set(addr_map) == wanted_addr, 'native address-constant set differs from the decoded stores')
    if src_map:
        require(len(set(src_map.values())) >= min(16, len(src_map)),
                'native test source bytes are too uniform to prove the copy')
    for v in addr_map.values():
        require(v not in (0,) and v < 2 ** 64, 'native test address constant must be a non-zero 64-bit value')
        require(all(((v >> (8 * k)) & 0xFF) not in (0, 0xAA) for k in range(8)),
                'native test address constant needs eight distinct non-zero, non-poison bytes')

    sub = lambda toks: bytes(Y.substitute(toks, src_map.__getitem__, addr_map.__getitem__))  # noqa: E731
    expected = sub(region)
    _, poisoned_tokens = Y.region(stores, c.POISON)
    expected_poisoned = sub(poisoned_tokens)
    require(native['bytes'] == expected.hex(), 'native region bytes differ (zero-initialised)')
    require(native['bytes_poisoned'] == expected_poisoned.hex(), 'native region bytes differ (poisoned: coverage)')

    readme = (ROOT / 'native/README.md').read_text()
    require(f'core/{name}.{{hpp,cpp}}' in readme, 'README row missing')
    require(f'check_f{rva:x}.py' in readme, 'checker not listed in the README binary-tie list')

    print(f'OK {name}: {spec["body_length"]} bytes, sha256={spec["body_sha256"][:16]}..., '
          f'{len(stores)} stores ({d["steps"]} simulated steps) into {base:#x} ({len(region)} bytes, '
          f'{sum(covered)} covered); native dump matches')
