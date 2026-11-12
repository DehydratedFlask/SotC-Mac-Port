#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 537 (RVA 0x1324c0, 795 B, index 537).

Batch 23, slot 537: one straight-line body that zeroes an 8-byte word at 0x2ee12e0, registers it with
__cxa_atexit(0x131f00, obj, 0x1604218), then performs 40 conversions through the 203 B helper
0x157e90, storing each `%ax` result at 0x2ee12f0 + 2 * i. Eleven of the 40 inputs are
`vxorps %xmm0,%xmm0,%xmm0`; the other 29 are rip-relative float words in .rodata.

Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an
original RVA. The destructor RVA and the __dso_handle are IDENTITY TAGS, recorded and never run.

The checker carries an INDEPENDENT Python port of 0x157e90 (written from the same instructions, in a
different language) and recomputes all 40 stored halves from the ELF's rodata. It then compares the
result with `kSlot537Halves` in the C++ fixture, so a drift in either port, in the input table, or in
the fixture's expected values is caught. The M6 driver runs a fail-closed mutation self-test;
--tamper elf (body byte) and --tamper-native (fixture table) are explicit rejection probes.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import l4_slot_lib as lib  # noqa: E402

BASE = 0x1324C0
SIZE = 795
HELPER = 0x157E90
HELPER_SIZE = 203

OBJ = 0x2EE12E0
TABLE = 0x2EE12F0
COUNT = 40
DTOR = 0x131F00
DSO = 0x1604218
ATEXIT = 0xF8F390

# Every one of the 40 call sites and the 40 store sites, read from the pinned bytes. The decoder
# below re-derives the targets from these addresses; nothing here is a hand-computed result.
CALL_SITES = [
    0x1324ED, 0x132501, 0x132515, 0x132529, 0x13253D, 0x132551, 0x132565, 0x132579,
    0x13258D, 0x1325A1, 0x1325B5, 0x1325C9, 0x1325DD, 0x1325ED, 0x1325FD, 0x13260D,
    0x132621, 0x132635, 0x132649, 0x13265D, 0x132671, 0x132685, 0x132699, 0x1326AD,
    0x1326C1, 0x1326D5, 0x1326E9, 0x1326FD, 0x13270D, 0x13271D, 0x13272D, 0x13273D,
    0x132751, 0x132765, 0x132779, 0x132789, 0x13279D, 0x1327AD, 0x1327BD, 0x1327CD,
]
STORE_SITES = [
    0x1324FA, 0x13250E, 0x132522, 0x132536, 0x13254A, 0x13255E, 0x132572, 0x132586,
    0x13259A, 0x1325AE, 0x1325C2, 0x1325D6, 0x1325E6, 0x1325F6, 0x132606, 0x13261A,
    0x13262E, 0x132642, 0x132656, 0x13266A, 0x13267E, 0x132692, 0x1326A6, 0x1326BA,
    0x1326CE, 0x1326E2, 0x1326F6, 0x132706, 0x132716, 0x132726, 0x132736, 0x13274A,
    0x13275E, 0x132772, 0x132782, 0x132796, 0x1327A6, 0x1327B6, 0x1327C6, 0x1327D2,
]

