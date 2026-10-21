#!/usr/bin/env python3
"""Fixture tests for ps4_dynlink using synthetic bytes only (no game data)."""
import struct
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import ps4_dynlink as d


def image(vaddr, payload):
    ph = [dict(type=d.PT_LOAD, flags=5, offset=0, vaddr=vaddr, paddr=vaddr,
               filesz=len(payload), memsz=len(payload), align=8)]
    return d.Image(bytes(payload), ph)


def two_segment_image():
    """File-backed segments at 0x1000/0x2000 with a vaddr gap, plus a filesz < memsz tail."""
    elf = bytearray(0x400)
    elf[0x100:0x110] = bytes(range(16))
    elf[0x200:0x210] = bytes(range(0x80, 0x90))
    elf[0x300:0x308] = b'\xaa' * 8
    ph = [
        dict(type=d.PT_LOAD, flags=5, offset=0x100, vaddr=0x1000, paddr=0x1000,
             filesz=16, memsz=16, align=8),
        dict(type=d.PT_LOAD, flags=5, offset=0x200, vaddr=0x2000, paddr=0x2000,
             filesz=16, memsz=16, align=8),
        dict(type=d.PT_LOAD, flags=6, offset=0x300, vaddr=0x3000, paddr=0x3000,
             filesz=8, memsz=32, align=8),
    ]
    return d.Image(bytes(elf), ph)


def elf_header(ident=b'\x7fELF\x02\x01\x01' + b'\0' * 9, machine=62, phnum=1,
               phentsize=56):
    """Little-endian x86-64 ELF64 header; phoff is fixed at 64."""
    return struct.pack('<16sHHIQQQIHHHHHH', ident, 2, machine, 1, 0, 64, 0, 0,
                       64, phentsize, phnum, 0, 0, 0)


def program_header(ptype=d.PT_LOAD, vaddr=0x1000, filesz=8, memsz=8):
    return struct.pack('<IIQQQQQQ', ptype, 5, 0, vaddr, vaddr, filesz, memsz, 8)


