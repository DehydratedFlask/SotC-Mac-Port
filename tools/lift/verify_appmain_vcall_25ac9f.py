#!/usr/bin/env python3
"""Byte-bind app-main's 0x25ac9f indirect call and its constructor vtable context.

Reads only the caller-supplied reconstructed ELF. The constructor's +0x28 slot is
reported as a conditional static table value; the callsite edge stays unresolved
because the value loaded from the object's vptr at runtime is not proven unchanged.
"""
import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_PATH = ROOT / 'local/re/eboot.elf'
ELF_SHA256 = 'd0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3'
ELF_SIZE = 65_214_308
FUNCTION_ENTRY = 0x25a8d0
CALLSITE = 0x25ac9f
VTABLE_BASE = 0x15c8cc8
ADDRESS_POINT = VTABLE_BASE + 0x10
STATIC_SLOT_ADDRESS = ADDRESS_POINT + 0x28

sys.path.insert(0, str(ROOT / 'scripts'))
import ps4_dynlink as d


def require(condition, message):
    if not condition:
        raise SystemExit(f'FAIL: {message}')


def mappings(elf, headers, address, size):
    """Return every file-backed PHDR mapping for one virtual-address span."""
    result = []
    for index, ph in enumerate(headers):
        if ph['vaddr'] <= address and address + size <= ph['vaddr'] + ph['filesz']:
            offset = ph['offset'] + address - ph['vaddr']
            result.append({
                'index': index,
                'type': ph['type'],
                'flags': ph['flags'],
                'offset': offset,
                'bytes': elf[offset:offset + size],
            })
    return result


def read_pt_load(elf, headers, address, expected, label):
    loads = [m for m in mappings(elf, headers, address, len(expected)) if m['type'] == 1]
    require(len(loads) == 1, f'{label}: expected one PT_LOAD file mapping, found {len(loads)}')
    require(loads[0]['bytes'] == expected,
            f'{label} at {address:#x}: got {loads[0]["bytes"].hex()}, expected {expected.hex()}')
    return loads[0]


def read_unique_data(elf, headers, address, size, label):
    """Use PT_LOAD when present; otherwise require one unambiguous file-backed PHDR."""
    all_maps = mappings(elf, headers, address, size)
    loads = [m for m in all_maps if m['type'] == 1]
    selected = loads if loads else all_maps
    require(len(selected) == 1,
            f'{label} at {address:#x}: expected one selected mapping, found {len(selected)}')
    return selected[0], all_maps


def check_instructions(elf, headers):
    checks = [
        ('main stack object address', 0x536742, bytes.fromhex('4c8da5c0c0ffff')),
        ('main passes object to constructor', 0x53675e, bytes.fromhex('4c89e7')),
        ('main constructor call', 0x536775, bytes.fromhex('e826030000')),
        ('constructor retains object', 0x536ab8, bytes.fromhex('4889fb')),
        ('constructor vtable-base LEA', 0x536ac7, bytes.fromhex('488d05fa210901')),
        ('constructor advances address point', 0x536af2, bytes.fromhex('4883c010')),
        ('constructor stores vptr', 0x536afa, bytes.fromhex('488903')),
        ('main reloads object vptr', 0x536839, bytes.fromhex('488b85c0c0ffff')),
        ('main restores same object argument', 0x536840, bytes.fromhex('488dbdc0c0ffff')),
        ('main sets second argument', 0x536847, bytes.fromhex('48c7c6ffffffff')),
        ('main dispatches slot +0x18', 0x53684e, bytes.fromhex('ff5018')),
        ('app-main saves incoming object in r15', 0x25a8f2, bytes.fromhex('4989ff')),
        ('same object passed before earlier indirect call', 0x25aa00, bytes.fromhex('4c89ff')),
        ('earlier indirect call on the object', 0x25aa06, bytes.fromhex('ff5020')),
        ('same object passed to direct helper', 0x25ab12, bytes.fromhex('4c89ff')),
        ('direct helper call on the object', 0x25ab15, bytes.fromhex('e826b8ffff')),
        ('same object passed to second direct helper', 0x25ac0d, bytes.fromhex('4c89ff')),
        ('second direct helper call', 0x25ac10, bytes.fromhex('e8bb000000')),
        ('callsite reloads object first word', 0x25ac99, bytes.fromhex('498b07')),
        ('callsite passes same object in rdi', 0x25ac9c, bytes.fromhex('4c89ff')),
        ('target indirect call', CALLSITE, bytes.fromhex('ff5028')),
    ]
    output = []
    for label, address, expected in checks:
        mapping = read_pt_load(elf, headers, address, expected, label)
        output.append({
            'label': label,
            'address': hex(address),
            'bytes': expected.hex(),
            'file_offset': hex(mapping['offset']),
            'program_header': mapping['index'],
        })
    # The RIP-relative LEA at 0x536ac7 points to the vtable base.
    lea = checks[4][2]
    displacement = struct.unpack('<i', lea[3:7])[0]
    vtable_base = 0x536ac7 + len(lea) + displacement
    require(vtable_base == VTABLE_BASE,
            f'constructor vtable base is {vtable_base:#x}, expected {VTABLE_BASE:#x}')
    return output, vtable_base


