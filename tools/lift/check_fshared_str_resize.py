#!/usr/bin/env python3
"""ELF-tie the 0xf3c0 SharedStr resize and the 0xf6b0 NUL helper.

Private: needs local/re/eboot.elf. Pins both body hashes, the capacity comparison, the four
alignment constants, every allocation call target, the tag-0/tag-1 frees, and the exact return
paths. The return-path pins record a correction to shared-string-20/report.md §3.2: only the
capacity-short path (0xf513) sets `r15b = 1`; paths 1 and 2 zero `r15d` and return 0.
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
RESIZE_RVA = 0xF3C0
RESIZE_SIZE = 459
RESIZE_SHA256 = "980c6911da8fad573162fd1d97f77b1dc20677bb7eedbee3e7e5fc8fe25227fb"
TERMINATE_RVA = 0xF6B0
TERMINATE_SIZE = 32
TERMINATE_SHA256 = "bfaf9b09a6f5f0345f8341cbc75da04f7ebe4dcf9ae0d697d86bbac82da25be1"
HEADER = ROOT / "native/core/shared_str_resize_f3c0.hpp"
SOURCE = ROOT / "native/core/shared_str_resize_f3c0.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def call_target(img, addr: int) -> int:
    raw = img.read(addr, 5)
    require(raw[0] == 0xE8, f"call opcode at {addr:#x}")
    return addr + 5 + struct.unpack("<i", raw[1:5])[0]


def rel32_target(img, addr: int, opcode: int) -> int:
    raw = img.read(addr, 5)
    require(raw[0] == opcode, f"rel32 opcode at {addr:#x}")
    return addr + 5 + struct.unpack("<i", raw[1:5])[0]


def short_target(img, addr: int) -> int:
    raw = img.read(addr, 2)
    require(raw[0] == 0xEB, f"short jmp opcode at {addr:#x}")
    return addr + 2 + struct.unpack("<b", raw[1:2])[0]


def main() -> int:
    elf = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf)
    img = d.Image(elf, ph)

    resize = img.read(RESIZE_RVA, RESIZE_SIZE)
    terminate = img.read(TERMINATE_RVA, TERMINATE_SIZE)
    require(hashlib.sha256(resize).hexdigest() == RESIZE_SHA256, "0xf3c0 sha256")
    require(hashlib.sha256(terminate).hexdigest() == TERMINATE_SHA256, "0xf6b0 sha256")
    require(resize[-1:] == b"\xc3", "0xf3c0 ends in ret at 0xf58a")
    require(terminate[-1:] == b"\xc3", "0xf6b0 ends in ret at 0xf6cf")

    for addr, pattern, what in (
        (0xF3C0, "554889e541574156415541545350", "0xf3c0 prologue"),
        (0xF3CE, "4889fb", "holder -> rbx"),
        (0xF3D1, "4989f6", "newLen -> r14"),
        (0xF3D4, "488b4318", "load control +0x18"),
        (0xF3D8, "4885c0", "test control"),
        (0xF3DB, "7431", "je control == 0 path"),
        (0xF3DD, "488b00", "load *control"),
        (0xF3E0, "4883f801", "cmp *control, 1"),
        (0xF3E4, "0f8586000000", "jne shared path"),
        # Capacity proof: cmp qword [rbx+0x10], r14 ; jae path 3.
        (0xF3EA, "4c397310", "cmp qword [rbx+0x10],r14"),
        (0xF3EE, "0f8316010000", "jae 0xf50a (capacity sufficient)"),
        # Path 4 thresholds and the three alignment constants.
        (0xF3F4, "4981fe80000000", "cmp newLen, 0x80"),
        (0xF3FB, "0f8712010000", "ja 0xf513 (medium/large class)"),
        (0xF401, "498d7e10", "lea rdi,[r14+0x10] (small slack)"),
        (0xF405, "4883e7f0", "and rdi,-0x10 (align16)"),
        (0xF42A, "4883e7fc", "and rdi,-0x4 (align4)"),
        (0xF513, "4981fe00040000", "cmp newLen, 0x400"),
        (0xF51A, "7710", "ja 0xf52c (large class)"),
        (0xF51C, "498dbe00010000", "lea rdi,[r14+0x100] (medium slack)"),
        (0xF523, "4881e700ffffff", "and rdi,-0x100 (align256)"),
        (0xF52C, "498dbe00040000", "lea rdi,[r14+0x400] (large slack)"),
        (0xF533, "4881e700fcffff", "and rdi,-0x400 (align1024)"),
        # Path 2 release: lock xadd -1, cmp previous, tag-0 then tag-1 frees.
        (0xF4B0, "488b4318", "reload control before release"),
        (0xF4B9, "48c7c1ffffffff", "mov rcx,-1"),
        (0xF4C0, "f0480fc108", "lock xadd qword [rax],rcx"),
        (0xF4C5, "4883f901", "cmp rcx,1"),
        (0xF4CB, "488b3b", "mov rdi,[rbx] old data"),
        (0xF4CE, "31f6", "xor esi,esi (tag 0)"),
        (0xF4D5, "488b7b18", "reload control for tag-1 free"),
        (0xF4D9, "4885ff", "test control"),
        (0xF4DC, "7405", "je skip control free"),
        # Path 1 control init.
        (0xF45C, "488708", "xchg qword [rax],rcx (*control = 1)"),
        (0xF45F, "48894318", "store control +0x18"),
        # Path 2 control init.
        (0xF501, "488708", "xchg qword [rax],rcx (new control = 1)"),
        (0xF504, "48894318", "store new control +0x18"),
        # Path 4 old-data free and install.
        (0xF561, "488b3b", "mov rdi,[rbx] old data"),
        (0xF564, "31f6", "xor esi,esi (tag 0)"),
        (0xF56B, "4c893b", "store new data +0x00"),
        (0xF56E, "4c896310", "store capacity +0x10"),
        (0xF572, "4c897308", "store size +0x08"),
        # Return paths: only 0xf576 sets r15b = 1.
        (0xF430, "4531ff", "path 1: xor r15d,r15d"),
        (0xF46B, "e909010000", "path 1: jmp 0xf579"),
        (0xF50A, "4c897308", "path 3: size = newLen"),
        (0xF50E, "4531ff", "path 2/3: xor r15d,r15d"),
        (0xF511, "eb66", "path 2/3: jmp 0xf579"),
        (0xF576, "41b701", "path 4: mov r15b,1"),
        (0xF579, "4489f8", "epilogue: mov eax,r15d"),
        # 0xf6b0: call resize, then data[size] = 0.
        (0xF6B0, "554889e553504889fb", "0xf6b0 prologue"),
        (0xF6BE, "488b0b", "mov rcx,[rbx] data"),
        (0xF6C1, "488b5308", "mov rdx,[rbx+0x08] size"),
        (0xF6C5, "c6041100", "mov byte [rcx+rdx],0"),
    ):
        raw = bytes.fromhex(pattern)
        require(img.read(addr, len(raw)) == raw, what)

    # Allocations: path 1 data/control, path 2 data/control, path 4 data.
    for addr, target, what in (
        (0xF43B, 0x12820, "path 1 data allocation"),
        (0xF44C, 0x17070, "path 1 control allocation"),
        (0xF49A, 0x12820, "path 2 data allocation"),
        (0xF4F7, 0x17070, "path 2 control allocation"),
        (0xF54A, 0x12820, "path 4 data allocation"),
        (0xF4D0, 0x12AF0, "path 2 old-data tag-0 free"),
        (0xF4DE, 0x170B0, "path 2 old-control tag-1 free"),
        (0xF566, 0x12AF0, "path 4 old-data tag-0 free"),
        (0xF6B9, 0xF3C0, "0xf6b0 -> 0xf3c0"),
    ):
        require(call_target(img, addr) == target, what)

    # Path 1 jump and path 2/3 jump both land on the common epilogue; path 4 falls through to it.
    require(rel32_target(img, 0xF46B, 0xE9) == 0xF579, "path 1 jump target")
    require(short_target(img, 0xF511) == 0xF579, "path 2/3 jump target")
    # The only `mov r15b,1` in the body is path 4.
    require(resize.count(bytes.fromhex("41b701")) == 1, "exactly one mov r15b,1")
    # Two explicit zeroings of the return register, at the pinned offsets.
    require(resize.count(bytes.fromhex("4531ff")) == 2, "exactly two xor r15d,r15d")

    header = HEADER.read_text()
    for token in (
        "kSharedStrResizeF3c0EndRva = 0xF58B",
        "kSharedStrResizeF3c0Sha256",
        "980c6911da8fad573162fd1d97f77b1dc20677bb7eedbee3e7e5fc8fe25227fb",
        "kSharedStrTerminateF6b0Rva = 0xF6B0",
        "kSharedStrTerminateF6b0Size = 32",
        "kSharedStrTerminateF6b0EndRva = 0xF6D0",
        "bfaf9b09a6f5f0345f8341cbc75da04f7ebe4dcf9ae0d697d86bbac82da25be1",
        "kSharedStrResizeDataTag = 0",
        "kSharedStrResizeControlTag = 1",
        "kSharedStrResizeSmallLimit = 0x80",
        "kSharedStrResizeMediumLimit = 0x400",
        "kSharedStrResizeSmallAlign = 0x10",
        "kSharedStrResizeMediumAlign = 0x100",
        "kSharedStrResizeLargeAlign = 0x400",
        "mov r15b,1",
        "NOT modelled",
    ):
        require(token in header, f"header token {token!r}")

    source = SOURCE.read_text()
    for token in (
        "resize_shared_str_f3c0",
        "terminate_shared_str_f6b0",
        "kSharedStrResizeDataTag",
        "kSharedStrResizeControlTag",
        "kSharedStrResizeSmallSlack",
        "kSharedStrResizeMediumSlack",
        "kSharedStrResizeLargeSlack",
        "align_down",
        "std::atomic_ref",
        "fetch_sub",
        "std::memcpy",
        "reallocated_capacity_short",
    ):
        require(token in source, f"source token {token!r}")

    print(
        "OK check_fshared_str_resize: 0xf3c0 459B sha=980c6911…, 0xf6b0 32B sha=bfaf9b09…; "
        "capacity cmp [rbx+0x10]/jae, align4/16/256/1024, data tag 0 + control tag 1, "
        "path-4 al=1 only (paths 1/2 return 0); guest allocator NOT executed"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
