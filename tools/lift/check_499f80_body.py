#!/usr/bin/env python3
"""Byte-oracle for the 193-byte app-main helper `0x499f80` (CPU-01-499F80-BODY-NATIVE-01).

Offline only: reads `local/re/eboot.elf`, never loads, relocates or executes an original RVA. This is the
full structural oracle; `check_init_499f80_body.py` keeps the original digest/wiring statement.

Pinned here:

* **Body identity**: sha256 of the 193-byte body and its FDE length (start and length).
* **Every call target**: eight relative calls to six targets, in address order, with the raw `e8` byte
  census proving no ninth call hides in the body and no `e9` tail jump exists.
* **Control flow**: four short `je` branches, all landing inside the body, and no early `ret` - the
  routine has no error/status return (E1046).
* **Ordering**: the `obj+0x48` zero store sits between the CritSec init call and the first buffer
  allocation; the previous-object release order is buffer B (tag 2), buffer A (tag 2), CritSec destroy,
  object free (tag 1), then the replacement cell write.
* **Offset-only globals**: the three global cells are loaded by rip-relative displacement and the native
  model addresses them only through caller-supplied cells (no RVA literal appears in the `.cpp`).

The next call at `0x531df4` (`0x56dcb0`) is deliberately out of scope and is not pinned or continued.
"""

from __future__ import annotations

import hashlib
import importlib.util
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/lift"))
from slot_pin_util import Elf, const_value, digest, require  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_RVA = 0x499F80
BODY_LEN = 193
BODY_SHA256 = "9ea23d248ce29318034c8f929f5cc435bbf7f92f57113ebf93b5b4c8e853f8ce"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"

# (site, target) in address order. 0x17070 / 0x17090 / 0x170b0 / 0x170c0 are the tiny allocator wrappers
# (core/alloc_wrappers_17070); 0x3b310 / 0x3b380 are the CritSec init/destroy shims (core/critsec_3b310).
CALLS = (
    (0x499FA4, 0x17070),   # object, 0x50 bytes
    (0x499FAF, 0x3B310),   # CritSec init at obj+0
    (0x499FC0, 0x17090),   # buffer A, 0xa00 bytes
    (0x499FDC, 0x17090),   # buffer B, 0xa00 bytes
    (0x49A010, 0x170C0),   # release old+0x28 (tag 2)
    (0x49A01E, 0x170C0),   # release old+0x08 (tag 2)
    (0x49A026, 0x3B380),   # CritSec destroy on the old object
    (0x49A02E, 0x170B0),   # free the old object (tag 1)
)

# Global cells, reached by rip-relative addressing only.
GLOBAL_STORE = 0x1604988        # qword store of the effective argument
GLOBAL_DEFAULT_ARG = 0x15FF9A8  # cmoveq source when the argument is null
GLOBAL_OLD_OBJECT = 0x2EF9FC8   # previous-object cell, swapped at the end

HDR = ROOT / "native/core/init_499f80_body.hpp"
SRC = ROOT / "native/core/init_499f80_body.cpp"
TEST = ROOT / "native/tests/test_init_499f80_body.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def load_fdes():
    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    ranges, _duplicates = census.load_fde_ranges(FDE)
    return ranges


def code_only(text: str) -> str:
    """Strip comments so documentation mentions of a cell are not mistaken for addressing it."""
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return "\n".join(line.split("//")[0] for line in text.splitlines())


def branch_target(e: Elf, site: int) -> int:
    """Target of a short `je rel8` (`74 cb`). Any other form fails the pin."""
    first, rel = e.read(site, 2)
    require(first == 0x74, f"0x{site:x} is not a short je")
    return site + 2 + struct.unpack("<b", bytes([rel]))[0]


