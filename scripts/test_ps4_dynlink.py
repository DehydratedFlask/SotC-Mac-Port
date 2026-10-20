#!/usr/bin/env python3
"""Fixture tests for ps4_dynlink using synthetic bytes only (no game data)."""
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import ps4_dynlink as d


def image(vaddr, payload):
    ph = [dict(type=d.PT_LOAD, flags=5, offset=0, vaddr=vaddr, paddr=vaddr,
               filesz=len(payload), memsz=len(payload), align=8)]
    return d.Image(bytes(payload), ph)


def dynamic_import_elf_fixture():
    """Build a minimal PS4 ELF with one import and JMPREL/RELA rows."""
    module_id, library_id = 0x1234, 0x56
    module_name, library_name = b'FixtureModule', b'libFixture'
    nid = d.nid('sceFixtureFunction')
    module_enc, library_enc = d.encode_id(module_id), d.encode_id(library_id)

    strings = bytearray(b'\0')

    def add_string(value):
        offset = len(strings)
        strings.extend(value + b'\0')
        return offset

    library_name_offset = add_string(library_name)
    module_name_offset = add_string(module_name)
    symbol_name_offset = add_string(f'{nid}#{library_enc}#{module_enc}'.encode('ascii'))

    def align8(value):
        return (value + 7) & ~7

    symbol_table_offset = align8(len(strings))
    relocation_offset = align8(symbol_table_offset + 2 * 24)
    target = 0x612340
    addend = -0x2a
    symbol_index = 1
    rela_target = 0x612348
    rela_type = 6
    rela_addend = -0x123
    symbol_table = (
        struct.pack('<IBBHQQ', 0, 0, 0, 0, 0, 0) +
        struct.pack('<IBBHQQ', symbol_name_offset, 0x12, 0, 0, 0, 0)
    )
    jump_relocation = struct.pack('<QQq', target, (symbol_index << 32) | 7, addend)
    rela_offset = align8(relocation_offset + len(jump_relocation))
    rela_relocation = struct.pack('<QQq', rela_target,
                                  (symbol_index << 32) | rela_type, rela_addend)
    dynlib = bytearray(rela_offset + len(rela_relocation))
    dynlib[:len(strings)] = strings
    dynlib[symbol_table_offset:symbol_table_offset + len(symbol_table)] = symbol_table
    dynlib[relocation_offset:relocation_offset + len(jump_relocation)] = jump_relocation
    dynlib[rela_offset:rela_offset + len(rela_relocation)] = rela_relocation

    dynamic_tags = [
        (d.DT_SCE['NEEDED_MODULE'], (module_id << 48) | (1 << 40) | (2 << 32) | module_name_offset),
        (d.DT_SCE['IMPORT_LIB'], (library_id << 48) | (0x102 << 32) | library_name_offset),
        (d.DT_SCE['EXPORT_LIB'], 0),
        (d.DT_SCE['STRTAB'], 0),
        (d.DT_SCE['STRSZ'], len(strings)),
        (d.DT_SCE['SYMTAB'], symbol_table_offset),
        (d.DT_SCE['SYMTABSZ'], len(symbol_table)),
        (d.DT_SCE['JMPREL'], relocation_offset),
        (d.DT_SCE['PLTRELSZ'], len(jump_relocation)),
        (d.DT_SCE['RELA'], rela_offset),
        (d.DT_SCE['RELASZ'], len(rela_relocation)),
        (0, 0),
    ]
    dynamic = b''.join(struct.pack('<QQ', tag, value) for tag, value in dynamic_tags)

    phoff = 64
    phnum = 2
    dynamic_offset = phoff + phnum * 56
    dynlib_offset = dynamic_offset + len(dynamic)
    ident = b'\x7fELF\x02\x01\x01' + b'\0' * 9
    header = struct.pack('<16sHHIQQQIHHHHHH', ident, 2, 62, 1, 0xfeed, phoff, 0, 0,
                         64, 56, phnum, 0, 0, 0)
    program_headers = (
        struct.pack('<IIQQQQQQ', d.PT_DYNAMIC, 4, dynamic_offset, 0, 0,
                     len(dynamic), len(dynamic), 8) +
        struct.pack('<IIQQQQQQ', d.PT_SCE_DYNLIBDATA, 4, dynlib_offset, 0, 0,
                     len(dynlib), len(dynlib), 8)
    )
    return (header + program_headers + dynamic + dynlib,
            dict(module_id=module_id, library_id=library_id, nid=nid,
                 module_enc=module_enc, library_enc=library_enc,
                 symbol_index=symbol_index, target=target, addend=addend,
                 rela_target=rela_target, rela_type=rela_type, rela_addend=rela_addend))