def dynamic_import_elf_fixture(symbol_type=2):
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
        struct.pack('<IBBHQQ', symbol_name_offset, 0x10 | symbol_type, 0, 0, 0, 0)
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

    def test_name_by_hash_resolves_requested_nid_and_omits_absent(self):
        token = 'synthetic_export_name'
        absent_token = 'absent_export_name'
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / 'fixture.cpp').write_text(f'int {token};\n')

            found = d.name_by_hash({d.nid(token), d.nid(absent_token)}, [root])

        self.assertEqual(found, {d.nid(token): token})

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
        self.assertEqual(symbol['type'], 'FUNC')
        self.assertEqual(symbol['bind'], 1)
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

    def test_parse_unmapped_symbol_type_as_decimal_string(self):
        elf, expected = dynamic_import_elf_fixture(symbol_type=3)
        parsed = d.parse(elf)

        symbol = parsed['symbols'][expected['symbol_index']]
        self.assertEqual(symbol['type'], '3')
        self.assertEqual(symbol['bind'], 1)

    def test_parse_rejects_symbol_table_size_not_multiple_of_24(self):
        elf, _ = dynamic_import_elf_fixture()
        elf = bytearray(elf)
        dynamic_offset = 64 + 2 * 56
        symtabsz_value_offset = dynamic_offset + 6 * 16 + 8
        struct.pack_into('<Q', elf, symtabsz_value_offset, 47)

        with self.assertRaises(ValueError) as error:
            d.parse(bytes(elf))
        self.assertEqual(str(error.exception), 'bad symbol table size')

    def test_parse_rejects_unterminated_final_symbol_name(self):
        elf, _ = dynamic_import_elf_fixture()
        elf = bytearray(elf)
        # This fixture's dynamic table follows its two 56-byte program headers;
        # DT_SCE_STRSZ is the fifth 16-byte dynamic entry.
        dynamic_offset = 64 + 2 * 56
        strsz_value_offset = dynamic_offset + 4 * 16 + 8
        strsz = struct.unpack_from('<Q', elf, strsz_value_offset)[0]
        struct.pack_into('<Q', elf, strsz_value_offset, strsz - 1)

        with self.assertRaises(ValueError):
            d.parse(bytes(elf))

    def test_parse_rejects_symbol_name_offset_past_string_table(self):
        elf, _ = dynamic_import_elf_fixture()
        elf = bytearray(elf)
        dynamic_offset = 64 + 2 * 56
        dynamic_size = struct.unpack_from('<Q', elf, 64 + 32)[0]
        dynlib_offset = dynamic_offset + dynamic_size
        strsz = struct.unpack_from('<Q', elf, dynamic_offset + 4 * 16 + 8)[0]
        symtab_offset = struct.unpack_from('<Q', elf, dynamic_offset + 5 * 16 + 8)[0]
        # Symbol 1's st_name field follows the null symbol's 24-byte entry.
        struct.pack_into('<I', elf, dynlib_offset + symtab_offset + 24, strsz)

        with self.assertRaises(ValueError) as error:
            d.parse(bytes(elf))
        self.assertEqual(
            str(error.exception), f'out of bounds: {strsz:#x}+0x1 / {strsz:#x}')

    def test_parse_rejects_pt_dynamic_without_dt_null(self):
        elf, _ = dynamic_import_elf_fixture()
        elf = bytearray(elf)
        # PT_DYNAMIC is the first program header (offset 64); p_filesz/p_memsz are at +32/+40.
        for field_offset in (64 + 32, 64 + 40):
            size = struct.unpack_from('<Q', elf, field_offset)[0]
            struct.pack_into('<Q', elf, field_offset, size - 16)

        with self.assertRaises(ValueError) as error:
            d.parse(bytes(elf))
        self.assertEqual(str(error.exception), 'unterminated PT_DYNAMIC segment')

    def test_parse_rejects_relocation_table_sizes_not_multiple_of_24(self):
        dynamic_offset = 64 + 2 * 56
        cases = (
            ('PLTRELSZ', 'bad JMPREL size'),
            ('RELASZ', 'bad RELA size'),
        )
        for tag_name, expected_message in cases:
            with self.subTest(tag=tag_name):
                elf, _ = dynamic_import_elf_fixture()
                elf = bytearray(elf)
                tag_offset = next(
                    offset
                    for offset in range(dynamic_offset, dynamic_offset + 12 * 16, 16)
                    if struct.unpack_from('<Q', elf, offset)[0] == d.DT_SCE[tag_name]
                )
                struct.pack_into('<Q', elf, tag_offset + 8, 23)

                with self.assertRaises(ValueError) as error:
                    d.parse(bytes(elf))
                self.assertEqual(str(error.exception), expected_message)

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

    def test_image_read_inside_one_file_backed_segment(self):
        img = two_segment_image()
        self.assertEqual(img.read(0x1000, 4), bytes(range(4)))
        self.assertEqual(img.read(0x1004, 12), bytes(range(4, 16)))
        self.assertEqual(img.read(0x2000, 16), bytes(range(0x80, 0x90)))
        self.assertEqual(img.read(0x1000, 16), bytes(range(16)))       # whole segment
        self.assertEqual(img.read(0x1008, 0), b'')                     # zero size inside

    def test_image_reads_sce_relro_segment(self):
        payload = b'relro-fixture'
        img = d.Image(payload, [dict(type=d.PT_SCE_RELRO, flags=4, offset=0,
                                     vaddr=0x4000, paddr=0x4000,
                                     filesz=len(payload), memsz=len(payload), align=8)])
        self.assertTrue(img.mapped(0x4000, len(payload)))
        self.assertEqual(img.read(0x4000, len(payload)), payload)

    def test_image_read_crossing_a_segment_gap_rejected(self):
        img = two_segment_image()
        with self.assertRaises(ValueError) as error:
            img.read(0x100c, 8)                                        # 0x100c..0x1014 spans the gap
        self.assertEqual(str(error.exception),
                         'vaddr 0x100c+8 not file-backed in a loadable segment')

    def test_image_mapped_requires_range_inside_one_segment(self):
        img = two_segment_image()
        self.assertTrue(img.mapped(0x1000, 16))
        self.assertTrue(two_segment_image().mapped(0x1010, 0))           # one-past-end empty range
        self.assertFalse(img.mapped(0x100c, 8))                         # crosses segment end into gap
        self.assertFalse(img.mapped(0x1010, 1))                         # nonzero range starts in gap

    def test_image_read_past_filesz_into_bss_tail_rejected(self):
        img = two_segment_image()
        self.assertTrue(img.mapped(0x3010, 4))                         # mapped via memsz
        with self.assertRaises(ValueError) as error:
            img.read(0x3010, 4)
        self.assertEqual(str(error.exception),
                         'vaddr 0x3010+4 not file-backed in a loadable segment')

    def test_image_read_zero_size_at_end_of_file_backed_segment(self):
        # Documented behaviour: the containment test is vaddr + size <= vaddr + filesz, so a
        # zero-length read exactly at the end of a file-backed segment succeeds with b''.
        img = two_segment_image()
        self.assertEqual(img.read(0x1010, 0), b'')
        # The same zero-length read past the file-backed extent is still rejected, so the
        # allowance follows filesz (not memsz) and is not a general "any address" escape.
        for vaddr in (0x1020, 0x3020):
            with self.subTest(vaddr=hex(vaddr)):
                with self.assertRaises(ValueError):
                    img.read(vaddr, 0)

    def test_program_headers_accepts_little_endian_x86_64(self):
        elf = elf_header() + program_header()
        entry, ph = d.program_headers(elf)
        self.assertEqual(entry, 0)
        self.assertEqual(len(ph), 1)
        self.assertEqual(ph[0]['vaddr'], 0x1000)
        self.assertEqual(ph[0]['memsz'], 8)

    def test_program_headers_rejects_wrong_program_header_entry_size(self):
        elf = elf_header(phentsize=55) + program_header()
        with self.assertRaises(ValueError) as error:
            d.program_headers(elf)
        self.assertEqual(str(error.exception), 'expected ELF64 program header size 56')

    def test_program_headers_rejects_wrong_class_data_and_machine(self):
        cases = (
            ('class-32-bit', b'\x7fELF\x01\x01\x01' + b'\0' * 9, 62),
            ('data-big-endian', b'\x7fELF\x02\x02\x01' + b'\0' * 9, 62),
            ('machine-x86-32', b'\x7fELF\x02\x01\x01' + b'\0' * 9, 3),
        )
        for label, ident, machine in cases:
            with self.subTest(case=label):
                elf = elf_header(ident=ident, machine=machine) + program_header()
                with self.assertRaises(ValueError) as error:
                    d.program_headers(elf)
                self.assertEqual(str(error.exception), 'expected little-endian x86-64 ELF')

    def test_program_headers_rejects_truncated_header_and_phdr_table(self):
        full = elf_header(phnum=2) + program_header() + program_header()
        for cut in (0, 32, 63):
            with self.subTest(cut=cut):
                with self.assertRaises(ValueError) as error:
                    d.program_headers(full[:cut])
                self.assertTrue(str(error.exception).startswith('out of bounds: '))
        # Header is intact but the declared phdr table is cut off mid-entry.
        with self.assertRaises(ValueError) as error:
            d.program_headers(full[:-8])
        self.assertTrue(str(error.exception).startswith('out of bounds: '))
        # An absurd e_phnum must fail on the first out-of-range entry, not index past the buffer.
        with self.assertRaises(ValueError) as error:
            d.program_headers(elf_header(phnum=0xffff) + program_header())
        self.assertTrue(str(error.exception).startswith('out of bounds: '))


if __name__ == '__main__':
    unittest.main()
