#!/usr/bin/env python3
"""Byte-oracle for `0x210b0` — the ConCmd manager constructor + pending-list drain
(CPU-01-CONCMD-MANAGER-210B0-01).

Offline only: reads `local/re/eboot.elf`, never loads, relocates or executes an original RVA. The
native model (`native/core/concmd_manager_210b0.{hpp,cpp}`) reproduces the body over caller-supplied
memory; nothing here is a runtime, boot, audio, save, engine-readiness or "the registry is populated"
claim.

What this checker pins, and why:

* **Every byte of the 273-byte body, twice.** The body digest is pinned, *and* the body is compared
  against a contiguous, address-ordered list of instruction fragments that must concatenate to it
  exactly. The digest alone would let a whole-instruction mutant hide behind a stale hash; the
  fragments alone would let a length change slide. Together they also fix the boundary the FDE gives
  (`(0x210b0, 273)`), the `ret` at `0x211c0`, the 15-byte pad after it, and that `0x211d0` is the next
  FDE start.
* **The transfer census.** Four relative calls to four distinct targets and four branches, no `e9`
  and no indirect transfer. The raw `e8` scan over the body finds exactly the four decoded call sites,
  so a fifth call cannot appear unnoticed. The four targets are `0x3b310` (CritSec init),
  `0xf8f3d0` / `0xf8f3e0` (guard acquire/release) and `0x20d80` (the register body).
* **No lock in the constructor.** `0x3b390` / `0x3b3c0` — the manager mutex lock/unlock the lookup and
  the register body use — are not among the call targets. Construction and draining take no manager
  lock; `0x20d80` takes it per command.
* **The drain calls the body, not the guard.** `0x21166` targets `0x20d80`. The guard entry `0x20d60`
  is pinned too (its 26-byte body, its `cmpb [rdi+0x18],0`, the `lea` of the cell `0x2dfcbc8` and the
  `jmp 0x20d80`), so the distinction is byte-visible: every queued command already has
  `registered == 1`, and a drain through `0x20d60` would insert nothing.
* **The cell is installed after the constructor.** `0x243a1` allocates 0x60 B, `0x243b1` calls
  `0x210b0`, and `0x243f5` (`mov %r14,(%r15)` on the `0x2dfcbc8` `lea` at `0x243b6`) stores the pointer
  afterwards. The manager body therefore branches on its **argument** (`testq %r13,%r13` in
  `0x20d80`), not on the cell, and a cell-reading drain would re-queue every command.
* **The drain order and the link arithmetic.** `0x21136` loads `[0x2dfce50]`, `0x21144`/`0x2116f`
  subtract 8, `0x2114b`/`0x21176` collapse a null link with `cmoveq`, `0x2114f`/`0x2117a` compare
  against the sentinel `0x2dfce48` (`lea` at `0x2113d`), and the body follows `element+0x08` at
  `0x2116b` — the forward chain, oldest first.
* **The re-termination block.** `0x21198` reads `element+0x10` and `0x211a7` writes `+0x08`, with
  `0x211af` storing into `0x2dfce58`. For a list built by `0x20d80` the first element's `prev` is the
  head link, so the two stores land on `head.next` and `head.prev` and the object ends empty; the
  checker pins the block, and the fixture pins both that result and the general (non-emptying) form.
* **The guard block is the shared construct.** The 63-byte static-list initialisation at `0x210f7`,
  `0x20e7f` (register) and `0x2171b` (lookup) must be equal once their seven RIP-relative
  displacements are masked, so a divergence in any of the three is caught here.
* **Native contract + honest limits.** Constant pins, the shared helpers the model must use, the
  absence of any lock in the constructor, the fixture's synthetic-only marker and its fail-visible
  cases, the CMake entries, and the requirement that the header keeps its NOT CLAIMED section.

A mutation self-test flips the first and last byte of every recorded pin and requires every mutant to
be rejected.
"""

from __future__ import annotations

import importlib.util
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/lift"))
from slot_pin_util import Elf, const_value, digest, require  # noqa: E402

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"

BODY_RVA = 0x210B0
BODY_LEN = 273
BODY_SHA256 = "619fba45d65cab01645eb1f8f8015c7585aeec31d4afe7f3b35ff2422e17ff40"

MANAGER_CELL = 0x2DFCBC8
PENDING_HEAD = 0x2DFCE50
PENDING_HEAD_PREV = 0x2DFCE58
PENDING_SENTINEL = 0x2DFCE48
PENDING_GUARD = 0x2DFCE60