class Tests(unittest.TestCase):
    def test_nid_known_values(self):
        self.assertEqual(d.nid('_init_env'), 'bzQExy189ZI')   # also derived by pinned prepare.py
        self.assertEqual(d.nid('atexit'), '8G2LB+A3rzg')

    def test_parse_import_and_jmprel_provenance(self):
        elf, expected = dynamic_import_elf_fixture()
        parsed = d.parse(elf)

        module = parsed['modules'][expected['module_id']]
        library = parsed['libraries'][expected['library_id']]
        self.assertEqual(module['name'], 'FixtureModule')
        self.assertEqual(module['enc'], expected['module_enc'])
        self.assertEqual(module['version'], [1, 2])
        self.assertEqual(library['name'], 'libFixture')
        self.assertEqual(library['enc'], expected['library_enc'])
        self.assertEqual(library['version'], 0x102)

        symbol = parsed['symbols'][expected['symbol_index']]
        self.assertEqual(symbol['nid'], expected['nid'])
        self.assertEqual(symbol['lib_enc'], library['enc'])
        self.assertEqual(symbol['mod_enc'], module['enc'])
        self.assertEqual(symbol['raw'],
                         f"{expected['nid']}#{expected['library_enc']}#{expected['module_enc']}")
        self.assertFalse(symbol['defined'])

        self.assertEqual(len(parsed['relocs']), 2)
        jmprel = next(r for r in parsed['relocs'] if r['table'] == 'JMPREL')
        self.assertEqual(jmprel['table'], 'JMPREL')
        self.assertEqual(jmprel['type'], 7)
        self.assertEqual(d.RELOC_NAMES[jmprel['type']], 'R_X86_64_JUMP_SLOT')
        self.assertEqual(jmprel['symbol'], expected['symbol_index'])
        self.assertEqual(jmprel['target'], expected['target'])
        self.assertEqual(jmprel['addend'], expected['addend'])

        rela = next(r for r in parsed['relocs'] if r['table'] == 'RELA')
        self.assertEqual(rela['table'], 'RELA')
        self.assertEqual(rela['type'], expected['rela_type'])
        self.assertEqual(d.RELOC_NAMES[rela['type']], 'R_X86_64_GLOB_DAT')
        self.assertEqual(rela['symbol'], expected['symbol_index'])
        self.assertEqual(rela['target'], expected['rela_target'])
        self.assertEqual(rela['addend'], expected['rela_addend'])

    def test_encode_id_widths(self):
        self.assertEqual([d.encode_id(v) for v in (0, 63, 64, 0xfff, 0x1000)],
                         ['A', '-', 'BA', '--', 'BAA'])

    def test_thunk_resolves_got_slot(self):
        code = b'\xff\x25' + struct.pack('<i', 0x10 - 6) + b'\0' * 16   # jmp [rip+0x10-6] at 0x100
        img = image(0x100, code)
        got = {0x110: dict(symbol=1, type='R_X86_64_JUMP_SLOT', nid='X', addend=0)}
        out = d.resolve_thunk(img, 0x100, got)
        self.assertEqual(out['got_slot'], '0x110')
        self.assertEqual(out['relocation']['nid'], 'X')
        self.assertEqual(d.resolve_thunk(image(0, b'\x90' * 8), 0, {})['kind'], 'not-a-plt-jmp')

    def test_ctor_walk_applies_relocs_skips_zero_stops_at_sentinel(self):
        payload = struct.pack('<4Q', 0xffffffffffffffff, 0, 0, 0)   # slots 0x1000..0x1018
        img = image(0x1000, payload)
        relocs = [dict(type=8, target=0x1010, addend=0x4242)]
        out = d.walk_ctors(img, relocs, 0x1018)
        self.assertEqual([e['target'] for e in out['entries']], ['0x4242'])
        self.assertEqual(out['sentinel'], '0x1000')

    def test_ctor_walk_unterminated_is_error(self):
        with self.assertRaises(ValueError):
            d.walk_ctors(image(0x1000, struct.pack('<2Q', 1, 2)), [], 0x1008, limit=1)

    def test_out_of_bounds_read_rejected(self):
        with self.assertRaises(ValueError):
            image(0, b'\0' * 8).read(4, 8)
        with self.assertRaises(ValueError):
            d.span(b'abc', 2, 5)


if __name__ == '__main__':
    unittest.main()
