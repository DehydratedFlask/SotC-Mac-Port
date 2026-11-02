#!/usr/bin/env python3
"""Re-derive the four slot-309 follow-up findings from the pinned image alone.

Tasks closed by this checker (all read-only, no provider query):

  CPU-01-STARTUP-SLOT-309-CALLER-ORDER-01        - who reaches `0x28af0` / `0x531d60`, and can
                                                   either run before slot `0xa505e0`?
  CPU-01-STARTUP-SLOT-309-HELPER-CALLER-CENSUS-01 - callers of `0xa50280` / `0xa50440`
  CPU-01-STARTUP-SLOT-309-NONZERO-CONSUMER-01     - is there a nonzero producer/reader/release of
                                                   the qword at envelope `+0x10`?
  CPU-01-STARTUP-SLOT-309-PAYLOAD-OWNERSHIP-01    - borrowed / copied / owned on the observed path?

The two "order" answers are not call-graph questions. `0xa505e0` is reached only as row 309 of the
backwards-indexed 573-entry constructor table (E1056), and the only startup ordering model in this
repository runs that table to completion before `main` (`0xf120`). Both of the traced consumers live
strictly downstream of `main`:

    main 0xf120 -> 0x536730 -> 0x536aa0 (workspace ctor, installs vptr 0x15c8cd8)
                            -> ... -> 0x25a8d0 (slot 3 of that vtable) -> call *0x20 -> 0x531d60
                            -> 0x255130 -> 0x255180 -> 0x264e50 -> 0x27990 -> [+0xd0] = 0x28af0

so neither can execute before slot 309. The `+0x10` payload is a literal zero seeded by slot 309
itself (`movq $0x0, 0x90(%rsp)`), copied three times by trivial qword moves, and never read,
compared or released: the only cleanup that runs on the envelope is a dispatch to a bare `ret`.

Every step below byte-matches the pinned ELF or the check fails. Nothing is executed. Skips cleanly
(exit 0) when the private image is absent.

Usage:
  python3 tools/lift/check_slot309_followups.py
  python3 tools/lift/check_slot309_followups.py --json local/private/slot309_followups.json
"""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))

ELF = ROOT / "local/re/eboot.elf"
FDE = ROOT / "local/validation/registry-analysis-agent/fde_ranges.json"
DIS = ROOT / "local/validation/registry-analysis-agent/all.dis"

ELF_SHA256 = "d0723afbdbd4cb3775c683a2b7f09534d371ce6d0b18146fc35b4d50c25861d3"

# --- slot 309 and the constructor table (E1056) -----------------------------------------------------
SLOT309 = 0xA505E0
SLOT309_SIZE = 1892                      # FDE [0xa505e0, 0xa50d44)
CTOR_TABLE_TOP = 0x2DD70E0
CTOR_TABLE_COUNT = 573
SLOT309_INDEX = 309
SLOT309_TABLE_ROW = CTOR_TABLE_TOP - SLOT309_INDEX * 8      # 0x2dd6738

# --- the three console-variable constructors and the one shared body --------------------------------
VAR_CTORS = {0xA4F4D0: 436, 0xA4FEC0: 436, 0xA50280: 436}
VAR_CTOR_SLOT309_SITES = {0xA4F4D0: 0xA50720, 0xA4FEC0: 0xA50AD8, 0xA50280: 0xA50C57}
SHARED_BODY = 0xA50440                    # called once, by 0xa50280 only
SHARED_BODY_SITE = 0xA503D7
VARSIG_SLOT309_SITES = (0xA5068B, 0xA50A3C, 0xA50BC5)   # `movq $0x0, 0x90(%rsp)`
ZERO_PAYLOAD_STORE = bytes.fromhex("48c7842490000000000000")   # 11 B, exactly, at all three sites