def main():
    elf = ELF_PATH.read_bytes()
    digest = hashlib.sha256(elf).hexdigest()
    require(len(elf) == ELF_SIZE, f'ELF size is {len(elf)}, expected {ELF_SIZE}')
    require(digest == ELF_SHA256, f'ELF SHA-256 is {digest}')
    _, headers = d.program_headers(elf)

    instruction_checks, vtable_base = check_instructions(elf, headers)
    require(vtable_base + 0x10 == ADDRESS_POINT, 'address-point calculation changed')

    slot18, slot18_maps = read_unique_data(elf, headers, ADDRESS_POINT + 0x18, 8,
                                          'constructor +0x18 slot')
    target18 = int.from_bytes(slot18['bytes'], 'little')
    require(target18 == FUNCTION_ENTRY,
            f'constructor +0x18 slot points to {target18:#x}, expected app-main {FUNCTION_ENTRY:#x}')

    slot28, slot28_maps = read_unique_data(elf, headers, STATIC_SLOT_ADDRESS, 8,
                                          'constructor +0x28 slot')
    conditional_target = int.from_bytes(slot28['bytes'], 'little')
    require(slot28['bytes'] == bytes.fromhex('b065530000000000'),
            f'constructor +0x28 slot bytes changed: {slot28["bytes"].hex()}')
    target_map, target_maps = read_unique_data(elf, headers, conditional_target, 1,
                                               'conditional target first byte')
    target_loads = [m for m in target_maps if m['type'] == 1 and m['flags'] & 1]
    require(len(target_loads) == 1,
            f'conditional target {conditional_target:#x} lacks one executable PT_LOAD mapping')
    require(target_loads[0]['bytes'] == b'\x55',
            f'conditional target first byte is {target_loads[0]["bytes"].hex()}, expected 55')

    call_mapping = next(c for c in instruction_checks if c['address'] == hex(CALLSITE))
    print('ELF_SHA256', digest)
    print('CALLSITE', f'{CALLSITE:#x}', call_mapping['bytes'],
          'call qword ptr [rax+0x28]')
    print('OBJECT_FLOW', '0x25a8f2 copies incoming RDI to R15; '
          '0x25ac99 loads [R15] into RAX; 0x25ac9c copies the current R15 value to RDI. '
          'Cross-call preservation is not asserted.')
    print('CONSTRUCTOR_VPTR', f'base={vtable_base:#x}', f'address_point={ADDRESS_POINT:#x}',
          f'+0x18={target18:#x}')
    print('CONDITIONAL_STATIC_SLOT', f'{STATIC_SLOT_ADDRESS:#x}',
          f'bytes={slot28["bytes"].hex()}', f'target={conditional_target:#x}',
          f'executable_PT_LOAD_offset={target_loads[0]["offset"]:#x}')
    print('CALLSITE_EDGE UNRESOLVED: the value loaded from [R15] at 0x25ac99 is not '
          'statically proven to remain the constructor vptr. Earlier calls take the current '
          'R15 value as an argument, including the unresolved call at 0x25aa06 and direct calls '
          'at 0x25ab15 and 0x25ac10; no byte evidence here proves those calls preserve the '
          'object first word or R15 across the call boundary. If the constructor vptr is '
          f'unchanged, the +0x28 table word is {conditional_target:#x}.')
    print('OK: byte-bound reaching-object sequence, constructor table context, and conditional '
          'slot word verified; no runtime, class, ABI, or G1 claim')


if __name__ == '__main__':
    main()
