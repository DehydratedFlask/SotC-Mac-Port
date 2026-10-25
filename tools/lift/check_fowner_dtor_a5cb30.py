#!/usr/bin/env python3
"""ELF-tie the lifted row-9 owner destructor `0xa5cb30` (`kOwnerMemberRows[9]`).

Private: needs `local/re/eboot.elf`. Read-only and offline: instructions are decoded and byte
ranges are hashed; **no guest byte is executed**. No emulator, interpreter, JIT or guest runtime.

Layers:
  1. body extent/hash, prologue/epilogue, padding, and no stack canary;
  2. the ordered direct-call table (19 sites / 7 targets) and the three indirect vtable calls;
  3. every load/store site: event flags, poly field, five vector spans, marker *field* loads, the
     tree/shared/free/refcount sites, the drain and the `+0x20`/`+0x18` head gate;
  4. loop strides and the `begin != null && begin != marker` free condition;
  5. the callee shapes that justify the hook contracts: 0x3b440 (delete event flag PLT),
     0x170b0/0x170c0 (tag 1/2 free), 0xb09690 (clear + free), 0xf590 (shared release),
     0xa62340 (empty-drain tail stores), 0x14d2d0/0x14ccc0 (refcount pair);
  6. the three callers (`0x255848`, `0x256cf0`, `0xa5be85`) and the row's dtor-then-free pairing;
  7. the native header/source/test tokens and their failure surface.
"""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_PATH = ROOT / "local/re/eboot.elf"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"
BODY_START = 0xA5CB30
BODY_END = 0xA5CD82  # exclusive
BODY_SIZE = 0x252
BODY_SHA256 = "0b5c450169b29ead0eed06b88cad7993ad01d310dc7142bc3a21302a727f0ff8"

HEADER = ROOT / "native/core/owner_dtor_a5cb30.hpp"
SOURCE = ROOT / "native/core/owner_dtor_a5cb30.cpp"
TEST = ROOT / "native/tests/test_owner_dtor_a5cb30.cpp"

# (site, target) in execution order. The three indirect calls are pinned separately by bytes.
CALLS = [
    (0xA5CB44, 0x3B440),  # event flag +0xa510
    (0xA5CB50, 0x3B440),  # event flag +0xa500
    (0xA5CBBC, 0x170C0),  # free +0x8bd0 buffer
    (0xA5CC0C, 0x170C0),  # free +0x72a8 buffer
    (0xA5CC18, 0xB09690), # tree +0x7270
    (0xA5CC24, 0xF590),   # shared +0x7250
    (0xA5CC44, 0xF590),   # +0x6fe8 element +0x28
    (0xA5CC4C, 0xF590),   # +0x6fe8 element +0x00
    (0xA5CC72, 0x170C0),  # free +0x6fe8 buffer
    (0xA5CC93, 0xF590),   # +0x6ac0 element
    (0xA5CCB9, 0x170C0),  # free +0x6ac0 buffer
    (0xA5CCD3, 0x170C0),  # free +0x68a0 buffer
    (0xA5CCDF, 0xA62340), # drain +0x5c60
    (0xA5CD04, 0xF590),   # +0x28 element +0x18
    (0xA5CD12, 0x14D2D0), # +0x28 conditional release
    (0xA5CD1E, 0x14CCC0), # +0x28 unconditional release
    (0xA5CD45, 0x170C0),  # free +0x28 buffer
    (0xA5CD5C, 0x14D2D0), # head conditional release
    (0xA5CD6A, 0x14CCC0), # head unconditional release
]

# Ordered native write sites (address, byte pattern, what). The drain's three tail stores live in
# 0xa62340 and are pinned there.
STORES = [
    (0xA5CB67, "49c786f8a4000000000000", "clear +0xa4f8"),
    (0xA5CD17, "c6430800", "clear the +0x28 element flag"),
    (0xA5CD23, "48c70300000000", "clear the +0x28 element pointer"),
    (0xA5CD61, "41c6461800", "clear the head flag +0x18"),
    (0xA5CD6F, "49c7461000000000", "clear the head pointer +0x10"),
]

PT_LOAD = 1
PT_SCE_RELRO = 0x61000010


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def load_segments(elf: bytes) -> list[tuple[int, int, int]]:
    e_phoff = struct.unpack_from("<Q", elf, 0x20)[0]
    e_phentsize, e_phnum = struct.unpack_from("<HH", elf, 0x36)
    out = []
    for i in range(e_phnum):
        base = e_phoff + i * e_phentsize
        p_type = struct.unpack_from("<I", elf, base)[0]
        if p_type not in (PT_LOAD, PT_SCE_RELRO):
            continue
        p_offset, p_vaddr = struct.unpack_from("<QQ", elf, base + 8)
        p_filesz = struct.unpack_from("<Q", elf, base + 32)[0]
        out.append((p_vaddr, p_offset, p_filesz))
    return out


