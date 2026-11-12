#!/usr/bin/env python3
"""ELF byte-oracle for the local ranged-int variable wrapper family (26 members of 435 B + 351 B twins), instantiated for startup slot 401 (0x621530): the wrappers
0x620e10 and 0x6211d0 that build `cp10_phaseBlendUse` and `cp10_phaseBlendTypeOverride`.

The family claim is checked by byte equality, never by a name: every wrapper equals the hash-pinned wrapper 0x7ec160 outside seven rel32 operands, every inner constructor equals the
hash-pinned ranged-int constructor 0xc580 outside six rel32 operands, and each operand is resolved to the ELF-derived row (wrapper, inner, mid vptr, functor class, final vptr, dtor, slot 3, slot 6)
that native/core/helper_620e10_local_int_var.hpp must contain verbatim. The four functor bodies of every class equal the int bodies 0xc720 / 0xc740 / 0xc760 / 0xc770 outside the 4 byte class
operand; final-class slots 3 and 6 are `mov eax,1; ret` (so the 0x26ea0 / 0x26db0 toggle range stays {1.0, 10.0}). The slot-401 call sites (two wrapper calls, functor tags 0x15cc318 / 0x15cc3b0, the
objects, names, lengths, init / range / flags, the empty-description argument, destroy, f590(name), atexit dtors) are pinned instruction by instruction and as two hash-pinned runs.

NOT established: what the variables mean at run time (INFERENCE only); the wrapper's functor re-wrap is modelled by its net effect (proven here by the body equalities, not by running anything); the census
of wrapper calls inside slot 401 is a byte-pattern scan, not a CFG proof.
Offline only: reads the pinned private ELF (local/re/eboot.elf), never loads, relocates or executes an original RVA. The inherited M6 driver runs a fail-closed mutation self-test; --tamper [elf|native] are rejection probes.
"""

from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import bool_page_lib as bp  # noqa: E402


from slot_pin_util import require  # noqa: E402

TEMPLATE_WRAPPER = 0x7EC160
WRAPPER_SIZE = 0x1B3
INNER_SIZE = 0x15F
RANGED_INT_CTOR = 0xC580
GUARD = 0x15FE518
SLOT_BASE = 0x621530
SLOT_SIZE = 3799