REGISTER_GUARD_RVA = 0x20D60
REGISTER_BODY_RVA = 0x20D80
REGISTER_BODY_LEN = 392
INSTALLER_RVA = 0x242C0

# The whole 273-byte body, in address order, one fragment per instruction. Concatenated they must
# reproduce the body exactly: the fragments are the mutation surface, the digest is the backstop.
FRAGMENTS = (
    (0x210B0, "554889e5"),                  # push rbp; mov rsp,rbp
    (0x210B4, "415741565350"),              # push r15, r14, rbx, rax
    (0x210BA, "4989fe"),                    # mov rdi,r14          (this)
    (0x210BD, "c5f9efc0"),                  # vpxor xmm0,xmm0,xmm0
    (0x210C1, "498d4608"),                  # lea rax,[r14+8]
    (0x210C5, "498d7e58"),                  # lea rdi,[r14+0x58]   (CritSec handle)
    (0x210C9, "49894608"),                  # [this+0x08] = this+8
    (0x210CD, "49894610"),                  # [this+0x10] = this+8
    (0x210D1, "49c7461800000000"),          # [this+0x18] = 0      (the lookup's root)
    (0x210D9, "c4c17a7f4620"),              # 16 zero bytes at this+0x20
    (0x210DF, "41c6462000"),                # byte 0 at this+0x20   (redundant with 0x210d9)
    (0x210E4, "c4c17a7f4638"),              # 16 zero bytes at this+0x38
    (0x210EA, "49c7464800000000"),          # [this+0x48] = 0
    (0x210F2, "e819a20100"),                # call 0x3b310
    (0x210F7, "8a0563bddd02"),              # movb 0x2dfce60(%rip),%al
    (0x210FD, "84c0"),                      # testb al,al
    (0x210FF, "7535"),                      # jne 0x21136
    (0x21101, "488d3d58bddd02"),            # lea rdi,[0x2dfce60]
    (0x21108, "e8c3e2f600"),                # call 0xf8f3d0
    (0x2110D, "85c0"),                      # testl eax,eax
    (0x2110F, "7425"),                      # je 0x21136
    (0x21111, "488d0538bddd02"),            # lea rax,[0x2dfce50]
    (0x21118, "488d3d41bddd02"),            # lea rdi,[0x2dfce60]
    (0x2111F, "c4e1f96ec0"),                # vmovq rax,xmm0
    (0x21124, "c5f970c044"),                # vpshufd $0x44
    (0x21129, "c5fa7f051fbddd02"),          # vmovdqu [0x2dfce50]
    (0x21131, "e8aae2f600"),                # call 0xf8f3e0
    (0x21136, "488b0513bddd02"),            # mov [0x2dfce50],%rax
    (0x2113D, "4c8d3d04bddd02"),            # lea r15,[0x2dfce48]
    (0x21144, "488d58f8"),                  # lea rbx,[rax-8]
    (0x21148, "4885c0"),                    # test rax,rax
    (0x2114B, "480f44d8"),                  # cmove rax,rbx
    (0x2114F, "4c39fb"),                    # cmp r15,rbx
    (0x21152, "7432"),                      # je 0x21186
    (0x21154, "6666662e0f1f840000000000"),  # 12-byte loop-alignment nop
    (0x21160, "4889df"),                    # mov rbx,rdi          (element)
    (0x21163, "4c89f6"),                    # mov r14,rsi          (manager)
    (0x21166, "e815fcffff"),                # call 0x20d80
    (0x2116B, "488b4308"),                  # mov [rbx+8],%rax
    (0x2116F, "488d58f8"),                  # lea rbx,[rax-8]
    (0x21173, "4885c0"),                    # test rax,rax
    (0x21176, "480f44d8"),                  # cmove rax,rbx
    (0x2117A, "4c39fb"),                    # cmp r15,rbx
    (0x2117D, "75e1"),                      # jne 0x21160
    (0x2117F, "488b05cabcdd02"),            # mov [0x2dfce50],%rax
    (0x21186, "488d48f8"),                  # lea rcx,[rax-8]
    (0x2118A, "4885c0"),                    # test rax,rax
    (0x2118D, "488d15bcbcdd02"),            # lea rdx,[0x2dfce50]
    (0x21194, "480f44c8"),                  # cmove rax,rcx
    (0x21198, "488b4110"),                  # mov [rcx+0x10],%rax
    (0x2119C, "488d48f8"),                  # lea rcx,[rax-8]
    (0x211A0, "4885c0"),                    # test rax,rax
    (0x211A3, "480f44c8"),                  # cmove rax,rcx
    (0x211A7, "48895108"),                  # mov %rdx,8(%rcx)   tail->next = &head (rdx = lea 0x2dfce50)
    (0x211AB, "4883c108"),                  # add $8,%rcx        rcx = tail + 8 = the tail link
    (0x211AF, "48890da2bcdd02"),            # mov %rcx,0x2dfce58(%rip)   head.prev = tail link
    (0x211B6, "4883c408"),                  # add rsp,8
    (0x211BA, "5b"),                        # pop rbx
    (0x211BB, "415e"),                      # pop r14
    (0x211BD, "415f"),                      # pop r15
    (0x211BF, "5d"),                        # pop rbp
    (0x211C0, "c3"),                        # retq
)

