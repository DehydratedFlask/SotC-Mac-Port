#!/usr/bin/env python3
"""ELF byte-oracle for the name-cell initialiser 0x23fd00 (86 B), called by startup slot 342 (twice, NULL source) and by 84 other static initialisers.

0x23fd00(cell=rdi, source=rsi): if source != NULL, cell->hash = 0x162020(source, 0x811c9dc5) (the native fnv_path_cstr_162020, hash-pinned here) and strncpy(cell + 4, source, 15); otherwise cell->hash = 0 and
strncpy(cell + 4, "NULL", 15) with the text at 0xfc15f3; in both cases cell[0x13] = 0. The strncpy import (PLT 0xf8f630, GOT 0x1600108) is tied to its unique JMPREL record, whose NID 6sJWiWSRuqk is the strncpy entry of
docs/service_map.json (a name-by-NID reading, not a lifted libc body). Every call, rip operand, immediate and store window of the body is pinned and the whole body is hash/extent-pinned.

What the 20 byte cell is used for is NOT established.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

def x_strncpy_import(e, spec) -> None:
    """PLT 0xf8f630 -> GOT 0x1600108 -> the unique JMPREL record whose symbol NID is 6sJWiWSRuqk (strncpy in docs/service_map.json)."""
    bp._bp_check_imports(e, ((0xF8F630, 0x1600108, "strncpy"),))
    parsed, _image = bp._bp_import_context()
    rows = [r for r in parsed["relocs"] if r["table"] == "JMPREL" and r["target"] == 0x1600108]
    bp.lib.require(len(rows) == 1 and parsed["symbols"][rows[0]["symbol"]]["nid"] == "6sJWiWSRuqk", "0xf8f630 imports NID 6sJWiWSRuqk (strncpy)")


bp.lib.EXTRAS["strncpy_import"] = x_strncpy_import

SPEC = {
    "slot": -1,
    "label": 'name_cell_23fd00',
    "no_table": True,
    "base": 0x23fd00,
    "size": 86,
    "body_sha256": "1cfc1c97d4a8b137951e149756f14d26d274ba2d891fefdf720e97b21e51f86a",
    "pad": "90909090909090909090",
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0xfc15f3, b'NULL'),
    ],
    "lengths": [
    ],
    "pins": [
        ("imm", 0x23fd12, "bec59d1c81"),
        ("call", 0x23fd1a, 0x162020),
        ("imm", 0x23fd26, "ba0f000000"),
        ("rip", 0x23fd34, 7, 3, 0xfc15f3),
        ("imm", 0x23fd3b, "ba0f000000"),
        ("imm", 0x23fd40, "41c70600000000"),
        ("call", 0x23fd47, 0xf8f630),
        ("imm", 0x23fd4c, "41c6461300"),
    ],
    "m6_callees": {
        'fnv_path_cstr_162020': (0x162020, 98, "a7d19297baf11957f065c2631731c010abc4d382f344a2125d59a09f65684d57"),
    },
    "m6_bytes": [
        (0x23fd0d, "4885db741e", 'test rbx,rbx ; je NULL branch (rsi == NULL)'),
        (0x23fd12, "bec59d1c814889dfe80123f2ff", 'non-NULL: esi = 0x811c9dc5 (the FNV offset basis), call 0x162020(source), cell->hash = eax'),
        (0x23fd22, "498d7e04ba0f0000004889de", 'non-NULL: rdi = cell + 4, edx = 15, rsi = source, jmp to the strncpy call'),
        (0x23fd30, "498d7e04488d35", 'NULL: rdi = cell + 4'),
        (0x23fd34, "488d35b818d800", 'NULL: rsi = 0xfc15f3 (the `NULL` text)'),
        (0x23fd3b, "ba0f000000", 'NULL: edx = 15'),
        (0x23fd40, "41c70600000000", 'NULL: cell->hash = 0'),
        (0x23fd47, "e8e4f8d400", 'strncpy(cell + 4, source-or-NULL-text, 15) through PLT 0xf8f630'),
        (0x23fd4c, "41c6461300", 'cell[0x13] = 0'),
        (0xfc15f3, "4e554c4c00", 'the NULL-branch text `NULL\\0`'),
    ],
    "m6_extras": ['strncpy_import'],
    "native": {'header': 'native/core/name_cell_23fd00.hpp', 'source': 'native/core/name_cell_23fd00.cpp', 'test': 'native/tests/test_name_cell_23fd00.cpp', 'consts': {'kNameCell23fd00Rva': 2358528, 'kNameCell23fd00Size': 86, 'kNameCell23fd00NullTextRva': 16520691, 'kNameCell23fd00CopyLength': 15},
        'header_tokens': ['char text[16]', 'kNameCell23fd00NullText[] = "NULL"', 'sizeof(HashedName20) == 20'], 'source_order': ['cell.hash = fnv_path_cstr_162020(source, kFnvOffsetBasisA2ddc0);', 'copy_padded(cell.text, source);', 'cell.hash = 0;', 'copy_padded(cell.text, kNameCell23fd00NullText);', "cell.text[15] = '\\0';"], 'test_tokens': ['section_null()', 'section_text()', 'c.hash == 0', 'std::memcmp(c.text, "NULL", 5) == 0', '"0123456789abcdefXYZ"', 'c.hash == 0x811c9dc5u']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'name cell 0x23fd00 hash-pinned (calls 0x162020 + strncpy PLT, NULL text, store windows)'))
