#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 454 (RVA 0x483e40, 1,431 B, index 454).

Batch 26. The body builds the network debug page: a 0xd8-byte ranged variable at 0x2ef85d8
(vptr 0x1594c08) named network_LocalPlayerId with the value cell at +0xd0, two 0xc8-byte bool
variables at 0x2ef86b0 / 0x2ef8778 (vptr 0x15940b0, value at +0xc0, tag 0xc2a0), four 0x38-byte
"Client:" records at 0x2ef8840, two RB trees at 0x2ef8920 / 0x2ef8938, a 0x20-byte
SServerObjectProperties view at 0x2ef8950 and a 0x1a8-header tree at 0x2ef8970.

Eight __cxa_atexit records in body order: 0x18a60, 0x20b0, 0x20b0, 0x482920 (the four-record
array), 0x47e0f0 (tree A), 0x47e0f0 (tree B), 0xf7b0 (properties view), 0x482f50 (tree C).
0x47e0f0 reaches the RB erase modelled in core/rb_tree_482a90; this slot's trees are empty.

Offline only: reads the pinned private ELF, never executes an original RVA. Destructors, vtable
slots and tags are identity values.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

BASE = 0x483E40
SIZE = 1431
ATEXIT = 0xF8F390

# Every e8 in the body, in order (site -> target). The last is __stack_chk_fail.
CALLS = [
    (0x483F1E, 0x3A980), (0x483F26, 0xF590), (0x483F57, 0x20D60), (0x483F5F, 0xF590),
    (0x483F75, ATEXIT), (0x483FE5, 0x20AF0), (0x484047, 0xF590), (0x484064, 0x20D60),
    (0x48406C, 0xF590), (0x484082, ATEXIT), (0x4840E7, 0x20AF0), (0x48413E, 0xF590),
    (0x48415B, 0x20D60), (0x484163, 0xF590), (0x48417C, ATEXIT), (0x4841B6, 0xA81700),
    (0x4841BE, 0xF590), (0x4841F8, 0xA81700), (0x484200, 0xF590), (0x48423A, 0xA81700),
    (0x484242, 0xF590), (0x484277, 0xA81700), (0x48427F, 0xF590), (0x484294, ATEXIT),
    (0x4842B1, 0x17070), (0x4842E0, ATEXIT), (0x4842FD, 0x17070), (0x484325, ATEXIT),
    (0x484360, ATEXIT), (0x48437D, 0x17070), (0x4843A9, ATEXIT), (0x4843D1, 0xF8F3A0),
]

OBJECT_LEAS = [
    (0x483E71, 0x2EF85D8), (0x483F88, 0x2EF86B0), (0x484087, 0x2EF8778),
    (0x484181, 0x2EF8840), (0x4841D3, 0x2EF8878), (0x484215, 0x2EF88B0),
    (0x484256, 0x2EF88E8), (0x4842A2, 0x2EF8920), (0x4842EE, 0x2EF8938),
    (0x484338, 0x2EF8950), (0x48436E, 0x2EF8970),
]

PINS = [("imm", BASE, "55"), ("imm", 0x4843D1, "e8")]
for site, target in CALLS:
    PINS.append(("call", site, target))
for site, target in OBJECT_LEAS:
    PINS.append(("rip", site, 7, 3, target))
PINS += [
    ("rip", 0x483E5F, 7, 3, 0x1594BF8),   # local-player vtable lea
    ("rip", 0x483F7A, 7, 3, 0x1594208),   # bool pre-vptr store
    ("rip", 0x483FEA, 7, 3, 0x15940A0),   # bool vtable lea
    ("rip", 0x483FFF, 7, 3, 0x15BE810),   # bool functor reloc cell
    ("rip", 0x4840FA, 7, 3, 0x15BE810),
]