# (site, target) in address order. Four calls to four distinct targets.
CALLS = (
    (0x210F2, 0x3B310),   # CritSec init on manager+0x58
    (0x21108, 0xF8F3D0),  # __cxa_guard_acquire(&0x2dfce60)
    (0x21131, 0xF8F3E0),  # __cxa_guard_release(&0x2dfce60)
    (0x21166, 0x20D80),   # Register(element, manager) — the body, not 0x20d60
)

# (site, opcode bytes, target). Four branches, no jump.
BRANCHES = (
    (0x210FF, "75", 0x21136),  # jne  -> guard already released
    (0x2110F, "74", 0x21136),  # je   -> guard_acquire returned 0
    (0x21152, "74", 0x21186),  # je   -> empty list
    (0x2117D, "75", 0x21160),  # jne  -> drain loop
)

# The 63-byte guarded-static initialisation, and the seven RIP-relative displacement slots inside it
# (byte offsets from the block start). Masking them must make the three copies identical.
GUARD_BLOCKS = (0x210F7, 0x20E7F, 0x2171B)
GUARD_BLOCK_LEN = 63
GUARD_DISPLACEMENTS = ((2, 4), (13, 4), (18, 4), (29, 4), (36, 4), (54, 4), (59, 4))
# For each block start: (instruction site offset, instruction length, displacement offset inside the
# instruction, target). The same seven RIP-relative fields, so every one of them is pinned in all
# three copies.
GUARD_DISP_TARGETS = (
    (0, 6, 2, 0x2DFCE60),    # movb guard,%al
    (10, 7, 3, 0x2DFCE60),   # lea rdi,[guard]      (acquire argument)
    (17, 5, 1, 0xF8F3D0),    # call __cxa_guard_acquire
    (26, 7, 3, 0x2DFCE50),   # lea rax,[head]
    (33, 7, 3, 0x2DFCE60),   # lea rdi,[guard]      (release argument)
    (50, 8, 4, 0x2DFCE50),   # vmovdqu [head]       (the self-link store)
    (58, 5, 1, 0xF8F3E0),    # call __cxa_guard_release
)

HDR = ROOT / "native/core/concmd_manager_210b0.hpp"
SRC = ROOT / "native/core/concmd_manager_210b0.cpp"
TEST = ROOT / "native/tests/test_concmd_manager_210b0.cpp"
BASE_HDR = ROOT / "native/core/concmd_base.hpp"
BASE_SRC = ROOT / "native/core/concmd_base.cpp"
CMAKE = ROOT / "native/CMakeLists.txt"


def load_fdes():
    spec = importlib.util.spec_from_file_location("cfg_census", ROOT / "tools/lift/cfg_census.py")
    census = importlib.util.module_from_spec(spec)
    sys.modules["cfg_census"] = census
    spec.loader.exec_module(census)
    ranges, _duplicates = census.load_fde_ranges(FDE)
    return ranges


def branch_target(e: Elf, site: int, opcode_hex: str) -> int:
    opcode = bytes.fromhex(opcode_hex)
    require(e.read(site, len(opcode)) == opcode, f"0x{site:x} opcode")
    rel = e.read(site + 1, 1)[0]
    return site + 2 + struct.unpack("<b", bytes([rel]))[0]


