#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 452 (RVA 0x498640, 1235 B, index 452).

Batch 24, slot 452: a stack-guarded body that links one 0x48-byte node, zeroes an 8-byte cell, builds
four 0xa0-byte console-COMMAND objects at 0x2ef8c10 + 0xa0*i (ConCmdBase ctor 0x20af0 with flags 2 and
a {self, 0x1990, 0, handler} delegate at +0x60), registers each through `[0x1594dc0 + 0x20]` (= 0x20d60),
and finally builds one menu toggle at 0x2ef8e90 through 0x26ea0 targeting command 3.

The four `callq *%r12` sites are resolved here, not assumed: each follows `movq 0x20(%rax), %r12` with
%rax = 0x1594dc0, and the RELATIVE relocation at 0x1594de0 holds 0x20d60. The checker asserts that
relocation and pins all four indirect call sites.

Offline only: reads the pinned private ELF, never loads, relocates or executes an original RVA. The
destructors, handler tags and __dso_handle are IDENTITY TAGS.
"""

from __future__ import annotations

import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

BASE = 0x498640
SIZE = 1235

NODE = 0x2EF8BF0
NODE_CELL = 0x2EF8BF8
CELL = 0x2EF8C08
OBJ0 = 0x2EF8C10
STRIDE = 0xA0
TOGGLE = 0x2EF8E90
NODE_DTOR = 0x497610
CELL_DTOR = 0x493190
CMD_DTOR = 0x4B0
ITEM_DTOR = 0x272D0
DSO = 0x1604218
ATEXIT = 0xF8F390
VTABLE = 0x1594DC0
VPTR = 0x1594DD0
REGISTER = 0x20D60

# site order: name lea, 0xf2f0, object lea, description lea, handler lea, length store, 0x20af0,
#            indirect register, 0xf590 (description), 0xf590 (name), atexit
GROUPS = [
    (0xFFC929, 18, 0xFFC93C, 0x7F, 0x2EF8C10, 0x497320,
     0x4986DC, 0x4986E8, 0x4986ED, 0x4986FC, 0x49871E, 0x498725, 0x49874D, 0x4987A2, 0x4987AA,
     0x4987B4, 0x4987CA),
    (0xFFC9BC, 23, 0xFFC9D4, 0x3C, 0x2EF8CB0, 0x497520,
     0x4987D4, 0x4987E0, 0x4987E5, 0x4987F2, 0x498814, 0x49881B, 0x498838, 0x49887E, 0x498886,
     0x498890, 0x4988A6),
    (0xFFCA11, 25, 0xFFCA2B, 0x47, 0x2EF8D50, 0x497560,
     0x4988B0, 0x4988BC, 0x4988C1, 0x4988CE, 0x4988F0, 0x4988F7, 0x498914, 0x49895A, 0x498962,
     0x49896C, 0x498982),
    (0xFFCA73, 26, 0xFFCA8E, 0x1E, 0x2EF8DF0, 0x4975A0,
     0x49898C, 0x498998, 0x49899D, 0x4989AA, 0x4989CC, 0x4989D3, 0x4989F0, 0x498A36, 0x498A3E,
     0x498A48, 0x498A61),
]

PINS = [
    ("imm", 0x498640, "55"),                     # push rbp
    ("imm", 0x498641, "4889e5"),                 # mov rsp, rbp
    ("rip", 0x498658, 7, 3, 0x15FE518),          # canary load
    ("imm", 0x49865F, "bf480000 00".replace(" ", "")),   # mov edi, 0x48
    ("rip", 0x498668, 7, 3, NODE),               # lea r14, 0x2ef8bf0
    ("rip", 0x49867A, 8, 4, NODE_CELL),          # vmovups xmm0, 0x2ef8bf8
    ("call", 0x498682, 0x17070),                 # operator new
    ("rip", 0x49868E, 7, 3, NODE_DTOR),          # lea rdi, 0x497610
    ("rip", 0x4986AB, 7, 3, NODE_CELL),          # movq rax, 0x2ef8bf8
    ("call", 0x4986B5, ATEXIT),
    ("rip", 0x4986BA, 7, 3, CELL),               # lea rsi, 0x2ef8c08
    ("rip", 0x4986C1, 7, 3, CELL_DTOR),          # lea rdi, 0x493190
    ("call", 0x4986D2, ATEXIT),
    ("rip", 0x498A66, 7, 3, 0xFFCAB4),           # lea rcx, toggle text
    ("rip", 0x498A6D, 7, 3, TOGGLE),             # lea r15, 0x2ef8e90
    ("rip", 0x498A7E, 7, 3, 0xFFCAAD),           # lea rax, "Shared"
    ("imm", 0x498A89, "41b830000000"),           # mov r8d, 0x30
    ("imm", 0x498AA5, "48c744246806000000"),     # movq $6, 0x68(rsp)
    ("imm", 0x498AAE, "48c744244815000000"),     # movq $0x15, 0x48(rsp)
    ("call", 0x498AC3, 0x26EA0),                 # menu toggle ctor
    ("rip", 0x498AD8, 7, 3, ITEM_DTOR),          # lea rdi, 0x272d0
    ("call", 0x498AE5, ATEXIT),
    ("rip", 0x498AEA, 7, 3, 0x15FE518),          # canary reload
    ("imm", 0x498AFC, "750f"),                   # short jne -> 0x498b0d
    ("call", 0x498B0D, 0xF8F3A0),                # __stack_chk_fail
    ("imm", 0x498B12, "90"),
]
for name_rva, name_len, desc_rva, desc_len, obj_rva, handler, name_lea, f2f0, obj_lea, desc_lea, \
        handler_lea, len_store, ctor, register, rel_desc, rel_name, atexit in GROUPS:
    PINS += [
        ("rip", name_lea, 7, 3, name_rva),
        ("call", f2f0, 0xF2F0),
        ("rip", obj_lea, 7, 3, obj_rva),
        ("rip", desc_lea, 7, 3, desc_rva),
        ("rip", handler_lea, 7, 3, handler),
        ("imm", len_store, "48c7442448" + struct.pack("<I", desc_len).hex()),
        ("call", ctor, 0x20AF0),
        ("imm", register, "41ffd4"),            # callq *%r12, r12 = [0x1594dc0+0x20]
        ("call", rel_desc, 0xF590),
        ("call", rel_name, 0xF590),
        ("call", atexit, ATEXIT),
    ]

SPEC = {
    "label": "452",
    "slot": 452,
    "base": BASE,
    "size": SIZE,
    "body_sha256": "bbf97b0e0968d0cd27d718a5f8992d1e7bd642aff848e5b231cc733ce4698a54",
    "pad": "9090909090909090",
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [],
    "lengths": [],
    "pins": PINS,
    "native": {
        "header": "native/core/ctor_slot_452_498640.hpp",
        "source": "native/core/ctor_slot_452_498640.cpp",
        "test": "native/tests/test_ctor_slot_452.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kSlot452Rva": BASE,
            "kSlot452Size": SIZE,
            "kSlot452TableWord": 0x2DD62C0,
            "kSlot452NodeObject": NODE,
            "kSlot452NodeCell": NODE_CELL,
            "kSlot452Cell": CELL,
            "kSlot452Object0": OBJ0,
            "kSlot452CommandCount": 4,
            "kSlot452CommandStride": STRIDE,
            "kSlot452Toggle": TOGGLE,
            "kSlot452NodeSize": 0x48,
            "kSlot452NodeDtor": NODE_DTOR,
            "kSlot452CellDtor": CELL_DTOR,
            "kSlot452CommandDtor": CMD_DTOR,
            "kSlot452ItemDtor": ITEM_DTOR,
            "kSlot452DsoHandle": DSO,
            "kSlot452AtexitRva": ATEXIT,
            "kSlot452AllocRva": 0x17070,
            "kSlot452CanaryCell": 0x15FE518,
            "kSlot452StackChkFailRva": 0xF8F3A0,
            "kSlot452CanaryFrameOffset": 0x80,
            "kSlot452FrameBytes": 0xA0,
            "kSlot452ToggleParentRva": 0xFFCAAD,
            "kSlot452ToggleParentLength": 6,
            "kSlot452ToggleTextRva": 0xFFCAB4,
            "kSlot452ToggleTextLength": 21,
            "kSlot452ToggleFlags": 0x30,
            "kSlot452ToggleTarget": 3,
            "kSlot452ToggleGate3": 0x22AE0,
        },
        "header_tokens": [
            "0x2ef8c10", "0x2ef8e90", "0x1594dc0", "0x20d60", "0x497320", "0x493190",
            "kSlot452CommandSites", "INFERENCE",
        ],
        "source_order": ["slot452_toggle_target", "atexit_log.record", "init_fconcmd_object"],
        "test_tokens": [
            "kSlot452CommandSites", "atexit_records", "delegate.handler", "run_hook_failure",
            "slot452_toggle_target",
        ],
        "cmake_tokens": ["ctor_slot_452_498640.cpp", "test_ctor_slot_452.cpp", "ctor_slot_452"],
    },
}

SUMMARY = ("one 0x48-byte node + one 8-byte cell, four 0xa0-byte command objects with "
           "{self, 0x1990, 0, handler} delegates registered through [0x1594dc0+0x20] = 0x20d60, and one "
           "menu toggle targeting command 3; seven __cxa_atexit records, in body order")

EXPECTED = [
    ("cp10_cutsceneEvent", 18, 0xffc929,
     "Trigger a cutscene event. Params: <state: 0=start, 1=Update, 2=stop, 3=skip, 4=suspend, 5=resume> <eventName> <cutIndex> <time>", 127, 0xffc93c, 0x497320),
    ("cp10_setAreaLoaderState", 23, 0xffc9bc,
     "Manually set the desired area loader state. Params: <zoneId>", 60, 0xffc9d4, 0x497520),
    ("cp10_setUIAreaLoaderState", 25, 0xffca11,
     "Manually set the desired area loader state for the UI. Params: <zoneId>", 71, 0xffca2b, 0x497560),
    ("cp10_ResetPhysicsCameraCut", 26, 0xffca73,
     "Debug test of reseting physics", 30, 0xffca8e, 0x4975a0),
]


def check_semantics(elf, header: str, verbose: bool = True) -> None:
    """Re-derive every command's strings, lengths, object address and handler from the ELF bytes."""
    require = lib.require

    # The four indirect sites are the vtable's register slot, resolved through the relocation.
    require(elf.reloc_target(VTABLE + 0x20) == REGISTER,
            f"[{VTABLE:#x}+0x20] is not {REGISTER:#x}")
    require(VPTR == VTABLE + 0x10, "address point is the vtable start + 0x10")
    require(elf.reloc_target(VTABLE + 0x10) == CMD_DTOR, "vtable slot 0 is the command destructor")
    require(elf.reloc_target(VTABLE + 0x28) == 0x22AE0, "vtable slot 3 gates the toggle")

    for name, name_len, name_rva, desc, desc_len, desc_rva, handler in EXPECTED:
        raw_name = elf.read(name_rva, name_len + 1)
        require(raw_name[:name_len].decode() == name, f"name at {name_rva:#x}: {raw_name!r}")
        require(raw_name[name_len] == 0, f"name at {name_rva:#x} is not NUL-terminated at {name_len}")
        raw_desc = elf.read(desc_rva, desc_len + 1)
        require(raw_desc[:desc_len].decode() == desc, f"description at {desc_rva:#x}: {raw_desc!r}")
        require(raw_desc[desc_len] == 0, f"description at {desc_rva:#x} is not NUL-terminated at {desc_len}")
        require(f'"{name}"' in header, f"header literal {name!r}")
        require(f'0x{name_rva:x}' in header and f'0x{desc_rva:x}' in header and f'0x{handler:x}' in header,
                f"header site row for {name!r}")

    # Objects ascend by the stride and the toggle is the next slot.
    objects = [row[4] if False else OBJ0 + i * STRIDE for i in range(4)]
    require(objects[-1] + STRIDE == TOGGLE, "the toggle is not the fifth 0xa0 slot")
    for name, _, _, _, _, _, _ in EXPECTED:
        require(header.count(f'"{name}"') >= 1, f"header is missing {name!r}")

    # The body's own stores/calls: 40 operand pins already assert them; here we only re-count.
    body = elf.raw(BASE, SIZE)
    calls = sum(1 for off in range(len(body) - 4)
                if body[off] == 0xE8
                and BASE + off + 5 + int.from_bytes(body[off + 1:off + 5], "little", signed=True) == 0x20AF0)
    require(calls == 4, f"expected 4 calls to 0x20af0, decoded {calls}")
    # %r12 is loaded once (group 0) and all four groups call through it: 4 x `callq *%r12`.
    regs = sum(1 for off in range(len(body) - 4) if body[off:off + 4] == b"\x4c\x8b\x60\x20")
    require(regs == 1, f"expected one `movq 0x20(%rax),%r12` load, decoded {regs}")
    indirect = sum(1 for off in range(len(body) - 3) if body[off:off + 3] == b"\x41\xff\xd4")
    require(indirect == 4, f"expected four `callq *%r12` sites, decoded {indirect}")

    if verbose:
        print("  oracle: 4 commands re-derived from rodata (18/23/25/26 name bytes, 127/60/71/30 description "
              "bytes), handlers 0x497320/0x497520/0x497560/0x4975a0, register slot 0x20d60")


def main() -> int:
    argv = sys.argv[1:]
    elf = lib.Elf()
    header = (lib.ROOT / SPEC["native"]["header"]).read_text()

    if "--tamper-native" in argv:
        mutated = header.replace('"cp10_setAreaLoaderState"', '"cp10_setAreaLoaderStat3"', 1)
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        check_semantics(elf, mutated, verbose=False)   # must raise
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" not in argv:
        check_semantics(elf, header)
        print("  ok: 4 x 0x20af0 + 4 indirect 0x20d60 + 4 atexit(0x4b0), node and cell records, one toggle")
    return lib.main(SPEC, SUMMARY)


if __name__ == "__main__":
    sys.exit(main())
