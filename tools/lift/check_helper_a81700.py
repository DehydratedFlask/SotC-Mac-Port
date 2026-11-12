#!/usr/bin/env python3
"""ELF-tie checker for the named-record registrar 0xa81700 and the two bodies it reaches (CPU-01-HELPER-CALLEE-MODELS-03).

Pins instruction by instruction: 0xa81700 (190 B; the record stores, the guarded first-use initialisation of the registry 0x32fd398 with guard byte
0x33053f0 and atexit(0xa81b80, registry, DSO 0x1604218), tail `jmp 0xa81840`), 0xa81840 (265 B; the two-vector push, four `reserve(1)` calls, stack protector),
0xa82050 (245 B; vector<T*>::reserve with the 1.5x growth, 0x17070 / memmove / 0x170b0, the two noreturn exits) and 0xa81b50 (34 B; the zeroing constructor the
inline initialisation copies). The imports used are NID-proven (__cxa_guard_acquire / __cxa_guard_release / __cxa_atexit / __stack_chk_fail / memmove).
The 20 call sites of startup slots 497 (0x262a80, 7 calls) and 513 (0x1d0770, 13 calls) are re-derived from bytes: record address (rip lea), kind (edx), flag (cl),
and the call; the owning slot bodies are hash-pinned and each contains exactly its listed calls.

NOT established: what the records mean; the exit-time unregistration (0xa81950 / 0xa819f0) and the registry destructor 0xa81b80 (identity only); the targets of the
two noreturn exits beyond "called, then ud2". The slot-side register assignments (r12 / r14 / rdi at each call) are a straight-line static reading of the pinned bytes.

Usage: python3 -I tools/lift/check_helper_a81700.py [--no-mutate] [--tamper elf|native]
"""
from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402
import helper_callee_lib as lib  # noqa: E402
import helper_callee_pins_d as pins_d  # noqa: E402
from slot_pin_util import require  # noqa: E402

lib.pins.PINS.update(pins_d.PINS)

REGISTRY, GUARD = 0x32FD398, 0x33053F0

# (call, record lea site, lea length, record address, edx site, edx bytes, ecx site, ecx bytes, rdi-move site or None, rdi-move bytes)
SLOT_497 = ("slot 497 (0x262a80)", 0x262A80, 9164, "629685642a828585b97e375f91d0a01551f1262125230769f85937a92ef8a2df", [
    (0x264938, 0x264900, 0x2EEE620, 0x264917, "31d2", 0x264919, "b901000000", 0x264935, "4c89e7"),
    (0x264995, 0x26495A, 0x2EEE658, 0x264971, "ba01000000", 0x264976, "b901000000", 0x264992, "4c89e7"),
    (0x2649EB, 0x2649B0, 0x2EEE690, 0x2649C7, "ba01000000", 0x2649CC, "b901000000", 0x2649E8, "4c89e7"),
    (0x264A41, 0x264A06, 0x2EEE6C8, 0x264A1D, "ba01000000", 0x264A22, "b901000000", 0x264A3E, "4c89e7"),
    (0x264A97, 0x264A5C, 0x2EEE700, 0x264A73, "ba01000000", 0x264A78, "b901000000", 0x264A94, "4c89e7"),
    (0x264AED, 0x264AB2, 0x2EEE738, 0x264AC9, "ba01000000", 0x264ACE, "b901000000", 0x264AEA, "4c89e7"),
    (0x264B43, 0x264B08, 0x2EEE770, 0x264B1F, "ba01000000", 0x264B24, "b901000000", 0x264B40, "4c89e7"),
])
SLOT_513 = ("slot 513 (0x1d0770)", 0x1D0770, 4050, "25c3f1eed1b09bf7e905f581c563108c26ab3771a8badaa19fe39b2d35510551", [
    (0x1D07DB, 0x1D078F, 0x2EE6D00, 0x1D07A9, "31d2", 0x1D07AB, "31c9", 0x1D07AD, "4c89f7"),   # r14 = record, mov rdi, r14
    (0x1D0826, 0x1D07FB, 0x2EE6D38, 0x1D0802, "31d2", 0x1D0804, "31c9", None, None),          # lea rdi directly
    (0x1D0871, 0x1D0846, 0x2EE6D70, 0x1D084D, "31d2", 0x1D084F, "31c9", None, None),
    (0x1D08BF, 0x1D0891, 0x2EE6DA8, 0x1D0898, "ba01000000", 0x1D089D, "31c9", None, None),
    (0x1D090D, 0x1D08DF, 0x2EE6DE0, 0x1D08E6, "ba01000000", 0x1D08EB, "31c9", None, None),
    (0x1D095B, 0x1D092D, 0x2EE6E18, 0x1D0934, "ba01000000", 0x1D0939, "31c9", None, None),
    (0x1D09A9, 0x1D097B, 0x2EE6E50, 0x1D0982, "ba01000000", 0x1D0987, "31c9", None, None),
    (0x1D09F7, 0x1D09C9, 0x2EE6E88, 0x1D09D0, "ba01000000", 0x1D09D5, "31c9", None, None),
    (0x1D0A45, 0x1D0A17, 0x2EE6EC0, 0x1D0A1E, "ba01000000", 0x1D0A23, "31c9", None, None),
    (0x1D0A87, 0x1D0A62, 0x2EE6EF8, 0x1D0A69, "ba01000000", 0x1D0A6E, "31c9", None, None),
    (0x1D0AC9, 0x1D0AA4, 0x2EE6F30, 0x1D0AAB, "ba01000000", 0x1D0AB0, "31c9", None, None),
    (0x1D0B0B, 0x1D0AE6, 0x2EE6F68, 0x1D0AED, "ba01000000", 0x1D0AF2, "31c9", None, None),
    (0x1D0B4B, 0x1D0B27, 0x2EE6FA0, 0x1D0B2E, "ba01000000", 0x1D0B33, "31c9", None, None),
])