def main() -> int:
    # ---- native contract (always checked, even without the private image) ---------------------
    hdr = HDR.read_text()
    src = SRC.read_text()
    test = TEST.read_text()
    cmake = CMAKE.read_text()

    require("alloc_wrappers_17070" in hdr, "header must include the allocator wrappers")
    require("critsec_3b310" in src, "body must use the native CritSec")
    require("run_init_499f80_body" in src, "missing body runner")
    for needle in ("Init499f80Stage", "alloc_object_failed", "critsec_init_failed",
                   "alloc_buffer_a_failed", "alloc_buffer_b_failed", "store_cell_written"):
        require(needle in hdr, f"header missing host status/observation {needle}")
    require("0x56dcb0" not in src, "this body must not continue into 0x56dcb0")
    # Offset-only globals: the model never hard-codes a global RVA in executable code (comments exempt),
    # and the header's identity tags carry exactly the three pinned addresses.
    src_code = code_only(src)
    for rva in (GLOBAL_STORE, GLOBAL_DEFAULT_ARG, GLOBAL_OLD_OBJECT):
        require(f"{rva:#x}" not in src_code.lower(),
                f"0x{rva:x} must not be addressed as a literal in the model")
    require(const_value(hdr, "kInit499f80GlobalStoreRva") == GLOBAL_STORE, "store cell identity tag")
    require(const_value(hdr, "kInit499f80GlobalDefaultArgRva") == GLOBAL_DEFAULT_ARG,
            "default-argument cell identity tag")
    require(const_value(hdr, "kInit499f80GlobalOldObjectRva") == GLOBAL_OLD_OBJECT,
            "old-object cell identity tag")
    require("globals_missing" in src and "hooks_missing" in src, "host rejections must be named")
    require("free_170c0" in src and "free_170b0" in src, "replacement releases must use the pinned wrappers")
    for needle in ("tests/test_init_499f80_body.cpp", "test_init_499f80_body"):
        require(needle in cmake, f"cmake missing {needle}")
    require("fail_at_call" in test, "fixture must inject allocation failures")
    require("free_tags.empty()" in test, "fixture must assert that nothing is rolled back")

    if not (ROOT / "local/re/eboot.elf").is_file() or not FDE.is_file():
        print("SKIP check_499f80_body: private image not present (module wiring checked)")
        return 0

    raw = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(raw) == ELF_SHA256, "unexpected ELF SHA256")
    e = Elf()
    body = e.read(BODY_RVA, BODY_LEN)

    # ---- body identity + FDE ------------------------------------------------------------------
    require(digest(body) == BODY_SHA256, "0x499f80 body sha256")
    found = load_fdes().find(BODY_RVA)
    require(found is not None and found[0] == BODY_RVA, "0x499f80 is not an FDE start")
    require(found[1] == BODY_LEN, f"0x499f80 FDE length {found[1]:#x} != {BODY_LEN:#x}")

    # ---- every call target, and no other call or tail jump -------------------------------------
    for site, target in CALLS:
        require(e.call(site) == target, f"0x{site:x} call target (want 0x{target:x})")
    raw_e8 = [BODY_RVA + i for i, byte in enumerate(body) if byte == 0xE8]
    require(raw_e8 == [site for site, _ in CALLS],
            f"call census changed: {[hex(x) for x in raw_e8]}")
    require(b"\xe9" not in body, "body must not gain a relative jump")

    # ---- control flow: four internal branches, no early return ---------------------------------
    require(branch_target(e, 0x49A000) == 0x49A036, "previous == new skips the store (0x49a000)")
    require(branch_target(e, 0x49A005) == 0x49A033, "previous == null goes to the store (0x49a005)")
    require(branch_target(e, 0x49A00E) == 0x49A015, "null buffer-B release is skipped (0x49a00e)")
    require(branch_target(e, 0x49A01C) == 0x49A023, "null buffer-A release is skipped (0x49a01c)")
    # The only `ret` is the epilogue; the other c3 byte is the modrm of `movq %rax,%rbx` at 0x499fa9.
    require(body[0x49A040 - BODY_RVA] == 0xC3, "epilogue retq at 0x49a040")
    require(0xC3 not in body[0x30:0xC0], "no early return between the prologue and the epilogue")
    e.bytes_at(0x49A033, "49891f" "4883c408" "5b" "415e" "415f" "5d" "c3",
               "0x49a033 store + epilogue")

    # ---- globals: rip-relative only ------------------------------------------------------------
    require(e.lea(0x499F8D) == GLOBAL_STORE, "0x499f8d leas the store cell 0x1604988")
    require(e.rip(0x499F94, 8, 4) == GLOBAL_DEFAULT_ARG, "0x499f94 cmoveq reads 0x15ff9a8")
    require(e.lea(0x499FE1) == GLOBAL_OLD_OBJECT, "0x499fe1 leas the old-object cell 0x2ef9fc8")
    e.bytes_at(0x499F8A, "4885ff", "0x499f8a testq %rdi,%rdi")
    e.bytes_at(0x499F9C, "488938", "0x499f9c movq %rdi,(%rax) -- the cell write")
    e.bytes_at(0x499FFA, "4d8b374939de", "0x499ffa reload + compare the old-object cell")

    # ---- fixed sizes and object offsets ---------------------------------------------------------
    e.bytes_at(0x499F9F, "bf50000000", "object allocation size 0x50")
    e.bytes_at(0x499FB4, "bf000a0000", "buffer A size 0xa00")
    e.bytes_at(0x499FD3, "bf000a0000", "buffer B size 0xa00")
    e.bytes_at(0x499FB9, "c7434800000000", "obj+0x48 u32 zero")
    e.bytes_at(0x499FC5, "48894308", "obj+0x08 = buffer A begin")
    e.bytes_at(0x499FC9, "48894310", "obj+0x10 = buffer A cursor")
    e.bytes_at(0x499FD8, "48894318", "obj+0x18 = buffer A end")
    e.bytes_at(0x499FE8, "48894328", "obj+0x28 = buffer B begin")
    e.bytes_at(0x499FEC, "48894330", "obj+0x30 = buffer B cursor")
    e.bytes_at(0x499FF6, "48894338", "obj+0x38 = buffer B end")
    e.bytes_at(0x49A007, "498b7e28", "release old+0x28 first")
    e.bytes_at(0x49A015, "498b7e08", "release old+0x08 second")

    # ---- ordering facts -------------------------------------------------------------------------
    sites = dict(CALLS)
    critsec_init = next(site for site, target in CALLS if target == 0x3B310)
    first_buffer = next(site for site, target in CALLS if target == 0x17090)
    require(critsec_init == 0x499FAF, "CritSec init site unchanged")
    require(0x499FB9 < first_buffer, "obj+0x48 is zeroed before the first buffer allocation")
    require(sites[0x49A010] == 0x170C0 and sites[0x49A01E] == 0x170C0, "both releases are tag-2 wrappers")
    require(sites[0x49A02E] == 0x170B0, "the object free is the tag-1 wrapper")
    require(0x49A010 < 0x49A01E < 0x49A026 < 0x49A02E < 0x49A033,
            "release order: buffer B, buffer A, CritSec destroy, object free, then the cell write")

    print("OK check_499f80_body: 193 B body (FDE 193), 8 calls / 6 targets, four internal branches, "
          "no early return, 0x48/0x08..0x38 offsets, rip-relative globals, release order")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