def guard_block(e: Elf, start: int) -> bytes:
    block = bytearray(e.read(start, GUARD_BLOCK_LEN))
    for offset, size in GUARD_DISPLACEMENTS:
        for i in range(size):
            block[offset + i] = 0
    return bytes(block)


def elf_pins(e: Elf) -> None:
    """Every byte-level statement about the image. Called again under mutation overlays."""
    body = e.read(BODY_RVA, BODY_LEN)
    if not e.skip_hash:
        require(digest(body) == BODY_SHA256, "0x210b0 body (273 B) sha256")

    # ---- the body, instruction by instruction ---------------------------------------------------
    assembled = bytearray()
    cursor = BODY_RVA
    for site, hexstr in FRAGMENTS:
        require(site == cursor, f"fragment at 0x{site:x} is not contiguous (expected 0x{cursor:x})")
        want = bytes.fromhex(hexstr)
        require(e.read(site, len(want)) == want, f"0x{site:x} fragment {hexstr}")
        assembled += want
        cursor += len(want)
    require(cursor == BODY_RVA + BODY_LEN, f"fragments cover 0x{cursor:x}, not the whole body")
    require(bytes(assembled) == body, "the fragments do not reproduce the body")

    # ---- boundary: FDE, ret, pad, next function -------------------------------------------------
    found = load_fdes().find(BODY_RVA)
    require(found == (BODY_RVA, BODY_LEN), f"0x210b0 FDE {found} != (0x{BODY_RVA:x}, {BODY_LEN})")
    require(body[-1] == 0xC3, "retq at 0x211c0")
    e.bytes_at(0x211C1, "90" * 15, "15-byte pad after the body")
    require(e.read(0x211D0, 4) == bytes.fromhex("554889e5"), "0x211d0 starts the next function")
    next_fde = load_fdes().find(0x211D0)
    require(next_fde is not None and next_fde[0] == 0x211D0, "0x211d0 is the next FDE start")

    # ---- transfer census ------------------------------------------------------------------------
    for site, target in CALLS:
        require(e.call(site) == target, f"0x{site:x} call target (want 0x{target:x})")
    decoded = {site for site, _target in CALLS}
    raw_e8 = {BODY_RVA + i for i, byte in enumerate(body) if byte == 0xE8}
    require(raw_e8 == decoded, f"call-like e8 bytes {sorted(hex(x) for x in raw_e8)} != {sorted(hex(x) for x in decoded)}")
    require(not any(byte == 0xE9 for byte in body), "the body has no relative jump")
    for site, opcode, target in BRANCHES:
        require(branch_target(e, site, opcode) == target, f"0x{site:x} branch target (want 0x{target:x})")
    require(len(CALLS) + len(BRANCHES) == 8, "transfer census")
    # No manager lock/unlock anywhere in the constructor.
    require(0x3B390 not in {target for _site, target in CALLS}, "0x3b390 must not be called here")
    require(0x3B3C0 not in {target for _site, target in CALLS}, "0x3b3c0 must not be called here")

    # ---- the constructor's stores ---------------------------------------------------------------
    e.bytes_at(0x210C5, "498d7e58", "0x210c5: the CritSec handle is this+0x58")
    e.bytes_at(0x210C9, "49894608", "0x210c9: [this+0x08] = this+8")
    e.bytes_at(0x210CD, "49894610", "0x210cd: [this+0x10] = this+8")
    e.bytes_at(0x210D1, "49c7461800000000", "0x210d1: the root cell +0x18 is zeroed")
    e.bytes_at(0x210D9, "c4c17a7f4620", "0x210d9: 16 zero bytes at +0x20")
    e.bytes_at(0x210DF, "41c6462000", "0x210df: the redundant byte store at +0x20")
    e.bytes_at(0x210E4, "c4c17a7f4638", "0x210e4: 16 zero bytes at +0x38")
    e.bytes_at(0x210EA, "49c7464800000000", "0x210ea: [this+0x48] = 0")

    # ---- the guarded static list, and the shared 63-byte construct -------------------------------
    require(e.rip(0x210F7, 6, 2) == PENDING_GUARD, "0x210f7 tests the guard byte 0x2dfce60")
    require(e.lea(0x21101) == PENDING_GUARD, "0x21101 uses the guard byte 0x2dfce60")
    require(e.lea(0x21111) == PENDING_HEAD, "0x21111 initialises 0x2dfce50")
    require(e.lea(0x21118) == PENDING_GUARD, "0x21118 releases the guard byte 0x2dfce60")
    e.bytes_at(0x21129, "c5fa7f05", "0x21129 is the vmovdqu self-link store")
    require(e.rip(0x21129, 8, 4) == PENDING_HEAD, "0x21129 stores the self-links into 0x2dfce50")
    blocks = [guard_block(e, start) for start in GUARD_BLOCKS]
    require(len(set(blocks)) == 1,
            "the 0x210f7 / 0x20e7f / 0x2171b guard blocks differ beyond their displacements")
    # Every displacement in every copy resolves to the same target, so the masked comparison above is
    # not hiding a redirected block.
    for start in GUARD_BLOCKS:
        for site_rel, length, disp_rel, target in GUARD_DISP_TARGETS:
            got = e.rip(start + site_rel, length, disp_rel)
            require(got == target, f"0x{start + site_rel:x} displacement -> 0x{got:x}, want 0x{target:x}")

    # ---- the drain ------------------------------------------------------------------------------
    require(e.rip(0x21136, 7, 3) == PENDING_HEAD, "0x21136 reads the head 0x2dfce50")
    require(e.lea(0x2113D) == PENDING_SENTINEL, "0x2113d uses the sentinel 0x2dfce48")
    e.bytes_at(0x21144, "488d58f8", "0x21144: element = link - 8")
    e.bytes_at(0x21148, "4885c0", "0x21148: test the link")
    e.bytes_at(0x2114B, "480f44d8", "0x2114b: a null link collapses the -8 (cmoveq)")
    e.bytes_at(0x2114F, "4c39fb", "0x2114f: compare against the sentinel")
    e.bytes_at(0x21160, "4889df", "0x21160: rdi = the element")
    e.bytes_at(0x21163, "4c89f6", "0x21163: rsi = the manager argument")
    e.bytes_at(0x2116B, "488b4308", "0x2116b: next = element+0x08")
    e.bytes_at(0x2116F, "488d58f8", "0x2116f: element = next - 8")
    e.bytes_at(0x21173, "4885c0", "0x21173: test the next link")
    e.bytes_at(0x21176, "480f44d8", "0x21176: null-collapse again")

    # ---- the re-termination block ---------------------------------------------------------------
    require(e.rip(0x2117F, 7, 3) == PENDING_HEAD, "0x2117f re-reads the head 0x2dfce50")
    e.bytes_at(0x21186, "488d48f8", "0x21186: first element = head.next - 8")
    require(e.lea(0x2118D) == PENDING_HEAD, "0x2118d loads &head")
    e.bytes_at(0x21198, "488b4110", "0x21198: read element+0x10 (the predecessor link)")
    e.bytes_at(0x2119C, "488d48f8", "0x2119c: predecessor element = link - 8")
    e.bytes_at(0x211A7, "48895108", "0x211a7: mov %rdx,8(%rcx): predecessor->next = &head (rdx from 0x2118d)")
    e.bytes_at(0x211AB, "4883c108", "0x211ab: add $8,%rcx: rcx = predecessor + 8 = the predecessor link")
    e.bytes_at(0x211AF, "48890d", "0x211af: mov %rcx,disp(%rip): a STORE of rcx, not a load")
    require(e.rip(0x211AF, 7, 3) == PENDING_HEAD_PREV, "0x211af writes head.prev 0x2dfce58")

    # ---- prologue/epilogue completeness ---------------------------------------------------------
    e.bytes_at(0x21154, "6666662e0f1f840000000000", "0x21154: 12-byte alignment nop in the loop")
    e.bytes_at(0x211B6, "4883c408" "5b" "415e" "415f" "5d" "c3", "0x211b6: epilogue")

    # ---- the register entry the drain must not use ----------------------------------------------
    e.bytes_at(REGISTER_GUARD_RVA, "807f18007401c3488d055abedd02c6471801488b30e906000000",
               "0x20d60: the once-flag guard, the cell lea, the flag store and jmp 0x20d80")
    require(e.lea(0x20D67) == MANAGER_CELL, "0x20d67 leas the manager cell 0x2dfcbc8")
    require(e.jmp(0x20D75) == REGISTER_BODY_RVA, "0x20d75 tail-jumps to 0x20d80")
    require(e.read(REGISTER_BODY_RVA, 2) == bytes.fromhex("5548"), "0x20d80 starts with push rbp")
    body_fde = load_fdes().find(REGISTER_BODY_RVA)
    require(body_fde == (REGISTER_BODY_RVA, REGISTER_BODY_LEN),
            f"0x20d80 FDE {body_fde} != (0x{REGISTER_BODY_RVA:x}, {REGISTER_BODY_LEN})")

    # ---- the installer: cell written AFTER the constructor --------------------------------------
    e.bytes_at(0x243A1, "bf60000000", "0x243a1: new(0x60)")
    require(e.call(0x243A6) == 0x17070, "0x243a6 calls the 0x60-byte allocator")
    require(e.call(0x243B1) == BODY_RVA, "0x243b1 calls 0x210b0")
    require(e.lea(0x243B6) == MANAGER_CELL, "0x243b6 leas the manager cell 0x2dfcbc8")
    e.bytes_at(0x243F5, "4d8937", "0x243f5: mov %r14,(%r15) — the install store")
    require(0x243B1 < 0x243F5, "the constructor runs before the cell is written")
    installer_fde = load_fdes().find(INSTALLER_RVA)
    require(installer_fde is not None and installer_fde[0] == INSTALLER_RVA,
            "0x242c0 is an FDE start (the installer)")

    # ---- the two guard thunks are PLT-style stubs ------------------------------------------------
    e.bytes_at(0xF8F3D0, "ff25020c6700", "0xf8f3d0 jumps through the GOT")
    e.bytes_at(0xF8F3E0, "ff25fa0b6700", "0xf8f3e0 jumps through the GOT")
    critsec_fde = load_fdes().find(0x3B310)
    require(critsec_fde == (0x3B310, 107), f"0x3b310 FDE {critsec_fde}")