# (wrapper, inner, mid vptr, functor class, final vptr, dtor, slot 3, slot 6): ELF-derived, identical to the rows of the native header.
ROWS = (
    (0x1cbe60, 0x1cc020, 0x1599058, 0x1599118, 0x1599148, 0x1c4f40, 0x1c8ac0, 0x1c8d40),
    (0x261b50, 0x261d10, 0x159c628, 0x159c7d8, 0x159c808, 0x253440, 0x25e3e0, 0x25e660),
    (0x261f10, 0x2620d0, 0x159c6a0, 0x159c870, 0x159c8a0, 0x253480, 0x25ef50, 0x25f220),
    (0x4c8f40, 0x4c9100, 0x15c1488, 0x15c1510, 0x15c1540, 0x4c45c0, 0x4c7f90, 0x4c82e0),
    (0x528da0, 0x528f60, 0x15c8148, 0x15c81b0, 0x15c81e0, 0x526040, 0x528320, 0x528370),
    (0x529160, 0x529320, 0x15c80d0, 0x15c8268, 0x15c8298, 0x526000, 0x5276f0, 0x527990),
    (0x5296d0, 0x529890, 0x15c8058, 0x15c8320, 0x15c8350, 0x525fc0, 0x526c70, 0x526cc0),
    (0x529a90, 0x529c50, 0x15c7fe0, 0x15c83b8, 0x15c83e8, 0x525f80, 0x526100, 0x526380),
    (0x56a9b0, 0x56ab70, 0x15ca690, 0x15ca828, 0x15ca858, 0x560460, 0x568660, 0x5688e0),
    (0x56ad70, 0x56af30, 0x15ca708, 0x15ca8c0, 0x15ca8f0, 0x5604a0, 0x5691d0, 0x569350),
    (0x56b150, 0x56b310, 0x15ca780, 0x15ca958, 0x15ca988, 0x560500, 0x569bc0, 0x569d40),
    (0x620e10, 0x620fd0, 0x15cc218, 0x15cc318, 0x15cc348, 0x6112e0, 0x61d820, 0x61d870),
    (0x6211d0, 0x621390, 0x15cc290, 0x15cc3b0, 0x15cc3e0, 0x611320, 0x61e2a0, 0x61e540),
    (0x720430, 0x7205f0, 0x15cce60, 0x15ccf60, 0x15ccf90, 0x70e150, 0x71c200, 0x71c250),
    (0x7207f0, 0x7209b0, 0x15cced8, 0x15cd038, 0x15cd068, 0x70e190, 0x71cc80, 0x71ccd0),
    (0x751190, 0x751350, 0x15cd270, 0x15cd3c8, 0x15cd3f8, 0x74aae0, 0x74e890, 0x74eb90),
    (0x751550, 0x751710, 0x15cd2e8, 0x15cd460, 0x15cd490, 0x74ab20, 0x74f670, 0x74f9f0),
    (0x751910, 0x751ad0, 0x15cd360, 0x15cd4f8, 0x15cd528, 0x74ab60, 0x750700, 0x750880),
    (0x7ec160, 0x7ec320, 0x15ce4d0, 0x15ce538, 0x15ce568, 0x7ea160, 0x7eb570, 0x7eb7f0),
    (0x829b30, 0x829cf0, 0x15ce9c8, 0x15ceae8, 0x15ceb18, 0x81be40, 0x825f40, 0x8261e0),
    (0xa324c0, 0xa32680, 0x15d1120, 0x15d1188, 0x15d11b8, 0xa2c850, 0xa31d90, 0xa31f10),
    (0xa4f4d0, 0xa4f690, 0x15d1a20, 0x15d1b78, 0x15d1ba8, 0xa47d20, 0xa4d490, 0xa4d760),
    (0xa4fec0, 0xa50080, 0x15d1a98, 0x15d1c30, 0x15d1c60, 0xa48240, 0xa4e190, 0xa4e310),
    (0xa50280, 0xa50440, 0x15d1b10, 0x15d1cc8, 0x15d1cf8, 0xa4ab50, 0xa4e830, 0xa4e9b0),
    (0xa624a0, 0xa62660, 0x15d1ea0, 0x15d1fa8, 0x15d1fd8, 0xa5bea0, 0xa60ea0, 0xa60ef0),
    (0xa62860, 0xa62a20, 0x15d1f18, 0x15d2080, 0x15d20b0, 0xa5bee0, 0xa61940, 0xa61990),
)

WRAPPER_WINDOWS = ((0x1E, 4), (0x25, 4), (0x5D, 4), (0x158, 4), (0x188, 4), (0x18F, 4), (0x1AF, 4))
INNER_WINDOWS = ((0x14, 4), (0x37, 4), (0x3E, 4), (0x49, 4), (0x13E, 4), (0x15B, 4))
INT_FN_BODIES = ((0xC720, 20, ((7, 4),), 4), (0xC740, 20, ((3, 4),), 0), (0xC760, 9, (), None), (0xC770, 1, (), None))


def same_outside(e, a: int, b: int, size: int, windows, what: str) -> None:
    """The bytes of [a, a+size) and [b, b+size) are equal outside the given (offset, length) windows (recording reads on both)."""
    cursor = 0
    for off, length in tuple(windows) + ((size, 0),):
        if off > cursor:
            require(e.read(a + cursor, off - cursor) == e.read(b + cursor, off - cursor),
                    f"{what}: {a:#x} equals {b:#x} over [+{cursor:#x}, +{off:#x})")
        cursor = off + length


