#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 495 (RVA 0x268ee0, 1698 B, index 495).

Batch 25. The body builds a debug page: a 0xc0-byte console VARIABLE at 0x2eeebb0 (vtable
0x15940a0 / address point 0x15940b0, slot 2 = 0x20d60, flags 1), a parsed blob record at 0x2eeeca8
filled by 0xa38960 from the two-dword blob at 0x1604934, and twelve 0x48-byte NET_ERROR_* item
records at 0x2eeecd8 + 0x48*i. Fourteen __cxa_atexit records: 0x20b0, 0x209d80, 0x268e20 x 12.

Offline only: the checker reads the pinned private ELF, never loads or executes an original RVA.
The destructors, the vtable slots and the +0xb0 function pointer are IDENTITY TAGS.
"""

from __future__ import annotations

import struct
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

BASE = 0x268EE0
SIZE = 1698
CMD = 0x2EEEBB0
REGION = 0x2EEEC70
BLOB = 0x2EEECA8
ITEM0 = 0x2EEECD8
STRIDE = 0x48
COUNT = 12
VPTR = 0x15940B0
NAME_RVA = 0xFC4960
DESC_RVA = 0xFC4B40
BLOB_RVA = 0x1604934
CMD_DTOR = 0x20B0
BLOB_DTOR = 0x209D80
ITEM_DTOR = 0x268E20
ATEXIT = 0xF8F390

ITEMS = [
    (0x2EEECD8, 0xFC497F, 35, "NET_ERROR_CONNECTIONMALFORMEDPACKET"),
    (0x2EEED20, 0xFC49A3, 37, "NET_ERROR_CONNECTIONGAMESTATEOVERFLOW"),
    (0x2EEED68, 0xFC49C9, 36, "NET_ERROR_CONNECTIONCOMPRESSIONERROR"),
    (0x2EEEDB0, 0xFC49EE, 38, "NET_ERROR_CONNECTIONDECOMPRESSIONERROR"),
    (0x2EEEDF8, 0xFC4A15, 31, "NET_ERROR_CONNECTIONSOCKETERROR"),
    (0x2EEEE40, 0xFC4A35, 27, "NET_ERROR_CONNECTIONTIMEOUT"),
    (0x2EEEE88, 0xFC4A51, 37, "NET_ERROR_CONNECTIONLOGGEDINELSEWHERE"),
    (0x2EEEED0, 0xFC4A77, 29, "NET_ERROR_CONNECTIONAUTOCLOSE"),
    (0x2EEEF18, 0xFC4A95, 40, "NET_ERROR_CONNECTIONAUTHORIZATIONFAILURE"),
    (0x2EEEF60, 0xFC4ABE, 42, "NET_ERROR_NETWORKSCENESERIALIZATIONFAILURE"),
    (0x2EEEFA8, 0xFC4AE9, 44, "NET_ERROR_NETWORKSCENEDESERIALIZATIONFAILURE"),
    (0x2EEEFF0, 0xFC4B16, 30, "NET_ERROR_NETWORKSCENESHUTDOWN"),
]

# per-item f590 and atexit call sites, in body order (from the instruction listing)
ITEM_F590 = [0x26908C, 0x269107, 0x269176, 0x2691E5, 0x269254, 0x2692C3,
             0x269325, 0x269394, 0x269403, 0x269472, 0x2694E1, 0x269550]
ITEM_ATEXIT = [0x2690A1, 0x269115, 0x269184, 0x2691F3, 0x269262, 0x2692D1,
               0x269333, 0x2693A2, 0x269411, 0x269480, 0x2694EF, 0x26955E]

PINS = [
    ("imm", 0x268EE0, "55"),                          # push rbp
    ("imm", 0x268EE1, "4889e5"),                      # mov rsp, rbp
    ("rip", 0x268EF1, 7, 3, 0x15FE518),               # stack cookie
    ("rip", 0x268F06, 7, 3, NAME_RVA),                # lea rsi, name
    ("rip", 0x268F0D, 7, 3, DESC_RVA),                # lea rdx, description
    ("rip", 0x268F54, 7, 3, CMD),                     # movq rbx, 0x2eeebb0 (pre-ctor vptr)
    ("call", 0x268F63, 0x20AF0),                      # ConCmdBase ctor
    ("rip", 0x268F88, 7, 3, CMD),                     # vptr 0x15940b0
    ("call", 0x268FC5, 0xF590),                       # release description
    ("call", 0x268FE2, 0x20D60),                      # register
    ("call", 0x268FEA, 0xF590),                       # release name
    ("call", 0x269003, ATEXIT),                       # atexit(0x20b0, command)
    ("rip", 0x269008, 7, 3, BLOB),                    # lea rbx, parsed record
    ("call", 0x269019, 0xA38960),                     # parse
    ("call", 0x26902B, ATEXIT),                       # atexit(0x209d80, record)
    ("rip", 0x269030, 7, 3, ITEM0),                   # first item
    ("rip", 0x2694F4, 7, 3, ITEM0 + 11 * STRIDE),     # last item
    ("call", 0x26957C, 0xF8F3A0),                     # __stack_chk_fail
    ("imm", 0x269581, "90"),                          # trailing FDE pad byte inside the body
]
for site in ITEM_F590:
    PINS.append(("call", site, 0xF590))
for site in ITEM_ATEXIT:
    PINS.append(("call", site, ATEXIT))

SPEC = {
    "label": "495",
    "slot": 495,
    "base": BASE,
    "size": SIZE,
    "body_sha256": "3ad3ff23056218d095a0a32184ea0644953a9a9929b1df8eba99a361f4715fee",
    "pad": "90" * 14,
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [],
    "lengths": [],
    "pins": PINS,
    "native": {
        "header": "native/core/ctor_slot_495_268ee0.hpp",
        "source": "native/core/ctor_slot_495_268ee0.cpp",
        "test": "native/tests/test_ctor_slot_495.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kSlot495Rva": BASE,
            "kSlot495Size": SIZE,
            "kSlot495TableWord": 0x2DD6168,
            "kSlot495Command": CMD,
            "kSlot495Region": REGION,
            "kSlot495BlobRecord": BLOB,
            "kSlot495Item0": ITEM0,
            "kSlot495ItemCount": COUNT,
            "kSlot495ItemStride": STRIDE,
            "kSlot495CommandSize": 0xC0,
            "kSlot495Vtable": 0x15940A0,
            "kSlot495Vptr": VPTR,
            "kSlot495PreVptr": 0x1594218,
            "kSlot495TagB0": 0x159C9A8,
            "kSlot495TagB0Value": 0xC2A0,
            "kSlot495Flags": 1,
            "kSlot495CommandDtor": CMD_DTOR,
            "kSlot495BlobDtor": BLOB_DTOR,
            "kSlot495ItemDtor": ITEM_DTOR,
            "kSlot495DsoHandle": 0x1604218,
            "kSlot495AtexitRva": ATEXIT,
            "kSlot495CanaryCell": 0x15FE518,
            "kSlot495StackChkFailRva": 0xF8F3A0,
            "kSlot495NameRva": NAME_RVA,
            "kSlot495NameLength": 30,
            "kSlot495DescriptionRva": DESC_RVA,
            "kSlot495DescriptionLength": 0,
            "kSlot495BlobRva": BLOB_RVA,
            "kSlot495BlobA": 0x20,
            "kSlot495BlobSize": 0x20,
            "kSlot495BlobBits": 2,
            "kSlot495BlobBytes": 1,
            "kSlot495BlobTotal": 0x21,
        },
        "header_tokens": [
            "0x268ee0", "0x2eeebb0", "0x15940b0", "0x20d60", "0xa38960", "0x2eeecd8",
            "0xbeefbeef", "INFERENCE",
        ],
        "source_order": ["family_view", "init_concmd_base_20af0", "parse_blob_a38960"],
        "test_tokens": ["kSlot495Items", "atexit_records", "blob_parsed", "run_hook_refusal"],
        "cmake_tokens": ["ctor_slot_495_268ee0.cpp", "test_ctor_slot_495.cpp", "ctor_slot_495"],
    },
}

SUMMARY = ("a 0xc0-byte console variable (vptr 0x15940b0, register through 0x20d60), a 0x28-byte blob "
           "record parsed by 0xa38960 from the two dwords at 0x1604934, twelve 0x48-byte NET_ERROR_* "
           "items, and fourteen __cxa_atexit records (0x20b0, 0x209d80, 0x268e20 x 12)")

K_MAX_DEPTH = 64
K_MAX_DWORDS = 4096


def validate(blob, i, status, depth):
    if depth > K_MAX_DEPTH:
        return False, i
    while True:
        if i >= len(blob):
            return False, i
        tag = blob[i]; i += 1
        low = tag & 0xFFFF
        kind = tag & 0xF0000000
        if kind in (0x00000000, 0x10000000):
            status[0] = 1 if (status[0] and low) else 0
        elif kind == 0x20000000:
            sub = [1, 0]
            ok, i = validate(blob, i, sub, depth + 1)
            if not ok:
                return False, i
            status[0] = 1 if (status[0] and low and sub[0]) else 0
        elif kind == 0x30000000:
            status[0] = 1 if (status[0] and (status[1] ^ 1)) else 0
        elif tag == 0xBEEFBEEF:
            status[0] = 1 if (status[0] and status[1]) else 0
        else:
            status[0] = 0
        if tag in (0x30000000, 0xBEEFBEEF):
            return True, i


def accumulate(blob, i, acc, depth):
    """acc = [a, b, c]; the caller starts b at 1 (the `movq $0x100000000` staging)."""
    if depth > K_MAX_DEPTH:
        return False, i
    while True:
        if i >= len(blob):
            return False, i
        tag = blob[i]; i += 1
        low = tag & 0xFFFF
        kind = tag & 0xF0000000
        if kind == 0x20000000:
            sub = [0, 0, 0]
            ok, i = accumulate(blob, i, sub, depth + 1)
            if not ok:
                return False, i
            acc[1] = 1 + sub[1] * low + acc[1]
            acc[0] += sub[0] * low
            acc[2] += low * sub[2]
        elif kind == 0x10000000:
            acc[1] += 1
            acc[0] += low
        elif kind == 0x00000000:
            acc[2] += low
        if tag in (0x30000000, 0xBEEFBEEF):
            return True, i


def parse(blob):
    status = [1, 1]
    ok, _ = validate(blob, 0, status, 0)
    if not ok:
        return None
    valid = 1 if status[0] else 0
    a = [0, 1, 0]
    ok, _ = accumulate(blob, 0, a, 0)
    if not ok:
        return None
    out = {"valid": valid, "a": (a[2] + a[0]) & 0xFFFFFFFF}
    b = [0, 1, 0]
    ok, _ = accumulate(blob, 0, b, 0)
    if not ok:
        return None
    out["size"] = b[0] & 0xFFFFFFFF
    c = [0, 1, 0]
    ok, _ = accumulate(blob, 0, c, 0)
    if not ok:
        return None
    out["bits"] = c[1] & 0xFFFFFFFF
    out["bytes"] = (out["bits"] + 7) >> 3
    out["total"] = (out["size"] + out["bytes"]) & 0xFFFFFFFF
    out["data"] = None
    out["released_first"] = False
    if valid:
        try:
            sentinel = blob.index(0xBEEFBEEF)
        except ValueError:
            return None
        count = sentinel + 1
        if count > K_MAX_DWORDS:
            return None
        out["data"] = blob[:count]
        out["released_first"] = True
    return out


def rip_leas(body, base):
    out = {}
    i = 0
    while i + 7 <= len(body):
        if body[i] in (0x48, 0x4C, 0x4D) and body[i + 1] == 0x8D and (body[i + 2] & 0xC7) == 0x05:
            disp = int.from_bytes(body[i + 3:i + 7], "little", signed=True)
            out[base + i] = base + i + 7 + disp
            i += 7
        else:
            i += 1
    return out


def check_semantics(elf, header, verbose=True):
    require = lib.require

    body = elf.raw(BASE, SIZE)
    leas = rip_leas(body, BASE)
    targets = Counter(leas.values())
    require(targets[NAME_RVA] >= 1, "name view lea")
    require(targets[DESC_RVA] >= 1, "description view lea")
    require(targets[CMD] == 1, "command object lea")
    require(targets[BLOB] == 1, "parsed record lea")
    require(targets[BLOB_RVA] == 1, "blob source lea")
    require(targets[CMD_DTOR] == 1 and targets[BLOB_DTOR] == 1, "the two head destructor leas")
    require(targets[ITEM_DTOR] == 1, "the item destructor is loaded once and reused by all 12 records")
    require(targets[0x15940A0] == 1, "vtable lea (the address point is +0x10)")

    # Length immediates: the name's 0x1e, then the twelve item lengths in body order.
    lens = []
    i = 0
    while i + 8 <= len(body):
        if body[i:i + 3] == b"\x48\xc7\x45" and body[i + 4:i + 8] != b"":
            if body[i + 3] == 0xB8:       # movq $imm, -0x48(%rbp)
                lens.append(int.from_bytes(body[i + 4:i + 8], "little"))
                i += 8
                continue
            if body[i + 3] == 0x98:       # movq $imm, -0x68(%rbp)
                require(int.from_bytes(body[i + 4:i + 8], "little") == 30, "name length immediate")
                i += 8
                continue
        i += 1
    require(lens == [spec[2] for spec in ITEMS], f"item length immediates: {lens}")

    # Strings, re-derived from rodata.
    raw = elf.read(NAME_RVA, 31)
    require(raw[:30].decode() == "network_AllowCrossPlatformPlay" and raw[30] == 0, "command name")
    require(elf.read(DESC_RVA, 1) == b"\x00", "empty description")
    for obj, rva, length, text in ITEMS:
        raw = elf.read(rva, length + 1)
        require(raw[:length].decode() == text, f"item text at {rva:#x}")
        require(raw[length] == 0, f"item text at {rva:#x} is not NUL-terminated at {length}")
        require(f'"{text}"' in header, f"header literal {text!r}")
        require(hex(rva)[2:] in header.replace("0X", "0x"), f"header RVA {rva:#x}")

    # The static blob and the values the parser derives from it.
    blob = list(struct.unpack("<2I", elf.read(BLOB_RVA, 8)))
    require(blob == [0x10000020, 0xBEEFBEEF], f"blob bytes {blob}")
    parsed = parse(blob)
    require(parsed is not None, "slot blob does not parse")
    expected = {"valid": 1, "a": 0x20, "size": 0x20, "bits": 2, "bytes": 1,
                "total": 0x21, "data": [0x10000020, 0xBEEFBEEF], "released_first": True}
    require(parsed == expected, f"parse mismatch: {parsed}")

    # Boundary vectors: leaf, nested group (invalid), end marker.
    leaf = parse([0x10000003, 0xBEEFBEEF])
    require(leaf["valid"] == 1 and leaf["bits"] == 2 and leaf["total"] == 4, f"leaf {leaf}")
    group = parse([0x20000002, 0x10000001, 0x30000000, 0xBEEFBEEF])
    require(group["valid"] == 1 and group["bits"] == 4 and group["total"] == 3, f"group {group}")
    require(parse([0x20000002, 0x10000001, 0xBEEFBEEF]) is None, "sub-run ended by the terminator")
    end = parse([0x30000000, 0xBEEFBEEF])
    require(end["valid"] == 0 and end["bits"] == 1 and end["total"] == 1, f"end {end}")

    if verbose:
        print("  oracle: name/description and 12 item strings re-derived from rodata; blob "
              "0x10000020/0xbeefbeef -> valid 1, a 0x20, size 0x20, bits 2, total 0x21")


def main() -> int:
    argv = sys.argv[1:]
    elf = lib.Elf()
    header = (lib.ROOT / SPEC["native"]["header"]).read_text()

    if "--tamper-native" in argv:
        mutated = header.replace('"NET_ERROR_CONNECTIONTIMEOUT"', '"NET_ERROR_CONNECTIONTIMEOU7"', 1)
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        check_semantics(elf, mutated, verbose=False)   # must raise
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" not in argv:
        check_semantics(elf, header)
        print("  ok: 1 x 0x20af0 + 1 x 0x20d60 + 1 x 0xa38960, 12 items, 14 atexit rows")
    return lib.main(SPEC, SUMMARY)


if __name__ == "__main__":
    sys.exit(main())
