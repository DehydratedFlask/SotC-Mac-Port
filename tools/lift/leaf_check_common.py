#!/usr/bin/env python3
"""Shared binary-vs-native tie for the batch-34 leaf constructor lifts (private; needs local/re/eboot.elf).

Each tools/lift/check_f<rva>.py pins its own RVA, body length, body SHA-256, ordered store list and
region image, then calls run(spec). run() independently re-decodes the body from the pinned ELF with
tools/lift/leaf_ctor_decode.py (exact encodings; unknown bytes raise), compares everything with the
pins, and compares the native test's --dump (zero-initialised and 0xAA-poisoned struct) with the
decoded store image, so bytes the original never writes must stay untouched in the native lift.
"""
import hashlib
import json
import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/lift'))
import leaf_ctor_decode as L  # noqa: E402

POISON = 0xAA


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL {message}')


def native_executable(name):
    builds = [os.environ['SOTC_NATIVE_BUILD']] if os.environ.get('SOTC_NATIVE_BUILD') else []
    builds += ['local/build/native', 'local/build/native-agent-a']
    for build in builds:
        candidate = (ROOT / build / name)
        if candidate.is_file():
            return candidate
    raise SystemExit(f'FAIL {name} not built in {builds}')


def run(spec):
    rva, name = spec['rva'], spec['name']
    elf, image = L.load_image()
    require(hashlib.sha256(elf).hexdigest() == L.ELF_SHA256, 'unexpected ELF SHA256')
    body = image.read(rva, spec['body_length'])
    require(hashlib.sha256(body).hexdigest() == spec['body_sha256'],
            f'body bytes changed: {hashlib.sha256(body).hexdigest()}')
    require(body[-1] == 0xC3, 'body does not end with the expected ret')

    decoded = L.decode(image, rva)
    require(decoded['end'] == rva + spec['body_length'], f'decoded body ends at {decoded["end"]:#x}')
    require(len(decoded['insns']) == spec['instructions'], f'{len(decoded["insns"])} instructions decoded')
    stores = decoded['stores']
    require(stores == spec['stores'], f'store list changed: {stores}')
    base, region = L.region_image(stores, 0)
    _, poisoned = L.region_image(stores, POISON)
    require(base == spec['region_base'], f'region base is {base:#x}')
    require(region.hex() == spec['region'], 'decoded region image differs')

    classes = ROOT / 'local/re/work/ctor-classes.json'
    if classes.is_file():
        row = json.loads(classes.read_text())['rows'][spec['ctor_index']]
        require(int(row['rva'], 16) == rva, f'ctor class index {spec["ctor_index"]} is not {rva:#x}')
        require(row['class'] == 'other' and row['direct_calls'] == 0 and row['indirect'] == 0,
                'ctor-classes row is not a call-free other body')

    exe = native_executable(f'test_{name}')
    native = json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True,
                                       text=True).stdout)
    require(native['rva'] == rva, 'native RVA differs')
    require(native['body_length'] == spec['body_length'], 'native body length differs')
    require(native['base'] == base, 'native region base differs')
    require(native['bytes'] == region.hex(), 'native region bytes differ (zero-initialised)')
    require(native['bytes_poisoned'] == poisoned.hex(), 'native region bytes differ (poisoned: coverage)')

    readme = (ROOT / 'native/README.md').read_text()
    require(f'core/{name}.{{hpp,cpp}}' in readme, 'README row missing')
    require(f'check_f{rva:x}.py' in readme, 'checker not listed in the README binary-tie list')

    print(f'OK {name}: {spec["body_length"]} bytes, sha256={spec["body_sha256"][:16]}..., '
          f'{len(stores)} stores into {base:#x} ({len(region)} bytes); native dump matches')