PINS = [
    ("imm", 0x1324C0, "55"),                    # push rbp
    ("imm", 0x1324C1, "4889e5"),                # mov rsp, rbp
    ("rip", 0x1324C4, 7, 3, OBJ),               # lea rsi, 0x2ee12e0
    ("rip", 0x1324CB, 7, 3, DTOR),              # lea rdi, 0x131f00
    ("rip", 0x1324D2, 7, 3, DSO),               # lea rdx, 0x1604218
    ("imm", 0x1324D9, "48c70600000000"),        # movq $0, (rsi)
    ("call", 0x1324E0, ATEXIT),                 # __cxa_atexit
    ("imm", 0x1327D9, "5dc3"),                  # pop rbp; ret
    # Every conversion and every store: the operand is decoded, not assumed.
    ("imm", HELPER, "c5f97ec2b9"),              # vmovd xmm0, edx
    ("imm", 0x157E94, "b917080000"),            # mov ecx, 0x817 (bextr control: start 23, len 8)
    ("imm", 0x157EA8, "81e6ffff7f00"),          # and esi, 0x7fffff
    ("imm", 0x157EAE, "2500800000"),            # and eax, 0x8000
    ("imm", 0x157EB3, "83c190"),                # add ecx, -0x70
    ("imm", 0x157EB8, "81f98f000000"),          # cmp ecx, 0x8f  (e == 0xff)
    ("imm", 0x157EC8, "31c9"),                  # xor ecx, ecx
    ("imm", 0x157ECA, "c1ee0d"),                # shr esi, 0xd
    ("imm", 0x157ECD, "0f94c1"),                # sete cl
    ("imm", 0x157ED4, "0d007c0000"),            # or eax, 0x7c00
    ("imm", 0x157EDA, "83f9f6"),                # cmp ecx, -0xa
    ("imm", 0x157EDF, "41b87e0000"),            # mov r8d, 0x7e
    ("imm", 0x157EE5, "b97d000000"),            # mov ecx, 0x7d
    ("imm", 0x157EEA, "81ce00008000"),          # or esi, 0x800000 (restore the implicit bit)
    ("imm", 0x157EF0, "bf01000000"),            # mov edi, 1
    ("imm", 0x157EFD, "d3e7"),                  # shl edi, cl
    ("imm", 0x157F02, "d3ea"),                  # shr edx, cl
    ("imm", 0x157F06, "83e201"),                # and edx, 1
    ("imm", 0x157F09, "8d543aff"),              # lea edx, -1(rdx,rdi)
    ("imm", 0x157F0D, "d3fa"),                  # sar edx, cl
    ("imm", 0x157F12, "bf0d010000"),            # mov edi, 0x10d (bextr: start 13, len 1)
    ("imm", 0x157F20, "8d9416ff"),              # lea edx, 0xfff(rsi,rdx)
    ("imm", 0x157F29, "81e600008000"),          # and esi, 0x800000 (the carry)
    ("imm", 0x157F33, "4183f9"),                # cmp r9d, 0x1f
    ("imm", 0x157F39, "0d007c0000"),            # or eax, 0x7c00 (saturate)
    ("imm", 0x157F3F, "41c1e1"),                # shl r9d, 0xa
    ("imm", 0x157F43, "c1ea0d"),                # shr edx, 0xd
    ("imm", 0x157F55, "0d007c0000"),            # or eax, 0x7c00 (mantissa == 0 -> inf)
    ("imm", 0x157ED9, "c3"), ("imm", 0x157F11, "c3"), ("imm", 0x157F3E, "c3"),
    ("imm", 0x157F54, "c3"), ("imm", 0x157F5A, "c3"),
]
for _i in range(COUNT):
    PINS.append(("call", CALL_SITES[_i], HELPER))
    PINS.append(("rip", STORE_SITES[_i], 7, 3, TABLE + 2 * _i))   # 66 89 05 + disp32

