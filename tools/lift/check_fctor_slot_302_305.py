#!/usr/bin/env python3
"""Read-only ELF tie for slots 302/305. Hashes pin complete bodies; operand checks pin
zeroing/cleanup order, vptr publication and all six guard/copy blocks. No game code is executed.
Detailed disassembly stays private. --mutate proves selected operand and source mutations fail.
"""
from __future__ import annotations
import re
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from slot_pin_util import ATEXIT, DSO, ROOT, TABLE_START, Elf, const_value, require

BODIES = {
    0xaaeee0: (343, '4012494fd53ba7564476a19317647e45e578587314b2e6ae6041037657806b2a'),
    0xaaf040: (8, '4e7704d36f166b2a455d03d9b50b56f57967922360c110a597d252af775f6e1e'),
    0xaaf050: (52, '5d5e787b76415b2ed8c34f8aaf90e0cf8c45170230ec4570f299e251dc4621f9'),
    0xa77b60: (462, 'af371dcfaf3dd503487a60f4128ce047e649ce6812b31818146bdfb87cc4785a'),
    0x51230: (5, 'aa09a221759003ecfb7adbac16fb84910a8883fc2a7a7bd88ae6809503ad6e5b'),
}


def check_elf(e: Elf) -> None:
    for rva, (size, sha) in BODIES.items():
        e.body_hash(f'body {rva:#x}', rva, size, sha, always=rva not in (0xaaeee0, 0xa77b60))
    require(e.u64(TABLE_START - 8 * 302) == 0xaaeee0, 'table302')
    require(e.u64(TABLE_START - 8 * 305) == 0xa77b60, 'table305')
    # First handle has the dtor/DSO address loads between zeroing and registration.
    require(e.lea(0xaaeeea) == 0x3305440, 'first handle')
    require(e.call(0xaaeef4) == 0xaaf040, 'first zero')
    require(e.lea(0xaaeef9) == 0xaaf050 and e.lea(0xaaef00) == DSO, 'dtor/DSO')
    require(e.call(0xaaef10) == ATEXIT, 'first atexit')
    e.bytes_at(0xaaef07, '4889de4c89ff4c89f2', 'first cleanup argument order')
    for i in range(1, 10):
        site = 0xaaef15 + 29 * (i - 1)
        require(e.lea(site) == 0x3305440 + 8 * i, f'handle{i}')
        e.bytes_at(site + 7, '4889df', 'zero argument')
        require(e.call(site + 10) == 0xaaf040, 'zero helper')
        e.bytes_at(site + 15, '4c89ff4889de4c89f2', 'cleanup arguments')
        require(e.call(site + 24) == ATEXIT, 'cleanup registration')
    require(e.lea(0xaaf01a) + 16 == 0x15d4638, 'identity vptr')
    e.bytes_at(0xaaf021, '4883c010', 'vptr address point')
    require(e.rip(0xaaf025, 7, 3) == 0x3305490, 'identity cell')
    e.bytes_at(0xaaf040, '48c70700000000c3', 'zero-qword helper')
    e.bytes_at(0xaaf056, '488b1f4885db741f', 'cleanup null check')
    require(e.call(0xaaf061) == 0xaaf260, 'live cleanup remains contract')
    require(e.call(0xaaf06a) == 0x170b0 and e.jmp(0xaaf078) == 0x170b0, 'live buffer/object frees')
    e.bytes_at(0xaaf07d, '4883c4085b5dc3', 'null cleanup returns without calls')
    # Every guard access copies its source to one distinct destination in source order.
    for i in range(6):
        site = 0xa77b64 + 76 * i
        pair, guard = (0x32fb190, 0x32fb1a0) if i < 3 else (0x32fb1a8, 0x32fb1b8)
        require(e.rip(site, 6, 2) == guard, 'guard byte')
        e.bytes_at(site + 6, '84c0752f', 'skip initialized guard')
        require(e.lea(site + 10) == guard, 'acquire guard argument')
        require(e.call(site + 17) == 0xf8f3d0, 'guard acquire')
        e.bytes_at(site + 22, '85c0741f', 'skip initializer when acquire returns zero')
        require(e.call(site + 26) == 0x51230, 'pair zero helper')
        require(e.lea(site + 31) == guard, 'release guard argument')
        require(e.rip(site + 38, 7, 3) == pair, 'pair low store')
        require(e.rip(site + 45, 7, 3) == pair + 8, 'pair high store')
        require(e.call(site + 52) == 0xf8f3e0, 'guard release')
        require(e.rip(site + 57, 8, 4) == pair, 'pair load')
        require(e.lea(site + 65) == 0x32fb130 + 16 * i, 'copy destination')
        e.bytes_at(site + 72, 'c5f81100', '16-byte copy')