def x_local_family(e, spec) -> None:
    """All 26 members: wrapper == 0x7ec160 and inner == 0xc580 outside their rel32 windows; operands, class tables and functor bodies resolved."""
    require(len(ROWS) == 26 and len({r[0] for r in ROWS}) == 26, "26 distinct family members")
    for wrapper, inner, mid, fn, final, dtor, slot3, slot6 in ROWS:
        same_outside(e, TEMPLATE_WRAPPER, wrapper, WRAPPER_SIZE, WRAPPER_WINDOWS, "wrapper")
        require(e.rip(wrapper + 0x1B, 7, 3) == GUARD, f"{wrapper:#x}: stack guard load")
        require(e.rip(wrapper + 0x22, 7, 3) == final - 0x10, f"{wrapper:#x}: final vtable lea (table + 0x10 is added at +0x56)")
        require(e.rip(wrapper + 0x5A, 7, 3) == fn, f"{wrapper:#x}: functor class lea")
        require(e.call(wrapper + 0x157) == inner, f"{wrapper:#x}: inner constructor call")
        require(e.call(wrapper + 0x187) == 0x20D60, f"{wrapper:#x}: concmd registration call")
        require(e.rip(wrapper + 0x18C, 7, 3) == GUARD, f"{wrapper:#x}: stack guard reload")
        require(e.call(wrapper + 0x1AE) == 0xF8F3A0, f"{wrapper:#x}: __stack_chk_fail")
        require(inner == wrapper + 0x1C0, f"{wrapper:#x}: inner constructor follows the wrapper")
        same_outside(e, RANGED_INT_CTOR, inner, INNER_SIZE, INNER_WINDOWS, "inner")
        require(e.rip(inner + 0x11, 7, 3) == GUARD, f"{inner:#x}: stack guard load")
        require(e.call(inner + 0x36) == 0x20AF0, f"{inner:#x}: concmd base ctor call")
        require(e.rip(inner + 0x3B, 7, 3) == mid - 0x10, f"{inner:#x}: mid vtable lea")
        require(e.rip(inner + 0x46, 7, 3) == fn, f"{inner:#x}: functor class lea")
        require(e.rip(inner + 0x13B, 7, 3) == GUARD, f"{inner:#x}: stack guard reload")
        require(e.call(inner + 0x15A) == 0xF8F3A0, f"{inner:#x}: __stack_chk_fail")
        # final class: slot 0 = dtor, slot 2 = 0x20d60, slots 3 / 6 = `mov eax,1; ret`; the mid class shares slots 2, 3, 5, 6
        for index, target in ((0, dtor), (2, 0x20D60), (3, slot3), (6, slot6)):
            require(e.reloc_target(final + 8 * index) == target, f"{final:#x}: final class slot {index}")
        for index, target in ((2, 0x20D60), (3, slot3), (5, 0x223C0), (6, slot6)):
            require(e.reloc_target(mid + 8 * index) == target, f"{mid:#x}: mid class slot {index}")
        e.bytes_at(slot3, "b801000000c3", f"{wrapper:#x}: slot 3 is mov eax,1; ret")
        e.bytes_at(slot6, "b801000000c3", f"{wrapper:#x}: slot 6 is mov eax,1; ret")
        # functor class: four bodies equal the int bodies outside the class operand, which is the class itself
        for index, (int_body, size, windows, class_operand) in enumerate(INT_FN_BODIES):
            body = e.reloc_target(fn + 8 * index)
            same_outside(e, int_body, body, size, windows, f"{fn:#x} functor slot {index}")
            if class_operand is not None:
                require(e.rip(body + (4 if index == 0 else 0), 7, 3) == fn, f"{fn:#x} functor slot {index} stores its own class")


# Caller-twin class tables: (twin tag, the wrapper class whose four slot bodies it aliases). Private tables of a CALLING startup slot (398 / 399).
CALLER_TWINS = ((0x15cc4a8, 0x15c8268), (0x15cc508, 0x15c8268))


def x_caller_twins(e, spec) -> None:
    """Every caller twin's four slot targets equal the four targets of the aliased wrapper class (itself proven int-twin by local_family); none is a family class."""
    own = {r[3] for r in ROWS}
    require([t for t, _ in CALLER_TWINS] == sorted(t for t, _ in CALLER_TWINS) and len(set(CALLER_TWINS)) == len(CALLER_TWINS), "caller twins sorted and distinct")
    for tag, alias in CALLER_TWINS:
        require(alias in own and tag not in own and tag != 0x15944A0, f"{tag:#x}: a twin of a family class, not itself a family class")
        for index in range(4):
            require(e.reloc_target(tag + 8 * index) == e.reloc_target(alias + 8 * index), f"{tag:#x} caller twin slot {index} aliases {alias:#x}")
        e.bytes_at(e.reloc_target(tag + 0x18), "c3", f"{tag:#x}: caller twin destroy is a bare ret")