def _imm_or_zero(hexstr: str) -> int:
    raw = bytes.fromhex(hexstr)
    if raw in (b"\x31\xd2", b"\x31\xc9"):
        return 0
    require(raw[0] in (0xBA, 0xB9) and len(raw) == 5, f"mov r32, imm32 {hexstr}")
    return struct.unpack("<I", raw[1:])[0]


def _check_slot(e, spec) -> list[tuple[int, int, int, int]]:
    name, base, size, sha, sites = spec
    fde = lib.fde_ranges()
    if fde is not None:
        require(fde.get(hex(base)) == size, f"{name}: FDE length")
    e.body_hash(name, base, size, sha, always=True)
    body = e.raw(base, size)
    found = [base + i for i in range(size - 4) if body[i] == 0xE8
             and base + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == 0xA81700]
    require(found == [s[0] for s in sites], f"{name}: exactly the listed calls to 0xa81700, got {[hex(x) for x in found]}")
    table = []
    for call, lea, record, edx_site, edx_hex, ecx_site, ecx_hex, mov_site, mov_hex in sites:
        require(e.call(call) == 0xA81700, f"{name}: call at {call:#x} reaches 0xa81700")
        require(e.lea(lea) == record, f"{name}: record loaded at {lea:#x} is {record:#x}")
        e.bytes_at(edx_site, edx_hex, f"{name}: edx (kind) before the call at {call:#x}")
        e.bytes_at(ecx_site, ecx_hex, f"{name}: ecx (flag) before the call at {call:#x}")
        if mov_site is not None:
            e.bytes_at(mov_site, mov_hex, f"{name}: mov rdi, record before the call at {call:#x}")
            require(call - mov_site <= 0x40, f"{name}: the rdi move is near the call at {call:#x}")
        else:
            require(call - lea <= 0x30, f"{name}: the rdi lea is near the call at {call:#x}")
        table.append((call, record, _imm_or_zero(edx_hex), _imm_or_zero(ecx_hex)))
    return table


