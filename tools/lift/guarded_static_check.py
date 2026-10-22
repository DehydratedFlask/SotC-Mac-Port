#!/usr/bin/env python3
"""Shared binary-vs-native tie for ctors 0x78c760 / 0x268ce0 and their helpers (private; needs local/re/eboot.elf).

run(spec) pins body SHA-256s, call targets and BSS cells; resolves both PLT thunks to the Itanium guard
imports by NID (computed from the names, not hard-coded); INTERPRETS the pinned machine code cold (guard 0,
all watched cells poisoned 0xAA) and warm (guard 1) with tools/lift/guarded_static_decode.py; checks the
exact set of bytes written; scans every rip-relative reference to the static cells (spec['writer_check']);
and compares everything with `test_guarded_static_helpers --dump`.
"""
import hashlib
import json
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import guarded_static_decode as G  # noqa: E402
import leaf_check_common as base  # noqa: E402

require, ROOT = base.require, base.ROOT
THUNKS = {0xF8F3D0: '__cxa_guard_acquire', 0xF8F3E0: '__cxa_guard_release'}
POISON = 0xAA


def code_segment(elf):
    ph = G.dyn.program_headers(elf)[1]
    cp = next(p for p in ph if p['type'] == G.dyn.PT_LOAD and p['flags'] & 1)
    return elf[cp['offset']:cp['offset'] + cp['filesz']], cp['vaddr']


def xrefs(code, vaddr0, target):
    """Disp-field addresses of rip-relative references to target (heuristic modrm scan as in xref_rip.py)."""
    hits = []
    for i in range(1, len(code) - 4):
        if (code[i - 1] & 0xC7) != 0x05:
            continue
        disp = struct.unpack_from('<i', code, i)[0]
        for extra in (0, 1, 4):
            if vaddr0 + i + 4 + extra + disp == target:
                hits.append((i, extra))
    return hits


def classify(code, i, extra):
    """Opcode bytes preceding the disp field at code[i]."""
    return code[i - 3:i].hex(), code[i - 2:i].hex(), code[i - 4:i].hex()


def hexat(m, addr, n):
    return bytes(m.rd(addr + k) for k in range(n)).hex()


def run(spec):
    elf, image = G.load_image()
    require(hashlib.sha256(elf).hexdigest() == G.ELF_SHA256, 'unexpected ELF SHA256')
    got = G.import_names(elf)
    probe = G.Machine(image, got)
    for t, want in THUNKS.items():
        require(probe.thunk(t) == want, f'thunk {t:#x} is {probe.thunk(t)}, expected {want}')

    shas = {}
    for key in ('ctor', 'helper'):
        fn = G.decode_function(image, spec[key])
        n = fn[-1][0] + fn[-1][1] - spec[key]
        shas[key] = (n, hashlib.sha256(image.read(spec[key], n)).hexdigest())
        require(shas[key] == (spec[key + '_length'], spec[key + '_sha256']), f'{key} body changed: {shas[key]}')
    require(image.read(0x51230, 8) == bytes.fromhex('31c031d2c3909090'), '0x51230 bytes changed')

    classes = ROOT / 'local/re/work/ctor-classes.json'
    if classes.is_file():
        row = json.loads(classes.read_text())['rows'][spec['ctor_index']]
        require(int(row['rva'], 16) == spec['ctor'] and row['direct_calls'] == 1 and row['indirect'] == 0
                and row['returns'] == 1, 'ctor-classes row disagrees with the decode')

    calls = lambda rva: [a for _, _, nm, a in G.decode_function(image, rva) if nm == 'call']
    require(calls(spec['ctor']) == [spec['helper']], 'ctor must call only the helper')
    require(calls(spec['helper']) == spec['helper_calls'], f'helper calls changed: {[hex(x) for x in calls(spec["helper"])]}')

    code, v0 = code_segment(elf)
    spec['writer_check'](code, v0)

    def seed(guard):
        s = {a: POISON for a in spec['poison']}
        s[spec['guard']] = guard
        return s

    cold = G.Machine(image, got, seed(0))
    cold.run(spec['ctor'])
    imports = [n for _, n in cold.calls if n != 'in-image']
    require(imports == ['__cxa_guard_acquire', '__cxa_guard_release'], f'cold calls: {cold.calls}')
    written = sorted({a for a, _ in cold.writes})
    require(written == spec['written'], f'cold write set changed: {[hex(a) for a in written]}')
    warm = G.Machine(image, got, seed(1))
    warm.run(spec['ctor'])
    require([n for _, n in warm.calls if n != 'in-image'] == [], 'warm path still calls guard imports')
    require(sorted({a for a, _ in warm.writes}) == spec['warm_written'], 'warm write set changed')
    return cold, warm


def native():
    exe = base.native_executable('test_guarded_static_helpers')
    return json.loads(subprocess.run([str(exe), '--dump'], check=True, capture_output=True, text=True).stdout)


def readme_check(rva_hex):
    readme = (ROOT / 'native/README.md').read_text()
    require('core/guarded_static_helpers.{hpp,cpp}' in readme and f'check_f{rva_hex}.py' in readme,
            'README row/checker listing missing')