def x_slot401_sites(e, spec) -> None:
    """Startup slot 401 (0x621530): the two wrapper calls, in the observed order, with the observed arguments."""
    body = e.raw(SLOT_BASE, SLOT_SIZE)   # unrecorded: a census over the hash-pinned ELF, not a pin (a flipped end byte cannot change it)
    for wrapper in (0x620E10, 0x6211D0):
        hits = [off for off in range(SLOT_SIZE - 4) if body[off] == 0xE8
                and (SLOT_BASE + off + 5 + int.from_bytes(body[off + 1:off + 5], "little", signed=True)) == wrapper]
        require(len(hits) == 1, f"slot 401 contains exactly one byte-pattern call to {wrapper:#x}")
    # site 1: cp10_phaseBlendUse
    require(e.rip(0x621839, 7, 3) == 0x126C259, "site 1: name lea")
    e.cstring(0x126C259, b"cp10_phaseBlendUse")
    e.bytes_at(0x62186A, "48c744242812000000", "site 1: name length 0x12")
    e.bytes_at(0x621844, "c744241c00000000", "site 1: init 0")
    e.bytes_at(0x62184C, "c744241800000000", "site 1: min 0")
    e.bytes_at(0x621854, "c744241402000000", "site 1: max 2")
    e.bytes_at(0x6218BE, "41b901000000", "site 1: flags 1")
    require(e.rip(0x62187D, 7, 3) == 0x15CC318, "site 1: argument functor tag")
    require(e.call(0x62189E) == 0xF730, "site 1: empty description (static string)")
    require(e.rip(0x6218A3, 7, 3) == 0x300D0C0, "site 1: object")
    e.bytes_at(0x6218CA, "504156", "site 1: description pushed, then the argument block")
    require(e.call(0x6218CD) == 0x620E10, "site 1: wrapper call")
    e.bytes_at(0x6218E3, "ff5018", "site 1: call [tag + 0x18] (functor destroy)")
    require(e.call(0x6218E9) == 0xF590, "site 1: f590 on the name view")
    require(e.rip(0x6218EE, 7, 3) == 0x6112E0, "site 1: atexit destructor")
    require(e.call(0x6218FB) == 0xF8F390, "site 1: __cxa_atexit")
    # site 2: cp10_phaseBlendTypeOverride
    require(e.rip(0x621900, 7, 3) == 0x126C26C, "site 2: name lea")
    e.cstring(0x126C26C, b"cp10_phaseBlendTypeOverride")
    e.bytes_at(0x621936, "48c74424281b000000", "site 2: name length 0x1b")
    e.bytes_at(0x62190B, "c744241c00000000", "site 2: init 0")
    e.bytes_at(0x621913, "c744241801000000", "site 2: min 1")
    e.bytes_at(0x62191B, "c744241404000000", "site 2: max 4")
    e.bytes_at(0x62198A, "41b901000000", "site 2: flags 1")
    require(e.rip(0x621949, 7, 3) == 0x15CC3B0, "site 2: argument functor tag")
    require(e.call(0x62196A) == 0xF730, "site 2: empty description (static string)")
    require(e.rip(0x62196F, 7, 3) == 0x300D198, "site 2: object")
    e.bytes_at(0x621996, "50488d44244850", "site 2: description pushed, then the argument block")
    require(e.call(0x62199D) == 0x6211D0, "site 2: wrapper call")
    e.bytes_at(0x6219B1, "ff5018", "site 2: call [tag + 0x18] (functor destroy)")
    require(e.call(0x6219B7) == 0xF590, "site 2: f590 on the name view")
    require(e.rip(0x6219BC, 7, 3) == 0x611320, "site 2: atexit destructor")
    require(e.call(0x6219C9) == 0xF8F390, "site 2: __cxa_atexit")
    # the two runs, as hashes: no other call sits between the wrapper call and the atexit record
    e.body_hash("slot 401 site 1 run", 0x621839, 199, "2eeb79c44b3bd0b4982d1de05df8535d0c5b6ec18ced2a9045f3eb03076b3e29", always=True)
    e.body_hash("slot 401 site 2 run", 0x621900, 206, "6af9161c3bbdc00cba0c96320319056befcb545bc487d7a7a4261f58633149a9", always=True)
    e.body_hash("slot 401 site 1 after-call run", 0x6218D2, 46, "0578275c9a1e733e400ed0c8e2439aeefec524e414c41c048b7724c94ccb5629", always=True)
    e.body_hash("slot 401 site 2 after-call run", 0x6219A2, 44, "653664b0ffe7363fb93c8daa1a8f174655093fffdfd42f50cb32c62a39b1c87c", always=True)