def extra(e, fams) -> None:
    bp._bp_check_imports(e, ((0xF8F3D0, 0x15FFFD8, "__cxa_guard_acquire"), (0xF8F3E0, 0x15FFFE0, "__cxa_guard_release"),
                             (0xF8F390, 0x15FFFB8, "__cxa_atexit"), (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),
                             (0xF8F480, 0x1600030, "memmove")))
    t497 = _check_slot(e, SLOT_497)
    t513 = _check_slot(e, SLOT_513)
    # records are a contiguous 0x38 stride array in each slot; kinds / flags are the derived columns the native header documents
    for table, first_kind in ((t497, 0), (t513, 0)):
        for i, (call, record, kind, flag) in enumerate(table):
            require(record == table[0][1] + 0x38 * i, f"record {i} at {record:#x} continues the 0x38 stride array")
        require(table[0][2] == first_kind, "first record kind is 0")
    require([r[2] for r in t497] == [0, 1, 1, 1, 1, 1, 1] and [r[3] for r in t497] == [1] * 7,
            "slot 497 (kind, flag) columns: kinds 0,1,1,1,1,1,1 and flag 1")
    require([r[2] for r in t513] == [0, 0, 0] + [1] * 10 and [r[3] for r in t513] == [0] * 13,
            "slot 513 (kind, flag) columns: kinds 0,0,0,1 x10 and flag 0")