def read_rva(elf: bytes, segments, rva: int, size: int) -> bytes:
    for vaddr, offset, filesz in segments:
        if vaddr <= rva and rva + size <= vaddr + filesz:
            start = offset + (rva - vaddr)
            return elf[start:start + size]
    raise SystemExit(f"FAIL rva {rva:#x} (+{size:#x}) is not file-backed")


def u8(elf: bytes, segments, rva: int) -> int:
    return read_rva(elf, segments, rva, 1)[0]


def rel32_target(elf: bytes, segments, site: int) -> int:
    rel = struct.unpack("<i", read_rva(elf, segments, site + 1, 4))[0]
    return site + 5 + rel


def rip_target(elf: bytes, segments, site: int, disp_off: int, length: int) -> int:
    disp = struct.unpack("<i", read_rva(elf, segments, site + disp_off, 4))[0]
    return site + length + disp


def pin(elf: bytes, segments, addr: int, pattern: str, what: str) -> None:
    size = len(pattern) // 2
    require(read_rva(elf, segments, addr, size) == bytes.fromhex(pattern), f"{what} @ {addr:#x}")


def check_body(elf: bytes, segments) -> None:
    body = read_rva(elf, segments, BODY_START, BODY_SIZE)
    require(len(body) == BODY_SIZE, f"body size {len(body):#x}")
    require(hashlib.sha256(body).hexdigest() == BODY_SHA256, "body sha256")
    require(body[0] == 0x55 and body[-1] == 0xC3, "push rbp ... ret boundaries")
    pin(elf, segments, BODY_START, "554889e54157415653504989fe",
        "prologue: push rbp; mov rbp,rsp; push r15/r14/rbx/rax; mov r14,rdi")
    pin(elf, segments, 0xA5CD77, "4883c4085b415e415f5d", "epilogue restore")
    require(read_rva(elf, segments, BODY_END, 14) == b"\x90" * 14, "14 B 0x90 padding")
    require(read_rva(elf, segments, 0xA5CD90, 8) == bytes.fromhex("554889e541574156"),
            "next body 0xa5cd90")
    require(b"\x64\x48\x8b\x04\x25" not in body, "unexpected stack canary")


def check_calls(elf: bytes, segments) -> None:
    actual = []
    for site, target in CALLS:
        require(u8(elf, segments, site) == 0xE8, f"call opcode at {site:#x}")
        require(rel32_target(elf, segments, site) == target, f"call {site:#x} -> {target:#x}")
        actual.append((site, target))
    require([site for site, _ in actual] == sorted(site for site, _ in actual),
            "direct calls not in ascending/execution order")
    require(len(CALLS) == 19, "direct call count")

    # Three indirect calls: [object] vtable +8, then [element] vtable slot 0 twice.
    pin(elf, segments, 0xA5CB61, "488b07", "load the +0xa4f8 vptr")
    pin(elf, segments, 0xA5CB64, "ff5008", "call [vptr+8] for +0xa4f8")
    pin(elf, segments, 0xA5CB90, "488b03", "load the +0x8bd0 element vptr")
    pin(elf, segments, 0xA5CB96, "ff10", "call [vptr] for +0x8bd0")
    pin(elf, segments, 0xA5CBE0, "488b03", "load the +0x72a8 element vptr")
    pin(elf, segments, 0xA5CBE6, "ff10", "call [vptr] for +0x72a8")


def check_event_order(elf: bytes, segments) -> None:
    pin(elf, segments, 0xA5CB3D, "498dbe10a50000", "rdi = object + 0xa510")
    pin(elf, segments, 0xA5CB49, "498dbe00a50000", "rdi = object + 0xa500")
    require(0xA5CB3D < 0xA5CB49, "event flag order: +0xa510 before +0xa500")


def check_poly(elf: bytes, segments) -> None:
    pin(elf, segments, 0xA5CB55, "498bbef8a40000", "load +0xa4f8")
    pin(elf, segments, 0xA5CB5C, "4885ff", "test the polymorphic pointer")
    pin(elf, segments, 0xA5CB5F, "7411", "null => skip the vcall and the clear")
    require(STORES[0][0] == 0xA5CB67, "poly clear site order")


