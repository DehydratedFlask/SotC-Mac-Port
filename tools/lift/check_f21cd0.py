#!/usr/bin/env python3
"""ELF-tie the native lift of original function RVA 0x21cd0 (B-insert body, bump-allocator).

Pins the 204 B body and its sha, the canary guard cell, the begin/end/grow call targets,
the {code,code,int 0xea} stack aggregate, the bump/end offset values, the copy instruction
encodings, the fn!=0 self-link, and the native header constants. Private: needs
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
RVA = 0x21CD0
SIZE = 204  # 0x21cd0..0x21d9c = 0xCC
FN_SHA256 = "2fb96a5555881520b7acfc957968929533dc2547baf79fba00fa8026f12953ef"
END_RVA = 0x21D9C  # end of __stack_chk_fail call (exclusive); nop pad to 0x21da0

# --- Call targets and their exact sites ---
CALL_BEGIN = 0x21D27       # e8 → 0x3b390
BEGIN_TARGET = 0x3B390
CALL_GROW = 0x21D73        # e8 → 0x23210
GROW_TARGET = 0x23210
CALL_END = 0x21D7B         # e8 → 0x3b3c0
END_TARGET = 0x3B3C0
CALL_STACKCHK_FAIL = 0x21D97  # e8 → 0xf8f3a0
STACKCHK_FAIL_TARGET = 0xF8F3A0

# --- Guard cell ---
GUARD_LOAD = 0x21CDF   # mov r12,[rip+disp32] → 0x15fe518
GUARD_LOAD_ENC = "4c8b25"
GUARD_CELL = 0x15FE518

# --- Stack aggregate fields ---
CODE1 = 0xF95DF0   # function signature string
CODE2 = 0xF95C06   # source file path string
EXCEPTION_CODE = 0xEA

# --- Prologue offset sites ---
LEA_R14_LOCK = 0x21CFE    # lea r14,[rbx+0x58]
MOV_RDI_BEGIN = 0x21D0C   # mov rdi,r14 (begin arg)
VMOVQ_RDX = 0x21D02       # vmovq xmm0,rdx
VMOVQ_RCX = 0x21D07       # vmovq xmm1,rcx
VPUNPCKL = 0x21D0F        # vpunpcklqdq xmm0,xmm0,xmm1
VMOVDQU_STACK = 0x21D1B   # vmovdqu [rbp-0x40],xmm0
MOV_EA_STACK = 0x21D20    # mov dword [rbp-0x30],0xea
LEA_CODE1 = 0x21CE9       # lea rdx,[rip+disp] → CODE1
LEA_CODE2 = 0x21CF0       # lea rcx,[rip+disp] → CODE2

# --- Capacity check and bump ---
MOV_BUMP_LOAD = 0x21D2C   # mov rax,[rbx+0x40]
CMP_END = 0x21D30         # cmp rax,[rbx+0x48]
JAE_GROW = 0x21D34        # jae → grow path (+0x33)
LEA_STRIDE = 0x21D36       # lea rcx,[rax+0x20]
XOR_ZERO = 0x21D3A         # vxorps xmm0,xmm0,xmm0
MOV_BUMP_STORE = 0x21D3E   # mov [rbx+0x40],rcx
VMOVUPS_ZERO = 0x21D42     # vmovups [rax],xmm0  (zeros first 16 B)

# --- Descriptor copy ---
VMOVDQU_LOAD = 0x21D46    # vmovdqu xmm0,[r15+0x08]  (invoke+offset)
VMOVDQU_STORE = 0x21D4C   # vmovdqu [rax+0x08],xmm0
MOV_CTX_LOAD = 0x21D51    # mov rcx,[r15]  (ctx)
MOV_CTX_STORE = 0x21D54   # mov [rax],rcx
MOV_FN_LOAD = 0x21D57     # mov rcx,[r15+0x18]  (fn)
MOV_FN_STORE = 0x21D5B    # mov [rax+0x18],rcx

# --- Self-link ---
TEST_FN = 0x21D5F         # test rcx,rcx
JE_SKIP = 0x21D62         # je +0x14 → skip self-link
SELF_LINK = 0x21D64       # mov [rax],rax  (slot[0x00] = &slot[0x00])

# --- Grow path ---
ADD_GROW_ARG = 0x21D69    # add rbx,0x38

# --- End-call setup ---
MOV_RDI_END = 0x21D78     # mov rdi,r14

# --- Stack-check epilogue ---
CANARY_RECHECK = 0x21D80  # mov rax,[r12]
CANARY_CMP = 0x21D84      # cmp rax,[rbp-0x28]
CANARY_JNE = 0x21D88      # jne → __stack_chk_fail
RET = 0x21D96             # c3

# --- Container offsets ---
BUMP_OFFSET = 0x40        # [rbx+0x40]
END_OFFSET = 0x48         # [rbx+0x48]
LOCK_OFFSET = 0x58        # r14 = rbx+0x58
GROW_ARG_OFFSET = 0x38    # add rbx,0x38
ENTRY_STRIDE = 0x20       # lea rcx,[rax+0x20]
COPY_SIZE = 0x20          # total copy: ctx(8) + invoke+offset(16) + fn(8)

NATIVE_HPP = ROOT / "native/core/insert_21cd0.hpp"
NATIVE_CPP = ROOT / "native/core/insert_21cd0.cpp"
NATIVE_TEST = ROOT / "native/tests/test_insert_21cd0.cpp"


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def rip_target(img: d.Image, site: int, size: int, disp: int) -> int:
    return site + size + disp


def main() -> int:
    elf_bytes = (ROOT / "local/re/eboot.elf").read_bytes()
    require(hashlib.sha256(elf_bytes).hexdigest() == ELF_SHA256, "unexpected ELF SHA256")
    _, ph = d.program_headers(elf_bytes)
    img = d.Image(elf_bytes, ph)

    # --- Extent and sha ---
    fn = img.read(RVA, SIZE)
    fn_digest = hashlib.sha256(fn).hexdigest()
    require(fn_digest == FN_SHA256,
            f"fn 0x{RVA:x} sha256 mismatch: got {fn_digest}")
    require(img.read(RET, 1) == b"\xc3", f"ret at 0x{RET:x}")
    require(img.read(0x21DA0, 1) == b"\x55",
            "next function at 0x21da0 must start with push rbp")

    # --- Stack canary guard cell ---
    guard = img.read(GUARD_LOAD, 7)
    require(guard[:3] == bytes.fromhex(GUARD_LOAD_ENC),
            f"mov r12,[rip+..] at 0x{GUARD_LOAD:x}")
    guard_disp = struct.unpack("<i", guard[3:])[0]
    require(rip_target(img, GUARD_LOAD, 7, guard_disp) == GUARD_CELL,
            f"guard cell 0x{GUARD_CELL:x}")
    # Two-level deref.
    require(img.read(0x21D13, 4) == bytes.fromhex("498b0424"),
            "two-level canary deref: mov rax,[r12]")
    require(img.read(0x21D17, 4) == bytes.fromhex("488945d8"),
            "canary store: mov [rbp-0x28],rax")

    # --- Begin call → 0x3b390 ---
    call = img.read(CALL_BEGIN, 5)
    require(call[0] == 0xE8, f"begin call opcode at 0x{CALL_BEGIN:x}")
    bt = rip_target(img, CALL_BEGIN, 5, struct.unpack("<i", call[1:])[0])
    require(bt == BEGIN_TARGET,
            f"begin call target 0x{BEGIN_TARGET:x}, got 0x{bt:x}")
    require(img.read(BEGIN_TARGET, 1) == b"\xe9",
            f"begin thunk at 0x{BEGIN_TARGET:x} must be jmp stub")

    # --- End call → 0x3b3c0 ---
    ce = img.read(CALL_END, 5)
    require(ce[0] == 0xE8, f"end call opcode at 0x{CALL_END:x}")
    et = rip_target(img, CALL_END, 5, struct.unpack("<i", ce[1:])[0])
    require(et == END_TARGET,
            f"end call target 0x{END_TARGET:x}, got 0x{et:x}")

    # --- Grow call → 0x23210 ---
    cg = img.read(CALL_GROW, 5)
    require(cg[0] == 0xE8, f"grow call opcode at 0x{CALL_GROW:x}")
    gt = rip_target(img, CALL_GROW, 5, struct.unpack("<i", cg[1:])[0])
    require(gt == GROW_TARGET,
            f"grow call target 0x{GROW_TARGET:x}, got 0x{gt:x}")

    # --- {code,code,int 0xea} stack aggregate ---
    # LEA code1 (rdx) and code2 (rcx).
    l1 = img.read(LEA_CODE1, 7)
    require(l1[:3] == bytes.fromhex("488d15"),
            f"lea rdx,[rip+..] at 0x{LEA_CODE1:x}")
    l1_disp = struct.unpack("<i", l1[3:])[0]
    require(rip_target(img, LEA_CODE1, 7, l1_disp) == CODE1,
            f"code1 = 0x{CODE1:x}, got 0x{rip_target(img, LEA_CODE1, 7, l1_disp):x}")

    l2 = img.read(LEA_CODE2, 7)
    require(l2[:3] == bytes.fromhex("488d0d"),
            f"lea rcx,[rip+..] at 0x{LEA_CODE2:x}")
    l2_disp = struct.unpack("<i", l2[3:])[0]
    require(rip_target(img, LEA_CODE2, 7, l2_disp) == CODE2,
            f"code2 = 0x{CODE2:x}, got 0x{rip_target(img, LEA_CODE2, 7, l2_disp):x}")

    # vmovq packing: vmovq xmm0,rdx ; vmovq xmm1,rcx ; vpunpcklqdq.
    require(img.read(VMOVQ_RDX, 5) == bytes.fromhex("c4e1f96ec2"),
            "vmovq xmm0,rdx at 0x21d02")
    require(img.read(VMOVQ_RCX, 5) == bytes.fromhex("c4e1f96ec9"),
            "vmovq xmm1,rcx at 0x21d07")
    require(img.read(VPUNPCKL, 4) == bytes.fromhex("c5f16cc0"),
            "vpunpcklqdq xmm0,xmm0,xmm1 at 0x21d0f")

    # Store packed xmm0 to [rbp-0x40].
    require(img.read(VMOVDQU_STACK, 5) == bytes.fromhex("c5fa7f45c0"),
            "vmovdqu [rbp-0x40],xmm0 at 0x21d1b")
    # Store int 0xea at [rbp-0x30].
    require(img.read(MOV_EA_STACK, 7) == bytes.fromhex("c745d0ea000000"),
            "mov dword [rbp-0x30],0xea at 0x21d20")

    # --- Lock offset 0x58: lea r14,[rbx+0x58] → rdi for begin ---
    require(img.read(LEA_R14_LOCK, 4) == bytes.fromhex("4c8d7358"),
            "lea r14,[rbx+0x58]")
    require(img.read(MOV_RDI_BEGIN, 3) == bytes.fromhex("4c89f7"),
            "mov rdi,r14 (begin arg)")

    # --- Capacity check ---
    require(img.read(MOV_BUMP_LOAD, 4) == bytes.fromhex("488b4340"),
            f"mov rax,[rbx+0x40] at 0x{MOV_BUMP_LOAD:x}")
    require(img.read(CMP_END, 4) == bytes.fromhex("483b4348"),
            f"cmp rax,[rbx+0x48] at 0x{CMP_END:x}")
    require(img.read(JAE_GROW, 2) == bytes.fromhex("7333"),
            f"jae → grow at 0x{JAE_GROW:x}")

    # --- Fast-path bump and zero ---
    require(img.read(LEA_STRIDE, 4) == bytes.fromhex("488d4820"),
            f"lea rcx,[rax+0x20] at 0x{LEA_STRIDE:x}")
    require(img.read(XOR_ZERO, 4) == bytes.fromhex("c5fc57c0"),
            "vxorps xmm0,xmm0,xmm0")
    require(img.read(MOV_BUMP_STORE, 4) == bytes.fromhex("48894b40"),
            f"mov [rbx+0x40],rcx at 0x{MOV_BUMP_STORE:x}")
    require(img.read(VMOVUPS_ZERO, 4) == bytes.fromhex("c5fc1100"),
            f"vmovups [rax],xmm0 at 0x{VMOVUPS_ZERO:x}")

    # --- Descriptor copy ---
    require(img.read(VMOVDQU_LOAD, 6) == bytes.fromhex("c4c17a6f4708"),
            f"vmovdqu xmm0,[r15+0x08] at 0x{VMOVDQU_LOAD:x}")
    require(img.read(VMOVDQU_STORE, 5) == bytes.fromhex("c5fa7f4008"),
            f"vmovdqu [rax+0x08],xmm0 at 0x{VMOVDQU_STORE:x}")
    require(img.read(MOV_CTX_LOAD, 3) == bytes.fromhex("498b0f"),
            f"mov rcx,[r15] at 0x{MOV_CTX_LOAD:x}")
    require(img.read(MOV_CTX_STORE, 3) == bytes.fromhex("488908"),
            f"mov [rax],rcx at 0x{MOV_CTX_STORE:x}")
    require(img.read(MOV_FN_LOAD, 4) == bytes.fromhex("498b4f18"),
            f"mov rcx,[r15+0x18] at 0x{MOV_FN_LOAD:x}")
    require(img.read(MOV_FN_STORE, 4) == bytes.fromhex("48894818"),
            f"mov [rax+0x18],rcx at 0x{MOV_FN_STORE:x}")

    # --- Self-link ---
    require(img.read(TEST_FN, 3) == bytes.fromhex("4885c9"),
            f"test rcx,rcx at 0x{TEST_FN:x}")
    require(img.read(JE_SKIP, 2) == bytes.fromhex("7414"),
            f"je skip self-link at 0x{JE_SKIP:x}")
    require(img.read(SELF_LINK, 3) == bytes.fromhex("488900"),
            f"mov [rax],rax at 0x{SELF_LINK:x}")

    # --- Grow arg: add rbx,0x38 ---
    require(img.read(ADD_GROW_ARG, 4) == bytes.fromhex("4883c338"),
            f"add rbx,0x38 at 0x{ADD_GROW_ARG:x}")

    # --- End-call: mov rdi,r14 ---
    require(img.read(MOV_RDI_END, 3) == bytes.fromhex("4c89f7"),
            f"mov rdi,r14 (end arg) at 0x{MOV_RDI_END:x}")

    # --- Stack-check epilogue ---
    require(img.read(CANARY_RECHECK, 4) == bytes.fromhex("498b0424"),
            f"mov rax,[r12] at 0x{CANARY_RECHECK:x}")
    require(img.read(CANARY_CMP, 4) == bytes.fromhex("483b45d8"),
            f"cmp rax,[rbp-0x28] at 0x{CANARY_CMP:x}")
    require(img.read(CANARY_JNE, 2) == bytes.fromhex("750d"),
            f"jne → stack_chk_fail at 0x{CANARY_JNE:x}")
    chk = img.read(CALL_STACKCHK_FAIL, 5)
    require(chk[0] == 0xE8,
            f"stack_chk_fail call opcode at 0x{CALL_STACKCHK_FAIL:x}")
    chk_tgt = rip_target(img, CALL_STACKCHK_FAIL, 5,
                         struct.unpack("<i", chk[1:])[0])
    require(chk_tgt == STACKCHK_FAIL_TARGET,
            f"stack_chk_fail → 0x{STACKCHK_FAIL_TARGET:x}")

    # --- Verify string content at CODE1/CODE2 ---
    sig = img.read(CODE1, 96).decode("ascii", errors="replace").rstrip("\x00")
    require("ConCmdManager::RegisterGetCmdsForEditorCallback" in sig,
            f"code1 string mismatch: {sig[:80]}...")
    src = img.read(CODE2, 64).decode("ascii", errors="replace").rstrip("\x00")
    require("ConVar.cpp" in src,
            f"code2 string mismatch: {src}")

    # --- Native header constants ---
    hpp = NATIVE_HPP.read_text()
    for token in (
        "kInsert21cd0Rva = 0x21cd0",
        "kInsert21cd0Size = 204",
        "kInsert21cd0EndRva = 0x21d9c",
        "kInsert21cd0BeginThunk = 0x3b390",
        "kInsert21cd0EndThunk = 0x3b3c0",
        "kInsert21cd0GrowFn = 0x23210",
        "kInsert21cd0GuardCell = 0x15fe518",
        "kInsert21cd0StackChkFail = 0xf8f3a0",
        "kInsert21cd0Code1 = 0xf95df0",
        "kInsert21cd0Code2 = 0xf95c06",
        "kInsert21cd0ExceptionCode = 0xea",
        "kInsert21cd0EntrySize = 0x20",
        "kInsert21cd0LockOffset = 0x58",
        "kInsert21cd0GrowArgOffset = 0x38",
        "kInsert21cd0BumpOffset = 0x40",
        "kInsert21cd0EndOffset = 0x48",
        "kInsert21cd0CopySize = 0x20",
        "NOT modelled",
        "bump-allocator",
        "DIFFERENT container",
    ):
        require(token in hpp, f"header token {token!r}")
    require(len(NATIVE_CPP.read_text()) > 0, "impl present")
    test_src = NATIVE_TEST.read_text()
    require("hook_grow" in test_src, "test defines grow hook")
    require("self-linked" in test_src, "test asserts self-link")
    require("bump advance" in test_src, "test asserts bump advance")
    require("begin_called" in test_src, "test observes begin call")

    print(
        "OK check_f21cd0: extent=0x21cd0+204B sha=" + fn_digest[:12] + "…; "
        "guard 0x15fe518; begin→0x3b390 end→0x3b3c0 grow→0x23210; "
        "{code=" + hex(CODE1)[:10] + "…,code=" + hex(CODE2)[:10] + "…,int=0xea}; "
        "bump [rbx+0x40]+=0x20 vs [rbx+0x48]; "
        "vmovdqu copy descriptor[0x08..0x18)→slot+0x08; mov ctx+fn; "
        "fn!=0 self-link; chk_fail→0xf8f3a0; "
        "begin/end thunks + grow fn NOT modelled"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