# --- the envelope (`+0x10` payload, `+0x20` tag) ----------------------------------------------------
# Three tag tables, four slots each: copy-ctor / tag+copy / copy-assign / bare-ret destroy.
TAG_TABLES = {
    0x15D1B78: (0xA4F470, 0xA4F490, 0xA4F4B0, 0xA4F4C0),
    0x15D1C30: (0xA4FE60, 0xA4FE80, 0xA4FEA0, 0xA4FEB0),
    0x15D1CC8: (0xA50220, 0xA50240, 0xA50260, 0xA50270),
}
TAG_BYTES = {
    # helper: (exact body bytes, FDE extent - the 18/17-byte bodies sit in a 20-byte FDE slot)
    0xA4F470: ("488b4610488d0dfd26b8004889471048894f20c3", 20),
    0xA4F490: ("488d05e126b80048894720488b461048894710c3", 20),
    0xA4F4B0: ("488b461048894710c3", 9),
    0xA4F4C0: ("c3", 1),
    0xA4FE60: ("488b4610488d0dc51db8004889471048894f20c3", 20),
    0xA4FE80: ("488d05a91db80048894720488b461048894710c3", 20),
    0xA4FEA0: ("488b461048894710c3", 9),
    0xA4FEB0: ("c3", 1),
    0xA50220: ("488b4610488d0d9d1ab8004889471048894f20c3", 20),
    0xA50240: ("488d05811ab80048894720488b461048894710c3", 20),
    0xA50260: ("488b461048894710c3", 9),
    0xA50270: ("c3", 1),
}
# `mov %rax, 0x10(%rdi)` - the only instruction shape that writes an envelope payload.
STORE_10_RDI = bytes.fromhex("48894710")
VAR_CLASS_VPTS = {0x15D1B98: 0xA47D20, 0x15D1C50: 0xA48240, 0x15D1CE8: 0xA4AB50}

# --- the shared body's own slot-0 dispatch and the var-3 destructor --------------------------------
SHARED_BODY_TAG_LOAD = bytes.fromhex("488b83c0000000")     # mov 0xc0(%rbx),%rax
SHARED_BODY_DEST = bytes.fromhex("4c8da3a0000000")         # lea 0xa0(%rbx),%r12
SHARED_BODY_CALL = bytes.fromhex("ff10")                   # call *(%rax)
DTOR = 0xA4AB50
DTOR_SIZE = 54
# `push rbp; mov rsp,rbp; push rbx; push rax; lea 0x15d1b00; mov rbx,rdi; lea 0xa0(%rbx),%rdi;
#  add $0x10,%rax; mov %rax,(%rbx); mov 0xc0(%rbx),%rax; call *0x18(%rax); mov %rbx,%rdi;
#  add $0x8,%rsp; pop rbx; pop rbp; jmp 0x20b50`
DTOR_BODY = ("554889e55350488d05a36fb8004889fb488dbba00000004883c010488903488b83c0000000"
             "ff50184889df4883c4085b5de9ca5f5dff")

# --- callable 0x28af0: one data cell, one reader, one bound-member write ---------------------------
CALLBACK = 0x28AF0
CALLBACK_SIZE = 4077
CALLBACK_CELL = 0x15FE540
CALLBACK_READ = 0x27AB1                                   # vmovq cell,%xmm0
CALLBACK_READ_BYTES = ("c5fa7e05876a5d01", 8)             # VEX2 vmovq, disp32 -> 0x15fe540
CALLBACK_WRITE = 0x27AC4                                  # vmovdqu %xmm0,0xd0(%r15)
CALLBACK_WRITE_BYTES = ("c4c17a7f87d0000000", 9)
CALLBACK_BIND_SELF = 0x27AD1                              # mov %r15,0xc8(%r15)
CALLBACK_BIND_BYTES = ("4d89bfc8000000", 7)
CALLBACK_INSTALLER = 0x27990

# --- vtable 0x531d60: one row, one address point, two vptr installs, one dispatch ------------------
VIRTUAL = 0x531D60
VIRTUAL_SIZE = 3890
VTABLE_BASE = 0x15C8CC8                                   # two zero qwords, then the address point
VTABLE_ADDR_POINT = 0x15C8CD8
VTABLE_ROW = 0x15C8CF8                                    # address_point + 0x20
VTABLE_INSTALLS = {0x536AC7: "ctor", 0x537F9A: "dtor"}          # the `lea 0x15c8cc8` sites
VTABLE_STORES = {0x536AFA: "488903", 0x537FA8: "498906"}          # mov %rax,(%rbx) / (%r14)
LEA_VTABLE = bytes.fromhex("488d05")                      # lea disp32(%rip),%rax
ADD_0x10 = bytes.fromhex("4883c010")                      # add $0x10,%rax
WORKSPACE_CTOR = 0x536AA0
WORKSPACE_CTOR_SIZE = 5350
WORKSPACE_CTOR_SITE = 0x536775
BOOTSTRAP = 0x536730
BOOTSTRAP_SIZE = 298
BOOTSTRAP_SITE = 0xF15A
MAIN = 0xF120
MAIN_SIZE = 81
DISPATCH = 0x25AA06                                       # callq *0x20(%rax)
DISPATCH_WINDOW = 0x25A9FD                                # the receiver reload + arg setup + call
DISPATCH_BYTES = ("498b074c89ff4c89f6ff5020", 12)         # mov (%r15),%rax; mov %r15,%rdi; mov %r14,%rsi; call *0x20(%rax)
DISPATCH_OWNER = 0x25A8D0                                 # slot 3 of the same table (0x15c8cf0)

