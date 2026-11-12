#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 342 (RVA 0x7d8e10, 1070 bytes), batch 10 (the survey flagged "1 other indirect call, 1 unmodelled helper").

Static resolution: the `callq *0x20(%rax)` at 0x7d91e8 has rax = `leaq 0x1594dc0(%rip)` (0x7d919d, unchanged until the call), so it is the console-command class entry [0x1594dc0 + 0x20] whose RELA record is pinned to 0x20d60
(register-once; the `fcmd_vtable` extra); the two `callq *0x18(%rax)` (0x7d8fe7 / 0x7d90d5) have rax = [rsp + 0x80], stored from `leaq 0x15ce3f0(%rip)` (0x7d8faa / 0x7d9081) and reloaded at 0x7d8fdc / 0x7d90ca, i.e. the
float caller-functor destroy of tag 0x15ce3f0. The unmodelled helper 0x23fd00 (x2, NULL source) is core/name_cell_23fd00 (hash-pinned in this checker as `name_cell_23fd00` and by check_name_cell_23fd00.py); 0xf2f0 is the
40 byte borrowed name view (hash-pinned, strlen import tied). No indirect target is unresolved; nothing is emulated or executed.

Shape: bool variable enableCutsceneDebugCommands (0x325ec38, tag 0x15ce3d0, value = default 1), unbounded floats skipCutsceneHoldTime / skipCutscenePromptDisplayTime (0x325ed00 / 0x325edd8, tag 0x15ce3f0, 2.0f), two
NULL-source 0x23fd00 cells (0x325eeb0 / 0x325eec4) and the console command cp10_skipCurCutsceneCut (0x325ef00, flags 2, handler tag 0x7d8d60 never called, atexit 0x4b0). The handler body is hash-pinned by FDE extent; vtables,
handler tags and destructors are IDENTITY TAGS (recorded, never run). The cutscene-debug reading is inference from the strings.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited driver runs a fail-closed mutation self-test; --tamper [elf|native] are explicit rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402