def native_pins() -> None:
    hdr = HDR.read_text()
    src = SRC.read_text()
    test = TEST.read_text()
    base_h = BASE_HDR.read_text()
    base_c = BASE_SRC.read_text()
    cmake = CMAKE.read_text()

    require("0x210b0" in hdr and "ConCmdManager" in hdr, "header must name the RVA and the class")
    require(const_value(hdr, "kConCmdManager210b0Rva") == BODY_RVA, "RVA pin")
    require(const_value(hdr, "kConCmdManager210b0Bytes") == BODY_LEN, "273-byte body pin")
    require(BODY_SHA256 in hdr, "the body digest must stay in the header")
    for name, want in (("kConCmdManager210b0StorageBytes", 0x60),
                       ("kConCmdManager210b0SelfLinkAOffset", 0x08),
                       ("kConCmdManager210b0SelfLinkBOffset", 0x10),
                       ("kConCmdManager210b0RootOffset", 0x18),
                       ("kConCmdManager210b0ZeroSpanAOffset", 0x20),
                       ("kConCmdManager210b0ZeroByteOffset", 0x20),
                       ("kConCmdManager210b0SizeOffset", 0x28),
                       ("kConCmdManager210b0ZeroSpanBOffset", 0x38),
                       ("kConCmdManager210b0OwnedPointerOffset", 0x38),
                       ("kConCmdManager210b0ZeroQwordOffset", 0x48),
                       ("kConCmdManager210b0MutexOffset", 0x58),
                       ("kConCmdManager210b0ZeroSpanBytes", 16),
                       ("kConCmdManager210b0GuardBlockBytes", GUARD_BLOCK_LEN)):
        require(const_value(hdr, name) == want, f"offset pin {name}")
    require("kConCmdManager210b0SentinelDelta = -8;" in hdr, "sentinel delta pin (-8)")
    require("NOT CLAIMED" in hdr and "engine readiness" in hdr,
            "the header must keep its explicit non-claims")
    require("HOST-ONLY ADDITIONS" in hdr, "the header must label its host-only additions")
    require("0x243f5" in hdr and "0x243b1" in hdr,
            "the header must state the install order (constructor before cell store)")

    require("ensure_concmd_pending_list" in src, "the drain must share the guarded init")
    require("register_concmd_body_20d80" in src, "the drain must use the register body 0x20d80")
    require("critsec_init_at" in src, "the constructor must initialise the reviewed CritSec")
    require("PROBE" not in src and "overfit" not in src,
            "the constructor model must not carry a result-fitted probe")
    require("store_ptr(tail, kPendingNextOffset, &head)" in src,
            "0x211a7 must be modelled as the store [tail+8] = &head")
    require("head.prev = static_cast<ConCmdListNode*>(prev_link)" in src,
            "0x211ab/0x211af must be modelled as head.prev = the predecessor link")
    require("critsec_lock_at" not in src and "critsec_unlock_at" not in src,
            "the constructor must not lock: the body takes no lock")
    require("register_concmd_20d60" not in src,
            "the drain must not use the once-flag guard entry 0x20d60")
    require("reinterpret_cast<std::uintptr_t>(0x210b0" not in src,
            "the RVA may only appear as an identity tag")

    require("register_concmd_body_20d80" in base_h and "register_concmd_body_20d80" in base_c,
            "concmd_base must expose the body the drain calls")
    require("return register_concmd_body_20d80(cmd, registry.manager.manager, registry);" in base_c,
            "0x20d60 must be the guard plus that body")
    require("if (manager != nullptr)" in base_c,
            "the body must branch on its manager argument, not on the cell")

    # Fixture honesty and fail-visible coverage.
    require("SYNTHETIC ONLY" in test, "the fixture must state that its registry is synthetic")
    require('"shadowDemoMode"' not in test and '"r_fps"' not in test,
            "the fixture must not build either pinned retail name")
    require("NOT a model of 0x211d0" in test, "the fixture map must be labelled as a stand-in")
    for needle in ("ConCmdManagerCtorFail::null_manager", "ConCmdManagerCtorFail::mutex_init",
                   "ConCmdManagerCtorFail::register_refused", "ConCmdManagerCtorFail::null_head_link",
                   "ConCmdManagerCtorFail::null_drained_link", "ConCmdManagerCtorFail::null_tail_link"):
        require(needle in test, f"the fixture must cover {needle}")
    require("CHECK(!created.list_emptied)" in test,
            "the fixture must pin the general re-termination form (a list that is not emptied)")
    require("6b. the two re-termination stores are observable" in test,
            "the fixture must pin both re-termination stores with a stale head.prev and a poisoned tail")
    require("log.cell_seen_null" in test and "map.cell_seen_null" in test,
            "the fixture must prove the drain runs while the manager cell is still null")
    require("alpha.pending.next == &beta.pending" in test,
            "the fixture must prove the drain leaves the queued nodes linked")
    require("find_concmd_21620" in test, "the fixture must drive the native 0x21620 after the drain")

    for needle in ("core/concmd_manager_210b0.cpp", "tests/test_concmd_manager_210b0.cpp",
                   "test_concmd_manager_210b0"):
        require(needle in cmake, f"cmake missing {needle}")