SPEC = {
    "name": "check_helper_a81700",
    "summary": ("0xa81700 (190 B) + 0xa81840 (265 B) + 0xa82050 (245 B) + 0xa81b50 (34 B) pinned per instruction with the imports NID-proven; "
                "the 7 + 13 call sites of slots 497 / 513 re-derived (record, kind, flag)"),
    "families": [],
    "bodies": [
        {"name": "0xa81700 named record ctor", "rva": 0xA81700, "size": 190, "fde": 190, "pins": "record_ctor_a81700",
         "sha": "d1cd3b80f06fb1259fd898a474ba53bccf44eca8b9bc0c508b8df3b2e9c39411"},
        {"name": "0xa81840 two-vector push", "rva": 0xA81840, "size": 265, "fde": 265, "pins": "registry_push_a81840",
         "sha": "5b5f3c60b45197d5521030ce80c1de96ed4dc5540a6d1863798c4a2a6d7d5283"},
        {"name": "0xa82050 vector reserve", "rva": 0xA82050, "size": 245, "fde": 245, "pins": "vec_reserve_a82050",
         "sha": "def6d82b0c831bb693039b2cf3b92f338ee2c41b185148965f3d9ac201cd9831"},
        {"name": "0xa81b50 registry zeroing ctor", "rva": 0xA81B50, "size": 34, "fde": 34, "pins": "registry_ctor_a81b50",
         "sha": "99b8398a9607e7e3870e3157958ca1ac8a5903b1dfad1cd959d2f70f43bd808f"},
    ],
    "rips": [
        ("guard byte test", 0xA8173C, 6, 2, GUARD), ("guard acquire arg", 0xA81746, 7, 3, GUARD),
        ("atexit destructor", 0xA81756, 7, 3, 0xA81B80), ("atexit dso handle", 0xA8175D, 7, 3, 0x1604218),
        ("registry for atexit", 0xA81768, 7, 3, REGISTRY), ("init +0x08 (16 B)", 0xA8176F, 8, 4, REGISTRY + 0x08),
        ("init +0x18", 0xA8177B, 11, 3, REGISTRY + 0x18), ("init +0x50", 0xA81786, 10, 2, REGISTRY + 0x50),
        ("init +0x28 (32 B)", 0xA81790, 8, 4, REGISTRY + 0x28), ("guard release arg", 0xA8179D, 7, 3, GUARD),
        ("push registry arg", 0xA817A9, 7, 3, REGISTRY), ("push stack protector load", 0xA8184F, 7, 3, 0x15FE518),
    ],
    "calls": [
        ("guard acquire", 0xA8174D, 0xF8F3D0), ("atexit", 0xA81798, 0xF8F390), ("guard release", 0xA817A4, 0xF8F3E0),
        ("push reserve 1a", 0xA81890, 0xA82050), ("push reserve 1b", 0xA818B5, 0xA82050),
        ("push reserve 2a", 0xA818F7, 0xA82050), ("push reserve 2b", 0xA81914, 0xA82050),
        ("push stack_chk_fail", 0xA81943, 0xF8F3A0),
        ("reserve allocate 0x17070", 0xA820DF, 0x17070), ("reserve memmove", 0xA820FB, 0xF8F480),
        ("reserve free 0x170b0", 0xA8210C, 0x170B0), ("reserve length_error exit", 0xA82137, 0xA82150),
        ("reserve bad_alloc exit", 0xA8213E, 0xF8F5F0),
    ],
    "jmps": [("tail jmp to the push", 0xA817B9, 0xA81840)],
    "relocs": [],
    "bytes": [
        ("reserve growth: shr rbx / add / cmovb / cmovb", 0xA820B6, "48d1eb4829de4801cb4839ce490f42dc4839d3480f42da"),
        ("reserve max element count", 0xA82083, "48b8ffffffffffffff1f"),
        ("reserve noreturn ud2 x2", 0xA8213C, "0f0b"), ("reserve noreturn ud2 b", 0xA82143, "0f0b"),
    ],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_a81700_named_record.hpp", "helper_c": "native/core/helper_a81700_named_record.cpp",
        "test": "native/tests/test_helper_a81700.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kNamedRecordCtorRva": 0xA81700, "kNamedRecordCtorSize": 190, "kRecordRegistryPushRva": 0xA81840,
                     "kRecordRegistryPushSize": 265, "kVecReserveRva": 0xA82050, "kVecReserveSize": 245,
                     "kRecordRegistryCtorRva": 0xA81B50, "kRecordRegistryCtorSize": 34, "kRecordRegistryDtorRva": 0xA81B80,
                     "kRecordRegistryObjectRva": REGISTRY, "kRecordRegistryGuardRva": GUARD, "kNamedRecordStride": 0x38},
    },
    "tokens": {
        "helper_h": ["std::uint8_t gap24[4];        // never written", "std::uint8_t flag;            // +0x31            cl",
                     "RecordPtrVec second;          // +0x28 .. +0x3f",
                     "static_assert(sizeof(RecordRegistry32fd398) == 0x58",
                     "inline constexpr std::uint64_t kVecMaxElements = 0x1fffffffffffffffULL;"],
        "helper_c": ["record.name.ctl = name.ctl;", "inc_ctl(record.name.ctl);", "record.kind = kind;", "record.q28 = 0;",
                     "record.b30 = 0;", "record.flag = flag;", "if (cell.guard == 0) {",
                     "construct_record_registry_a81b50(cell.registry);", "out.registry_initialised_now = true;",
                     "std::size_t grown = capacity >> 1;", "if (headroom < capacity) grown = 0;",
                     "if (grown < want) grown = want;", "alloc_17070(grown * sizeof(void*), alloc)",
                     "free_170b0(vec.begin, alloc).ok", "vec.cap = buffer + grown;",
                     "if (vec.end == vec.cap) {", "*vec.end = record;", "++vec.end;",
                     "push_back_a81840(cell.registry.first, &record, alloc)", "push_back_a81840(cell.registry.second, &record, alloc)",
                     "if (kVecMaxElements - size < n) {"],
        "test": ["const std::size_t want[] = {1, 2, 3, 4, 6, 9, 13, 19, 28, 42};", "CHECK(ctl == 6);",
                 "CHECK(rec.kind == 0x1d && rec.q28 == 0 && rec.b30 == 0 && rec.flag == 1);",
                 "CHECK(r3.ok && r3.new_capacity == 101 && g.last_size == 101 * 8);",
                 "refusal == RecordRegistryRefusal::no_allocator"],
        "cmake": ["core/helper_a81700_named_record.cpp", "add_test(NAME helper_a81700 COMMAND test_helper_a81700)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