SPEC = {
    "slot": 342,
    "base": 0x7d8e10,
    "size": 1070,
    "body_sha256": "339bff3437d936f21123a96f4a1ebbbcd3d445130260ecf42f93c119cb16b932",
    "pad": "9090",
    "callees": ['concmd_base_ctor', 'concmd_register', 'cmd_dtor', 'release_f590', 'bool_dtor', 'bool_fn_copy', 'bool_fn_clone', 'bool_fn_move', 'bool_fn_destroy', 'unbounded_float_ctor', 'float_dtor', 'float_fn_copy', 'float_fn_clone', 'float_fn_move', 'float_fn_destroy'],
    "bool_tags": [0x15ce3d0],
    "float_tags": [0x15ce3f0],
    "strings": [
        (0x1289670, b'enableCutsceneDebugCommands'),
        (0x128968c, b'skipCutsceneHoldTime'),
        (0x12896a1, b'skipCutscenePromptDisplayTime'),
        (0x1289704, b'cp10_skipCurCutsceneCut'),
        (0x128971c, b'Skips the current cutscene cut.'),
        (0x12897bd, b''),
    ],
    "lengths": [
        ('enableCutsceneDebugCommands', 0x7d8e83, "48c74424281b000000"),
        ('skipCutsceneHoldTime', 0x7d8f86, "48c744240814000000"),
        ('skipCutscenePromptDisplayTime', 0x7d9072, "48c74424081d000000"),
        ('Skips the current cutscene cut.', 0x7d917b, "48c74424281f000000"),
    ],
    "pins": [
        ("rip", 0x7d8e28, 7, 3, 0x15fe518),
        ("rip", 0x7d8e2f, 7, 3, 0x1594208),
        ("rip", 0x7d8e36, 7, 3, 0x325ec38),
        ("rip", 0x7d8e3d, 7, 3, 0x12897bd),
        ("rip", 0x7d8e44, 7, 3, 0x1289670),
        ("imm", 0x7d8e59, "b901000000"),
        ("imm", 0x7d8e64, "4983c410"),
        ("imm", 0x7d8e83, "48c74424281b000000"),
        ("imm", 0x7d8e9b, "48c744245800000000"),
        ("call", 0x7d8ea4, 0x20af0),
        ("rip", 0x7d8ea9, 7, 3, 0x15940a0),
        ("rip", 0x7d8ebe, 7, 3, 0x15ce3d0),
        ("imm", 0x7d8ec5, "4883c010"),
        ("imm", 0x7d8ed0, "c7436900000000"),
        ("imm", 0x7d8edc, "48c783a000000000000000"),
        ("imm", 0x7d8eee, "48c783b800000000000000"),
        ("call", 0x7d8ef9, 0xf590),
        ("imm", 0x7d8f04, "c683c000000001"),
        ("imm", 0x7d8f0b, "c6436801"),
        ("call", 0x7d8f0f, 0x20d60),
        ("call", 0x7d8f17, 0xf590),
        ("rip", 0x7d8f1c, 7, 3, 0x20b0),
        ("rip", 0x7d8f23, 7, 3, 0x1604218),
        ("call", 0x7d8f2d, 0xf8f390),
        ("rip", 0x7d8f32, 7, 3, 0x15942a0),
        ("rip", 0x7d8f39, 7, 3, 0x325ed00),
        ("rip", 0x7d8f40, 7, 3, 0x128968c),
        ("imm", 0x7d8f62, "b901000000"),
        ("imm", 0x7d8f67, "48c744247000000000"),
        ("rip", 0x7d8f79, 7, 3, 0x15ce3f0),
        ("imm", 0x7d8f86, "48c744240814000000"),
        ("imm", 0x7d8fa1, "48c744243800000000"),
        ("imm", 0x7d8fb2, "48c784248800000000000000"),
        ("imm", 0x7d8fbe, "4983c710"),
        ("call", 0x7d8fcf, 0x6aa00),
        ("call", 0x7d8fd7, 0xf590),
        ("imm", 0x7d8fe7, "ff5018"),
        ("imm", 0x7d8ff0, "c783d000000000000040"),
        ("imm", 0x7d8ffa, "c7436800000040"),
        ("call", 0x7d9001, 0x20d60),
        ("call", 0x7d9009, 0xf590),
        ("rip", 0x7d900e, 7, 3, 0x20f0),
        ("rip", 0x7d9015, 7, 3, 0x1604218),
        ("call", 0x7d901f, 0xf8f390),
        ("rip", 0x7d9024, 7, 3, 0x325edd8),
        ("rip", 0x7d902b, 7, 3, 0x12896a1),
        ("imm", 0x7d903f, "48c744247000000000"),
        ("imm", 0x7d9051, "b901000000"),
        ("rip", 0x7d905f, 7, 3, 0x15ce3f0),
        ("imm", 0x7d9072, "48c74424081d000000"),
        ("rip", 0x7d9089, 7, 3, 0x12897bd),
        ("imm", 0x7d9090, "48c784248800000000000000"),
        ("imm", 0x7d90a7, "48c744243800000000"),
        ("call", 0x7d90bd, 0x6aa00),
        ("call", 0x7d90c5, 0xf590),
        ("imm", 0x7d90d5, "ff5018"),
        ("imm", 0x7d90de, "c783d000000000000040"),
        ("imm", 0x7d90e8, "c7436800000040"),
        ("call", 0x7d90ef, 0x20d60),
        ("call", 0x7d90f7, 0xf590),
        ("rip", 0x7d90fc, 7, 3, 0x20f0),
        ("rip", 0x7d9103, 7, 3, 0x1604218),
        ("call", 0x7d910d, 0xf8f390),
        ("rip", 0x7d9112, 7, 3, 0x325eeb0),
        ("call", 0x7d911b, 0x23fd00),
        ("rip", 0x7d9120, 7, 3, 0x325eec4),
        ("call", 0x7d9129, 0x23fd00),
        ("rip", 0x7d9133, 7, 3, 0x1289704),
        ("call", 0x7d913f, 0xf2f0),
        ("rip", 0x7d9144, 7, 3, 0x325ef00),
        ("rip", 0x7d914b, 8, 4, 0x15fe5e0),
        ("rip", 0x7d9153, 7, 3, 0x128971c),
        ("imm", 0x7d9163, "b902000000"),
        ("rip", 0x7d9174, 7, 3, 0x7d8d60),
        ("imm", 0x7d917b, "48c74424281f000000"),
        ("call", 0x7d9198, 0x20af0),
        ("rip", 0x7d919d, 7, 3, 0x1594dc0),
        ("imm", 0x7d91b5, "c4e37d19c001"),
        ("imm", 0x7d91bb, "c4e3f916c101"),
        ("imm", 0x7d91dd, "48c7839000000000000000"),
        ("imm", 0x7d91e8, "ff5020"),
        ("call", 0x7d91f0, 0xf590),
        ("call", 0x7d91fa, 0xf590),
        ("rip", 0x7d91ff, 7, 3, 0x4b0),
        ("rip", 0x7d9206, 7, 3, 0x1604218),
        ("call", 0x7d9210, 0xf8f390),
        ("rip", 0x7d9215, 7, 3, 0x15fe518),
        ("call", 0x7d9238, 0xf8f3a0),
        ("imm", 0x7d91a9, "488d4810"),
        ("imm", 0x7d8faa, "4889842480000000"),
        ("imm", 0x7d8fdc, "488b842480000000"),
        ("imm", 0x7d9081, "4889842480000000"),
        ("imm", 0x7d90ca, "488b842480000000"),
        ("imm", 0x7d8f79, "488d057054df00"),
        ("imm", 0x7d905f, "488d058a53df00"),
    ],
    "m6_callees": {
        'handler_tag_7d8d60': (0x7d8d60, 13, "bb4b0a65457ca57f40650d3a0faf1e5ca5593a7c10e2edfc94b2dc87c3f89e69"),
        'name_cell_23fd00': (0x23fd00, 86, "1cfc1c97d4a8b137951e149756f14d26d274ba2d891fefdf720e97b21e51f86a"),
        'name_view_f2f0': (0xf2f0, 40, "0173e3a3f11a8aa020b87731d2eef5026621d6f1fa0967581b0dab3df646973b"),
    },
    "m6_extras": ['bp_atexit_imports', 'bp_strlen_import', 'fcmd_vtable'],
    "native": {'header': 'native/core/ctor_slot_342_7d8e10.hpp', 'source': 'native/core/ctor_slot_342_7d8e10.cpp', 'test': 'native/tests/test_ctor_slot_342.cpp', 'consts': {'kSlot342Rva': 8228368, 'kSlot342Size': 1070, 'kSlot342BoolFnTag': 22864848, 'kSlot342FloatFnTag': 22864880, 'kSlot342HandlerTag': 8228192, 'kSlot342VarFlags': 1, 'kSlot342BoolNameLength': 27, 'kSlot342BoolValue': 1, 'kSlot342FloatCount': 2, 'kSlot342FloatValueBits': 1073741824, 'kSlot342NameCellCount': 2, 'kSlot342CommandDescriptionLength': 31, 'kSlot342BoolRva': 52816952, 'kSlot342Float0Rva': 52817152, 'kSlot342Float1Rva': 52817368, 'kSlot342Cell0Rva': 52817584, 'kSlot342Cell1Rva': 52817604, 'kSlot342CommandRva': 52817664},
        'header_tokens': ['kSlot342BoolName[] = "enableCutsceneDebugCommands"', '"skipCutsceneHoldTime", "skipCutscenePromptDisplayTime"', 'kSlot342CommandName[] = "cp10_skipCurCutsceneCut"', 'kSlot342CommandDescription[] = "Skips the current cutscene cut."', 'IDENTITY TAGS', 'name_cell_23fd00'], 'source_order': ['lane_m6::init_bool_var_literal(objects.bvar', 'helper_init_unbounded_float_var_site(objects.fvar[n], spec, concmd)', 'init_name_cell_23fd00(objects.cells[n], nullptr)', 'init_fconcmd_object(objects.command, cmd, concmd)'], 'test_tokens': ['kSlot342Rva == 0x7d8e10 && kSlot342Size == 1070', 'section_alone()', 'section_failures()', 'r.ok && r.rows_done == 6', 'creg.pending.size() == 4', 'creg.atexit_log.size() == 4', 'check_atexit(creg, 0, 0x20b0, &o.bvar)', 'check_atexit(creg, 1, 0x20f0, &o.fvar[0])', 'check_atexit(creg, 3, 0x4b0, &o.command)', 'std::memcmp(o.cells[n].text, "NULL", 5) == 0', 'creg.manager.manager = &token']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'bool var + two unbounded floats (tag 0x15ce3f0, *0x18(%rax) functor destroy) + two NULL 0x23fd00 cells + console command cp10_skipCurCutsceneCut (*0x20(%rax) resolved to 0x20d60)'))