# --- the startup walker: `main` runs the whole constructor list before the workspace bootstrap -------
WALKER = 0x20                                             # called from main, walks the table backwards
CTOR_SENTINEL = CTOR_TABLE_TOP - 8 * CTOR_TABLE_COUNT      # 0x2dd5ef8 = 0xffffffffffffffff
CTOR_LAST_ENTRY = CTOR_TABLE_TOP - 8 * (CTOR_TABLE_COUNT - 1)   # 0x2dd5f00 = 0x1a20
MAIN_CALL_WALKER = 0xF14D                                 # call 0x20
MAIN_CALL_BOOTSTRAP = 0xF15A                              # call 0x536730


def require(cond: bool, msg: str) -> None:
    if not cond:
        raise SystemExit(f"FAIL {msg}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--json", type=Path, default=None)
    args = parser.parse_args()

    if not ELF.is_file():
        print("SKIP check_slot309_followups: private pinned image not present")
        return 0

    import ps4_dynlink as d  # noqa: E402  (needs scripts/ on sys.path)

    elf = ELF.read_bytes()
    require(hashlib.sha256(elf).hexdigest() == ELF_SHA256, "pinned ELF digest changed")
    _, program_headers = d.program_headers(elf)
    image = d.Image(elf, program_headers)
    relocs = d.parse(elf)["relocs"]

    def as_int(value) -> int:
        return value if isinstance(value, int) else int(str(value), 16)

    rows = [(as_int(r["target"]), as_int(r["type"]), as_int(r["addend"]))
            for r in relocs if as_int(r["type"]) == 8]                      # R_X86_64_RELATIVE
    addend_index: dict[int, list[int]] = {}
    for target, _type, addend in rows:
        addend_index.setdefault(addend, []).append(target)
    addend_at: dict[int, int] = {}
    table: dict[int, int] = {}
    ctor_index_of: dict[int, int] = {}
    for target, _type, addend in rows:
        addend_at[target] = addend
        if target > CTOR_TABLE_TOP or (CTOR_TABLE_TOP - target) % 8:
            continue
        index = (CTOR_TABLE_TOP - target) // 8
        if index < CTOR_TABLE_COUNT:
            table[target] = addend
            ctor_index_of[addend] = index

    fde: dict[int, int] = {}
    if FDE.is_file():
        fde = {int(k, 16): v for k, v in json.loads(FDE.read_text()).items()}

    def reads(vaddr: int, size: int) -> bytes:
        return image.read(vaddr, size)

    def near_refs(targets) -> dict[int, list[tuple[int, str]]]:
        """Complete `e8`/`e9`+disp32 scan over every file-backed byte of every loadable segment.

        Decoder-independent: it finds every near call/jmp encoding that lands on a target whether or
        not the byte sits on an instruction boundary."""
        wanted = set(targets)
        found: dict[int, list[tuple[int, str]]] = {t: [] for t in wanted}
        for seg in image.loads:
            chunk = elf[seg["offset"]:seg["offset"] + seg["filesz"]]
            base = seg["vaddr"]
            for off in range(len(chunk) - 4):
                opcode = chunk[off]
                if opcode not in (0xE8, 0xE9):
                    continue
                dest = base + off + 5 + struct.unpack_from("<i", chunk, off + 1)[0]
                if dest in found:
                    found[dest].append((base + off, "call" if opcode == 0xE8 else "jmp"))
        return found

    def rip_target(addr: int, disp_at: int, end: int) -> int:
        return addr + struct.unpack_from("<i", reads(addr, end), disp_at)[0]

    def check_call(site: int, target: int) -> bool:
        window = reads(site, 5)
        return window[0] == 0xE8 and site + 5 + struct.unpack_from("<i", window, 1)[0] == target

    # ---------------------------------------------------------------- A. slot 309 is a data-only edge
    slot_hits = near_refs([SLOT309])[SLOT309]
    require(not slot_hits, f"a near call/jmp to 0x{SLOT309:x} appeared at {slot_hits}")
    require(len(addend_index.get(SLOT309, [])) == 1,
            f"slot 309 now has {len(addend_index.get(SLOT309, []))} relocations")
    require(addend_index[SLOT309][0] == SLOT309_TABLE_ROW,
            "the slot-309 table row moved")
    require(table.get(SLOT309_TABLE_ROW) == SLOT309, "row 0x2dd6738 no longer holds slot 309")
    require(len([1 for (_t, _ty, a) in rows if a == SLOT309]) == 1,
            "a second RELATIVE relocation references slot 309")

    # ---------------------------------------------- B. constructor callers of the three var ctors
    census_targets = [*VAR_CTORS, SHARED_BODY]
    calls = near_refs(census_targets)
    for ctor, site in VAR_CTOR_SLOT309_SITES.items():
        got = calls[ctor]
        require([s for s, _k in got] == [site],
                f"0x{ctor:x} callers changed: {[(hex(s), k) for s, k in got]}")
    require([s for s, _k in calls[SHARED_BODY]] == [SHARED_BODY_SITE],
            f"0x{SHARED_BODY:x} callers changed: {[(hex(s), k) for s, k in calls[SHARED_BODY]]}")
    for ctor, size in VAR_CTORS.items():
        if fde:
            require(fde.get(ctor) == size, f"0x{ctor:x} FDE size changed")
        require(not addend_index.get(ctor), f"0x{ctor:x} is now address-taken (relocation present)")
    require(not addend_index.get(SHARED_BODY), "0xa50440 is now address-taken")
    for ctor, site in VAR_CTOR_SLOT309_SITES.items():
        frozen = reads(site, 5)
        require(frozen[0] == 0xE8 and site + 5 + struct.unpack_from("<i", frozen, 1)[0] == ctor,
                f"call at 0x{site:x} no longer targets 0x{ctor:x}")

    # ------------------------------------------- C. the envelope: four slots per tag, bare-ret destroy
    for tag, body in TAG_TABLES.items():
        for i, helper in enumerate(body):
            require(addend_at.get(tag + 8 * i) == helper,
                    f"tag 0x{tag:x} slot {i} is 0x{addend_at.get(tag + 8 * i, 0):x}, expected 0x{helper:x}")
            require(struct.unpack_from("<Q", reads(tag + 8 * i, 8))[0] == helper,
                    f"tag 0x{tag:x} slot {i} image word disagrees with its RELA row")
            if fde:
                require(fde.get(helper) == TAG_BYTES[helper][1],
                        f"0x{helper:x} FDE size changed")
            require(not near_refs([helper])[helper],
                    f"0x{helper:x} is now directly called")
    for helper, (body, _extent) in TAG_BYTES.items():
        require(reads(helper, len(body) // 2).hex() == body, f"0x{helper:x} body bytes changed")
    require(reads(0xA50270, 1) == b"\xc3", "the envelope destroy is no longer a bare ret")

    # ------------------------------------- D. the +0x10 payload has exactly one producer: literal zero
    for site in VARSIG_SLOT309_SITES:
        require(reads(site, 11) == ZERO_PAYLOAD_STORE,
                f"0x{site:x} is not `movq $0x0, 0x90(%rsp)`")
    require(reads(0xA50B26, 5).hex() == "4c8d742460",
            "0xa50b26 is not `lea 0x60(%rsp),%r14` (the arg7 block of the 0xa50280 call)")
    require(reads(0xA50C53, 4).hex() == "41544156",
            "the push pair before 0xa50c57 changed (arg7/arg8 order)")
    # Every `mov %rax,0x10(%rdi)` in the tag families is one of the nine copy helpers (slots 0/1/2);
    # the three slot-3 destroys are bare `ret`s and touch nothing.
    payload_writers = []
    for helper, (body, _extent) in TAG_BYTES.items():
        blob = bytes.fromhex(body)
        if STORE_10_RDI in blob:
            payload_writers.append(helper)
    copies = sorted(h for h, (b, _e) in TAG_BYTES.items() if bytes.fromhex(b) != b"\xc3")
    require(sorted(payload_writers) == copies,
            f"envelope payload writers changed: {[hex(h) for h in sorted(payload_writers)]}")

    # ------------------------------------------- E. the shared body writes the object envelope once
    require(reads(0xA50539, 7) == SHARED_BODY_TAG_LOAD, "0xa50539 changed (tag load)")
    require(reads(0xA50540, 7) == SHARED_BODY_DEST, "0xa50540 changed (envelope destination)")
    require(reads(0xA5054D, 2) == SHARED_BODY_CALL, "0xa5054d changed (slot-0 dispatch)")
    require(addend_at.get(0x15D1CC8) == 0xA50220, "the slot-0 copy-ctor row moved")

    # ------------------------------------------------ F. the var-3 destructor never reads +0xb0
    dtor_body = reads(DTOR, DTOR_SIZE)
    if fde:
        require(fde.get(DTOR) == DTOR_SIZE, "0xa4ab50 FDE size changed")
    require(dtor_body.hex() == DTOR_BODY, "the 0xa4ab50 destructor body changed")
    require(b"\xb0" not in dtor_body, "the destructor now mentions an 0xb0 displacement")
    for vpt, dtor in VAR_CLASS_VPTS.items():
        require(addend_at.get(vpt + 0x10) == dtor, f"class vtable 0x{vpt:x}+0x10 moved")

    # ------------------------------------ G. the callback: one cell, one reader, one bound-member write
    require(not near_refs([CALLBACK])[CALLBACK], "0x28af0 is now directly called")
    require(addend_index.get(CALLBACK) == [CALLBACK_CELL],
            f"0x28af0 relocations changed: {[hex(t) for t in addend_index.get(CALLBACK, [])]}")
    require(struct.unpack_from("<Q", reads(CALLBACK_CELL, 8))[0] == CALLBACK,
            "the 0x15fe540 image word no longer holds 0x28af0")
    if fde:
        require(fde.get(CALLBACK) == CALLBACK_SIZE, "0x28af0 FDE size changed")
        require(fde.get(CALLBACK_INSTALLER) is not None, "0x27990 has no FDE")
    body, size = CALLBACK_READ_BYTES
    window = reads(CALLBACK_READ, size)
    require(window.hex() == body, "the only 0x15fe540 reader changed")
    require(CALLBACK_READ + size + struct.unpack_from("<i", window, 4)[0] == CALLBACK_CELL,
            "the 0x15fe540 reader no longer targets the callback cell")
    body, size = CALLBACK_WRITE_BYTES
    require(reads(CALLBACK_WRITE, size).hex() == body, "the [r15+0xd0] bound-member write changed")
    body, size = CALLBACK_BIND_BYTES
    require(reads(CALLBACK_BIND_SELF, size).hex() == body, "the +0xc8 self pointer write changed")

    # ------------------------- H. the virtual: one vtable row, two installs, one dispatch, after main
    require(not near_refs([VIRTUAL])[VIRTUAL], "0x531d60 is now directly called")
    require(addend_index.get(VIRTUAL) == [VTABLE_ROW],
            f"0x531d60 relocations changed: {[hex(t) for t in addend_index.get(VIRTUAL, [])]}")
    require(VTABLE_ROW - VTABLE_ADDR_POINT == 0x20, "the vtable row is no longer at +0x20")
    require(reads(VTABLE_BASE, 16) == b"\x00" * 16,
            "the two words before the address point are no longer zero (not a plain vtable)")
    require(addend_at.get(0x15C8CF0) == DISPATCH_OWNER, "slot 3 of the vtable is no longer 0x25a8d0")
    # R_X86_64_RELATIVE leaves the addend in place as well as in the RELA row; both must agree.
    require(struct.unpack_from("<Q", reads(VTABLE_ROW, 8))[0] == VIRTUAL,
            "the 0x15c8cf8 image word no longer holds 0x531d60")
    require(struct.unpack_from("<Q", reads(0x15C8CF0, 8))[0] == DISPATCH_OWNER,
            "the 0x15c8cf0 image word no longer holds 0x25a8d0")
    if fde:
        require(fde.get(VIRTUAL) == VIRTUAL_SIZE, "0x531d60 FDE size changed")
        require(fde.get(WORKSPACE_CTOR) == WORKSPACE_CTOR_SIZE, "0x536aa0 FDE size changed")
        require(fde.get(BOOTSTRAP) == BOOTSTRAP_SIZE, "0x536730 FDE size changed")
        require(fde.get(MAIN) == MAIN_SIZE, "0xf120 FDE size changed")
    for site, kind in VTABLE_INSTALLS.items():
        window = reads(site, 7)
        require(window[:3] == LEA_VTABLE, f"vptr install 0x{site:x} ({kind}) is not `lea`")
        require(site + 7 + struct.unpack_from("<i", window, 3)[0] == VTABLE_BASE,
                f"vptr install 0x{site:x} ({kind}) no longer loads 0x{VTABLE_BASE:x}")
    require(reads(0x536AF2, 4) == ADD_0x10 and reads(0x537FA4, 4) == ADD_0x10,
            "the `add $0x10,%rax` address-point step changed")
    for store, body in VTABLE_STORES.items():
        require(reads(store, 3).hex() == body, f"vptr store 0x{store:x} changed")
    require(addend_index.get(VTABLE_ADDR_POINT) is None,
            "the vptr is now also stored as a relocation (an install site went unnoticed)")
    require([s for s, _k in near_refs([WORKSPACE_CTOR])[WORKSPACE_CTOR]] == [WORKSPACE_CTOR_SITE],
            "0x536aa0 callers changed")
    require([s for s, _k in near_refs([BOOTSTRAP])[BOOTSTRAP]] == [BOOTSTRAP_SITE],
            "0x536730 no longer has `main` as its only caller")
    body, size = DISPATCH_BYTES
    require(reads(DISPATCH_WINDOW, size).hex() == body,
            "the `call *0x20(%rax)` dispatch shape changed")
    require(reads(DISPATCH, 3).hex() == "ff5020", "0x25aa06 is no longer `call *0x20(%rax)`")

    # No member of either chain is a startup constructor-table entry: there is no table order to lose.
    chain = [CALLBACK, CALLBACK_INSTALLER, 0x264E50, 0x255180, 0x255130, VIRTUAL,
             DISPATCH_OWNER, WORKSPACE_CTOR, 0x537F90]
    hits = [(hex(a), ctor_index_of[a]) for a in chain if a in ctor_index_of]
    require(not hits, f"a chain member became a startup-table entry: {hits}")
    require(len(table) == CTOR_TABLE_COUNT, "constructor table is no longer 573 complete rows")
    require(ctor_index_of.get(SLOT309) == SLOT309_INDEX, "slot 309 lost its table index")

    # Self-test: the near-call scan is live (a wrong target must find nothing).
    require(not near_refs([SLOT309 + 1])[SLOT309 + 1], "near-call scan is not discriminating")

    # ------------------------------------------------------------ I. the startup order, from code
    # `main` (the ELF entry point 0xf120) calls the constructor walker at 0x20 and then, straight-line,
    # the workspace bootstrap 0x536730. The walker starts at the table TOP and steps DOWN by 8, so it
    # runs index 0, 1, ... 572 and stops on the -1 sentinel one qword below entry 572.
    require(reads(WALKER, 7).hex() == "554889e5415653", "the 0x20 constructor walker prologue changed")
    require(reads(0x53, 7)[:3] == b"\x48\x8d\x1d", "the walker no longer `lea`s the table top")
    require(0x53 + 7 + struct.unpack_from("<i", reads(0x53, 7), 3)[0] == CTOR_TABLE_TOP,
            "the walker's table top is no longer 0x2dd70e0")
    require(reads(0x60, 4).hex() == "4883c3f8", "the walker no longer steps -8 (backwards)")
    require(reads(0x64, 3).hex() == "488b03", "the walker no longer loads the row")
    require(reads(0x6C, 4).hex() == "4883f8ff", "the walker lost its -1 sentinel compare")
    require(reads(0x72, 2).hex() == "ffd0", "the walker no longer `call *%rax`")
    require(struct.unpack_from("<Q", reads(CTOR_SENTINEL, 8))[0] == 0xFFFFFFFFFFFFFFFF,
            "the -1 sentinel below entry 572 moved: the table length is no longer proven")
    require(struct.unpack_from("<Q", reads(CTOR_LAST_ENTRY, 8))[0] == 0x1A20,
            "constructor entry 572 is no longer 0x1a20")
    require(check_call(MAIN_CALL_WALKER, WALKER), "main no longer calls the walker at 0xf14d")
    require(reads(MAIN_CALL_WALKER + 5, 8).hex() == "31d24489f74c89fe",
            "the straight-line gap between the walker and the bootstrap call changed")
    require(check_call(MAIN_CALL_BOOTSTRAP, BOOTSTRAP),
            "main no longer calls 0x536730 at 0xf15a")

    result = {
        "task": ["CPU-01-STARTUP-SLOT-309-CALLER-ORDER-01",
                 "CPU-01-STARTUP-SLOT-309-HELPER-CALLER-CENSUS-01",
                 "CPU-01-STARTUP-SLOT-309-NONZERO-CONSUMER-01",
                 "CPU-01-STARTUP-SLOT-309-PAYLOAD-OWNERSHIP-01"],
        "inputs": {"elf_sha256": ELF_SHA256},
        "slot309": {"rva": f"0x{SLOT309:x}", "index": SLOT309_INDEX,
                    "table_row": f"0x{SLOT309_TABLE_ROW:x}",
                    "near_calls": 0, "relocations": 1},
        "var_ctor_callers": {f"0x{c:x}": [f"0x{s:x}" for s, _k in calls[c]] for c in VAR_CTORS},
        "shared_body_callers": [f"0x{s:x}" for s, _k in calls[SHARED_BODY]],
        "envelope": {"payload_offset": "0x10", "tag_offset": "0x20",
                     "tag_tables": {f"0x{t:x}": [f"0x{h:x}" for h in b]
                                    for t, b in TAG_TABLES.items()},
                     "destroy": "0xa50270 (bare ret)",
                     "seed": [f"0x{s:x}" for s in VARSIG_SLOT309_SITES]},
        "callback_0x28af0": {"cell": f"0x{CALLBACK_CELL:x}", "reader": f"0x{CALLBACK_READ:x}",
                             "write": f"0x{CALLBACK_WRITE:x} -> [r15+0xd0]",
                             "bind_self": f"0x{CALLBACK_BIND_SELF:x} -> [r15+0xc8]"},
        "virtual_0x531d60": {"row": f"0x{VTABLE_ROW:x}",
                             "address_point": f"0x{VTABLE_ADDR_POINT:x}",
                             "vptr_installs": {f"0x{s:x}": k for s, k in VTABLE_INSTALLS.items()},
                             "dispatch": f"0x{DISPATCH:x} call *0x20"},
        "closure": {"main": f"0x{MAIN:x}", "walker": f"0x{WALKER:x}",
                    "walker_call": f"0x{MAIN_CALL_WALKER:x}",
                    "bootstrap_call": f"0x{MAIN_CALL_BOOTSTRAP:x}",
                    "bootstrap": f"0x{BOOTSTRAP:x}",
                    "workspace_ctor": f"0x{WORKSPACE_CTOR:x}",
                    "table_entries_in_chain": 0,
                    "sentinel": f"0x{CTOR_SENTINEL:x}", "last_entry": f"0x{CTOR_LAST_ENTRY:x}"},
        "fde_present": bool(fde),
    }
    if args.json is not None:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")
        print(f"wrote {args.json}")

    print(f"OK check_slot309_followups: slot 309 is reached only as table row "
          f"0x{SLOT309_TABLE_ROW:x}; 0xa50280 has 1 caller (0xa50c57) and 0xa50440 has 1 caller "
          f"(0xa503d7); the three tag tables are 4 copy-only helpers each with a bare-ret destroy; "
          f"the +0x10 payload is seeded by a literal zero at 0xa5068b/0xa50a3c/0xa50bc5; "
          f"0x28af0 has 1 cell (0x15fe540), 1 reader (0x27ab1) and 0 direct calls; "
          f"0x531d60 is row 0x{VTABLE_ROW:x} of the vtable at 0x{VTABLE_ADDR_POINT:x} installed only "
          f"by 0x536afa/0x537fa8, dispatched at 0x{DISPATCH:x}")
    print(f"   order: main 0x{MAIN:x} calls the walker 0x{WALKER:x} at 0x{MAIN_CALL_WALKER:x} and, "
          f"straight-line, the bootstrap 0x{BOOTSTRAP:x} at 0x{MAIN_CALL_BOOTSTRAP:x}; the walker "
          f"starts at the table top and steps -8 to the -1 sentinel at 0x{CTOR_SENTINEL:x}, so slot 309 "
          f"runs before everything downstream of the bootstrap")
    print("STATIC_ANALYSIS_VERIFIED only: no runtime execution, no native code and no game claim")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