def check_vectors(elf: bytes, segments) -> None:
    # ---- +0x8bd0: begin/end at +0x8bd0/+0x8bd8, marker field at +0x8bf0. ----------------------
    pin(elf, segments, 0xA5CB72, "498b9ed08b0000", "begin +0x8bd0")
    pin(elf, segments, 0xA5CB79, "4d8bbed88b0000", "end +0x8bd8")
    pin(elf, segments, 0xA5CB98, "4881c3c8000000", "stride 0xc8 (+0x8bd0)")
    pin(elf, segments, 0xA5CBA4, "498b9ed08b0000", "reload begin before the free")
    pin(elf, segments, 0xA5CBB0, "49399ef08b0000", "compare begin with the *value* at +0x8bf0")
    pin(elf, segments, 0xA5CBB7, "7408", "inline marker => no free")
    require(rel32_target(elf, segments, 0xA5CBBC) == 0x170C0, "free +0x8bd0 -> 0x170c0")

    # ---- +0x72a8: same shape, marker field at +0x72c8. ---------------------------------------
    pin(elf, segments, 0xA5CBC1, "498b9ea8720000", "begin +0x72a8")
    pin(elf, segments, 0xA5CBC8, "4d8bbeb0720000", "end +0x72b0")
    pin(elf, segments, 0xA5CBE8, "4881c3c8000000", "stride 0xc8 (+0x72a8)")
    pin(elf, segments, 0xA5CBF4, "498b9ea8720000", "reload begin before the free")
    pin(elf, segments, 0xA5CC00, "49399ec8720000", "compare begin with the *value* at +0x72c8")

    # ---- +0x6fe8: 0x48 stride, hi holder (+0x28) before lo holder (+0x00). -------------------
    pin(elf, segments, 0xA5CC29, "498b9ee86f0000", "begin +0x6fe8")
    pin(elf, segments, 0xA5CC30, "4d8bbef06f0000", "end +0x6ff0")
    pin(elf, segments, 0xA5CC40, "488d7b28", "shared release of element +0x28 first")
    pin(elf, segments, 0xA5CC49, "4889df", "then the element +0x00")
    pin(elf, segments, 0xA5CC51, "4883c348", "stride 0x48")
    pin(elf, segments, 0xA5CC5A, "498b9ee86f0000", "reload begin before the free")
    pin(elf, segments, 0xA5CC66, "49399e08700000", "compare begin with the *value* at +0x7008")

    # ---- +0x6ac0: 0x28 stride, one holder per element. ---------------------------------------
    pin(elf, segments, 0xA5CC77, "498b9ec06a0000", "begin +0x6ac0")
    pin(elf, segments, 0xA5CC7E, "4d8bbec86a0000", "end +0x6ac8")
    pin(elf, segments, 0xA5CC98, "4883c328", "stride 0x28")
    pin(elf, segments, 0xA5CCA1, "498b9ec06a0000", "reload begin before the free")
    pin(elf, segments, 0xA5CCAD, "49399ee06a0000", "compare begin with the *value* at +0x6ae0")

    # ---- +0x68a0: POD buffer, no element loop, marker field +0x68c0. --------------------------
    pin(elf, segments, 0xA5CCBE, "498bbea0680000", "load +0x68a0 begin")
    pin(elf, segments, 0xA5CCC5, "4885ff", "test begin")
    pin(elf, segments, 0xA5CCCA, "4939bec0680000", "compare begin with the *value* at +0x68c0")
    pin(elf, segments, 0xA5CCC8, "740e", "null => no free")

    # ---- +0x28: 0x38 stride, shared +0x18, flag +8, ref pair, marker field +0x48. -------------
    pin(elf, segments, 0xA5CCE4, "498b5e28", "begin +0x28")
    pin(elf, segments, 0xA5CCE8, "4d8b7e30", "end +0x30")
    pin(elf, segments, 0xA5CD00, "488d7b18", "shared holder = element +0x18")
    pin(elf, segments, 0xA5CD09, "807b0800", "element flag byte +8")
    pin(elf, segments, 0xA5CD0F, "488b3b", "conditional release target")
    pin(elf, segments, 0xA5CD1B, "488b3b", "unconditional release target")
    pin(elf, segments, 0xA5CD2A, "4883c338", "stride 0x38")
    pin(elf, segments, 0xA5CD33, "498b5e28", "reload begin before the free")
    pin(elf, segments, 0xA5CD3C, "49395e48", "compare begin with the *value* at +0x48")


def check_stores(elf: bytes, segments) -> None:
    for site, pattern, what in STORES:
        pin(elf, segments, site, pattern, what)
    require([site for site, _, _ in STORES] == sorted(site for site, _, _ in STORES),
            "store sites not in ascending/execution order")
    pin(elf, segments, 0xA5CB5F, "7411", "the poly clear is conditional")