SPEC = {
    "label": "454",
    "slot": 454,
    "base": BASE,
    "size": SIZE,
    "body_sha256": "88217ac442e8f020453863f84a4be06faa7f6590a4820cf25222603949ba2fd5",
    "pad": "90" * 9,
    "callees": ["concmd_base_ctor", "concmd_register", "release_f590"],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
        (0xFFA960, b"network_LocalPlayerId"),
        (0xFFA976, b"network_Interpolation"),
        (0xFFA98C, b"network_ShowObjectBandwidth"),
        (0xFFA9A8, b"SServerObjectProperties"),
        (0xFFB0DF, b"Client: object count"),
        (0xFFB0F4, b"Client: objects created this frame"),
        (0xFFB117, b"Client: objects destroyed this frame"),
        (0xFFB13C, b"Client: updates failed"),
        (0xFFB16F, b""),
    ],
    "lengths": [],
    "pins": PINS,
    "native": {
        "header": "native/core/ctor_slot_454_483e40.hpp",
        "source": "native/core/ctor_slot_454_483e40.cpp",
        "test": "native/tests/test_ctor_slot_454.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kSlot454Rva": BASE,
            "kSlot454Size": SIZE,
            "kSlot454TableWord": 0x2DD62B0,
            "kSlot454LocalPlayer": 0x2EF85D8,
            "kSlot454Interpolation": 0x2EF86B0,
            "kSlot454ShowBandwidth": 0x2EF8778,
            "kSlot454Record0": 0x2EF8840,
            "kSlot454RecordCount": 4,
            "kSlot454RecordStride": 0x38,
            "kSlot454TreeA": 0x2EF8920,
            "kSlot454TreeB": 0x2EF8938,
            "kSlot454TreeC": 0x2EF8970,
            "kSlot454Properties": 0x2EF8950,
            "kSlot454LocalPlayerVptr": 0x1594C08,
            "kSlot454LocalPlayerFunctorVptr": 0x1595660,
            "kSlot454BoolVptr": 0x15940B0,
            "kSlot454BoolFnTagValue": 0xC2A0,
            "kSlot454LocalPlayerDtor": 0x18A60,
            "kSlot454BoolDtor": 0x20B0,
            "kSlot454RecordsDtor": 0x482920,
            "kSlot454TreeDtorA": 0x47E0F0,
            "kSlot454TreeDtorB": 0x482F50,
            "kSlot454ViewDtor": 0xF7B0,
            "kSlot454AtexitRva": ATEXIT,
            "kSlot454CanaryCell": 0x15FE518,
            "kSlot454StackChkFailRva": 0xF8F3A0,
            "kSlot454PropertiesLength": 0x17,
        },
        "header_tokens": [
            "0x483e40", "0x2ef85d8", "0x2ef86b0", "0x2ef8778", "0x2ef8840",
            "0x1594c08", "0x15940b0", "0x482a90", "0x482920", "0x47e0f0", "0x482f50",
            "INFERENCE",
        ],
        "source_order": ["rb_tree_init", "family_view", "init_concmd_base_20af0", "register_concmd_20d60"],
        "test_tokens": ["kSlot454RecordText", "functor_transfers", "atexit_log.at", "rb_tree_release"],
        "cmake_tokens": ["rb_tree_482a90.cpp", "ctor_slot_454_483e40.cpp", "test_ctor_slot_454.cpp"],
    },
}

SUMMARY = ("a 0xd8-byte ranged variable (vptr 0x1594c08, value at +0xd0), two 0xc8-byte bool "
           "variables (vptr 0x15940b0), four 0x38-byte records, two 0x38-header RB trees plus a "
           "0x1a8-header tree, a 0x20-byte SServerObjectProperties view, and eight __cxa_atexit "
           "records (0x18a60, 0x20b0 x2, 0x482920, 0x47e0f0 x2, 0xf7b0, 0x482f50)")


def check_semantics(elf, header, verbose=True):
    require = lib.require
    body = elf.raw(BASE, SIZE)
    seen = []
    for i in range(len(body) - 5):
        if body[i] == 0xE8:
            target = BASE + i + 5 + int.from_bytes(body[i + 1:i + 5], "little", signed=True)
            seen.append((BASE + i, target))
    require(seen == CALLS, f"call sequence: {[(hex(a), hex(b)) for a, b in seen]}")
    require(sum(1 for _, target in seen if target == ATEXIT) == 8, "eight __cxa_atexit records")
    require(sum(1 for _, target in seen if target == 0x17070) == 3, "three header allocations")
    require(sum(1 for _, target in seen if target == 0xA81700) == 4, "four records")
    require(sum(1 for _, target in seen if target == 0x20D60) == 3, "three registrations")

    require(elf.u64(0x2DD70E0 - 8 * 454) == BASE, "constructor table word 454")
    for rva, text in SPEC["strings"]:
        raw = elf.read(rva, len(text))
        require(raw == text, f"rodata at {rva:#x}")
    require(elf.read(0xFFB16F, 1) == b"\x00", "empty description string")
    if verbose:
        print("  oracle: 32 call sites in body order, 8 atexit rows, 3 x 0x17070, 4 x 0xa81700, "
              "3 x 0x20d60; strings re-derived from rodata")


def main() -> int:
    argv = sys.argv[1:]
    elf = lib.Elf()
    header = (lib.ROOT / SPEC["native"]["header"]).read_text()

    if "--tamper-native" in argv:
        mutated = header.replace('"SServerObjectProperties"', '"SServerObjectPropertie5"')
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        lib.check_native(SPEC, {"header": mutated, "source": (lib.ROOT / SPEC["native"]["source"]).read_text(),
                                "test": (lib.ROOT / SPEC["native"]["test"]).read_text(),
                                "cmake": (lib.ROOT / SPEC["native"]["cmake"]).read_text()})
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" not in argv:
        check_semantics(elf, header)
        print("  ok: 3 commands, 4 records, 3 trees, 8 atexit rows")
    return lib.main(SPEC, SUMMARY)


if __name__ == "__main__":
    sys.exit(main())