def check_native(texts: dict[str, str]) -> None:
    for file, constants in {
        'h302': {'kSlot302Rva': 0xaaeee0, 'kSlot302BodySize':343, 'kSlot302HandleCount':10,
                 'kSlot302DtorRva':0xaaf050, 'kSlot302IdentityVptr':0x15d4638},
        'h305': {'kSlot305Rva':0xa77b60, 'kSlot305BodySize':462, 'kSlot305PairCount':6},
    }.items():
        for name, value in constants.items(): require(const_value(texts[file], name) == value, name)
    for token in ('handle = nullptr;', 'cleanup.record(kSlot302DtorRva, &handle)',
                  'ok = recorded && ok;', 'objects.identity_vptr = kSlot302IdentityVptr;'):
        require(token in texts['c302'], f'native302 {token}')
    require(texts['c302'].index('handle = nullptr;') < texts['c302'].index('cleanup.record(')
            < texts['c302'].index('objects.identity_vptr ='), 'native302 ordering')
    require('static const Pair128 value = pair_zero_51230();' in texts['c305'], 'second static')
    for i in range(6):
        helper = 'static_pair_a77960' if i < 3 else 'static_pair_32fb1a8'
        require(f'pairs[{i}] = {helper}();' in texts['c305'], f'native305 copy{i}')
    require('init_slot302_aaeee0(s.slot302, s.concmd.atexit_log)' in texts['startup'], 'production302')
    require('init_slot305_a77b60(s.slot305)' in texts['startup'], 'production305')


def main() -> None:
    e = Elf(); check_elf(e)
    paths = {'h302':'native/core/ctor_slot_302_aaeee0.hpp', 'c302':'native/core/ctor_slot_302_aaeee0.cpp',
             'h305':'native/core/ctor_slot_305_a77b60.hpp', 'c305':'native/core/ctor_slot_305_a77b60.cpp',
             'startup':'native/core/startup_ctors.cpp'}
    texts = {k:(ROOT/p).read_text() for k,p in paths.items()}; check_native(texts)
    mutations = 0
    if '--mutate' in sys.argv:
        e.skip_hash = True
        # Changed zero target, destructor, final identity cell, and every guard/source/destination.
        sites = [0xaaeef4+1, 0xaaeef9+3, 0xaaf025+3] + [0xaaef15+29*i+3 for i in range(9)]
        sites += [0xa77b64+76*i+off for i in range(6) for off in (2,27,41,49,61,68)]
        for site in sites:
            e.overlay = {site:e.raw(site,1)[0]^1}
            try: check_elf(e)
            except SystemExit: mutations += 1
            else: raise SystemExit(f'FAIL ELF mutation survived {site:#x}')
            e.overlay = {}
        for key, token in [('c302','handle = nullptr;'), ('c302','cleanup.record(kSlot302DtorRva, &handle)'),
                           ('c305','pairs[5] = static_pair_32fb1a8();')]:
            mutant = dict(texts); mutant[key] = mutant[key].replace(token, '')
            try: check_native(mutant)
            except SystemExit: mutations += 1
            else: raise SystemExit(f'FAIL native mutation survived {key}/{token}')
    print(f'OK check_fctor_slot_302_305: 343B + 462B, ten cleanup registrations, six guarded copies; {mutations} mutations rejected')

if __name__ == '__main__': main()