def check_drain(elf: bytes, segments) -> None:
    pin(elf, segments, 0xA5CCD8, "498dbe605c0000", "rdi = object + 0x5c60")
    require(rel32_target(elf, segments, 0xA5CCDF) == 0xA62340, "drain -> 0xa62340")
    # 0xa62340: [base+0x10] is the flag word; null => the spin and lane walk are dead.
    pin(elf, segments, 0xA6234E, "4d8b7e10", "r15 = [base+0x10]")
    pin(elf, segments, 0xA62352, "4d85ff", "test the flag word")
    pin(elf, segments, 0xA62355, "7445", "null => skip the spin to the tail")
    pin(elf, segments, 0xA62412, "49c7462000000000", "tail: [base+0x20] = 0")
    pin(elf, segments, 0xA6241A, "41c7463800000000", "tail: [base+0x38] = 0")
    pin(elf, segments, 0xA62422, "49c74630ffffffff", "tail: [base+0x30] = -1")
    require(0xA62412 < 0xA6241A < 0xA62422, "drain tail store order")


def check_head(elf: bytes, segments) -> None:
    pin(elf, segments, 0xA5CD4A, "41807e2000", "read the +0x20 gate")
    pin(elf, segments, 0xA5CD4F, "7426", "gate 0 => skip the head block")
    pin(elf, segments, 0xA5CD51, "41807e1800", "read the +0x18 flag")
    pin(elf, segments, 0xA5CD56, "740e", "flag 0 => skip the conditional release")
    pin(elf, segments, 0xA5CD58, "498b7e10", "head pointer qword")
    pin(elf, segments, 0xA5CD66, "498b7e10", "reload the head pointer for the release")
    pin(elf, segments, 0xA5CD6A, "e851ff6eff", "head release call encoding")


def check_callees(elf: bytes, segments) -> None:
    # 0x3b440: mov rdi,[rdi]; jmp 0xf8f960 (sceKernelDeleteEventFlag PLT, GOT 0x16002a0).
    pin(elf, segments, 0x3B440, "488b3f", "event dtor loads the handle")
    pin(elf, segments, 0x3B443, "e91845f500", "event dtor jumps to the PLT")
    require(rel32_target(elf, segments, 0x3B443) == 0xF8F960, "0x3b440 -> 0xf8f960")
    pin(elf, segments, 0xF8F960, "ff253a096700", "delete-event-flag PLT stub")
    require(rip_target(elf, segments, 0xF8F960, 2, 6) == 0x16002A0,
            "delete-event-flag GOT slot 0x16002a0")

    # 0x170b0 (tag 1) / 0x170c0 (tag 2) both tail into the tagged deallocate dispatcher 0x12af0.
    pin(elf, segments, 0x170B0, "be01000000", "free tag 1")
    pin(elf, segments, 0x170C0, "be02000000", "free tag 2")
    require(rel32_target(elf, segments, 0x170B5) == 0x12AF0, "0x170b0 -> 0x12af0")
    require(rel32_target(elf, segments, 0x170C5) == 0x12AF0, "0x170c0 -> 0x12af0")

    # 0xb09690: clear (0xb0a080) then free [at+8] via 0x170b0.
    require(rel32_target(elf, segments, 0xB09699) == 0xB0A080, "0xb09690 -> clear 0xb0a080")
    pin(elf, segments, 0xB0969E, "488b7b08", "tree node = [at+8]")
    require(rel32_target(elf, segments, 0xB096A8) == 0x170B0, "tree node free -> 0x170b0")

    # 0xf590: [holder+0x18] control block, refcount decrement, last-ref destroy and free.
    pin(elf, segments, 0xF599, "488b4318", "shared control = [holder+0x18]")
    pin(elf, segments, 0xF5A2, "48c7c1ffffff", "shared decrement -1")
    pin(elf, segments, 0xF5AE, "4883f901", "compare the previous count with 1")
    require(rel32_target(elf, segments, 0xF5B9) == 0x12AF0, "shared destroy -> 0x12af0")
    require(rel32_target(elf, segments, 0xF5CD) == 0x170B0, "shared control free -> 0x170b0")

    # 0x14d2d0: decrement [object+0x40], last ref only.
    pin(elf, segments, 0x14D2E3, "f00fc14740", "conditional refcount xadd [object+0x40]")
    pin(elf, segments, 0x14D2E8, "83f801", "conditional last-ref compare")
    # 0x14ccc0: decrement [object+0x30], last ref only.
    pin(elf, segments, 0x14CCE7, "f0410fc14e30", "refcount xadd [object+0x30]")
    pin(elf, segments, 0x14CCED, "83f901", "last-ref compare")


