#!/usr/bin/env python3
"""ELF byte-oracle for startup constructor slot 448 (RVA 0x4b8680), lane M6.

The inclusive function body is 0x4b8680..0x4b8874 (501 bytes): the reachable
return and stack-check-failure paths end there, eleven alignment NOPs follow,
and the next routine begins at 0x4b8880. Ghidra's body AddressSet and an
independent offline x86-64 walk agree on this range and all 107 instructions.

Offline only: reads the pinned private ELF, never loads, relocates or executes
an original RVA. Pins the constructor-table word, full body/hash, every decoded
instruction byte and operand target, four referenced strings and their length
immediates, bool functor/vtable relocations, imported atexit/canary identities,
shared callees, and the native model/test contract. The inherited M6 driver
runs the fail-closed mutation suite and supports an explicit --tamper probe.
"""

from __future__ import annotations

from functools import lru_cache
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lane_m6_lib as lib  # noqa: E402
from slot_pin_util import ELF_SHA256, ROOT, digest, require  # noqa: E402

sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as dyn  # noqa: E402
import tiny_slot_decode as tiny  # noqa: E402


@lru_cache(maxsize=1)
def _slot448_import_context():
    """Parse imports from the pinned ELF without executing or relocating it."""
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(elf) == ELF_SHA256, "slot-448 import parser ELF SHA256")
    parsed = dyn.parse(elf)
    return parsed, dyn.Image(elf, parsed["ph"])


def x_slot448_imports(e, spec) -> None:
    """Tie both imported thunks to their unique JMPREL NID records."""
    parsed, image = _slot448_import_context()
    imports = (
        (0xF8F390, 0x15FFFB8, "__cxa_atexit"),
        (0xF8F3A0, 0x15FFFC0, "__stack_chk_fail"),
    )
    for plt, expected_got, expected_name in imports:
        stub = e.read(plt, 6)
        require(stub[:2] == b"\xff\x25", f"PLT thunk {plt:#x} opcode")
        got = e.rip(plt, 6, 2)
        require(got == expected_got, f"PLT thunk {plt:#x} GOT target")
        rows = [r for r in parsed["relocs"] if r["table"] == "JMPREL" and r["target"] == got]
        require(len(rows) == 1, f"one JMPREL row for {plt:#x} GOT {got:#x}")
        reloc = rows[0]
        require(reloc["type"] == 7, f"{plt:#x} relocation is R_X86_64_JUMP_SLOT")
        require(reloc["addend"] == 0, f"{plt:#x} JMPREL addend is zero")

        # The M6 mutation driver perturbs every recorded relocation identity.
        e.reloc_pins.add(got)
        require((reloc["addend"] ^ (1 if got in e.reloc_flip else 0)) == 0,
                f"{plt:#x} JMPREL addend mutation")

        symbol = parsed["symbols"][reloc["symbol"]]
        expected_nid = dyn.nid(expected_name)
        require(symbol["type"] == "FUNC" and not symbol["defined"],
                f"{plt:#x} external function symbol")
        require(symbol["nid"] == expected_nid, f"{plt:#x} NID for {expected_name}")
        require(tiny.import_name(parsed, image, plt) == expected_nid,
                f"{plt:#x} decoded import agrees for {expected_name}")


def x_slot448_bool_vtable(e, spec) -> None:
    """Pin every slot of the final bool-variable vtable reached by both objects."""
    table = 0x1594218
    targets = (0x20B0, 0xC260, 0x20D60, 0xB890, 0xB8A0, 0x223C0, 0xB930)
    for index, target in enumerate(targets):
        require(e.reloc_target(table + 8 * index) == target,
                f"slot-448 final bool vtable {table:#x} entry {index}")
    require(e.reloc_target(0x1604218) == 0x1604218, "atexit DSO self-relocation")


def x_slot448_boundary(e, spec) -> None:
    """Verify the short canary branch target and the first bytes after alignment."""
    branch = e.read(0x4B885F, 2)
    require(branch[0] == 0x75, "stack-check path uses short JNE")
    require(0x4B8861 + int.from_bytes(branch[1:2], "little", signed=True) == 0x4B8870,
            "stack-check JNE targets __stack_chk_fail call")


lib.EXTRAS["slot448_imports"] = x_slot448_imports
lib.EXTRAS["slot448_bool_vtable"] = x_slot448_bool_vtable
lib.EXTRAS["slot448_boundary"] = x_slot448_boundary


