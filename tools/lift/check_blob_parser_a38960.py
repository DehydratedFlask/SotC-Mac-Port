#!/usr/bin/env python3
"""ELF byte-oracle for the tagged-blob reader 0xa38960 and its helpers 0xa39eb0 / 0xa39fd0.

Batch 25. The parser validates a tag stream, accumulates {a,b,c} over it three times, derives the
record at +0x00..+0x14, allocates a first refcount, copies the stream through its 0xbeefbeef
terminator when the flag is set, and releases the first refcount again. The port lives in
native/core/blob_parser_a38960.{hpp,cpp}; this checker re-derives every stored value from the ELF
and cross-checks an independent Python port against the fixture's expectations.

Offline only: reads the pinned private ELF, never executes an original RVA.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

BASE = 0xA38960
SIZE = 0x1B4
VALIDATE = 0xA39EB0
VALIDATE_SIZE = 0x120
ACCUMULATE = 0xA39FD0
ACCUMULATE_SIZE = 0xF0
SENTINEL = 0xBEEFBEEF
MAX_DEPTH = 64
MAX_DWORDS = 4096
VALIDATE_SHA = "96b5e7073315ba94cafcfcbb6e193a3ddf39bf8d008315c719d450410bc2dca1"
ACCUMULATE_SHA = "50a54c41c980417a22ea5c0c1dd55a0efb1687d20c93dd6c7cc14ce76fd52816"

PINS = [
    ("imm", BASE, "55"),                               # push rbp
    ("imm", BASE + 1, "4889e5"),                       # mov rsp, rbp
    ("rip", 0xA38971, 7, 3, 0x15FE518),                # stack cookie
    ("call", 0xA38998, VALIDATE),                      # status walk
    ("call", 0xA389C6, ACCUMULATE),                    # first pass
    ("call", 0xA389EC, ACCUMULATE),                    # second pass
    ("call", 0xA38A0F, ACCUMULATE),                    # third pass
    ("call", 0xA38A3A, 0x17090),                       # first refcount new[4]
    ("call", 0xA38A77, 0x17090),                       # payload new[4 * count]
    ("call", 0xA38AAC, 0xF8F470),                      # memcpy
    ("call", 0xA38AB6, 0x17090),                       # second refcount new[4]
    ("jmp", 0xA38B09, 0x170C0),                        # delete[] first refcount (tail)
    ("call", 0xA38B0E, 0xF8F3A0),                      # stack-chk-fail
    ("rip", 0xA39EC1, 7, 3, 0x15FE518),                # validator cookie
    ("call", 0xA39F4C, VALIDATE),                      # group recursion
    ("call", 0xA39FBC, 0xF8F3A0),
    ("rip", 0xA39FE1, 7, 3, 0x15FE518),                # accumulator cookie
    ("call", 0xA3A045, ACCUMULATE),                    # group recursion
    ("call", 0xA3A0BA, 0xF8F3A0),
]

SPEC = {
    "label": "blob_parser_a38960",
    "slot": 0,
    "no_table": True,          # a shared callee, not a constructor-table entry
    "base": BASE,
    "size": SIZE,
    "body_sha256": "1c62b18db8cec908be5ef3e3ba8832f5e6d872f1b382e35c892b4481b8b2b234",
    "pad": "90" * 12,
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [],
    "lengths": [],
    "pins": PINS,
    "native": {
        "header": "native/core/blob_parser_a38960.hpp",
        "source": "native/core/blob_parser_a38960.cpp",
        "test": "native/tests/test_blob_parser_a38960.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kBlobValidateRva": VALIDATE,
            "kBlobValidateSize": VALIDATE_SIZE,
            "kBlobAccumulateRva": ACCUMULATE,
            "kBlobAccumulateSize": ACCUMULATE_SIZE,
            "kBlobParseRva": BASE,
            "kBlobParseSize": SIZE,
            "kBlobSentinel": SENTINEL,
            "kBlobKindMask": 0xF0000000,
            "kBlobCountMask": 0xFFFF,
            "kBlobKindElement": 0,
            "kBlobKindLeaf": 0x10000000,
            "kBlobKindGroup": 0x20000000,
            "kBlobKindEnd": 0x30000000,
            "kBlobMaxDepth": MAX_DEPTH,
            "kBlobMaxDwords": MAX_DWORDS,
        },
        "header_tokens": [
            "0xa38960", "0xa39eb0", "0xa39fd0", "0xbeefbeef", "0xf8f470", "0x28",
        ],
        "source_order": ["blob_validate", "blob_accumulate", "parse_blob_a38960"],
        "test_tokens": ["slot_blob", "group_blob", "depth_cap", "allocator_hooks"],
        "cmake_tokens": ["blob_parser_a38960.cpp", "test_blob_parser_a38960.cpp", "blob_parser_a38960"],
    },
}

SUMMARY = ("0xa38960 (436 B) + validator 0xa39eb0 + accumulator 0xa39fd0: three accumulation passes "
           "from {0,1,0}, a first refcount always released, payload copied through the 0xbeefbeef "
           "sentinel, recursion capped at 64, stream capped at 4096 dwords")

EXPECTED = {
    "slot": {"valid": 1, "a": 0x20, "size": 0x20, "bits": 2, "bytes": 1, "total": 0x21,
             "data": [0x10000020, SENTINEL], "released_first": True},
    "leaf": {"valid": 1, "a": 3, "size": 3, "bits": 2, "bytes": 1, "total": 4,
             "data": [0x10000003, SENTINEL], "released_first": True},
    "group": {"valid": 1, "a": 2, "size": 2, "bits": 4, "bytes": 1, "total": 3,
              "data": [0x20000002, 0x10000001, 0x30000000, SENTINEL], "released_first": True},
    "end": {"valid": 0, "a": 0, "size": 0, "bits": 1, "bytes": 1, "total": 1,
            "data": None, "released_first": False},
}


def validate(blob, i, status, depth):
    if depth > MAX_DEPTH:
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
        elif tag == SENTINEL:
            status[0] = 1 if (status[0] and status[1]) else 0
        else:
            status[0] = 0
        if tag in (0x30000000, SENTINEL):
            return True, i


def accumulate(blob, i, acc, depth):
    if depth > MAX_DEPTH:
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
        if tag in (0x30000000, SENTINEL):
            return True, i


def parse(blob):
    m = {"valid": 0, "a": 0, "size": 0, "bits": 0, "bytes": 0, "total": 0,
         "data": None, "released_first": False}
    status = [1, 1]
    ok, _ = validate(blob, 0, status, 0)
    if not ok:
        return None
    m["valid"] = 1 if status[0] else 0
    a = [0, 1, 0]
    ok, _ = accumulate(blob, 0, a, 0)
    if not ok:
        return None
    m["a"] = (a[2] + a[0]) & 0xFFFFFFFF
    b = [0, 1, 0]
    ok, _ = accumulate(blob, 0, b, 0)
    if not ok:
        return None
    m["size"] = b[0] & 0xFFFFFFFF
    c = [0, 1, 0]
    ok, _ = accumulate(blob, 0, c, 0)
    if not ok:
        return None
    m["bits"] = c[1] & 0xFFFFFFFF
    m["bytes"] = (m["bits"] + 7) >> 3
    m["total"] = (m["size"] + m["bytes"]) & 0xFFFFFFFF
    if m["valid"]:
        try:
            sentinel = blob.index(SENTINEL)
        except ValueError:
            return None
        count = sentinel + 1
        if count > MAX_DWORDS:
            return None
        m["data"] = blob[:count]
        m["released_first"] = True
    return m


def call_at(elf, addr):
    raw = elf.raw(addr, 5)
    lib.require(raw[0] == 0xE8, f"no call at {addr:#x}")
    return addr + 5 + int.from_bytes(raw[1:5], "little", signed=True)


def jmp_at(elf, addr):
    raw = elf.raw(addr, 5)
    lib.require(raw[0] == 0xE9, f"no jmp at {addr:#x}")
    return addr + 5 + int.from_bytes(raw[1:5], "little", signed=True)


def check_semantics(elf, header, verbose=True):
    require = lib.require

    require(hashlib.sha256(elf.raw(BASE, SIZE)).hexdigest() == SPEC["body_sha256"], "parser digest")
    require(hashlib.sha256(elf.raw(VALIDATE, VALIDATE_SIZE)).hexdigest() == VALIDATE_SHA, "validator digest")
    require(hashlib.sha256(elf.raw(ACCUMULATE, ACCUMULATE_SIZE)).hexdigest() == ACCUMULATE_SHA,
            "accumulator digest")

    # The three bodies call each other exactly as the port assumes.
    require(call_at(elf, 0xA38998) == VALIDATE, "validate call")
    for site in (0xA389C6, 0xA389EC, 0xA38A0F):
        require(call_at(elf, site) == ACCUMULATE, f"accumulate call at {site:#x}")
    require(call_at(elf, 0xA38A3A) == 0x17090 and call_at(elf, 0xA38A77) == 0x17090,
            "array allocations")
    require(call_at(elf, 0xA38AAC) == 0xF8F470, "memcpy")
    require(call_at(elf, 0xA38AB6) == 0x17090, "second refcount")
    require(call_at(elf, 0xA39F4C) == VALIDATE, "validator recursion")
    require(call_at(elf, 0xA3A045) == ACCUMULATE, "accumulator recursion")
    require(jmp_at(elf, 0xA38B09) == 0x170C0, "delete[] tail")

    # The slot's static blob and the four boundary vectors.
    blob = list(int.from_bytes(elf.read(0x1604934 + 4 * i, 4), "little") for i in range(2))
    require(blob == EXPECTED["slot"]["data"], f"slot blob {blob}")
    for name, stream in (("slot", blob), ("leaf", [0x10000003, SENTINEL]),
                         ("group", [0x20000002, 0x10000001, 0x30000000, SENTINEL]),
                         ("end", [0x30000000, SENTINEL])):
        got = parse(stream)
        require(got == EXPECTED[name], f"{name} vector: {got}")

    require(parse([0x20000001] * 70 + [SENTINEL]) is None, "the recursion cap must refuse")
    require(parse([0x20000002, 0x10000001, SENTINEL]) is None, "a stream overrun must refuse")
    require("0xbeefbeef" in header, "header sentinel constant")

    if verbose:
        print("  oracle: 3 bodies hash-pinned; slot blob -> valid 1, a 0x20, size 0x20, bits 2, "
              "total 0x21; leaf/group/end boundaries match")


def main() -> int:
    argv = sys.argv[1:]
    elf = lib.Elf()
    header = (lib.ROOT / SPEC["native"]["header"]).read_text()

    if "--tamper-native" in argv:
        mutated = header.replace("kBlobSentinel = 0xbeefbeef", "kBlobSentinel = 0xbeefbeee", 1)
        if mutated == header:
            raise SystemExit("FAIL --tamper-native could not edit the header")
        texts = lib.read_native(SPEC)
        texts["header"] = mutated
        lib.check_native(SPEC, texts)   # must raise: kBlobSentinel no longer 0xbeefbeef
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" not in argv:
        check_semantics(elf, header)
        print("  ok: 0xa38960 three-pass parse, first refcount released, payload through the sentinel")
    return lib.main(SPEC, SUMMARY)


if __name__ == "__main__":
    sys.exit(main())