SPEC = {
    "label": "537",
    "slot": 537,
    "base": BASE,
    "size": SIZE,
    "body_sha256": "9bed06c20e7219f64e962cd93ec7437750e3d2567ece3249563e1225a5cda8b1",
    "pad": "9090909090554889",   # two nops, then the next function's push rbp / mov rsp,rbp
    "callees": [],
    "bool_tags": [],
    "float_tags": [],
    "strings": [],
    "lengths": [],
    "pins": PINS,
    "native": {
        "header": "native/core/ctor_slot_537_1324c0.hpp",
        "source": "native/core/ctor_slot_537_1324c0.cpp",
        "test": "native/tests/test_ctor_slot_537.cpp",
        "cmake": "native/CMakeLists.txt",
        "consts": {
            "kSlot537Rva": BASE,
            "kSlot537Size": SIZE,
            "kSlot537TableWord": 0x2DD6018,
            "kSlot537Object": OBJ,
            "kSlot537Table": TABLE,
            "kSlot537Count": COUNT,
            "kSlot537Stride": 2,
            "kSlot537LastHalf": TABLE + 2 * (COUNT - 1),
            "kSlot537Dtor": DTOR,
            "kSlot537DsoHandle": DSO,
            "kSlot537AtexitRva": ATEXIT,
            "kSlot537ConverterRva": HELPER,
            "kSlot537ConverterSize": HELPER_SIZE,
            "kSlot537ZeroInputs": 11,
        },
        "header_tokens": [
            "0x2ee12e0", "0x2ee12f0", "0x157e90", "0x131f00", "0x1604218",
            "kSlot537InputBits", "INFERENCE",
        ],
        "source_order": ["slot537_f32_to_f16", "log.record"],
        "test_tokens": [
            "kSlot537Halves", "halves_written", "0x477ff000", "0x33800000",
            "section_converter_subnormal", "slot537_f32_to_f16",
        ],
        "cmake_tokens": ["ctor_slot_537_1324c0.cpp", "test_ctor_slot_537.cpp", "ctor_slot_537"],
    },
}

SUMMARY = ("40 binary32 words (29 rip-relative loads, 11 vxorps zeros) converted through 0x157e90 and "
           "stored as binary16 at 0x2ee12f0 + 2*i; one __cxa_atexit(0x131f00, 0x2ee12e0) record")

M32 = 0xFFFFFFFF


def _s32(value: int) -> int:
    value &= M32
    return value - 0x100000000 if value & 0x80000000 else value


def _sar32(value: int, count: int) -> int:
    value &= M32
    count &= 31
    value = value - 0x100000000 if value & 0x80000000 else value
    return (value >> count) & M32


def f32_to_f16(bits: int) -> int:
    """Independent port of 0x157e90, written from the instructions. Not IEEE: this is the body's code.

    e = exp field, mant = mantissa, sign already at bit 15. `ecx = e - 0x70` selects the path:
    ecx <= 0 subnormal/zero, ecx == 0x8f (e == 0xff) inf/NaN, else normal round-half-to-even.
    """
    edx = bits & M32
    exp = (edx >> 23) & 0xFF
    mant = edx & 0x7FFFFF
    sign = (edx >> 16) & 0x8000
    ecx = (exp - 0x70) & M32

    if _s32(ecx) <= 0:
        if _s32(ecx) < -0xA:
            return sign                                    # flush to a signed zero
        m = mant | 0x800000
        shift_lo = (0x7D - exp) & M32
        shift_hi = (0x7E - exp) & M32
        round = (1 << (shift_lo & 31)) & M32               # 1 << (0x7d - e)
        value = (m >> (shift_hi & 31)) & M32               # m >> (0x7e - e), taken before the add
        round = (m + round) & M32                          # addl %esi,%edi
        value &= 1
        value = (round - 1 + value) & M32                  # lea -1(rdx,rdi)
        value = _sar32(value, shift_hi)                    # sarl (0x7e - e)
        return (sign | value) & 0xFFFF

    if ecx == 0x8F:                                        # e == 0xff
        if mant == 0:
            return sign | 0x7C00
        payload = mant >> 13
        return (sign | payload | (1 if payload == 0 else 0) | 0x7C00) & 0xFFFF

    result_exp = (exp - 0x6F) & M32                        # e - 0x6f
    low_bit = (edx >> 13) & 1                              # bextr 0x10d
    d = (mant + low_bit + 0xFFF) & M32
    carry = d & 0x800000
    if carry == 0:
        result_exp = (exp - 0x70) & M32                    # cmove ecx, r9d
    if _s32(result_exp) >= 0x1F:
        return sign | 0x7C00                               # saturate to infinity
    result_exp = (result_exp << 10) & M32
    return (sign | ((d >> 13) if carry == 0 else 0) | result_exp) & 0xFFFF


