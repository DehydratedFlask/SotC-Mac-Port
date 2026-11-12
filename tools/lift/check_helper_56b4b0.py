#!/usr/bin/env python3
"""ELF-tie checker for the node-reference builder 0x56b4b0 (CPU-01-HELPER-CALLEE-MODELS-03).

Pins 0x56b4b0 (138 B, FDE 138) per instruction: the guarded static HashedStr at 0x2ed3640 (guard 0x2ed3668, first use 0xf730 + 0x14c4a0 + atexit(0x95610)),
`0x14c3a0(out, static)`, `0x14d880()`, the stores `[out+0x28] = node` / `byte [out+0x30] = 0` and the tail `jmp 0x14ccb0` with rdi = node. The three imports it
uses are NID-proven (__cxa_guard_acquire / __cxa_guard_release / __cxa_atexit). The same static, guard and destructor are used by 0x14c9a0 (rip operands
re-derived from its body). The call site in startup slot 422 (0x56b980, call at 0x56c39d) is pinned: rbx = the record 0x2f08928 (lea at 0x56c37d), `mov rdi, rbx`
at 0x56c392, and exactly one call to 0x56b4b0 in the body. Whole-image scan: the only other direct caller is at 0x691cc9 (not modelled here).

Usage: python3 -I tools/lift/check_helper_56b4b0.py [--no-mutate] [--tamper elf|native]
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

SLOT_BASE, SLOT_SIZE = 0x56B980, 2682
SLOT_SHA = "8e29b66eb02ad4d10fbfedb164c42dceff120de8c5d640c598f10fbf5a35c541"
STATIC, GUARD, DTOR = 0x2ED3640, 0x2ED3668, 0x95610


def _lea_targets(raw: bytes, base: int) -> set[int]:
    out = set()
    for i in range(len(raw) - 6):
        if raw[i] in (0x48, 0x4C) and raw[i + 1] == 0x8D and (raw[i + 2] & 0xC7) == 0x05:
            out.add(base + i + 7 + struct.unpack("<i", raw[i + 3:i + 7])[0])
    return out


def extra(e, fams) -> None:
    bp._bp_check_imports(e, ((0xF8F3D0, 0x15FFFD8, "__cxa_guard_acquire"), (0xF8F3E0, 0x15FFFE0, "__cxa_guard_release"),
                             (0xF8F390, 0x15FFFB8, "__cxa_atexit")))
    # 0x14c9a0 (already native, registry_object_14c9a0) loads the very same static, guard and destructor
    fde = lib.fde_ranges()
    size_14c9a0 = fde.get("0x14c9a0") if fde else 464
    require(size_14c9a0 == 464, "0x14c9a0 FDE length 464")
    e.body_hash("0x14c9a0 registry object ctor", 0x14C9A0, 464,
                "91c83a0d5d613fa291dbaf66316b994b4cd4c3c5400cae42e3fe3f97288ce883", always=True)
    loaded = _lea_targets(e.raw(0x14C9A0, 464), 0x14C9A0)
    require({STATIC, GUARD, DTOR} <= loaded, "0x14c9a0 loads the same static, guard and destructor as 0x56b4b0")
    # startup slot 422 call site
    e.body_hash("slot 422 (0x56b980)", SLOT_BASE, SLOT_SIZE, SLOT_SHA, always=True)
    require(e.call(0x56C39D) == 0x56B4B0, "slot 422: call at 0x56c39d reaches 0x56b4b0")
    require(e.lea(0x56C37D) == 0x2F08928, "slot 422: rbx = the record 0x2f08928")
    require(e.read(0x56C37D, 3) == bytes.fromhex("488d1d"), "slot 422: `lea rbx, [rip + disp]`")
    e.bytes_at(0x56C392, "4889df", "slot 422: mov rdi, rbx before the call")
    body = e.raw(SLOT_BASE, SLOT_SIZE)
    found = [SLOT_BASE + i for i in range(SLOT_SIZE - 4) if body[i] == 0xE8
             and SLOT_BASE + i + 5 + struct.unpack("<i", body[i + 1:i + 5])[0] == 0x56B4B0]
    require(found == [0x56C39D], f"slot 422: exactly one call to 0x56b4b0, got {[hex(x) for x in found]}")


SPEC = {
    "name": "check_helper_56b4b0",
    "summary": ("0x56b4b0 (138 B, sha 725d88e5...30dcbd) pinned per instruction with its callees 0xf730 / 0x14c4a0 / 0x14c3a0 / "
                "0x14d880 / tail 0x14ccb0; slot 422 passes the record 0x2f08928"),
    "families": [],
    "bodies": [
        {"name": "0x56b4b0 node reference builder", "rva": 0x56B4B0, "size": 138, "fde": 138, "pins": "node_ref_56b4b0",
         "sha": "725d88e59ccf43288ef78c68af95e2c56d59f6124f4e5999fb1e6fdab430dcbd"},
    ],
    "rips": [
        ("guard test", 0x56B4B7, 7, 3, GUARD), ("guard acquire arg", 0x56B4C7, 7, 3, GUARD),
        ("static ctor arg", 0x56B4DC, 7, 3, STATIC), ("atexit destructor", 0x56B4F0, 7, 3, DTOR),
        ("atexit dso handle", 0x56B4F7, 7, 3, 0x1604218), ("guard release arg", 0x56B506, 7, 3, GUARD),
        ("copy source", 0x56B512, 7, 3, STATIC),
    ],
    "calls": [
        ("guard acquire", 0x56B4CE, 0xF8F3D0), ("empty string 0xf730", 0x56B4D7, 0xF730),
        ("hashed ctor 0x14c4a0", 0x56B4EB, 0x14C4A0), ("atexit", 0x56B501, 0xF8F390),
        ("guard release", 0x56B50D, 0xF8F3E0), ("copy_hashed 0x14c3a0", 0x56B51C, 0x14C3A0),
        ("getter 0x14d880", 0x56B521, 0x14D880),
    ],
    "jmps": [("tail add_ref 0x14ccb0", 0x56B535, 0x14CCB0)],
    "relocs": [],
    "bytes": [("store node", 0x56B526, "48894328"), ("store flag", 0x56B52A, "c6433000"), ("rdi = node", 0x56B52E, "4889c7")],
    "extra": extra,
    "native": {
        "helper_h": "native/core/helper_56b4b0_node_ref.hpp", "helper_c": "native/core/helper_56b4b0_node_ref.cpp",
        "test": "native/tests/test_helper_56b4b0.cpp", "cmake": "native/CMakeLists.txt",
    },
    "constants": {
        "helper_h": {"kNodeRef56b4b0Rva": 0x56B4B0, "kNodeRef56b4b0Size": 138, "kNodeRefStaticHashedRva": STATIC,
                     "kNodeRefStaticGuardRva": GUARD, "kNodeRefStaticDtorRva": DTOR, "kNodeRefNodeOffset": 0x28,
                     "kNodeRefFlagOffset": 0x30},
    },
    "tokens": {
        "helper_h": ["void build_node_ref_56b4b0(NodeRef56b4b0& out) noexcept;", "Object14c9a0* node;           // +0x28",
                     "std::uint8_t flag;            // +0x30"],
        "helper_c": ["static_empty_hashed_2ed3640();", "copy_hashed_14c3a0(out.id, empty);", "getter_14d880();",
                     "out.node = node;", "out.flag = 0;", "add_ref_14ccb0(static_cast<void*>(node));"],
        "test": ["CHECK(node->a30 == 2);", "CHECK(b.node == node && node->a30 == 3);"],
        "cmake": ["core/helper_56b4b0_node_ref.cpp", "add_test(NAME helper_56b4b0 COMMAND test_helper_56b4b0)"],
    },
}

if __name__ == "__main__":
    raise SystemExit(lib.main(SPEC))
