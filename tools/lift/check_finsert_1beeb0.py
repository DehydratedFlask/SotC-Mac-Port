#!/usr/bin/env python3
"""ELF-tie the native lift of original function RVA 0x1beeb0 (list-3 insert body).

Pins the 284 B body and its sha, the `mov edi,0x60` + `operator new` call, the node-vtable
store, the `vmovups` copy pair, the fn!=0 self-link, the +0x48 -> node[0x58] store (and that
+0x40 is the vcall table, not stored), the list-3 push-front relinks, the +0x50 count
increment, the stack-canary guard cell, and the native header constants. Private: needs
local/re/eboot.elf.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
RVA = 0x1BEEB0
SIZE = 284
FN_SHA256 = "9a05e72ebcc8c6b444140e4d7c398cb3ea85cf2e5cdcf2eebb657795957a97af"
END = 0x1BEFCC

MOV_EDI_60 = 0x1BEECF
CALL_NEW = 0x1BEEE1
NEW_TARGET = 0x17090
LEA_NODE_VTABLE = 0x1BEEE9
NODE_VTABLE = 0x1598CE0
STORE_NODE_VTABLE = 0x1BEEF0
STORE_NODE_40 = 0x1BEEF4
STORE_NODE_58_ZERO = 0x1BEEFC
VMOVUPS_LOAD = 0x1BEF04
VMOVUPS_STORE = 0x1BEF0A
SELF_LINK_LEA = 0x1BEF20
SELF_LINK_STORE = 0x1BEF24
READ_DESC_48 = 0x1BEF27
READ_DESC_40 = 0x1BEF35
VCALL_SLOT1 = 0x1BEF44
STORE_NODE_58 = 0x1BEF85
LINK_NODE_PREV = 0x1BEF90
LINK_SENT_NEXT_LOAD = 0x1BEF9A
LINK_NODE_NEXT = 0x1BEF9E
LINK_OLD_HEAD_PREV = 0x1BEFA2
LINK_SENT_NEXT_STORE = 0x1BEFA5
COUNT_INC = 0x1BEFB0
STACKCHK_JNE = 0x1BEFBB
RET = 0x1BEFCB
STACKCHK_CALL = 0x1BEFCC
STACKCHK_TARGET = 0xF8F3A0
GUARD_CELL = 0x15FE518

NATIVE = ROOT / "native/core/insert_1beeb0.hpp"
IMPL = ROOT / "native/core/insert_1beeb0.cpp"
TEST = ROOT / "native/tests/test_insert_1beeb0.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def rip_target(img: d.Image, site: int, size: int, disp: int) -> int:
    return site + size + disp


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    fn = img.read(RVA, SIZE)
    require(hashlib.sha256(fn).hexdigest() == FN_SHA256, "fn 0x1beeb0 sha256")
    require(img.read(RET, 1) == b"\xc3", "ret at 0x1befcb")

    # operator new(0x60): mov edi,0x60 then call 0x17090.
    require(img.read(MOV_EDI_60, 5) == bytes.fromhex("bf60000000"), "mov edi,0x60 at 0x1beecf")
    call = img.read(CALL_NEW, 5)
    require(call[0] == 0xE8, f"call opcode at {CALL_NEW:#x}")
    require(rip_target(img, CALL_NEW, 5, struct.unpack("<i", call[1:])[0]) == NEW_TARGET,
            "operator new call target 0x17090")

    # node vtable store: lea 0x1598ce0 -> [rbx+0x50].
    lea = img.read(LEA_NODE_VTABLE, 7)
    require(lea[:3] == bytes.fromhex("488d0d"), f"lea rcx,[rip+..] at {LEA_NODE_VTABLE:#x}")
    require(rip_target(img, LEA_NODE_VTABLE, 7, struct.unpack("<i", lea[3:])[0]) == NODE_VTABLE,
            "node vtable lea target 0x1598ce0")
    require(img.read(STORE_NODE_VTABLE, 4) == bytes.fromhex("48894b50"), "mov [rbx+0x50],rcx")
    require(img.read(STORE_NODE_40, 8) == bytes.fromhex("48c7434000000000"), "node[0x40]=0")
    require(img.read(STORE_NODE_58_ZERO, 8) == bytes.fromhex("48c7435800000000"), "node[0x58]=0")

    # the 0x20 B copy: vmovups (%r12),%ymm0 ; vmovups %ymm0,0x10(%rbx).
    require(img.read(VMOVUPS_LOAD, 6) == bytes.fromhex("c4c17c100424"), "vmovups load at 0x1bef04")
    require(img.read(VMOVUPS_STORE, 5) == bytes.fromhex("c5fc114310"), "vmovups store at 0x1bef0a")

    # fn!=0 self-link: lea rax,[rbx+0x10] ; mov [rax],rax (guarded by test/je).
    require(img.read(0x1BEF15, 6) == bytes.fromhex("c4e3f916c001"), "vpextrq rax,xmm0,1")
    require(img.read(0x1BEF1E, 2) == bytes.fromhex("7407"), "je +7 (skip self-link)")
    require(img.read(SELF_LINK_LEA, 4) == bytes.fromhex("488d4310"), "lea rax,[rbx+0x10]")
    require(img.read(SELF_LINK_STORE, 3) == bytes.fromhex("488900"), "mov [rax],rax (self-link)")

    # +0x48 -> node[0x58]; +0x40 is the vcall table (call [rax+0x08]), NOT stored.
    require(img.read(READ_DESC_48, 5) == bytes.fromhex("498b442448"), "read descriptor[0x48]")
    require(img.read(READ_DESC_40, 5) == bytes.fromhex("498b442440"), "read descriptor[0x40]")
    require(img.read(VCALL_SLOT1, 3) == bytes.fromhex("ff5008"), "call [rax+0x08] vcall slot 1")
    require(img.read(STORE_NODE_58, 4) == bytes.fromhex("48894b58"), "mov [rbx+0x58],rcx (+0x48)")

    # list-3 push-front relinks and +0x50 count.
    require(img.read(LINK_NODE_PREV, 3) == bytes.fromhex("4c893b"), "mov [rbx],r15 (node->prev)")
    require(img.read(LINK_SENT_NEXT_LOAD, 4) == bytes.fromhex("498b4708"), "mov rax,[r15+0x08]")
    require(img.read(LINK_NODE_NEXT, 4) == bytes.fromhex("48894308"), "mov [rbx+0x08],rax")
    require(img.read(LINK_OLD_HEAD_PREV, 3) == bytes.fromhex("488918"), "mov [rax],rbx")
    require(img.read(LINK_SENT_NEXT_STORE, 4) == bytes.fromhex("49895f08"), "mov [r15+0x08],rbx")
    require(img.read(COUNT_INC, 4) == bytes.fromhex("48ff4010"), "incq [rax+0x10] (+0x50 count)")

    # stack-canary guard: read-only cell 0x15fe518; fail path -> 0xf8f3a0.
    guard = img.read(0x1BEEC1, 7)
    require(guard[:3] == bytes.fromhex("488b05"), "canary mov at 0x1beec1")
    require(rip_target(img, 0x1BEEC1, 7, struct.unpack("<i", guard[3:])[0]) == GUARD_CELL,
            "canary cell 0x15fe518")
    require(img.read(STACKCHK_JNE, 2) == bytes.fromhex("750f"), "jne -> stack_chk_fail")
    chk = img.read(STACKCHK_CALL, 5)
    require(chk[0] == 0xE8, "stack_chk_fail call opcode")
    require(rip_target(img, STACKCHK_CALL, 5, struct.unpack("<i", chk[1:])[0]) == STACKCHK_TARGET,
            "stack_chk_fail -> 0xf8f3a0")

    header = NATIVE.read_text()
    for token in (
        "kInsert1beeb0Rva = 0x1beeb0",
        "kInsert1beeb0Size = 284",
        "kInsert1beeb0EndRva = 0x1befcc",
        "kInsert1beeb0NodeVtable = 0x1598ce0",
        "kInsert1beeb0DescriptorVtable = 0x1594060",
        "kInsert1beeb0OperatorNew = 0x17090",
        "kInsert1beeb0NodeSize = 0x60",
        "kInsert1beeb0CopySize = 0x20",
        "kInsert1beeb0List3Offset = 0x40",
        "kInsert1beeb0Count3Offset = 0x50",
        "NOT modelled",
        "0x15fe518",
        "node[0x58] receives the descriptor's",
        "+0x48",
        "NOT its `+0x40`",
    ):
        require(token in header, f"header token {token!r}")
    require(len(IMPL.read_text()) > 0, "impl present")
    test_src = TEST.read_text()
    # E549: the copy-and-swap net effect is modelled as the callable value store.
    require("callable_value" in header, "callable value field")
    require("node->callable_value = descriptor.context[2]" in IMPL.read_text(),
            "callable value store")
    require("callable_value == d.context[2]" in test_src, "test asserts the callable value")
    require("slot_58 == d.slot_48" in test_src, "test asserts +0x48 -> +0x58")
    require("slot_58 != d.vtable" in test_src, "test asserts +0x40 NOT stored to +0x58")

    print(
        "OK check_finsert_1beeb0: extent=0x1beeb0+284B sha=" + FN_SHA256[:12] + "…; "
        "new(0x60)->0x17090; node vtable 0x1598ce0; vmovups 0x20B copy @0x1bef04/0x1bef0a; "
        "fn!=0 self-link; +0x48->node[0x58] (NOT +0x40); list-3 push-front; incq +0x50 count; "
        "guard 0x15fe518/0xf8f3a0; copy-and-swap helpers identified (E548/E549); callable value modelled"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