def _parse_array(source: str, name: str, mode: str) -> list[int]:
    """Read a constexpr array out of C++ source. mode: 'rva' (any width), 'bits' (8 digits + u),
    'half' (exactly 4 digits)."""
    match = re.search(rf"{name}\[[^\]]*\] = \{{(.*?)\}};", source, re.S)
    if not match:
        raise SystemExit(f"FAIL cannot find {name} in the native source")
    if mode == "bits":
        values = re.findall(r"0x([0-9a-fA-F]{8})u", match.group(1))
    elif mode == "half":
        values = re.findall(r"0x([0-9a-fA-F]{4})\b", match.group(1))
    else:
        values = re.findall(r"0x([0-9a-fA-F]+)", match.group(1))
    return [int(value, 16) for value in values]


def check_semantics(elf, header: str, test: str, verbose: bool = True) -> None:
    """Decode the body structurally and prove the fixture's 40 halves against an independent port."""
    body = elf.raw(BASE, SIZE + 8)

    # Provenance: each call consumes the most recent vmovss/vxorps before it.
    loads, zeros = [], []
    for offset in range(len(body) - 8):
        chunk = body[offset : offset + 8]
        if chunk[:4] == b"\xc5\xfa\x10\x05":
            disp = int.from_bytes(chunk[4:8], "little", signed=True)
            loads.append((BASE + offset, BASE + offset + 8 + disp))
        elif chunk[:4] == b"\xc5\xf8\x57\xc0":
            zeros.append((BASE + offset, 0))

    require = lib.require
    require(len(loads) == 29, f"expected 29 vmovss loads, decoded {len(loads)}")
    require(len(zeros) == 11, f"expected 11 vxorps zero vectors, decoded {len(zeros)}")

    sources = sorted(loads + zeros)
    provenance, bits = [], []
    for site in CALL_SITES:
        previous = [entry for entry in sources if entry[0] < site]
        require(previous, f"call at {site:#x} has no preceding vector input")
        source = previous[-1]
        provenance.append(source[1])
        bits.append(int.from_bytes(elf.read(source[1], 4), "little") if source[1] else 0)

    header_rvas = _parse_array(header, "kSlot537InputRvas", "rva")
    header_bits = _parse_array(header, "kSlot537InputBits", "bits")
    require(header_rvas == provenance, f"header input RVAs drifted: {header_rvas} != {provenance}")
    require(header_bits == bits, "header input bit patterns drifted from the ELF's rodata")
    require(sum(1 for value in provenance if value == 0) == 11, "zero-vector count")

    halves = [f32_to_f16(value) for value in bits]
    fixture = _parse_array(test, "kSlot537Halves", "half")
    require(len(fixture) == COUNT, f"fixture table has {len(fixture)} entries")
    require(fixture == halves,
            "fixture halves disagree with the independent port of 0x157e90: "
            f"{[hex(v) for v in fixture]} != {[hex(v) for v in halves]}")

    if verbose:
        print(f"  oracle: {len(loads)} loads + {len(zeros)} zero vectors -> 40 halves "
              f"({len(set(halves))} distinct), fixture table matches")


def main() -> int:
    argv = sys.argv[1:]
    elf = lib.Elf()
    header = (lib.ROOT / SPEC["native"]["header"]).read_text()
    test = (lib.ROOT / SPEC["native"]["test"]).read_text()

    if "--tamper-native" in argv:
        mutated = test.replace("0x3f3f,", "0x3f3e,", 1)
        require_change = mutated != test
        if not require_change:
            raise SystemExit("FAIL --tamper-native could not edit the fixture table")
        check_semantics(elf, header, mutated, verbose=False)   # must raise
        print("tamper-native NOT detected", file=sys.stderr)
        return 0

    if "--tamper" not in argv:
        check_semantics(elf, header, test)
        print("  ok: body decodes to 40 stores at 0x2ee12f0 + 2*i and 40 calls to 0x157e90")
    return lib.main(SPEC, SUMMARY)


if __name__ == "__main__":
    sys.exit(main())