def check_callers(elf: bytes, segments) -> None:
    for site in (0x255848, 0x256CF0, 0xA5BE85):
        require(u8(elf, segments, site) == 0xE8, f"caller opcode at {site:#x}")
        require(rel32_target(elf, segments, site) == BODY_START, f"caller {site:#x}")
    require(0x255848 < 0x256CF0 < 0xA5BE85, "caller order")
    # Row teardown: dtor then free 0x170b0, then clear the pointer slot.
    require(rel32_target(elf, segments, 0x256CF0) == 0xA5CB30, "row dtor call")
    require(rel32_target(elf, segments, 0x256CF8) == 0x170B0, "row free call")
    pin(elf, segments, 0x256CD8, "4c8d2511230a03", "row pointer slot lea")
    require(rip_target(elf, segments, 0x256CD8, 3, 7) == 0x32F8FF0,
            "row slot 0x32f8ff0")


def check_native_sources() -> None:
    header = HEADER.read_text()
    for token in (
        "kOwnerDtorA5CB30InstrCount = 142",
        "kOwnerDtorA5CB30CallCount = 22",
        "kOwnerDtorA5CB30DirectCallCount = 19",
        "kOwnerA5C850HeadGateOffset = 0x20",
        "kOwnerA5C850HeadFlagOffset = 0x18",
        "kOwnerA5C850HeadPtrOffset = 0x10",
        "kOwnerA5C850VectorMarkerSlot = 0x20",
        "event_flag_destroy",
        "poly_release",
        "element_release",
        "tree_destroy",
        "shared_release",
        "buffer_free",
        "drain_armed",
        "ref_release_conditional",
        "ref_release",
        '"event_flag_1"',
        '"vector_8bd0_element"',
        '"vector_6fe8_element"',
        '"vector_28_conditional"',
        '"head_release"',
        "0xa62340",
        "not lifted",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in (
        "owner_dtor_a5cb30",
        "kOwnerA5C850Event1Offset",
        "kOwnerA5C850Event0Offset",
        "kOwnerA5C850PolyOffset",
        "kOwnerA5C850TreeOffset",
        "kOwnerA5C850SharedOffset",
        "kOwnerA5C850LaneAtomicOffset",
        "kOwnerA5C850HeadGateOffset",
        "kOwnerA5C850HeadFlagOffset",
        "kOwnerA5C850HeadPtrOffset",
        "kOwnerA5C850DrainWordOffset",
        "kOwnerA5C850DrainTailMaskOffset",
        "kOwnerA5C850DrainTailSeqOffset",
        "kOwnerA5C850DrainTailCountOffset",
        "store_u64",
        "store_ptr",
        "span_is_exact",
        '"vector_72a8_span"',
        '"vector_6ac0_element"',
        '"vector_68a0_buffer"',
        '"drain"',
        '"head_conditional"',
    ):
        require(token in source, f"source token {token!r}")
    require("memset(o" not in source, "source must not memset the object")
    require("std::atomic" not in source, "the empty drain path is plain stores, not a spin")

    test = TEST.read_text()
    for token in (
        "owner_ctor_a5c850",
        "shared_release_f590",
        "expect_steps",
        "section_constructed",
        "section_armed",
        "section_missing_hooks",
        "section_prefixes",
        "section_double_destroy",
        "kOwnerDtorA5CB30CallCount == 22",
        "0xA524",
        "vector_8bd0_span",
        "tree_destroy",
    ):
        require(token in test, f"test token {token!r}")


def main() -> int:
    require(ELF_PATH.is_file(), f"{ELF_PATH} is absent (private game image required)")
    elf = ELF_PATH.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    segments = load_segments(elf)

    check_body(elf, segments)
    check_event_order(elf, segments)
    check_poly(elf, segments)
    check_vectors(elf, segments)
    check_stores(elf, segments)
    check_calls(elf, segments)
    check_drain(elf, segments)
    check_head(elf, segments)
    check_callees(elf, segments)
    check_callers(elf, segments)
    check_native_sources()

    print(
        f"OK check_fowner_dtor_a5cb30: body 0x252 sha256 {BODY_SHA256[:12]}..., "
        f"19 direct + 3 indirect calls, 5 native stores + 3 drain tail stores, "
        f"event +0xa510->+0xa500, 6 vector teardowns, dtor-then-free row callers "
        f"0x255848/0x256cf0/0xa5be85; guest bytes NOT executed"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