def mutation_self_test(e: Elf) -> int:
    e.skip_hash = True
    e.recording = True
    e.pins = set()
    elf_pins(e)
    e.recording = False
    survivors = 0
    mutants = 0
    for addr, size in sorted(e.pins):
        for byte_addr in sorted({addr, addr + size - 1}):
            mutants += 1
            e.overlay = {}  # pristine value first: never compute a mutant from a mutated byte
            e.overlay = {byte_addr: e.raw(byte_addr, 1)[0] ^ 0x01}
            try:
                elf_pins(e)
            except SystemExit:
                pass
            else:
                survivors += 1
                print(f"mutation survived at 0x{byte_addr:x} (pin 0x{addr:x}+{size})", file=sys.stderr)
    e.overlay = {}
    e.skip_hash = False
    require(survivors == 0, f"{survivors} ELF mutations survived")
    return mutants


def main() -> int:
    # ---- native contract (always checked, even without the private image) ------------------------
    native_pins()

    if not (ROOT / "local/re/eboot.elf").is_file() or not FDE.is_file():
        print("SKIP check_concmd_manager_210b0: private image not present (module wiring checked)")
        return 0

    raw = (ROOT / "local/re/eboot.elf").read_bytes()
    require(digest(raw) == ELF_SHA256, "unexpected ELF SHA256")

    e = Elf()
    e.recording = True
    elf_pins(e)
    mutants = mutation_self_test(e)

    print(f"OK check_concmd_manager_210b0: 0x210b0 273 B body (FDE 273) as {len(FRAGMENTS)} contiguous "
          "instruction fragments, 4 calls / 4 targets with no lock, the shared 63-byte guarded list "
          "construct, the FIFO drain through 0x20d80 (not 0x20d60) with the manager argument, the "
          "re-termination block 0x2117f..0x211af, and the install order 0x243b1 < 0x243f5; "
          f"{mutants} mutants rejected. No construction result, engine bind or boot behaviour is "
          "proven.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
