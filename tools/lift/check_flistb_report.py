#!/usr/bin/env python3
"""ELF-tie the list-B reader/clear and the bug-data report tree (E575).

Private: needs local/re/eboot.elf. Pins the two list-B phases, the report node/value/append/
property functions, the deep-copy assign, the two literals and the native constants.
"""

from __future__ import annotations

import hashlib
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import ps4_dynlink as d  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

FUNCTIONS = [
    ("list-B reader", 0x1BE2F7, 239, "af6ee6af50e9affc563a4f0f4ba94932599f2dabce0c8fb604e9678b3a095b97"),
    ("list-B clear", 0x1BE458, 69, "a2399c5325553e2052282676367790d6801cecb0fefccbd15090af75e5694afc"),
    ("list-B reset", 0x1BE4A4, 19, "4a2be0a3edde0db84c88c508808fd87dec42007f8cbad27f1d02044485eb40f0"),
    ("report node ctor", 0xAB1B20, 317, "c462b2be09d9f331e8f4487dd78d57e4b97f2c8982d371fa29dd39d70651f6d7"),
    ("report value ctor", 0xAB0AD0, 121, "a0241d6ec8084a45b3e97ad472d047b535ac54fbffce14d9c9bb226f8b67871a"),
    ("report append", 0xAB0D40, 61, "1b251336ef896f938394db369fcf250887886bad3e4fbea844a4d89a7e13ca27"),
    ("property set", 0xAB2AC0, 688, "61db4ab5a0515ef18a1d5437a822287908776178bad1954987358b5b985a8d0c"),
    ("string assign", 0x1BEAA0, 349, "f474dcef30889740260cd319a90ff88f47268dfa5eaa5f5f8e4c8b4d6f829e47"),
    ("node dtor", 0xAB21D0, 500, "e908acb05b8ff547cd25f164bdb69b111b1b8fdb9eaf528dc575e7c59caa480c"),
    ("node deleting dtor", 0xAB23D0, 28, "2d29f8be8e02815efddc41d4032815445838eb404614dfc70a3c3334686c8635"),
]

PATTERNS = [
    (0x1BDD90, "bf50000000", "mov edi,0x50 (list-B node)"),
    (0x1BDDDF, "49ff442430", "inc qword [r12+0x30]"),
    (0x1BE2F7, "498d4520", "lea rax,[r13+0x20] (sentinel)"),
    (0x1BE2FB, "4d8b6d20", "mov r13,[r13+0x20] (FIFO head)"),
    (0x1BE356, "4d8b6530", "mov r12,[r13+0x30] (value data)"),
    (0x1BE3B2, "498b5510", "mov rdx,[r13+0x10] (name data)"),
    (0x1BE3D5, "4d8b6d00", "mov r13,[r13] (next)"),
    (0x1BE4AE, "49c744243000000000", "mov qword [r12+0x30],0"),
    (0xAB0D40, "48897e70", "mov [rsi+0x70],rdi (parent)"),
    (0xAB0D52, "48c786c000000000000000", "mov qword [rsi+0xc0],0 (next sibling)"),
    (0x1BEAB9, "488b4320", "assign: load size"),
]

NATIVE = ROOT / "native/core/appinfo_listb.hpp"
REPORT = ROOT / "native/core/report_tree.hpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    for name, rva, size, sha in FUNCTIONS:
        body = img.read(rva, size)
        require(hashlib.sha256(body).hexdigest() == sha, f"{name} sha256")

    for addr, pattern, what in PATTERNS:
        raw = bytes.fromhex(pattern)
        require(img.read(addr, len(raw)) == raw, f"{what} at {addr:#x}")

    require(img.read(0xFB3E4C, 11) == b"Attachment\x00", '"Attachment" literal')
    require(img.read(0xFB3E34, 5) == b"name\x00", '"name" literal')

    # The two address points exist and their first slot is a real function pointer.
    for header, point, what in (
        (0x15D4AB0, 0x15D4AC0, "report node vtable"),
        (0x15D4CD8, 0x15D4CE8, "report value vtable"),
        (0x15D4E48, 0x15D4E58, "property map vtable"),
    ):
        require(img.read(header, 8) == b"\x00" * 8, f"{what} offset_to_top")
        require(img.read(point, 8) != b"\x00" * 8, f"{what} first slot")

    listb = NATIVE.read_text().lower()
    for token in (
        "kappinfolistbreaderrva = 0x1be2f7",
        "kappinfolistbreadersize = 239",
        "kappinfolistbclearrva = 0x1be458",
        "kappinfolistbclearsize = 69",
        "kappinfolistbresetrva = 0x1be4a4",
        "kappinfolistbvaluekind = 4",
        "attachment",
        "not modelled",
    ):
        require(token in listb, f"list-B header token {token!r}")

    report = REPORT.read_text().lower()
    for token in ("report_node_new", "report_value_new", "report_link_child",
                  "report_set_property", "last-write-wins", "not modelled"):
        require(token in report, f"report header token {token!r}")

    print(
        f"OK check_flistb_report: {len(FUNCTIONS)} functions, {len(PATTERNS)} byte patterns; "
        "FIFO reader, value-then-name clear, 8-byte count reset, deep-copy report; "
        "screenshot query and root adoption NOT claimed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