def x_local_imports(e, spec) -> None:
    bp._bp_check_imports(e, ((0xF8F390, 0x15FFFB8, "__cxa_atexit"), (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail")))


bp.lib.EXTRAS["local_family"] = x_local_family
bp.lib.EXTRAS["caller_twins"] = x_caller_twins
bp.lib.EXTRAS["slot401_sites"] = x_slot401_sites
bp.lib.EXTRAS["local_imports"] = x_local_imports


def template_pins(base: int, final: int, fn: int, inner: int):
    """The instruction pins of the 7ec160-shaped wrapper at `base` (relative offsets of the hash-pinned template)."""
    return [
        ("rip", base + 0x1B, 7, 3, GUARD),
        ("rip", base + 0x22, 7, 3, final - 0x10),
        ("imm", base + 0x56, "4983c610"),
        ("rip", base + 0x5A, 7, 3, fn),
        ("imm", base + 0x6F, "48c78558ffffff00000000"),
        ("imm", base + 0x81, "48c78570ffffff00000000"),
        ("imm", base + 0x98, "c4e37d19c001"),
        ("imm", base + 0x9E, "c4e3f916c101"),
        ("imm", base + 0xD1, "ff5108"),
        ("imm", base + 0xE2, "ff10"),
        ("imm", base + 0xF8, "ff10"),
        ("imm", base + 0x104, "ff10"),
        ("imm", base + 0x123, "ff5018"),
        ("call", base + 0x157, inner),
        ("imm", base + 0x16A, "ff5018"),
        ("call", base + 0x187, 0x20D60),
        ("rip", base + 0x18C, 7, 3, GUARD),
        ("call", base + 0x1AE, 0xF8F3A0),
    ]


ROW_A = next(r for r in ROWS if r[0] == 0x620E10)
ROW_B = next(r for r in ROWS if r[0] == 0x6211D0)


def row_token(r) -> str:
    return "{" + ", ".join("0x%x" % v for v in r) + "}"


SPEC = {
    "slot": -1,
    "label": 'helper_620e10',
    "no_table": True,
    "base": 0x620E10,
    "size": 435,
    "body_sha256": "4a3513e8ccf28c4ac268d12b2744cff67d2eb885231e902ae4db9e7700fa0c3a",
    "pad": "90909090909090909090909090",
    "callees": ['concmd_base_ctor', 'concmd_register'],
    "bool_tags": [],
    "float_tags": [],
    "strings": [
    ],
    "lengths": [
    ],
    "pins": template_pins(0x620E10, ROW_A[4], ROW_A[3], ROW_A[1]),
    "m6_callees": {
        'wrapper_template_7ec160': (0x7ec160, 435, "30b38c418e5682d6024b6b14aae540471882733b02d715752a867ba753f25307"),
        'ranged_int_ctor_c580': (0xc580, 351, "a93bc0f34cfd49fbbcbc7f2186b2668de6e49f5f54012caafca7f17dfb396bce"),
        'wrapper_6211d0': (0x6211d0, 435, "d9ed437d76237287d6ae91b420cf3023d7859042248872d7f6aaacdcbee891b1"),
        'inner_620fd0': (0x620fd0, 351, "328b6664f5aa90e9cf2e007f25fc3ab7942526e2ddb1ffe32fe492c9ac601904"),
        'inner_621390': (0x621390, 351, "c39abe8eb8096ac812c685c5172245b8c8539ae5da6641f5afa6eb78fa5ed833"),
        'dtor_6112e0': (0x6112e0, 54, "e1e8ccaa8a3e061ed599a76b00a49e63cca35f66dbef5ae2764c43684fbd6928"),
        'dtor_611320': (0x611320, 54, "7292538486e95dec49acfd2cdfdc7f246906ed4dad4b7c9eed137d096a2a23b1"),
        'int_fn_copy_c720': (0xc720, 20, "5b63116e7f29674de81acb2a9317619a1c1dcbdbc53f98785a3029c04aa7a914"),
        'int_fn_clone_c740': (0xc740, 20, "3b5a8a9fd4b3e15b1f5f14f73127ac98c5becb3a88fd096f8ccfb2015e991593"),
    },
    "m6_bytes": [
        (0x620e10 + 0x4C, "488db3d0000000", "wrapper: rsi = this + 0xd0 (the variable's own value cell)"),
        (0x620e10 + 0x65, "4c8933", "wrapper: [this] = final vptr (pre-store)"),
        (0x620e10 + 0x16D, "4c8933", "wrapper: [this] = final vptr (after the inner ctor)"),
        (0x620e10 + 0x173, "488b8df8feffff", "wrapper: rcx = init pointer"),
        (0x620e10 + 0x17A, "8b01", "wrapper: eax = *init"),
        (0x620e10 + 0x17C, "8983d0000000", "wrapper: [this+0xd0] = eax (value)"),
        (0x620e10 + 0x182, "8b01", "wrapper: eax = *init (second read)"),
        (0x620e10 + 0x184, "894368", "wrapper: [this+0x68] = eax (default)"),
    ],
    "m6_extras": ['local_family', 'caller_twins', 'slot401_sites', 'local_imports'],
    "native": {'header': 'native/core/helper_620e10_local_int_var.hpp', 'source': 'native/core/helper_620e10_local_int_var.cpp', 'test': 'native/tests/test_helper_620e10.cpp',
        'consts': {'kLocalIntVarWrapperSize': 435, 'kLocalIntVarInnerSize': 351, 'kLocalIntVarClassCount': 26, 'kLocalIntVarSlot3Result': 1, 'kLocalIntVarSlot6Result': 1,
                   'kLocalIntWrapperPhaseBlendUse': 0x620e10, 'kLocalIntWrapperPhaseBlendType': 0x6211d0,
                   'kSlot401PhaseBlendUseObject': 0x300d0c0, 'kSlot401PhaseBlendTypeObject': 0x300d198},
        'header_tokens': ['IDENTITY TAGS'] + [row_token(r) for r in ROWS] + [
            'LocalIntVarCtorResult helper_local_int_var_ctor(', 'DebugMenuTarget local_int_var_menu_target(', 'HelperRefusal helper_26db0_named_item_local_int(',
            'HelperIntSiteResult helper_init_local_int_var_site(', 'kLocalIntCallerTwins{{{0x15cc4a8, 0x15c8268}, {0x15cc508, 0x15c8268}}}', 'bool is_local_int_caller_twin_tag(', '"cp10_phaseBlendUse", 18, 0, 0, 2, 1, 0x15cc318, false}', '"cp10_phaseBlendTypeOverride", 27, 0, 1, 4, 1, 0x15cc3b0, false}'],
        'source_order': ['cls == nullptr', 'args.fn_vptr != cls->fn_vtbl', 'is_local_int_caller_twin_tag(args.fn_vptr, cls->fn_vtbl)', 'var.base.vptr = cls->final_vptr;', 'lifted.fn_vptr = kRangedIntFnVtbl;',
                         'init_ranged_int_var_c580(var, name, &var.value, min_value, max_value, flags, lifted, description)', 'var.base.vptr = cls->mid_vptr;', 'var.fn_vptr = cls->fn_vtbl;',
                         'out.arg_destroy_ok = true;', 'var.base.vptr = cls->final_vptr;\n    var.value = static_cast<std::uint32_t>(initial);', 'var.default_value = static_cast<std::uint32_t>(initial);',
                         'register_concmd_20d60(var.base, registry)', 'release_inert_view_f590(description)', 'release_inert_view_f590(name)', 'registry.atexit_log.record(cls->dtor_rva, &var)'],
        'test_tokens': ['kLocalIntVarWrapperSize == 0x1b3 && kLocalIntVarInnerSize == 0x15f && kLocalIntVarClassCount == 26', 'CHECK(!is_int_functor_tag(row.fn_vtbl)',
                        'CHECK(v.value_ref == &v.value && v.value == 3 && v.default_value == 3);', 'CHECK(!r.ok && r.refusal == HelperRefusal::unadmitted_arg_functor && !r.arg_destroy_ok);',
                        'r.refusal == HelperRefusal::wrong_helper', 'CHECK(creg.atexit_log.at(0).dtor_rva == 0x6112e0 && creg.atexit_log.at(0).object == &a);',
                        'CHECK(creg.atexit_log.at(1).dtor_rva == 0x611320 && creg.atexit_log.at(1).object == &b);', 'empty_string_f730()',
                        'CHECK(item.range_min == 1.0f && item.range_max == 10.0f);', 'HelperRefusal::unknown_target_class', 'CHECK(kLocalIntCallerTwins.size() == 2 && is_local_int_caller_twin_tag(0x15cc4a8, 0x15c8268) && is_local_int_caller_twin_tag(0x15cc508, 0x15c8268));']},
}


if __name__ == "__main__":
    raise SystemExit(bp.main(SPEC, 'local ranged-int variable wrapper family (26 x 435 B wrapper + 351 B inner, twins of 0x7ec160 / 0xc580) and the two slot-401 sites'))