SPEC = {
    "slot": 448,
    "base": 0x4B8680,
    "size": 501,
    "body_sha256": "08429c742843a30cdd17ac575e640e5583428003e3b8a7021e58b8434c281502",
    "pad": "9090909090909090909090",
    "callees": [
        "concmd_base_ctor",
        "concmd_register",
        "bool_dtor",
        "release_f590",
        "bool_fn_copy",
        "bool_fn_clone",
        "bool_fn_move",
        "bool_fn_destroy",
        "bool_slot3",
        "bool_slot6",
    ],
    "bool_tags": [0x15C0C30],
    "float_tags": [],
    "strings": [
        (0x1001DA0, b"shadow_DisableWorldZoomOut"),
        (0x1001DBB, b"Disables the world zooming out effect when transitioning to the pause menu."),
        (0x1001E07, b"shadow_mapShowBossCloudIndices"),
        (0x1001E26, b"If true it displays the boss cloud index over the first icon in the group."),
    ],
    "lengths": [
        ("shadow_DisableWorldZoomOut", 0x4B86E1, "48c745b81a000000"),
        ("Disables the world zooming out effect when transitioning to the pause menu.",
         0x4B86E9, "48c745984b000000"),
        ("shadow_mapShowBossCloudIndices", 0x4B87B1, "48c745b81e000000"),
        ("If true it displays the boss cloud index over the first icon in the group.",
         0x4B87C2, "48c745984a000000"),
    ],
    "pins": [
        # Full instruction encodings from the contiguous 107-instruction walk.
        ("imm", 0x4B8680, "55"),  # push
        ("imm", 0x4B8681, "4889e5"),  # mov
        ("imm", 0x4B8684, "4157"),  # push
        ("imm", 0x4B8686, "4156"),  # push
        ("imm", 0x4B8688, "4155"),  # push
        ("imm", 0x4B868A, "4154"),  # push
        ("imm", 0x4B868C, "53"),  # push
        ("imm", 0x4B868D, "4883ec48"),  # sub
        ("imm", 0x4B8691, "488b05805e1401"),  # mov
        ("imm", 0x4B8698, "4c8d3569bb0d01"),  # lea
        ("imm", 0x4B869F, "488d1d3a20a402"),  # lea
        ("imm", 0x4B86A6, "488d35f396b400"),  # lea
        ("imm", 0x4B86AD, "488d150797b400"),  # lea
        ("imm", 0x4B86B4, "4c8d65b0"),  # lea
        ("imm", 0x4B86B8, "4c8d7d90"),  # lea
        ("imm", 0x4B86BC, "c5f857c0"),  # vzero
        ("imm", 0x4B86C0, "b901000000"),  # mov
        ("imm", 0x4B86C5, "488b00"),  # mov
        ("imm", 0x4B86C8, "4983c610"),  # add
        ("imm", 0x4B86CC, "4889df"),  # mov
        ("imm", 0x4B86CF, "488945d0"),  # mov
        ("imm", 0x4B86D3, "488975b0"),  # mov
        ("imm", 0x4B86D7, "48895590"),  # mov
        ("imm", 0x4B86DB, "4c89e6"),  # mov
        ("imm", 0x4B86DE, "4c89fa"),  # mov
        ("imm", 0x4B86E1, "48c745b81a000000"),  # mov
        ("imm", 0x4B86E9, "48c745984b000000"),  # mov
        ("imm", 0x4B86F1, "c5f81145c0"),  # vmovups_store
        ("imm", 0x4B86F6, "c5f81145a0"),  # vmovups_store
        ("imm", 0x4B86FB, "4c8933"),  # mov
        ("imm", 0x4B86FE, "e8ed83b6ff"),  # call
        ("imm", 0x4B8703, "488d83c0000000"),  # lea
        ("imm", 0x4B870A, "4889df"),  # mov
        ("imm", 0x4B870D, "c5fc57c0"),  # vzero
        ("imm", 0x4B8711, "488d0d18851001"),  # lea
        ("imm", 0x4B8718, "48894360"),  # mov
        ("imm", 0x4B871C, "c7436900000000"),  # mov
        ("imm", 0x4B8723, "c5fc114370"),  # vmovups_store
        ("imm", 0x4B8728, "48c783a000000000000000"),  # mov
        ("imm", 0x4B8733, "48898bb0000000"),  # mov
        ("imm", 0x4B873A, "48c783b800000000000000"),  # mov
        ("imm", 0x4B8745, "4c8933"),  # mov
        ("imm", 0x4B8748, "c683c000000001"),  # mov
        ("imm", 0x4B874F, "c6436801"),  # mov
        ("imm", 0x4B8753, "e80886b6ff"),  # call
        ("imm", 0x4B8758, "4c89ff"),  # mov
        ("imm", 0x4B875B, "e8306eb5ff"),  # call
        ("imm", 0x4B8760, "4c89e7"),  # mov
        ("imm", 0x4B8763, "e8286eb5ff"),  # call
        ("imm", 0x4B8768, "4c8d3d4199b4ff"),  # lea
        ("imm", 0x4B876F, "488d15a2ba1401"),  # lea
        ("imm", 0x4B8776, "4889de"),  # mov
        ("imm", 0x4B8779, "4c89ff"),  # mov
        ("imm", 0x4B877C, "e80f6cad00"),  # call
        ("imm", 0x4B8781, "488d1d2020a402"),  # lea
        ("imm", 0x4B8788, "488d057896b400"),  # lea
        ("imm", 0x4B878F, "4c8d65b0"),  # lea
        ("imm", 0x4B8793, "4c8d6d90"),  # lea
        ("imm", 0x4B8797, "c5f857c0"),  # vzero
        ("imm", 0x4B879B, "b901000000"),  # mov
        ("imm", 0x4B87A0, "488945b0"),  # mov
        ("imm", 0x4B87A4, "488d057b96b400"),  # lea
        ("imm", 0x4B87AB, "4c89e6"),  # mov
        ("imm", 0x4B87AE, "4c89ea"),  # mov
        ("imm", 0x4B87B1, "48c745b81e000000"),  # mov
        ("imm", 0x4B87B9, "c5f81145c0"),  # vmovups_store
        ("imm", 0x4B87BE, "48894590"),  # mov
        ("imm", 0x4B87C2, "48c745984a000000"),  # mov
        ("imm", 0x4B87CA, "c5f81145a0"),  # vmovups_store
        ("imm", 0x4B87CF, "4889df"),  # mov
        ("imm", 0x4B87D2, "4c8933"),  # mov
        ("imm", 0x4B87D5, "e81683b6ff"),  # call
        ("imm", 0x4B87DA, "488d83c0000000"),  # lea
        ("imm", 0x4B87E1, "4889df"),  # mov
        ("imm", 0x4B87E4, "c5fc57c0"),  # vzero
        ("imm", 0x4B87E8, "488d0d41841001"),  # lea
        ("imm", 0x4B87EF, "48894360"),  # mov
        ("imm", 0x4B87F3, "c7436900000000"),  # mov
        ("imm", 0x4B87FA, "c5fc114370"),  # vmovups_store
        ("imm", 0x4B87FF, "48c783a000000000000000"),  # mov
        ("imm", 0x4B880A, "48898bb0000000"),  # mov
        ("imm", 0x4B8811, "48c783b800000000000000"),  # mov
        ("imm", 0x4B881C, "4c8933"),  # mov
        ("imm", 0x4B881F, "c683c000000000"),  # mov
        ("imm", 0x4B8826, "c6436800"),  # mov
        ("imm", 0x4B882A, "e83185b6ff"),  # call
        ("imm", 0x4B882F, "4c89ef"),  # mov
        ("imm", 0x4B8832, "e8596db5ff"),  # call
        ("imm", 0x4B8837, "4c89e7"),  # mov
        ("imm", 0x4B883A, "e8516db5ff"),  # call
        ("imm", 0x4B883F, "488d15d2b91401"),  # lea
        ("imm", 0x4B8846, "4c89ff"),  # mov
        ("imm", 0x4B8849, "4889de"),  # mov
        ("imm", 0x4B884C, "e83f6bad00"),  # call
        ("imm", 0x4B8851, "488b05c05c1401"),  # mov
        ("imm", 0x4B8858, "488b00"),  # mov
        ("imm", 0x4B885B, "483b45d0"),  # cmp
        ("imm", 0x4B885F, "750f"),  # jne
        ("imm", 0x4B8861, "4883c448"),  # add
        ("imm", 0x4B8865, "5b"),  # pop
        ("imm", 0x4B8866, "415c"),  # pop
        ("imm", 0x4B8868, "415d"),  # pop
        ("imm", 0x4B886A, "415e"),  # pop
        ("imm", 0x4B886C, "415f"),  # pop
        ("imm", 0x4B886E, "5d"),  # pop
        ("imm", 0x4B886F, "c3"),  # ret
        ("imm", 0x4B8870, "e82b6bad00"),  # call
        # Semantic target checks alongside the exact complete encodings above.
        ("call", 0x4B86FE, 0x20AF0),
        ("call", 0x4B8753, 0x20D60),
        ("call", 0x4B875B, 0xF590),
        ("call", 0x4B8763, 0xF590),
        ("call", 0x4B877C, 0xF8F390),
        ("call", 0x4B87D5, 0x20AF0),
        ("call", 0x4B882A, 0x20D60),
        ("call", 0x4B8832, 0xF590),
        ("call", 0x4B883A, 0xF590),
        ("call", 0x4B884C, 0xF8F390),
        ("call", 0x4B8870, 0xF8F3A0),
        ("rip", 0x4B8691, 7, 3, 0x15FE518),
        ("rip", 0x4B8698, 7, 3, 0x1594208),
        ("rip", 0x4B869F, 7, 3, 0x2EFA6E0),
        ("rip", 0x4B86A6, 7, 3, 0x1001DA0),
        ("rip", 0x4B86AD, 7, 3, 0x1001DBB),
        ("rip", 0x4B8711, 7, 3, 0x15C0C30),
        ("rip", 0x4B8768, 7, 3, 0x20B0),
        ("rip", 0x4B876F, 7, 3, 0x1604218),
        ("rip", 0x4B8781, 7, 3, 0x2EFA7A8),
        ("rip", 0x4B8788, 7, 3, 0x1001E07),
        ("rip", 0x4B87A4, 7, 3, 0x1001E26),
        ("rip", 0x4B87E8, 7, 3, 0x15C0C30),
        ("rip", 0x4B883F, 7, 3, 0x1604218),
        ("rip", 0x4B8851, 7, 3, 0x15FE518),
    ],
    "m6_callees": {
        "atexit_thunk_f8f390": (0xF8F390, 6, "9fe985b7f057388a29453618dffcfbcbc78b344a465d9d19f676521dd0c4dd2f"),
        "stack_chk_fail_thunk_f8f3a0": (0xF8F3A0, 6, "87b4ef50f63101b63a4e326983bfb7dd2b956d58db346523076d02cf312125ea"),
    },
    "m6_extras": ["slot448_imports", "slot448_bool_vtable", "slot448_boundary"],
    "m6_bytes": [
        (0x4B8880, "554889e553", "next routine prologue after alignment padding"),
    ],
    "native": {
        "header": "native/core/ctor_slot_448_4b8680.hpp",
        "source": "native/core/ctor_slot_448_4b8680.cpp",
        "test": "native/tests/test_ctor_slot_448.cpp",
        "consts": {
            "kSlot448Rva": 0x4B8680,
            "kSlot448Size": 501,
            "kSlot448BoolFnTag": 0x15C0C30,
            "kSlot448Var1NameLength": 0x1A,
            "kSlot448Var1DescriptionLength": 0x4B,
            "kSlot448Var2NameLength": 0x1E,
            "kSlot448Var2DescriptionLength": 0x4A,
        },
        "header_tokens": [
            "ConCmdBoolVar disable_world_zoom_out",
            "ConCmdBoolVar map_show_boss_cloud_idx",
            "Slot448Objects",
            "kSlot448Var1Name[] = \"shadow_DisableWorldZoomOut\"",
            "kSlot448Var2Name[] = \"shadow_mapShowBossCloudIndices\"",
        ],
        "source_order": [
            "init_bool_var_literal(objects.disable_world_zoom_out",
            "kSlot448BoolFnTag, concmd, 1, 1",
            "init_bool_var_literal(objects.map_show_boss_cloud_idx",
            "kSlot448BoolFnTag, concmd, 0, 0",
            "out.ok = out.first.ok && out.second.ok;",
        ],
        "test_tokens": [
            "kSlot448Size == 501",
            "sizeof(Slot448Objects) == 2 * sizeof(ConCmdBoolVar)",
            "r.first.views_inert && r.second.views_inert",
            "v.base.registered == 1 && v.base.flags == 1",
            "v.value == value && v.default_value == value",
            "CHECK(creg.pending.size() == 2)",
            "check_bool_common_desc(o.disable_world_zoom_out, 0x15c0c30, 1,",
            "check_bool_common_desc(o.map_show_boss_cloud_idx, 0x15c0c30, 0,",
            "same_view(o.disable_world_zoom_out.base.name, \"shadow_DisableWorldZoomOut\", 0x1a)",
            "same_view(o.map_show_boss_cloud_idx.base.name, \"shadow_mapShowBossCloudIndices\", 0x1e)",
            "check_atexit(creg, 0, 0x20b0, &o.disable_world_zoom_out)",
            "check_atexit(creg, 1, 0x20b0, &o.map_show_boss_cloud_idx)",
            "CHECK(!r.ok && !r.first.ok && !r.second.ok && !r.first.registration.ok && !r.second.registration.ok",
        ],
    },
}


if __name__ == "__main__":
    raise SystemExit(lib.main(
        SPEC,
        "two bool console variables (1/1 and 0/0), borrowed views, shared bool tag, atexit destructor",
    ))
